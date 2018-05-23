#include "pagan/PaganParser.h"
#include "exception/GenericException.h"
#include "util/StringConversion.h"
#include <cstddef>

namespace Pagan {


PaganParser::PaganParser()
  : lexicalStream_(NULL),
    parseTree_(NULL) { };
  
PaganParser::~PaganParser() {
  delete parseTree_;
}

void PaganParser::parseProgram(PaganLexicalStream& lexicalStream__) {
  // Destroy any existing parse tree
  delete parseTree_;
  
  // Remember the lexical stream (purely as a convenience so we don't
  // need to pass it around to all the parse functions)
  lexicalStream_ = &lexicalStream__;
  
  // Parse everything
  parseTree_ = program();
}

PaganParseNode& PaganParser::parseTree() {
  return *parseTree_;
}

PaganLexeme PaganParser::currentLexeme() {
  return lexicalStream_->peek();
}

int PaganParser::currentLineNum() {
  return currentLexeme().lineNum();
}

std::string PaganParser::currentLineNumString() {
  return std::string("Line ")
                     + StringConversion::toString(
                        currentLineNum())
                     + ": ";
}
  
/*bool PaganParser::programPending() {
  
  if (statementsPending()) {
    return true;
  }
    // Empty programs are not allowed
//    throw GenericException(PAGAN_SRCANDLINE,
//                           "PaganParser::programPending()",
//                           currentLineNumString()
//                             + "Syntax error: Empty program");
  
  return false;
} */

bool PaganParser::check(PaganLexeme::TypeID type) {
  if (currentLexeme().isType(type)) {
    return true;
  }
  
  return false;
}

PaganLexeme PaganParser::advance() {
  return lexicalStream_->get();
}

PaganLexeme PaganParser::match(PaganLexeme::TypeID type) {
  if (!check(type)) {
    std::string errorString = currentLineNumString();
    errorString += "Syntax error: Expected "
                             + type
                             + ", got "
                             + currentLexeme().type();
    if (!(currentLexeme().stringValue().empty())) {
      errorString += " (identifier: "
                       + currentLexeme().stringValue()
                       + ")";
    }
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganParser::match(PaganLexeme::TypeID)",
                           errorString);
  }
  
  return advance();
}

PaganParseNode* PaganParser::program() {
  PaganParseNode* node
    = new PaganParseNode("PROGRAM", currentLineNum());
  
  node->setRight(statements());

  return node;
}
  
bool PaganParser::statementsPending() {
  return statementPending();
}

PaganParseNode* PaganParser::statements() {
  PaganParseNode* node = statement();
  
  if (statementsPending()) {
    PaganParseNode* root
      = new PaganParseNode("STATEMENTS", currentLineNum());
    root->setLeft(node);
    root->setRight(statements());
    return root;
  }
  else {
    return node;
  }
  
}
  
bool PaganParser::statementPending() {
//  return expressionPending();
  return expressionPending()
      || initializerPending()
      || ifStatementPending()
      || whileStatementPending()
      || blockPending();
}

PaganParseNode* PaganParser::statement() {
  PaganParseNode* node
    = new PaganParseNode("STATEMENT", currentLineNum());
      
  if (expressionPending()) {
    node->setRight(expression());
  }
  else if (initializerPending()) {
    node->setRight(initializer());
  }
  else if (ifStatementPending()) {
    node->setRight(ifStatement());
  }
  else if (whileStatementPending()) {
    node->setRight(whileStatement());
  }
  else if (blockPending()) {
    node->setRight(block());
  }
  
  return node;
  
}

bool PaganParser::expressionPending() {
  return primaryPending();
}

PaganParseNode* PaganParser::expression() {
  PaganParseNode* node = primary();
  
  // binary operator
  if (operatorPending()) {
    // operator
    PaganParseNode* root
      = new PaganParseNode("EXPRESSION_BINARY", currentLineNum());
    root->setLeft(operator_());
    
    // right expression
    PaganParseNode* right
      = new PaganParseNode("EXPRESSION_BINARY_R", currentLineNum());
    right->setLeft(node);
    right->setRight(expression());
    root->setRight(right);
    
    return root;
  }
  // sole primary
  else {
    return node;
  }
}
  
bool PaganParser::primaryPending() {
  return check("STRING")
          || check("INTEGER")
          || check("VARIABLE")
          || unaryOperatorPending()
          || functionDefinitionPending()
          || structDefinitionPending()
          || check("OPEN_PAREN");
}

