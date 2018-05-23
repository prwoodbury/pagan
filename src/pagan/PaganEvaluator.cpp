#include "pagan/PaganEvaluator.h"
#include "exception/GenericException.h"
#include "exception/EnvironmentLookupException.h"
#include "util/StringConversion.h"
#include <string>

using namespace std;

namespace Pagan {


void PaganEvaluator::evaluateProgram(PaganParseNode& parseTree) {
  // Create global environment
  PaganEnvironment* globalEnv = new PaganEnvironment();
  
  // Add built-ins
  
  addBuiltIn(*globalEnv, "print", (void*)(&evalPrint));
  addBuiltIn(*globalEnv, "println", (void*)(&evalPrintLn));
  addBuiltIn(*globalEnv, "prettyPrint", (void*)(&evalPrettyPrint));
  addBuiltIn(*globalEnv, "Array", (void*)(&evalArray));
  addBuiltIn(*globalEnv, "arrayGet", (void*)(&evalGetArray));
  addBuiltIn(*globalEnv, "arraySet", (void*)(&evalSetArray));
  addBuiltIn(*globalEnv, "arrayDelete", (void*)(&evalDeleteArray));
//  addBuiltIn(*globalEnv, "swap", (void*)(&evalSwap));
//  addBuiltIn(*globalEnv, "copy", (void*)(&evalCopy));
  addBuiltIn(*globalEnv, "error", (void*)(&evalError));
  addBuiltIn(*globalEnv, "readStdInput", (void*)(&evalReadStdInput));
  addBuiltIn(*globalEnv, "stringLength", (void*)(&evalStringLength));
  addBuiltIn(
    *globalEnv, "getStringCharacter", (void*)(&evalGetStringCharacter));
  
  // Evaluate program
  PaganParseNode* result = eval(&parseTree, globalEnv);
  
//  cout << *result << endl;
  
  delete result;
  delete globalEnv;
}
  
void PaganEvaluator::addBuiltIn(PaganEnvironment& environment,
                       std::string name,
                       void* evalFunctionPointer) {
  PaganParseNode* node
    = new PaganParseNode("BUILTIN", 0);
  node->setFvp(evalFunctionPointer);
  environment.insert(name, *node);
}
  
int PaganEvaluator::asInt(PaganParseNode* node) {
  if (node->isType("INTEGER")) {
    return node->intValue();
  }
  else if (node->isType("STRING")) {
    return StringConversion::fromString<int>(node->stringValue());
  }
  else {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::eval("
                           "PaganParseNode*, PaganEnvironment*)",
                           std::string("Line ")
                           + StringConversion::toString(node->lineNum())
                           + ": "
                           + "Cannot coerce "
                           + node->type()
                           + " to int)");
  }
}

PaganString PaganEvaluator::asString(PaganParseNode* node) {
  if (node->isType("INTEGER")) {
    return StringConversion::toString(node->intValue());
  }
  else if (node->isType("STRING")) {
    return node->stringValue();
  }
  else {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::eval("
                           "PaganParseNode*, PaganEnvironment*)",
                           std::string("Line ")
                           + StringConversion::toString(node->lineNum())
                           + ": "
                           + "Cannot coerce "
                           + node->type()
                           + " to string)");
  }
}
  
void PaganEvaluator::unaryOperatorException(
    PaganParseNode* right, PaganParseNode* op) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::unaryOperatorException("
                           "PaganParseNode*, PaganParseNode*,"
                           "PaganParseNode*)",
                           std::string("Line ")
                           + StringConversion::toString(right->lineNum())
                           + ": "
                           + "Invalid operand to unary operator "
                           + op->type()
                           + ": "
                           + right->type());
}
  
void PaganEvaluator::binaryOperatorException(
    PaganParseNode* left, PaganParseNode* right, PaganParseNode* op) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::binaryOperatorException("
                           "PaganParseNode*, PaganParseNode*,"
                           "PaganParseNode*)",
                           std::string("Line ")
                           + StringConversion::toString(left->lineNum())
                           + ": "
                           + "Invalid operands to operator "
                           + op->type()
                           + ": "
                           + left->type()
                           + " and "
                           + right->type());
}

