#ifndef PAGANENVIRONMENTVARIABLE_H
#define PAGANENVIRONMENTVARIABLE_H


#include "pagan/PaganParseNode.h"
#include <string>

namespace Pagan {


/**
 * A variable entry in a PaganEnvironment table.
 */
class PaganEnvironmentVariable {
public:
  typedef std::string PaganVariableIdentifier;

  PaganEnvironmentVariable();
  PaganEnvironmentVariable(const PaganEnvironmentVariable& var);
  PaganEnvironmentVariable(PaganVariableIdentifier name__,
                           PaganParseNode& value__);
  ~PaganEnvironmentVariable();
  
  PaganVariableIdentifier name() const;
  void setName(PaganVariableIdentifier name__);
  
  PaganParseNode* value();
  void setValue(PaganParseNode& value__);
protected:
  
  /**
   * The variable's identifier.
   */
  PaganVariableIdentifier name_;
  
  /**
   * The variable's value.
   */
  PaganParseNode* value_;
  
};


}


#endif
