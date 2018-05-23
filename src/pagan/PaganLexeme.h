#ifndef PAGANLEXEME_H
#define PAGANLEXEME_H


#include "PaganPrimitives.h"
#include <iostream>
#include <string>

namespace Pagan {


/**
 * Generic class representing all types of Pagan lexemes.
 */
class PaganLexeme {
public:
  typedef std::string TypeID;

  PaganLexeme();
  virtual ~PaganLexeme();
  PaganLexeme(TypeID type__,
              int lineNum__);
  PaganLexeme(TypeID type__,
              int lineNum__,
              PaganInt intValue__);
  PaganLexeme(TypeID type__,
              int lineNum__,
              PaganString stringValue__);
  
  TypeID type() const;
  void setType(TypeID type__);
  
  int lineNum() const;
  void setLineNum(int lineNum__);
  
  PaganInt intValue() const;
  void setIntValue(PaganInt intValue__);
  
  PaganString stringValue() const;
  void setStringValue(PaganString stringValue__);
  
  /**
   * Returns true if the lexeme is of the given type and false otherwise.
   */
  bool isType(TypeID type__) const;
  
  /**
   * std::ostream operator<< overload for formatted printing.
   */
  friend std::ostream& operator<<(std::ostream& ofs,
                                  const PaganLexeme& lexeme);
  
protected:
  
  /**
   * The lexeme's type.
   */
  TypeID type_;
  
  /**
   * The line number from which the lexeme originated.
   */
  int lineNum_;
  
  /**
   * The lexeme's integer value (only applicable to certain types).
   */
  PaganInt intValue_;
  
  /**
   * The lexeme's string value (only applicable to certain types).
   */
  PaganString stringValue_;
  
};


}


#endif
