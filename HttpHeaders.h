#ifndef HttpHeaders_h
#define HttpHeaders_h

#include <list>
#include <string>
/// class HttpHeaders - Class to parse/create HTTP headers
class HttpHeaders {
  // Attributes
public:
        struct HttpHeader
    {
        HttpHeader (const std::string &key, const std::string &value)
            : m_key (key), m_value (value) { }

        bool
        operator==(const std::string &key) const
        {
            return key == m_key;
        }

        std::string m_key;
        std::string m_value;
    };
typedef std::list<HttpHeader> HeadersList;

private:
  HeadersList m_headers;
  // Operations
public:
  HttpHeaders ();
  /// @brief Parse HTTP headers
  /// @param buffer		(in)
  /// @param size		(in)
  const char * ParseHeaders (const char* buffer, size_t size);
  /* @brief Format HTTP headers
     *
     * Another note. Buffer size should be en
  /// @param buffer		(in/out) */
  void FormatHeaders (std::string& buffer) const;
  /// @brief  Add HTTP header
  /// @param key		(in)
  /// @param value		(in)
  void AddHeader (const std::string& key, const std::string& value);
  /// @brief Remove HTTP header
  /// @param key		(in)
  void RemoveHeader (const std::string& key);
  /// @brief Modify HTTP header
  /// @param key		(in)
  /// @param value		(in)
  void ModifyHeader (const std::string& key, const std::string& value);
  /* @brief Find value for the `key' http header
     *
     * If header doesn't exi
  /// @param key		(in) */
  std::string FindHeader (const std::string& key);
};

#endif // HttpHeaders_h
