#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc = 0x0;

char memory[8*1024];    // only 8KB of memory located at address 0

void emitError(string s)
{
    cout << s;
    exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW){
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

string abi(unsigned int reg)
{
    string r;
    switch (reg)
    {
        case 0:
            //cout << "zero";
            r = "zero";
            break;
        case 1:
            //cout << "ra";
            r = "ra";
            break;
        case 2:
            //cout << "sp";
            r = "sp";
            break;
        case 3:
            //cout << "gp";
            r = "gp";
            break;
        case 4:
            //cout << "tp";
            r = "tp";
            break;
        case 5:
            //cout << "t0";
            r = "t0";
            break;
        case 6:
            //cout << "t1";
            r = "t1";
            break;
        case 7:
            //cout << "t2";
            r = "t2";
            break;
        case 8:
            //cout << "s0";
            r = "s0";
            break;
        case 9:
            //cout << "s1";
            r = "s1";
            break;
        case 10:
            //cout << "a0";
            r = "a0";
            break;
        case 11:
            //cout << "a1";
            r = "a1";
            break;
        case 12:
            //cout << "a2";
            r = "a2";
            break;
        case 13:
            //cout << "a3";
            r = "a3";
            break;
        case 14:
            //cout << "a4";
            r = "a4";
            break;
        case 15:
            //cout << "a5";
            r = "a5";
            break;
        case 16:
            //cout << "a6";
            r = "a6";
            break;
        case 17:
            //cout << "a7";
            r = "a7";
            break;
        case 18:
            //cout << "s2";
            r = "s2";
            break;
        case 19:
            //cout << "s3";
            r = "s3";
            break;
        case 20:
            //cout << "s4";
            r = "s4";
            break;
        case 21:
            //cout << "s5";
            r = "s5";
            break;
        case 22:
            //cout << "s6";
            r = "s6";
            break;
        case 23:
            //cout << "s7";
            r = "s7";
            break;
        case 24:
            //cout << "s8";
            r = "s8";
            break;
        case 25:
            //cout << "s9";
            r = "s9";
            break;
        case 26:
            cout << "s10";
            r = "s10";
            break;
        case 27:
            //cout << "s11";
            r = "s11";
            break;
        case 28:
            //cout << "t3";
            r = "t3";
            break;
        case 29:
            //cout << "t4";
            r = "t4";
            break;
        case 30:
            //cout << "t5";
            r = "t5";
            break;
        case 31:
            //cout << "t6";
            r = "t6";
            break;
    }
    return r;
}

void instDecExec(unsigned int instWord)
{
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;
    funct7 = 0;
    
    unsigned int instPC = pc - 4;
    
    opcode = instWord & 0x0000007F;
    if ( opcode == 0x33 || (opcode == 0x13) || opcode == 0x3 || opcode == 0x67 || opcode == 0x63 || opcode == 0x37 || opcode == 0x17 || opcode == 0x6F )
    { rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    rs1 = (instWord >> 15) & 0x0000001F;
        rs2 = (instWord >> 20) & 0x0000001F; }
    
    // — inst[31] — inst[30:25] inst[24:21] inst[20]
    I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
    
    U_imm = ((instWord >> 12) & 0xFFFFF000); //| (((instWord >> 31) ? 0xFFFFF800/*0xFFF*/ : 0x0));
    
    J_imm = ((instWord >> 12) & 0xFFFFF); //| (((instWord >> 31) ? 0xFFFFF800/*0xFFF00000*/ : 0x0));
    
    B_imm = ((instWord >> 7) & 0x781); //| (((instWord >> 11) ? 0x000007FF : 0x0));
    unsigned int temp_b;
    temp_b = ((instWord >> 25) & 0xFD); //| (((instWord >> 31) ? 0xFFFFF800 : 0x0));
    
    B_imm = B_imm + temp_b;
    

    
    printPrefix(instPC, instWord);
    
    if(opcode == 0x33){        // R Instructions
        switch(funct3){
            case 0: if(funct7 == 32) {
                cout << "\tSUB\tx" << abi(rd) << ", x" << abi(rs1) << ", x" << rs2 << "\n";
            }
            else {
                cout << "\tADD\tx" << abi(rd) << ", x" << abi(rs1) << ", x" << abi(rs2) << "\n";
            }
                break;
            default:
                cout << "\tUnkown R Instruction\n";
        }
    } else if(opcode == 0x13){    // I instructions
        switch(funct3){
            case 0:    cout << "\tADDI\tx" << abi(rd) << ", x" << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
                break;
            default:
                cout << "\tUnkown I Instruction\n ";
        }
    }
    else if(opcode == 0x3)  // I
    { switch(funct3) {
        case 0: cout << "\tLB\tx" << abi(rd) << ", " << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
            break;
        case 1: cout << "\tLH\tx" << abi(rd) << ", " << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
            break;
        case 2: cout << "\tLW\tx" << abi(rd) << ", " << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
            break;
        default:
            cout << "\tUnkown I Instruction\n ";
    }
    }
    
    else if(opcode == 0x67) // I
    {
        cout << "\tJALR\tx" << abi(rd) << ", x" << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
    }
    else if(opcode == 0x63)
    {
        switch(funct3){
            case 0: cout << "\tBEQ\tx" << abi(rs1) << ", x" << abi(rs2) << ", " << hex << "0x" << (int)B_imm << "\n";
                break;
            case 1: cout << "\tBNE\tx" << abi(rs1) << ", x" << abi(rs2) << ", " << hex << "0x" << (int)B_imm << "\n";
                break;
            case 4: cout << "\tBLT\tx" << abi(rs1) << ", x" << abi(rs2) << ", " << hex << "0x" << (int)B_imm << "\n";
                break;
            case 5: cout << "\tBGE\tx" << abi(rs1) << ", x" << abi(rs2) << ", " << hex << "0x" << (int)B_imm << "\n";
                break;
            case 6: cout << "\tBLTU\tx" << abi(rs1) << ", x" << abi(rs2) << ", " << hex << "0x" << (int)B_imm << "\n";
                break;
            case 7: cout << "\tBGEU\tx" << abi(rs1) << ", x" << abi(rs2) << ", " << hex << "0x" << (int)B_imm << "\n";
                break;
            default:
                cout << "\tUnkown B Instruction\n ";
        }
    }
    else if(opcode == 0x37 || opcode == 0x17) //U
    {   if(opcode == 0x37)
        cout << "\tLUI\tx" << abi(rd) << hex << " 0x" << (int)U_imm << "\n";
        else
        cout << "\tAUIPC\tx" << abi(rd) << hex << " 0x" << (int)U_imm << "\n";
    }
    else if (opcode == 0x6F) // J
    {
        cout << "\tJAL\tx" << abi(rd) << hex << " 0x" << (int)J_imm << "\n";
    }
    
    
    
    unsigned int opcode_compressed;
    unsigned int CI_imm, CJ_imm, CL_imm, CB_imm, ci_imm, CR_imm;
    
    opcode_compressed = instWord & 0x3;
    if(opcode == 0x00)
    {  rd = ((instWord >> 2) & 0x00000007);
        funct3 = ((instWord >> 13) & 0x00000007);
        unsigned int CIW_imm;
        CIW_imm = ((instWord >> 2) & 0xFF);
        rs1 = ((instWord >> 7) & 0x00000007);
        rs2 = ((instWord >> 2) & 0x00000007);
        CL_imm = ((instWord >> 5) & 0x11);
        unsigned int tempCL_imm ;
       tempCL_imm = ((instWord >> 10) & 0x7);
        CL_imm = CL_imm + tempCL_imm ;
        
        switch(funct3)
        {
            case 0: cout << "/tC.ADDI4SPN/tx" << abi(rd) << hex << ", 0x" << (int)CIW_imm << "\n";
                break;
            case 2: cout << "/tC.LW/tx" << abi(rd) << hex << ", 0x" << (int)CIW_imm << "(" << abi(rs1) << ")" << "\n";
                break;
            case 6: cout << "/tC.SW/tx" << abi(rs2) << hex << ", 0x" << (int)CIW_imm << "(" << abi(rs1) << ")" << "\n";
                break;
            default:
                cout << "\tUnkown Compressed Instruction\n ";
        }
    }
    if(opcode == 0x1)
    {
        unsigned int tempCI_imm, funct2, Other_funct2, other_rs1, CB_imm, temp_cb ;
        CI_imm = ((instWord >> 2) & 0x1F);
        tempCI_imm = ((instWord >> 12) & 0x1) ;
        CI_imm = CI_imm + tempCI_imm ;
        
        funct3 = ((instWord >> 13) & 0x00000007);
        
        rd = (instWord >> 7) & 0x0000001F;
        
        other_rs1 = (instWord >> 7) & 0x7;
        rs1 = (instWord >> 7) & 0x0000001F;
        
        CJ_imm = (instWord >> 2) & 0x7FF;
        
        funct2 = (instWord >> 10) & 0x3;
        Other_funct2 = (instWord >> 5) & 0x3;
        
        rs2 = (instWord >> 2) & 0x7;
        
        CB_imm = (instWord >> 2) & 0x67;
        temp_cb = (instWord >> 10) & 0x31;
        CB_imm = CB_imm + temp_cb ;
        switch(funct3){
                
            case 0: if (rd == 0 || rs1 == 0)
                    cout << "/tNOP/t" << "\n";
                    else
                    cout << "/tC.ADDI/tx" << abi(rd) << ", x" << abi(rd) << ", " << hex << "0x" << (int)CI_imm << "\n";
                break;
            case 1: cout << "\tC.JAL\tx1" << ", 0x" << (int)J_imm << "\n";
                break;
            case 2:
                cout << "\tC.LI\tx" << abi(rd) << ", 0x" << (int)J_imm << "\n";
                
                break;
            case 3:
                if(rd == 2)
                cout << "\tC.ADDI16SP\tx" << 2 << hex << ", 0x" << (int)CI_imm << "\n";
                else
                    cout << "\tC.LUI\tx" << abi(rd) << hex << ", 0x" << (int)CI_imm << "\n";
                break;
            case 4: switch(funct2)
                        {
                            case 0: cout << "\tC.SRLI\tx" << abi(rd) << ", x" << abi(rd) << hex << ", 0x" << (int)CI_imm << "\n";
                                break;
                            case 1:  cout << "\tC.SRAI\tx" << abi(rd) << ", x" << abi(rd) << hex << "0x" << (int)CI_imm << "\n";
                                break;
                            case 2: cout << "\tC.ANDI\tx" << abi(rd) << hex << "0x" << (int)CI_imm << "\n";
                                break;
                            case 3: switch(Other_funct2)
                                {
                                    case 0: cout << "\tC.SUB\tx" << abi(other_rs1) << ", x" << abi(other_rs1) << ", x" << abi(rs2)  << "\n";
                                        break;
                                    case 1: cout << "\tC.XOR\tx" << abi(other_rs1) << ", x" << abi(other_rs1) << ", x" << abi(rs2)  << "\n";
                                        break;
                                    case 2: cout << "\tC.OR\tx" << abi(other_rs1) << ", x" << abi(other_rs1) << ", x" << abi(rs2)  << "\n";
                                        break;
                                        
                                    case 3: cout << "\tC.AND\tx" << abi(other_rs1) << ", x" << abi(other_rs1) << ", x" << abi(rs2)  << "\n";
                                        break;
                                    default:
                                        cout << "\tUnkown Compressed Instruction\n ";
                                }
                            default:
                                cout << "\tUnkown Compressed Instruction\n ";
                    }
                break;
                
            case 5: cout << "\tC.JAL\tx0" << ", 0x" << (int)J_imm << "\n";
                
                break;
            case 6: cout << "\tBEQZ\tx" << abi(rs1) << ", x0, " << hex << "0x" << (int)CB_imm << "\n";
                
                break;
            case 7: cout << "\tBNEZ\tx" << abi(rs1) << ", x0, " << hex << "0x" << (int)CB_imm << "\n";
            
            default:
                cout << "\tUnkown Compressed Instruction\n ";
        }
    }
   
    else {
        cout << "\tUnkown Instruction\n ";
    }
    
}

int main(int argc, char *argv[]){
    
    unsigned int instWord=0;
    ifstream inFile;
    ofstream outFile;
    
    if(argc<2) emitError("use: rvcdiss <machine_code_file_name>");
    
    inFile.open(argv[1], ios::in | ios::binary | ios::ate);
    
    if(inFile.is_open())
    {
        int fsize = inFile.tellg();
        
        inFile.seekg (0, inFile.beg);
        if(!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");
        
        while(true){
            instWord =     (unsigned char)memory[pc] |
            (((unsigned char)memory[pc+1])<<8) |
            (((unsigned char)memory[pc+2])<<16) |
            (((unsigned char)memory[pc+3])<<24);
            pc += 4;
            // remove the following line once you have a complete simulator
            if(pc==40) break;            // stop when PC reached address 32
            instDecExec(instWord);
        }
    } else emitError("Cannot access input file\n");
}
