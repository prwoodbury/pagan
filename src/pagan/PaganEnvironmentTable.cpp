#include "pagan/PaganEnvironmentTable.h"
#include <cstddef>

using namespace std;

namespace Pagan {


PaganEnvironmentTable::PaganEnvironmentTable() { }
  
PaganEnvironmentTable
    ::PaganEnvironmentTable(const PaganEnvironmentTable& table) {
  for (PaganEnvironmentVariableCollection::const_iterator it
          = table.variables_.cbegin();
       it != table.variables_.cend();
       ++it) {
    variables_.push_back(new PaganEnvironmentVariable(*(*it)));
  }
}
  
PaganEnvironmentTable::~PaganEnvironmentTable() {
  for (PaganEnvironmentVariableCollection::iterator it = variables_.begin();
       it != variables_.end();
       ++it) {
    delete *it;
  }
}

void PaganEnvironmentTable::addVariable(PaganEnvironmentVariable* variable) {
  variables_.push_front(variable);
}

void PaganEnvironmentTable::addVariable(
    PaganEnvironmentVariable::PaganVariableIdentifier variableName,
    PaganParseNode& variableValue) {
  addVariable(new PaganEnvironmentVariable(variableName, variableValue));
}

PaganEnvironmentVariable* PaganEnvironmentTable::getVariable(
    PaganEnvironmentVariable::PaganVariableIdentifier variableName) {
  // Do a linear search for the given variable.
  // More recent variables are at the front of the list, so they shadow
  // older definitions.
  for (PaganEnvironmentVariableCollection::iterator it = variables_.begin();
       it != variables_.end();
       ++it) {
    if (variableName.compare((*it)->name()) == 0) {
      return (*it);
    }
  }
  
  // Variable not found
  return NULL;
}

void PaganEnvironmentTable::display(std::ostream& ofs) const {
  ofs << "********Environment start********" << endl;
  for (PaganEnvironmentVariableCollection::const_iterator it
          = variables_.cbegin();
       it != variables_.cend();
       ++it) {
    ofs << "[" << (*it)->name() << "]" << endl
         << "[" << *((*it)->value()) << "]" << endl;
  }
  ofs << "*********Environment end*********" << endl;
}


} 
