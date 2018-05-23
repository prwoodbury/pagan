#include "pagan/PaganLexer.h"
#include "pagan/PaganStdInputStream.h"
#include "util/StringConversion.h"
#include "exception/GenericException.h"
#include <fstream>
#include <cctype>
#include <iostream>

using namespace std;

namespace Pagan {


void PaganLexer::lexProgram(PaganLexicalStream& lexicalStream,
                            PaganInputStream& inputStream) {
  while (!inputStream.eof()) {
    skipWhitespace(lexicalStream, inputStream);
    lexicalStream.append(nextLexeme(inputStream));
  }
}

PaganLexeme PaganLexer::nextLexeme(PaganInputStream& inputStream) {
  char next = inputStream.get();
  
  if (inputStream.eof()) {
    return PaganLexeme("EOF", inputStream.lineNum());
  }
  
  // 1-character terminals
  switch (next) {
  case '(':
    return PaganLexeme("OPEN_PAREN", inputStream.lineNum());
    break;
  case ')':
    return PaganLexeme("CLOSE_PAREN", inputStream.lineNum());
    break;
  case '.':
    return PaganLexeme("DOT", inputStream.lineNum());
    break;
  case '+':
    return PaganLexeme("PLUS", inputStream.lineNum());
    break;
  case '-':
    return PaganLexeme("MINUS", inputStream.lineNum());
    break;
  case '*':
    return PaganLexeme("STAR", inputStream.lineNum());
    break;
  case '/':
    return PaganLexeme("FORWARD_SLASH", inputStream.lineNum());
    break;
  case '=':
    // distinguish between = and ==
    if (inputStream.get() == '=') {
      return PaganLexeme("EQUALSEQUALS", inputStream.lineNum());
    }
    else {
      inputStream.unget();
      return PaganLexeme("EQUALS", inputStream.lineNum());
    }
    break;
  case '!':
    // distinguish between ! and !=
    if (inputStream.get() == '=') {
      return PaganLexeme("NOT_EQUALS", inputStream.lineNum());
    }
    else {
      inputStream.unget();
      return PaganLexeme("EXCLAMATION_MARK", inputStream.lineNum());
    }
    break;
  case '<':
    // distinguish between < and <=
    if (inputStream.get() == '=') {
      return PaganLexeme("LEFT_ANGLEBRACKETEQUALS", inputStream.lineNum());
    }
    else {
      inputStream.unget();
      return PaganLexeme("LEFT_ANGLEBRACKET", inputStream.lineNum());
    }
    break;
  case '>':
    // distinguish between > and >=
    if (inputStream.get() == '=') {
      return PaganLexeme("RIGHT_ANGLEBRACKETEQUALS", inputStream.lineNum());
    }
    else {
      inputStream.unget();
      return PaganLexeme("RIGHT_ANGLEBRACKET", inputStream.lineNum());
    }
    break;
  case '&':
    return PaganLexeme("AMPERSAND", inputStream.lineNum());
    break;
  case '|':
    return PaganLexeme("PIPE", inputStream.lineNum());
    break;
//  case '"':
//    return PaganLexeme("DOUBLEQUOTE");
//    break;
//  case '\\':
//    return PaganLexeme("BACKSLASH", inputStream.lineNum());
//    break;
  case ',':
    return PaganLexeme("COMMA", inputStream.lineNum());
    break;
//  case '_':
//    return PaganLexeme("UNDERSCORE", inputStream.lineNum());
//    break;
  case '{':
    return PaganLexeme("OPEN_CURLYBRACE", inputStream.lineNum());
    break;
  case '}':
    return PaganLexeme("CLOSE_CURLYBRACE", inputStream.lineNum());
    break;
  default:
    
    break;
  }
  
  // Multi-character terminals
  // Integer literals
  if (isdigit(next)) {
    inputStream.unget();
    return nextIntegerLexeme(inputStream);
  }
  // Keywords and variable identifiers
  else if (isalpha(next)) {
    inputStream.unget();
    return nextVarKeywordLexeme(inputStream);
  }
  // String literals
  else if (next == '"') {
    inputStream.unget();
    return nextStringLexeme(inputStream);
  }
  else {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganLexer::nextLexeme(PaganInputStream&)",
                           std::string("Unable to lex symbol '")
                           + next + "' at line "
                           + StringConversion::toString(
                                inputStream.lineNum()));
  }
}

