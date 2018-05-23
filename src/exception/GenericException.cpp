#include "exception/GenericException.h"

namespace Pagan {


GenericException::GenericException(const char* nameOfSourceFile__,
                                   int lineNum__,
                                   const std::string& source__,
                                   const std::string& problem__)
  : PaganException(nameOfSourceFile__,
                   lineNum__,
                   source__),
    problem_(problem__) { };

const char* GenericException::what() const throw() {
  return problem_.c_str();
}

std::string GenericException::problem() const {
  return problem_;
}


} 
