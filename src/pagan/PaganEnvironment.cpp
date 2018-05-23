#include "pagan/PaganEnvironment.h"
#include "exception/GenericException.h"
#include "exception/EnvironmentLookupException.h"
#include <string>
#include <cstddef>

using namespace std;

namespace Pagan {


PaganEnvironment::PaganEnvironment() {
  // Add empty initial environment table
  tables_.push_front(new PaganEnvironmentTable());
}

PaganEnvironment::PaganEnvironment(PaganEnvironment& env,
                 PaganEnvironmentTable* extension) {
  tables_ = env.tables_;
  if (extension != NULL) {
    tables_.push_front(extension);
  }
  else {
    tables_.push_front(new PaganEnvironmentTable());
  }
}

void PaganEnvironment::insert(
            PaganEnvironmentVariable::PaganVariableIdentifier name,
            PaganParseNode& value) {
  tables_.front()->addVariable(name, value);
}

PaganParseNode* PaganEnvironment::lookup(
    PaganEnvironmentVariable::PaganVariableIdentifier name) {
  PaganEnvironmentVariable* variable = lookupVariable(name);
  return variable->value();
}

void PaganEnvironment::update(
              PaganEnvironmentVariable::PaganVariableIdentifier name,
              PaganParseNode& value) {
  PaganEnvironmentVariable* variable = lookupVariable(name);
  variable->setValue(value);
}

/*PaganEnvironment* PaganEnvironment::extend(PaganEnvironmentTable* table) {
  PaganEnvironment* newEnvironment
    = new PaganEnvironment();
  if (table == NULL) {
    
  }
} */

void PaganEnvironment::display(std::ostream& ofs) {
  for (PaganEnvironmentTableCollection::iterator it = tables_.begin();
       it != tables_.end();
       ++it) {
    (*it)->display(ofs);
  }
}

void PaganEnvironment::displayLocal(std::ostream& ofs) {
  tables_.front()->display(ofs);
}

PaganEnvironmentVariable* PaganEnvironment::lookupVariable(
    PaganEnvironmentVariable::PaganVariableIdentifier name) {
  for (PaganEnvironmentTableCollection::iterator it = tables_.begin();
       it != tables_.end();
       ++it) {
    PaganEnvironmentVariable* variable
      = (*it)->getVariable(name);
    
    if (variable != NULL) {
      return variable;
    }
  }
  
  // Variable not present in environment
  throw EnvironmentLookupException(PAGAN_SRCANDLINE,
                         "PaganEnvironment::lookupVariable"
                         "(PaganEnvironmentVariable::PaganVariableIdentifier)",
                         name);
}


} 
