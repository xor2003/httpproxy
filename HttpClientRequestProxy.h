#ifndef HttpClientRequestProxy_h
#define HttpClientRequestProxy_h


#if __cplusplus > 199711L   // if c++0x supported then use hash map
#define MAP std::unordered_map
#include <unordered_map>
#else
#define MAP std::map
#include <map>
#endif

typedef MAP < std::string, std::string > cache_t;

class HttpFilter;
class HttpStatistics;

/// class HttpClientRequestProxy - Proxing single client: caching or blocking http requests
class HttpClientRequestProxy {
  // Attributes
private:
  /// Incoming socket
  int        m_client_fd;
  /// Proxy cache
  cache_t * m_cache;
  /// Hostname filter
  HttpFilter* m_bfilter;
  HttpStatistics* m_hstatistics;
  pthread_mutex_t *    m_mutex;
  // Operations
public:
  HttpClientRequestProxy (int client_fd_i, cache_t* cache_i, HttpFilter* bfilter_i, HttpStatistics* hstatistics,
                               pthread_mutex_t *    mutex_i);
  ~HttpClientRequestProxy ();
  /// Working thread executer
  /// @param thread_p		(in)
  void executer();
protected:
  int  run ();
};

#endif
