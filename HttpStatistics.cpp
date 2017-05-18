/**
 * @File Implementation for site statistics
 *
 */
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

#include <string.h>

#include "HttpStatistics.h"
#include "HttpCommon.h"

HttpStatistics::HttpStatistics()
{
    pthread_mutex_init (&stat_mutex, NULL);

}

void HttpStatistics::display() const
{
    std::cout << std::endl;
    for( StatMap::const_iterator i = stat.begin(); i!= stat.end(); i++)
        std::cout << "Site: " << i->first << " Count " << i->second.count << " Bytes " << i->second.bytes << std::endl;
    std::cout << std::endl;
}

void HttpStatistics::add(const std::string& site, double bytes)
{
  pthread_mutex_lock (&stat_mutex);

    StatMap::iterator i = stat.find(site); // Search in log
    if (i == stat.end() )
      {
        stat_s myst;
             myst.count=1;  // first time
             myst.bytes=bytes;
         stat[site]=myst;
      }
      else
      {
         ++(i->second).count; // increase counters
         (i->second).bytes+=bytes;
      }
  pthread_mutex_unlock (&stat_mutex);

}