PaganParseNode* PaganParser::primary() {
  PaganParseNode* node = NULL;
  
  if (check("STRING")) {
    node = new PaganParseNode(match("STRING"));
  }
  else if (check("INTEGER")) {
    node = new PaganParseNode(match("INTEGER"));
  }
  else if (check("VARIABLE")) {
    node = new PaganParseNode(match("VARIABLE"));
    
    // possible function call
    if (functionCallSubPending()) {
      PaganParseNode* root
        = functionCallSub();
      
      // attach to deepest level of tree
      PaganParseNode* finder = root;
      while (finder->left() != NULL) {
        finder = finder->left();
      }
      finder->setLeft(node);
        
      node = root;
    }
    // possible dot access
    else if (check("DOT")) {
      PaganParseNode* root
        = new PaganParseNode("DOT_ACCESS", currentLineNum());
        
      root->setRight(node);
        
      // accessed variable
      match("DOT");
      root->setLeft(dotAccessList());
      
      node = root;
      
      // possible function calls
      PaganParseNode* callCheck
        = functionCallR();
      if (callCheck != NULL) {
        // attach to deepest level of tree
        PaganParseNode* finder = callCheck;
        while (finder->left() != NULL) {
          finder = finder->left();
        }
        finder->setLeft(node);
        node = callCheck;
      }
      
    }
    
  }
  else if (unaryOperatorPending()) {
    node = unaryOperator();
    node->setRight(primary());
  }
  else if (functionDefinitionPending()) {
    node = functionDefinition();
    
    // **IMMEDIATE FUNCTION CALL
    // While it's a neat feature, this causes a nasty syntax issue because
    // the parser cannot distinguish between a function definition followed
    // by an immediate call versus a parenthesized expression that happens
    // to directly follow a function definition. Since I don't want to add
    // some special terminator character, and it's relatively useless anyway,
    // I've taken it out.
/*    // possible function call
    if (functionCallSubPending()) {
      PaganParseNode* root
        = functionCallSub();
      
      // attach to deepest level of tree
      PaganParseNode* finder = root;
      while (finder->left() != NULL) {
        finder = finder->left();
      }
      finder->setLeft(node);
        
      node = root;
    } */
    
  }
  else if (structDefinitionPending()) {
    node = structDefinition();
  }
  else {
    match("OPEN_PAREN");
    node = new PaganParseNode("PAREN_EXPRESSION", currentLineNum());
    node->setRight(expression());
    match("CLOSE_PAREN");
  }
  
  return node;
}
  
bool PaganParser::operatorPending() {
  return check("PLUS")
          || check("MINUS")
          || check("STAR")
          || check("FORWARD_SLASH")
          || check("EQUALSEQUALS")
          || check("NOT_EQUALS")
          || check("EQUALS")
          || check("LEFT_ANGLEBRACKET")
          || check("RIGHT_ANGLEBRACKET")
          || check("LEFT_ANGLEBRACKETEQUALS")
          || check("RIGHT_ANGLEBRACKETEQUALS")
          || check("AMPERSAND")
          || check("PIPE");
}

PaganParseNode* PaganParser::operator_() {
  PaganParseNode* node = NULL;
  
  if (check("PLUS")) {
    node = new PaganParseNode(match("PLUS"));
  }
  else if (check("MINUS")) {
    node = new PaganParseNode(match("MINUS"));
  }
  else if (check("STAR")) {
    node = new PaganParseNode(match("STAR"));
  }
  else if (check("FORWARD_SLASH")) {
    node = new PaganParseNode(match("FORWARD_SLASH"));
  }
  else if (check("EQUALSEQUALS")) {
    node = new PaganParseNode(match("EQUALSEQUALS"));
  }
  else if (check("NOT_EQUALS")) {
    node = new PaganParseNode(match("NOT_EQUALS"));
  }
  else if (check("EQUALS")) {
    node = new PaganParseNode(match("EQUALS"));
  }
  else if (check("LEFT_ANGLEBRACKET")) {
    node = new PaganParseNode(match("LEFT_ANGLEBRACKET"));
  }
  else if (check("RIGHT_ANGLEBRACKET")) {
    node = new PaganParseNode(match("RIGHT_ANGLEBRACKET"));
  }
  else if (check("LEFT_ANGLEBRACKETEQUALS")) {
    node = new PaganParseNode(match("LEFT_ANGLEBRACKETEQUALS"));
  }
  else if (check("RIGHT_ANGLEBRACKETEQUALS")) {
    node = new PaganParseNode(match("RIGHT_ANGLEBRACKETEQUALS"));
  }
  else if (check("AMPERSAND")) {
    node = new PaganParseNode(match("AMPERSAND"));
  }
  else {
    node = new PaganParseNode(match("PIPE"));
  }
  
  return node;
}
  
