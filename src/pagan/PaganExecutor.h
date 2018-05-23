#ifndef PAGANEXECUTOR_H
#define PAGANEXECUTOR_H


#include "pagan/PaganInputStream.h"

namespace Pagan {


/**
 * Primary class for executing Pagan programs.
 */
class PaganExecutor {
public:
  /**
   * Reads, interprets, and executes the Pagan program in the inputStream.
   */
  static void runProgram(PaganInputStream& inputStream);
protected:
};


}


#endif
