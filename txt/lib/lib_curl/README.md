## lib_curl - covers curl_easy
<a href="#lib_curl"></a>

All uses of lib_curl are covered in unittest:lib_curl.% <br/>. unittest is based on the lib_curl::PrintCurlResp <br/>
An insight for debugging is that lib_curl::PrintCurlResp function produces functional curlrc text. <br/>
If it is saved in, say xyz, than curl --config xyz will execute it as if it is a full curl command line <br/>
To see unittest in action one can start bin/curl_server and execute commands like <br/>
$curl --config test/atf_unit/lib_curl.GET_Echo
