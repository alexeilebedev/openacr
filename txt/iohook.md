## Iohook & Fbuf

In a non-blocking single-threaded application, there can be only one situation where time is 
yielded to the system: when that time is not needed. That's how we avoid taking 100% CPU
while continuing to call ourselves `non-blocking`.

### Iohook

The `Iohook` mechanism is a way to attach (hook?) file descriptor polling to the main loop.

Let's begin talking about `Iohooks` by creating a sample program. For this chapter, the sample
program is `sample`:

    $ acr_ed -create -target sample -write
    ...

The concept is simple. We register interest in either reading or writing a file descriptor,
attach a callback, and insert the iohook into the system-specific event mechanism. On
BSD systems (FreeBSD and MacOS (Mach)) it's `kevent`, on Linux it's `epoll`.

The program will read characters from stdin and print them as c++-quoted characters
with timestamp.
First, we'll need a field for the Iohook:

    $ acr_ed -create -field sample.FDb.read -arg algo_lib.FIohook -write
    ...


    static void DoRead() {
        char buf;
        int rc=0;
        do {
            rc=read(sample::_db.read.fildes.value,&buf,1);
            if (rc==1) {
                tempstr out;
                char_PrintCppSingleQuote(buf,out);
                prlog(CurrUnTime()<<" "<<out);
            } else if (rc==-1 && errno==EAGAIN) {
            } else {
                IohookRemove(sample::_db.read);
            }
        } while (rc>0);
    }

    void sample::Main() {
        _db.read.fildes=algo::Fildes(0);
        algo::SetBlockingMode(_db.read.fildes,false);
        callback_Set0(_db.read,DoRead);
        IOEvtFlags flags;
        read_Set(flags,true);
        IohookAdd(_db.read,flags);
        MainLoop();
    }

Let's run the program:

    $ sample
    5
    2019-05-17T16:26:52.95788 '5'
    2019-05-17T16:26:52.95818 '\n'
    ^K
    2019-05-17T16:26:56.931429 '\013'
    2019-05-17T16:26:56.931444 '\n'
    ^D
    <exits>
    
Our primitive non-blocking program does what we intended.
This code could be combined with other non-blocking code,
and the behaviors the two will be combined.

See the chapter on [Scheduling](#scheduling-and-main-loop) for more information.
