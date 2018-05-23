#ifndef PAGANPARSENODE_H
#define PAGANPARSENODE_H


#include "pagan/PaganLexeme.h"
#include <iostream>

namespace Pagan {


/**
 * A node in a Pagan parse tree.
 * Extends PaganParseNode with binary tree functionality.
 */
class PaganParseNode : public PaganLexeme {
public:

  PaganParseNode();
  virtual ~PaganParseNode();
  PaganParseNode(TypeID type__,
              int lineNum__);
/*  PaganParseNode(TypeID type__,
              int lineNum__,
              PaganInt intValue__);
  PaganParseNode(TypeID type__,
              int lineNum__,
              PaganString stringValue__); */
  PaganParseNode(const PaganLexeme& lexeme);
  PaganParseNode(const PaganParseNode& node);
/*  PaganParseNode(PaganParseNode* node); */
  
  PaganParseNode* left();
  void setLeft(PaganParseNode* left__);
  
  PaganParseNode* right();
  void setRight(PaganParseNode* right__);
  
  void* voidp();
  void setVoidp(void* voidp__);
  
  void* fvp();
  void setFvp(void* fvp__);
  
  /**
   * Print "pretty" representation of parse tree.
   */
  void prettyPrint(std::ostream& ofs);
  
  /**
   * Print "ugly" representation of parse tree.
   */
  friend std::ostream& operator<<(std::ostream& ofs,
                                  const PaganParseNode& node);
  
protected:
  
  /**
   * Helper for pretty printing that properly indents a line.
   */
  void printIndentation(std::ostream& ofs,
                        int indent,
                        bool linebreaks = true);
  
  /**
   * Helper for pretty printing that breaks a line if needed.
   */
  void breakLine(std::ostream& ofs, bool linebreaks);

  /**
   * Internal implementation of pretty printing.
   */
  void prettyPrintStep(std::ostream& ofs, int indent, bool linebreaks = true);
  
  /**
   * Left child pointer.
   */
  PaganParseNode* left_;
  
  /**
   * Right child pointer.
   */
  PaganParseNode* right_;
  
  /**
   * Void pointer.
   * Ownership is not taken when set.
   * Used for storing environment pointers during evaluation.
   */
  void* voidp_;
  
  /**
   * Evaluation function pointer.
   */
  void* fvp_;
  
};


}


#endif 
