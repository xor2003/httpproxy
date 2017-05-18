/**
 * @file Implementation for HttpProxy main file
 *
 */

// C++ Libraries
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <utility>

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

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "HttpRequest.h"
#include "HttpCommon.h"
#include "HttpFilter.h"
#include "HttpStatistics.h"
#include "HttpClientRequestProxy.h"
#include "ParseException.h"



HttpClientRequestProxy::HttpClientRequestProxy(int       client_fd_i,
        cache_t *    cache_i,
        HttpFilter* bfilter_i,
        HttpStatistics* hstatistics_i,
        pthread_mutex_t *    mutex_i):
    m_client_fd(client_fd_i),
    m_cache(cache_i),
    m_bfilter(bfilter_i),
    m_hstatistics(hstatistics_i),
    m_mutex(mutex_i)
{
    /*
    // Make threads to deal with logic
    pthread_t thread_id;              // We're going to detach, IDGAF this variable can die
    pthread_create (&thread_id, NULL,
                    HttpClientRequestProxy::executer,
                    static_cast<void *>(this) );
    pthread_detach (thread_id);
    */
}

HttpClientRequestProxy::~HttpClientRequestProxy()
{
    // Close and free
    close (m_client_fd);
}

/**
 * @brief Calls client_connected()
 */
void
HttpClientRequestProxy::executer ()
{
    // Cast client_fd into an int*, dereference it, call the right function
    //static_cast<HttpClientRequestProxy *>(thread_p)->run ();
    run();

    delete static_cast<HttpClientRequestProxy *>(this);

    return;// NULL;
}

/**
 * @brief Receives message fromt client and sends it to remote, back to client
 * @returns 0 if success, -1 if error
 */
int
HttpClientRequestProxy::run()
{
    // Set up buffer
    std::string    client_buffer;

    // Loop until we get "\r\n\r\n"
    while (strstr (client_buffer.c_str (), "\r\n\r\n") == NULL)
    {
        char  buf[BUFSIZE];
        if (recv (m_client_fd, buf, sizeof (buf), 0) < 0)
        {
            perror ("recv");
            return -1;
        }
        client_buffer.append (buf);
    }

//  TRACE ("Buffer ~" << client_buffer << "~");
    // Parse the request, prepare response
    HttpRequest    client_req;
    bool parse_exception = false;
    try
    {
        client_req.ParseRequest (client_buffer.c_str(), client_buffer.size());
    }
    catch (ParseException & ex)
    {
        ERROR( "Exception raised: " << ex.what());
        parse_exception = true;
        // When in doubt, assume HTTP/1.0
        std::string  client_res = "HTTP/1.0 ";

        // Our server only has two bad responses, at least up to here
        std::string cmp = "Request is not GET";
        if (cmp == ex.what())
            client_res += "400 Bad Request\r\n\r\n";
        else
            client_res += "501 Not Implemented\r\n\r\n";

        // Send the bad stuff!
        if (send (m_client_fd, client_res.c_str (), client_res.length (), 0) == -1)
            perror ("send");
        return 0;
    }

    {
        // If host not specified in first line, find it in the AddHeaders
        std::string remote_host;
        if (client_req.GetHost ().length () == 0)
            remote_host = client_req.FindHeader ("Host");
        else
            remote_host = client_req.GetHost ();

        if (m_bfilter->check(remote_host))
        {

            // When in doubt, assume HTTP/1.0
            std::string  client_res = "HTTP/1.0 403 Forbidden\r\n\r\n";

            // Send the bad stuff!
            if (send (m_client_fd, client_res.c_str (), client_res.length (), 0) == -1)
                perror ("send");
            TRACE(std::endl<<"Access to " << remote_host << " blocked"<<std::endl);
            return 0;
        }

        // We're not doing persistent stuff
        client_req.ModifyHeader ("Connection", "close");

        // Make all plaintext
        client_req.RemoveHeader ("Accept-Encoding");

        // Format the new request
        std::string remote_req;

        // Prereserve buffer
        remote_req.reserve(BUFSIZE);
        client_req.FormatRequest (remote_req);

        std::stringstream  ss;;
        ss << client_req.GetPort ();
        std::string remote_port (ss.str ());

        // String of full path, for searching/adding to cache
        std::string
        full_path = remote_host + client_req.GetPath ();

        // Response "from remote host" to send back to client
        std::string  remote_res;
        remote_res.reserve(BUFSIZE*10);

        // Check cache for response
        cache_t::iterator it = m_cache->find (full_path);

        // In cache
        if (it != m_cache->end ())
        {
            //fprintf(stderr, "in cache\n");
            remote_res = it->second;
        }

        // Not in cache, make a req for it
        else
        {
            // Make connection to remote host
            int
            remote_fd = make_client_connection (remote_host, remote_port);
            if (remote_fd < 0)
            {
                ERROR( "Couldn't make a connection to remote host "<<remote_host<<" on port "<< remote_port );
                return -1;
            }

            //fprintf(stderr, "server: sending request\n");
            TRACE ("server: sending request:" << remote_req);
            // Send new request to remote host
            if (send (remote_fd, remote_req.c_str(), remote_req.size(), 0) == -1)
            {
                perror ("send");
                close (remote_fd);
                return -1;
            }

            //fprintf(stderr, "not in cache\n");
            TRACE ("get_data_from_host");

            // Store data from remote host into response string
            if (get_data_from_host (remote_fd, remote_res) != 0)
            {
                close (remote_fd);
                return -1;
            }
            //   TRACE ("server: getting responce:" << remote_res );

            // Add to the cache if Cache-Control is not private
            // This function must be atomic, as the cache is a shared object
            if (strstr (remote_res.c_str (), "Cache-Control: private") == NULL)
            {
                pthread_mutex_lock (m_mutex);

                if (m_cache->size()==MAXCACHESIZE) m_cache->erase(m_cache->begin());

                m_cache->insert (std::pair < std::string, std::string > (full_path, remote_res));
                pthread_mutex_unlock (m_mutex);
            }

            close (remote_fd);
        }

        // By now remote_res has entire response, ship that back wholesale back to the client
        if (send (m_client_fd, remote_res.c_str (), remote_res.length (), 0) == -1)
        {
            perror ("send");
            return -1;
        }
        m_hstatistics->add(remote_host,remote_res.length());
    }

    return 0;
}

