#ifndef ENVIRONMENTLOOKUPEXCEPTION_H
#define ENVIRONMENTLOOKUPEXCEPTION_H


#include "exception/PaganException.h"
#include <string>

namespace Pagan {


/**
 * Exception thrown when a variable cannot be found in an environment.
 */
class EnvironmentLookupException : public PaganException {
public:
  EnvironmentLookupException(const char* nameOfSourceFile__,
                 int lineNum__,
                 const std::string& source__,
                 const std::string& variableName__);
  
  /**
   * Returns the name of the variable that could not be found.
   */
  std::string variableName() const;
  
  /**
   * Override of std::exception::what().
   * @return C-string explaining the exception.
   */
  const char* what() const throw();
  
protected:
  /**
   * The name of the variable that could not be found.
   */
  std::string variableName_;
};


}


#endif
