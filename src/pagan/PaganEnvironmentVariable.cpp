#include "pagan/PaganEnvironmentVariable.h"
#include <cstddef>

namespace Pagan {


PaganEnvironmentVariable::PaganEnvironmentVariable()
  : value_(NULL) { }
  
PaganEnvironmentVariable::PaganEnvironmentVariable(
    const PaganEnvironmentVariable& var)
  : name_(var.name_),
    value_(NULL) {
  if (var.value_ != NULL) {
    value_ = new PaganParseNode(*(var.value_));
  }
}
  
PaganEnvironmentVariable::PaganEnvironmentVariable(
                         PaganVariableIdentifier name__,
                         PaganParseNode& value__)
  : name_(name__),
    value_(new PaganParseNode(value__)) { }

PaganEnvironmentVariable::~PaganEnvironmentVariable() {
  delete value_;
}

PaganEnvironmentVariable::PaganVariableIdentifier
    PaganEnvironmentVariable::name() const {
  return name_;
}

void PaganEnvironmentVariable::setName(PaganVariableIdentifier name__) {
  name_ = name__;
}

PaganParseNode* PaganEnvironmentVariable::value() {
  return value_;
}

void PaganEnvironmentVariable::setValue(PaganParseNode& value__) {
  delete value_;
  value_ = new PaganParseNode(value__);
}


} 
