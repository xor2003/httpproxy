#ifndef HttpResponse_h
#define HttpResponse_h

#include "HttpHeaders.h"

/// class HttpResponse
/*! @brief Class to parse/create HTTP responses
 *
 *  HttpResponse objects are created from parsing a buffer containing a HTTP
 *  response.
*/
class HttpResponse : public HttpHeaders {
  // Attributes
private:
  std::string  m_version;
  std::string  m_statusCode;
  std::string  m_statusMsg;
  // Operations
public:
  /*! @brief Default constructor
     *
     * Example:
     * HttpRequest req;
*/
  HttpResponse ();
  /*! @brief Parse HTTP header
     *
     * Example:
     * HttpRequest req;
     *
  /// @param buffer		(in)
  /// @param size		(in)
  */
  const char* ParseResponse (const char* buffer, size_t size);
  /// @brief Get total length of the HTTP header (buffer size necessary to hold formatted HTTP request)
  size_t GetTotalLength ();
  /// @brief Get version of the HTTP request
  std::string GetVersion () const;
  /// @brief Set version of the HTTP request
  /// @param version		(in)
  void SetVersion (const std::string& version);
  /// @brief Get status code
  std::string GetStatusCode () const;
  /// @brief Set status code
  /// @param code		(in)
  void SetStatusCode (const std::string& code);
  /// @brief Get status message
  std::string GetStatusMsg () const;
  /// @brief Set status message
  /// @param msg		(in)
  void SetStatusMsg (const std::string& msg);
};

#endif // HttpResponse_h
