#include <llvm/Pass.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Function.h>

#include <list>
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

typedef struct {
  size_t num_insts = 0;
} mat_ir_profile_t;

class MAT
{
 private:
  static int    location_id;
  static size_t path_name_id;
  static unordered_map<string, size_t> file2hash_umap;

 protected:
  Module *MAT_M;
  LLVMContext *MAT_CTX;
  DataLayout *MAT_DL;
  unordered_map<string, Constant*> mat_func_umap;


  void init_instrumented_functions(Module *M);
  void get_instruction_id(Instruction *I, int *file_id, int *loc_id);
  Constant* get_control_func(Type *type);
  int get_path(Instruction *I, const char **file_name, const char **dir_name);
  void get_line_column(Instruction *I, int *line, int *column);
  int get_lien_colum_id(Instruction *I)
  int insert_func(Instruction *I, BasicBlock *BB, int offset, int control, Value *type, Value* addr, Value* size, Value* num_insts);
};

class MATFunc: public MAT, public FunctionPass
{
 public:
  static char ID;

 MATFunc()
   : MAT(), FunctionPass(ID) {}
  ~MATFunc(){}

  virtual bool doInitialization(Module &M);
  virtual bool runOnFunction(Function &F);
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  
 private:
  int instrument_init_and_finalize(Function &F);
  bool is_memory_access(Instruction &I);

  int analyse_data_dependency(Function &F, mat_ir_profile_t *prof);
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


