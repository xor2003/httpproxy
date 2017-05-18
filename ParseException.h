#ifndef ParseException_h
#define ParseException_h


#include <exception>

/// class ParseException -
class ParseException : public std::exception {
  // Attributes
private:
  std::string  m_reason;
  // Operations
public:
    ParseException (const std::string &reason) : m_reason (reason) { }
    virtual ~ParseException () throw () { }
    virtual const char* what() const throw ()
    {
        return m_reason.c_str ();
    }
};

#endif // ParseException_h
