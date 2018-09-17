// (C) 2018-2019 NYSE | Intercontinental Exchange
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/iohook.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/algo.h"

// Register IOHOOK to be called whenever an IO operation is possible.
// OK to add an fd twice with different flags. Subsequent calls override previous ones.
// Add iohook to epoll in read, write or read/write mode
// Optionally, add as edge triggered
void algo_lib::IoHookAdd(algo_lib::FIohook& iohook, algo::IOEvtFlags inflags) NOTHROW {
    bool isnew = !iohook.in_epoll;
    iohook.evt_flags = inflags;
    struct epoll_event ev;
    ev.events                     = 0;
    ev.events |= EPOLLET;// edge-trigger only
    if (read_Get(inflags)) ev.events  |= EPOLLIN;
    if (write_Get(inflags)) ev.events |= EPOLLOUT;
    ev.data.ptr = (void*)&iohook;
    int req     = isnew ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    int rc      = epoll_ctl(algo_lib::_db.epoll_fd, req, iohook.fildes.value, &ev);
    int err     = errno;
    (void)rc;// coverity
    // file descriptors cannot be added to epoll, linux returns EPERM.
    // we support such use, and immediately mark the file descriptor as "ready" -- why not?
    if (err == EPERM) {
        callback_Call(iohook,iohook);// immediate callback
    }
    iohook.in_epoll = true;
    algo_lib::_db.n_iohook += isnew;
}

// -----------------------------------------------------------------------------

// Remove iohook from epoll
void algo_lib::IoHookRemove(algo_lib::FIohook& iohook) NOTHROW {
    if (iohook.in_epoll) {
        algo_lib::_db.n_iohook -= 1;
        iohook.in_epoll = false;
        // epoll_ctl:
        // passing NULL as last argument causes crash in DBL2.0
        // but is valid on Linux >2.6.9
        struct epoll_event ev;
        ev.events = 0;
        ev.data.ptr = (void*)&iohook;
        // iohook may not be in epoll at all, if
        // it is a file descriptor that was successfully ignored
        // in IoHookAdd.
        int rc = epoll_ctl(algo_lib::_db.epoll_fd,EPOLL_CTL_DEL,iohook.fildes.value,&ev);
        (void)rc;// coverity
    }
}
