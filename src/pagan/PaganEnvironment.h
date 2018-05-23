#ifndef PAGANENVIRONMENT_H
#define PAGANENVIRONMENT_H


#include "pagan/PaganEnvironmentTable.h"
#include <list>
#include <iostream>

namespace Pagan {


/**
 * An environment containing Pagan variable tables.
 */
class PaganEnvironment {
public:
  PaganEnvironment();
  
  /**
   * Extension constructor.
   * Constructs a new environment with an added table.
   */
  PaganEnvironment(PaganEnvironment& env,
                   PaganEnvironmentTable* extension);
  
  /**
   * Places a new variable in the most local table of the environment.
   */
  void insert(PaganEnvironmentVariable::PaganVariableIdentifier name,
              PaganParseNode& value);
  
  /**
   * Finds a the value of a variable in the environment.
   * Throws if the variable is not found.
   */
  PaganParseNode*
      lookup(PaganEnvironmentVariable::PaganVariableIdentifier name);
  
  /**
   * Changes the value of the most local occurence of a variable.
   * Throws if the variable is not found.
   */
  void update(PaganEnvironmentVariable::PaganVariableIdentifier name,
              PaganParseNode& value);
  
  /**
   * Returns a new environment with an added table.
   */
  PaganEnvironment* extend(PaganEnvironmentTable* table);
  
  /**
   * Environment printer.
   */
  void display(std::ostream& ofs);
  
  /**
   * Localmost environment printer.
   */
  void displayLocal(std::ostream& ofs);
  
protected:

  /**
   * Helper for find environment variables.
   */
  PaganEnvironmentVariable*
      lookupVariable(PaganEnvironmentVariable::PaganVariableIdentifier name);
  
  typedef std::list<PaganEnvironmentTable*>
    PaganEnvironmentTableCollection;
  
  PaganEnvironmentTableCollection tables_;
  
};


}


#endif