PaganParseNode* PaganEvaluator::eval(
    PaganParseNode* pt, PaganEnvironment* env) {
    
  if (pt->isType("PROGRAM")) {
    return eval(pt->right(), env);
  }
  else if (pt->isType("STATEMENTS")) {
    delete eval(pt->left(), env);
    // return the value of the last statement
    return eval(pt->right(), env);
  }
  else if (pt->isType("STATEMENT")) {
    return eval(pt->right(), env);
  }
  else if (pt->isType("PAREN_EXPRESSION")) {
    return evalParenExpression(pt, env);
  }
  else if (pt->isType("EXPRESSION_BINARY")) {
    return evalExpressionBinary(pt, env);
  }
  else if (pt->isType("UPLUS")) {
    return evalUplus(pt, env);
  }
  else if (pt->isType("UMINUS")) {
    return evalUminus(pt, env);
  }
  else if (pt->isType("UEXCLAMATION_MARK")) {
    return evalUexclamationMark(pt, env);
  }
  else if (pt->isType("VAR")) {
    return evalVar(pt, env);
  }
  else if (pt->isType("VARIABLE")) {
    return evalVariable(pt, env);
  }
  else if (pt->isType("BLOCK")) {
    return evalBlock(pt, env);
  }
  else if (pt->isType("IF")) {
    return evalIf(pt, env);
  }
  else if (pt->isType("WHILE")) {
    return evalWhile(pt, env);
  }
  else if (pt->isType("FUNCTION")) {
    return evalFunction(pt, env);
  }
  else if (pt->isType("STRUCT")) {
    return evalStruct(pt, env);
  }
  else if (pt->isType("FUNCTION_CALL")) {
    return evalFunctionCall(pt, env);
  }
  else if (pt->isType("DOT_ACCESS")) {
    return evalDotAccess(pt, env);
  }
  else if (pt->isType("INTEGER")) {
    return new PaganParseNode(*pt);
  }
  else if (pt->isType("STRING")) {
    return new PaganParseNode(*pt);
  }
  else if (pt->isType("ARRAY")) {
    return new PaganParseNode(*pt);
  }
  else {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::eval("
                           "PaganParseNode*, PaganEnvironment*)",
                           std::string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Malformed expression (type: "
                           + pt->type()
                           + ")");
  }
}
  
PaganParseNode* PaganEvaluator::evalExpressionBinary(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* op = pt->left();
  
  if (op->isType("PLUS")) {
    return evalPlus(pt, env);
  }
  else if (op->isType("MINUS")) {
    return evalMinus(pt, env);
  }
  else if (op->isType("STAR")) {
    return evalStar(pt, env);
  }
  else if (op->isType("FORWARD_SLASH")) {
    return evalForwardSlash(pt, env);
  }
  else if (op->isType("EQUALSEQUALS")) {
    return evalEqualsEquals(pt, env);
  }
  else if (op->isType("NOT_EQUALS")) {
    return evalNotEquals(pt, env);
  }
  else if (op->isType("EQUALS")) {
    return evalEquals(pt, env);
  }
  else if (op->isType("LEFT_ANGLEBRACKET")) {
    return evalLeftAngleBracket(pt, env);
  }
  else if (op->isType("RIGHT_ANGLEBRACKET")) {
    return evalRightAngleBracket(pt, env);
  }
  else if (op->isType("LEFT_ANGLEBRACKETEQUALS")) {
    return evalLeftAngleBracketEquals(pt, env);
  }
  else if (op->isType("RIGHT_ANGLEBRACKETEQUALS")) {
    return evalRightAngleBracketEquals(pt, env);
  }
  else if (op->isType("AMPERSAND")) {
    return evalAmpersand(pt, env);
  }
  else if (op->isType("PIPE")) {
    return evalPipe(pt, env);
  }
  else {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalExpressionBinary("
                           "PaganParseNode*, PaganEnvironment*)",
                           std::string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Invalid binary operator (type: "
                           + pt->type()
                           + ")");
  }
}
  
PaganParseNode* PaganEvaluator::evalParenExpression(
    PaganParseNode* pt, PaganEnvironment* env) {
  return eval(pt->right(), env);
}
  
PaganParseNode* PaganEvaluator::evalVar(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* value = NULL;
  
  // user-assigned initial value
  if (pt->right() != NULL) {
    value = eval(pt->right(), env);
  }
  // default initialization
  else {
    value = new PaganParseNode("INTEGER", pt->lineNum());
  }
  
  // add to environment
  env->insert(pt->left()->stringValue(), *value);
  
  return value;
}
  
PaganParseNode* PaganEvaluator::evalVariable(
    PaganParseNode* pt, PaganEnvironment* env) {
  try {
    PaganParseNode* result = 
      new PaganParseNode(*(env->lookup(pt->stringValue())));
    
    return result;
  }
  catch (EnvironmentLookupException& e) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalVariable("
                           "PaganParseNode*, PaganEnvironment*)",
                           std::string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Variable '"
                           + e.variableName()
                           + "' not found in current environment");
  }
}
  
PaganParseNode* PaganEvaluator::evalBlock(
    PaganParseNode* pt, PaganEnvironment* env) {
  if (pt->right() != NULL) {
    return eval(pt->right(), env);
  }
  else {
    return new PaganParseNode("INTEGER", pt->lineNum());
  }
}
  
PaganParseNode* PaganEvaluator::evalIf(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* result = NULL;
  
  PaganParseNode* condition = eval(pt->left(), env);
  
  // evaluate body
  if (asInt(condition) != 0) {
    result = eval(pt->right()->left(), env);
  }
  // evaluate else (if one exists)
  else {
    if (pt->right()->right() != NULL) {
      result = eval(pt->right()->right(), env);
    }
  }
    
  delete condition;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalWhile(
    PaganParseNode* pt, PaganEnvironment* env) {
  // default return value is default integer
  PaganParseNode* result
     = new PaganParseNode("INTEGER", pt->lineNum());
  
  PaganParseNode* condition = eval(pt->left(), env);
  
  // evaluate body
  while (asInt(condition) != 0) {
    delete result;
    result = eval(pt->right(), env);
    
    delete condition;
    condition = eval(pt->left(), env);
  }
    
  delete condition;
  
  return result;
}
  
void PaganEvaluator::evaluateFunctionCallArgs(
    NodeCollection& args,
    PaganParseNode* pt, PaganEnvironment* env) {
  while (pt != NULL) {
    PaganParseNode* node = eval(pt->left(), env);
    args.push_back(node);
    
    pt = pt->right();
  }
}
  
void PaganEvaluator::getFunctionFormalParameters(
    NodeCollection& params,
    PaganParseNode* pt, PaganEnvironment* env) {
  while (pt != NULL) {
    PaganParseNode* node = pt->left();
    params.push_back(node);
    
    pt = pt->right();
  }
}
  
PaganEnvironmentTable* PaganEvaluator::makeEnvironmentTable(
    const NodeCollection& args,
    const NodeCollection& params) {
  PaganEnvironmentTable* table = new PaganEnvironmentTable();
  
  NodeCollection::const_iterator it1 = args.cbegin();
  NodeCollection::const_iterator it2 = params.cbegin();
  // lists must be the same size!
  while (it1 != args.cend()) {
    table->addVariable((*it2)->stringValue(), *(*it1));
    ++it1;
    ++it2;
  }
  
  return table;
}
  
PaganParseNode* PaganEvaluator::evalFunction(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* closure
    = new PaganParseNode("CLOSURE", pt->lineNum());
  
  // remember defining environment
  PaganParseNode* envp = new PaganParseNode("ENVIRONMENTP", pt->lineNum());
  envp->setVoidp((void*)env);
  closure->setLeft(envp);
  
  // remember formal parameters and body
  closure->setRight(new PaganParseNode(*(pt->right())));
  
  // If function is not anonymous, add to defining environment
  if (pt->left() != NULL) {
    if (pt->left()->isType("VARIABLE")) {
      env->insert(pt->left()->stringValue(), *closure);
    }
    else {
      throw GenericException(PAGAN_SRCANDLINE,
                             "PaganEvaluator::evalFunction("
                             "PaganParseNode*, PaganEnvironment*)",
                             string("Line ")
                             + StringConversion::toString(pt->lineNum())
                             + ": "
                             + "Function name is not VARIABLE");
    }
  }
  
  return closure;
}
  
PaganParseNode* PaganEvaluator::evalStruct(
    PaganParseNode* pt, PaganEnvironment* env) {
//  PaganParseNode* result
//    = new PaganParseNode("STRUCT_OBJ", pt->lineNum());
    
  // Create a new environment and evaluate the struct body within it.
  // The resulting environment becomes the struct's initial state.
  
  PaganEnvironment* newEnv
    = new PaganEnvironment(*env, NULL);
    
  // Evaluate struct body and throw away result
  delete eval(pt->right(), newEnv);
  
  // Remember evaluated environment
//  PaganParseNode* envp = new PaganParseNode("ENVIRONMENTP", pt->lineNum());
  PaganParseNode* result = new PaganParseNode("ENVIRONMENTP", pt->lineNum());
  result->setVoidp((void*)newEnv);
//  result->setLeft(envp);

  // If struct is not anonymous, add to defining environment
  if (pt->left() != NULL) {
    if (pt->left()->isType("VARIABLE")) {
      env->insert(pt->left()->stringValue(), *result);
    }
    else {
      throw GenericException(PAGAN_SRCANDLINE,
                             "PaganEvaluator::evalStruct("
                             "PaganParseNode*, PaganEnvironment*)",
                             string("Line ")
                             + StringConversion::toString(pt->lineNum())
                             + ": "
                             + "Struct name is not VARIABLE");
    }
  }
  
  return result;
}

PaganParseNode* PaganEvaluator::evalFunctionCall(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* result = NULL;
  PaganParseNode* closure = NULL;
  PaganParseNode* left = pt->left();
  
  // Retrieve the closure
  // Named function
/*  if (left->isType("VARIABLE")) {
//    closure = env->lookup(left->stringValue());
    closure = eval(pt->left(), env);
  }
  // Anonymous function
  else {
//    closure = left;
    closure = eval(pt->left(), env);
  } */
  
  // The left node may be a variable, a function definition,
  // or a function call. In any case, evaluating it should always
  // result in a closure.
  closure = eval(left, env);
  
  // Evaluate function arguments
  NodeCollection args;
  evaluateFunctionCallArgs(args, pt->right(), env);
    
  if (closure->isType("BUILTIN")) {
    // cast the evaluation function pointer to the appropriate type and call it
    return ((*(PaganParseNode* (*)(NodeCollection&,PaganParseNode*,PaganEnvironment*))
           (closure->fvp())))
           (args, pt, env);
  }
  else if (!(closure->isType("CLOSURE"))) {
    throw GenericException(PAGAN_SRCANDLINE,
                         "PaganEvaluator::evalFunctionCall("
                         "PaganParseNode*, PaganEnvironment*)",
                         string("Line ")
                         + StringConversion::toString(pt->lineNum())
                         + ": "
                         + "Attempted to call "
                         + StringConversion::toString(left->type())
                         + " as a function");
  }
  
  // Get formal parameters
  NodeCollection formalParams;
  getFunctionFormalParameters(formalParams, closure->right()->left(), env);
  
  if (args.size() != formalParams.size()) {
    std::string errorMsg;
    errorMsg += string("Line ")
                         + StringConversion::toString(pt->lineNum())
                         + ": "
                         + "Wrong number of arguments to function ";
    if (left->isType("VARIABLE")) {
      errorMsg += left->stringValue()
                    + ": ";
    }
    else {
      errorMsg += "<anonymous>: ";
    }
    errorMsg += "expected "
                         + StringConversion::toString(formalParams.size())
                         + ", got "
                         + StringConversion::toString(args.size());
    throw GenericException(PAGAN_SRCANDLINE,
                         "PaganEvaluator::evalFunctionCall("
                         "PaganParseNode*, PaganEnvironment*)",
                         errorMsg);
  }
  
  // Convert formal parameters and arguments to PaganEnvironmentTable
  PaganEnvironmentTable* environmentTable
    = makeEnvironmentTable(args, formalParams);
  
  // Get closure environment
  PaganEnvironment* definingEnv
    = (PaganEnvironment*)(closure->left()->voidp());
  
  // Extend environment
//  PaganEnvironment extendedEnvironment(
//    *definingEnv, environmentTable);

  //////////////////////////////////////////////
  //  //  //  //  //  //  //  //  //  //  //  //
  //!!!!!!!!!!THIS LEAKS MEMORY!!!!!!!!!!!!!!!//
  //  //  //  //  //  //  //  //  //  //  //  //
  //!!!!!!!!!!THIS LEAKS MEMORY!!!!!!!!!!!!!!!//
  //  //  //  //  //  //  //  //  //  //  //  //
  //!!!!!!!!!!THIS LEAKS MEMORY!!!!!!!!!!!!!!!//
  //  //  //  //  //  //  //  //  //  //  //  //
  //////////////////////////////////////////////
  PaganEnvironment* extendedEnvironmentP = new PaganEnvironment(
    *definingEnv, environmentTable);
  PaganEnvironment& extendedEnvironment = *extendedEnvironmentP;
    
//  extendedEnvironment.display(cerr);
//  cerr << closure->right()->right()->type() << endl;
  
  // Make the call
  result = eval(closure->right()->right(), &extendedEnvironment);
  
  // Destroy everything we don't need anymore
  for (NodeCollection::iterator it = args.begin();
       it != args.end();
       ++it) {
    delete *it;
  }
  for (NodeCollection::iterator it = formalParams.begin();
       it != formalParams.end();
       ++it) {
    delete *it;
  }
//  delete environmentTable;
//  delete left;
  // Does closure need to be deleted here to avoid leaking memory??
//  if (!(left->isType("VARIABLE"))) {
//    delete closure;
//  }
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalDotAccess(
    PaganParseNode* pt, PaganEnvironment* env) {
  
  // for each access:
  // 1. evaluate variable in current environment. It should be an ENVIRONMENTP
  // (if there is a sub-access) or a variable (if it is the innermost access)
  // 2. retrieve environment
  // 3. return evaluation of the next dot access in the retrieved environment
    
  // rightmost access
  if (pt->left() == NULL) {
    // do not evaluate yet!
    PaganParseNode* left = pt->right();
    
    if (!(left->isType("VARIABLE"))) {
      throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalDotAccess("
                           "PaganParseNode*, PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Invalid dot access of "
                           + StringConversion::toString(left->type())
                           + " instead of VARIABLE");
    }
    
    return eval(left, env);
  }
  // intermediate access
  else {
  
    PaganParseNode* left = eval(pt->right(), env);
  
    if (!(left->isType("ENVIRONMENTP"))) {
      throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalDotAccess("
                           "PaganParseNode*, PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Invalid use of dot operator on "
                           + StringConversion::toString(left->type())
                           + " instead of ENVIRONMENTP");
    }
    
    // retrieve environment
    PaganEnvironment* newEnv
      = (PaganEnvironment*)(left->voidp());
    
    // evaluate sub-access in closure environment
    return eval(pt->left(), newEnv);
    
  }
  
}
  
/*PaganParseNode* PaganEvaluator::evalDotAccessEnvironment(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right(), env);

  if (!(left->isType("ENVIRONMENTP"))) {
    throw GenericException(PAGAN_SRCANDLINE,
                         "PaganEvaluator::evalDotAccess("
                         "PaganParseNode*, PaganEnvironment*)",
                         string("Line ")
                         + StringConversion::toString(pt->lineNum())
                         + ": "
                         + "Invalid use of dot operator on "
                         + StringConversion::toString(left->type())
                         + " instead of ENVIRONMENTP");
  }
  
  // retrieve environment
  PaganEnvironment* newEnv
    = (PaganEnvironment*)(left->voidp());
  
  // evaluate sub-access in closure environment
  PaganParseNode* result = eval(pt->left(), newEnv);
  
  if (result->left() == NULL) {
    return result;
  }
  else {
    return evalDotAccessEnvironment(result->left(), newEnv);
  }
} */
  
PaganParseNode* PaganEvaluator::evalDotAccessAssignment(
    PaganParseNode* pt, PaganEnvironment* env,
    PaganParseNode* assignValue) {
    
  // rightmost access
  if (pt->left() == NULL) {
    // do not evaluate yet!
    PaganParseNode* left = pt->right();
    
    if (!(left->isType("VARIABLE"))) {
      throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalDotAccess("
                           "PaganParseNode*, PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Invalid dot access of "
                           + StringConversion::toString(left->type())
                           + " instead of VARIABLE");
    }
    
    env->update(left->stringValue(),
                *assignValue);
    return new PaganParseNode(*assignValue);
  }
  // intermediate access
  else {
  
    PaganParseNode* left = eval(pt->right(), env);
  
    if (!(left->isType("ENVIRONMENTP"))) {
      throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalDotAccess("
                           "PaganParseNode*, PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Invalid use of dot operator on "
                           + StringConversion::toString(left->type())
                           + " instead of ENVIRONMENTP");
    }
    
    // retrieve environment
    PaganEnvironment* newEnv
      = (PaganEnvironment*)(left->voidp());
    
    // evaluate sub-access in environment
    return evalDotAccessAssignment(pt->left(), newEnv, assignValue);
    
  }
  
/*  PaganParseNode* left = eval(pt->right(), env);
    
  // rightmost access
  if (pt->left() == NULL) {
    if (!(left->isType("VARIABLE"))) {
      throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalDotAccessAssignment("
                           "PaganParseNode*, PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Invalid dot access assignment of "
                           + StringConversion::toString(left->type())
                           + " instead of VARIABLE");
    }
    
    env->update(left->stringValue(),
                *assignValue);
    return new PaganParseNode(*assignValue);
  }
  // intermediate access
  else {
  
    if (!(left->isType("CLOSURE"))) {
      throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalDotAccessAssignment("
                           "PaganParseNode*, PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Invalid use of dot operator assignment on "
                           + StringConversion::toString(left->type())
                           + " instead of CLOSURE");
    }
    
    // retrieve closure's true environment (not defining environment!)
    PaganEnvironment* closureEnv
      = (PaganEnvironment*)(left->left()->envp());
    
    // evaluate sub-access in closure environment
    return eval(pt->left(), closureEnv);
    
//    delete closure;
    
  } */
  
              
              
}

PaganParseNode* PaganEvaluator::evalPlus(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // addition
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue(leftNative + rightNative);
  }
  // concatenation
  else if (left->isType("STRING")) {
    PaganString leftNative = asString(left);
    PaganString rightNative = asString(right);
    result = new PaganParseNode("STRING", left->lineNum());
    result->setStringValue(leftNative + rightNative);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}

PaganParseNode* PaganEvaluator::evalMinus(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // subtraction
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue(leftNative - rightNative);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalStar(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // multiplication
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue(leftNative * rightNative);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalForwardSlash(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // division
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    
    if (rightNative == 0) {
      throw GenericException(PAGAN_SRCANDLINE,
                             "PaganEvaluator::evalForwardSlash("
                             "PaganParseNode*, PaganEnvironment*)",
                             std::string("Line ")
                             + StringConversion::toString(pt->lineNum())
                             + ": "
                             + "Division by zero");
    }
    
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue(leftNative / rightNative);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalEqualsEquals(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // int equality
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative == rightNative)
                          ? 1
                          : 0);
  }
  // string equality
  else if (left->isType("STRING")) {
    PaganString leftNative = asString(left);
    PaganString rightNative = asString(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative.compare(rightNative) == 0)
                          ? 1
                          : 0);
  }
  // object equality
  else if (left->isType("ENVIRONMENTP")) {
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((left->voidp() == right->voidp())
                          ? 1
                          : 0);
  }
  else {
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue(0);
//    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalNotEquals(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // int equality
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative != rightNative)
                          ? 1
                          : 0);
  }
  // string equality
  else if (left->isType("STRING")) {
    PaganString leftNative = asString(left);
    PaganString rightNative = asString(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative.compare(rightNative) != 0)
                          ? 1
                          : 0);
  }
  else {
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue(1);
//    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalEquals(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = pt->right()->left();
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // simple variable assignment
  if (left->isType("VARIABLE")) {
    result = new PaganParseNode(*right);
    env->update(left->stringValue(),
                *result);
  }
  // dot access assignment (evaluation required)
  else if (left->isType("DOT_ACCESS")) {
    result = evalDotAccessAssignment(left, env, right);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
//  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalLeftAngleBracket(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // int equality
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative < rightNative)
                          ? 1
                          : 0);
  }
  // string equality
  else if (left->isType("STRING")) {
    PaganString leftNative = asString(left);
    PaganString rightNative = asString(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative < rightNative)
                          ? 1
                          : 0);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalRightAngleBracket(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // int equality
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative > rightNative)
                          ? 1
                          : 0);
  }
  // string equality
  else if (left->isType("STRING")) {
    PaganString leftNative = asString(left);
    PaganString rightNative = asString(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative > rightNative)
                          ? 1
                          : 0);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalLeftAngleBracketEquals(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // int equality
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative <= rightNative)
                          ? 1
                          : 0);
  }
  // string equality
  else if (left->isType("STRING")) {
    PaganString leftNative = asString(left);
    PaganString rightNative = asString(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative <= rightNative)
                          ? 1
                          : 0);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalRightAngleBracketEquals(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  // int equality
  if (left->isType("INTEGER")) {
    PaganInt leftNative = asInt(left);
    PaganInt rightNative = asInt(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative >= rightNative)
                          ? 1
                          : 0);
  }
  // string equality
  else if (left->isType("STRING")) {
    PaganString leftNative = asString(left);
    PaganString rightNative = asString(right);
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue((leftNative >= rightNative)
                          ? 1
                          : 0);
  }
  else {
    binaryOperatorException(left, right, pt->left());
  }
  
  delete left;
  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalAmpersand(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
//  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  PaganInt resultNative = asInt(left);
//  cout << resultNative << endl;
  
  // short-circuit
  if (resultNative == 0) {
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue(0);
  }
  else {
    result = eval(pt->right()->right(), env);
  }
  
//  cout << result->type() << " " << result->intValue() << endl;
  
  delete left;
//  delete right;
  
  return result;
}

