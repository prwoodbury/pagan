#include "pagan/PaganLexeme.h"

namespace Pagan {


PaganLexeme::PaganLexeme()
  : lineNum_(0),
    intValue_(0) { }

PaganLexeme::~PaganLexeme() { }
  
PaganLexeme::PaganLexeme(TypeID type__,
                         int lineNum__)
  : type_(type__),
    lineNum_(lineNum__),
    intValue_(0) { }
    
PaganLexeme::PaganLexeme(TypeID type__,
                         int lineNum__,
                         PaganInt intValue__)
  : type_(type__),
    lineNum_(lineNum__),
    intValue_(intValue__) { }
    
PaganLexeme::PaganLexeme(TypeID type__,
                         int lineNum__,
                         PaganString stringValue__)
  : type_(type__),
    lineNum_(lineNum__),
    intValue_(0),
    stringValue_(stringValue__) { }
  
PaganLexeme::TypeID PaganLexeme::type() const {
  return type_;
}

void PaganLexeme::setType(TypeID type__) {
  type_ = type__;
}
  
int PaganLexeme::lineNum() const {
  return lineNum_;
}

void PaganLexeme::setLineNum(int lineNum__) {
  lineNum_ = lineNum__;
}

PaganInt PaganLexeme::intValue() const {
  return intValue_;
}

void PaganLexeme::setIntValue(PaganInt intValue__) {
  intValue_ = intValue__;
}

PaganString PaganLexeme::stringValue() const {
  return stringValue_;
}

void PaganLexeme::setStringValue(PaganString stringValue__) {
  stringValue_ = stringValue__;
}

bool PaganLexeme::isType(TypeID type__) const {
  if (type_.compare(type__) == 0) {
    return true;
  }
  
  return false;
}

std::ostream& operator<<(std::ostream& ofs,
                         const PaganLexeme& lexeme) {
  ofs << "PaganLexeme: " << std::endl
      << "\ttype_: " << lexeme.type_ << std::endl
      << "\tlineNum_: " << lexeme.lineNum_ << std::endl
      << "\tintValue_: " << lexeme.intValue_ << std::endl
      << "\tstringValue_: " << lexeme.stringValue_ << std::endl;
  
  return ofs;
}


} 
