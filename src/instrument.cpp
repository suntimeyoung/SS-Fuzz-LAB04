#include <random>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
    struct InstrumentFunc : public FunctionPass {
        static char ID;
        // random gen.
        std::mt19937_64 rng;
        std::uniform_int_distribution<uint64_t> dist;

        InstrumentFunc() : FunctionPass(ID), rng(std::random_device{}()), dist(std::uniform_int_distribution<uint64_t>(0, UINT64_MAX)) {}
        
        bool runOnFunction(Function &F) override;
    };
}

char InstrumentFunc::ID = 0;

bool InstrumentFunc::runOnFunction(Function &F) {
    Module *M = F.getParent();
    LLVMContext &Context = M->getContext();
      
    // create `logBasic`.
    FunctionCallee logFunc = M->getOrInsertFunction("logBasic", Type::getVoidTy(Context), Type::getInt64Ty(Context));

    // instrument `forkserver` in the entrypoint.
    if (F.getName() == "main") {
        IRBuilder<> forkBuilder(&(*(F.getEntryBlock().getFirstInsertionPt())));
        FunctionCallee forkServer = M->getOrInsertFunction("forkServer", Type::getVoidTy(Context), Type::getInt64Ty(Context));
        forkBuilder.CreateCall(forkServer);
    }
      
    for (auto &B : F) {

        uint64_t randomValue = dist(rng);
        Value *logArg = ConstantInt::get(Type::getInt64Ty(Context), randomValue);
            
        // instrument `logBasic` in every block.
        IRBuilder<> logBuilder(&(*(B.getFirstInsertionPt())));
        logBuilder.CreateCall(logFunc, {logArg});
            
        // errs() << "Inserted log call in basic block with random value: " << randomValue << "\n";
    }
    return true;
}


static RegisterStandardPasses RegisterMyPass(
  PassManagerBuilder::EP_EarlyAsPossible,
  [](const PassManagerBuilder &Builder,
  legacy::PassManagerBase &PM) {

    PM.add(new InstrumentFunc());

});