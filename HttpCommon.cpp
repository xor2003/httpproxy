/**
 * @file Implementation for common elements
 *
 */

// C++ Libraries
#include <iostream>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cctype>
#include <locale>

// C Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

// C Network/Socket Libraries
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <string.h>

#include "HttpCommon.h"

/**
 * @brief trim from both ends
 */
std::string& trim (std::string & s)
{
    s.erase (s.begin (), std::find_if (s.begin (), s.end (), std::not1 (std::ptr_fun < int, int >(std::isspace))));
    s.erase (std::find_if (s.rbegin (), s.rend (), std::not1 (std::ptr_fun < int, int >(std::isspace))).base (), s.end ());
    return (s);
}

// get sockaddr, IPv4 or IPv6:
void *
get_in_addr (struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

/**
 * @brief Creates socket, binds it to port, & listens
 * @returns fd of socket, <0 if error
 */
int
make_server_listener (const std::string& port)
{
    // Create address structs
    struct addrinfo hints, *res;
    int sock_fd;

    // Load up address structs
    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4  AF_UNSPEC;  // IPv4&6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int addr_status = getaddrinfo (NULL, port.c_str(), &hints, &res);
    if (addr_status != 0)
    {
        ERROR( "Cannot get info");
        return -1;
    }

    // Loop through results, connect to first one we can
    struct addrinfo *p;
    for (p = res; p != NULL; p = p->ai_next)
    {
        // Create the socket
        sock_fd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock_fd < 0)
        {
            ERROR("server: cannot open socket");
            continue;
        }

        // Set socket options
        int yes = 1;
        int opt_status = setsockopt (sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int));
        if (opt_status == -1)
        {
            perror ("server: setsockopt");
            exit (1);
        }

        // Bind the socket to the port
        int bind_status = bind (sock_fd, res->ai_addr, res->ai_addrlen);
        if (bind_status != 0)
        {
            close (sock_fd);
            perror ("server: Cannot bind socket");
            continue;
        }

        // Bind the first one we can
        break;
    }

    // No binds happened
    if (p == NULL)
    {
        ERROR( "server: failed to bind");
        return -2;
    }

    // Don't need the structure with address info any more
    freeaddrinfo (res);

    // Start listening
    if (listen (sock_fd, BACKLOG) == -1)
    {
        perror ("listen");
        exit (1);
    }

    return sock_fd;
}

/**
 * @brief Creates socket, connects to remote host
 * @returns fd of socket, <0 if error
 */
int
make_client_connection (const std::string & host, const std::string & port)
{
    // Create address structs
    struct addrinfo hints, *res;
    int sock_fd;

    // Load up address structs
    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //IPv4  AF_UNSPEC; // IPv46
    hints.ai_socktype = SOCK_STREAM;

    TRACE ("make_client_connection HOST: " << host << " PORT: " << port);

    int addr_status = getaddrinfo (host.c_str(), port.c_str(), &hints, &res);
    if (addr_status != 0)
    {
        ERROR( "Cannot get info");
        return -1;
    }

    // Loop through results, connect to first one we can
    struct addrinfo *p;
    for (p = res; p != NULL; p = p->ai_next)
    {
        // Create the socket
        sock_fd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock_fd < 0)
        {
            perror ("client: cannot open socket");
            continue;
        }

        // Make connection
        int connect_status = connect (sock_fd, p->ai_addr, p->ai_addrlen);
        if (connect_status < 0)
        {
            close (sock_fd);
            perror ("client: connect");
            continue;
        }

        // Bind the first one we can
        break;
    }

    // No binds happened
    if (p == NULL)
    {
        ERROR( "client: failed to connect");
        // Don't need the structure with address info any more
        freeaddrinfo (res);
        return -2;
    }

    // Print out IP address
    //char s[INET6_ADDRSTRLEN];
    //inet_ntop (p->ai_family, get_in_addr ((struct sockaddr *) p->ai_addr), s, sizeof s);
    //fprintf(stderr, "client: connecting to %s\n", s);

    // Don't need the structure with address info any more
    freeaddrinfo (res);

    TRACE ("Connected");
    return sock_fd;
}

/**
 * @brief Gets all data from remote host
 * @returns 0 on success, <0 on failure
 * @param result The string that the data is appended to
 */
int
get_data_from_host (int remote_fd, std::string & result)
{
    // Loop until we get the last segment? packet?
    for (;;)
    {
        char res_buf[BUFSIZE];

        // Get data from remote
        int num_recv = recv (remote_fd, res_buf, sizeof (res_buf), 0);
        if (num_recv < 0)
        {
            perror ("recv");
            return -1;
        }

        // If we didn't recieve anything, we hit the end
        else if (num_recv == 0)
            break;

        // Append the buffer to the response if we got something
        result.append (res_buf, num_recv);
    }
    return 0;
}
