#ifndef HttpRequest_h
#define HttpRequest_h

#include "HttpHeaders.h"

/* class HttpRequest - @brief Class to parse/create HTTP requests
 *
 *  HttpRequest objects are created */
class HttpRequest : public HttpHeaders {

    enum MethodEnum
    {
        GET = 0,
        UNSUPPORTED = 1
    };

  // Attributes
private:
  MethodEnum m_method;
  std::string m_host;
  unsigned short m_port;
  std::string m_path;
  std::string m_version;
  // Operations
public:
  /* @brief Default constructor
     *
     * Example:
     * HttpRequest req;
*/
  HttpRequest ();
  /* @brief Parse HTTP header
     *
     * Example:
     * HttpRequest req;
     *
     @param buffer		(in)
     @param size		(in) */
  const char * ParseRequest (const char * buffer, size_t size);
  /* @brief Format HTTP request
     *
     * Note that buffer size should be enough
  /// @param std::string&		(in) */
  void FormatRequest ( std::string&) const;
  /// @brief Get method of the HTTP request
  MethodEnum GetMethod () const;
  /// @brief Set method of the HTTP request
  /// @param method		(in)
  void SetMethod (MethodEnum method);
  /// @brief Get host of the HTTP request
  std::string GetHost() const;
  /// @brief Set host of the HTTP request
  /// @param host		(in)
  void SetHost (const std::string& host);
  /// @brief Get port of the HTTP request
  unsigned short GetPort() const;
  /// @brief Set port of the HTTP request
  /// @param port		(in)
  void SetPort (unsigned short port);
  /// @brief Get path of the HTTP request
  std::string GetPath () const;
  /// @brief Set path of the HTTP request
  /// @param path		(in)
  void SetPath (const std::string& path);
  /// @brief Get version of the HTTP request
  std::string GetVersion () const;
  /// @brief Set version of the HTTP request
  /// @param version		(in)
  void SetVersion (const std::string& version);
};

#endif // HttpRequest_h
