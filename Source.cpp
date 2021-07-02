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

void addLabel(unsigned int immed, int &count)
{
	unsigned int currPc = pc - 4;
	unsigned int addressL = currPc + (immed * 2);
	symbol_table.push_back(addressL);

}
void printPrefix(unsigned int instA, unsigned int instW) {
	for (int i = 0; i < symbol_table.size(); i++)
	{
		if (symbol_table[i] == instA)
			cout << "L" << i + 1 << ":" << endl;
	}
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
void instDecExec(unsigned int instWord, int &count)
{
	unsigned int rd, rs1, rs2, rd_c, funct3, funct12, funct7=0, funct3_c, opcode, opcode_c;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm, CSR, nzuimm, uimm1, uimm2, rs1_c, rs2_c;
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
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
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
		default:
			cout << "\tUnkown I Instruction \n";
		}
	}

	else if (opcode==0xF) {      
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
			cout << "\tCSRRW\t" << abi(rd) <<  ", " << hex << "0x" << (int)CSR << ", " << abi(rs1) << "\n";
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
		temp = ((J_imm & 0x00000001) << 11) | ((J_imm & 0x00000002) << 11) | ((J_imm & 0x00000004) << 11) | ((J_imm & 0x00000008) << 11) | ((J_imm & 0x00000010) << 11) | ((J_imm & 0x00000020) << 11) | ((J_imm & 0x00000040) << 11) | ((J_imm & 0x00000080) << 11) | ((J_imm & 0x00000100) << 2) | ((J_imm & 0x00000200) >> 9) | ((J_imm & 0x00000400) >> 9) | ((J_imm & 0x00000800) >> 9) | ((J_imm & 0x00001000) >> 9) | ((J_imm & 0x00002000) >> 9) | ((J_imm & 0x00004000) >> 9) | ((J_imm & 0x00008000) >> 9) | ((J_imm & 0x00010000) >> 9) | ((J_imm & 0x00020000) >> 9) | ((J_imm & 0x00040000) >> 9) | ((J_imm & 0x00080000));
		//cout << "\tJAL\t" << abi(rd) << ", " << hex << "0x" << (int) temp << "\n";
		cout << "\tJAL\t" << abi(rd) << ", " << "L" << count << "\n";
		addLabel(temp, count);
	}
	else if (opcode_c == 0x0)
	{
		switch (funct3_c)
		{unsigned int temp;
		case 0:
			
			/*temp[0] = nzuimm[6];
			temp[1] = nzuimm[7];
			temp[2] = nzuimm[1];
			temp[3] = nzuimm[0];
			temp[4] = nzuimm[5];
			temp[5] = nzuimm[4];
			temp[6] = nzuimm[3];
			temp[2] = nzuimm[2];
			cout << */
			temp = ((nzuimm & 0x0001) << 6) | ((nzuimm & 0x0002) << 7) | ((nzuimm & 0x0004) << 1) | ((nzuimm & 0x0008) >> 1) | ((nzuimm & 0x0010) >> 3) | ((nzuimm & 0x0020) >> 5) | ((nzuimm & 0x0040) >> 1)  | ((nzuimm & 0x0080) >> 3);
			cout << "\tC.ADDI4SPN\tx" << rd_c << ", " << hex << "0x" << (int)temp << "\n";
			break;
		case 2:
			//unsigned int temp;
			temp = ((uimm1 & 0x0001) << 3) | ((uimm1 & 0x0002) >> 1) | ((uimm2 & 0x0001) >> 1) | ((uimm2 & 0x0002) >> 1) | ((uimm2 & 0x0004) >> 1); // I must append 2 zeros at the end of temp
			cout << "\tC.LW\tx" << rd_c << ", x" << rs1_c << ", " << hex << "0x" << (int)temp << "\n";
			break;
		case 6:
			//unsigned int temp;
			temp = ((uimm1 & 0x0001) << 3) | ((uimm1 & 0x0002) >> 1) | ((uimm2 & 0x0001) >> 1) | ((uimm2 & 0x0002) >> 1) | ((uimm2 & 0x0004) >> 1); // I must append 2 zeros at the end of temp, shift p 2?
			cout << "\tC.SW\tx" << rs1_c << ", x" << rs2_c << ", " << hex << "0x" << (int)temp << "\n";
			break;
		default:
			cout << "\tUnkown compressed Instruction \n";

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
			// remove the following line once you have a complete simulator
			if (pc == 88)break;			// stop when pc reached address 32
			instDecExec(instword, count);
		}
	}
	else emitError("cannot access input file\n");

	system("pause");
	return 0;
}