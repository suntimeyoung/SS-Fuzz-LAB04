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

// bool InsertLogBasic::runOnFunction(Function &F) {
//     // 创建或获取全局计数器变量
//     Module *M = F.getParent();
//     LLVMContext &Context = F.getContext();
//     GlobalVariable *address = dyn_cast<GlobalVariable>(M->getOrInsertGlobal("blockAddress", Type::getInt32Ty(Context)));
//     if (!address) {
//         address = new GlobalVariable(*M, Type::getInt32Ty(Context), false, GlobalValue::InternalLinkage, ConstantInt::get(Type::getInt32Ty(Context), 0), "blockAddress");
//     }
//     for (auto &B : F) {
//         errs() << "this is basicblock\n";
//         BasicBlock::iterator IP = B.getFirstInsertionPt();
//         IRBuilder<> Builder(&(*IP)); // 将构造器位置设在基本块的开始
//         // 加载当前计数
//         LoadInst* currentAdd = Builder.CreateLoad(Type::getInt32Ty(Context), address, "loadAddress");
//         // 创建自增操作
//         Value *incremented = Builder.CreateAdd(currentAdd, ConstantInt::get(Type::getInt32Ty(Context), 1), "increment");
//         // 存储回全局变量
//         Builder.CreateStore(incremented, address);

//     }
//   return true;
// }

char BranchInstrumentationPass::ID = 0;

static void registerBranchInstrumentationPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new BranchInstrumentationPass());
}

static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerBranchInstrumentationPass);
