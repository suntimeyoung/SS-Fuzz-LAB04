#include <random>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Analysis/TargetLibraryInfo.h"

#include "../src/loop.hpp"

using namespace llvm;

namespace {
    struct InstrumentFunc : public FunctionPass {
        static char ID;
        // random gen.
        std::mt19937 rng;
        std::uniform_int_distribution<uint32_t> dist;

        InstrumentFunc() : FunctionPass(ID), rng(std::random_device{}()), dist(std::uniform_int_distribution<uint32_t>(0, UINT32_MAX)) {}
        
        bool runOnFunction(Function &F) override;
    };
}

char InstrumentFunc::ID = 0;

bool InstrumentFunc::runOnFunction(Function &F) {

    if (F.getName() == "__cxx_global_var_init" || F.getName() == "_GLOBAL__sub_I_test.cpp") {
        return true;
    }
    
    Module *M = F.getParent();
    Module &ModuleRef = *M;
    LLVMContext &Context = M->getContext();
    IntegerType *Int8Ty  = IntegerType::getInt8Ty(Context);
    IntegerType *Int32Ty = IntegerType::getInt32Ty(Context);

    /* Get globals for the SHM region and the previous location. Note that
     __cfl_prev_loc is thread-local. */

    GlobalVariable *AFLMapPtr = M->getGlobalVariable("__cfl_area_ptr");
    if (!AFLMapPtr) {
        AFLMapPtr = new GlobalVariable(
            ModuleRef, PointerType::getUnqual(Int32Ty), false,
            GlobalValue::ExternalLinkage, nullptr, "__cfl_area_ptr",
            nullptr, GlobalVariable::GeneralDynamicTLSModel, 0, false);
    }

    GlobalVariable *AFLPrevLoc = M->getGlobalVariable("__cfl_prev_loc");
    if (!AFLPrevLoc) {
        AFLPrevLoc = new GlobalVariable(
            ModuleRef, Int32Ty, false, GlobalValue::ExternalLinkage, nullptr, "__cfl_prev_loc",
            nullptr, GlobalVariable::GeneralDynamicTLSModel, 0, false);
    }

    // Define or get the existing 'UpdateCoverageMap' function
    FunctionCallee updateCovMap = M->getOrInsertFunction(
        "UpdateCoverageMap",
        FunctionType::get(Type::getVoidTy(Context), {Int32Ty, PointerType::getUnqual(Int32Ty)}, false)
    );

      
    for (auto &B : F) {

        IRBuilder<> IRB(&(*(B.getFirstInsertionPt())));

        /* Generate the current location of the bb by random value */
        uint32_t cur_loc = dist(rng) % (1 << FSHM_MAX_ITEM_POW2);
        ConstantInt *CurLoc = ConstantInt::get(Int32Ty, cur_loc);

        /* Load prev_loc */
        LoadInst *PrevLoc = IRB.CreateLoad(Int32Ty, AFLPrevLoc);
        Value *PrevLocCasted = IRB.CreateZExt(PrevLoc, IRB.getInt32Ty());

        /* Load SHM ptr */
        LoadInst *MapPtr = IRB.CreateLoad(PointerType::getUnqual(Int32Ty), AFLMapPtr); // Ensure correct pointer type is used
        Value *MapPtrIdx = IRB.CreateGEP(Int32Ty, MapPtr, IRB.CreateXor(PrevLocCasted, CurLoc)); // GEP needs correct pointer types

        /* Update bitmap */
        LoadInst *Counter = IRB.CreateLoad(Int32Ty, MapPtrIdx);
        Value *Incr = IRB.CreateAdd(Counter, ConstantInt::get(Int32Ty, 1));
        IRB.CreateStore(Incr, MapPtrIdx);

        // Call the new function 'UpdateCoverageMap'
        // IRB.CreateCall(updateCovMap, {IRB.CreateXor(PrevLocCasted, CurLoc), MapPtrIdx});
        
        /* Set prev_loc to cur_loc >> 1 */
        IRB.CreateStore(ConstantInt::get(Int32Ty, cur_loc >> 1), AFLPrevLoc);
    }

    // instrument `Forkserver` in the entrypoint.
    if (F.getName() == "main") {
        IRBuilder<> forkBuilder(&(*(F.getEntryBlock().getFirstInsertionPt())));
        FunctionCallee forkServer = M->getOrInsertFunction("ForkServer", Type::getVoidTy(Context), Type::getInt64Ty(Context));
        forkBuilder.CreateCall(forkServer);
    }

    return true;
}


static RegisterStandardPasses RegisterMyPass(
  PassManagerBuilder::EP_EarlyAsPossible,
  [](const PassManagerBuilder &Builder,
  legacy::PassManagerBase &PM) {

    PM.add(new InstrumentFunc());

});