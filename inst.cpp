#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

#include "common.h"

string reg_to_bin(string reg)
{
    return num_to_bin(stoul(reg.substr(1)), 5);
}

string num_to_imm(string num)
{
    return num_to_bin(static_cast<uint32_t>(stoi(num)), 12);
}

int32_t calc_offset(string label)
{
    return static_cast<int32_t>(label_to_text_addr(label)) - static_cast<int32_t>(cur_text_addr);
}

string gen_R_type(string op, vector<string> args)
{
    string opcode, funct3, funct7, rd, rs1, rs2;

    if (op == "slli" || op == "srli" || op == "srai")
        opcode = "0010011";
    else if (op == "add" || op == "sub" || op == "sll" || op == "slt" || op == "sltu" || op == "xor"
            || op == "srl" || op == "sra" || op == "or" || op == "and")
        opcode = "0110011";
    else
        opcode = "1010011";

    if (op == "slli")
        funct3 = "001";
    else if (op == "srli" || op == "srai")
        funct3 = "101";
    else if (op == "add" || op == "sub")
        funct3 = "000";
    else if (op == "sll")
        funct3 = "001";
    else if (op == "slt")
        funct3 = "010";
    else if (op == "sltu")
        funct3 = "011";
    else if (op == "xor")
        funct3 = "100";
    else if (op == "srl" || op == "sra")
        funct3 = "101";
    else if (op == "or")
        funct3 = "110";
    else if (op == "and")
        funct3 = "111";
    else if (op == "fsgnj.s")
        funct3 = "000";
    else if (op == "fsgnjn.s")
        funct3 = "001";
    else if (op == "fsgnjx.s")
        funct3 = "010";
    else if (op == "fmin.s")
        funct3 = "000";
    else if (op == "fmax.s")
        funct3 = "001";
    else if (op == "feq.s")
        funct3 = "010";
    else if (op == "flt.s")
        funct3 = "001";
    else if (op == "fle.s")
        funct3 = "000";
    else if (op == "fmv.s.x")
        funct3 = "000";
    else
        funct3 = "000"; // rm = RNE

    if (op == "slli" || op == "srli" || op == "add" || op == "sll" || op == "slt" || op == "sltu"
            || op == "xor" || op == "srl" || op == "or" || op == "and")
        funct7 = "0000000";
    else if (op == "srai" || op == "sub" || op == "sra")
        funct7 = "0100000";
    else if (op == "fadd.s")
        funct7 = "0000000";
    else if (op == "fsub.s")
        funct7 = "0000100";
    else if (op == "fmul.s")
        funct7 = "0001000";
    else if (op == "fdiv.s")
        funct7 = "0001100";
    else if (op == "fsqrt.s")
        funct7 = "0101100";
    else if (op == "fsgnj.s" || op == "fsgnjn.s" || op == "fsgnjx.s")
        funct7 = "0010000";
    else if (op == "fmin.s" || op == "fmax.s")
        funct7 = "0010100";
    else if (op == "fcvt.w.s" || op == "fcvt.wu.s")
        funct7 = "1100000";
    else if (op == "feq.s" || op == "flt.s" || op == "fle.s")
        funct7 = "1010000";
    else if (op == "fcvt.s.w" || op == "fcvt.s.wu")
        funct7 = "1101000";
    else if (op == "fmv.s.x")
        funct7 = "1111000";

    rd = reg_to_bin(args[0]);
    rs1 = reg_to_bin(args[1]);
    if (op == "slli" || op == "srli" || op == "srai") { // shamt
        uint32_t shamt_val = stoul(args[2]);
        if (!(shamt_val < 32)) {
            report_error("too large immediate");
            report_cur_line();
            close_asm_and_exit();
        }
        rs2 = num_to_bin(shamt_val, 5);
    }
    else if (op == "fsqrt.s" || op == "fcvt.w.s" || op == "fcvt.s.w" || op == "fmv.s.x")
        rs2 = "00000";
    else if (op == "fcvt.wu.s" || op == "fcvt.s.wu")
        rs2 = "00001";
    else
        rs2 = reg_to_bin(args[2]);

    return funct7 + rs2 + rs1 + funct3 + rd + opcode; // reversed
}

string gen_I_type(string op, vector<string> args)
{
    string opcode, funct3, imm, rd, rs1;

    if (op == "addi" || op == "slti" || op == "sltiu" || op == "xori" || op == "ori" || op == "andi")
        opcode = "0010011";
    else if (op == "jalr")
        opcode = "1100111";
    else if (op == "lw")
        opcode = "0000011";
    else if (op == "flw")
        opcode = "0000111";

    if (op == "addi")
        funct3 = "000";
    else if (op == "slti")
        funct3 = "010";
    else if (op == "sltiu")
        funct3 = "011";
    else if (op == "xori")
        funct3 = "100";
    else if (op == "ori")
        funct3 = "110";
    else if (op == "andi")
        funct3 = "111";
    else if (op == "jalr")
        funct3 = "000";
    else if (op == "lw" || op == "flw")
        funct3 = "010";

    int32_t imm_val = stoi(args[2]);
    if (!(-2048 <= imm_val && imm_val < 2048)) {
        report_error("too large immediate");
        report_cur_line();
        close_asm_and_exit();
    }

    imm = num_to_bin(imm_val, 12);
    rd = reg_to_bin(args[0]);
    rs1 = reg_to_bin(args[1]);

    return imm + rs1 + funct3 + rd + opcode;
}


