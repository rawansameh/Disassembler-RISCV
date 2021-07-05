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
#include<string>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <vector>
using namespace std;

unsigned int pc = 0x0;

char memory[8 * 1024];	

vector<unsigned int> symbol_table;
void emitError(string s)  // string bdl char*
{
	cout << s;
	exit(0);
}
// do a fu to fill symp ta first
void addLabel(unsigned int immed, int &count, unsigned int instWord) // here we add labels of jumps to construct a symbol table
{
	if ((instWord & 0x00000003) != 0x3)    // if it is 16 bits, we do -2 because we did +2 below
	{
		unsigned int currPc = pc - 2;
		unsigned int addressL = currPc + (immed * 2);
		symbol_table.push_back(addressL);
	}
	else
	{
		unsigned int currPc = pc - 4;  // if it is 32 bits, we do -4 because we did +4 below
		unsigned int addressL = currPc + (immed * 2);
		symbol_table.push_back(addressL);
	}

}
void printPrefix(unsigned int instA, unsigned int instW) {
	for (int i = 0; i < symbol_table.size(); i++)
	{
		if ((instW & 0x00000003) != 0x3) {  // if next word is 16 bits , we do pc -2 to put label before it not before the one after it
			if (symbol_table[i] -2 == instA)
				cout << "L" << i + 1 << ":" << endl;
		}
		else
		{
			if (symbol_table[i] == instA)
				cout << "L" << i + 1 << ":" << endl;
		}
	}
	if ((instW & 0x00000003) != 0x3)
	{
		cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(4) << instW;  // to make printed machine code only 16 bits without 16 preceeding zeros
	}
	else 
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;    // instA is memory location and instW is corresponding machine code 
}

string abi(unsigned int reg) // get abi for I extension mainly
{
	string r;
	switch (reg)
	{
	case 0:
		
		r = "zero";
		break;
	case 1:
		
		r = "ra";
		break;
	case 2:
		
		r = "sp";
		break;
	case 3:
		
		r = "gp";
		break;
	case 4:
		
		r = "tp";
		break;
	case 5: 
		
		r = "t0";
		break;
	case 6:
		
		r = "t1";
		break;
	case 7:
		
		r = "t2";
		break;
	case 8:
		
		r = "s0";
		break;
	case 9:
		
		r = "s1";
		break;
	case 10:
		
		r = "a0";
		break;
	case 11:
		
		r = "a1";
		break;
	case 12:
		
		r = "a2";
		break;
	case 13:
		
		r = "a3";
		break;
	case 14:
		
		r = "a4";
		break;
	case 15:
		
		r = "a5";
		break;
	case 16:
		
		r = "a6";
		break;
	case 17:
		
		r = "a7";
		break;
	case 18:
		
		r = "s2";
		break;
	case 19:
		
		r = "s3";
		break;
	case 20:
		
		r = "s4";
		break;
	case 21:
		
		r = "s5";
		break;
	case 22:
		
		r = "s6";
		break;
	case 23:
		
		r = "s7";
		break;
	case 24:
		
		r = "s8";
		break;
	case 25:
		
		r = "s9";
		break;
	case 26:
		
		r = "s10";
		break;
	case 27:
		
		r = "s11";
		break;
	case 28:
		
		r = "t3";
		break;
	case 29:
		
		r = "t4";
		break;
	case 30:
		
		r = "t5";
		break;
	case 31:
		
		r = "t6";
		break;
	}
	return r;
}

string abi_c(unsigned int reg) // same but for compressed (c extension)
{
	string r;
	switch (reg)
	{
	case 0:
		
		r = "s0"; //x8
		break;
	case 1:
	
		r = "s1"; //x9
		break;
	case 2:
		
		r = "a0"; //x10
		break;
	case 3:
		
		r = "a1"; //x11
		break;
	case 4:
		
		r = "a2"; //x12
		break;
	case 5:
		
		r = "a3"; //x13
		break;
	case 6:
		
		r = "a4"; //x14
		break;
	case 7:
		
		r = "a5"; //x15
		break;
	
	}
	return r;
}