PaganParseNode* PaganEvaluator::evalPipe(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* left = eval(pt->right()->left(), env);
//  PaganParseNode* right = eval(pt->right()->right(), env);
  
  PaganParseNode* result = NULL;
  
  PaganInt resultNative = asInt(left);
  
  // short-circuit
  if (resultNative != 0) {
    result = new PaganParseNode("INTEGER", left->lineNum());
    result->setIntValue(1);
  }
  else {
    result = eval(pt->right()->right(), env);
  }
  
  delete left;
//  delete right;
  
  return result;
}
  
PaganParseNode* PaganEvaluator::evalUplus(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* right = eval(pt->right(), env);
  
  PaganParseNode* result = NULL;
  
  if (right->isType("INTEGER")) {
    result = new PaganParseNode(*right);
    result->setIntValue(+(result->intValue()));
  }
  else {
    unaryOperatorException(right, pt);
  }
  
  delete right;
  
  return result;
}

PaganParseNode* PaganEvaluator::evalUminus(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* right = eval(pt->right(), env);
  
  PaganParseNode* result = NULL;
  
  if (right->isType("INTEGER")) {
    result = new PaganParseNode(*right);
    result->setIntValue(-(result->intValue()));
  }
  else {
    unaryOperatorException(right, pt);
  }
  
  delete right;
  
  return result;
}

