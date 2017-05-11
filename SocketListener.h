#ifndef SocketListener_h
#define SocketListener_h

#include "HttpClientRequestProxy.h"

/// class SocketListener -
class SocketListener {
  // Associations
  HttpClientRequestProxy* unnamed_1;
  // Attributes
private:
  std::set<HttpClientRequestProxy*> m_list;
  // Operations
public:
  void executer ();
};

#endif // SocketListener_h
