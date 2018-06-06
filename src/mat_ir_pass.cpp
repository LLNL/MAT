#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "llvm/Pass.h"
#include <llvm/IR/Module.h>
#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
//#include <llvm/IR/InstIterator.h>
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
//#include "llvm/IR/ModuleSlotTracker.h"
#include "llvm/IR/Attributes.h"
//#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <list>
#include <vector>

#include "mat_ir_pass.h"
#include "mat.h"
#include "mat_util.h"

#define MAT_VERSION (1)


int MAT::insert_func(Instruction *I, BasicBlock *BB, int offset, int control, Value* ptr, Value* size)
{
  vector<Value*> arg_vec;
  IRBuilder<> builder(I);
  builder.SetInsertPoint(BB, (offset)? ++builder.GetInsertPoint():builder.GetInsertPoint());
  Constant* func = mat_func_umap.at(MAT_CONTROL_STR);
  arg_vec.push_back(ConstantInt::get(Type::getInt32Ty(*MAT_CTX), control));
  if (!ptr) ptr = ConstantPointerNull::get(Type::getInt64PtrTy(*MAT_CTX));
  arg_vec.push_back(ptr);
  if (!size) size = ConstantInt::get(Type::getInt64Ty(*MAT_CTX), 0);
  arg_vec.push_back(size);
  builder.CreateCall(func, arg_vec);
  return 1;
}

int MAT::instrument_init_and_finalize(Function &F)
{
  int modified_counter = 0;
  int is_hook_enabled = 0;
  if (F.getName() == "main") {
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
	if (!is_hook_enabled) {
	  modified_counter += insert_func(&I, &BB, MAT_IR_PASS_INSERT_BEFORE, MAT_INIT, NULL, NULL);
	  is_hook_enabled = 1;
	}
	if (dyn_cast<ReturnInst>(&I)) {
	  modified_counter += insert_func(&I, &BB, MAT_IR_PASS_INSERT_BEFORE, MAT_FIN, NULL, NULL);
	} 
      }
    }
  } 
  return modified_counter;
}

int MAT::instrument_load_store(Function &F, BasicBlock &BB, Instruction &I)
{
  int modified_counter = 0;
  if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
    insert_func(&I, &BB, MAT_IR_PASS_INSERT_AFTER , MAT_TRACE,
		ConstantInt::get(Type::getInt64Ty(*MAT_CTX), 0),
		ConstantInt::get(Type::getInt64Ty(*MAT_CTX), 0));
    modified_counter += 2;
  } else if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
    insert_func(&I, &BB, MAT_IR_PASS_INSERT_AFTER , MAT_TRACE,
		ConstantInt::get(Type::getInt64Ty(*MAT_CTX), 0),
		ConstantInt::get(Type::getInt64Ty(*MAT_CTX), 0));
    modified_counter += 2;
  }
  return modified_counter;
}
			       

int MAT::handle_function(Function &F)
{
  int modified_counter = 0;
  modified_counter += instrument_init_and_finalize(F);
  return modified_counter;
}

int MAT::handle_basicblock(Function &F, BasicBlock &BB)
{
  return 0;
}

int MAT::handle_instruction(Function &F, BasicBlock &BB, Instruction &I)
{
  int modified_counter = 0;
  modified_counter += instrument_load_store(F, BB, I); /* */
  return modified_counter;
}

void MAT::init_instrumented_functions(Module &M)
{

  LLVMContext &ctx = M.getContext();
  mat_func_umap[MAT_CONTROL_STR] =  M.getOrInsertFunction(MAT_CONTROL_STR,
							      Type::getVoidTy(ctx),
							      Type::getInt32Ty(ctx),
							      Type::getInt64PtrTy(ctx),
							      Type::getInt64Ty(ctx),
							      NULL);
  return;
}

bool MAT::doInitialization(Module &M)
{
  MAT_M   = &M;
  MAT_CTX = &(M.getContext());
  MAT_DL  = new DataLayout(MAT_M);
  init_instrumented_functions(M);


  return true;
}

bool MAT::runOnFunction(Function &F)
{
  int modified_counter = 0;
  modified_counter += handle_function(F);
  for (BasicBlock &BB : F) {
    modified_counter += handle_basicblock(F, BB);
    for (Instruction &I : BB) {
      modified_counter += handle_instruction(F, BB, I);
    }
  }
  MAT_DBG("test");
  return modified_counter > 0;
}


void MAT::getAnalysisUsage(AnalysisUsage &AU) const
{
}

char MAT::ID = 0;
//static RegisterPass<DASample> A("dasample", "dasample", false, false);

static void registerMAT(const PassManagerBuilder &, legacy::PassManagerBase &PM)
{
  PM.add(new MAT);
}

static RegisterStandardPasses RegisterMAT(PassManagerBuilder::EP_EarlyAsPossible, registerMAT);


/* ============== End of class ================================= */

