/**
 * @file Implementation for HttpRequest class
 *
 */

//#include "memmem.h"

#include <string.h>             // helpers to copy C-style strings

#include <algorithm>
#include <iterator>
#include <functional>
#include <cctype>
#include <locale>

#include "HttpCommon.h"
#include "ParseException.h"
#include "HttpHeaders.h"

HttpHeaders::HttpHeaders ()
{
}

const char *
HttpHeaders::ParseHeaders (const char *buffer, size_t size)
{
    const char *curPos = buffer;

    std::string key;
    std::string value;

    while (((size_t) (curPos - buffer) <= size - 2) && (*curPos != '\r' && *(curPos + 1) != '\n'))
    {
        const char *endline = (const char *) strstr (curPos, "\r\n");
        if (endline == NULL)
        {
            throw ParseException ("Header line does end with \\r\\n");
        }

        if (*curPos == ' ' || *curPos == '\t')    // multi-line header
        {
            if (key == "")
                throw ParseException ("Multi-line header without actual header");


            std::string newline (curPos, endline - curPos);
            // TRACE ("Multi-line header: " << value << " + " << newline);

            // reusing key from previous iteration
            value += "\r\n" + newline;
            ModifyHeader (key, value);
        }
        else
        {
            const char *header_key = (const char *) strstr (curPos, ":");

            if (header_key == NULL)
            {
                throw ParseException ("HTTP header doesn't contain ':'");
            }

            key.assign(curPos, header_key - curPos);
            value = std::string (header_key + 1, endline - header_key - 1);
            trim (value);         // remove any leading spaces if present

            // TRACE ("Key: [" << key << "], value: [" << value << "]");

            ModifyHeader (key, value);
        }

        curPos = endline + 2;
    }

    // TRACE ("Left: " << (int)(curPos-buffer) << ", size: " << size);
    if (static_cast < size_t > (curPos - buffer + 2) <= size)
    {
        curPos += 2;              // skip '\r\n'
    }
    else
    {
        throw ParseException ("Parsed buffer does not contain \\r\\n");
    }
    return curPos;
}

void
HttpHeaders::FormatHeaders (std::string& buffer) const
{
    for (HeadersList::const_iterator header = m_headers.begin (); header != m_headers.end (); ++header)
    {
        buffer += header->m_key + ": " + header->m_value + "\r\n";
    }

}


void
HttpHeaders::AddHeader (const std::string & key, const std::string & value)
{
    //HttpHeaders::HttpHeader temp(key, value);
    m_headers.push_back (HttpHeader(key, value));
}

void
HttpHeaders::RemoveHeader (const std::string & key)
{
    std::list < HttpHeader >::iterator item = std::find (m_headers.begin (), m_headers.end (), key);
    if (item != m_headers.end ())
        m_headers.erase (item);
}

void
HttpHeaders::ModifyHeader (const std::string & key, const std::string & value)
{
    std::list < HttpHeader >::iterator item = std::find (m_headers.begin (), m_headers.end (), key);
    if (item != m_headers.end ())
        item->m_value = value;
    else
        AddHeader (key, value);
}

std::string HttpHeaders::FindHeader (const std::string & key)
{
    std::list < HttpHeader >::iterator item = std::find (m_headers.begin (), m_headers.end (), key);
    if (item != m_headers.end ())
        return item->m_value;
    else
        return "";
}
