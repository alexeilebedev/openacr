## Tracing

For more information, see `[[prlog.md]]`

Tracing of various components is done with `prlog_cat` and `-trace` command line option
The trace expression is a regx, so to enable categories `stdout` and `xyz`, you would use
'-trace:(stdout|xyz)'

### AMS Stream Tracing

When the traace expression is applied using the function `lib_ams::ApplyTrace`,
both log categories and AMS streams are considered.

The trace expression is also applied to each each stream as follows:
The following string is matched against the regex:
- proc_id/stream_id       enable all tracing in ascii
- proc_id/stream_id,r     enable tracing of read messages in ascii
- proc_id/stream_id,w     enable tracing of written messages in ascii
- proc_id/stream_id,rb    enable tracing of read messages in binary
- proc_id/stream_id,wb    enable tracing of written messages in binary

Messages are considered "written" to a stream when the process calls `lib_ams::WriteMsg`.
This is when tracing happens.

Notably, `WriteMsg` is called when a messages is read from stdin and posted to a stream.

Message is considered read from a stream when it is skipped using `lib_ams::SkipMsg`
This happens after all of the outputs produced while handling the message.
Examples of ams stream trace expressions:

- %/%.out-0                        read+write, output from all processes in the tree
- %/%.md-0,w                       all messages written to md-0 stream
- (bbox-0/bbox-0.out-0,r|...)      explicit list

### Performance Monitoring Tracing

TBD
