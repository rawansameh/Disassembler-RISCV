/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the RV32I "ADD", "SUB" and "ADDI" instructions only. 
	It prints "Unkown Instruction" for all other instructions!
	
	Usage example:
		$ rvcdiss t1.bin
	should print out:
		0x00000000	0x00100013	ADDI	x0, x0, 0x1
		0x00000004	0x00100093	ADDI	x1, x0, 0x1
		0x00000008	0x00100113	ADDI	x2, x0, 0x1
		0x0000000c	0x001001b3	ADD		x3, x0, x1
		0x00000010	0x00208233	ADD		x4, x1, x2
		0x00000014	0x004182b3	ADD		x5, x3, x4
		0x00000018	0x00100893	ADDI	x11, x0, 0x1
		0x0000001c	0x00028513	ADDI	xa, x5, 0x0
		0x00000020	0x00000073	Unkown Instruction 

	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc = 0x0;

char memory[8*1024];	// only 8KB of memory located at address 0

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
	unsigned int rd, rs1, rs2, funct3, funct7 = 0, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;

	// — inst[31] — inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	printPrefix(instPC, instWord);

	if(opcode == 0x33){		// R Instructions
		switch(funct3){
			case 0: if(funct7 == 32) {
								cout << "\tSUB\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
							}
							else {
								cout << "\tADD\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
							}
							break;
			case 1: 
								cout << "\tSLL\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
							
							break;
			case 2: 
								cout << "\tSLT\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
							
							break;
			case 3: 
								cout << "\tSLTU\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
							
							break;
			case 4: 
								cout << "\tXOR\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
							
							break;
			case 5: if (funct7 == 32) {
								cout << "\tSRA\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
							}
							else {
								cout << "\tSRL\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
							}
							break;
		
			default:
							cout << "\tUnkown R Instruction \n";
		}
	} else if(opcode == 0x13){	// I instructions
		switch(funct3){
			case 0:	cout << "\tADDI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			case 1:	cout << "\tSLLI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			case 2:	cout << "\tSLTI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			case 3:	cout << "\tSLTIU\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			case 4:	cout << "\tXORI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			case 5: if (funct7 == 32) {
				cout << "\tSRAI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
			}
				  else {
				cout << "\tSRLI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
			}
				  break;
			case 6:	cout << "\tORI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			case 7:	cout << "\tANDI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			default:
					cout << "\tUnkown I Instruction \n";
		}
	}
	else if (opcode == 0x3) {	// I-Load instructions
		switch (funct3) {
			case 4:	cout << "\tLBU\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			case 5:	cout << "\tLHU\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
					break;
			default:
					cout << "\tUnkown I Instruction \n";
		}
	}
	else if (opcode == 0x23) {	// S instructions
		switch (funct3) {
		case 0:	cout << "\tSB\t" << abi(rd) << ", " << hex << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
			break;
		case 1:	cout << "\tSH\t" << abi(rd) << ", " << hex << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
			break;
		case 2:	cout << "\tSW\t" << abi(rd) << ", " << hex << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
			break;
		default:
			cout << "\tUnkown I Instruction \n";
		}
	}
	else {
		cout << "\tUnkown Instruction \n";
	}


}

int main(int argc, char *argv[]){

	unsigned int instWord=0;
	ifstream inFile;
	ofstream outFile;

	if(argc<2) emitError("use: rvcdiss <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if(inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg (0, inFile.beg);
		if(!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");

		while(true){
				instWord = 	(unsigned char)memory[pc] |
							(((unsigned char)memory[pc+1])<<8) |
							(((unsigned char)memory[pc+2])<<16) |
							(((unsigned char)memory[pc+3])<<24);
				pc += 4;
				// remove the following line once you have a complete simulator
				if(pc==40) break;			// stop when PC reached address 32
				instDecExec(instWord);
		}
	} else emitError("Cannot access input file\n");
}