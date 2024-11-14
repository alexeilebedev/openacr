//
// License: GPL
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
// Target: sample (exe)
// Exceptions: yes
// Source: cpp/sample/sample.cpp
//

#include "include/algo.h"
#include "include/algo.h"
#include "include/sample.h"

void sample::Main() {
    for (int i=0; i<10; i++) {
        sample::FRec &rec = rec_Alloc();
        rec.rec = 10-i;
        rec_XrefMaybe(rec);
    }
    if (FRec *rec = ind_rec_Find(3)) {
        zd_sel_rec_Insert(*zd_rec_Next(*rec));
        zd_sel_rec_Insert(*rec);
        zd_sel_rec_Insert(*zd_rec_Prev(*rec));
        prlog("next element after 3 is "<<zd_rec_Next(*rec)->rec);
        //rec_Delete(*rec);
    }
    ind_beg(_db_zd_rec_curs,rec,_db) {
        prlog(rec.rec);
    }ind_end;

    prlog("I am here");

    ind_beg(_db_bh_rec_curs,rec,_db) {
        prlog(rec.rec);
    }ind_end;

    ind_beg(_db_zd_sel_rec_curs,rec,_db) {
        prlog("selected: "<<rec.rec);
    }ind_end;

    sample::MainLoop();
}
