/**
 * @file Implementation for HttpResponse class
 *
 */

//#include "memmem.h"

#include <string.h>             // helpers to copy C-style strings

#include <algorithm>

#include "HttpResponse.h"
#include "HttpCommon.h"

HttpResponse::HttpResponse ()
{
}

const char *
HttpResponse::ParseResponse (const char *buffer, size_t size)
{
    const char *curPos = buffer;

    const char *endline = (const char *) strstr (curPos, "\r\n");
    if (endline == NULL)
    {
        throw ParseException ("HTTP Request doesn't end with \\r\\n");
    }

    std::string requestLine (curPos, endline - curPos);
    size_t posFirstSpace = requestLine.find (" ");
    size_t posSecondSpace = requestLine.find (" ", posFirstSpace + 1);

    if (posFirstSpace == std::string::npos || posSecondSpace == std::string::npos)
    {
        throw ParseException ("Incorrectly formatted response");
    }

    // 1. HTTP version
    if (requestLine.compare (0, 5, "HTTP/") != 0)
    {
        throw ParseException ("Incorrectly formatted HTTP request");
    }
    std::string version = requestLine.substr (5, posFirstSpace - 5);
    // TRACE (version);
    SetVersion (version);

    // 2. Response code
    std::string code = requestLine.substr (posFirstSpace + 1, posSecondSpace - posFirstSpace - 1);
    // TRACE (code);
    SetStatusCode (code);

    std::string msg = requestLine.substr (posSecondSpace + 1, requestLine.size () - posSecondSpace - 1);
    // TRACE (msg);
    SetStatusMsg (msg);

    curPos = endline + 2;
    return ParseHeaders (curPos, size - (curPos - buffer));
}


/*
char *
HttpResponse::FormatResponse (char *buffer) const
{
  char *bufLastPos = buffer;

  bufLastPos = mymemcat (bufLastPos, "HTTP/", 5);
  bufLastPos = mymemcat (bufLastPos, m_version.c_str (), m_version.size ());
  bufLastPos = mymemcat (bufLastPos, " ", 1);
  bufLastPos = mymemcat (bufLastPos, m_statusCode.c_str (), m_statusCode.size ());
  bufLastPos = mymemcat (bufLastPos, " ", 1);
  bufLastPos = mymemcat (bufLastPos, m_statusMsg.c_str (), m_statusMsg.size ());
  bufLastPos = mymemcat (bufLastPos, "\r\n", 2);

  bufLastPos = HttpHeaders::FormatHeaders (bufLastPos);
  bufLastPos = mymemcat (bufLastPos, "\r\n", 2);

  return bufLastPos;
}
*/

std::string
HttpResponse::GetVersion () const
{
    return m_version;
}

void
HttpResponse::SetVersion (const std::string & version)
{
    m_version = version;
}

std::string
HttpResponse::GetStatusCode () const
{
    return m_statusCode;
}

void
HttpResponse::SetStatusCode (const std::string & code)
{
    m_statusCode = code;
}

std::string
HttpResponse::GetStatusMsg () const
{
    return m_statusMsg;
}

void
HttpResponse::SetStatusMsg (const std::string & msg)
{
    m_statusMsg = msg;
}