PaganParseNode* PaganEvaluator::evalUexclamationMark(
    PaganParseNode* pt, PaganEnvironment* env) {
  PaganParseNode* right = eval(pt->right(), env);
  
  PaganParseNode* result = NULL;
  
  if (right->isType("INTEGER")) {
    result = new PaganParseNode(*right);
    result->setIntValue((result->intValue() == 0)
                          ? 1
                          : 0);
  }
  else {
    unaryOperatorException(right, pt);
  }
  
  delete right;
  
  return result;
}
                         
void PaganEvaluator::printNodeStandard(
    PaganParseNode* node, std::ostream& ofs) {
  if (node->isType("STRING")) {
    ofs << node->stringValue();
  }
  else if (node->isType("INTEGER")) {
    ofs << asString(node);
  }
  else {
    node->prettyPrint(ofs);
  }
}
                         
PaganParseNode* PaganEvaluator::evalPrint(
    NodeCollection& args, 
    PaganParseNode* pt,
    PaganEnvironment* env) {
  
  for (NodeCollection::iterator it = args.begin();
       it != args.end();
       ++it) {
//    cout << asString((*it));
    printNodeStandard(*it);
  }
  
  return NULL;
}
                         
PaganParseNode* PaganEvaluator::evalPrintLn(
    NodeCollection& args, 
    PaganParseNode* pt,
    PaganEnvironment* env) {
  
  evalPrint(args, pt, env);
  cout << endl;
  
  return NULL;
}
                         
