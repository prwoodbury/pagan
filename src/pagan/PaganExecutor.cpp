#include "pagan/PaganExecutor.h"
#include "pagan/PaganLexer.h"
#include "pagan/PaganParser.h"
#include "pagan/PaganEvaluator.h"
#include <iostream>

using namespace std;

namespace Pagan {


void PaganExecutor::runProgram(PaganInputStream& inputStream) {
//  PaganLexer lexer;
  
  PaganLexicalStream lexicalStream;
  PaganLexer::lexProgram(lexicalStream, inputStream);
  
//  while (!lexicalStream.eof()) {
//    cout << lexicalStream.get() << endl;
//  }

  PaganParser parser;
  parser.parseProgram(lexicalStream);
  
  PaganParseNode& parseTree = parser.parseTree();
//  parseTree.prettyPrint(cout);
//  cout << parseTree << endl;
//  cout << parseTree.left() << " " << parseTree.right() << endl;

  PaganEvaluator::evaluateProgram(parseTree);
  
}


}
