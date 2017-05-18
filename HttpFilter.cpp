/**
 * @File Implementation for site filter
 *
 */
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

#include <string.h>

#include "HttpFilter.h"
#include "HttpCommon.h"

HttpFilter::HttpFilter()
{
    load("blacklist.txt");
}

HttpFilter::HttpFilter(const std::string& filename)
{
    load(filename);
}

void HttpFilter::load(const std::string& filename)
{
    std::ifstream infile( filename.c_str() );

    while (infile)
    {
        std::string s;
        if (!getline( infile, s )) break;
        block_list.insert( s );

    }
    TRACE("Blacklist rules loaded: " << block_list.size() );
}

bool HttpFilter::check(const std::string& site) const
{

    SET<std::string>::const_iterator i = block_list.find(site);
    if (i != block_list.end() )
        return true;
    const std::string& subdomain=site;

    for( SET<std::string>::const_iterator i = block_list.begin(); i!= block_list.end(); i++)
        if (strstr(i->c_str() , subdomain.c_str() ) != NULL)
            return true;

    return false;

}
