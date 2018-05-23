#ifndef PAGANEVALUATOR_H
#define PAGANEVALUATOR_H


#include "pagan/PaganParseNode.h"
#include "pagan/PaganEnvironment.h"
#include <string>
#include <list>
#include <iostream>

namespace Pagan {


/**
 * Evaluator for Pagan parse trees.
 */
class PaganEvaluator {
public:
  static void evaluateProgram(PaganParseNode& parseTree);
protected:

  typedef std::list<PaganParseNode*> NodeCollection;

  // Coercion helpers
  
  static PaganInt asInt(PaganParseNode* node);
  
  static PaganString asString(PaganParseNode* node);
  
  // Exception helpers
  
  static void unaryOperatorException(
    PaganParseNode* right, 
    PaganParseNode* op);
  
  static void binaryOperatorException(
    PaganParseNode* left, PaganParseNode* right, 
    PaganParseNode* op);
  
  // Function argument and parameter helpers
  
  static void evaluateFunctionCallArgs(
    NodeCollection& args,
    PaganParseNode* pt, PaganEnvironment* env);
  
  static void getFunctionFormalParameters(
    NodeCollection& params,
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganEnvironmentTable* makeEnvironmentTable(
    const NodeCollection& args,
    const NodeCollection& params);
  
  // Top-level evaluators

  static PaganParseNode* eval(PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalParenExpression(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalExpressionBinary(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalVar(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalVariable(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalBlock(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalIf(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalWhile(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalFunction(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalStruct(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalFunctionCall(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalDotAccess(
    PaganParseNode* pt, PaganEnvironment* env);
  
//  static PaganParseNode* evalDotAccessEnvironment(
//    PaganParseNode* pt, PaganEnvironment* env);
  
  // A special version of evalDotAccess for handling assignment.
  // It's obviously inelegant, but this is the easiest way to skirt the right-
  // associativity issues caused by sequential dot accesses.
  
  static PaganParseNode* evalDotAccessAssignment(
    PaganParseNode* pt, PaganEnvironment* env,
    PaganParseNode* assignValue);
    
  // Unary operators
  
  static PaganParseNode* evalUplus(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalUminus(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalUexclamationMark(
    PaganParseNode* pt, PaganEnvironment* env);
    
  // Binary operators
  
  static PaganParseNode* evalPlus(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalMinus(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalStar(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalForwardSlash(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalEqualsEquals(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalNotEquals(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalEquals(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalLeftAngleBracket(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalRightAngleBracket(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalLeftAngleBracketEquals(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalRightAngleBracketEquals(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalAmpersand(
    PaganParseNode* pt, PaganEnvironment* env);
  
  static PaganParseNode* evalPipe(
    PaganParseNode* pt, PaganEnvironment* env);
  
  // Built-in function helpers and evaluators
  
  static void addBuiltIn(PaganEnvironment& environment,
                         std::string name,
                         void* evalFunctionPointer);
                         
  static void printNodeStandard(PaganParseNode* node,
                                std::ostream& ofs = std::cout);
                         
  static PaganParseNode* evalPrint(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalPrintLn(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalPrettyPrint(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalArray(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalGetArray(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalSetArray(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalDeleteArray(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
//  static PaganParseNode* evalSwap(
//    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
//  static PaganParseNode* evalCopy(
//    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalError(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalReadStdInput(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalStringLength(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
                         
  static PaganParseNode* evalGetStringCharacter(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env);
  
};


}


#endif
