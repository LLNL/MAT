// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InstIterator.h>
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
//#include "llvm/IR/ModuleSlotTracker.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/DebugInfoMetadata.h"
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
#include "mat_io.h"

#define MAT_VERSION (1)

int MAT::location_id = 0;
int MAT::file_local_id = 0;
int MAT::path_name_id = 0;
int MAT::instruction_id = 1;
std::unordered_map<string, size_t> MAT::file2hash_umap;

void MAT::init_instrumented_functions(Module *M)
{
  MAT_ERR("This is not used");
  LLVMContext &ctx = M->getContext();
  // void mat_control(int control, int file_id, int loc_id, int type, void *addr, size_t size)
  mat_func_umap[MAT_CONTROL_STR] =  M->getOrInsertFunction(MAT_CONTROL_STR,
							   Type::getVoidTy(ctx),
							   Type::getInt32Ty(ctx),
							   Type::getInt32Ty(ctx),
							   Type::getInt32Ty(ctx),
							   Type::getInt32Ty(ctx),
							   Type::getInt64PtrTy(ctx),
							   Type::getInt64Ty(ctx));
  return;
}

Constant*  MAT::get_control_func(Type *type)
{
  LLVMContext &ctx = MAT_M->getContext();
  Constant* func;

  //void mat_control(int control, int file_id, int loc_id, int type, void *addr, size_t size, int num_insts)
  func =  MAT_M->getOrInsertFunction(MAT_CONTROL_STR,
				     Type::getVoidTy(ctx),  /* return */
				     Type::getInt32Ty(ctx), /* control */
				     Type::getInt64Ty(ctx), /* global_id */
				     Type::getInt32Ty(ctx), /* type */
				     type,                  /* addr */
				     Type::getInt64Ty(ctx), /* size_t */
				     Type::getInt32Ty(ctx)  /* num_insts */
#if __clang_major__ <= 4
				     , NULL);
#else
                                     );
#endif
  
  return func;
}

int MAT::get_filepath_hash(Instruction *I)
{
  const char *file_name, *dir_name;
  string path_name;
  if (!path_name_id) {
    if (get_path(I, &file_name, &dir_name)) {
      decltype(file2hash_umap)::hasher hash{ file2hash_umap.hash_function() };
      path_name = string(dir_name) + "/" + string(file_name) ;
      path_name_id = hash(path_name);
      if (!path_name_id) path_name_id++;
    }
  }
  return path_name_id;
}

size_t MAT::get_global_instruction_id(Instruction *I)
{  
  if (inst_ptr_to_id->find(I) == inst_ptr_to_id->end()) {
    MAT_ERR("No such instruction pointer");
    I->print(errs(), false);
  }
  return inst_ptr_to_id->at(I);
}
  
int MAT::get_path(Instruction *I, const char **file_name, const char **dir_name)
{
  const char *filename, *dirname;
  if (const DebugLoc &dbloc = I->getDebugLoc()) {
    if (DIScope *discope = dyn_cast<DIScope>(dbloc.getScope())) {
      filename = discope->getFilename().data();
      dirname  = discope->getDirectory().data();
    } else {
      MAT_ERR("Third operand of DebugLoc is not DIScope");
    }
    *file_name = filename;
    *dir_name  = dirname;
    return 1;
  }
  *file_name = NULL;
  *dir_name = NULL;
  return 0;
}

