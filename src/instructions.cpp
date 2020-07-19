#include "instructions.hpp"
#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>


instructions::instructions(unsigned int& inst, int* base_register_in, unsigned char* base_data_in, char* base_inst_in, unsigned int nPC_in){
    rs = ((inst>>21)&0x1F) + base_register_in;
    rt = ((inst>>16)&0x1F) + base_register_in;
    rd = ((inst>>11)&0x1F) + base_register_in;
    shamt = ((inst>>6)&0x1F);
    funct = (inst&0x3F);
    imm = (inst&0xFFFF);
    pseudo_address = (inst&0x3FFFFFF);
    base_inst = base_inst_in - 0x10000000;
    base_data = base_data_in - 0x20000000;
    nPC = nPC_in;
    reg31 = base_register_in + 31;
}

int instructions::SignExtImm(unsigned int i){
    if((i&0x8000)!=0){
        i = i|0xFFFF0000;
    }
    return i;
}


void instructions::ADD(){
    if(((*rs<0) && (*rt<0) && (*rs+*rt >=0)) || ((*rs >0) && (*rt >0) && (*rs+*rt <= 0))){
        std::cerr << "Overflow" << std::endl;
        std::exit(0xF6); //Overflow;
    }
    else{
        *rd = *rs + *rt;
        nPC = nPC + 4;
    }

    //MAKE SURE SHIFT IS 0?
}

void instructions::ADDI(){      /// IMM make sure to sign extend
    int i = SignExtImm(imm);
    if(((*rs<0) && (i<0) && (*rs+i >=0)) || ((*rs >0) && (i >0) && (*rs+i <= 0))){
        std::cerr << "Overflow" << std::endl;
        std::exit(0xF6); //Overflow;
    }
    else{
        *rt = *rs + i;
        nPC = nPC + 4;
    }
    //check to make sure that when add negative number, it doesn't show overflow, like ADD
}

void instructions::ADDIU(){
    *rt = *rs + SignExtImm(imm);
    nPC = nPC + 4;
}

void instructions::ADDU(){              //overflow is ignored. Same as ADD but without ignored overflow. Does this mean that it is not unsigned?
    *rd = *rs + *rt;
    nPC = nPC + 4;
}

void instructions::AND(){
    *rd = *rs & *rt;
    nPC = nPC + 4;
}

void instructions::ANDI(){
    *rt = *rs & imm;
    nPC = nPC + 4;
}

void instructions::BEQ(){
    int i = SignExtImm(imm);
    if(*rs==*rt){
        nPC = nPC + (i<<2);
    }
    else{
        nPC = nPC + 4;
    }
}

void instructions::BGEZ(){             //Signed or unsigned? Needs to be done for all branch instructions.      Do bit 16..20 need to be 0? (To specify register 0?)
    int i = SignExtImm(imm);
    if(*rs>=0){
        nPC = nPC + (i<<2);
    }
    else {
        nPC = nPC + 4;
    }
}

void instructions::BGEZAL(){
    int i = SignExtImm(imm);
    *reg31 = (nPC + 4);
    if(*rs>=0){
        nPC = nPC + (i<<2);
    }
    else {
        nPC = nPC + 4;
    }
}

void instructions::BGTZ(){
    int i = SignExtImm(imm);
    if(*rs>0){
        nPC = nPC + (i<<2);
    }
    else {
        nPC = nPC + 4;
    }
}

void instructions::BLEZ(){
    int i = SignExtImm(imm);
    if(*rs<=0){
        nPC = nPC + (i<<2);
    }
    else {
        nPC = nPC + 4;
    }
}

void instructions::BLTZ(){
    int i = SignExtImm(imm);
    if(*rs<0){
        nPC = nPC + (i<<2);
    }
    else {
        nPC = nPC + 4;
    }
}

void instructions::BLTZAL(){
    int i = SignExtImm(imm);
    *reg31 = nPC + 4;
    if(*rs<0){
        nPC = nPC + (i<<2);
    }
    else {
        nPC = nPC + 4;
    }
}

void instructions::BNE(){
    int i = SignExtImm(imm);
    if(*rs!=*rt){
        nPC = nPC + (i<<2);
    }
    else {
        nPC = nPC + 4;
    }
}

void instructions::DIV(int& LO, int& HI){
    if(*rt != 0){
        LO = *rs / *rt;
        HI = *rs % *rt;
    }
    nPC = nPC + 4;
}

void instructions::DIVU(int& LO, int& HI){
    if(*rt != 0){
        LO = unsigned(*rs) / unsigned(*rt);
        HI = unsigned(*rs) % unsigned(*rt);
    }
    nPC = nPC + 4;
}

