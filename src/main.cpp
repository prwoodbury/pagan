#include "pagan/PaganStdInputStream.h"
#include "pagan/PaganExecutor.h"
#include "exception/PaganException.h"
#include <iostream>
#include <fstream>

using namespace Pagan;
using namespace std;

int main(int argc, char* argv[]) {

  try {
  
  // If an input file has been specified on the command line, read from it
  if (argc > 1) {
    ifstream ifs(argv[1]);
    PaganStdInputStream inputStream(ifs);
    PaganExecutor::runProgram(inputStream);
  }
  // Otherwise, read from stdin
  else {
    PaganStdInputStream inputStream(std::cin);
    PaganExecutor::runProgram(inputStream);
  }
  
  return 0;
  
  }
  catch (PaganException& e) {
    cerr << "***Pagan exception***" << endl;
    cerr << e.what() << endl;
  }
  catch (std::exception& e) {
    cerr << "Unhandled std::exception: " << e.what() << endl;
  }
  catch (...) {
    cerr << "Unknown exception, aborting" << endl;
  }
} 
