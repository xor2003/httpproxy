#ifndef HttpFilter_h
#define HttpFilter_h

#if __cplusplus > 199711L // if c++0x supported then use hash set
#define SET std::unordered_set
#include <unordered_set>
#else
#define SET std::set
#include <set>
#endif


#include <string>
#include "HttpClientRequestProxy.h"

/// class HttpFilter - Load blacklist from file
class HttpFilter {
  // Attributes
private:
  SET<std::string> block_list;
  // Operations
public:
  HttpFilter ();
  HttpFilter (const std::string& filename);
  void  load (const std::string& filename);
  /* Check if in list.
    Returns true if contain, false if not
  /// @param site		(in) */
  bool  check (const std::string& site) const;
};

#endif // HttpFilter
