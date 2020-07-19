#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

using u8bit = uint8_t;
using s8bit = int8_t;

#include "memorymap.hpp"

#include "instructions.hpp"

struct special_registers{
    unsigned int PC = 0x10000000;
    unsigned int nPC = 0x10000004;
    unsigned int EPC;
    int HI = 0;
    int LO = 0;
    unsigned int Exception = 0;
};

void loadfile(std::string filename){
    double size;      //std::streampos 
    char* memblock = &ADDR_INSTR[0];

    std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        if(size>0x1000000){
            std::cerr << "Error, too many instructions" << std::endl;
            exit(-11);
        } 
        if(size == 0){
            exit(-21);
        }
        //check that the size is smaller than what it is supposed to be      
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();
    }
    else{
        std::cerr << "Error, can't open input file" << std::endl;
        exit(-21);
    }
}

void r_syntax_check(unsigned int inst){
    switch(inst&0x3F){
        case 0x20:{if(((inst>>6)&(0x1F)) != 0 ){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}
        case 0x21:{if(((inst>>6)&(0x1F)) != 0){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}
        case 0x24:{if(((inst>>6)&(0x1F)) != 0 ){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}
        case 0x1A:{if(((inst>>5)&(0x7FF)) != 0){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}
        case 0x1B:{if(((inst>>5)&(0x7FF)) != 0){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}
        case 0x08:{if(((inst>>4)&(0x1FFFF)) != 0 ){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}
        case 0x10:{if((((inst>>6)&(0x1F)) != 0) || (((inst>>16)&(0x3FF)) != 0)){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}
        case 0x12:{if((((inst>>6)&(0x1F)) != 0) || (((inst>>16)&(0x3FF)) != 0)){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}
        case 0x18:{if(((inst>>6)&(0x3FF)) != 0 ){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;} //MULT
        case 0x11:{if(((inst>>6)&(0x7FFF)) != 0){std::cerr << "Invalid syntax" << std::hex << inst<< std::endl; std::exit(-12);} break;} //MTHI
        case 0x13:{if(((inst>>6)&(0x7FFF)) != 0){std::cerr << "Invalid syntax8" << std::endl; std::exit(-12);} break;} //MTLO
        case 0x19:{if(((inst>>6)&(0x3FF)) != 0 ){std::cerr << "Invalid syntax7" << std::endl; std::exit(-12);} break;} //MULTU
        case 0x25:{if(((inst>>6)&(0x1F)) != 0 ){std::cerr << "Invalid syntax10" << std::endl; std::exit(-12);} break;} //OR
        case 0x2A:{if(((inst>>6)&(0x1F)) != 0 ){std::cerr << "Invalid syntax5" << std::endl; std::exit(-12);} break;} //SLT
        case 0x2B:{if(((inst>>6)&(0x1F)) != 0 ){std::cerr << "Invalid syntax4" << std::endl; std::exit(-12);} break;} //SLTU
        case 0x06:{if(((inst>>6)&(0x1F)) != 0 ){std::cerr << "Invalid syntax3" << std::endl; std::exit(-12);} break;} //SRLV
        case 0x22:{if(((inst>>6)&(0x1F)) != 0){std::cerr << "Invalid syntax2" << std::endl; std::exit(-12);} break;} //SUB
        case 0x23:{if(((inst>>6)&(0x1F)) != 0){std::cerr << "Invalid syntax1" << std::endl; std::exit(-12);} break;} //SUBU

    }

}

void i_syntax_check(unsigned int inst){
    switch((inst>>26)&0x3F){
        case 0x07:{if(((inst>>16)&(0x1F)) != 0){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;} //BGTZ
        case 0x06:{if(((inst>>16)&(0x1F)) != 0){std::cerr << "Invalid syntax" << std::endl; std::exit(-12);} break;}//BLEZ
    }

}

// void bindisplay(unsigned int n, int s){
//     int t = 1<<(s-1);
//     for(int i = 0 ; i < s ; i++){
//         std::cerr << int((n&(t))>>(s-1));
//         n = n << 1;
//     }  
//      std::cerr << std::endl;
// }

int main(int argc, char* argv[]){
    special_registers sp_reg;
    //Read Binary and place in instruction memory + Set PC to first location
    std::string filename;
    if(argc == 2){
        filename = argv[1];
    }
    loadfile(filename);
        

    unsigned int inst_code = 0;

    while(sp_reg.PC != 0){
        //Check if instruction is out of range
        if(sp_reg.PC>=0x11000000 || sp_reg.PC<0x10000000 || sp_reg.PC%4 != 0){
            //Out of range
            std::cerr << "ERROR -11" << std::endl;
            exit(-11);
            //Should invalid instruction be specified else where?
        }
        
        unsigned int inst_code = int(((ADDR_INSTR[sp_reg.PC-0x10000000]&0xFF) << 24)|((ADDR_INSTR[sp_reg.PC+1-0x10000000]&0xFF) << 16)|((ADDR_INSTR[sp_reg.PC+2-0x10000000]&0xFF) << 8)|(ADDR_INSTR[sp_reg.PC+3-0x10000000]&0xFF));
        instructions inst(inst_code,&ADDR_REGISTER[0], &ADDR_DATA[0], &ADDR_INSTR[0], sp_reg.nPC);// defined an object of the class

        sp_reg.PC = sp_reg.nPC;

        char opcode = ((inst_code>>26)&0x3F);

        if(opcode == 0x00){
            r_syntax_check(inst_code);
            switch(inst.funct){
                case 0x20: {
                    inst.ADD();                    
                    break;
                }
                case 0x21: {
                    inst.ADDU();                    
                    break;
                }
                case 0x24: {
                    inst.AND();                    
                    break;
                }
                case 0x1A: {
                    inst.DIV(sp_reg.LO, sp_reg.HI);                    
                    break;
                }
                case 0x1B: {
                    inst.DIVU(sp_reg.LO, sp_reg.HI);                    
                    break;
                }
                case 0x08: {
                    inst.JR();
                    break;
                }
                case 0x09: {
                    inst.JALR();
                    break;
                }
                case 0x10: {
                    inst.MFHI(sp_reg.HI);                    
                    break;
                }
                case 0x12: {
                    inst.MFLO(sp_reg.LO);                    
                    break;
                }
                case 0x11: {
                    inst.MTHI(sp_reg.HI);                    
                    break;
                }
                case 0x13: {
                    inst.MTLO(sp_reg.LO);                    
                    break;
                }
                case 0x18: {
                    inst.MULT(sp_reg.HI,sp_reg.LO);                    
                    break;
                }
                case 0x19: {
                    inst.MULTU(sp_reg.HI,sp_reg.LO);                    
                    break;
                }
                case 0x25: {
                    inst.OR();                    
                    break;
                }
                case 0x00: {
                    inst.SLL();                    
                    break;
                }
                case 0x04: {
                    inst.SLLV();                    
                    break;
                }
                case 0x2A: {
                    inst.SLT();                    
                    break;
                }
                case 0x2B: {
                    inst.SLTU();                    
                    break;
                }
                case 0x03: {
                    inst.SRA();                    
                    break;
                }
                case 0x07: {
                    inst.SRAV();
                    break;
                }
                case 0x02: {
                    inst.SRL();                    
                    break;
                }
                case 0x06: {
                    inst.SRLV();                    
                    break;
                }
                case 0x22: {
                    inst.SUB();                    
                    break;
                }
                case 0x23: {
                    inst.SUBU();                    
                    break;
                }
                case 0x26: {
                    inst.XOR();
                    break;
                }
                default:{
                    std::cerr << "Invalid instruction" << std::endl;
                    exit(-12);
                    break;
                }
                
            }

        }
        //else if (opcode == 0x02 || opcode == 0x03){
            //J-type
        //}
        else{
            i_syntax_check(inst_code);
            //I-type    // and J-type?
            switch(opcode){
                case 0x08:{
                    inst.ADDI();
                    break;
                }
                case 0x09:{
                    inst.ADDIU();
                    break;
                }
                case 0x0C:{
                    inst.ANDI();
                    break;
                }
                case 0x04:{
                    inst.BEQ();
                    break;
                }
                case 0x01:{
                    if((inst_code&0x110000)==0x010000){
                        inst.BGEZ();
                    }
                    else if((inst_code&0x110000)==0x110000){
                        inst.BGEZAL();
                    }
                    else if((inst_code&0x110000)==0x000000){
                        inst.BLTZ();
                    }
                    else if((inst_code&0x110000)==0x100000){
                        inst.BLTZAL();
                    }
                    else{
                        std::cerr << "Invalid instruction" << std::endl;
                        exit(-12);
                    }
                    break;
                }
                case 0x07:{
                    inst.BGTZ();
                    break;
                }
                 case 0x06:{
                    inst.BLEZ();
                    break;
                }
                 case 0x05:{
                    inst.BNE();
                    break;
                }
                case 0x02:{             
                    inst.J();
                    break;
                }
                case 0x03:{
                    inst.JAL();
                    break;
                }
                case 0x20:{
                    inst.LB();                    
                    break;
                }
                case 0x24:{
                    inst.LBU();
                    break;
                }
                case 0x21:{
                    inst.LH();
                    break;
                }
                case 0x25:{
                    inst.LHU();
                    break;
                }
                case 0x0F:{
                    inst.LUI();                    
                    break;
                }
                case 0x23:{
                    inst.LW();                    
                    break;
                }
                case 0x22:{
                    inst.LWL();
                    break;
                }
                case 0x26:{
                    inst.LWR();
                    break;
                }
                case 0x0D:{
                    inst.ORI();                    
                    break;
                }
                case 0x28:{
                    inst.SB();                    
                    break;
                }
                case 0x29:{
                    inst.SH();
                    break;
                }
                case 0x0A:{
                    inst.SLTI();                    
                    break;
                }
                case 0x0B:{
                    inst.SLTIU();                    
                    break;
                }
                case 0x2B:{
                    inst.SW();
                    break;
                }
                case 0x0E:{
                    inst.XORI();
                    break;
                }
                default:{
                    std::cerr << "Invalid instruction" << std::endl;
                    exit(-12);
                    break;
                }               
            }
        }

        sp_reg.nPC = inst.nPC;

        /*
            check at the end if there are any execptions stored in the class
            Link the class exeption to the special registers

        */
       ADDR_REGISTER[0] = 0;
       //print_registers();
    }
    std::exit(ADDR_REGISTER[2]);
    
}
