#ifndef PAGANENVIRONMENTTABLE_H
#define PAGANENVIRONMENTTABLE_H


#include "pagan/PaganEnvironmentVariable.h"
#include <list>
#include <iostream>

namespace Pagan {


/**
 * A list of PaganEnvironmentVariables in an environment.
 */
class PaganEnvironmentTable {
public:
  PaganEnvironmentTable();
  
  PaganEnvironmentTable(const PaganEnvironmentTable& table);
  
  ~PaganEnvironmentTable();
  
  /**
   * Alternate interface for addVariable separating name and value fields.
   */
  void addVariable(
      PaganEnvironmentVariable::PaganVariableIdentifier variableName,
      PaganParseNode& variableValue);
  
  /**
   * Looks up a variable from the table.
   * The most recently defined variable by the given name will be returned.
   * Returns NULL if no variable by the given name exists in the table.
   */
  PaganEnvironmentVariable* getVariable(
    PaganEnvironmentVariable::PaganVariableIdentifier variableName);
    
  /**
   * Printer.
   */
  void display(std::ostream& ofs) const;
  
protected:
  
  typedef std::list<PaganEnvironmentVariable*>
    PaganEnvironmentVariableCollection;
  
  /**
   * Adds a new variable to the table.
   * The new variable will shadow any existing variable by the same name.
   */
  void addVariable(PaganEnvironmentVariable* variable);
  
  /**
   * The environment variable list.
   */
  PaganEnvironmentVariableCollection variables_;
  
};


}


#endif
