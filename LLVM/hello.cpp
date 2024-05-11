#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include <llvm/IR/Instructions.h>
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
  struct Hello : public FunctionPass {
    static char ID;
    Hello() : FunctionPass(ID) {}
    bool runOnFunction(Function &F) override {
      errs() << "Hello: ";
      errs().write_escaped(F.getName()) << '\n';
      for (auto &B : F) {
        Instruction *last_inst = B.getTerminator();
        errs() << *last_inst << "\n";
        errs() << dyn_cast<BranchInst>(last_inst) << "\n";
      }
      return false;
    }
  };
}

char Hello::ID = 0;

// Register for clang
static RegisterStandardPasses Y(PassManagerBuilder::EP_EarlyAsPossible,
  [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
    PM.add(new Hello());
  });

int main() {
  std::string ir_filename = "../test.ll";

  LLVMContext context;
  SMDiagnostic error;

  // Parse the LLVM IR file into a Module
  std::unique_ptr<Module> M = parseIRFile(ir_filename, error, context);

  if (!M) {
    // If parsing failed, print error message
    error.print("hello", errs());
    return 1;
  }

  // Create an instance of the Hello pass
  Hello hello_pass;
  Module *Mod = M.get();

  // Iterate over functions in the module
    for (Function &F: *Mod) {
    // Run the Hello pass on each function
    hello_pass.runOnFunction(F);
  }

  return 0;
}