bool PaganParser::unaryOperatorPending() {
  return check("PLUS")
          || check("MINUS")
          || check("EXCLAMATION_MARK");
}

PaganParseNode* PaganParser::unaryOperator() {
  PaganParseNode* node = NULL;
  
  if (check("PLUS")) {
    match("PLUS");
    node = new PaganParseNode(
      "UPLUS", currentLineNum());
  }
  else if (check("MINUS")) {
    match("MINUS");
    node = new PaganParseNode(
      "UMINUS", currentLineNum());
  }
  else {
    match("EXCLAMATION_MARK");
    node = new PaganParseNode(
      "UEXCLAMATION_MARK", currentLineNum());
  }
  
  return node;
}
  
bool PaganParser::optExpressionListPending() {
  return true;
}

PaganParseNode* PaganParser::optExpressionList() {
//  PaganParseNode* root
//    = new PaganParseNode("OPT_EXPRESSION_LIST", currentLineNum());
  PaganParseNode* node = NULL;
  
  if (expressionListPending()) {
    node = expressionList();
  }
  
  return node;
}
  
bool PaganParser::expressionListPending() {
  return expressionPending();
}

PaganParseNode* PaganParser::expressionList() {
  PaganParseNode* node
    = new PaganParseNode("EXPRESSION_LIST", currentLineNum());
  
  node->setLeft(expression());
  
  if (check("COMMA")) {
    match("COMMA");
    node->setRight(expressionList());
  }
  
  return node;
}
  
bool PaganParser::functionDefinitionPending() {
  return check("FUNCTION");
}

PaganParseNode* PaganParser::functionDefinition() {
  
  PaganParseNode* node
    = new PaganParseNode(match("FUNCTION"));
  
  // named function
  if (check("VARIABLE")) {
    node->setLeft(new PaganParseNode(match("VARIABLE")));
  }
  
  node->setRight(
    new PaganParseNode("FUNCTION_R", currentLineNum()));
  // formal parameters
  match("OPEN_PAREN");
  node->right()->setLeft(optVariableList());
  match("CLOSE_PAREN");
  // body
  node->right()->setRight(block());
  
  return node;
}
  
bool PaganParser::structDefinitionPending() {
  return check("STRUCT");
}

PaganParseNode* PaganParser::structDefinition() {
  
  PaganParseNode* node
    = new PaganParseNode(match("STRUCT"));
  
  // named struct
  if (check("VARIABLE")) {
    node->setLeft(new PaganParseNode(match("VARIABLE")));
  }
  
  // body
  node->setRight(block());
  
  return node;
}
  
/*bool PaganParser::functionCallPending() {
  return check("VARIABLE")
          || functionDefinitionPending();
}

PaganParseNode* PaganParser::functionCall() {
  PaganParseNode* root
    = new PaganParseNode("FUNCTION_CALL", currentLineNum());
  root->setRight(new PaganParseNode("FUNCTION_CALL_R", currentLineNum()));
    
  // calling named function
  if (check("VARIABLE")) {
    root->setLeft(new PaganParseNode(match("VARIABLE")));
  }
  // calling anonymous function
  else {
    root->setLeft(functionDefinition());
  }
    
  match("OPEN_PAREN");
  
  // arguments
  root->setRight(optExpressionList());
  
  match("CLOSE_PAREN");
  
  // possible calls of this call
  root->right()->setRight(functionCallR());
    
  return root;
} */
  
bool PaganParser::functionCallSubPending() {
  return check("OPEN_PAREN");
}

PaganParseNode* PaganParser::functionCallSub() {
  PaganParseNode* node
    = new PaganParseNode("FUNCTION_CALL", currentLineNum());

  match("OPEN_PAREN");
  
  // arguments
  node->setRight(optExpressionList());
  
  match("CLOSE_PAREN");
  
  // possible calls of this call
  PaganParseNode* next = functionCallR();
  // Attach to deepest level of tree.
  // In order to parse these calls, we must evaluate them
  // from left to right. However, our recursive descent approach
  // naturally produces right-to-left evaluation. The simplest
  // way to deal with it is a linear search.
  if (next != NULL) {
    PaganParseNode* finder = next;
    while (finder->left() != NULL) {
      finder = finder->left();
    }
    finder->setLeft(node);
    // return the root of the tree (outermost call)
    return next;
  }
  else {
    // this is the outermost call, and the root node in the tree
    return node;
  }
}

