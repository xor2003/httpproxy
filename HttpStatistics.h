#ifndef HttpStatistics_h
#define HttpStatistics_h

#include "HttpClientRequestProxy.h"

#if __cplusplus > 199711L // if c++0x supported then use hash map
#define MAP std::unordered_map
#include <unordered_map>
#else
#define MAP std::map
#include <map>
#endif

#include <string>
#include <pthread.h>

struct stat_s
{
  unsigned long count;
  double bytes;
};

typedef MAP <std::string,stat_s> StatMap;


/// class HttpStatistics -
class HttpStatistics {
  // Attributes
private:
  pthread_mutex_t   stat_mutex;
  StatMap  stat;
  // Operations
public:
  /// @brief Init statistics
  HttpStatistics ();
  /// @brief Display statistics
  void  display () const;
  /// @brief Collect statistics
  /// @param site		(in)
  /// @param bytes		(in)
  void  add (const std::string& site, double bytes);
};

#endif // HttpStatistics_h
