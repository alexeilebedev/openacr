//
// Target: lib_ams (lib) -- Library for AMS middleware, supporting file format & messaging
// Exceptions: yes
// Source: cpp/lib/ams/file.cpp
//

#include "include/algo.h"
#include "include/lib_ams.h"

#include <lz4.h>

///////////////////////////////////////////////////////////////////////////////
//
// WRITE
//
///////////////////////////////////////////////////////////////////////////////

// Whether write file is in good state - valid fd and no failure
bool lib_ams::OkQ(lib_ams::FWritefile &wf) {
    return ValidQ(wf.fd) && !wf.fail;
}

// Check for failure. This is intended for checking Close errors.
// After Close, OkQ could not be used for this purpose,
// but failure flag is preserved, and could be retrieved with FailQ.
bool lib_ams::FailQ(lib_ams::FWritefile &wf) {
    return wf.fail;
}

// Open file for appending messages
void lib_ams::OpenAppend(lib_ams::FWritefile &wf, algo::strptr filename) {
    // silently close fd, no error raised,
    // use explicit Close call to close and catch error
    Close(wf);

    // clear buffers from previous run
    buf_RemoveAll(wf);
    cbuf_RemoveAll(wf);

    // reset message block from previous run
    wf.block = ams::MsgBlock();

    // clear error flags from previous run
    wf.fail = false;

    // open file
    wf.filename = filename;
    wf.fd = OpenFile(wf.filename,algo_FileFlags_append);

    // check message blocks for integrity and get last block
    if (OkQ(wf)) {
        lib_ams::FReadfile rf;
        OpenRead(rf,filename);
        ScanToEnd(rf);
        wf.block = rf.block; // copy last block header
        wf.fail |= !OkQ(rf); // means broken file
        Close(rf);
        StartNextMsgBlock(wf);
    }
    zd_flush_Insert(wf);
}

// Flush data to file
void lib_ams::zd_flush_Step() {
    lib_ams::FWritefile &wf = *zd_flush_RemoveFirst();
    Flush(wf);
    if (OkQ(wf)) {
        zd_flush_Insert(wf);
    }
}

// Write message (buffered)
void lib_ams::WriteMsg(lib_ams::FWritefile &wf, ams::MsgHeader &msg) {
    if (OkQ(wf)) {
        // append to buffer
        algo::memptr dst = buf_AllocN(wf,msg.length);
        memcpy(dst.elems,&msg,msg.length);
        ++wf.block.n_messages;

        if (wf.buf_n >= wf.buf_thr) {
            zd_flush_Remove(wf);
            Flush(wf);
            if (OkQ(wf)) {
                // insert back at tail
                zd_flush_Insert(wf);
                if (zd_flush_First() == &wf) {
                    // this wr is on head, adjust next time
                    _db.zd_flush_next = algo_lib::_db.clock + lib_ams::_db.zd_flush_delay;
                }
            }
        }
    }
}

// Write buffer as message block
void lib_ams::Flush(lib_ams::FWritefile &wf) {
    if (OkQ(wf) && buf_N(wf)) {
        Compress(wf);
        WriteMsgBlock(wf);
        StartNextMsgBlock(wf);
    }
}

// Compress buffer - form full message in cbuf
void lib_ams::Compress(lib_ams::FWritefile &wf) {
    // Wost case when compression is not possible,
    // but some overhead is still required.
    cbuf_Reserve(wf,sizeof(ams::MsgBlock)+LZ4_compressBound(buf_N(wf)));
    ams::MsgBlock &block = *new (wf.cbuf_elems) ams::MsgBlock(wf.block);
    // memorize original (uncompressed) length
    block.original_length = buf_N(wf);
    i32 n_written = LZ4_compress_default((char *)wf.buf_elems, // source addr
                                         (char *)messages_Addr(block), // destination addr
                                         buf_N(wf), // source length
                                         cbuf_Max(wf)-sizeof(ams::MsgBlock)); // size of destination buffer
    // adjust total length
    wf.cbuf_n = block.length = sizeof(ams::MsgBlock) + n_written;
    // Compression returns zero in case of error,
    // and number of written bytes in cases of success.
    // Error is actually not possible, as compression is guaranteed to succeed
    // if destination buffer size >= LZ4_compressBound(sourceSize).
    // Just in case for calculation mistakes.
    wf.fail |= !n_written;
}

// Write message block to file
void lib_ams::WriteMsgBlock(lib_ams::FWritefile &wf) {
    if (OkQ(wf)) {
        wf.fail |= !WriteFile(wf.fd,wf.cbuf_elems,cbuf_N(wf));
    }
}

// starts new message block - discard buffer, adjust seqno
void lib_ams::StartNextMsgBlock(lib_ams::FWritefile &wf) {
    if (OkQ(wf)) {
        buf_RemoveAll(wf);
        cbuf_RemoveAll(wf);
        wf.block.first_seqno += wf.block.n_messages;
        wf.block.n_messages = 0;
        wf.block.original_length = 0;
    }
}

// Close written file
void lib_ams::Close(lib_ams::FWritefile &wf) {
    Flush(wf);
    zd_flush_Remove(wf);
    if (ValidQ(wf.fd)) {
        wf.fail |= close(wf.fd.value)!=0;
    }
    wf.fd = algo::Fildes();
}

// cleanup function
void lib_ams::fd_Cleanup(lib_ams::FWritefile& parent) {
    Close(parent);
}

///////////////////////////////////////////////////////////////////////////////
//
// READ
//
///////////////////////////////////////////////////////////////////////////////

// Whether read file is in good state - valid fd and no failure.
// EOF is not a failure.
bool lib_ams::OkQ(lib_ams::FReadfile &rf) {
    return ValidQ(rf.fd) && !rf.fail;
}

