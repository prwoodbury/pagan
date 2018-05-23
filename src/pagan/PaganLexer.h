#ifndef PAGANLEXER_H
#define PAGANLEXER_H


#include "pagan/PaganInputStream.h"
#include "pagan/PaganLexicalStream.h"
#include <string>

namespace Pagan {


/**
 * Primary class for transforming PaganInputStreams to PaganLexicalStreams.
 */
class PaganLexer {
public:
  /**
   * Reads a Pagan program from inputStream and appends lexemes to lexemeStream.
   * The inputStream will be advanced to EOF.
   */
  static void lexProgram(PaganLexicalStream& lexicalStream,
                         PaganInputStream& inputStream);
protected:

  /**
   * Reads and returns the next lexeme in the inputStream.
   */
  static PaganLexeme nextLexeme(PaganInputStream& inputStream);
  
  /**
   * Reads and returns an INTEGER lexeme from the inputStream.
   */
  static PaganLexeme nextIntegerLexeme(PaganInputStream& inputStream);
  
  /**
   * Reads and returns a STRING lexeme from the inputStream.
   */
  static PaganLexeme nextStringLexeme(PaganInputStream& inputStream);
  
  /**
   * Reads and returns a variable or keyword lexeme from the inputStream.
   */
  static PaganLexeme nextVarKeywordLexeme(PaganInputStream& inputStream);
  
  /**
   * Advances the inputStream past whitespace, comments, etc.
   * This also functions as a primitive preprocessor; if external file
   * inclusions are present in the source, they will be detected and their
   * content added to the lexicalStream.
   */
  static void skipWhitespace(PaganLexicalStream& lexicalStream,
                             PaganInputStream& inputStream);
  
  /**
   * Advances the inputStream past the next occurence of the given symbol.
   */
  static void skipSymbol(PaganInputStream& inputStream,
                         char symbol);
  
  /**
   * Advances the inputStream past the next occurence of the given symbol.
   * The read input is placed in dst. The symbol is not included in the
   * output.
   */
  static void readToSymbol(std::string& dst,
                           PaganInputStream& inputStream,
                           char symbol);
  
};


}


#endif
