#ifndef PAGANSTDINPUTSTREAM_H
#define PAGANSTDINPUTSTREAM_H


#include "pagan/PaganInputStream.h"
#include <iostream>

namespace Pagan {


/**
 * Implementation of PaganInputStream for std::istream.
 */
class PaganStdInputStream : public PaganInputStream {
public:
  PaganStdInputStream(std::istream& ifs__);
  virtual ~PaganStdInputStream();
  
  virtual char get();
  
  virtual char peek();
  
  virtual void unget();
  
  virtual bool eof() const;
  
  virtual int lineNum() const;
protected:
  std::istream& ifs_;
  
  int lineNum_;
};

}


#endif
