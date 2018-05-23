#include "pagan/PaganStdInputStream.h"
#include <cstddef>

namespace Pagan {

    
PaganStdInputStream::PaganStdInputStream(std::istream& ifs__)
  : PaganInputStream(),
    ifs_(ifs__),
    lineNum_(1) { }
    
PaganStdInputStream::~PaganStdInputStream() { }

char PaganStdInputStream::get() {
  char next = ifs_.get();
  
  if (next == '\n') {
    ++lineNum_;
  }
  
  return next;
}
  
char PaganStdInputStream::peek() {
  return ifs_.peek();
}

void PaganStdInputStream::unget() {
  ifs_.unget();
  
  // Ensure line count remains in sync if a line break is put back
  if (ifs_.peek() == '\n') {
    --lineNum_;
  }
}

bool PaganStdInputStream::eof() const {
//  return ifs_.eof();
  // Treat any stream failure as EOF
  return ifs_.eof() || ifs_.fail();
}
  
int PaganStdInputStream::lineNum() const {
  return lineNum_;
}


} 
