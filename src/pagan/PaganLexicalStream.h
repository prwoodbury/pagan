#ifndef PAGANLEXICALSTREAM_H
#define PAGANLEXICALSTREAM_H


#include "pagan/PaganLexeme.h"
#include <vector>

namespace Pagan {


/**
 * Represents a stream of ordered lexemes.
 * Access is read-write, but new lexemes may only be appended rather than
 * inserted arbitrarily.
 */
class PaganLexicalStream {
public:
  PaganLexicalStream();
  
  /**
   * Fetches the next lexeme in the stream and advances the stream.
   */
  PaganLexeme get();
  
  /**
   * Fetches the next lexeme in the stream without advancing it.
   */
  PaganLexeme peek();
  
  /**
   * "Undoes" the previous get() operation, restoring the stream's last state.
   */
  void unget();
  
  /**
   * Appends a new lexeme to the stream.
   */
  void append(PaganLexeme lexeme);
  
  /**
   * Removes the final lexeme from the stream.
   */
  void unappend();
  
  /**
   * Returns true if end of the stream has been reached and false otherwise.
   */
  bool eof() const;
  
protected:
  typedef std::vector<PaganLexeme> LexemeCollection;
  
  /**
   * The contained lexemes.
   */
  LexemeCollection lexemes_;
  
  /**
   * Index in lexemes_ of the next lexeme to be fetched by get().
   */
  unsigned int getPos_;
  
};


}


#endif 