void instructions::J(){
    nPC = (nPC&0xF0000000)|(pseudo_address<<2);                 //Why & nPC?
}

void instructions::JALR(){ 
    //rs and rd must not be equal --> undefined
    *rd = nPC + 4;
    nPC = unsigned(*rs);
}

void instructions::JAL(){
    *reg31 = nPC + 4;
    nPC = (nPC&0xF0000000)|(pseudo_address<<2);
}

void instructions::JR(){
    nPC = unsigned(*rs);
}

void instructions::LB(){           //MEMORY             ADD OUT OF RANGE AND READ INPUT
    //Add out of range
    int i = SignExtImm(imm);
    if((*rs + i >=0x10000000) && (*rs + i <0x11000000)){
        *rt = (int8_t)(*(base_inst+*rs+i));
        nPC = nPC + 4;
    }
    else if((*rs + i >=0x30000000) && (*rs + i <0x30000004)){ 
        if (((*rs + i) %4) == 3){
            *rt = (int8_t)(getchar());
        }
        else {
            int temp = getchar();
            *rt = 0;
        }
        if(std::cin.eof()|| feof(stdin)){   //!std::CIN.EOF
                *rt = -1;
        }
        if(std::ferror(stdin)){std::exit(-21);}
        nPC = nPC + 4;
    }
    else if(*rs + i>=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else{
        *rt =(int8_t)(*(base_data+*rs+i));
        nPC = nPC + 4;
    }
}


void instructions::LBU(){           //MEMORY
    int i = SignExtImm(imm);
    if((*rs + i >=0x10000000) && (*rs + i <0x11000000)){
        *rt = (uint8_t)(*(base_inst+*rs+i));
        nPC = nPC + 4;
    }
    else if((*rs + i >= 0x30000000) && (*rs + i <0x30000004)){        //How do you do this range.
        if (((*rs + i) %4) == 3){
            *rt = (uint8_t)(getchar());
        }
        else {
            int temp = getchar();
            *rt = 0;
        }
        if(std::cin.eof()|| feof(stdin)){   //!std::CIN.EOF
                *rt = 0xFF;
        }
        nPC = nPC + 4;
        if(std::ferror(stdin)){std::exit(-21);}
    }
    else if(*rs + i>=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else{
        *rt = *(base_data+*rs+i);
        nPC = nPC + 4;
    }
}

void instructions::LH() {            //MEMORY
    int i = SignExtImm(imm);
    if ((*rs + i)%2 != 0){
        std::cerr << "Unaligned" << std::endl;
        std::exit(-11);
    }
    if((*rs + i >=0x10000000) && (*rs + i <0x11000000)){
        *rt = (int8_t)(*(base_inst+*rs+i))<<8|(uint8_t)(*(base_inst+*rs+i+1));
        nPC = nPC + 4;
    }
    else if((*rs + i >= 0x30000000) && (*rs + i <0x30000004)){
        if (((*rs + i) %4) == 2){
            *rt = (int8_t)(getchar());
        }
        else {
            int temp = getchar();
            *rt = 0;
        }
        if(std::cin.eof() || feof(stdin)){
            *rt = -1; 
        }
        nPC = nPC + 4;
        if(std::ferror(stdin)){std::exit(-21);}
    }
    else if(*rs + i>=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else{
        *rt = (int8_t)(*(base_data+*rs+i))<<8|(*(base_data+*rs+i+1));
        nPC = nPC + 4;
    }
    //test for bus error?
}

void instructions::LHU(){ 
    int i = SignExtImm(imm);
    if ((*rs + i)%2 != 0){
        std::cerr << "Unaligned" << std::endl;
        std::exit(-11);
    }
    if((*rs + i >=0x10000000) && (*rs + i <0x11000000)){
        *rt = ((uint8_t)(*(base_inst+*rs+i))<<8)|(uint8_t)(*(base_inst+*rs+i+1));
        nPC = nPC + 4;
    }
    else if((*rs + i >= 0x30000000) && (*rs + i <0x30000004)){
        if (((*rs + i) %4) == 2){
            *rt = (uint8_t)(getchar());
        }
        else {
            int temp = getchar();
            *rt = 0;
        }
        if(std::cin.eof() || feof(stdin)){
            *rt = 0xFFFF; 
        }
        nPC = nPC + 4;
        if(std::ferror(stdin)){std::exit(-21);}
    }
    else if(*rs + i>=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else{
        *rt = ((uint8_t)(*(base_data+*rs+i))<<8)|(uint8_t)(*(base_data+*rs+i+1));
        nPC = nPC + 4;
    }
}

void instructions::LUI(){
    *rt = (imm<<16);
    nPC = nPC + 4;
}

void instructions::LW(){        //MEMORY
    int i = SignExtImm(imm);
    if((*rs+i)%4 != 0){
        std::cerr << "Unaligned" << std::endl;
        std::exit(-11); 
    }
    if((*rs + i >=0x10000000) && (*rs + i <0x11000000)){
        *rt = (uint8_t)(*(*rs + i + base_inst))<<24 | (uint8_t)(*(*rs + i + base_inst + 1))<<16 | (uint8_t)(*(*rs + i + base_inst + 2))<<8 | (uint8_t)(*(*rs + i + base_inst+3));
        nPC = nPC + 4;
    }
    else if(*rs + i == 0x30000000){
        *rt = int(getchar());
        nPC = nPC + 4;
        if(std::ferror(stdin)){std::exit(-21);}
    }
    else if(((*rs + i) >=  0x24000000)|| ((*rs + i) < 0x20000000)){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else{
        *rt = *(*rs + i + base_data)<<24 | *(*rs + i + base_data + 1)<<16 | *(*rs + i + base_data + 2)<<8 | *(*rs + i + base_data+3);
        nPC = nPC + 4;
    }
    //check for bus error?
}

void instructions::LWL(){       //MEMORY
    int i = SignExtImm(imm);
    int temp = (*rs+i)%4;

    if((*rs + i >=0x10000000) && (*rs + i <0x11000000)){
        switch(temp){
            case 3: {
                *rt = (*rt&0x00FFFFFF)|(uint8_t)(*(*rs+i+base_inst)&0xFF)<<24;
                break;
            }
            case 2: {
                *rt = (*rt&0x0000FFFF)|(uint8_t)(*(*rs+i+base_inst)&0xFF)<<24|(uint8_t)(*(*rs+i+base_inst+1)&0xFF)<<16;
                break;
            }
            case 1: {
                *rt = (*rt&0x000000FF)|(uint8_t)(*(*rs+i+base_inst)&0xFF)<<24|(uint8_t)(*(*rs+i+base_inst+1)&0xFF)<<16|(uint8_t)(*(*rs+i+base_inst+2)&0xFF)<<8;
                break;
            }
            case 0: {
                *rt = (uint8_t)(*(*rs + i + base_inst))<<24 | (uint8_t)(*(*rs + i + base_inst + 1))<<16 | (uint8_t)(*(*rs + i + base_inst + 2))<<8 | (uint8_t)(*(*rs + i + base_inst+3));
                break;
            }
        }
        nPC = nPC + 4;
    }
    else if((*rs + i >=0x30000000) && (*rs + i <0x30000004)){
        if((*rs + i)%4==0){
            *rt = (int8_t)(getchar());          ///Sign extend or not?
        }
        else {
            int getc = (int8_t)(getchar());
            switch((*rs + i)%4){
                case 3: {
                    *rt = (*rt&0x00FFFFFF)|((getc)<<24);
                    break;
                }
                case 2: {
                    *rt = (*rt&0x0000FFFF)|((getc)<<16);
                    break;
                }
                case 1: {
                    *rt = (*rt&0x000000FF)|((getc)<<8);
                    break;
                }
            }
        }
        nPC = nPC + 4;              ///WAS NOT HERE
        if(std::ferror(stdin)){std::exit(-21);}
    }
    else if(*rs + i>=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else{
        switch(temp){
            case 3: {
                *rt = (*rt&0x00FFFFFF)|(*(*rs+i+base_data)&0xFF)<<24;
                break;
            }
            case 2: {
                *rt = (*rt&0x0000FFFF)|(*(*rs+i+base_data)&0xFF)<<24|(*(*rs+i+base_data+1)&0xFF)<<16;
                break;
            }
            case 1: {
                *rt = (*rt&0x000000FF)|(*(*rs+i+base_data)&0xFF)<<24|(*(*rs+i+base_data+1)&0xFF)<<16|(*(*rs+i+base_data+2)&0xFF)<<8;
                break;
            }
            case 0: {
                *rt = *(*rs + i + base_data)<<24 | *(*rs + i + base_data + 1)<<16 | *(*rs + i + base_data + 2)<<8 | *(*rs + i + base_data+3);
                break;
            }
        }
        nPC = nPC + 4;
    }
    //check for bus error?
}

void instructions::LWR(){       //MEMORY
    int i = SignExtImm(imm);
    int temp = (*rs+i)%4;
     if((*rs + i >=0x10000000) && (*rs + i <0x11000000)){
        switch(temp){
            case 3: {
                *rt = ((*(*rs + i + base_inst-3)&0xFF)<<24) | (((*(*rs + i + base_inst - 2))&0xFF)<<16) |(((*(*rs + i + base_inst - 1))&0xFF)<<8) | ((*(*rs + i + base_inst))&0xFF);
                break;
            }
            case 2: {
                *rt = (*rt&0xFF000000)|(*(*rs+i+base_inst)&0xFF)|(*(*rs+i+base_inst-1)&0xFF)<<8|(*(*rs+i+base_inst-2)&0xFF)<<16;
                break;
            }
            case 1: {
                *rt = (*rt&0xFFFF0000)|(*(*rs+i+base_inst)&0xFF)|(*(*rs+i+base_inst-1)&0xFF)<<8;
                break;
            }
            case 0: {
                *rt = (*rt&0xFFFFFF00)|(*(*rs+i+base_inst)&0xFF);
                break;
            }
        }
        nPC = nPC + 4;
    }
    else if((*rs + i >= 0x30000000) && (*rs + i <0x30000004)){
        if(temp==3){
            *rt = (int8_t)(getchar());
        }
        else {
            int temp1 = getchar();
            switch(temp){
                case 2: {
                    *rt = (*rt&0xFF000000);
                    break;
                }
                case 1: {
                    *rt = (*rt&0xFFFF0000);
                    break;
                }
                case 0: {
                    *rt = (*rt&0xFFFFFF00);
                    break;
                }
            }
        }
        if(std::cin.eof()|| feof(stdin)){
            switch(temp){
                case 3: {
                    *rt = 0xFFFFFFFF;
                }
                case 2: {
                    *rt = (*rt&0xFF000000)|0xFFFFFF;
                    break;
                }
                case 1: {
                    *rt = (*rt&0xFFFF0000)|0xFFFF;
                    break;
                }
                case 0: {
                    *rt = (*rt&0xFFFFFF00)|0xFF;
                    break;
                }
            }
        }
        if(std::ferror(stdin)){std::exit(-21);}
        nPC = nPC + 4;
    }
    else if(*rs + i>=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else{
        switch(temp){
            case 3: {
                *rt = (*(*rs + i + base_data-3)&0xFF)<<24 | (*(*rs + i + base_data- 2)&0xFF)<<16 | (*(*rs + i + base_data - 1)&0xFF)<<8 | (*(*rs + i + base_data)&0xFF);
                break;
            }
            case 2: {
                *rt = (*rt&0xFF000000)|(*(*rs+i+base_data)&0xFF)|(*(*rs+i+base_data-1)&0xFF)<<8|(*(*rs+i+base_data-2)&0xFF)<<16;
                break;
            }
            case 1: {
                *rt = (*rt&0xFFFF0000)|(*(*rs+i+base_data)&0xFF)|(*(*rs+i+base_data-1)&0xFF)<<8;
                break;
            }
            case 0: {
                *rt = (*rt&0xFFFFFF00)|(*(*rs+i+base_data)&0xFF);
                break;
            }
        }
        nPC = nPC + 4;
    }
    //check for bus error?
}

void instructions::MFHI(int& HI){
    *rd = HI;
    nPC = nPC + 4;
}

void instructions::MFLO(int& LO){
    *rd = LO;
    nPC = nPC + 4;
}

void instructions::MTHI(int& HI){
    HI = *rs;
    nPC = nPC + 4;
}

void instructions::MTLO(int& LO){
    LO = *rs;
    nPC = nPC + 4;
}

void instructions::MULT(int& HI,int& LO){
    int64_t reg_rs = *rs;
    int64_t reg_rt = *rt;
    int64_t temp = (reg_rs * reg_rt);
    HI = (temp>>32);
    LO = (temp<<32)>>32;
    nPC = nPC + 4;
    //check for underflow, because of multiplying 2 negative numbers
}

void instructions::MULTU(int& HI, int& LO){
    uint64_t reg_rs = (uint32_t)(*rs);
    uint64_t reg_rt = (uint32_t)(*rt);
    uint64_t temp = (reg_rs * reg_rt);
    HI = (temp>>32);
    LO = (temp<<32)>>32;
    nPC = nPC + 4;
}

void instructions::OR(){
   *rd = *rs|*rt;
   nPC = nPC + 4;
}

void instructions::ORI(){
    *rt = *rs|imm;
    nPC = nPC + 4;
}

void instructions::SB(){              //MEMORY
    int i = SignExtImm(imm);
    if((*rs + i >= 0x30000004) && (*rs + i <0x30000008)){        //How do you do this range.
        if ((*rs+i)%4 == 3) {
            putchar(*rt & 0xFF);
        }
        else {
            putchar(0);
        }
        if(std::ferror(stdout)){std::exit(-21);}
        nPC = nPC + 4;
    }
    else if(*rs + i >=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else {
        *(base_data+*rs+i) = *rt & 0xFF;
        nPC = nPC + 4;
    }
}

void instructions::SH(){              //MEMORY
    int i = SignExtImm(imm);
    if ((*rs + i)%2 != 0){
        std::cerr << "Unaligned" << std::endl;
        std::exit(-11);
    }
    else if((*rs + i >= 0x30000004) && (*rs + i <0x30000008)){        //How do you do this range.
        if ((*rs+i)%4 == 2) {
            putchar(*rt & 0xFF);
        }
        else {
            putchar(0);
        }
        if(std::ferror(stdout)){std::exit(-21);}
        nPC = nPC + 4;
    }
    else if(*rs + i >=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else{
        *(base_data+*rs+i+1) = *rt & 0xFF;
        *(base_data+*rs+i) = (*rt>>8) & 0xFF;
        nPC = nPC + 4;
    }
}

void instructions::SLL(){
    *rd = *rt << shamt;
    nPC = nPC + 4;
}

void instructions::SLLV(){
    *rd = *rt << *rs;
    nPC = nPC + 4;
}

void instructions::SLT(){
    if(*rs<*rt){
        *rd = 1;
        nPC = nPC + 4;
    }
    else{
        *rd = 0;
        nPC = nPC + 4;
    }
}

void instructions::SLTI(){
    if(*rs<SignExtImm(imm)){
        *rt = 1;
        nPC = nPC + 4;
    }
    else{
        *rt = 0;
        nPC = nPC + 4;
    }
}

void instructions::SLTIU(){
    int i = SignExtImm(imm);
    if(unsigned(*rs)<unsigned(i)){
        *rt = 1;
        nPC = nPC + 4;
    }
    else{
        *rt = 0;
        nPC = nPC + 4;
    }
}

void instructions::SLTU(){
    if(unsigned(*rs)<unsigned(*rt)){
        *rd = 1;
        nPC = nPC + 4;
    }
    else{
        *rd = 0;
        nPC = nPC + 4;
    }
}

void instructions::SRA(){
    *rd = *rt >> shamt; //$d = $t >> h, but sign bit is shifted in
    nPC = nPC + 4;
}

void instructions::SRAV(){
    *rd = *rt >> *rs;
    nPC = nPC + 4;
}

void instructions::SRL(){
    *rd = unsigned(*rt) >> shamt; //$d = $t >> h, but zeroes are shifted in
    nPC = nPC + 4;
}

void instructions::SRLV(){
    *rd = unsigned(*rt) >> *rs; //$d = $t >> h, but zeroes are shifted in
    nPC = nPC + 4;
}

void instructions::SUB(){
    int temp = *rs - *rt;
    if((*rs < 0 && *rt >= 0 && temp>=0) || (*rs >= 0 && *rt<0 && temp<0)){
        std::exit(0xF6); //Overflow
    }
    *rd = *rs - *rt;
    nPC = nPC + 4;
}

void instructions::SUBU(){
    *rd = *rs - *rt;
    nPC = nPC + 4;
}

void instructions::SW(){              //MEMORY
    int i = SignExtImm(imm);
    if((*rs + i)%4 != 0){
        std::cerr << "Unaligned" << std::endl;
        std::exit(-11);
    }
    if((*rs + i) == 0x30000004){        //How do you do this range.
        putchar(*rt & 0xFF);
        nPC = nPC + 4;
        if(std::ferror(stdout)){std::exit(-21);}
    }
    else if(*rs + i >=  0x24000000|| *rs + i < 0x20000000){
        std::cerr << "Out of range" << std::endl;
        std::exit(-11);
    }
    else {
        *(base_data+*rs+i+3) = *rt & 0xFF;
        *(base_data+*rs+i+2) = (uint8_t)(((*rt>>8) & 0xFF));
        *(base_data+*rs+i+1) = (uint8_t)((*rt>>16) & 0xFF);
        *(base_data+*rs+i) = (*rt>>24) & 0xFF;
        nPC = nPC + 4;
    }
}

void instructions::XOR(){
    *rd = *rs ^ *rt;
    nPC = nPC + 4;
}

void instructions::XORI(){
    *rt = *rs ^ imm;
    nPC = nPC + 4;
}
