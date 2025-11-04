/*
Aitiana L. Mondragon
CS 4390
Dr. Moore
Assignment - LLVM Assignment 1
November 3, 2025
*/

/* DerivedInductionVar.cpp 
 *
 * This pass detects and eliminates derived induction variables using ScalarEvolution.
 *
 * Compatible with New Pass Manager
*/

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"//added for ive
#include "llvm/IR/IRBuilder.h" // added for ive
#include "llvm/ADT/SmallVector.h"

using namespace llvm;

namespace {

class DerivedInductionVar : public PassInfoMixin<DerivedInductionVar> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    auto &LI = AM.getResult<LoopAnalysis>(F);
    auto &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

    errs() << " ==== ANALYZING FOR DERIVED INDUCTION VARIABLES ==== \n";

    for (Loop *L : LI) {
      analyzeLoopRecursive(L, SE, F.getName(), 0);
    }

    errs() << " ==== ELIMINATING THE DERIVED INDUCTION VARIABLES ==== \n";

    bool Changed = false;
    for (Loop *L : LI) {
      if (eliminateIV(L, SE))
        Changed = true;
    }

    if (Changed)
      return PreservedAnalyses::none();
    return PreservedAnalyses::all();
  }

private:
  void analyzeLoopRecursive(Loop *L, ScalarEvolution &SE, StringRef FunctionName,
                            unsigned Depth = 0) {
    BasicBlock *Header = L->getHeader();
    if (!Header)
      return;

    std::string indent(Depth * 2, ' ');
    errs() << indent << "Analyzing loop in function " << FunctionName
           << " (depth " << Depth << ")\n";

    
    for (PHINode &PN : Header->phis()) {
      if (!PN.getType()->isIntegerTy())
        continue;
      const SCEV *S = SE.getSCEV(&PN);
      // seeing if affine
      if (auto *AR = dyn_cast<SCEVAddRecExpr>(S)) {
        const SCEV *Step = AR->getStepRecurrence(SE);
        const SCEV *Start = AR->getStart();
        if (AR->isAffine()) {
          errs() << indent << "  Derived induction variable: " << PN.getName()
                 << " = {" << *Start << ",+," << *Step << "}<"
                 << Header->getName() << ">\n";
        }
      }
    }
    // recurisve call
    for (Loop *SubLoop : L->getSubLoops())
      analyzeLoopRecursive(SubLoop, SE, FunctionName, Depth + 1);
  }

  // Collect basic IVs vs derived IVs in a loop header.
  void collectIVs(Loop *L, ScalarEvolution &SE,
                  SmallVectorImpl<PHINode *> &BasicIVs,
                  SmallVectorImpl<PHINode *> &DerivedIVs) {
    BasicBlock *Header = L->getHeader();
    if (!Header)
      return;

    for (PHINode &PN : Header->phis()) {
      if (!PN.getType()->isIntegerTy())
        continue;

      const SCEV *S = SE.getSCEV(&PN);
      auto *AR = dyn_cast<SCEVAddRecExpr>(S);
      if (!AR)
        continue;

      if (!AR->isAffine())
        continue;

      const SCEV *Step = AR->getStepRecurrence(SE);

      if (isa<SCEVConstant>(Step))
        BasicIVs.push_back(&PN);
      else
        DerivedIVs.push_back(&PN);
    }
  }

  bool eliminateIV(Loop *L, ScalarEvolution &SE) {
    bool Changed = false;
    BasicBlock *Header = L->getHeader();
    if (!Header)
      return false;

    errs() << "Processing loop for elimination: " << Header->getName() << "\n";

    // Get an insertion point 
    auto It = Header->getFirstNonPHIIt();
    Instruction *HeaderInsert = nullptr;
    if (It != Header->end()) {
      HeaderInsert = &*It;
    } else {
      HeaderInsert = Header->getTerminator();
    }

    if (!HeaderInsert) {
      errs() << "  Skipping loop (no insertion point)\n";
      // handling loops
      bool subChanged = false;
      for (Loop *Sub : L->getSubLoops())
        if (eliminateIV(Sub, SE))
          subChanged = true;
      return subChanged;
    }

    Module *M = Header->getModule();
    if (!M) {
      errs() << "  No module for header; skipping\n";
      return false;
    }

    const DataLayout &DL = M->getDataLayout();
    SCEVExpander Expander(SE, DL, "ivrewrite");

    SmallVector<PHINode *, 8> BasicIVs;
    SmallVector<PHINode *, 8> DerivedIVs;
    collectIVs(L, SE, BasicIVs, DerivedIVs);

    if (!BasicIVs.empty()) {
      errs() << "  Basic IVs:\n";
      for (PHINode *PN : BasicIVs)
        errs() << "    " << PN->getName() << "\n";
    }
    if (!DerivedIVs.empty()) {
      errs() << "  Derived IVs:\n";
      for (PHINode *PN : DerivedIVs)
        errs() << "    " << PN->getName() << "\n";
    }

    SmallVector<PHINode *, 8> ToRemove;

    // Expand and replace derived IVs
    for (PHINode *PN : DerivedIVs) {
      const SCEV *S = SE.getSCEV(PN);

      auto *AR = dyn_cast<SCEVAddRecExpr>(S);
      if (!AR || !AR->isAffine())
        continue;

      const SCEV *Step = AR->getStepRecurrence(SE);
      
      // Skip basic ivs
      if (auto *C = dyn_cast<SCEVConstant>(Step)) {
        if (C->getAPInt() == 1) {
          errs() << "    Skipping likely-basic IV: " << PN->getName() << "\n";
          continue;
        }
      }

      errs() << "    Expanding derived IV " << PN->getName() << " : " << *AR << "\n";

      // Expand the AddRec into IR
      Value *NewVal = Expander.expandCodeFor(AR, PN->getType(), HeaderInsert);
      if (!NewVal) {
        errs() << "      Expansion failed for " << PN->getName() << "\n";
        continue;
      }

      PN->replaceAllUsesWith(NewVal);
      if (PN->use_empty())
        ToRemove.push_back(PN);
      else
        errs() << "      After replacement, PHI still has uses (will not erase)\n";

      Changed = true;
    }

    // Erase vars that became dead
    for (PHINode *PN : ToRemove) {
      errs() << "    Erasing PHI: " << PN->getName() << "\n";
      PN->eraseFromParent();
    }
    //recursive call
    for (Loop *SubLoop : L->getSubLoops()) {
      if (eliminateIV(SubLoop, SE))
        Changed = true;
    }

    return Changed;
  }
};

} // namespace

// Register the pass
llvm::PassPluginLibraryInfo getDerivedInductionVarPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DerivedInductionVar", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "derived-iv") {
                    FPM.addPass(DerivedInductionVar());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getDerivedInductionVarPluginInfo();
}