int
main (int argc, char *argv[])
{
    TRACE ("HTTP Proxy started")
    // Create a server
    std::string listen_port("8080");
    if (argc>1) listen_port=argv[1];
    int sock_fd = make_server_listener (listen_port);
    if (sock_fd < 0)
    {
        ERROR( "Couldn't make proxy server listener on port " << listen_port);
        return 1;
    }

    TRACE ("Proxy listening on: " << listen_port << " TCP port");
    // Initialize cache
    cache_t cache;
    HttpFilter filter; // Load blacklist filter
    HttpStatistics hstatistics; // Initialize statistics

    pthread_mutex_t  cache_mutex;
    pthread_mutex_init (&cache_mutex, NULL);

    /*
     * Create an asio::io_service and a thread_group (through pool in essence)
     */
    boost::asio::io_service ioService;
    boost::thread_group threadpool;


    /*
     * This will start the ioService processing loop. All tasks
     * assigned with ioService.post() will start executing.
     */
    boost::asio::io_service::work work(ioService);

    /*
     * This will add threads to the thread pool. (You could just put it in a for loop)
     */
    for(size_t i=0; i<20; i++)
    {


        threadpool.create_thread(
            boost::bind(&boost::asio::io_service::run, &ioService)
        );
    }


    //printf("server: waiting for connections...\n");
    int request_counter=0;
    // Main accept loop
    for (;;)
    {
        struct sockaddr_storage
            their_addr;             // connector's address information
        char
        s[INET6_ADDRSTRLEN];
        socklen_t
        sin_size = sizeof their_addr;

        // Accept connections
        int
        client_fd = accept (sock_fd, (struct sockaddr *) &their_addr,
#ifdef _HPUX_SOURCE
                            (int*)
#endif
                            &sin_size);
        if (client_fd == -1)
        {
            perror ("accept");
            continue;
        }

        // Print out IP address
        inet_ntop (their_addr.ss_family, get_in_addr ((struct sockaddr *) &their_addr), s, sizeof s);

        // Create and run working thread
        HttpClientRequestProxy* request = new HttpClientRequestProxy(client_fd, &cache, &filter, &hstatistics, &cache_mutex);

        /*
         * This will assign tasks to the thread pool.
         * More about boost::bind: "http://www.boost.org/doc/libs/1_54_0/libs/bind/bind.html#with_functions"
         */
        ioService.post(boost::bind(&HttpClientRequestProxy::executer, request));

        // Display site statistics each 5 request
        ++request_counter;
        if ((request_counter % 5)==0) hstatistics.display();
    }

    /*
     * This will stop the ioService processing loop. Any tasks
     * you add behind this point will not execute.
    */
    ioService.stop();

    /*
     * Will wait till all the threads in the thread pool are finished with
     * their assigned tasks and 'join' them. Just assume the threads inside
     * the threadpool will be destroyed by this method.
     */
    threadpool.join_all();

    return 0;
}