PaganParseNode* PaganEvaluator::evalPrettyPrint(
    NodeCollection& args, 
    PaganParseNode* pt,
    PaganEnvironment* env) {
  
  for (NodeCollection::iterator it = args.begin();
       it != args.end();
       ++it) {
//    cout << asString((*it));
    (*it)->prettyPrint(cout);
  }
  
  return NULL;
}
                         
PaganParseNode* PaganEvaluator::evalArray(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 1) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalArray("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to Array() "
                           + "(should be 1)");
  }
  
  PaganParseNode* numElements = *(args.begin());
  int numElementsNative = asInt(numElements);
  
  PaganParseNode* result
    = new PaganParseNode("ARRAY", pt->lineNum());
  
  // create NULL-initialized pointer array
  PaganParseNode** elements
    = new PaganParseNode*[numElementsNative];
  for (int i = 0; i < numElementsNative; i++) {
    elements[i] = NULL;
  }
  
  // place array in fvp
  result->setFvp((void*)elements);
  
  // create extended environment
//  PaganEnvironment* extEnv = new PaganEnvironment(*env, NULL);
  
  // add built-ins to extended environment
//  addBuiltIn(*extEnv, "get", (void*)(&evalArrayGet));
//  addBuiltIn(*extEnv, "set", (void*)(&evalArraySet));
  
  // add a false environment entry pointing back to the ENVIRONMENTP node
  // so we can retrieve it later