int MAT::insert_func(Instruction *I, BasicBlock *BB, int offset, int control,
		     Value* type, Value* addr, Value* size, Value* num_insts)
{
  vector<Value*> arg_vec;
  IRBuilder<> builder(I);
  Constant* func;
  Type *addr_type;
  size_t global_id;
  
  builder.SetInsertPoint(BB, (offset)? ++builder.GetInsertPoint():builder.GetInsertPoint());
  //  func = mat_func_umap.at(MAT_CONTROL_STR);
  addr_type = (!addr)? MAT_INT64PTRTY:addr->getType();
  func = get_control_func(addr_type);
  
  arg_vec.push_back(MAT_CONST_INT32TY(control));

  //  get_uniq_instruction_id(I, &file_id, &loc_id);
  //  arg_vec.push_back(MAT_CONST_INT32TY(file_id));
  //  arg_vec.push_back(MAT_CONST_INT32TY(loc_id));
  global_id = get_global_instruction_id(I);
  arg_vec.push_back(MAT_CONST_INT64TY(global_id));
  
  if (!type) type = MAT_CONST_INT32TY(0);
  arg_vec.push_back(type);
  if (!addr) addr = MAT_CONST_INT64PTRTY_NULL;
  arg_vec.push_back(addr);
  if (!size) size = MAT_CONST_INT64TY(0);
  arg_vec.push_back(size);
  if (!num_insts) num_insts = MAT_CONST_INT32TY(0);
  arg_vec.push_back(num_insts);
  
  builder.CreateCall(func, arg_vec);
  //  addr->mutateType(tmp_type);
  //MAT_DBG("inserted !!");
  return 1;
}


void MAT::get_uniq_instruction_id(Instruction *I, int *file_id, int *loc_id)
{
  *file_id = get_filepath_hash(I);
  *loc_id = location_id++;
  return;
}

void MAT::get_line_column(Instruction *I, int *line, int *column)
{
  if (const DebugLoc &dbloc = I->getDebugLoc()) {
    *line   = dbloc.getLine();
    *column = dbloc.getCol();
  } else {
    *line = 0;
    *column = 0;
  }
  return;
}

int MAT::get_lien_colum_id(Instruction *I)
{
  int line = 0, column = 0;
  if (const DebugLoc &dbloc = I->getDebugLoc()) {
    line   = dbloc.getLine();
    column = dbloc.getCol();
  }
  return 100000 * line  + column;
}

/* ===================================================== */
/*                      MATFunc                          */
/* ===================================================== */


char MATFunc::ID = 0;


MATFunc::MATFunc()
  : MAT()
  , FunctionPass(ID)
{
  char* env;
  int env_int;

  if (NULL != (env = getenv(MAT_ENV_DEP_FILE))) {
    data_dependency_dir = env;
  } else {
    data_dependency_dir = ".mat";
  }

  inst_ptr_to_id = new unordered_map<Instruction*, size_t>();
  data_dependency_umap = new unordered_map<size_t, vector<size_t>*>();
}

int MATFunc::instrument_init_and_finalize(Function &F)
{
  int modified_counter = 0;
  int is_hook_enabled = 0;
  if (F.getName() == "main") {
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
	if (!is_hook_enabled) {
	  modified_counter += insert_func(&I, &BB, MAT_IR_PASS_INSERT_BEFORE, MAT_INIT, NULL, NULL, NULL, NULL);
	  is_hook_enabled = 1;
	}
	if (dyn_cast<ReturnInst>(&I)) {
	  modified_counter += insert_func(&I, &BB, MAT_IR_PASS_INSERT_BEFORE, MAT_FIN, NULL, NULL, NULL, NULL);
	} 
      }
    }
  } 
  return modified_counter;
}

int MATFunc::instrument_load_store(Function &F, BasicBlock &BB, Instruction &I, mat_ir_profile_t *profile)
{
  int modified_counter = 0;
  if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
    Value *address_of_store = SI->getPointerOperand();
    PointerType* pointerType = cast<PointerType>(address_of_store->getType());
    uint64_t storeSize = MAT_DL->getTypeStoreSize(pointerType->getPointerElementType());
    insert_func(&I, &BB, MAT_IR_PASS_INSERT_AFTER , MAT_TRACE,
		MAT_CONST_INT32TY(MAT_TRACE_STORE), address_of_store, MAT_CONST_INT64TY(storeSize),
		MAT_CONST_INT32TY(profile->num_insts));
    modified_counter += 1;
  } else if (LoadInst *LI = dyn_cast<LoadInst>(&I)) {
    Value *address_of_load = LI->getOperand(0);
    PointerType* pointerType = cast<PointerType>(address_of_load->getType());
    uint64_t loadSize = MAT_DL->getTypeStoreSize(pointerType->getPointerElementType());
    insert_func(&I, &BB, MAT_IR_PASS_INSERT_AFTER , MAT_TRACE,
    		MAT_CONST_INT32TY(MAT_TRACE_LOAD), address_of_load, MAT_CONST_INT64TY(loadSize),
		MAT_CONST_INT32TY(profile->num_insts));
    modified_counter += 1;
  }
  return modified_counter;
}

