#include "thumbsim.hpp"
#include <list>
#include <iterator>
// These are just the register NUMBERS
#define PC_REG 15  
#define LR_REG 14
#define SP_REG 13

// These are the contents of those registers
#define PC rf[PC_REG]
#define LR rf[LR_REG]
#define SP rf[SP_REG]

typedef struct HazardData {
      bool defs;            
      unsigned int defreg;  
      bool load;            
} HazardData;

typedef struct BTAC{
      int state;
} BTAC;


vector<unsigned int> btacAdd(vector<unsigned int> v, unsigned int addr){
   unsigned int x = (addr << 23) >> 26;
   v[x] = 1;
   return v;
}

bool btacSearch(vector<unsigned int> v, unsigned int addr){
   unsigned int x = (addr << 23) >> 26;
   return(v[x] == 1);
}

list<HazardData> ilist;
Stats stats;
Caches caches(0);
vector<unsigned int> vect(128, 0);
BTAC btac;

void addList(HazardData hd, list <HazardData> *ilist){
   if(ilist->size() < 3){
      ilist->push_front(hd);
   }
   else{
      ilist->pop_back();
      ilist->push_front(hd);
   }
}

int checkHazard(int reg, int reg2 = 100){
   int i = 1;
   for(list<HazardData>::iterator it =ilist.begin(); it != ilist.end(); ++it){
      if(it->load){
         if(reg == it->defreg || reg2 == it->defreg){
            stats.cycles += (3-i);
   //         printf("Hazard cycles added %d at %d\n", 3-i, i);
            it->load = false;
            return 1;
         }
         if(reg2 != 100  && reg2 == it->defreg){
            stats.cycles += (3-i);
 //           printf("Hazard cycles added %d at %d\n", 3-i, i);
            return 1;
         }
         
      }
      i++;
   }
   return 0;
}
int checkEarly(list <HazardData> ilist, int reg){
   int i = 1;
   for(list<HazardData>::iterator it = ilist.begin(); it != ilist.end(); ++it){
    //  printf("Is %d a Load: %d\n", it->defreg , it->load);
      if(reg == it->defreg && it->load){
            stats.cycles += (3-i);
    //        printf("Early cycles at %d added %d at %d\n", it->defreg, 3-i, i);
            return 1;
      }
      i++;
   }
//   HazardData x = ilist.back();
//   if(x.load && reg == x.defreg){
//      stats.cycles += 2;
//      printf("cycles added %d\n", 2);
//   }
//   return 0;
}

// CPE 315: you'll need to implement a custom sign-extension function
// in addition to the ones given below, specifically for the unconditional
// branch instruction, which has an 11-bit immediate field
unsigned int signExtend16to32ui(short i){
  return static_cast<unsigned int>(static_cast<int>(i));
}
unsigned int signExtend11to32ui(short i){
   short mask1 = 2047;  //0000011111111111
   short mask2 = 63488; //1111100000000000
   short MSB;
   i = i & mask1;
   MSB = i >> 10;
   if(MSB == 1){
      i = i | mask2;
   }
  return static_cast<unsigned int>(static_cast<int>(i));
}

unsigned int signExtend8to32ui(char i) {
  return static_cast<unsigned int>(static_cast<int>(i));
}

// This is the global object you'll use to store condition codes N,Z,V,C
// Set these bits appropriately in execute below.
ASPR flags;

