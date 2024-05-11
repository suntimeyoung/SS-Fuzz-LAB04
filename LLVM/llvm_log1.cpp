#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
    class BranchInstrumentationPass : public FunctionPass {
    public:
        static char ID;
        BranchInstrumentationPass() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) override {
            for (auto &B : F) {
                Instruction *last_inst = B.getTerminator();
                errs() << *last_inst << "\n";
                errs() << dyn_cast<BranchInst>(last_inst) << "\n";
                for (auto &I : B) {
                    if (BranchInst *BI = dyn_cast<BranchInst>(&I)) {
                        if (BI->isConditional()) {
                            IRBuilder<> Builder(BI);
                            Value *cond = BI->getCondition();
                            Function *logFunc = cast<Function>(F.getParent()->getOrInsertFunction(
                                "logBranch", Type::getVoidTy(F.getContext()), Type::getInt1Ty(F.getContext())
                            ).getCallee());
                            Builder.CreateCall(logFunc, {cond});
                        }
                    }
                }
            }
            return false;
        }
    };
}

char BranchInstrumentationPass::ID = 0;

static void registerBranchInstrumentationPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new BranchInstrumentationPass());
}

static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerBranchInstrumentationPass);
