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

#define MAT_IR_PASS_INSERT_AFTER  (1)
#define MAT_IR_PASS_INSERT_BEFORE (0)

class MATFunc: public FunctionPass
{
 public:
  static char ID;

 MATFunc()
   : FunctionPass(ID) {}
  ~MATFunc(){}

  virtual bool doInitialization(Module &M);
  virtual bool runOnFunction(Function &F);
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  
 private:
  unordered_map<string, Constant*> mat_func_umap;
  Module *MAT_M;
  LLVMContext *MAT_CTX;
  DataLayout *MAT_DL;

  int instrument_init_and_finalize(Function &F);
  void init_instrumented_functions(Module &M);
  int insert_func(Instruction *I, BasicBlock *BB, int offset, int control, Value* ptr, Value* size);


  /* Outer Handlers */
  int handle_function(Function &F);
  int handle_basicblock(Function &F, BasicBlock &BB);
  int handle_instruction(Function &F, BasicBlock &BB, Instruction &I);
  int instrument_load_store(Function &F, BasicBlock &BB, Instruction &I);
  /* Loop Handlers */
  
  
};


class MATLoop: public LoopPass
{
 public:
  static char ID;
 MATLoop(): LoopPass(ID){}
  ~MATLoop(){}

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM);
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;

 private:
  int handle_loop(Loop *L, LPPassManager &LPM);
};