// CPE 315: You need to implement a function to set the Negative and Zero
// flags for each instruction that does that. It only needs to take
// one parameter as input, the result of whatever operation is executing
void setZeroNegative(int num){
   if(num == 0){
      flags.Z = 1;   
      flags.N =0;
   }
   else if(num < 0){
      flags.Z = 0;
      flags.N = 1;
   }
   else{
      flags.Z = 0;
      flags.N = 0;
   }
}
// This function is complete, you should not have to modify it
void setCarryOverflow (int num1, int num2, OFType oftype) {
  switch (oftype) {
    case OF_ADD:
      if (((unsigned long long int)num1 + (unsigned long long int)num2) ==
          ((unsigned int)num1 + (unsigned int)num2)) {
        flags.C = 0;
      }
      else {
        flags.C = 1;
      }
      if (((long long int)num1 + (long long int)num2) ==
          ((int)num1 + (int)num2)) {
        flags.V = 0;
      }
      else {
        flags.V = 1;
      }
      break;
    case OF_SUB:
      if (num1 >= num2) {
        flags.C = 1;
      }
      else if ((long long int)((unsigned long long int)num1 - (unsigned long long int)num2) ==
          (int)((unsigned int)num1 - (unsigned int)num2)) {
        flags.C = 0;
      }
      else {
        flags.C = 1;
      }
      if (((num1==0) && (num2==0)) ||
          (((long long int)num1 - (long long int)num2) ==
           ((int)num1 - (int)num2))) {
        flags.V = 0;
      }
      else {
        flags.V = 1;
      }
      break;
    case OF_SHIFT:
      // C flag unaffected for shifts by zero
      if (num2 != 0) {
        if (((unsigned long long int)num1 << (unsigned long long int)num2) ==
            ((unsigned int)num1 << (unsigned int)num2)) {
          flags.C = 0;
        }
        else {
          flags.C = 1;
        }
      }
      // Shift doesn't set overflow
      break;
    default:
      cerr << "Bad OverFlow Type encountered." << __LINE__ << __FILE__ << endl;
      exit(1);
  }
}

// CPE 315: You're given the code for evaluating BEQ, and you'll need to 
// complete the rest of these conditions. See Page 208 of the armv7 manual
static int checkCondition(unsigned short cond) {
  switch(cond) {
    case EQ:
      if (flags.Z == 1) {
        return TRUE;
      }
      break;
    case NE:
      if (flags.Z == 0) {
         return TRUE; //Ask about this
      }
      break;
    case CS:
      if (flags.C == 1){
         return TRUE;
      }
      break;
    case CC:
      if (flags.C == 0){
         return TRUE;
      }
      break;
    case MI:
      if (flags.N == 1){
         return TRUE;
      }
      break;
    case PL:
      if (flags.N == 0){
         return TRUE;
      }
      break;
    case VS:
      if (flags.V == 1){
         return TRUE;
      }
      break;
    case VC:
      if (flags.V == 0){
         return TRUE;
      }
      break;
    case HI:
      if (flags.C == 1 && flags.Z == 0){
         return TRUE;
      }
      break;
    case LS:
      if (flags.C == 0 && flags.Z == 1){
         return TRUE;
      }
      break;
    case GE:
      if (flags.N == flags.V)
       return TRUE;
      break;
    case LT:
      if (flags.N != flags.V)
       return TRUE;
      break;
    case GT:
      if (flags.Z == 0 && (flags.N == flags.V))
         return TRUE;
      break;
    case LE:
      if (flags.Z == 1 || (flags.N != flags.V))
         return TRUE;
      break;
    case AL:
      return TRUE;
      break;
  }
  return FALSE;
}

