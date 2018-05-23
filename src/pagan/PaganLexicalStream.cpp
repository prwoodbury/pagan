#include "pagan/PaganLexicalStream.h"
#include "exception/GenericException.h"

namespace Pagan {


PaganLexicalStream::PaganLexicalStream()
  : getPos_(0) { }
  
PaganLexeme PaganLexicalStream::get() {
  if (getPos_ >= lexemes_.size()) {
    throw GenericException(PAGAN_SRCANDLINE,
                          "PaganLexicalStream::get()",
                          "getPos_ >= lexemes_.size()");
  }
  
  return lexemes_[getPos_++];
  
  // Advance stream only if more input exists
//  if (getPos_ < (lexemes_.size() - 1)) {
//    ++getPos_;
//  }
}

PaganLexeme PaganLexicalStream::peek() {
  PaganLexeme lexeme = get();
  unget();
  return lexeme;
}

void PaganLexicalStream::unget() {
  if (getPos_ <= 0) {
    throw GenericException(PAGAN_SRCANDLINE,
                          "PaganLexicalStream::unget()",
                          "getPos_ <= 0)");
  }
  
  --getPos_;
}

void PaganLexicalStream::append(PaganLexeme lexeme) {
  lexemes_.push_back(lexeme);
}

void PaganLexicalStream::unappend() {
  lexemes_.pop_back();
}

bool PaganLexicalStream::eof() const {
  if (getPos_ >= lexemes_.size()) {
    return true;
  }
  
  return false;
}


} 