string gen_S_type(string op, vector<string> args)
{
    string opcode, funct3, imm, rs1, rs2;

    if (op == "sw")
        opcode = "0100011";
    else if (op == "fsw")
        opcode = "0100111";

    funct3 = "010";

    imm = num_to_bin(stoi(args[2]), 12);
    rs2 = reg_to_bin(args[0]);
    rs1 = reg_to_bin(args[1]);

    return imm.substr(0, 7) + rs2 + rs1 + funct3 + imm.substr(7) + opcode;
}

string gen_SB_type(string op, vector<string> args)
{
    string opcode, funct3, imm, rs1, rs2;

    opcode = "1100011";
    if (op == "beq")
        funct3 = "000";
    else if (op == "bne")
        funct3 = "001";
    else if (op == "blt")
        funct3 = "100";
    else if (op == "bge")
        funct3 = "101";
    else if (op == "bltu")
        funct3 = "110";
    else if (op == "bgeu")
        funct3 = "111";

    int32_t offset = calc_offset(args[2]);
    if (!(-4096 <= offset && offset < 4096)) {
        report_error("too far branch");
        report_cur_line();
        close_asm_and_exit();
    }
    imm = num_to_bin(offset, 13);
    rs1 = reg_to_bin(args[0]);
    rs2 = reg_to_bin(args[1]);

    return imm.substr(0, 1) + imm.substr(2, 6) + rs2 + rs1 + funct3 + imm.substr(8, 4) + imm.substr(1, 1) + opcode;
}

string gen_U_type(string op, vector<string> args)
{
    string opcode, imm, rd;

    if (op == "lui")
        opcode = "0110111";
    else if (op == "auipc")
        opcode = "0010111";

    imm = num_to_bin(stoul(args[1]) >> 12, 20);
    rd = reg_to_bin(args[0]);

    return imm + rd + opcode;
}

string gen_UJ_type(string op, vector<string> args)
{
    string opcode, imm, rd;

    opcode = "1101111";
    int32_t offset = calc_offset(args[1]);
    if (!(-524288 <= offset && offset < 524288)) {
        report_error("too far jump");
        report_cur_line();
        close_asm_and_exit();
    }
    imm = num_to_bin(offset, 21);
    rd = reg_to_bin(args[0]);

    return imm.substr(0, 1) + imm.substr(10, 10) + imm.substr(9, 1) + imm.substr(1, 8) + rd + opcode;
}

// special
string gen_SP_type(string op, vector<string> args)
{
    string opcode, rd, funct3, rs1;

    opcode = "0001011";

    if (op == "inb")
        funct3 = "001";
    else if (op == "outb")
        funct3 = "010";
    else
        funct3 = "100";

    if (op == "inb")
        rd = reg_to_bin(args[0]);
    else
        rd = "00000";

    if (op == "outb")
        rs1 = reg_to_bin(args[0]);
    else
        rs1 = "00000";

    return num_to_bin(0, 12) + rs1 + funct3 + rd + opcode;
}


void process_instruction(vector<string> elems)
{
    string op = elems[0];
    vector<string> args(elems.begin() + 1, elems.end());

    if (op == "lw" || op == "flw" || op == "sw" || op == "fsw") {
        string b = args.back();
        args.pop_back();
        vector<string> ps = split_string(b, "()");
        args.push_back(ps[1]); // reg comes first
        if (isdigit(ps[0][0]) || ps[0][0] == '-')
            args.push_back(ps[0]);
        else
            args.push_back(to_string(label_to_data_addr(ps[0])));
    } else if (op == "addi" || op == "lui") {
        string b = args.back();
        if (!(isdigit(b[0]) || b[0] == '-')) {
            args.pop_back();
            if (op == "addi") {
                uint32_t no_ext = label_to_text_addr(b) & 0xfff;
                uint32_t ext = (no_ext & 0x800) ? (no_ext | 0xfffff000) : no_ext;
                args.push_back(to_string((int32_t)ext));
            }
            else
                args.push_back(to_string(label_to_text_addr(b) & 0xfffff000));
        }
    }

    string inst;
    if (op == "slli" || op == "srli" || op == "srai" || op == "add" || op == "sub"
            || op == "sll" || op == "slt" || op == "sltu" || op == "xor"
            || op == "srl" || op == "sra" || op == "or" || op == "and"
            || op == "fadd.s" || op == "fsub.s" || op == "fmul.s" || op == "fdiv.s"
            || op == "fsqrt.s" || op == "fsgnj.s" || op == "fsgnjn.s" || op == "fsgnjx.s"
            || op == "fmin.s" || op == "fmax.s" || op == "fcvt.w.s" || op == "fcvt.wu.s"
            || op == "feq.s" || op == "flt.s" || op == "fle.s"
            || op == "fcvt.s.w" || op == "fcvt.s.wu" || op == "fmv.s.x")
        inst = gen_R_type(op, args);
    else if (op == "addi" || op == "slti" || op == "sltiu" || op == "xori" || op == "ori" || op == "andi"
            || op == "lw" || op == "flw" || op == "jalr")
        inst = gen_I_type(op, args);
    else if (op == "sw" || op == "fsw")
        inst = gen_S_type(op, args);
    else if (op == "beq" || op == "bne" || op == "blt" || op == "bge" || op == "bltu" || op == "bgeu")
        inst = gen_SB_type(op, args);
    else if (op == "lui" || op == "auipc")
        inst = gen_U_type(op, args);
    else if (op == "jal")
        inst = gen_UJ_type(op, args);
    else if (op == "inb" || op == "outb" || op == "halt")
        inst = gen_SP_type(op, args);
    else {
        report_error("invalid instruction");
        report_cur_line();
        close_asm_and_exit();
    }

    text_words.push_back(bin_to_word(inst));
}

