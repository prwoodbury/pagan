#include "exception/EnvironmentLookupException.h"

using namespace std;

namespace Pagan {


EnvironmentLookupException::EnvironmentLookupException(
                 const char* nameOfSourceFile__,
                 int lineNum__,
                 const std::string& source__,
                 const std::string& variableName__)
  : PaganException(nameOfSourceFile__,
                   lineNum__,
                   source__),
    variableName_(variableName__) { }

const char* EnvironmentLookupException::what() const throw() {
  return (string("Unable to locate variable '")
            + variableName_
            + "' in environment").c_str();
}

std::string EnvironmentLookupException::variableName() const {
  return variableName_;
}


} 
