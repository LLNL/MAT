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

#define MAT_CONST_INT64TY(val)     ConstantInt::get(Type::getInt64Ty(*MAT_CTX), val)
#define MAT_CONST_INT32TY(val)     ConstantInt::get(Type::getInt32Ty(*MAT_CTX), val)
#define MAT_CONST_INT64PTRTY_NULL  ConstantPointerNull::get(Type::getInt64PtrTy(*MAT_CTX))



#define MAT_IR_PASS_INSERT_AFTER  (1)
#define MAT_IR_PASS_INSERT_BEFORE (0)

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
  int get_path(Instruction *I, const char **file_name, const char **dir_name);
  int insert_func(Instruction *I, BasicBlock *BB, int offset, int control, Value *type, Value* addr, Value* size);
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



  /* Outer Handlers */
  int handle_function(Function &F);
  int handle_basicblock(Function &F, BasicBlock &BB);
  int handle_instruction(Function &F, BasicBlock &BB, Instruction &I);
  int instrument_load_store(Function &F, BasicBlock &BB, Instruction &I);
  
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


