#CXXFLAGS:=-O2 -std=c++0x -g -Wall
CXXFLAGS:=-O2 -g -lpthread

SOURCES:=HttpClientRequestProxy.cpp HttpCommon.cpp HttpFilter.cpp HttpHeaders.cpp HttpRequest.cpp HttpResponse.cpp HttpStatistics.cpp
HEADERS:=HttpClientRequestProxy.h HttpCommon.h HttpFilter.h HttpHeaders.h HttpRequest.h HttpResponse.h HttpStatistics.h ParseException.h SocketListener.h

all::http_proxy

# http-get

http_proxy: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o http_proxy

#http-get: http-get.cpp $(SOURCES) $(HEADERS)
#	$(CXX) $(CXXFLAGS) http-get.cpp $(SOURCES) -o http-get


clean:
	rm http_proxy HttpClientRequestProxy.cpp