bool MATFunc::is_memory_access(Instruction &I)
{
  if (dyn_cast<StoreInst>(&I)) return true;
  if (dyn_cast<LoadInst>(&I)) return true;
  return false;
}

void MATFunc::add_data_dependency(size_t src, size_t dst)
{
  vector<size_t> *src_vec;
  if (data_dependency_umap->find(dst) == data_dependency_umap->end()) {
    data_dependency_umap->insert(make_pair(dst, new vector<size_t>()));
  }
  src_vec = data_dependency_umap->at(dst);
  src_vec->push_back(src);
  return;
}

int MATFunc::analyse_data_dependency(Function &F, mat_ir_profile_t *prof)
{
  DependenceInfo *depinfo;
  depinfo = &getAnalysis<DependenceAnalysisWrapperPass>().getDI();
  // check dependencies between each pair of instructions
  for (inst_iterator Iit = inst_begin(F), Iit_end = inst_end(F); Iit != Iit_end; Iit++) {
    Instruction &I = *Iit;
    for (inst_iterator Jit = Iit; Jit != Iit_end; Jit++) {
      Instruction &J = *Jit;
      if (!(is_memory_access(I) && is_memory_access(J))) continue;
      unique_ptr<Dependence> infoPtr;
      /* &I == dep->getSrc(), &J == dep->getDst() */
      /* Src(&I) happens before Dst(&J) */
      infoPtr = depinfo->depends(&I, &J, true);
      Dependence *dep = infoPtr.get();
      
      if (dep != NULL && dep->isOrdered()) {
	//	int line_I, line_J;
	//	int col_I, col_J;  
	//if (!dep->isLoopIndependent()) errs() << "[L]";
	//if (dep->isConfused()) errs() << "[C]";
	//	dep->getDst()->print(errs(), false); 
	//	errs() << "   ---> ";
	//	dep->getSrc()->print(errs(), false);
	//	errs() << "\n";
	// MAT_DBG("I: %p, Src: %p, J: %p, Dst: %p",
	// 	&I, dep->getSrc(), &J, dep->getDst()
	// 	);
	add_data_dependency(
			    get_global_instruction_id(dep->getSrc()),
			    get_global_instruction_id(dep->getDst())
			    );


      }
    }
  }
  return 0;
}

int MATFunc::handle_function(Function &F, mat_ir_profile_t *profile)
{
  int modified_counter = 0;
  assign_id_to_instructions(F);
  modified_counter += instrument_init_and_finalize(F);
  modified_counter += analyse_data_dependency(F, profile);
  return modified_counter;
}

int MATFunc::handle_basicblock(Function &F, BasicBlock &BB, mat_ir_profile_t *profile)
{
  return 0;
}

int MATFunc::handle_basicblock_postprocess(Function &F, BasicBlock &BB, mat_ir_profile_t *profile)
{
  size_t size;
  int rest_num_insts;
  size = BB.size();
  rest_num_insts = profile->num_insts;
  Instruction &I = BB.back();
  insert_func(&I, &BB, MAT_IR_PASS_INSERT_BEFORE, MAT_BB,
	      NULL, NULL,
	      MAT_CONST_INT64TY(size),
	      MAT_CONST_INT32TY(rest_num_insts));
  profile->num_insts = 0;
  return 1;
}

