#ifndef PAGANPARSER_H
#define PAGANPARSER_H


#include "pagan/PaganLexicalStream.h"
#include "pagan/PaganParseNode.h"

namespace Pagan {


/**
 * Pagan parse tree generator.
 * Transforms PaganLexicalStreams into a single PaganLexeme containing the
 * parse tree for the generated program.
 */
class PaganParser {
public:
  PaganParser();
  
  ~PaganParser();
  
  /**
   * Generates the parse tree for the program in the lexicalStream.
   */
  void parseProgram(PaganLexicalStream& lexicalStream__);
  
  /**
   * Returns a reference to the generated parse tree.
   * Do not call before calling parseProgram().
   */
  PaganParseNode& parseTree();
  
protected:
  
  /**
   * Pointer to the lexical stream used to generate the parse tree.
   * Ownership is not taken; this exists purely for the convenience of not
   * having to pass the stream around as a parameter to the various
   * parsing functions.
   */
  PaganLexicalStream* lexicalStream_;
  
  /**
   * The generated parse tree.
   * This is owned by the parser and is deleted on destruction.
   */
  PaganParseNode* parseTree_;

  /**
   * Returns a reference to the lexeme currently being analyzed.
   */
  PaganLexeme currentLexeme();
  
  /**
   * Returns line number of the current lexeme.
   */
  int currentLineNum();
  
  /**
   * Returns formatted string of current line number.
   */
  std::string currentLineNumString();
  
  /**
   * Returns true only if current lexeme is of the given type.
   */
  bool check(PaganLexeme::TypeID type);
  
  /**
   * Advances the lexicalStream_.
   */
  PaganLexeme advance();
  
  /**
   * Advances the lexicalStream_ only if current lexeme is of the given type.
   */
  PaganLexeme match(PaganLexeme::TypeID type);

  // Pending and read functions for each nonterminal
  
//  bool programPending();
  PaganParseNode* program();
  
  bool statementsPending();
  PaganParseNode* statements();
  
  bool statementPending();
  PaganParseNode* statement();
  
  bool expressionPending();
  PaganParseNode* expression();
  
  bool primaryPending();
  PaganParseNode* primary();
  
  bool operatorPending();
  PaganParseNode* operator_();
  
  bool unaryOperatorPending();
  PaganParseNode* unaryOperator();
  
  bool optExpressionListPending();
  PaganParseNode* optExpressionList();
  
  bool expressionListPending();
  PaganParseNode* expressionList();
  
  bool functionDefinitionPending();
  PaganParseNode* functionDefinition();
  
  bool structDefinitionPending();
  PaganParseNode* structDefinition();
  
  bool functionCallSubPending();
  PaganParseNode* functionCallSub();
  
  bool functionCallRPending();
  PaganParseNode* functionCallR();
  
  bool dotAccessListPending();
  PaganParseNode* dotAccessList();
  
  bool optVariableListPending();
  PaganParseNode* optVariableList();
  
  bool variableListPending();
  PaganParseNode* variableList();
  
  bool blockPending();
  PaganParseNode* block();
  
  bool optStatementsPending();
  PaganParseNode* optStatements();
  
  bool initializerPending();
  PaganParseNode* initializer();
  
  bool ifStatementPending();
  PaganParseNode* ifStatement();
  
  bool optElsePending();
  PaganParseNode* optElse();
  
  bool whileStatementPending();
  PaganParseNode* whileStatement();
  
  
};


}


#endif
