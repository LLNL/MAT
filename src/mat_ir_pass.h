// Copyright 2019 Lawrence Livermore National Security, LLC
// See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: Apache-2.0

#include <llvm/Pass.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Function.h>

#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace llvm;
using namespace std;

#define MAT_INT64PTRTY (Type::getInt64PtrTy(*MAT_CTX))
#define MAT_CONST_INT64TY(val)     ConstantInt::get(Type::getInt64Ty(*MAT_CTX), val)
#define MAT_CONST_INT32TY(val)     ConstantInt::get(Type::getInt32Ty(*MAT_CTX), val)
#define MAT_CONST_INT64PTRTY_NULL  ConstantPointerNull::get(Type::getInt64PtrTy(*MAT_CTX))

#define MAT_IR_PASS_INSERT_AFTER  (1)
#define MAT_IR_PASS_INSERT_BEFORE (0)

#define MAT_ENV_DEP_FILE ("MAT_DEP_DIR")

typedef struct {
  size_t num_insts = 0;
} mat_ir_profile_t;

class MAT
{
 public:
  MAT() {}
  ~MAT(){}
  
 private:
  static int location_id;
  static int file_local_id;

  static int path_name_id;
  static unordered_map<string, size_t> file2hash_umap;

 protected:
  Module *MAT_M;
  LLVMContext *MAT_CTX;
  DataLayout *MAT_DL;
  static int instruction_id;
  unordered_map<Instruction*, size_t> *inst_ptr_to_id;
  unordered_map<string, Constant*> mat_func_umap;

  void init_instrumented_functions(Module *M);
  int get_filepath_hash(Instruction *I);
  size_t get_global_instruction_id(Instruction *I);
  void get_uniq_instruction_id(Instruction *I, int *file_id, int *loc_id);
  Constant* get_control_func(Type *type);
  int get_path(Instruction *I, const char **file_name, const char **dir_name);
  void get_line_column(Instruction *I, int *line, int *column);
  int get_lien_colum_id(Instruction *I);
  int insert_func(Instruction *I, BasicBlock *BB, int offset, int control, Value *type, Value* addr, Value* size, Value* num_insts);

  
  
};

class MATFunc: public MAT, public FunctionPass
{
 public:
  static char ID;

  MATFunc();
  ~MATFunc(){}

  virtual bool doInitialization(Module &M);
  virtual bool doFinalization(Module &M);
  virtual bool runOnFunction(Function &F);
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  
 private:
  std::string data_dependency_dir;
  unordered_map<size_t, vector<size_t>*> *data_dependency_umap;
  
  int instrument_init_and_finalize(Function &F);
  bool is_memory_access(Instruction &I);


  void add_data_dependency(size_t src, size_t dst);
  int analyse_data_dependency(Function &F, mat_ir_profile_t *prof);
  void assign_id_to_instructions(Function &F);
  int instrument_load_store(Function &F, BasicBlock &BB, Instruction &I, mat_ir_profile_t *profile);  
  /* Outer Handlers */
  int handle_function(Function &F, mat_ir_profile_t *profile);
  int handle_basicblock(Function &F, BasicBlock &BB, mat_ir_profile_t *profile);
  int handle_basicblock_postprocess(Function &F, BasicBlock &BB, mat_ir_profile_t *profile);
  int handle_instruction(Function &F, BasicBlock &BB, Instruction &I, mat_ir_profile_t *profile);
};


class MATLoop: public MAT, public LoopPass
{
 public:
  static char ID;
  static size_t loop_id;
 MATLoop()
   : MAT(), LoopPass(ID){}
  ~MATLoop(){}

  virtual bool doInitialization(Loop *L, LPPassManager &LPM);
  virtual bool runOnLoop(Loop *L, LPPassManager &LPM);
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;

 private:
  int handle_loop(Loop *L, LPPassManager &LPM);
};


