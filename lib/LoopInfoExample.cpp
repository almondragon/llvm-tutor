// LoopInfoExample.cpp
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;



// helper to recursively print loops
void printLoopInfo(Loop *L, unsigned depth=0){
	std::string indent(depth*2, ' ');

	errs() << indent << "Loop header: ";
	L->getHeader()->printAsOperand(errs(), false);
	errs() << "\n";

	if (L->getLoopPreheader())
		errs() << indent << "	Preheader: " << *L->getLoopPreheader() << "\n";

	if (L->getLoopLatch())
		errs() << indent << "	Loop Latch: " << *L->getLoopLatch() << "\n";

  // Print loop exit blocks
  SmallVector<BasicBlock *, 4> exitBlocks;
  L->getExitBlocks(exitBlocks);
  if (!exitBlocks.empty()) {
    errs() << indent << "   Exit blocks:\n";
    for (auto *BB : exitBlocks) {
        errs() << indent << "      ";
        BB->printAsOperand(errs(), false);
        errs() << "\n";
    }
  }


	errs() << indent << "	Number of blocks: " << L->getNumBlocks() << "\n";
	
	for (Loop *subloops : L->getSubLoops()) {
		printLoopInfo(subloops, depth+1);
	}	
}




namespace {
struct LoopInfoExample : public PassInfoMixin<LoopInfoExample> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    errs() << "Analyzing function: " << F.getName() << "\n";

    // Get LoopInfo for this function
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    for (Loop *L : LI) {
      printLoopInfo(L, 0);
    }

    return PreservedAnalyses::all();
  }
};
} // namespace

// Register pass plugin
llvm::PassPluginLibraryInfo getLoopInfoExamplePluginInfo() {
  return {
      LLVM_PLUGIN_API_VERSION, "loop-info-example", LLVM_VERSION_STRING,
      [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
               ArrayRef<PassBuilder::PipelineElement>) {
              if (Name == "loop-info-example") {
                FPM.addPass(LoopInfoExample());
                return true;
              }
              return false;
            });
      }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLoopInfoExamplePluginInfo();
}

