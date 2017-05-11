# httpproxy
Code Description
================

A simple implementation of an HTTP proxy server and client in C++ using sockets and pthreads. **Basic** server-side caching is also implemented.

Additional features:
+Porting to C++, using hash maps
+Blocking using site blacklist.txt file
+Site statistics

+Platforms: HP-UX (Itanium), Linux, Cygwin

http-proxy
----------

The main routine creates a server. The cache is initialized in the main routine so it is shared across
different threads. Responses stored in the cache do not expire until http-proxy 
exits. 
The cache is a C++ Standard Library map or unordered_map, which maps absolute URIs (string) to full
HTTP responses (string). Each thread must acquire a shared lock before writing
to the cache to prevent race conditions. The server, in the main routine, then
goes into an infinite loop, waiting to accept connections. Each connection
spawns a new thread using POSIX threads and processes the request. If a remote
request is made, the response from the remote host is sent to the client whole.
Some sites can be blocked by specifying in blacklist.txt
Site statistics is displayed in console.
Only GET method supported currently.
Proxy can switch conections to plain text to dump information.

Build and Run
=============

Execute GNU make (from cygwin) or use Code::Blocks IDE.

run ./http-proxy [proxy port]

Connect using your web broser 
      using HTTP proxy settigs 
                      localhost:8080