bool PaganParser::functionCallRPending() {
  return true;
}

PaganParseNode* PaganParser::functionCallR() {
  if (check("OPEN_PAREN")) {
    PaganParseNode* node
      = new PaganParseNode("FUNCTION_CALL", currentLineNum());
  
    match("OPEN_PAREN");
    
    // arguments
    node->setRight(optExpressionList());
    
    match("CLOSE_PAREN");
    
    // possible calls of this call
    PaganParseNode* next = functionCallR();
    // Attach to deepest level of tree.
    // In order to parse these calls, we must evaluate them
    // from left to right. However, our recursive descent approach
    // naturally produces right-to-left evaluation. The simplest
    // way to deal with it is a linear search.
    if (next != NULL) {
      PaganParseNode* finder = next;
      while (finder->left() != NULL) {
        finder = finder->left();
      }
      finder->setLeft(node);
      // return the root of the tree (outermost call)
      return next;
    }
    else {
      // this is the outermost call, and the root node in the tree
      return node;
    }
  }
  else {
    return NULL;
  }
}
  
bool PaganParser::dotAccessListPending() {
  return check("VARIABLE");
}

PaganParseNode* PaganParser::dotAccessList() {
  PaganParseNode* node
    = new PaganParseNode("DOT_ACCESS", currentLineNum());
  
  node->setRight(new PaganParseNode(match("VARIABLE")));
  
  // sub-access
  if (check("DOT")) {
    match("DOT");
    node->setLeft(dotAccessList());
  }
  
  return node;
}
  
bool PaganParser::optVariableListPending() {
  return true;
}

PaganParseNode* PaganParser::optVariableList() {
  PaganParseNode* node = NULL;
  
  if (variableListPending()) {
    node = variableList();
  }
  
  return node;
}

bool PaganParser::variableListPending() {
  return check("VARIABLE");
}

PaganParseNode* PaganParser::variableList() {
  PaganParseNode* node
    = new PaganParseNode("VARIABLE_LIST", currentLineNum());
  
  node->setLeft(new PaganParseNode(match("VARIABLE")));
  
  if (check("COMMA")) {
    match("COMMA");
    node->setRight(variableList());
  }
  
  return node;
}
  
bool PaganParser::blockPending() {
  return check("OPEN_CURLYBRACE");
}

PaganParseNode* PaganParser::block() {
  match("OPEN_CURLYBRACE");
  
  PaganParseNode* node
    = new PaganParseNode("BLOCK", currentLineNum());
  
  node->setRight(optStatements());
  
  match("CLOSE_CURLYBRACE");
  return node;
}
  
bool PaganParser::optStatementsPending() {
  return true;
}

PaganParseNode* PaganParser::optStatements() {
  PaganParseNode* node = NULL;
  
  if (statementsPending()) {
    node = statements();
  }
  
  return node;
}
  
bool PaganParser::initializerPending() {
  return check("VAR");
}

PaganParseNode* PaganParser::initializer() {
  PaganParseNode* node
    = new PaganParseNode(match("VAR"));
    
//  PaganParseNode* node
//    = new PaganParseNode("INITIALIZER", currentLineNum());
  
  node->setLeft(new PaganParseNode(match("VARIABLE")));
  
  // optional initialization value
  if (check("EQUALS")) {
    match("EQUALS");
    node->setRight(expression());
  }
  
  return node;
}
  
bool PaganParser::ifStatementPending() {
  return check("IF");
}

PaganParseNode* PaganParser::ifStatement() {
  PaganParseNode* node
    = new PaganParseNode(match("IF"));
  
  // condition
  match("OPEN_PAREN");
  node->setLeft(statement());
  match("CLOSE_PAREN");
  
  node->setRight(new PaganParseNode("IF_R", currentLineNum()));
  
  // body
  node->right()->setLeft(statement());
  
  // optElse
  node->right()->setRight(optElse());
  
  return node;
}
  
bool PaganParser::optElsePending() {
  return true;
}

PaganParseNode* PaganParser::optElse() {
  if (check("ELSE")) {
    match("ELSE");
    return statement();
  }
  else {
    return NULL;
  }
}
  
bool PaganParser::whileStatementPending() {
  return check("WHILE");
}

PaganParseNode* PaganParser::whileStatement() {
  PaganParseNode* node
    = new PaganParseNode(match("WHILE"));
  
  // condition
  match("OPEN_PAREN");
  node->setLeft(statement());
  match("CLOSE_PAREN");
  
  // body
  node->setRight(statement());
  
  return node;
}


} 