//  extEnv->insert("_ARRAY", *result);
  
  // place extended environment in voidp
//  result->setVoidp((void*)(extEnv));
  
  return result;
}
                         
/*PaganParseNode* PaganEvaluator::evalArrayGet(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 1) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalArrayGet("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to Array.get() "
                           + "(should be 1)");
  }
  
  PaganParseNode* elementNum = *(args.begin());
  
  // retrieve the array object
  PaganParseNode* arrayNode = env->lookup("_ARRAY");
  
  PaganParseNode** elements
    = (PaganParseNode**)(arrayNode->fvp());
    
  return elements[asInt(elementNum)];
}
                         
PaganParseNode* PaganEvaluator::evalArraySet(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 2) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalArraySet("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to Array.set() "
                           + "(should be 2)");
  }
  
  
  
  PaganParseNode* elementNum = *(args.begin());
  int elementNumNative = asInt(elementNum);
  PaganParseNode* value = *(++(args.begin()));
  
  // retrieve the array object
//  PaganParseNode* left = eval(pt->left(), env);
//  env->display(cerr);
  PaganParseNode* test = evalDotAccessEnvironment(pt->left(), env);
  PaganParseNode* arrayNode = env->lookup("_ARRAY");
  
  PaganParseNode** elements
    = (PaganParseNode**)(arrayNode->fvp());
    
  // destroy existing content before replacing
  delete elements[elementNumNative];
  elements[elementNumNative] = new PaganParseNode(*value);
    
  return elements[elementNumNative];
} */
                         