// Check for failure after close.
// EOF is not a failure.
bool lib_ams::FailQ(lib_ams::FReadfile &rf) {
    return rf.fail;
}

// Check for EOF
bool lib_ams::EofQ(lib_ams::FReadfile &rf) {
    return rf.eof;
}

// Whether can read from file - file is good and no EOF
bool lib_ams::CanReadQ(lib_ams::FReadfile &rf) {
    return OkQ(rf) && !EofQ(rf);
}

// Open file for reading messages
void lib_ams::OpenRead(lib_ams::FReadfile &rf, algo::strptr filename) {
    Close(rf);
    rf.eof = false;
    rf.fail = false;
    rf.block = ams::MsgBlock();
    cbuf_RemoveAll(rf);
    buf_RemoveAll(rf);
    offset_RemoveAll(rf);
    rf.filename = filename;
    rf.fd = OpenFile(filename,algo_FileFlags_read);
}

// Read message by sequence number
ams::MsgHeader *lib_ams::ReadMsg(lib_ams::FReadfile &rf, u64 seqno) {
    ams::MsgHeader  *msg = NULL;
    ScanToSeqno(rf,seqno);
    if (OkQ(rf) && rf.block.first_seqno <= seqno && seqno < rf.block.first_seqno + rf.block.n_messages) {
        u32 *offset = offset_Find(rf,seqno - rf.block.first_seqno);
        if (offset) {
            //prlog(*offset);
            msg = (ams::MsgHeader*)(rf.buf_elems + *offset);
            //prlog("msg type " << msg->type << "  length:" << msg->length);
        }
    }
    return msg;
}

// Scan file up to end
void lib_ams::ScanToEnd(lib_ams::FReadfile &rf) {
    Scan(rf,NULL);
}

// Scan up to seqno, rewind if needed
void lib_ams::ScanToSeqno(lib_ams::FReadfile &rf, u64 seqno) {
    Scan(rf,&seqno);
}

// Scan and block load engine.
// if seqno is NULL, scans to end, fills block with last block.
// If seqno is present, scans up to given seqno, reads cbuf with payload
// Note is does only forward scan.
void lib_ams::Scan(lib_ams::FReadfile &rf,const u64 *seqno) {
    if (OkQ(rf) && seqno && *seqno < rf.block.first_seqno) {
        rf.eof = false;
        rf.block = ams::MsgBlock();
        rf.fail |= bool(lseek(rf.fd.value,0,SEEK_SET));
    }
    ams::MsgHeader hdr;
    while ((!seqno || rf.block.first_seqno + rf.block.n_messages <= *seqno) && Read(rf,&hdr,sizeof hdr)) {
        if (MsgBlock_Castdown(hdr)) {
            rf.fail |= hdr.length < sizeof rf.block;
            u32 plen = hdr.length - sizeof rf.block;
            if (Read(rf,(u8*)&rf.block + sizeof hdr,sizeof rf.block - sizeof hdr)) {
                if (seqno && rf.block.first_seqno + rf.block.n_messages > *seqno) {
                    cbuf_Reserve(rf,plen); // assume empty, AbsReserve calls realloc
                    rf.cbuf_n = plen;
                    Read(rf,rf.cbuf_elems,plen);
                    Uncompress(rf);
                    Reindex(rf);
                } else {
                    Skip(rf,hdr.length - sizeof rf.block);
                }
            }
        } else {
            rf.fail |= hdr.length < sizeof hdr;
            Skip(rf,hdr.length - sizeof hdr);
        }
    }
}

// Skip bytes
void lib_ams::Skip(lib_ams::FReadfile &rf,u32 skip) {
    if (CanReadQ(rf)) {
        rf.fail |= lseek(rf.fd.value,skip,SEEK_CUR)<0;
    }
}

// Read bytes
bool lib_ams::Read(lib_ams::FReadfile &rf,void *buf,u32 n) {
    bool ret = false;
    if (CanReadQ(rf)) {
        int n_read = read(rf.fd.value,buf,n);
        if (!n_read) {
            rf.eof = true;
        } else if (n_read < 0) {
            rf.fail = true; // TBD EAGAIN
        } else if (u32(n_read) == n) {
            ret = true;
        } else {
            rf.fail = true; // TBD repeat
        }
    }
    return ret;
}

// Uncompress
void lib_ams::Uncompress(lib_ams::FReadfile &rf) {
    if (OkQ(rf)) {
        // There is no method to compute decompressed size,
        // and distinguish buffer overrun from other error.
        // Application shall a priori know decompressed size limit.
        // We take it from message block header, where it was put to
        // on the compression.
        buf_Reserve(rf,rf.block.original_length);
        i32 rc = LZ4_decompress_safe((char*)rf.cbuf_elems,(char*)rf.buf_elems, cbuf_N(rf), buf_Max(rf));
        rf.fail |= rc<0 || u32(rc) != rf.block.original_length;
        rf.buf_n = rf.fail ? 0 : rc;
    }
}

// Reindex
void lib_ams::Reindex(lib_ams::FReadfile &rf) {
    if (OkQ(rf)) {
        offset_RemoveAll(rf);
        offset_Reserve(rf,rf.block.n_messages);
        ind_beg(ams::MsgHeader_curs,msg,buf_Getary(rf)) {
            offset_Alloc(rf) =  i32((u8*)msg - rf.buf_elems); // store offset
        }ind_end;
    }
}

// Close read file
void lib_ams::Close(lib_ams::FReadfile &wf) {
    if (ValidQ(wf.fd)) {
        close(wf.fd.value);
    }
    wf.fd = algo::Fildes();
}

// cleanup function
void lib_ams::fd_Cleanup(lib_ams::FReadfile& parent) {
    Close(parent);
}
