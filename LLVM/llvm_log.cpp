#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
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
    class InsertLogBasic64 : public FunctionPass {
    public:
        static char ID;
        InsertLogBasic64() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
            for (auto &B : F) {
                Instruction *TInst = B.getTerminator();
                IRBuilder<> Builder(TInst);
                LLVMContext &Context = F.getContext();

                // 获取或插入 logBasic 函数定义
                FunctionCallee logFunc = F.getParent()->getOrInsertFunction(
                    "logBasic", FunctionType::get(Type::getVoidTy(Context), {Type::getInt64Ty(Context)}, false)
                );

                // 创建调用 logBasic 的指令，参数为基本块终结指令的地址，确保为64位
                Value *TInstAddr = Builder.CreatePtrToInt(TInst, Type::getInt64Ty(Context));
                Builder.CreateCall(logFunc, {TInstAddr});

                errs() << "logBasic called with address: " << *TInstAddr << "\n";
            }
            return true;
        }
    };
}

char InsertLogBasic64::ID = 0;
static void registerInsertLogBasic64(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new InsertLogBasic64());
}

static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerInsertLogBasic64);