int MATFunc::handle_instruction(Function &F, BasicBlock &BB, Instruction &I, mat_ir_profile_t *profile)
{
  int modified_counter = 0;
  CallInst *CI = NULL;
  StringRef name;

  if ((CI = dyn_cast<CallInst>(&I)) != NULL) {
    name = CI->getCalledValue()->stripPointerCasts()->getName();
    if (name.startswith("mat")) return modified_counter;
    if (name.startswith("llvm.dbg")) return modified_counter;
  }
  
  profile->num_insts++;
  modified_counter += instrument_load_store(F, BB, I, profile); /* */
  if (modified_counter > 0) profile->num_insts = 0;
  return modified_counter;
}

bool MATFunc::doInitialization(Module &M)
{
  MAT_M   = &M;
  MAT_CTX = &(M.getContext());
  MAT_DL  = new DataLayout(MAT_M);
  //  init_instrumented_functions(&M);

  //  const string &file_name = MAT_M->getSourceFileName();
  //  const string &file_name = MAT_M->getModuleIdentifier();

  return true;
}

bool MATFunc::doFinalization(Module &M)
{
  FILE* fd;
  int status;
  const string &file_name = MAT_M->getSourceFileName();
  const char* file_name_c = file_name.c_str();
  char path[PATH_MAX];
  
  /* Dump dependency files*/
  if (0 != (status = mkdir(data_dependency_dir.c_str(), S_IRWXU))) {
    //    MAT_ERR("Failed to create directory: %s", data_dependency_dir);
  }
  sprintf(path, "%s/%s.mdep", data_dependency_dir.c_str(), file_name_c);
  fd = mat_io_fopen(path, "wb");
  for (pair<size_t, vector<size_t>*> e: *data_dependency_umap) {
    mat_io_fwrite(&e.first, sizeof(size_t), 1, fd);
    mat_io_fwrite(&e.second->front(), sizeof(size_t), e.second->size(), fd);
  }
  mat_io_fclose(fd);
  
  return true;
}


void MATFunc::assign_id_to_instructions(Function &F)
{
  int file_id = 0;
  size_t id;
  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      if (file_id == 0) {
	file_id  = get_filepath_hash(&I);
	id = file_id;
	id = id << 32;
      }
      inst_ptr_to_id->insert(make_pair(&I, id + instruction_id++));
    }
  }
  return;
}

bool MATFunc::runOnFunction(Function &F)
{
  int modified_counter = 0;
  mat_ir_profile_t profile;
  

  memset(&profile, 0, sizeof(mat_ir_profile_t));
  modified_counter += handle_function(F, &profile);
  for (BasicBlock &BB : F) {
    modified_counter += handle_basicblock(F, BB, &profile);
    for (Instruction &I : BB) {
      modified_counter = handle_instruction(F, BB, I, &profile);
    }
    modified_counter += handle_basicblock_postprocess(F, BB, &profile);
  }
  return modified_counter > 0;
}


void MATFunc::getAnalysisUsage(AnalysisUsage &AU) const
{
  AU.addRequired<DependenceAnalysisWrapperPass>();
  //  AU.setPreservesAll();
}

/* ===================================================== */
/*                      MATLoop                          */
/* ===================================================== */

char MATLoop::ID = 1;
size_t MATLoop::loop_id = 0;

bool MATLoop::doInitialization(Loop *L, LPPassManager &LPM)
{
  MAT_M   = L->getLoopPreheader()->getModule();
  MAT_CTX = &(MAT_M->getContext());
  MAT_DL  = new DataLayout(MAT_M);
  //  init_instrumented_functions(MAT_M);
  return true;
}


