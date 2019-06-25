/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the "ADD", "SUB" and "ADDI"instructions and prints
	"Unkown Instruction" for all other instructions!
	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

int regs[32] = { 0 };//to represent the the 32 diffrent regs hence the size of the array
unsigned int pc = 0x0;

char memory[8 * 1024];	// only 8KB of memory located at address 0

void emitError(char *s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) {
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void instDecExec(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
	unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	funct7 = (instWord >> 25) & 0x0000007F;

	// â€” inst[31] â€” inst[30:25] inst[24:21] inst[20]
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	B_imm = 2 * (((instWord >> 8) & 0xF) | ((instWord >> 25) & 0x3F) | ((instWord >> 7) & 0x1) | ((instWord >> 31) & 0x1) | (((instWord >> 31) ? 0xFFFFF800 : 0x0)));

	printPrefix(instPC, instWord);

	if (opcode == 0x33) {		// R Instructions
		switch (funct3) {
		case 0: if (funct7 == 32) {
			cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] - regs[rs2];
		}
				else {
			cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] + regs[rs2];
		}
				break;
		case 1:
			cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] << regs[rs2];
			break;
		case 2:
			cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";/////////////////not done
			if (regs[rs1] < regs[rs2]) regs[rd] = 1; else regs[rd] = 0;
			break;
		case 3:
			cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";/////////////////not done
			if (regs[rs1] < regs[rs2]) regs[rd] = 1; else regs[rd] = 0;
			break;
		case 4:
			cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] ^ regs[rs2];
			break;
		case 5:
			if (funct7 == 32) 
			{
			cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = (signed int)regs[rs1] >> regs[rs2];
			}
			else
			{
			cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = (unsigned int)regs[rs1] >> regs[rs2];
			}
			break;
		case 6:
			cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] | regs[rs2];
			break;
		case 7:
			cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] & regs[rs2];
			break;
		default:
			cout << "\tUnkown R Instruction \n";

		}

	}
	else if (opcode == 0x13) {	// I instructions
		switch (funct3) {
		case 0:	cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (signed int)I_imm << "\n";
			regs[rd] = regs[rs1] + (signed)I_imm;
			break;
		case 2:
			cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (signed int)I_imm << "\n";
			if ((signed)regs[rs1] < (signed int)I_imm)regs[rd] = 1; else regs[rd] = 0;
			break;
		case 3:
			cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (signed int)I_imm << "\n";
			if ((unsigned)regs[rs1] < (unsigned)I_imm)regs[rd] = 1; else regs[rd] = 0;
			break;
		case 4:
			cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (signed int)I_imm << "\n";
			regs[rd] = regs[rs1] ^ I_imm;
			break;
		case 6:
			cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (signed int)I_imm << "\n";
			regs[rd] = regs[rs1] | I_imm;
			break;
		case 7:
			cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (signed int)I_imm << "\n";
			regs[rd] = regs[rs1] & I_imm;
			break;
		case 1:
			cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)rs2 << "\n";
			regs[rd] = regs[rs1] << (int)I_imm;
			break;
		case 5:
			if (funct7 == 32) 
			{
				cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (signed int)rs2 << "\n";
				regs[rd] = (signed int)regs[rs1] >> regs[rs2];
			}
			else
			{
				cout << "\tSRL\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)rs2 << "\n";
				regs[rd] = (unsigned int)regs[rs1] >> regs[rs2];
			}
			break;
		default:
			cout << "\tUnkown I Instruction \n";
		}
	}
	else if (opcode == 0x63) //B instructions
		switch (funct3)
          case 0:cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)B_imm << "\n";
	if (regs[rs1] == regs[rs2]) /*do true stuff*/;
	else {
		cout << "\tUnkown Instruction \n";
	}

}

int main(int argc, char *argv[]) {

	unsigned int instWord = 0;
	ifstream inFile;
	ofstream outFile;

	if (argc < 1) emitError((char*)"use: rv32i_sim <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if (inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg(0, inFile.beg);
		if (!inFile.read((char *)memory, fsize)) emitError((char*)"Cannot read from input file\n");

		while (true) {
			instWord = (unsigned char)memory[pc] |
				(((unsigned char)memory[pc + 1]) << 8) |
				(((unsigned char)memory[pc + 2]) << 16) |
				(((unsigned char)memory[pc + 3]) << 24);
			pc += 4;
			// remove the following line once you have a complete simulator
			if (pc == 32) break;			// stop when PC reached address 32
			instDecExec(instWord);
		}

		// dump the registers
		for (int i = 0; i < 32; i++)
			cout << "x" << dec << i << ": \t" << "0x" << hex << std::setfill('0') << std::setw(8) << regs[i] << "\n";

	}
	else emitError((char*)"Cannot access input file\n");
}