void execute() {
  Data16 instr = imem[PC];
  Data16 instr2;
  Data32 temp(0); // Use this for STRB instructions
  Thumb_Types itype;
  // the following counts as a read to PC
  unsigned int pctarget = PC + 2;
  unsigned int addr;
  int i, n, offset;
  unsigned int list, mask;
  int num1, num2, result, BitCount;
  unsigned int bit;
  HazardData hd;
  hd.load = 0;
  /* Convert instruction to correct type */
  /* Types are described in Section A5 of the armv7 manual */
  BL_Type blupper(instr);
  ALU_Type alu(instr);
  SP_Type sp(instr);
  DP_Type dp(instr);
  LD_ST_Type ld_st(instr);
  MISC_Type misc(instr);
  COND_Type cond(instr);
  UNCOND_Type uncond(instr);
  LDM_Type ldm(instr);
  STM_Type stm(instr);
  LDRL_Type ldrl(instr);
  ADD_SP_Type addsp(instr);

  BL_Ops bl_ops;
  ALU_Ops add_ops;
  DP_Ops dp_ops;
  SP_Ops sp_ops;
  LD_ST_Ops ldst_ops;
  MISC_Ops misc_ops;

  stats.numRegReads++;
  // This counts as a write to the PC register
  rf.write(PC_REG, pctarget);
  stats.numRegWrites++;

  itype = decode(ALL_Types(instr));

  // CPE 315: The bulk of your work is in the following switch statement
  // All instructions will need to have stats and cache access info added
  // as appropriate for that instruction.
  switch(itype) {
    case ALU:
      add_ops = decode(alu);
      switch(add_ops) {
        case ALU_LSLI:
          setCarryOverflow(rf[alu.instr.lsli.rm], alu.instr.lsli.imm, OF_SHIFT);
          setZeroNegative(rf[alu.instr.lsli.rm] << alu.instr.lsli.imm); 
          rf.write(alu.instr.lsli.rd, rf[alu.instr.lsli.rm] << alu.instr.lsli.imm);
          //printf("%x\n", rf[alu.instr.lsli.rd]);
          checkHazard(alu.instr.lsli.rm);
          stats.numRegReads++;
          stats.numRegWrites++;
          break;
        case ALU_ADDR:
          // needs stats
          //printf("rn %x\n", rf[alu.instr.addr.rn]);
          //printf("rm %x\n", rf[alu.instr.addr.rm]);
          setCarryOverflow(rf[alu.instr.addr.rn], rf[alu.instr.addr.rm], OF_ADD);
          setZeroNegative(rf[alu.instr.addr.rn] + rf[alu.instr.addr.rm]); 
          rf.write(alu.instr.addr.rd, rf[alu.instr.addr.rn] + rf[alu.instr.addr.rm]);
          //printf("rd %x\n", rf[alu.instr.addr.rd]);
          stats.numRegWrites++;
          checkHazard(alu.instr.addr.rn, alu.instr.addr.rm);
          //Flags
          stats.numRegReads+=2;
          break;
        case ALU_SUBR:
          setCarryOverflow(rf[alu.instr.subr.rn], rf[alu.instr.subr.rm], OF_SUB);
          setZeroNegative(rf[alu.instr.addr.rn] - rf[alu.instr.addr.rm]); 
          rf.write(alu.instr.subr.rd, rf[alu.instr.subr.rn] - rf[alu.instr.subr.rm]);
          stats.numRegWrites++;
          checkHazard(alu.instr.subr.rn, alu.instr.subr.rm);
          //Flags
          stats.numRegReads+=2;
          break;
        case ALU_ADD3I:
          // needs stats
          setCarryOverflow(rf[alu.instr.add3i.rn], alu.instr.add3i.imm, OF_ADD);
          setZeroNegative(rf[alu.instr.add3i.rn] + alu.instr.add3i.imm); 
          rf.write(alu.instr.add3i.rd, rf[alu.instr.add3i.rn] + alu.instr.add3i.imm);
          stats.numRegWrites++;
          checkHazard(alu.instr.add3i.rn);
          //Flags
          stats.numRegReads++;
          break;
        case ALU_SUB3I:
          setCarryOverflow(rf[alu.instr.sub3i.rn], alu.instr.sub3i.imm, OF_SUB);
          setZeroNegative(rf[alu.instr.sub3i.rn] - alu.instr.sub3i.imm); 
          rf.write(alu.instr.sub3i.rd, rf[alu.instr.sub3i.rn] - alu.instr.sub3i.imm);
          stats.numRegWrites++;
          checkHazard(alu.instr.sub3i.rn);
          //Flags
          stats.numRegReads++;
          break;
        case ALU_MOV:
          // needs stats
          setZeroNegative(alu.instr.mov.imm); 
          rf.write(alu.instr.mov.rdn, alu.instr.mov.imm);
          stats.numRegWrites++;
          //Flags
          break;
        case ALU_CMP: //changed this
          //Flags
          setCarryOverflow(rf[alu.instr.cmp.rdn], alu.instr.cmp.imm, OF_SUB);
          setZeroNegative(rf[alu.instr.cmp.rdn] - alu.instr.cmp.imm); 
          stats.numRegReads++;
          checkHazard(alu.instr.cmp.rdn);
          break;
        case ALU_ADD8I:
          // needs stats 
          setCarryOverflow(rf[alu.instr.add8i.rdn], alu.instr.add8i.imm, OF_ADD);
          setZeroNegative(rf[alu.instr.add8i.rdn] + alu.instr.add8i.imm); 
          rf.write(alu.instr.add8i.rdn, rf[alu.instr.add8i.rdn] + alu.instr.add8i.imm);
          stats.numRegWrites++;
          checkHazard(alu.instr.add8i.rdn);
          //Flags
          stats.numRegReads++; // added in by me 
          break;
        case ALU_SUB8I:
          setCarryOverflow(rf[alu.instr.sub8i.rdn], alu.instr.sub8i.imm, OF_SUB);
          setZeroNegative(rf[alu.instr.sub8i.rdn] - alu.instr.sub8i.imm); 
          rf.write(alu.instr.sub8i.rdn, rf[alu.instr.sub8i.rdn] - alu.instr.sub8i.imm);
          stats.numRegWrites++;
          checkHazard(alu.instr.sub8i.rdn);
          //Flags
          stats.numRegReads++; // added in by me 
          break;
        default:
          cout << "instruction not implemented" << endl;
          exit(1);
          break;
      }
      break;
    case BL: 
      // This instruction is complete, nothing needed here
      bl_ops = decode(blupper);
      if (bl_ops == BL_UPPER) {
        // PC has already been incremented above
        instr2 = imem[PC]; // is imem a memory read?
        BL_Type bllower(instr2);
        if (blupper.instr.bl_upper.s) {
          addr = static_cast<unsigned int>(0xff<<24) | 
            ((~(bllower.instr.bl_lower.j1 ^ blupper.instr.bl_upper.s))<<23) |
            ((~(bllower.instr.bl_lower.j2 ^ blupper.instr.bl_upper.s))<<22) |
            ((blupper.instr.bl_upper.imm10)<<12) |
            ((bllower.instr.bl_lower.imm11)<<1);
        }
        else {
          addr = ((blupper.instr.bl_upper.imm10)<<12) |
            ((bllower.instr.bl_lower.imm11)<<1);
        }
        // return address is 4-bytes away from the start of the BL insn
        rf.write(LR_REG, PC + 2);
        // Target address is also computed from that point
        rf.write(PC_REG, PC + 2 + addr);

        stats.numRegReads += 1; // changed by me
        stats.numRegWrites += 2; 
      }
      else {
        cerr << "Bad BL format." << endl;
        exit(1);
      }
      break;
    case DP:
      dp_ops = decode(dp);
      switch(dp_ops) {
        case DP_CMP:
          setCarryOverflow(rf[dp.instr.DP_Instr.rdn], rf[dp.instr.DP_Instr.rm], OF_SUB);
          setZeroNegative(rf[dp.instr.DP_Instr.rdn] - rf[dp.instr.DP_Instr.rm]); 
          stats.numRegReads+= 2;
          //checkHazard(dp.instr.DP_Instr.rdn);
          //implemented
          break;
        case DP_MUL:
         rf.write(dp.instr.DP_Instr.rdn, rf[dp.instr.DP_Instr.rdn] * rf[dp.instr.DP_Instr.rm]);
         setZeroNegative(rf[dp.instr.DP_Instr.rdn]); 
         stats.numRegReads+= 2;
         stats.numRegWrites++;
         //checkHazard(dp.instr.DP_Instr.rdn);
         break;
      }
      break;
    case SPECIAL:
      sp_ops = decode(sp);
      switch(sp_ops) {
        case SP_MOV:
          // needs stats
          rf.write((sp.instr.mov.d << 3 ) | sp.instr.mov.rd, rf[sp.instr.mov.rm]);
          stats.numRegWrites++;
          stats.numRegReads++;
          //Flag
          setZeroNegative(sp.instr.mov.d << 3  | sp.instr.mov.rd); 
          checkHazard(sp.instr.mov.rd << 3 | sp.instr.mov.rd, sp.instr.mov.rm);
          break;
        case SP_ADD:
          setCarryOverflow(rf[sp.instr.add.d << 3  | sp.instr.add.rd], rf[sp.instr.add.rm], OF_ADD);
          rf.write((sp.instr.add.d << 3 ) | sp.instr.add.rd, rf[sp.instr.add.d << 3 | sp.instr.add.rd] + rf[sp.instr.add.rm]);
          setZeroNegative(rf[sp.instr.add.d << 3  | sp.instr.add.rd] - rf[sp.instr.add.rm]); 
          stats.numRegWrites++;
          stats.numRegReads+= 2;
          checkHazard(sp.instr.add.rd << 3 | sp.instr.add.rd, sp.instr.add.rm);
          break;
        case SP_CMP:
          // need to implement these
          setCarryOverflow(rf[sp.instr.cmp.d << 3  | sp.instr.cmp.rd], rf[sp.instr.cmp.rm], OF_SUB);
          setZeroNegative(rf[sp.instr.cmp.d << 3  | sp.instr.cmp.rd] - rf[sp.instr.cmp.rm]); 
          stats.numRegReads+= 2;
          checkHazard(sp.instr.cmp.rd << 3 | sp.instr.cmp.rd, sp.instr.cmp.rm);
          break;
      }
      break;
    case LD_ST:
      // You'll want to use these load and store models
      // to implement ldrb/strb, ldm/stm and push/pop
      ldst_ops = decode(ld_st);
      switch(ldst_ops) {
        case STRI:
          // functionally complete, needs stats
          addr = rf[ld_st.instr.ld_st_imm.rn] + ld_st.instr.ld_st_imm.imm * 4;
          dmem.write(addr, rf[ld_st.instr.ld_st_imm.rt]);
          stats.numRegReads += 2;
          stats.numMemWrites++;
          checkEarly(ilist, ld_st.instr.ld_st_imm.rt);
          caches.access(addr);
          break;
        case LDRI:
          // functionally complete, needs stats
          addr = rf[ld_st.instr.ld_st_imm.rn] + ld_st.instr.ld_st_imm.imm * 4;
          //printf("%x\n", addr);
          rf.write(ld_st.instr.ld_st_imm.rt, dmem[addr]);
          //printf("%x\n", rf[ld_st.instr.ld_st_imm.rt]);
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.numMemReads++;
          hd.load = true;
          hd.defs = true;
          hd.defreg = ld_st.instr.ld_st_imm.rt;
          checkHazard(ld_st.instr.ld_st_imm.rn);
          caches.access(addr);
          break;
        case STRR:
          // implemented
          addr = rf[ld_st.instr.ld_st_reg.rn] + rf[ld_st.instr.ld_st_reg.rm];
          dmem.write(addr, rf[ld_st.instr.ld_st_reg.rt]);
          stats.numRegReads += 3; 
          stats.numMemWrites++;
          checkEarly(ilist, ld_st.instr.ld_st_imm.rt);
          caches.access(addr);
          break;
        case LDRR:
          // implemented
          addr = rf[ld_st.instr.ld_st_reg.rn] + rf[ld_st.instr.ld_st_reg.rm];
          rf.write(ld_st.instr.ld_st_reg.rt, dmem[addr]);
          stats.numRegWrites++;
          stats.numRegReads += 2;
          stats.numMemReads++;
          hd.load = true;
          hd.defs = true;
          hd.defreg = ld_st.instr.ld_st_imm.rt;
          checkHazard(ld_st.instr.ld_st_imm.rn, ld_st.instr.ld_st_imm.rn);
          caches.access(addr);
          break;
        case STRBI:
          // functionally complete, needs stats
          addr = rf[ld_st.instr.ld_st_imm.rn] + ld_st.instr.ld_st_imm.imm;
          temp = dmem[addr];
          temp.set_data_ubyte4(0, rf[ld_st.instr.ld_st_imm.rt]&0xFF);
          dmem.write(addr, temp);
          stats.numRegReads+=2;
          stats.numMemWrites++;
          stats.numMemReads++;
          checkEarly(ilist, ld_st.instr.ld_st_imm.rt);
          caches.access(addr);
          break;
        case LDRBI:
          // implemented
          addr = rf[ld_st.instr.ld_st_imm.rn] + ld_st.instr.ld_st_imm.imm;
          //printf("%x\n", addr);
          temp = dmem[addr];
          rf.write(ld_st.instr.ld_st_reg.rt, temp); 
          //Stats
          stats.numRegReads++;
          stats.numMemReads++;
          stats.numRegWrites++;
          hd.load = true;
          hd.defs = true;
          hd.defreg = ld_st.instr.ld_st_imm.rt;
          checkHazard(ld_st.instr.ld_st_imm.rn);
          caches.access(addr);
          break;
        case STRBR:
          // implemented
          addr = rf[ld_st.instr.ld_st_reg.rn] + rf[ld_st.instr.ld_st_reg.rm];
          temp = dmem[addr];
          temp.set_data_ubyte4(0, rf[ld_st.instr.ld_st_reg.rt]&0xFF);
          dmem.write(addr, temp);
          stats.numRegReads+=3;
          stats.numMemWrites++;
          stats.numMemReads++;
          checkEarly(ilist, ld_st.instr.ld_st_imm.rt);
          caches.access(addr);
          break;
        case LDRBR:
          // implemented
          addr = rf[ld_st.instr.ld_st_reg.rn] + rf[ld_st.instr.ld_st_reg.rm];
          temp = dmem[addr];
          rf.write(ld_st.instr.ld_st_reg.rt, temp); 
          stats.numRegReads+=2;
          stats.numMemReads++;
          stats.numRegWrites++;
          hd.load = true;
          hd.defs = true;
          hd.defreg = ld_st.instr.ld_st_imm.rt;
          checkHazard(ld_st.instr.ld_st_imm.rn, ld_st.instr.ld_st_reg.rm);
          caches.access(addr);
          break;
      }
      break;
    case MISC:
      misc_ops = decode(misc);
      switch(misc_ops) {
        case MISC_PUSH:
          // implemented
          // Getting Register Count
          int i, reg_count, mask;
          unsigned int returnTo;
          i = 0;
          reg_count = 0;
          mask = 1;
          for(i; i < 8; i ++){ //Check if other registers need to be pushed
            if(mask & misc.instr.push.reg_list){
               reg_count += 1;
            }
            mask = mask << 1;
          }
          stats.numRegReads++;
          //Pushing Register
          i = 0;
          mask = 1;
          if(mask & misc.instr.push.m){
             reg_count++;
          }
          addr = SP - (4 * reg_count);
          returnTo = addr;
          for(i; i < 8; i++){
            if(mask & misc.instr.push.reg_list){
               caches.access(addr);
               dmem.write(addr, rf[i]);  
               addr = addr + 4;
               stats.numRegReads++;
               stats.numMemWrites++;
            }
            mask = mask << 1;
          }
          if(1 & misc.instr.push.m){
             caches.access(addr);
             dmem.write(addr, LR); 
             stats.numMemWrites++;
             stats.numRegReads++;
             addr += 4;
          }
          rf.write(SP_REG, returnTo);
          stats.numRegWrites++;
          break;
        case MISC_POP:
          addr = SP;
          stats.numRegReads++;
          mask = 1;
          for(i = 0; i < 8; i++){
            if(mask & misc.instr.pop.reg_list){
               caches.access(addr);
               rf.write(i,dmem[addr]);
               addr += 4;
               stats.numRegWrites++;
               stats.numMemReads++;
            }
            mask = mask << 1;
          }
          if(1  & misc.instr.pop.m){
            caches.access(addr);
            rf.write(PC_REG, dmem[addr]);
            addr += 4;
            stats.numRegWrites++;
            stats.numMemReads++;
          }
          rf.write(SP_REG, addr);
          stats.numRegWrites++;
          stats.numRegReads++;
          break;
        case MISC_SUB:
          // functionally complete
          rf.write(SP_REG, SP - (misc.instr.sub.imm*4));
          stats.numRegWrites++;
          stats.numRegReads++;
          break;
        case MISC_ADD:
          // functionally complete
          rf.write(SP_REG, SP + (misc.instr.add.imm*4));
          stats.numRegWrites++;
          break;
        case MISC_UXTB:
          // unsigned int signExtend8to32ui(char i);
          // unsigned int mask2 = 255; 
          // unsigned int extended = signExtend8to32ui(rf[misc.instr.uxtb.rm]);
          // rf.write(misc.instr.uxtb.rd, mask2 & extended);
          rf.write(misc.instr.uxtb.rd, rf[misc.instr.uxtb.rm].data_ubyte4(0));
          stats.numRegWrites++;
          stats.numRegReads++;
          break;
      }
      break;
    case COND:
      decode(cond);
      // Once you've completed the checkCondition function,
      // this should work for all your conditional branches.
      int offset;
      offset = signExtend8to32ui(cond.instr.b.imm); 
      if (checkCondition(cond.instr.b.cond)){
         if(!btacSearch(vect, PC + 2 * signExtend8to32ui(cond.instr.b.imm) + 2)){
            vect = btacAdd(vect, PC + 2 * signExtend8to32ui(cond.instr.b.imm) + 2);
         //   printf("STATIC\n");
            if(offset >= 0)
               btac.state = 2;
            else
               btac.state = 3;
            stats.cycles += 3;
         }
         rf.write(PC_REG, PC + 2 * signExtend8to32ui(cond.instr.b.imm) + 2);
         stats.numRegWrites++;
         stats.numRegReads++;
         if(offset >= 0)
            stats.numForwardBranchesTaken++;
         else
            stats.numBackwardBranchesTaken++;
         if(btac.state < 4){
            btac.state++;
         }
         if(btac.state < 3){
            stats.cycles += 5;
           // printf("MISPREDICT1\n");
         }
      }
      else{
        if(offset > 0)
         stats.numForwardBranchesNotTaken++;
        
        else
         stats.numBackwardBranchesNotTaken++;
        if(btac.state > 1){
         btac.state -= 1;
        }
        if(btac.state > 2){
         stats.cycles += 5;
         //printf("MISPREDICT2\n");
        }
      } 
      stats.numBranches++;
      break;
    case UNCOND:
      // Essentially the same as the conditional branches, but with no
      // condition check, and an 11-bit immediate field
      decode(uncond);
      offset = signExtend11to32ui(uncond.instr.b.imm); 
      rf.write(PC_REG, PC + 2 * offset + 2);
      stats.numBranches++;
      stats.numRegReads++;
      stats.numRegWrites++;
      stats.cycles += 3;
      //printf("Uncond 3\n");
      break;
    case LDM:
      decode(ldm);
      // need to implement
      int writeback, reg_count;
      
      writeback = 0;
      reg_count = 0;
      addr = rf[ldm.instr.ldm.rn];
      stats.numRegReads++;
      mask = 1;
      for(i = 0; i < 8; i ++){ 
        if(mask & ldm.instr.ldm.reg_list){
           reg_count += 1;
        }
        mask = mask << 1;
      }
      mask = 1;
      for(i = 0; i < 8; i++){
         if(mask & ldm.instr.ldm.reg_list){
            caches.access(addr);
            rf.write(i,dmem[addr]);
            addr += 4;
            stats.numRegWrites++;
            stats.numMemReads++;
         }
         else if(i == ldm.instr.ldm.rn)
             writeback = 1;
             mask = mask << 1;
          }
          
      break;
    case STM:
      decode(stm);
      i = 0;
      reg_count = 0;
      mask = 1;
      //Getting Registers
      for(i; i < 8; i ++){ 
        if(mask & stm.instr.stm.reg_list){
           reg_count += 1;
        }
        mask = mask << 1;
      }
      //initial address
      addr = stm.instr.stm.rn - (4 * reg_count);
      stats.numRegReads++;
      i = 0;
      mask = 1;
      //storing
      for(i; i < 8; i++){
        if(mask & stm.instr.stm.reg_list){
           dmem.write(addr, rf[i]);  
           addr = addr + 4;
           stats.numRegReads++;
           stats.numMemWrites++;
           caches.access(addr);
        }
        mask = mask << 1;
      }
      rf.write(stm.instr.stm.rn, rf[stm.instr.stm.rn] + (4 * reg_count));
      stats.numRegReads++;
      stats.numRegWrites++;
      break;
    case LDRL:
      // This instruction is complete, nothing needed
      decode(ldrl);
      // Need to check for alignment by 4
      if (PC & 2) {
        addr = PC + 2 + (ldrl.instr.ldrl.imm)*4;
        stats.numRegReads++;
      }
      else {
        addr = PC + (ldrl.instr.ldrl.imm)*4;
        stats.numRegReads++;
      }
      // Requires two consecutive imem locations pieced together
      temp = imem[addr] | (imem[addr+2]<<16);  // temp is a Data32
      rf.write(ldrl.instr.ldrl.rt, temp);
      // One write for updated reg
      stats.numRegWrites++;
      // imem , including implementation of it
      stats.numMemReads++ ;
      break;
    case ADD_SP:
      // needs stats
      decode(addsp);
      rf.write(addsp.instr.add.rd, SP + (addsp.instr.add.imm*4));
      stats.numRegWrites++;
      stats.numRegReads++;
      checkHazard(addsp.instr.add.rd);
      break;
    default:
      cout << "[ERROR] Unknown Instruction to be executed" << endl;
      exit(1);
      break;
  }
  addList(hd, &ilist);
  stats.instrs++;
  stats.cycles++;
}
