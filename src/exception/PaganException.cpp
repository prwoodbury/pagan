#include "exception/PaganException.h"

namespace Pagan {


PaganException::PaganException()
  : nameOfSourceFile_("unknown source file"),
    lineNum_(-1),
    source_("unknown source") { };

PaganException::PaganException(const char* nameOfSourceFile__,
                               int lineNum__,
                               const std::string& source__)
  : nameOfSourceFile_(std::string(nameOfSourceFile__)),
    lineNum_(lineNum__),
    source_(source__) { };

const std::string& PaganException::nameOfSourceFile() const {
  return nameOfSourceFile_;
}

int PaganException::lineNum() const {
  return lineNum_;
}

const std::string& PaganException::source() const {
  return source_;
}


} 
