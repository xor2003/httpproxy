/**
 * @file Header for common functionality
 *
 */

#ifndef _HTTP_COMMON_H_
#define _HTTP_COMMON_H_

#ifdef NDEBUG
#define TRACE(x)
#else
#include <iostream>
#define TRACE(x) std::clog << x << std::endl;
#endif // NDEBUG

#define ERROR(x) std::cerr << x << std::endl;

#include "ParseException.h"
#include <string>

const int BACKLOG = 100;
const int BUFSIZE = 4096; // size of the input buffer
const unsigned int MAXCACHESIZE = 10; // page cache size

//char* mymemcat(char *OUT, const char *IN, size_t N);

std::string& trim (std::string & s);

void *get_in_addr (struct sockaddr *sa);

int make_server_listener (const std::string & port);
int make_client_connection (const std::string & host, const std::string & port);
int get_data_from_host (int remote_fd, std::string &result);

#endif //__HTTP_COMMON_H__
