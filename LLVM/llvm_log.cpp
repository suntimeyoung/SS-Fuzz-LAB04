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
    struct InsertLogBasic : public FunctionPass {
        static char ID;
        InsertLogBasic() : FunctionPass(ID) {}
        bool runOnFunction(Function &F) override;
    };
}

char InsertLogBasic::ID = 0;

bool InsertLogBasic::runOnFunction(Function &F) {
    for (auto &B : F) {
        errs() << "this is basicblock\n";
        Instruction *TInst = B.getTerminator();
        IRBuilder<> Builder(TInst);
        LLVMContext &Context = F.getContext();

        // 获取或插入 logBasic 函数定义
        FunctionCallee logFunc = F.getParent()->getOrInsertFunction(
            "logBasic", FunctionType::get(Type::getVoidTy(Context), {Type::getInt64Ty(Context)}, false)
        );

        // 创建调用 logBasic 的指令，参数为基本块终结指令的地址，确保为64位
        errs() << TInst << "\n";
        Value *TInstAddr = Builder.CreatePtrToInt(TInst, Type::getInt64Ty(Context));
        ArrayRef<Value*> args{TInstAddr};
        // Builder.CreateCall(logFunc, args);

        errs() << "logBasic called with address: " << TInstAddr << "\n";
        TInstAddr->getType()->print(errs());
        errs() << "\n";
    }
  return false;
}


static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
  [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
    PM.add(new InsertLogBasic());
  });