PaganParseNode* PaganEvaluator::evalDeleteArray(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 1) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalArray("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to deleteArray() "
                           + "(should be 1)");
  }
  
  PaganParseNode* arrayNode = *(args.begin());
  
  if (!(arrayNode->isType("ARRAY"))) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalDeleteArray("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Argument to deleteArray() is not ARRAY");
  }
  
  delete[] (PaganParseNode*)(arrayNode->voidp());
  
  return NULL;
}
                         
PaganParseNode* PaganEvaluator::evalGetArray(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 2) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalGetArray("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to arrayGet() "
                           + "(should be 2)");
  }
  
  PaganParseNode* arrayNode = *(args.begin());
  PaganParseNode* elementNum = *(++(args.begin()));
  
  if (!(arrayNode->isType("ARRAY"))) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalAccessArray("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Argument to deleteArray() is not ARRAY");
  }
  
  PaganParseNode** elements
    = (PaganParseNode**)(arrayNode->fvp());
  
  return elements[asInt(elementNum)];
}
                         
PaganParseNode* PaganEvaluator::evalSetArray(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 3) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalGetArray("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to arraySet() "
                           + "(should be 3)");
  }
  
  PaganParseNode* arrayNode = *(args.begin());
  PaganParseNode* elementNum = *(++(args.begin()));
  PaganParseNode* elementVal = *(++(++(args.begin())));
  
  if (!(arrayNode->isType("ARRAY"))) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalAccessArray("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Argument to deleteArray() is not ARRAY");
  }
  
  PaganParseNode** elements
    = (PaganParseNode**)(arrayNode->fvp());
  
  int elementNumNative = asInt(elementNum);
  
  delete elements[elementNumNative];
  elements[elementNumNative] = new PaganParseNode(*elementVal);
  
  return NULL;
}
                         
