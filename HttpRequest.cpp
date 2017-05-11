/**
 * @file Implementation for HttpRequest class
 *
 */

#include <sstream>              // String stream for int conversion
#include <vector>               // String stream for int conversion
#include <algorithm>
#include <functional>
#include <iterator>

#include <string.h>             // helpers to copy C-style strings
#include <stdlib.h>

#include "HttpRequest.h"
#include "HttpCommon.h"

#include "ParseException.h"

static void
tokenize_string (const std::string & original_string, const std::string & delimiters, std::vector < std::string > &tokens)
{
    tokens.clear ();
    if (original_string.empty ())
        return;

    size_t pos_start = original_string.find_first_not_of (delimiters);
    size_t pos_end = original_string.find_first_of (delimiters, pos_start);

    while (std::string::npos != pos_start /*&& std::string::npos != pos_end */ )
    {
        tokens.push_back (original_string.substr (pos_start, pos_end - pos_start));
        pos_start = original_string.find_first_not_of (delimiters, pos_end);
        pos_end = original_string.find_first_of (delimiters, pos_start);
    }
}

HttpRequest::HttpRequest ():m_method (UNSUPPORTED), m_port (0)
{
}

const char *
HttpRequest::ParseRequest (const char* buffer, size_t size)
{
    const char *curPos = buffer;

    const char *endline = (const char *) strstr (curPos, "\r\n");
    if (endline == NULL)
    {
        throw ParseException ("HTTP Request doesn't end with \\r\\n");
    }

    std::string requestLine (curPos, endline - curPos), host_, path_;
    TRACE ("requestLine: " << requestLine);

    unsigned short port_ = 80;
    std::vector < std::string > tokens;
    tokenize_string (requestLine, " ", tokens);

    // 1. Request type
    TRACE ("Tokens size: " << tokens.size ());
    if (tokens.empty ())
        throw ParseException ("Incorrectly formatted request");

    const std::string & method_s = tokens[0];
    TRACE ("Method: " << method_s);
    if (method_s != "GET")
    {
        throw ParseException ("Request is not GET");
    }
    SetMethod (GET);

    // 2. Request path
    if (tokens.size () < 2)
        throw ParseException ("Incorrectly formatted request");

    // TRACE ("Token2: " << *token);
    const std::string prot_end_marker ("://");
    const std::string & url_s = tokens[1];
    std::string::const_iterator prot_i = search (url_s.begin (), url_s.end (), prot_end_marker.begin (), prot_end_marker.end ());

    if (prot_i != url_s.end ())
    {
        advance (prot_i, prot_end_marker.length ());
    }
    else
    {
        prot_i = url_s.begin ();
    }

    const char match[] = { '/', ':' };
    std::string::const_iterator host_i = find_first_of (prot_i, url_s.end (), match, match + 2);
    host_.assign (prot_i, host_i);

    TRACE ("Host: " << host_);
    SetHost (host_);

    if (host_i != url_s.end () && *host_i == ':')
    {
        ++host_i;

        std::string::const_iterator port_i = find (host_i, url_s.end (), '/');
        std::string port_s (host_i, port_i);
//      TRACE ("port_s: " << port_s);
        port_ = atoi (port_s.c_str ());

        host_i = port_i;
    }
    TRACE ("Port: " << port_);
    SetPort (port_);

    if (host_i != url_s.end ())
    {
        path_.assign (host_i, url_s.end ());
    }

    TRACE ("Path: " << path_);
    SetPath (path_);

    // 3. Request version
    if (tokens.size () < 3)
        throw ParseException ("Incorrectly formatted request");
    // TRACE ("Token3: " << *token);
    const std::string ver_marker ("HTTP/");
    const std::string & ver_s = tokens[2];
    std::string::const_iterator posHTTP_i = search (ver_s.begin (), ver_s.end (), ver_marker.begin (), ver_marker.end ());

    if (posHTTP_i == ver_s.end ())
    {
        throw ParseException ("Incorrectly formatted HTTP request");
    }
    std::string version (posHTTP_i + 5, ver_s.end ());
    // TRACE (version);
    TRACE ("Version: " << version);
    SetVersion (version);

    curPos = endline + 2;
    return ParseHeaders (curPos, size - (curPos - buffer));
}


void
HttpRequest::FormatRequest (std::string& buffer) const
{
    if (m_method != GET)
        throw ParseException ("Only GET method is supported");

    TRACE ("m_path" << m_path);
    buffer = "GET " + m_path + " HTTP/" + m_version + "\r\n";
    HttpHeaders::FormatHeaders (buffer);
    buffer += "\r\n";

}


HttpRequest::MethodEnum HttpRequest::GetMethod () const
{
    return m_method;
}

void
HttpRequest::SetMethod (HttpRequest::MethodEnum method)
{
    m_method = method;
}

// const std::string &
// HttpRequest::GetProtocol () const
// {
//   return m_protocol;
// }

// void
// HttpRequest::SetProtocol (const std::string &protocol)
// {
//   m_protocol = protocol;
// }

std::string
HttpRequest::GetHost () const
{
    return m_host;
}

void
HttpRequest::SetHost (const std::string & host)
{
    m_host = host;

    if (m_port != 80)
    {
        std::stringstream ss;
        ss << m_port;
        ModifyHeader ("Host", m_host + ":" + ss.str ());
    }
    else
        ModifyHeader ("Host", m_host);
}

unsigned short
HttpRequest::GetPort () const
{
    return m_port;
}

void
HttpRequest::SetPort (unsigned short port)
{
    m_port = port;

    if (m_port != 80)
    {
        std::stringstream ss;
        ss << m_port;
        ModifyHeader ("Host", m_host + ":" + ss.str ());
    }
    else
        ModifyHeader ("Host", m_host);
}

std::string
HttpRequest::GetPath () const
{
    return m_path;
}

void
HttpRequest::SetPath (const std::string & path)
{
    m_path = path;
}

std::string
HttpRequest::GetVersion () const
{
    return m_version;
}

void
HttpRequest::SetVersion (const std::string & version)
{
    m_version = version;
}