void instDecExec(unsigned int instWord, int &count)
{
	unsigned int rd, rs1, rs2, rd_c, funct3, funct12, funct7=0, funct3_c, opcode, opcode_c;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm, CSR, nzuimm, uimm1, uimm2, rs1_c, rs2_c;
	unsigned int CI_imm_r, tempCI_imm_r, funct3_r, rd_r, other_rs1_r, CJ_imm_r, funct2_r, Other_funct2_r, rs2_r, CB_imm_r, temp_cb_r, rs1_r;
	unsigned int address;

	unsigned int instPC = pc - 4;  

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	funct7 = (instWord >> 25) & 0x0000007F;
	funct12 = (instWord >> 20) & 0x00000FFF;
	// — inst[31] — inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));//hena bygeeb awel 11 bits and then gets sign bit by shifting 31 bits so if it is 1, it generates 20 1s to fill the 32 register if not it fills it withh zeros
	CSR = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	opcode_c = instWord & 0x0003;  // wla 8 zeros?
	rd_c = (instWord >> 2) & 0x0007;
	rs2_c = (instWord >> 2) & 0x0007;
	uimm1 = (instWord >> 5) & 0x0003;
	nzuimm = (instWord >> 5) & 0x00FF;
	rs1_c = (instWord >> 7) & 0x0007;
	uimm2 = (instWord >> 10) & 0x0007;

	J_imm = (instWord >> 12) & 0x000FFFFF;
	/*for (int i = 5; i < 13; i++)
	{
		nzuimm[i - 5] = (instWord >> i) & 0x0001;
	}*/
	funct3_c = (instWord >> 13) & 0x0007;

	CI_imm_r = ((instWord >> 2) & 0x1F);
	tempCI_imm_r = ((instWord >> 12) & 0x1);
	funct3_r = ((instWord >> 13) & 0x00000007);

	rd_r = (instWord >> 7) & 0x0000001F;

	other_rs1_r = (instWord >> 7) & 0x7;
	rs1_r = (instWord >> 7) & 0x0000001F;

	CJ_imm_r = (instWord >> 2) & 0x7FF;

	funct2_r = (instWord >> 10) & 0x3;
	Other_funct2_r = (instWord >> 5) & 0x3;

	rs2_r = (instWord >> 2) & 0x7;

	CB_imm_r = (instWord >> 2) & 0x67;
	temp_cb_r = (instWord >> 10) & 0x31;
	
	printPrefix(instPC, instWord);

	if (opcode == 0x33) {		// R Instructions
		switch (funct3) {
		case 0: if (funct7 == 32) {
			cout << "\tSUB\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
		}
			  else
			  {
			cout << "\tADD\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
		}
			  break;
		case 1:
			cout << "\tSLL\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";

			break;
		case 2:
			cout << "\tSLT\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
			break;
		case 3:  cout << "\tSLTU\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
			break;
		case 4:  cout << "\tXOR\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
			break;
		case 5: if (funct7 == 32)
		{
			cout << "\tSRA\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
		}
			  else
		{
			cout << "\tSRL\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
		}
		break;
		case 6:  cout << "\tOR\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
			break;
		case 7:  cout << "\tAND\t" << abi(rd) << ", " << abi(rs1) << ", " << abi(rs2) << "\n";
			break;
		default:
			cout << "\tUnkown R Instruction \n";
		}
	}
	else if (opcode == 0x13) {	// I instructions
	switch (funct3) {
	case 0:	cout << "\tADDI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
		break;
	case 1:    cout << "\tSLLI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
		break;
	case 2:    cout << "\tSLTI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
		break;
	case 3:    cout << "\tSLTIU\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
		break;
	case 4:    cout << "\tXORI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
		break;
	case 5: if (funct7 == 32) {
		cout << "\tSRAI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
	}
		  else {
		cout << "\tSRLI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
	}
		  break;
	case 6:    cout << "\tORI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
		break;
	case 7:    cout << "\tANDI\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
		break;
	default:
		cout << "\tUnkown I Instruction \n";
	}
	}

	else if (opcode == 0xF) {
	switch (funct3) {
	case 0:
		cout << "\tFENCE\t" << "1, 1" << "\n";// FENCE instruction
		break;
	case 1:
		cout << "\tFENCE.I\t" << "\n"; //FENCE.I instruction
		break;
	default:
		cout << "\tUnkown R Instruction \n";
	}
	}

	else if (opcode == 0x73)
	{
	switch (funct3)
	{
	case 0:
		if (funct12 == 1)
		{
			cout << "\tEBREAK\t" << "\n"; 
		}
		else
			cout << "\tECALL\t" << "\n";
		break;

	case 1:
		cout << "\tCSRRW\t" << abi(rd) << ", " << hex << "0x" << (int)CSR << ", " << abi(rs1) << "\n";
		break;
	case 2:
		cout << "\tCSRRS\t" << abi(rd) << ", " << hex << "0x" << (int)CSR << ", " << abi(rs1) << "\n";
		break;
	case 3:
		cout << "\tCSRRC\t" << abi(rd) << ", " << hex << "0x" << (int)CSR << ", " << abi(rs1) << "\n";
		break;
	case 5:
		cout << "\tCSRRWI\t" << abi(rd) << ", " << hex << "0x" << (int)CSR << ", " << hex << "0x" << rs1 << "\n";
		break;
	case 6:
		cout << "\tCSRRSI\t" << abi(rd) << ", " << hex << "0x" << (int)CSR << ", " << hex << "0x" << rs1 << "\n";
		break;
	case 7:
		cout << "\tCSRRCI\t" << abi(rd) << ", " << hex << "0x" << (int)CSR << ", " << hex << "0x" << rs1 << "\n";
		break;
	default:
		cout << "\tUnkown R Instruction \n";
	}
	}

	else if (opcode == 0x6F)
	{
	unsigned int temp = 0x00000000;
	temp = ((J_imm & 0x00001) << 11) | ((J_imm & 0x00002) << 11) | ((J_imm & 0x00004) << 11) | ((J_imm & 0x00008) << 11) | ((J_imm & 0x00010) << 11) | ((J_imm & 0x00020) << 11) | ((J_imm & 0x00040) << 11) | ((J_imm & 0x00080) << 11) | ((J_imm & 0x00100) << 2) | ((J_imm & 0x00200) >> 9) | ((J_imm & 0x00400) >> 9) | ((J_imm & 0x00800) >> 9) | ((J_imm & 0x01000) >> 9) | ((J_imm & 0x02000) >> 9) | ((J_imm & 0x04000) >> 9) | ((J_imm & 0x08000) >> 9) | ((J_imm & 0x10000) >> 9) | ((J_imm & 0x20000) >> 9) | ((J_imm & 0x40000) >> 9) | ((J_imm & 0x80000));
	
	cout << "\tJAL\t" << abi(rd) << ", " << "L" << count << "\n";
	addLabel(temp, count, instWord);
	count++;
	}
	else if (opcode == 0x03)  // I
	{
		
		switch (funct3) {

		case 0: cout << "\tLB\t" << abi(rd) << ", " << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
			break;
		case 1: cout << "\tLH\t" << abi(rd) << ", " << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
			break;
		case 2: cout << "\tLW\t" << abi(rd) << ", " << "0x" << (int)I_imm << "(" << abi(rs1) << ")" << "\n";
			break;
		case 4:    cout << "\tLBU\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
			break;
		case 5:    cout << "\tLHU\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
			break;
		default:
			cout << "\tUnkown I Instruction\n ";
		}
	}

	else if (opcode == 0x67) // I
	{
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	cout << "\tJALR\t" << abi(rd) << ", " << abi(rs1) << ", " << hex << "0x" << (int)I_imm << "\n";
	}
	else if (opcode == 0x63)   // B type
	{
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	B_imm = ((instWord >> 7) & 0x1);
	unsigned int B_imm_temp1, B_imm_temp2, B_imm_temp3, temp;
	B_imm_temp1 = ((instWord >> 8) & 0xF);
	B_imm_temp2 = ((instWord >> 25) & 0x3F);
	B_imm_temp3 = ((instWord >> 31) & 0x1);
	temp = ((B_imm & 0x00001) << 11) | (B_imm_temp1 & 0x00001) | (B_imm_temp1 & 0x00002) | (B_imm_temp1 & 0x00004) | (B_imm_temp1 & 0x00008) | (B_imm_temp2 & 0x00001) | (B_imm_temp2 & 0x00002) | (B_imm_temp2 & 0x00004) | (B_imm_temp2 & 0x00008) | (B_imm_temp2 & 0x00010) | (B_imm_temp2 & 0x00020) | (B_imm_temp3 & 0x00001);
	

	switch (funct3) {
	case 0: 
		cout << "\tBEQ\t" << abi(rs1) << ", " << abi(rs2) << ", " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	case 1: 
		cout << "\tBNE\t" << abi(rs1) << ", " << abi(rs2) << ", " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	case 4: 
		cout << "\tBLT\t" << abi(rs1) << ", " << abi(rs2) << ", " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	case 5: 
		cout << "\tBGE\t" << abi(rs1) << ", " << abi(rs2) << ", " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	case 6: 
		cout << "\tBLTU\t" << abi(rs1) << ", " << abi(rs2) << ", " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	case 7: 
		cout << "\tBGEU\t" << abi(rs1) << ", " << abi(rs2) << ", " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	default:
		cout << "\tUnkown B Instruction\n ";
	}
	}

	else if (opcode == 0x37) //U
	{
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	U_imm = ((instWord >> 12) & 0xFFFFF);

	cout << "\tLUI\t" << abi(rd) << hex << ", 0x" << (int)U_imm << "\n";

	}
	else if (opcode == 0x17) // U
	{
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	U_imm = ((instWord >> 12) & 0xFFFFF);
	cout << "\tAUIPC\t" << abi(rd) << hex << ", 0x" << (int)U_imm << "\n";

}
	else if (opcode == 0x23) { // S instructions
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	S_imm = (instWord >> 7) & 0xF;
	unsigned int S_imm_temp;
	S_imm_temp = (instWord >> 25) & 0x7F;

	S_imm = ((S_imm_temp & 0x7F) << 5) | S_imm;

	switch (funct3) {
	case 0:    cout << "\tSB\t" << abi(rs2) << ", " << hex << "0x" << (int)S_imm << "(" << abi(rs1) << ")" << "\n";
		break;
	case 1:    cout << "\tSH\t" << abi(rs2) << ", " << hex << "0x" << (int)S_imm << "(" << abi(rs1) << ")" << "\n";
		break;
	case 2:    cout << "\tSW\t" << abi(rs2) << ", " << hex << "0x" << (int)S_imm << "(" << abi(rs1) << ")" << "\n";
		break;
	default:
		cout << "\tUnkown I Instruction \n";
	}
	}
	else if (opcode_c == 0x0) //quadrant 0 compressed instructions
	{
	switch (funct3_c)
	{
		unsigned int temp;
	case 0:
		temp = ((nzuimm & 0x01) << 1) | ((nzuimm & 0x02) >> 1) | ((nzuimm & 0x04) << 2) | ((nzuimm & 0x08) << 2) | ((nzuimm & 0x10) << 2) | ((nzuimm & 0x20) >> 2) | ((nzuimm & 0x40) >> 4) | ((nzuimm & 0x80) >> 4);
		cout << "\tC.ADDI4SPN\t" << abi(rd_c) << ", " << hex << "0x" << (int)temp *4<< "\n";
		break;
	case 2:
		//unsigned int temp;
		temp = ((uimm1 & 0x0001) << 3) | ((uimm1 & 0x0002) >> 1) | ((uimm2 & 0x0001) >> 1) | ((uimm2 & 0x0002) >> 1) | ((uimm2 & 0x0004) >> 1); // I must append 2 zeros at the end of temp
		cout << "\tC.LW\t" << abi_c(rd_c) << ", " << hex << "0x" << (int)temp * 8 << "(" << abi_c(rs1_c) << ")" << "\n";
		break;
	case 6:
		//unsigned int temp;
		temp = ((uimm1 & 0x0001) << 3) | ((uimm1 & 0x0002) >> 1) | ((uimm2 & 0x0001) >> 1) | ((uimm2 & 0x0002) >> 1) | ((uimm2 & 0x0004) >> 1); // I must append 2 zeros at the end of temp, shift p 2?
		cout << "\tC.SW\t" << abi_c(rs2_c) << ", " << hex << "0x" << (int)temp * 8 << "(" << abi_c (rs1_c) << ")" << "\n";
		break;
	default:
		cout << "\tUnkown compressed Instruction \n";

	}
	}
	// quadrant 1 compressed instructions
	else if (opcode_c == 0x1) {
	unsigned int temp = 0x00000000;
	switch (funct3_r) {
		
	case 0: if (rd_r == 0 || rs1_r == 0)
		cout << "\tC.NOP\t" << "\n";
		  else
	{
		temp = ((tempCI_imm_r & 0x01) << 5) | CI_imm_r;
		cout << "\tC.ADDI\t" << abi(rd_r) << ", " << hex << "0x" << (int)temp << "\n";
	}
		break;
	case 1:
		temp = ((CJ_imm_r & 0x0001) << 4) | ((CJ_imm_r & 0x0002) >> 1) | ((CJ_imm_r & 0x0004) >> 1) | ((CJ_imm_r & 0x0008)>> 1) | ((CJ_imm_r & 0x0010)<< 2) | (CJ_imm_r & 0x0020) | ((CJ_imm_r & 0x0040) << 3) | (CJ_imm_r & 0x0080) | (CJ_imm_r & 0x0100) | ((CJ_imm_r & 0x0200) >> 6) | (CJ_imm_r & 0x0400);
		cout << "\tC.JAL\t" << ", " << "L" << count << "\n"; // pc = pc + temp*2
		addLabel(temp, count, instWord);
		count++;

		break;
	case 2:
		temp = ((tempCI_imm_r & 0x01) << 5) | CI_imm_r;
		cout << "\tC.LI\t" << abi(rd_r) << ", 0x" << hex << (int)temp << "\n"; ////////////////////////check

		break;
	case 3:
		if (rd == 2)
		{
			temp = ((tempCI_imm_r & 0x01) << 5) | ((CI_imm_r & 0x01) << 1) | ((CI_imm_r & 0x02) << 2) | ((CI_imm_r & 0x04) << 2) | ((CI_imm_r & 0x08) >> 1) | ((CI_imm_r & 0x10) >> 4);
			cout << "\tC.ADDI16SP\t" << hex << " 0x" << (int)temp << "\n"; ////////////////////////
		}
		else
		{
			temp = ((tempCI_imm_r & 0x01) << 5) | CI_imm_r;
			cout << "\tC.LUI\t" << abi(rd_r) << hex << ", 0x" << (int)temp << "\n";////////////////////////check

		}
		break;
	case 4: switch (funct2_r)
	{
	case 0:
		temp = ((tempCI_imm_r & 0x01) << 5) | CI_imm_r;
		cout << "\tC.SRLI\t" << abi(rd_r) << hex << ", 0x" << (int)temp << "\n";
		break;
	case 1:
		temp = ((tempCI_imm_r & 0x01) << 5) | CI_imm_r;
		cout << "\tC.SRAI\t" << abi(rd_r) << hex << ", 0x" << (int)temp << "\n"; ////////////////////////check

		break;
	case 2:
		temp = ((tempCI_imm_r & 0x01) << 5) | CI_imm_r;
		cout << "\tC.ANDI\tx" << rd_r << hex << ", 0x" << (int)temp << "\n"; //////////////////////
		break;
	case 3: switch (Other_funct2_r)
	{
		
	case 0: 
		cout << "\tC.SUB\t" << abi_c(other_rs1_r) << ", " << abi_c(other_rs1_r) << "\n";
		break;
	case 1: 
		cout << "\tC.XOR\t" << abi_c(other_rs1_r) << ", " << abi_c(other_rs1_r) <<  "\n";
		break;
	case 2: 
		cout << "\tC.OR\t" << abi_c(other_rs1_r) << ", " << abi_c(other_rs1_r) << "\n";
		break;

	case 3: 
		cout << "\tC.AND\t" << abi_c(other_rs1_r) << ", " << abi_c(other_rs1_r) << "\n";
		break;
	default:
		cout << "\tUnkown Compressed Instruction\n ";
	}
		  break;
	default:
		cout << "\tUnkown Compressed Instruction\n ";
	}
		  break;

	case 5:
		temp = ((CJ_imm_r & 0x0001) << 4) | ((CJ_imm_r & 0x0002) >> 1) | ((CJ_imm_r & 0x0004) >> 1) | ((CJ_imm_r & 0x0008) >> 1) | ((CJ_imm_r & 0x0010) << 2) | (CJ_imm_r & 0x0020) | ((CJ_imm_r & 0x0040) << 3) | (CJ_imm_r & 0x0080) | (CJ_imm_r & 0x0100) | ((CJ_imm_r & 0x0200) >> 6) | (CJ_imm_r & 0x0400);
		cout << "\tC.J\t" << " " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	case 6:
		temp = ((temp_cb_r & 0x01) >> 3) | ((temp_cb_r & 0x02) >> 3) | ((temp_cb_r & 0x04)) | ((CB_imm_r & 0x01) << 4) | ((CB_imm_r & 0x02) >> 1) | ((CB_imm_r & 0x04) >> 1) | ((CB_imm_r & 0x08) >> 2) | ((CB_imm_r & 0x10) >> 2);
		cout << "\tBEQZ\t" << abi_c(rs1_r) << ", " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	case 7:
		temp = ((temp_cb_r & 0x01) >> 3) | ((temp_cb_r & 0x02) >> 3) | ((temp_cb_r & 0x04)) | ((CB_imm_r & 0x01) << 4) | ((CB_imm_r & 0x02) >> 1) | ((CB_imm_r & 0x04) >> 1) | ((CB_imm_r & 0x08) >> 2) | ((CB_imm_r & 0x10) >> 2);
		cout << "\tBNEZ\t" << abi_c(rs1_r) << ", " << "L" << count << "\n";
		addLabel(temp, count, instWord);
		count++;
		break;
	default:
		cout << "\tUnkown Compressed Instruction\n ";
	}
	
}
// quadrant 2 compressed instructions
	else if (opcode_c == 0x2)
	{
	unsigned int tempCSS_Imm, CSS_imm, temp_sp, funct1;
	CI_imm_r = ((instWord >> 2) & 0x1F);

	tempCI_imm_r = ((instWord >> 12) & 0x1);
	unsigned int temp = ((tempCI_imm_r & 0x01) << 5) | CI_imm_r;
	temp_sp = ((tempCI_imm_r & 0x01) >> 2) | ((CI_imm_r & 0x01) << 4) | ((CI_imm_r & 0x02) << 4) | ((CI_imm_r & 0x04) >> 2) | ((CI_imm_r & 0x08) >> 2) | ((CI_imm_r & 0x10) >> 2);

	funct3 = ((instWord >> 13) & 0x00000007);
	funct1 = ((instWord >> 12) & 0x1);

	rd = (instWord >> 7) & 0x0000001F;

	rs1 = (instWord >> 7) & 0x0000001F;

	CJ_imm_r = (instWord >> 2) & 0x7FF;

	rs2 = (instWord >> 2) & 0x0000001F;
	CSS_imm = (instWord >> 7) & 0x3F;
	tempCSS_Imm = ((CSS_imm & 0x00001) << 3) | ((CSS_imm & 0x00002) << 3) | (CSS_imm & 0x00004) | (CSS_imm & 0x00008) | ((CSS_imm & 0x00010) >> 2) | ((CSS_imm & 0x00020) >> 2);


	switch (funct3) {

	case 0:
		cout << "\tC.SLLI\t" << abi(rd) << ", " << hex << "0x" << (int)temp << "\n";
		break;


	case 2:
		cout << "\tC.LWSP\tx" << abi(rd) << hex << ", 0x" << (int)temp_sp*4 << "(" << "sp" << ")"<<"\n";
		break;

	case 4:


		switch (funct1) {

		case 0:

			if (rs2 == 0) {
				cout << "\tC.JR\t" << abi(rs1) << "\n";
			}
			else {
				cout << "\tC.MV\t" << abi(rd) << ", " << abi(rs2) << "\n";
			}

			break;

		case 1:
			if (rs2 == 0 && rs1 ==0) {

				cout << "\tC.EBREAK\t" << "\n";
			}
			else if (rs1!=0 && rs2!=0){

				cout << "\tC.ADD\t" << abi(rd) << ", " << abi(rs2) << "\n";

			}
			else if (rs1 != 0 && rs2 == 0)
			{
				cout << "\tC.JALR\t" << abi(rs1) << "\n";
			}
			break;
		}

		break;

	case 6:
		cout << "\tC.SWSP\t" << abi(rs2) << hex << ", 0x" << (int)tempCSS_Imm << "(" << "sp" << ")" << "\n";

		break;
	}
	}
	else  {
		cout << "\tUnkown Instruction \n";
	}

}

int main(int argc, char* argv[]) {
	int count = 1;
	unsigned int instword = 0;
	ifstream infile;
	ofstream outfile;

	if (argc < 2) emitError("use: rvcdiss <machine_code_file_name>\n");

	infile.open(argv[1], ios::in | ios::binary | ios::ate);

	if (infile.is_open())
	{
		int fsize = infile.tellg();

		infile.seekg(0, infile.beg);
		if (!infile.read((char*)memory, fsize)) emitError("cannot read from input file\n");

		while (true) {
			
			instword = (unsigned char)memory[pc] |
				(((unsigned char)memory[pc + 1]) << 8) |
				(((unsigned char)memory[pc + 2]) << 16) |
				(((unsigned char)memory[pc + 3]) << 24);
			pc += 4;
			if ((instword & 0x00000003) != 0x3)
			{
				pc -= 4;
				instword = (unsigned char)memory[pc] | (((unsigned char)memory[pc + 1]) << 8);
				pc += 2;
			}
			
			
			if (pc>=240)break;			
			instDecExec(instword, count);
		}
	}
	else emitError("cannot access input file\n");

	system("pause");
	return 0;
}