/*PaganParseNode* PaganEvaluator::evalSwap(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 2) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalSwap("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to swap() "
                           + "(should be 2)");
  }
  
  PaganParseNode* first = *(args.begin());
  PaganParseNode* second = *(++(args.begin()));
  
  cout << first->type() << endl;
  cout << second->type() << endl;
  
  PaganParseNode temp = *first;
  *first = *second;
  *second = temp;
  
  return NULL;
}
                         
PaganParseNode* PaganEvaluator::evalCopy(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 1) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalCopy("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to copy() "
                           + "(should be 1)");
  }
  
  PaganParseNode* src = *(args.begin());
  
  cout << src->type() << endl;
//  cout << second->type() << endl;
  
  return new PaganParseNode(*src);
} */
                         
PaganParseNode* PaganEvaluator::evalError(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  
  cerr << "***ERROR***";
  cerr << endl;
  cerr << "*** Line: " << pt->lineNum();
  cerr << endl;
  cerr << "*** ";
  
  for (NodeCollection::iterator it = args.begin();
       it != args.end();
       ++it) {
//    cout << asString((*it));
    printNodeStandard(*it, cerr);
  }
  
  cerr << endl;
  
  throw GenericException(PAGAN_SRCANDLINE,
                         "PaganEvaluator::evalError("
                         "NodeCollection&,PaganParseNode*,"
                         "PaganEnvironment*)",
                         string("Line ")
                         + StringConversion::toString(pt->lineNum())
                         + ": "
                         + "User-generated error triggered");
  
  return NULL;
}
                         
PaganParseNode* PaganEvaluator::evalReadStdInput(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 0) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalReadStdInput("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to readStdInput() "
                           + "(should be 0)");
  }
  
  string str;
  cin >> str;
  
  PaganParseNode* result = new PaganParseNode("STRING", pt->lineNum());
  result->setStringValue(str);
  
  return result;
}
                         
PaganParseNode* PaganEvaluator::evalStringLength(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 1) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalStringLength("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to evalStringLength() "
                           + "(should be 1)");
  }
  
  PaganParseNode* stringNode = *(args.begin());
  
  if (!(stringNode->isType("STRING"))) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalStringLength("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong argument type to evalStringLength() "
                           + "(should be STRING)");
  }
  
  PaganParseNode* result = new PaganParseNode("INTEGER", pt->lineNum());
  result->setIntValue(stringNode->stringValue().size());
  
  return result;
}
                         
PaganParseNode* PaganEvaluator::evalGetStringCharacter(
    NodeCollection& args, PaganParseNode* pt, PaganEnvironment* env) {
  if (args.size() != 2) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalGetStringCharacter("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong number of arguments to "
                           + "evalGetStringCharacter() "
                           + "(should be 2)");
  }
  
  PaganParseNode* stringNode = *(args.begin());
  PaganParseNode* intNode = *(++(args.begin()));
  
  if (!(stringNode->isType("STRING"))) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalStringLength("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Wrong argument type to evalStringLength() "
                           + "(should be STRING)");
  }
  
  PaganParseNode* result = new PaganParseNode("STRING", pt->lineNum());
  string str = stringNode->stringValue();
  int pos = intNode->intValue();
  if ((unsigned int)pos >= str.size()) {
    throw GenericException(PAGAN_SRCANDLINE,
                           "PaganEvaluator::evalStringLength("
                           "NodeCollection&,PaganParseNode*,"
                           "PaganEnvironment*)",
                           string("Line ")
                           + StringConversion::toString(pt->lineNum())
                           + ": "
                           + "Out-of-range string access");
  }
  
  result->setStringValue(string() + str[pos]);
  
  return result;
}


} 
