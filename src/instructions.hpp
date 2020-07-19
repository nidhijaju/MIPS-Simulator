#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

class instructions{
    public:
        instructions(unsigned int& inst, int* base_register, unsigned char* base_data_in, char* base_inst_in, unsigned int nPC_in);
        instructions();
        int* rs;
        int* rt;
        int* rd;
        unsigned char shamt;
        unsigned char funct;
        unsigned int imm;
        unsigned int pseudo_address;
        char* base_inst;
        unsigned char* base_data;
        unsigned int nPC;
        int* reg31;

        int SignExtImm(unsigned int i);



        void ADD();
        void ADDI();
        void ADDIU();
        void ADDU();
        void AND();
        void ANDI();
        void BEQ();
        void BGEZ();
        void BGEZAL();
        void BGTZ();
        void BLEZ();
        void BLTZ();
        void BLTZAL();
        void BNE();
        void DIV(int& LO, int& HI);
        void DIVU(int& LO, int& HI);
        void J();
        void JALR();
        void JAL();
        void JR();
        void LB();
        void LBU();
        void LH();
        void LHU();
        void LUI();
        void LW();
        void LWL();
        void LWR();
        void MFHI(int& HI);
        void MFLO(int& LO);
        void MTHI(int& HI);
        void MTLO(int& LO);
        void MULT(int& HI, int& LO);
        void MULTU(int& HI, int& LO);
        void OR();
        void ORI();
        void SB();
        void SH();
        void SLL();
        void SLLV();
        void SLT();
        void SLTI();
        void SLTIU();
        void SLTU();
        void SRA();
        void SRAV();
        void SRL();
        void SRLV();
        void SUB();
        void SUBU();
        void SW();
        void XOR();
        void XORI();

};

#endif

//Does ADDU have overflow
//Is the imm always positiv? 6 bit signed?
