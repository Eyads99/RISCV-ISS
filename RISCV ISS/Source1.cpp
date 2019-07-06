/*  This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the "ADD", "SUB" and "ADDI"instructions and prints
	"Unknown Instruction" for all other instructions!
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
void ecaller(int regs[32])
{
	int a1 = regs[11];
	unsigned int address;
	switch (regs[17]) {
	case 1: {cout << dec << regs[10] << endl; break; }
	case 4: {address = regs[10];
		while (memory[address] != '\0')
		{
			cout << memory[address]; address++;
		}
		break; }
	case 5: {cin >> regs[10]; break; }
	case 8: {char* point = &memory[regs[10]];
		fgets(point, a1, stdin); break; }
	case 10: {
		cout << "\n";
		for (int i = 0; i < 32; i++)
			cout << "x" << dec << i << ": \t" << "0x" << hex << std::setfill('0') << std::setw(8) << regs[i] << "\n";//dumping regs
		exit(0);
	}
	default:cout << "Unknown Ecall service\n";
	}
}
void instDecExecC(unsigned int instWord)
{
	unsigned int rd, rs2, funct2, funct3, funct4, funct6, opcode;
	unsigned int rd_c, rs1_c, rs2_c, rs2_r, rd_r;
	unsigned int CI_imm, CL_sp_imm, CIW_imm, CLS_imm, CS_sp_imm, CB_imm, CJ_imm, CP_16, CP_4;
	unsigned int instPC = pc - 2;

	opcode = instWord & 0x3;
	rd = (instWord >> 7) & 0x1f;
	rs2 = (instWord >> 2) & 0x1f;
	funct3 = (instWord >> 13) & 0x7;
	rd_c = (instWord >> 2) & 0x7;
	rs1_c = (instWord >> 7) & 0x7;
	rs2_c = (instWord >> 2) & 0x7;
	rs2_r = rs2 >> 3;
	rd_r = rd >> 3;

	CJ_imm = (((instWord >> 3) & 0x7) | ((instWord >> 8) & 0x8) | ((instWord << 2) & 0x10) | ((instWord >> 2) & 0x20) | ((instWord) & 0x40) | ((instWord >> 2) & 0x180) | ((instWord << 1) & 0x200) | ((instWord >> 2) & 0x400) | ((((instWord >> 12) & 0x1) ? 0xFFFFF800 : 0x0))) << 1; //fixed
	CI_imm = ((instWord >> 2) & 0x1f) | ((instWord >> 7) & 0x20) | ((((instWord >> 12) & 0x1) ? 0xFFFFFFF0 : 0x0));  // fixed
	CB_imm = (((instWord >> 3) & 0x3) | ((instWord >> 8) & 0xc) | ((instWord << 2) & 0x10) | ((instWord) & 0x60) | ((instWord >> 5) & 0x80) | ((((instWord >> 12) & 0x1) ? 0xFFFFFF00 : 0x0))) << 1;  // fixed
	CLS_imm = ((((instWord >> 6) & 0x1) | ((instWord >> 9) & 0xE) | ((instWord >> 1) & 10)));  // lw,sw   shift left 2
	CL_sp_imm = ((instWord >> 4) & 0x7) | ((instWord >> 9) & 0x8) | ((instWord << 2) & 0x30);  // lwsp  shift left 2   //fixed
	CS_sp_imm = ((instWord >> 9) & 0xf) | ((instWord >> 3) & 0x30); // swsp   shift left 2
	CP_16 = ((instWord >> 6) & 0x1) | ((instWord >> 1) & 0x2) | ((instWord >> 3) & 0x4) | ((instWord) & 0x18) | ((instWord >> 7) & 0x20) | ((((instWord >> 12) & 0x1) ? 0xFFFFFFE0 : 0x0));  //addisp16    shift left 4    // fixed
	CP_4 = ((instWord >> 6) & 0x1) | ((instWord >> 4) & 0x2) | ((instWord >> 9) & 0xc) | ((instWord >> 3) & 0xf0);  //addisp4    shift left 2
	printPrefix(instPC, instWord);

	if (opcode == 0x1) {
		switch (funct3)
		{
		case 0:
			if (rd == 0)
			{
				cout << "\tC.NOP\tx" << dec << rd << ", " << 0 << "\n";
				regs[0] += regs[0];//useless in practice but this is what the NOP instruction does
				break;
			}
			else
			{
				cout << "\tC.ADDI\tx" << dec << rd << ", " << (signed)CI_imm << "\n";
				regs[rd] = regs[rd] + (signed)CI_imm;
				break;
			}
		case 1:
			cout << "\tC.JAL\t" << hex<<"0x" << pc + (signed)CJ_imm - 2 << "\n";
			regs[1] = pc;
			pc = pc + (signed)CJ_imm - 2;
			break;

		case 2:
			cout << "\tC.LI\tx" << dec << rd << ", " << (signed)CI_imm << "\n";
			regs[rd] = (signed)CI_imm;
			break;
		case 3:if (rd == 2)
		{
			cout << "\tC.ADDISP16\tx" << dec << rd_c << ", " << CP_16 << "\n";
			rd_c += regs[2] + (CP_16 << 4); break;
		}
			   else
		{
			cout << "\tC.LUI\tx" << dec << rd << ", " << (signed)CI_imm << "\n";
			regs[rd] = (signed)CI_imm << 12;
			break;
		}
		case 4:
			if (rd_r == 0) {
				cout << "\tC.SRLI\tx" << dec << rd_c + 8 << ", " << (signed)CI_imm << "\n";
				regs[rd_c + 8] = regs[rd_c + 8] >> CI_imm;
				break;
			}
			else if (rd_r == 1)
			{
				cout << "\tC.SRAI\tx" << dec << rd_c + 8 << ", " << (signed)CI_imm << "\n";
				regs[rd_c + 8] = regs[rd_c + 8] >> CI_imm;
				break;
			}
			else if (rd_r == 2)
			{
				cout << "\tC.ANDI\tx" << dec << rd_c + 8 << ", " << (signed)CI_imm << "\n";
				regs[rd_c + 8] = regs[rd_c + 8] & (signed)CI_imm;
				break;
			}
			else if (rd_r == 4)
			{
				switch (rs2_r) {
				case 0:
					cout << "\tC.SUB\tx" << dec << rd_c + 8 << ", x" << rs2_c + 8 << "\n";
					regs[rd_c + 8] = regs[rd_c + 8] - regs[rs2_c + 8];
					break;
				case 1:
					cout << "\tC.XOR\tx" << dec << rd_c + 8 << ", x" << rs2_c + 8 << "\n";
					regs[rd_c + 8] = regs[rd_c + 8] ^ regs[rs2_c + 8];
					break;
				case 2:
					cout << "\tC.OR\tx" << dec << rd_c + 8 << ", x" << rs2_c + 8 << "\n";
					regs[rd_c + 8] = regs[rd_c + 8] | regs[rs2_c + 8];
					break;
				case 3:
					cout << "\tC.AND\tx" << dec << rd_c + 8 << ", x" << rs2_c + 8 << "\n";
					regs[rd_c + 8] = regs[rd_c + 8] & regs[rs2_c + 8];
					break;
				}
			}
		case 5:

			pc = pc + (signed)CJ_imm - 2;
                cout << "\tC.J\t" <<hex <<"0x" << pc<< "\n";
			break;
		case 6:
			cout << "\tC.BEQZ\tx" << dec << rs1_c + 8 <<", 0x"<<hex<< (signed)CB_imm << "\n";
			if (regs[rd_c + 8] == 0)
				pc = pc + (signed)CB_imm - 2;
			break;
		case 7:
			cout << "\tC.BNEZ\tx" << dec << rs1_c + 8 <<", 0x"<<hex<< (signed)CB_imm << "\n";
			if (regs[rd_c + 8] /*!= 0*/)
				pc = pc + (signed)CB_imm - 2;
			break;
		default:
			cout << "Unknown 1 type compressed instruction";
			break;
		}
	}

	else if (opcode == 0x2) {
		switch (funct3) {
		case 0:
			cout << "\tC.SLLI\tx" << dec << rd << ", " << (int)CI_imm << "\n";
			regs[rd] = regs[rd] << (int)CI_imm;
			break;
		case 2:
			cout << "\tC.LWSP\tx" << dec << rd << ", " << (int)CL_sp_imm << "\n";
			regs[rd] = regs[2] + (CL_sp_imm << 2);
			break;

		case 4:
			if (((instWord >> 12) & 0x1) == 0) {
				if (rs2 == 0) {
					cout << "\tC.JR\tx" << dec  << rd << "\n";
					pc = regs[rd];
					break;
				}
				else {
					cout << "\tC.MV\tx" << dec << rd << ", x" << rs2 << "\n";
					regs[rd] = regs[rs2];
					break;
				}
			}
			else {
				if ((rd == 0) && (rs2 == 0)) {
					//cout << "\tC.EBREAK NOT IMPLEMENTED\n";
				}
				else if (rs2 == 0) {
					cout << "\tC.JALR\tx" << dec << rd << "\n";
					regs[1] = pc ;
					pc = regs[rd];
					break;
				}
				else {
					cout << "\tC.ADD\tx" << dec << rd << ", x" << rs2 << "\n";
					regs[rd] = regs[rd] + regs[rs2];
					break;
				}
			}
		case 6:
			cout << "\tC.SWSP\tx" << dec << rs2 << CS_sp_imm << "\n";
			memory[regs[2] + (CS_sp_imm << 2)] = rs2;
			break;
		default:
			cout << "Unknown 2 type compressed instruction";
		}
	}
	else if (opcode == 0x0) {
		switch (funct3) {
		case 0:
			cout << "\tC.ADDI4SPN\tx" << dec << rd_c << ", x" << CP_4 << "\n";
			regs[rd_c] = regs[2] + (CP_4 << 2);
			break;
		case 2:
			cout << "\tC.LW\tx" << dec << rd_c << ", x" << rs1_c << CLS_imm << "\n";
			regs[rd_c] = ((memory[regs[rs1_c] + (signed)CLS_imm] & 0xff) | ((memory[regs[rs1_c] + (signed)CLS_imm + 1] << 8) & 0xff00) | ((memory[regs[rs1_c] + (signed)CLS_imm + 2] << 16) & 0xff0000) | ((memory[regs[rs1_c] + (signed)CLS_imm + 3] << 24) & 0xff000000));
			break;
		case 6:
			cout << "\tC.SW\tx" << dec << rs2_c << ", " << (signed)CLS_imm << "(x" << rs1_c << ")" << "\n";
			memory[regs[rs1_c] + (signed)CLS_imm] = regs[rs1_c] & 0xff;
			memory[regs[rs1_c] + (signed)CLS_imm + 1] = (regs[rs2_c] >> 8) & 0xff;
			memory[regs[rs1_c] + (signed)CLS_imm + 2] = (regs[rs2_c] >> 16) & 0xff;
			memory[regs[rs1_c] + (signed)CLS_imm + 3] = (regs[rs2_c] >> 24) & 0xff;
		default:
			cout << "\tUnknown 0 type compressed instruction\n";
		}
	}

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

	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	B_imm = (((instWord >> 8) & 0xF) | ((instWord >> 21) & 0x3F0) | ((instWord << 3) & 0x400) | ((instWord >> 20) & 0x800) | (((instWord >> 31) ? 0xFFFFF800 : 0x0))) << 1;
	S_imm = ((instWord >> 7) & 0x1F) | ((instWord >> 20) & 0xFE0) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	U_imm = ((instWord) & 0xFFFFF000);
	J_imm = (((instWord >> 21) & 0x3FF) | ((instWord >> 10) & 0x400) | ((instWord >> 1) & 0x7F800) | ((instWord >> 12) & 0x80000) | (((instWord >> 31) ? 0xFFF80000 : 0x0))) << 1;

	printPrefix(instPC, instWord);

	if (opcode == 0x33) {		// R Instructions
		switch (funct3) {
		case 0: if (funct7 == 32) {
			cout << "\tSUB\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] - regs[rs2];
		}
				else {
			cout << "\tADD\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] + regs[rs2];
		}
				break;
		case 1:
			cout << "\tSLL\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] << regs[rs2];
			break;
		case 2:
			cout << "\tSLT\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";/////////////////not done
			if ((signed)regs[rs1] < (signed)regs[rs2]) regs[rd] = 1; else regs[rd] = 0;
			break;
		case 3:
			cout << "\tSLTU\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";/////////////////not done
			if ((unsigned)regs[rs1] < (unsigned)regs[rs2]) regs[rd] = 1; else regs[rd] = 0;
			break;
		case 4:
			cout << "\tXOR\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] ^ regs[rs2];
			break;
		case 5:
			if (funct7 == 32)
			{
				cout << "\tSRA\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";
				regs[rd] = (signed int)regs[rs1] >> regs[rs2];
			}
			else
			{
				cout << "\tSRL\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";
				regs[rd] = (unsigned int)regs[rs1] >> regs[rs2];
			}
			break;
		case 6:
			cout << "\tOR\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] | regs[rs2];
			break;
		case 7:
			cout << "\tand\tx"<<dec << rd << ", x"<<dec << rs1 << ", x" << rs2 << "\n";
			regs[rd] = regs[rs1] & regs[rs2];
			break;
		default:
			cout << "\tUnknown R Instruction \n";

		}
	}
	else if (opcode == 0x13) {	// I instructions
		switch (funct3) {
		case 0:	cout << "\tADDI\tx" << dec << rd << ", x"<<dec << rs1 << ", "<<dec << (int)I_imm << "\n";
			regs[rd] = regs[rs1] + (signed)I_imm;
			break;
		case 2:
			cout << "\tSLTI\tx" << dec << rd << ", x"<<dec << rs1 << ", " << (int)I_imm << "\n";
			if ((signed)regs[rs1] < (signed int)I_imm)regs[rd] = 1; else regs[rd] = 0;
			break;
		case 3:
			cout << "\tSLTIU\tx" << dec << rd << ", x"<<dec << rs1 << ", " << (int)I_imm << "\n";
			if ((unsigned)regs[rs1] < (unsigned)I_imm) regs[rd] = 1; else regs[rd] = 0;
			break;
		case 4:
			cout << "\tXORI\tx" << dec << rd << ", x" << rs1 << ", " << (int)I_imm << "\n";
			regs[rd] = regs[rs1] ^ I_imm;
			break;
		case 6:
			cout << "\tORI\tx" << dec << rd << ", x" << rs1 << ", " << (int)I_imm << "\n";
			regs[rd] = regs[rs1] | I_imm;
			break;
		case 7:
			cout << "\tANDI\tx" << dec << rd << ", x" << rs1 << ", " << (int)I_imm << "\n";
			regs[rd] = regs[rs1] & I_imm;
			break;
		case 1:
			cout << "\tSLLI\tx" << dec << rd << ", x" << rs1 << ", " << (int)rs2 << "\n";
			regs[rd] = regs[rs1] << (int)I_imm;
			break;
		case 5:
			if (funct7 == 32)
			{
				cout << "\tSRAI\tx" << dec << rd << ", x" << rs1 << ", " << (signed int)rs2 << "\n";
				regs[rd] = (signed int)regs[rs1] >> (int)I_imm; //regs[rs2];
			}
			else
			{
				cout << "\tSRLI\tx" << dec << rd << ", x" << rs1 << ", " << (int)rs2 << "\n";
				regs[rd] = (unsigned int)regs[rs1] >> I_imm;// regs[rs2];
			}
			break;
		default:
			cout << "\tUnknown I Instruction \n";
		}
	}
	else if (opcode == 0x3)  // load instructions
	{
		switch (funct3) {
		case 0:
			cout << "\tLB\tx" << dec << rd << ", " << (signed)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = (memory[regs[rs1] + (signed)I_imm] & 0xff) | ((((regs[rd] >> 7) & 0x1) ? 0xFFFFFF00 : 0x0));
			break;
		case 1:
			cout << "\tLH\tx" << dec << rd << ", " << (signed)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = ((memory[regs[rs1] + (signed)I_imm] & 0xff) | ((memory[regs[rs1] + (signed)I_imm + 1] << 8) & 0xff00)) | ((((regs[rd] >> 15) & 0x1) ? 0xFFFF0000 : 0x0));
			break;
		case 2:
			cout << "\tLW\tx" << dec << rd << ", " << (signed)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = ((memory[regs[rs1] + (signed)I_imm] & 0xff) | ((memory[regs[rs1] + (signed)I_imm + 1] << 8) & 0xff00) | ((memory[regs[rs1] + (signed)I_imm + 2] << 16) & 0xff0000) | ((memory[regs[rs1] + (signed)I_imm + 3] << 24) & 0xff000000));
			break;
		case 4:
			cout << "\tLBU\tx" << dec << rd << ", " << (signed)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = (memory[regs[rs1] + (signed)I_imm] & 0xff) & 0x000000ff;
			break;
		case 5:
			cout << "\tLHU\tx" << dec << rd << ", " << (signed)I_imm << "(x" << rs1 << ")" << "\n";
			regs[rd] = ((memory[regs[rs1] + (signed)I_imm] & 0xff) | ((memory[regs[rs1] + (signed)I_imm + 1] << 8) & 0xff00)) & 0x0000ffff;
			break;
		default:
			cout << "\tUnknown Load Instruction \n";
		}
	}
	else if (opcode == 0x63)  //SB instructions
	{
		switch (funct3) {
		case 0:
			cout << "\tBEQ\tx" << dec << rs1 << ", x" << dec << rs2 << ", " << hex << "0x" << pc+(signed int)B_imm - 4 << "\n";
			if (regs[rs1] == regs[rs2]) { pc += (signed int)B_imm - 4; } break;
		case 1:
			cout << "\tBNE\tx" << dec << rs1 << ", x" << dec << rs2 << ", " << hex << "0x" << pc+(signed int)B_imm - 4 << "\n";
			if (regs[rs1] != regs[rs2]) { pc += (signed int)B_imm - 4; } break;/
		case 4:
			cout << "\tBLT\tx" << dec << rs1 << ", x" << dec << rs2 << ", " << hex << "0x" << pc+(signed int)B_imm - 4 << "\n";
			if (regs[rs1] < regs[rs2]) { pc += (signed int)B_imm - 4; } break;
		case 5:
			cout << "\tBGE\tx" << dec << rs1 << ", x" << dec << rs2 << ", " << hex << "0x" << pc+(signed int)B_imm - 4 << "\n";
			if (regs[rs1] >= regs[rs2]) { pc += (signed int)B_imm - 4; } break;
		case 6:
			cout << "\tBLTU\tx" << dec << rs1 << ", x" << dec << rs2 << ", " << hex << "0x" << pc+(signed int)B_imm - 4 << "\n";
			if ((unsigned int)regs[rs1] < (unsigned int)regs[rs2]) { pc += (signed int)B_imm - 4; } break;
		case 7:
			cout << "\tBGEU\tx" << dec << rs1 << ", x" << dec << rs2 << ", " << hex << "0x" << pc+(signed int)B_imm - 4 << "\n";
			if ((unsigned int)regs[rs1] >= (unsigned int)regs[rs2]) { pc += (signed int)B_imm - 4; } break;
		default:cout << "\tUnknown SB instruction\n";
		}
	}
	else if (opcode == 0x23)//S instructions
	{
		switch (funct3) {
		case 0:
			cout << "\tSB\tx" << dec << rs2 << ", " << (signed)S_imm << "(x" << rs1 << ")" << "\n";
			memory[regs[rs1] + (signed)S_imm] = regs[rs2] & 0xff;
			break;
		case 1:
			cout << "\tSH\tx" << dec << rs2 << ", " << (signed)S_imm << "(x" << rs1 << ")" << "\n";

			memory[regs[rs1] + (signed)S_imm] = regs[rs1] & 0xff;
			memory[regs[rs1] + (signed)S_imm + 1] = (regs[rs2] >> 8) & 0xff;
			break;
		case 2:
			cout << "\tSW\tx" << dec << rs2 << ", " << (signed)S_imm << "(x" << rs1 << ")" << "\n";

			memory[regs[rs1] + (signed)S_imm] = regs[rs1] & 0xff;
			memory[regs[rs1] + (signed)S_imm + 1] = (regs[rs2] >> 8) & 0xff;
			memory[regs[rs1] + (signed)S_imm + 2] = (regs[rs2] >> 16) & 0xff;
			memory[regs[rs1] + (signed)S_imm + 3] = (regs[rs2] >> 24) & 0xff;
			break;
		default:
			cout << "Unknown S instruction" << endl;
		}
	}
	else if (opcode == 0x37)//LUI instruction
	{
		cout << "\tLUI\tx" << dec << rd << ", " << hex << "0x" << (int)U_imm << "\n";
		regs[rd] = U_imm << 12;
	}
	else if (opcode == 0x17)//AUIPC instruction
	{
		cout << "\tAUIPC\tx" << dec << rd << ", " << hex << "0x" << (int)U_imm << "\n";
		regs[rd] = pc + (U_imm << 12);
	}
	else if (opcode == 0x6F)//JAL
	{
		cout << "\tJAL\tx" << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << pc + (signed)J_imm - 4 << "\n";
		regs[rd] = pc + 4;
		pc = pc + (signed)J_imm - 4;
	}
	else if (opcode == 0x67)//JALR
	{
		cout << "\tJALR\tx" << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << regs[rs1] + (signed)I_imm - 4 << "\n";
		regs[rd] = pc + 4;
		pc = regs[rs1] + (signed)I_imm - 4;
	}
	else if (opcode == 0x73)
	{
		cout << "\tECALL\n";
		ecaller(regs);

	}
	else {
		cout << "\tUnknown Instruction \n";
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
			regs[0] = 0;//makes sure that the zero reg is always 0
			instWord = ((unsigned char)memory[pc]);
			if ((instWord & 0x3) == 0x3)
			{
				instWord = instWord | (((unsigned char)memory[pc + 1]) << 8) |
					(((unsigned char)memory[pc + 2]) << 16) |
					(((unsigned char)memory[pc + 3]) << 24);
				pc += 4;
				instDecExec(instWord);
			}
			else
			{
				instWord = instWord | (((unsigned char)memory[pc + 1]) << 8);
				pc += 2;
				instDecExecC(instWord);
			}

		}

	}
	else emitError((char*)"Cannot access input file\n"); return 0;
}