#ifndef PAGANEXCEPTION_H
#define PAGANEXCEPTION_H


#include <exception>
#include <string>

// Macro to retrieve filename and line number for abbreviated
// exception constructors
#define PAGAN_SRCANDLINE __FILE__,__LINE__

namespace Pagan {


class PaganException : public std::exception {
public:
  PaganException();
  
  /**
   * Constructor.
   * @param nameOfSourceFile__ Name of the source file that triggered the
   * exception.
   * @param lineNum__ Number of the source file line at which the exception
   * was triggered.
   * @param source__ The function that triggered the exception.
   */
  PaganException(const char* nameOfSourceFile__,
                 int lineNum__,
                 const std::string& source__);
  
  const std::string& nameOfSourceFile() const;
  int lineNum() const;
  const std::string& source() const;
  
protected:

  /**
   * The name of the source file from which the exception was triggered.
   */
  std::string nameOfSourceFile_;

  /**
   * The number of the line at which the exception was triggered.
   */
  int lineNum_;

  /**
   * The name of the function that triggered the exception.
   */
  std::string source_;
  
};


}


#endif