int MATLoop::handle_loop(Loop *L, LPPassManager &LPM)
{
  int modified_counter = 0;
  BasicBlock *BB;
  Instruction *I;
  SmallVector<BasicBlock*, 10> BBs;
  /* Example: 
      for (<Preheader>; <Header>; <Latch>) {<Body>}
      But, there could be multiple Latches when while-loop.
   */

  /* Preheader */
  BB = L->getLoopPreheader();
  I = &(BB->front());
  insert_func(I, BB, MAT_IR_PASS_INSERT_AFTER, MAT_LOOP,
	      MAT_CONST_INT64TY(MAT_LOOP_PREHEADER), NULL, MAT_CONST_INT64TY(loop_id), NULL);
  /* Header begin */
  BB = L->getHeader();
  I = &(BB->front());
  insert_func(I, BB, MAT_IR_PASS_INSERT_BEFORE, MAT_LOOP,
	      MAT_CONST_INT64TY(MAT_LOOP_HEADER_BEGIN), NULL, MAT_CONST_INT64TY(loop_id), NULL);    
  /* Body begin */
  BB = L->getHeader();
  I = &(BB->back());
  if (BranchInst *BI = dyn_cast<BranchInst>(I)) {
    BB = BI->getSuccessor(0);
    I  = &(BB->front());
    insert_func(I, BB, MAT_IR_PASS_INSERT_BEFORE, MAT_LOOP,
		MAT_CONST_INT64TY(MAT_LOOP_BODY_BEGIN), NULL, MAT_CONST_INT64TY(loop_id), NULL);    
  } else {
    MAT_ERR("Loop header basicblock does not end with branch instruction");
  }
  /* Body end == Latch begin */
  L->getLoopLatches(BBs);
  for (auto BB: BBs) {
    I = &(BB->front());
    insert_func(I, BB, MAT_IR_PASS_INSERT_BEFORE, MAT_LOOP,
		MAT_CONST_INT64TY(MAT_LOOP_BODY_END), NULL, MAT_CONST_INT64TY(loop_id), NULL);
  }
  /* Exit node */
  // BB = L->getExitBlock();
  // I = &(BB->back());
  // insert_func(I, BB, MAT_IR_PASS_INSERT_BEFORE, MAT_LOOP, MAT_CONST_INT64TY(MAT_LOOP_EXIT), NULL, NULL);
  L->getExitBlocks(BBs);
  for (auto BB: BBs) {
    /* L is supposed not to have BB, i.e., L->contains(BB) must be always False. 
       But, L->ocntains(BB) returns True sometime. Is this Bug ?
     */
    if (!L->contains(BB)) {
      I = &(BB->front());
      insert_func(I, BB, MAT_IR_PASS_INSERT_BEFORE, MAT_LOOP,
		  MAT_CONST_INT64TY(MAT_LOOP_EXIT), NULL, MAT_CONST_INT64TY(loop_id), NULL);
    }
  }

  loop_id++;
  
  return modified_counter;
}

bool MATLoop::runOnLoop(Loop *L, LPPassManager &LPM)
{
  //  return handle_loop(L, LPM) > 0;
  return false;
}

void MATLoop::getAnalysisUsage(AnalysisUsage &AU) const
{}



static void registerMAT(const PassManagerBuilder &, legacy::PassManagerBase &PM)
{
  PM.add(new DependenceAnalysisWrapperPass);
  PM.add(new MATFunc);
  PM.add(new MATLoop);
}

//static RegisterStandardPasses RegisterMAT(PassManagerBuilder::EP_EarlyAsPossible, registerMAT);
static RegisterStandardPasses RegisterMAT(PassManagerBuilder::EP_OptimizerLast, registerMAT);

static RegisterPass<MATLoop> X("matloop", "MAT loop Pass",
			     false /* Only looks at CFG */,
			     false /* Analysis Pass */);

static RegisterPass<MATFunc> Y("matfunc", "MAT func Pass",
			     false /* Only looks at CFG */,
			     false /* Analysis Pass */);

// opt -load ../src/.libs/libmatir.so -matfunc mat_test_simple_ll-mat_test_simple.o 

/* ============== End of class ================================= */