PaganLexeme PaganLexer::nextIntegerLexeme(PaganInputStream& inputStream) {
  std::string intString;
  
  char next = inputStream.get();
  while (!(inputStream.eof()) && isdigit(next)) {
    intString += next;
    next = inputStream.get();
  }
  inputStream.unget();
  
  PaganInt nativeValue = StringConversion::fromString<int>(intString);
  
  return PaganLexeme("INTEGER", inputStream.lineNum(), nativeValue);
}

PaganLexeme PaganLexer::nextStringLexeme(PaganInputStream& inputStream) {
  std::string str;
  
  // skip leading '"'
  inputStream.get();
  
  readToSymbol(str, inputStream, '"');
  
  return PaganLexeme("STRING", inputStream.lineNum(), str);
}

PaganLexeme PaganLexer::nextVarKeywordLexeme(PaganInputStream& inputStream) {
  std::string name;
  
  char next = inputStream.get();
  // accept any combination of letters, numbers, and underscores
  // (but must begin with a letter)
  while (!(inputStream.eof())
           && (!isspace(next))
           && (isalpha(next)
               || isdigit(next)
               || (next == '_'))) {
    name += next;
    next = inputStream.get();
  }
  inputStream.unget();
  
  // Check for keywords
  if (name.compare("function") == 0) {
    return PaganLexeme("FUNCTION", inputStream.lineNum(), name);
  }
  else if (name.compare("struct") == 0) {
    return PaganLexeme("STRUCT", inputStream.lineNum(), name);
  }
  else if (name.compare("var") == 0) {
    return PaganLexeme("VAR", inputStream.lineNum(), name);
  }
  else if (name.compare("if") == 0) {
    return PaganLexeme("IF", inputStream.lineNum(), name);
  }
  else if (name.compare("else") == 0) {
    return PaganLexeme("ELSE", inputStream.lineNum(), name);
  }
  else if (name.compare("while") == 0) {
    return PaganLexeme("WHILE", inputStream.lineNum(), name);
  }
  // Not a keyword: assume variable identifier
  else {
    return PaganLexeme("VARIABLE", inputStream.lineNum(), name);
  }
}

void PaganLexer::skipWhitespace(PaganLexicalStream& lexicalStream,
                                PaganInputStream& inputStream) {
  char next;
  while (!(inputStream.eof())) {
    next = inputStream.get();
    
    // Comments
    if (next == '/') {
      // peek at next character
      char nextnext = inputStream.peek();
      
      // Single-line comment (//)
      if (nextnext == '/') {
        inputStream.get();
        
        // read to end of line
        skipSymbol(inputStream, '\n');
      }
      // Block comment (/* */)
      else if (nextnext == '*') {
        inputStream.get();
        
        char c;
        do {
          // read to next *
          skipSymbol(inputStream, '*');
          
          // get next character
          c = inputStream.get();
          
          // stop at EOF or when "*/" sequence is reached
        } while (!(inputStream.eof()) && (c != '/'));
      }
      // otherwise, not a comment: we're done
      else {
        inputStream.unget();
        break;
      }
      
    }
    // External file inclusion
    else if (next == '@') {
      // Read filename
      char c;
      std::string filename;
      while (!(inputStream.eof())) {
        // get next character
        c = inputStream.get();
        
        // stop when whitespace is reached
        if (isspace(c)) {
          break;
        }
        else {
          filename += c;
        }
      }
      
      // Open file and read lexemes
      std::ifstream ifs(filename.c_str());
      PaganStdInputStream newInputStream(ifs);
      lexProgram(lexicalStream, newInputStream);
      
      // Chop off the end-of-input lexeme so everything gets
      // properly integrated into the final program
      lexicalStream.unappend();
    }
    // If character is not whitespace and does not have some other special
    // meaning, we're done; otherwise, keep looping
    else if (!(isspace(next))) {
      inputStream.unget();
      break;
    }
    
  } 
}

void PaganLexer::skipSymbol(PaganInputStream& inputStream, char symbol) {
  while (!(inputStream.eof())) {
    char next = inputStream.get();
    if (next == symbol) {
      break;
    }
  }
}

void PaganLexer::readToSymbol(std::string& dst,
                                PaganInputStream& inputStream,
                                char symbol) {
  dst = "";
  while (!(inputStream.eof())) {
    char next = inputStream.get();
    if (next == symbol) {
      break;
    }
    else {
      dst += next;
    }
  }
}


} 
