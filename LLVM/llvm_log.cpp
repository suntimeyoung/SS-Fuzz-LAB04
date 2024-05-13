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
    struct InsertLogBasic : public FunctionPass {
        static char ID;
        std::mt19937_64 rng; // 随机数生成器
        std::uniform_int_distribution<uint64_t> dist;

        InsertLogBasic() : FunctionPass(ID), rng(std::random_device{}()), dist(std::uniform_int_distribution<uint64_t>(0, UINT64_MAX)) {}
        
        bool runOnFunction(Function &F) override;
    };
}

char InsertLogBasic::ID = 0;

bool InsertLogBasic::runOnFunction(Function &F) {
  Module *M = F.getParent();
  LLVMContext &Context = M->getContext();
    
  // 首先查找或创建log函数原型
  FunctionCallee logFunc = M->getOrInsertFunction("logBasic", Type::getVoidTy(Context), Type::getInt64Ty(Context));
    
  for (auto &B : F) {
    uint64_t randomValue = dist(rng); // 为当前基本块生成一个随机值

    // 插入指令的位置在基本块的开始
    BasicBlock::iterator IP = B.getFirstInsertionPt();
    IRBuilder<> Builder(&(*IP));
        
    // 构建一个64位常数
    Value *logArg = ConstantInt::get(Type::getInt64Ty(Context), randomValue);
        
    // 在基本块中插入调用log函数的指令
    Builder.CreateCall(logFunc, {logArg});
        
    // 打印信息到stderr
    errs() << "Inserted log call in basic block with random value: " << randomValue << "\n";
  }
  return true;
}


static RegisterStandardPasses RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
  [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
    PM.add(new InsertLogBasic());
  });