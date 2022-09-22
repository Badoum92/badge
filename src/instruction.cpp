#include "instruction.h"

#include <bit>
#include <cstdio>

#include "gameboy.h"

static void instr_nop(Gameboy&, const Instr&)
{}

static void instr_jp(Gameboy& gb, const Instr& instr)
{
    if (!gb.cpu.check_condition(instr.cond))
    {
        return;
    }
    gb.cpu.pc = instr.data;
}

static void instr_jr(Gameboy& gb, const Instr& instr)
{
    if (!gb.cpu.check_condition(instr.cond))
    {
        return;
    }
    int8_t data = bit_cast<int8_t>(instr.data);
    gb.cpu.pc += data;
}

static void instr_xor(Gameboy& gb, const Instr& instr)
{
    gb.cpu.a() ^= instr.data;
    gb.cpu.flag_z(gb.cpu.a() == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(0);
}

static void instr_ld8(Gameboy& gb, const Instr& instr)
{
    if (instr.mem_dst != 0)
    {
        gb.memory.write(instr.mem_dst, instr.data);
    }
    else
    {
        gb.cpu.get_reg8(instr.reg1) = instr.data;
    }
}

static void instr_ld16(Gameboy& gb, const Instr& instr)
{
    uint16_t data = instr.data;
    if (instr.op2 == Operand::SPR8)
    {
        data += gb.cpu.sp;
    }

    if (instr.mem_dst != 0)
    {
        gb.memory.write(instr.mem_dst, low_bits(data));
        gb.memory.write(instr.mem_dst + 1, high_bits(data));
    }
    else
    {
        gb.cpu.get_reg16(instr.reg1) = data;
    }
}

static void instr_ldh(Gameboy& gb, const Instr& instr)
{
    if (instr.mem_dst != 0)
    {
        gb.memory.write(instr.mem_dst, instr.data);
    }
    else
    {
        gb.cpu.a() = instr.data;
    }
}

static void instr_dec(Gameboy& gb, const Instr& instr)
{
    if (instr.mem_dst != 0)
    {
        gb.memory.write(instr.mem_dst, instr.data - 1);
    }
    else
    {
        gb.cpu.set_reg(instr.reg1, instr.data - 1);
    }
    gb.cpu.flag_z(instr.data - 1 == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((instr.data & 0x0f) == 0);
}

static void instr_ei_di(Gameboy& gb, const Instr& instr)
{
    gb.cpu.ime = instr.mnemonic == Mnemonic::EI;
}

static void instr_cp(Gameboy& gb, const Instr& instr)
{
    uint8_t res = gb.cpu.a() - instr.data;
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((0x0f & gb.cpu.a()) < (0x0f & instr.data));
    gb.cpu.flag_c(res > gb.cpu.a());
}

// clang-format off
Instr instructions[0x100] = {
    {{0x00, 0x00}, Mnemonic::NOP, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, instr_nop},
    {{0x01, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::BC, Operand::D16, Reg::NONE, nullptr},
    {{0x02, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::BC, Operand::R, Reg::A, nullptr},
    {{0x03, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::BC, Operand::NONE, Reg::NONE, nullptr},
    {{0x04, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::B, Operand::NONE, Reg::NONE, nullptr},
    {{0x05, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::B, Operand::NONE, Reg::NONE, instr_dec},
    {{0x06, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::D8, Reg::NONE, instr_ld8},
    {{0x07, 0x00}, Mnemonic::RLCA, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x08, 0x00}, Mnemonic::LD, Cond::NONE, Operand::A16, Reg::NONE, Operand::R, Reg::SP, nullptr},
    {{0x09, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::HL, Operand::R, Reg::BC, nullptr},
    {{0x0a, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::MR, Reg::BC, nullptr},
    {{0x0b, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::BC, Operand::NONE, Reg::NONE, instr_dec},
    {{0x0c, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::C, Operand::NONE, Reg::NONE, nullptr},
    {{0x0d, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::C, Operand::NONE, Reg::NONE, instr_dec},
    {{0x0e, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::D8, Reg::NONE, instr_ld8},
    {{0x0f, 0x00}, Mnemonic::RRCA, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x10, 0x00}, Mnemonic::STOP, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x11, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::DE, Operand::D16, Reg::NONE, nullptr},
    {{0x12, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::DE, Operand::R, Reg::A, nullptr},
    {{0x13, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::DE, Operand::NONE, Reg::NONE, nullptr},
    {{0x14, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::D, Operand::NONE, Reg::NONE, nullptr},
    {{0x15, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::D, Operand::NONE, Reg::NONE, instr_dec},
    {{0x16, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::D8, Reg::NONE, nullptr},
    {{0x17, 0x00}, Mnemonic::RLA, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x18, 0x00}, Mnemonic::JR, Cond::NONE, Operand::R8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x19, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::HL, Operand::R, Reg::DE, nullptr},
    {{0x1a, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::MR, Reg::DE, nullptr},
    {{0x1b, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::DE, Operand::NONE, Reg::NONE, instr_dec},
    {{0x1c, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::E, Operand::NONE, Reg::NONE, nullptr},
    {{0x1d, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::E, Operand::NONE, Reg::NONE, instr_dec},
    {{0x1e, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::D8, Reg::NONE, nullptr},
    {{0x1f, 0x00}, Mnemonic::RRA, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x20, 0x00}, Mnemonic::JR, Cond::NZ, Operand::R8, Reg::NONE, Operand::NONE, Reg::NONE, instr_jr},
    {{0x21, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::HL, Operand::D16, Reg::NONE, instr_ld16},
    {{0x22, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MRI, Reg::HL, Operand::R, Reg::A, nullptr},
    {{0x23, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0x24, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::H, Operand::NONE, Reg::NONE, nullptr},
    {{0x25, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::H, Operand::NONE, Reg::NONE, instr_dec},
    {{0x26, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::D8, Reg::NONE, nullptr},
    {{0x27, 0x00}, Mnemonic::DAA, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x28, 0x00}, Mnemonic::JR, Cond::Z, Operand::R8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x29, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::HL, Operand::R, Reg::HL, nullptr},
    {{0x2a, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::MR, Reg::HL, nullptr},
    {{0x2b, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::HL, Operand::NONE, Reg::NONE, instr_dec},
    {{0x2c, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::L, Operand::NONE, Reg::NONE, nullptr},
    {{0x2d, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::L, Operand::NONE, Reg::NONE, instr_dec},
    {{0x2e, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::D8, Reg::NONE, nullptr},
    {{0x2f, 0x00}, Mnemonic::CPL, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x30, 0x00}, Mnemonic::JR, Cond::NC, Operand::R8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x31, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::SP, Operand::D16, Reg::NONE, nullptr},
    {{0x32, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MRD, Reg::HL, Operand::R, Reg::A, instr_ld8},
    {{0x33, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::SP, Operand::NONE, Reg::NONE, nullptr},
    {{0x34, 0x00}, Mnemonic::INC, Cond::NONE, Operand::MR, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0x35, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::MR, Reg::HL, Operand::NONE, Reg::NONE, instr_dec},
    {{0x36, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::HL, Operand::D8, Reg::NONE, nullptr},
    {{0x37, 0x00}, Mnemonic::SCF, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x38, 0x00}, Mnemonic::JR, Cond::C, Operand::R8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x39, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::HL, Operand::R, Reg::SP, nullptr},
    {{0x3a, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::MRD, Reg::HL, nullptr},
    {{0x3b, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::SP, Operand::NONE, Reg::NONE, instr_dec},
    {{0x3c, 0x00}, Mnemonic::INC, Cond::NONE, Operand::R, Reg::A, Operand::NONE, Reg::NONE, nullptr},
    {{0x3d, 0x00}, Mnemonic::DEC, Cond::NONE, Operand::R, Reg::A, Operand::NONE, Reg::NONE, instr_dec},
    {{0x3e, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::D8, Reg::NONE, instr_ld8},
    {{0x3f, 0x00}, Mnemonic::CCF, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x40, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::R, Reg::B, nullptr},
    {{0x41, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::R, Reg::C, nullptr},
    {{0x42, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::R, Reg::D, nullptr},
    {{0x43, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::R, Reg::E, nullptr},
    {{0x44, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::R, Reg::H, nullptr},
    {{0x45, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::R, Reg::L, nullptr},
    {{0x46, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::MR, Reg::HL, nullptr},
    {{0x47, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::B, Operand::R, Reg::A, nullptr},
    {{0x48, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::R, Reg::B, nullptr},
    {{0x49, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::R, Reg::C, nullptr},
    {{0x4a, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::R, Reg::D, nullptr},
    {{0x4b, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::R, Reg::E, nullptr},
    {{0x4c, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::R, Reg::H, nullptr},
    {{0x4d, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::R, Reg::L, nullptr},
    {{0x4e, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::MR, Reg::HL, nullptr},
    {{0x4f, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::C, Operand::R, Reg::A, nullptr},
    {{0x50, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::R, Reg::B, nullptr},
    {{0x51, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::R, Reg::C, nullptr},
    {{0x52, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::R, Reg::D, nullptr},
    {{0x53, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::R, Reg::E, nullptr},
    {{0x54, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::R, Reg::H, nullptr},
    {{0x55, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::R, Reg::L, nullptr},
    {{0x56, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::MR, Reg::HL, nullptr},
    {{0x57, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::D, Operand::R, Reg::A, nullptr},
    {{0x58, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::R, Reg::B, nullptr},
    {{0x59, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::R, Reg::C, nullptr},
    {{0x5a, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::R, Reg::D, nullptr},
    {{0x5b, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::R, Reg::E, nullptr},
    {{0x5c, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::R, Reg::H, nullptr},
    {{0x5d, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::R, Reg::L, nullptr},
    {{0x5e, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::MR, Reg::HL, nullptr},
    {{0x5f, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::E, Operand::R, Reg::A, nullptr},
    {{0x60, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::R, Reg::B, nullptr},
    {{0x61, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::R, Reg::C, nullptr},
    {{0x62, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::R, Reg::D, nullptr},
    {{0x63, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::R, Reg::E, nullptr},
    {{0x64, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::R, Reg::H, nullptr},
    {{0x65, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::R, Reg::L, nullptr},
    {{0x66, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::MR, Reg::HL, nullptr},
    {{0x67, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::H, Operand::R, Reg::A, nullptr},
    {{0x68, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::R, Reg::B, nullptr},
    {{0x69, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::R, Reg::C, nullptr},
    {{0x6a, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::R, Reg::D, nullptr},
    {{0x6b, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::R, Reg::E, nullptr},
    {{0x6c, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::R, Reg::H, nullptr},
    {{0x6d, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::R, Reg::L, nullptr},
    {{0x6e, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::MR, Reg::HL, nullptr},
    {{0x6f, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::L, Operand::R, Reg::A, nullptr},
    {{0x70, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::HL, Operand::R, Reg::B, nullptr},
    {{0x71, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::HL, Operand::R, Reg::C, nullptr},
    {{0x72, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::HL, Operand::R, Reg::D, nullptr},
    {{0x73, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::HL, Operand::R, Reg::E, nullptr},
    {{0x74, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::HL, Operand::R, Reg::H, nullptr},
    {{0x75, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::HL, Operand::R, Reg::L, nullptr},
    {{0x76, 0x00}, Mnemonic::HALT, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0x77, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MR, Reg::HL, Operand::R, Reg::A, nullptr},
    {{0x78, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::B, nullptr},
    {{0x79, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::C, nullptr},
    {{0x7a, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::D, nullptr},
    {{0x7b, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::E, nullptr},
    {{0x7c, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::H, nullptr},
    {{0x7d, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::L, nullptr},
    {{0x7e, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::MR, Reg::HL, nullptr},
    {{0x7f, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::A, nullptr},
    {{0x80, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::B, nullptr},
    {{0x81, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::C, nullptr},
    {{0x82, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::D, nullptr},
    {{0x83, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::E, nullptr},
    {{0x84, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::H, nullptr},
    {{0x85, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::L, nullptr},
    {{0x86, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::MR, Reg::HL, nullptr},
    {{0x87, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::A, nullptr},
    {{0x88, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::B, nullptr},
    {{0x89, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::C, nullptr},
    {{0x8a, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::D, nullptr},
    {{0x8b, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::E, nullptr},
    {{0x8c, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::H, nullptr},
    {{0x8d, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::L, nullptr},
    {{0x8e, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::MR, Reg::HL, nullptr},
    {{0x8f, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::A, nullptr},
    {{0x90, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::R, Reg::B, Operand::NONE, Reg::NONE, nullptr},
    {{0x91, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::R, Reg::C, Operand::NONE, Reg::NONE, nullptr},
    {{0x92, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::R, Reg::D, Operand::NONE, Reg::NONE, nullptr},
    {{0x93, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::R, Reg::E, Operand::NONE, Reg::NONE, nullptr},
    {{0x94, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::R, Reg::H, Operand::NONE, Reg::NONE, nullptr},
    {{0x95, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::R, Reg::L, Operand::NONE, Reg::NONE, nullptr},
    {{0x96, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::MR, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0x97, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::R, Reg::A, Operand::NONE, Reg::NONE, nullptr},
    {{0x98, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::B, nullptr},
    {{0x99, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::C, nullptr},
    {{0x9a, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::D, nullptr},
    {{0x9b, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::E, nullptr},
    {{0x9c, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::H, nullptr},
    {{0x9d, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::L, nullptr},
    {{0x9e, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::MR, Reg::HL, nullptr},
    {{0x9f, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::R, Reg::A, nullptr},
    {{0xa0, 0x00}, Mnemonic::AND, Cond::NONE, Operand::R, Reg::B, Operand::NONE, Reg::NONE, nullptr},
    {{0xa1, 0x00}, Mnemonic::AND, Cond::NONE, Operand::R, Reg::C, Operand::NONE, Reg::NONE, nullptr},
    {{0xa2, 0x00}, Mnemonic::AND, Cond::NONE, Operand::R, Reg::D, Operand::NONE, Reg::NONE, nullptr},
    {{0xa3, 0x00}, Mnemonic::AND, Cond::NONE, Operand::R, Reg::E, Operand::NONE, Reg::NONE, nullptr},
    {{0xa4, 0x00}, Mnemonic::AND, Cond::NONE, Operand::R, Reg::H, Operand::NONE, Reg::NONE, nullptr},
    {{0xa5, 0x00}, Mnemonic::AND, Cond::NONE, Operand::R, Reg::L, Operand::NONE, Reg::NONE, nullptr},
    {{0xa6, 0x00}, Mnemonic::AND, Cond::NONE, Operand::MR, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0xa7, 0x00}, Mnemonic::AND, Cond::NONE, Operand::R, Reg::A, Operand::NONE, Reg::NONE, nullptr},
    {{0xa8, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::R, Reg::B, Operand::NONE, Reg::NONE, nullptr},
    {{0xa9, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::R, Reg::C, Operand::NONE, Reg::NONE, nullptr},
    {{0xaa, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::R, Reg::D, Operand::NONE, Reg::NONE, nullptr},
    {{0xab, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::R, Reg::E, Operand::NONE, Reg::NONE, nullptr},
    {{0xac, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::R, Reg::H, Operand::NONE, Reg::NONE, nullptr},
    {{0xad, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::R, Reg::L, Operand::NONE, Reg::NONE, nullptr},
    {{0xae, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::MR, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0xaf, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::R, Reg::A, Operand::NONE, Reg::NONE, instr_xor},
    {{0xb0, 0x00}, Mnemonic::OR, Cond::NONE, Operand::R, Reg::B, Operand::NONE, Reg::NONE, nullptr},
    {{0xb1, 0x00}, Mnemonic::OR, Cond::NONE, Operand::R, Reg::C, Operand::NONE, Reg::NONE, nullptr},
    {{0xb2, 0x00}, Mnemonic::OR, Cond::NONE, Operand::R, Reg::D, Operand::NONE, Reg::NONE, nullptr},
    {{0xb3, 0x00}, Mnemonic::OR, Cond::NONE, Operand::R, Reg::E, Operand::NONE, Reg::NONE, nullptr},
    {{0xb4, 0x00}, Mnemonic::OR, Cond::NONE, Operand::R, Reg::H, Operand::NONE, Reg::NONE, nullptr},
    {{0xb5, 0x00}, Mnemonic::OR, Cond::NONE, Operand::R, Reg::L, Operand::NONE, Reg::NONE, nullptr},
    {{0xb6, 0x00}, Mnemonic::OR, Cond::NONE, Operand::MR, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0xb7, 0x00}, Mnemonic::OR, Cond::NONE, Operand::R, Reg::A, Operand::NONE, Reg::NONE, nullptr},
    {{0xb8, 0x00}, Mnemonic::CP, Cond::NONE, Operand::R, Reg::B, Operand::NONE, Reg::NONE, nullptr},
    {{0xb9, 0x00}, Mnemonic::CP, Cond::NONE, Operand::R, Reg::C, Operand::NONE, Reg::NONE, nullptr},
    {{0xba, 0x00}, Mnemonic::CP, Cond::NONE, Operand::R, Reg::D, Operand::NONE, Reg::NONE, nullptr},
    {{0xbb, 0x00}, Mnemonic::CP, Cond::NONE, Operand::R, Reg::E, Operand::NONE, Reg::NONE, nullptr},
    {{0xbc, 0x00}, Mnemonic::CP, Cond::NONE, Operand::R, Reg::H, Operand::NONE, Reg::NONE, nullptr},
    {{0xbd, 0x00}, Mnemonic::CP, Cond::NONE, Operand::R, Reg::L, Operand::NONE, Reg::NONE, nullptr},
    {{0xbe, 0x00}, Mnemonic::CP, Cond::NONE, Operand::MR, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0xbf, 0x00}, Mnemonic::CP, Cond::NONE, Operand::R, Reg::A, Operand::NONE, Reg::NONE, nullptr},
    {{0xc0, 0x00}, Mnemonic::RET, Cond::NZ, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xc1, 0x00}, Mnemonic::POP, Cond::NONE, Operand::R, Reg::BC, Operand::NONE, Reg::NONE, nullptr},
    {{0xc2, 0x00}, Mnemonic::JP, Cond::NZ, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xc3, 0x00}, Mnemonic::JP, Cond::NONE, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, instr_jp},
    {{0xc4, 0x00}, Mnemonic::CALL, Cond::NZ, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xc5, 0x00}, Mnemonic::PUSH, Cond::NONE, Operand::R, Reg::BC, Operand::NONE, Reg::NONE, nullptr},
    {{0xc6, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::A, Operand::D8, Reg::NONE, nullptr},
    {{0xc7, 0x00}, Mnemonic::RST, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xc8, 0x00}, Mnemonic::RET, Cond::Z, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xc9, 0x00}, Mnemonic::RET, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xca, 0x00}, Mnemonic::JP, Cond::Z, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xcb, 0x00}, Mnemonic::PREFIX, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xcc, 0x00}, Mnemonic::CALL, Cond::Z, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xcd, 0x00}, Mnemonic::CALL, Cond::NONE, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xce, 0x00}, Mnemonic::ADC, Cond::NONE, Operand::R, Reg::A, Operand::D8, Reg::NONE, nullptr},
    {{0xcf, 0x00}, Mnemonic::RST, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd0, 0x00}, Mnemonic::RET, Cond::NC, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd1, 0x00}, Mnemonic::POP, Cond::NONE, Operand::R, Reg::DE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd2, 0x00}, Mnemonic::JP, Cond::NC, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd3, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd4, 0x00}, Mnemonic::CALL, Cond::NC, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd5, 0x00}, Mnemonic::PUSH, Cond::NONE, Operand::R, Reg::DE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd6, 0x00}, Mnemonic::SUB, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd7, 0x00}, Mnemonic::RST, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd8, 0x00}, Mnemonic::RET, Cond::C, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xd9, 0x00}, Mnemonic::RETI, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xda, 0x00}, Mnemonic::JP, Cond::C, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xdb, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xdc, 0x00}, Mnemonic::CALL, Cond::C, Operand::D16, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xdd, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xde, 0x00}, Mnemonic::SBC, Cond::NONE, Operand::R, Reg::A, Operand::D8, Reg::NONE, nullptr},
    {{0xdf, 0x00}, Mnemonic::RST, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xe0, 0x00}, Mnemonic::LDH, Cond::NONE, Operand::A8, Reg::NONE, Operand::R, Reg::A, instr_ldh},
    {{0xe1, 0x00}, Mnemonic::POP, Cond::NONE, Operand::R, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0xe2, 0x00}, Mnemonic::LD, Cond::NONE, Operand::MRIO, Reg::C, Operand::R, Reg::A, nullptr},
    {{0xe3, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xe4, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xe5, 0x00}, Mnemonic::PUSH, Cond::NONE, Operand::R, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0xe6, 0x00}, Mnemonic::AND, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xe7, 0x00}, Mnemonic::RST, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xe8, 0x00}, Mnemonic::ADD, Cond::NONE, Operand::R, Reg::SP, Operand::R8, Reg::NONE, nullptr},
    {{0xe9, 0x00}, Mnemonic::JP, Cond::NONE, Operand::MR, Reg::HL, Operand::NONE, Reg::NONE, nullptr},
    {{0xea, 0x00}, Mnemonic::LD, Cond::NONE, Operand::A16, Reg::NONE, Operand::R, Reg::A, nullptr},
    {{0xeb, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xec, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xed, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xee, 0x00}, Mnemonic::XOR, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xef, 0x00}, Mnemonic::RST, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xf0, 0x00}, Mnemonic::LDH, Cond::NONE, Operand::R, Reg::A, Operand::A8, Reg::NONE, instr_ldh},
    {{0xf1, 0x00}, Mnemonic::POP, Cond::NONE, Operand::R, Reg::AF, Operand::NONE, Reg::NONE, nullptr},
    {{0xf2, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::MRIO, Reg::C, nullptr},
    {{0xf3, 0x00}, Mnemonic::DI, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, instr_ei_di},
    {{0xf4, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xf5, 0x00}, Mnemonic::PUSH, Cond::NONE, Operand::R, Reg::AF, Operand::NONE, Reg::NONE, nullptr},
    {{0xf6, 0x00}, Mnemonic::OR, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xf7, 0x00}, Mnemonic::RST, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xf8, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::HL, Operand::SPR8, Reg::NONE, nullptr},
    {{0xf9, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::SP, Operand::R, Reg::HL, nullptr},
    {{0xfa, 0x00}, Mnemonic::LD, Cond::NONE, Operand::R, Reg::A, Operand::A16, Reg::NONE, nullptr},
    {{0xfb, 0x00}, Mnemonic::EI, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, instr_ei_di},
    {{0xfc, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xfd, 0x00}, Mnemonic::NONE, Cond::NONE, Operand::NONE, Reg::NONE, Operand::NONE, Reg::NONE, nullptr},
    {{0xfe, 0x00}, Mnemonic::CP, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, instr_cp},
    {{0xff, 0x00}, Mnemonic::RST, Cond::NONE, Operand::D8, Reg::NONE, Operand::NONE, Reg::NONE, nullptr}
};
// clang-format on

std::string to_string(const Gameboy& gb, const Instr& instr)
{
    auto op_to_str = [](const Gameboy& gb, const Instr& instr, Operand op, Reg reg, char* buf) {
        switch (op)
        {
        case Operand::NONE:
            return 0;
        case Operand::R:
            return sprintf(buf, " %s", reg_str[reg]);
        case Operand::MR:
            return sprintf(buf, " [%s]", reg_str[reg]);
        case Operand::MRI:
            return sprintf(buf, " [HL+]");
        case Operand::MRD:
            return sprintf(buf, " [HL-]");
        case Operand::MRIO:
            return sprintf(buf, " [0xff00+%s]", reg_str[reg]);
        case Operand::D8:
            return sprintf(buf, " 0x%02x", instr.data);
        case Operand::D16:
            return sprintf(buf, " 0x%04x", instr.data);
        case Operand::R8: {
            int8_t data = bit_cast<int8_t>(instr.data);
            if (instr.mnemonic == Mnemonic::JR)
            {
                return sprintf(buf, " 0x%04x", gb.cpu.pc + data);
            }
            else
            {
                return sprintf(buf, " %d", data);
            }
        }
        case Operand::A8: {
            uint16_t addr = instr.mem_dst != 0 ? instr.mem_dst : instr.mem_src;
            return sprintf(buf, " [0xff00+0x%02x]", addr - 0xff00);
        }
        case Operand::A16:
            return sprintf(buf, " [0x%04x]", instr.mem_dst);
        case Operand::SPR8: {
            int8_t data = bit_cast<int8_t>(instr.data);
            return sprintf(buf, " %d", data);
        }
        default:
            return 0;
        }
    };

    char buf[64] = {};
    size_t offset = 0;

    if (instr.func == nullptr)
    {
        offset += sprintf(buf + offset, "!");
    }

    if (instr.mnemonic == Mnemonic::NONE)
    {
        return "!?";
    }

    offset += sprintf(buf + offset, "%s", mnemonic_str[instr.mnemonic]);

    if (instr.cond != Cond::NONE)
    {
        offset += sprintf(buf + offset, " %s", cond_str[instr.cond]);
    }

    if (instr.op1 != Operand::NONE)
    {
        offset += op_to_str(gb, instr, instr.op1, instr.reg1, buf + offset);
    }

    if (instr.op2 != Operand::NONE)
    {
        offset += sprintf(buf + offset, ",");
        offset += op_to_str(gb, instr, instr.op2, instr.reg2, buf + offset);
    }

    return buf;
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*====================================================================================================================*/
/*====================================================================================================================*/
/*====================================================================================================================*/
/*====================================================================================================================*/
/*====================================================================================================================*/

static uint32_t instr_nop(Gameboy&, const _Instr&)
{
    return 1;
}

static uint32_t instr_ld_r16_d16(Gameboy& gb, const _Instr& instr)
{
    uint16_t data = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    gb.cpu.set_reg(instr.r1, data);
    return 3;
}

static uint32_t instr_ld_mr_r8(Gameboy& gb, const _Instr& instr)
{
    gb.memory.write(gb.cpu.get_reg16(instr.r1), gb.cpu.get_reg8(instr.r2));
    return 2;
}

static uint32_t instr_inc_r16(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.cpu.get_reg16(instr.r1) + 1);
    return 2;
}

static uint32_t instr_inc_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t data = gb.cpu.get_reg8(instr.r1);
    gb.cpu.set_reg(instr.r1, data + 1);
    gb.cpu.flag_z(data + 1 == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h((data & 0x0f) == 0x0f);
    return 1;
}

static uint32_t instr_dec_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t data = gb.cpu.get_reg8(instr.r1);
    gb.cpu.set_reg(instr.r1, data - 1);
    gb.cpu.flag_z(data - 1 == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((data & 0x0f) == 0);
}

static uint32_t instr_ld_r8_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.cpu.set_reg(instr.r1, data);
    return 2;
}

static uint32_t instr_rlca(Gameboy& gb, const _Instr&)
{
    uint8_t data = gb.cpu.a();
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x80);
    data = std::rotl(data, 1);
    gb.cpu.a() = data;
    return 1;
}

static uint32_t instr_ld_a16_r16(Gameboy& gb, const _Instr& instr)
{
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    gb.memory.write16(addr, gb.cpu.get_reg16(instr.r1));
    return 5;
}

static uint32_t instr_add_r16_r16(Gameboy& gb, const _Instr& instr)
{
    uint32_t x = gb.cpu.get_reg16(instr.r1);
    uint32_t y = gb.cpu.get_reg16(instr.r2);
    uint32_t res = x + y;
    gb.cpu.set_reg(instr.r1, res & 0xffff);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(((x & 0x0fff) + (y & 0x0fff)) & 0x1000);
    gb.cpu.flag_c(res > 0xffff);
    return 2;
}

static uint32_t instr_ld_r8_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.get_reg16(instr.r2));
    gb.cpu.set_reg(instr.r1, data);
    return 2;
}

static uint32_t instr_dec_r16(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.cpu.get_reg16(instr.r1) - 1);
    return 2;
}

static uint32_t instr_rrca(Gameboy& gb, const _Instr&)
{
    uint8_t data = gb.cpu.a();
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x01);
    data = std::rotr(data, 1);
    gb.cpu.a() = data;
    return 1;
}

static uint32_t instr_stop(Gameboy& gb, const _Instr&)
{
    gb.cpu.pc += 1;
    gb.cpu.stopped = true;
    return 1;
}

static uint32_t instr_rla(Gameboy& gb, const _Instr&)
{
    uint8_t c = gb.cpu.flag_c();
    gb.cpu.flag_c(gb.cpu.a() & 0x80);
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.a() = (gb.cpu.a() << 1) | c;
    return 1;
}

static uint32_t instr_jr_s8(Gameboy& gb, const _Instr& instr)
{
    int8_t data = bit_cast<int8_t>(gb.memory.read(gb.cpu.pc));
    gb.cpu.pc += 1;
    if (!gb.cpu.check_condition(instr.cond))
    {
        return 2;
    }
    gb.cpu.pc += data;
    return 3;
}

static uint32_t instr_rra(Gameboy& gb, const _Instr&)
{
    uint8_t c = gb.cpu.flag_c();
    gb.cpu.flag_c(gb.cpu.a() & 0x01);
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.a() = (gb.cpu.a() >> 1) | (c << 7);
    return 1;
}

static uint32_t instr_ld_hli_r8(Gameboy& gb, const _Instr&)
{
    gb.memory.write(gb.cpu.hl(), gb.cpu.a());
    gb.cpu.hl() += 1;
    return 2;
}

static uint32_t instr_daa(Gameboy& gb, const _Instr&)
{
    uint8_t a = gb.cpu.a();
    if (!gb.cpu.flag_n())
    {
        if (gb.cpu.flag_c() || a > 0x99)
        {
            a += 0x60;
            gb.cpu.flag_c(1);
        }
        if (gb.cpu.flag_h() || (a & 0x0f) > 0x09)
        {
            a += 0x06;
        }
    }
    else
    {
        if (gb.cpu.flag_c())
        {
            a -= 0x60;
        }
        if (gb.cpu.flag_h())
        {
            a -= 0x06;
        }
    }
    gb.cpu.flag_z(a == 0);
    gb.cpu.flag_h(0);
    gb.cpu.a() = a;
    return 1;
}

static uint32_t instr_ld_r8_hli(Gameboy& gb, const _Instr&)
{
    gb.cpu.a() = gb.memory.read(gb.cpu.hl());
    gb.cpu.hl() += 1;
    return 2;
}

static uint32_t instr_cpl(Gameboy& gb, const _Instr&)
{
    gb.cpu.a() ^= 0xff;
    gb.cpu.flag_n(1);
    gb.cpu.flag_h(1);
    return 1;
}

static uint32_t instr_ld_hld_r8(Gameboy& gb, const _Instr&)
{
    gb.memory.write(gb.cpu.hl(), gb.cpu.a());
    gb.cpu.hl() -= 1;
    return 2;
}

static uint32_t instr_inc_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    gb.memory.write(gb.cpu.get_reg16(instr.r1), data + 1);
    gb.cpu.flag_z(data + 1 == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h((data & 0x0f) == 0x0f);
    return 3;
}

static uint32_t instr_dec_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    gb.memory.write(gb.cpu.get_reg16(instr.r1), data - 1);
    gb.cpu.flag_z(data - 1 == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((data & 0x0f) == 0);
    return 3;
}

static uint32_t instr_ld_mr_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.memory.write(gb.cpu.get_reg16(instr.r1), data);
    return 3;
}

static uint32_t instr_ld_scf(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(1);
    return 1;
}

static uint32_t instr_ld_r8_hld(Gameboy& gb, const _Instr&)
{
    gb.cpu.a() = gb.memory.read(gb.cpu.hl());
    gb.cpu.hl() -= 1;
    return 2;
}

static uint32_t instr_ccf(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(gb.cpu.flag_c() ^ 1);
    return 1;
}

static uint32_t instr_ld_r8_r8(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.cpu.get_reg8(instr.r2));
    return 1;
}

static uint32_t instr_halt(Gameboy& gb, const _Instr&)
{
    gb.cpu.halted = true;
    return 1;
}

static void _instr_add(Gameboy& gb, uint16_t x, uint16_t y, Reg r)
{
    uint16_t res = x + y;
    gb.cpu.set_reg(r, res & 0xff);
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(((x & 0x0f) + (y & 0x0f)) & 0x10);
    gb.cpu.flag_c(res > 0xff);
}

static uint32_t instr_add_r8_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.cpu.get_reg8(instr.r2);
    _instr_add(gb, x, y, instr.r1);
    return 1;
}

static uint32_t instr_add_r8_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r2));
    _instr_add(gb, x, y, instr.r1);
    return 2;
}

static uint32_t instr_add_r8_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_add(gb, x, y, instr.r1);
    return 2;
}

static void _instr_adc(Gameboy& gb, uint16_t x, uint16_t y, Reg r)
{
    uint16_t res = x + y + gb.cpu.flag_c();
    gb.cpu.set_reg(r, res);
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(((x & 0x0f) + (y & 0x0f) + gb.cpu.flag_c()) & 0x10);
    gb.cpu.flag_c(res > 0xff);
}

static uint32_t instr_adc_r8_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.cpu.get_reg8(instr.r2);
    _instr_adc(gb, x, y, instr.r1);
    return 1;
}

static uint32_t instr_adc_r8_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r2));
    _instr_adc(gb, x, y, instr.r1);
    return 2;
}

static uint32_t instr_adc_r8_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_adc(gb, x, y, instr.r1);
    return 2;
}

static void _instr_sub(Gameboy& gb, uint16_t x, uint16_t y)
{
    uint16_t res = x - y;
    gb.cpu.a() = res;
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((x & 0x0f) < (y & 0x0f));
    gb.cpu.flag_c(x < y);
}

static uint32_t instr_sub_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_sub(gb, x, y);
    return 1;
}

static uint32_t instr_sub_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_sub(gb, x, y);
    return 2;
}

static uint32_t instr_sub_r8_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_sub(gb, x, y);
    return 2;
}

static void _instr_sbc(Gameboy& gb, uint16_t x, uint16_t y, Reg r)
{
    uint16_t res = x - (y + gb.cpu.flag_c());
    gb.cpu.set_reg(r, res);
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((x & 0x0f) < (y & 0x0f) + gb.cpu.flag_c());
    gb.cpu.flag_c(x < (y & 0x0f) + gb.cpu.flag_c());
}

static uint32_t instr_sbc_r8_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_sbc(gb, x, y, instr.r1);
    return 1;
}

static uint32_t instr_sbc_r8_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_sbc(gb, x, y, instr.r1);
    return 2;
}

static uint32_t instr_sbc_r8_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_sbc(gb, x, y, instr.r1);
    return 2;
}

static void _instr_and(Gameboy& gb, uint8_t x, uint8_t y)
{
    uint8_t res = x & y;
    gb.cpu.a() = res;
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(1);
    gb.cpu.flag_c(0);
}

static uint32_t instr_and_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_and(gb, x, y);
    return 1;
}

static uint32_t instr_and_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_and(gb, x, y);
    return 2;
}

static uint32_t instr_and_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_and(gb, x, y);
    return 2;
}

static void _instr_xor(Gameboy& gb, uint8_t x, uint8_t y)
{
    uint8_t res = x ^ y;
    gb.cpu.a() = res;
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(0);
}

static uint32_t instr_xor_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_xor(gb, x, y);
    return 1;
}

static uint32_t instr_xor_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_xor(gb, x, y);
    return 2;
}

static uint32_t instr_xor_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_xor(gb, x, y);
    return 2;
}

static void _instr_or(Gameboy& gb, uint8_t x, uint8_t y)
{
    uint8_t res = x | y;
    gb.cpu.a() = res;
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(0);
}

static uint32_t instr_or_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_or(gb, x, y);
    return 1;
}

static uint32_t instr_or_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_or(gb, x, y);
    return 2;
}

static uint32_t instr_or_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_or(gb, x, y);
    return 2;
}

static void _instr_cp(Gameboy& gb, uint16_t x, uint16_t y)
{
    uint16_t res = x - y;
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((x & 0x0f) < (y & 0x0f));
    gb.cpu.flag_c(x < y);
}

static uint32_t instr_cp_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_cp(gb, x, y);
    return 1;
}

static uint32_t instr_cp_mr(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_cp(gb, x, y);
    return 2;
}

static uint32_t instr_cp_d8(Gameboy& gb, const _Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_cp(gb, x, y);
    return 2;
}

static uint32_t instr_ret(Gameboy& gb, const _Instr& instr)
{
    if (!gb.cpu.check_condition(instr.cond))
    {
        return 5;
    }
    gb.cpu.pc = gb.memory.read16(gb.cpu.sp);
    gb.cpu.sp += 2;
    return instr.cond == Cond::NONE ? 4 : 2;
}

static uint32_t instr_pop(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.memory.read16(gb.cpu.sp));
    gb.cpu.sp += 2;
    return 3;
}

static uint32_t instr_jp_a16(Gameboy& gb, const _Instr& instr)
{
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    if (!gb.cpu.check_condition(instr.cond))
    {
        return 3;
    }
    gb.cpu.pc = addr;
    return 4;
}

static uint32_t instr_call(Gameboy& gb, const _Instr& instr)
{
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    if (!gb.cpu.check_condition(instr.cond))
    {
        return 3;
    }
    gb.cpu.sp -= 2;
    gb.memory.write16(gb.cpu.sp, gb.cpu.pc);
    gb.cpu.pc = addr;
    return 6;
}

static uint32_t instr_push(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.sp -= 2;
    gb.memory.write16(gb.cpu.sp, gb.cpu.get_reg16(instr.r1));
    return 4;
}

static uint32_t instr_rst(Gameboy& gb, const _Instr& instr)
{
    uint8_t addr = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.cpu.sp -= 2;
    gb.memory.write16(gb.cpu.sp, gb.cpu.pc);
    gb.cpu.pc = addr;
    return 4;
}

static uint32_t instr_prefix_cb(Gameboy& gb, const _Instr& instr)
{
    ASSERT(!"0xCB instructions not implemented");
}

static uint32_t instr_reti(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.ime = true;
    return instr_ret(gb, instr);
}

static uint32_t instr_ldh_a8_r8(Gameboy& gb, const _Instr& instr)
{
    uint8_t port = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.memory.write(0xff00 + port, gb.cpu.get_reg8(instr.r1));
    return 3;
}

static uint32_t instr_ldh_r8_a8(Gameboy& gb, const _Instr& instr)
{
    uint8_t port = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.cpu.set_reg(instr.r1, gb.memory.read(0xff00 + port));
    return 3;
}

static uint32_t instr_ldh_mr_r8(Gameboy& gb, const _Instr& instr)
{
    gb.memory.write(0xff00 + gb.cpu.get_reg8(instr.r1), gb.cpu.get_reg8(instr.r2));
    return 2;
}

static uint32_t instr_ldh_r8_mr(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.memory.read(0xff00 + gb.cpu.get_reg8(instr.r2)));
    return 2;
}

static uint32_t instr_ld_a16_r8(Gameboy& gb, const _Instr& instr)
{
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    gb.memory.write16(addr, gb.cpu.get_reg8(instr.r1));
    return 4;
}

static uint32_t instr_ld_r8_a16(Gameboy& gb, const _Instr& instr)
{
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    gb.cpu.set_reg(instr.r1, gb.memory.read(addr));
    return 4;
}

static uint32_t instr_add_r16_s8(Gameboy& gb, const _Instr& instr)
{
    int8_t data = bit_cast<int8_t>(gb.memory.read(gb.cpu.pc));
    gb.cpu.pc += 1;
    uint16_t r = gb.cpu.get_reg16(instr.r1);
    int32_t res = r + data;
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    if (data < 0)
    {
        gb.cpu.flag_h((r & 0x0fff) < -data);
        gb.cpu.flag_c(r < -data);
    }
    if (data > 0)
    {
        gb.cpu.flag_h(((r & 0x0fff) + data) & 0x1000);
        gb.cpu.flag_c(res > 0xffff);
    }
    gb.cpu.set_reg(instr.r1, res);
    return 4;
}

static uint32_t instr_jp_r16(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.pc = gb.cpu.get_reg16(instr.r1);
    return 1;
}

static uint32_t instr_ei(Gameboy& gb, const _Instr&)
{
    gb.cpu.ime = true;
    return 1;
}

static uint32_t instr_di(Gameboy& gb, const _Instr&)
{
    gb.cpu.ime = false;
    return 1;
}

static uint32_t instr_ld_r16_r16s8(Gameboy& gb, const _Instr& instr)
{
    int8_t data = bit_cast<int8_t>(gb.memory.read(gb.cpu.pc));
    gb.cpu.pc += 1;
    uint16_t r = gb.cpu.get_reg16(instr.r2);
    int32_t res = r + data;
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    if (data < 0)
    {
        gb.cpu.flag_h((r & 0x0fff) < -data);
        gb.cpu.flag_c(r < -data);
    }
    if (data > 0)
    {
        gb.cpu.flag_h(((r & 0x0fff) + data) & 0x1000);
        gb.cpu.flag_c(res > 0xffff);
    }
    gb.cpu.set_reg(instr.r1, res);
    return 3;
}

static uint32_t instr_ld_r16_r16(Gameboy& gb, const _Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.cpu.get_reg16(instr.r2));
    return 2;
}

_Instr _instructions[0x100] = {
    /*0x00*/ {instr_nop, nullptr},
    /*0x01*/ {instr_ld_r16_d16, nullptr, Reg::BC},
    /*0x02*/ {instr_ld_mr_r8, nullptr, Reg::BC, Reg::A},
    /*0x03*/ {instr_inc_r16, nullptr, Reg::BC},
    /*0x04*/ {instr_inc_r8, nullptr, Reg::B},
    /*0x05*/ {instr_dec_r8, nullptr, Reg::B},
    /*0x06*/ {instr_ld_r8_d8, nullptr, Reg::B},
    /*0x07*/ {instr_rlca, nullptr},
    /*0x08*/ {instr_ld_a16_r16, nullptr, Reg::SP},
    /*0x09*/ {instr_add_r16_r16, nullptr, Reg::HL, Reg::BC},
    /*0x0a*/ {instr_ld_r8_mr, nullptr, Reg::A, Reg::BC},
    /*0x0b*/ {instr_dec_r16, nullptr, Reg::BC},
    /*0x0c*/ {instr_inc_r8, nullptr, Reg::C},
    /*0x0d*/ {instr_dec_r8, nullptr, Reg::C},
    /*0x0e*/ {instr_ld_r8_d8, nullptr, Reg::C},
    /*0x0f*/ {instr_rrca, nullptr},
    /*0x10*/ {instr_stop, nullptr},
    /*0x11*/ {instr_ld_r16_d16, nullptr, Reg::DE},
    /*0x12*/ {instr_ld_mr_r8, nullptr, Reg::DE, Reg::A},
    /*0x13*/ {instr_inc_r16, nullptr, Reg::DE},
    /*0x14*/ {instr_inc_r8, nullptr, Reg::D},
    /*0x15*/ {instr_dec_r8, nullptr, Reg::D},
    /*0x16*/ {instr_ld_r8_d8, nullptr, Reg::D},
    /*0x17*/ {instr_rla, nullptr},
    /*0x18*/ {instr_jr_s8, nullptr},
    /*0x19*/ {instr_add_r16_r16, nullptr, Reg::HL, Reg::DE},
    /*0x1a*/ {instr_ld_r8_mr, nullptr, Reg::A, Reg::DE},
    /*0x1b*/ {instr_dec_r16, nullptr, Reg::DE},
    /*0x1c*/ {instr_inc_r8, nullptr, Reg::E},
    /*0x1d*/ {instr_dec_r8, nullptr, Reg::E},
    /*0x1e*/ {instr_ld_r8_d8, nullptr, Reg::E},
    /*0x1f*/ {instr_rra, nullptr},
    /*0x20*/ {},
    /*0x21*/ {},
    /*0x22*/ {},
    /*0x23*/ {},
    /*0x24*/ {},
    /*0x25*/ {},
    /*0x26*/ {},
    /*0x27*/ {},
    /*0x28*/ {},
    /*0x29*/ {},
    /*0x2a*/ {},
    /*0x2b*/ {},
    /*0x2c*/ {},
    /*0x2d*/ {},
    /*0x2e*/ {},
    /*0x2f*/ {},
    /*0x30*/ {},
    /*0x31*/ {},
    /*0x32*/ {},
    /*0x33*/ {},
    /*0x34*/ {},
    /*0x35*/ {},
    /*0x36*/ {},
    /*0x37*/ {},
    /*0x38*/ {},
    /*0x39*/ {},
    /*0x3a*/ {},
    /*0x3b*/ {},
    /*0x3c*/ {},
    /*0x3d*/ {},
    /*0x3e*/ {},
    /*0x3f*/ {},
    /*0x40*/ {},
    /*0x41*/ {},
    /*0x42*/ {},
    /*0x43*/ {},
    /*0x44*/ {},
    /*0x45*/ {},
    /*0x46*/ {},
    /*0x47*/ {},
    /*0x48*/ {},
    /*0x49*/ {},
    /*0x4a*/ {},
    /*0x4b*/ {},
    /*0x4c*/ {},
    /*0x4d*/ {},
    /*0x4e*/ {},
    /*0x4f*/ {},
    /*0x50*/ {},
    /*0x51*/ {},
    /*0x52*/ {},
    /*0x53*/ {},
    /*0x54*/ {},
    /*0x55*/ {},
    /*0x56*/ {},
    /*0x57*/ {},
    /*0x58*/ {},
    /*0x59*/ {},
    /*0x5a*/ {},
    /*0x5b*/ {},
    /*0x5c*/ {},
    /*0x5d*/ {},
    /*0x5e*/ {},
    /*0x5f*/ {},
    /*0x60*/ {},
    /*0x61*/ {},
    /*0x62*/ {},
    /*0x63*/ {},
    /*0x64*/ {},
    /*0x65*/ {},
    /*0x66*/ {},
    /*0x67*/ {},
    /*0x68*/ {},
    /*0x69*/ {},
    /*0x6a*/ {},
    /*0x6b*/ {},
    /*0x6c*/ {},
    /*0x6d*/ {},
    /*0x6e*/ {},
    /*0x6f*/ {},
    /*0x70*/ {},
    /*0x71*/ {},
    /*0x72*/ {},
    /*0x73*/ {},
    /*0x74*/ {},
    /*0x75*/ {},
    /*0x76*/ {},
    /*0x77*/ {},
    /*0x78*/ {},
    /*0x79*/ {},
    /*0x7a*/ {},
    /*0x7b*/ {},
    /*0x7c*/ {},
    /*0x7d*/ {},
    /*0x7e*/ {},
    /*0x7f*/ {},
    /*0x80*/ {},
    /*0x81*/ {},
    /*0x82*/ {},
    /*0x83*/ {},
    /*0x84*/ {},
    /*0x85*/ {},
    /*0x86*/ {},
    /*0x87*/ {},
    /*0x88*/ {},
    /*0x89*/ {},
    /*0x8a*/ {},
    /*0x8b*/ {},
    /*0x8c*/ {},
    /*0x8d*/ {},
    /*0x8e*/ {},
    /*0x8f*/ {},
    /*0x90*/ {},
    /*0x91*/ {},
    /*0x92*/ {},
    /*0x93*/ {},
    /*0x94*/ {},
    /*0x95*/ {},
    /*0x96*/ {},
    /*0x97*/ {},
    /*0x98*/ {},
    /*0x99*/ {},
    /*0x9a*/ {},
    /*0x9b*/ {},
    /*0x9c*/ {},
    /*0x9d*/ {},
    /*0x9e*/ {},
    /*0x9f*/ {},
    /*0xa0*/ {},
    /*0xa1*/ {},
    /*0xa2*/ {},
    /*0xa3*/ {},
    /*0xa4*/ {},
    /*0xa5*/ {},
    /*0xa6*/ {},
    /*0xa7*/ {},
    /*0xa8*/ {},
    /*0xa9*/ {},
    /*0xaa*/ {},
    /*0xab*/ {},
    /*0xac*/ {},
    /*0xad*/ {},
    /*0xae*/ {},
    /*0xaf*/ {},
    /*0xb0*/ {},
    /*0xb1*/ {},
    /*0xb2*/ {},
    /*0xb3*/ {},
    /*0xb4*/ {},
    /*0xb5*/ {},
    /*0xb6*/ {},
    /*0xb7*/ {},
    /*0xb8*/ {},
    /*0xb9*/ {},
    /*0xba*/ {},
    /*0xbb*/ {},
    /*0xbc*/ {},
    /*0xbd*/ {},
    /*0xbe*/ {},
    /*0xbf*/ {},
    /*0xc0*/ {},
    /*0xc1*/ {},
    /*0xc2*/ {},
    /*0xc3*/ {},
    /*0xc4*/ {},
    /*0xc5*/ {},
    /*0xc6*/ {},
    /*0xc7*/ {},
    /*0xc8*/ {},
    /*0xc9*/ {},
    /*0xca*/ {},
    /*0xcb*/ {},
    /*0xcc*/ {},
    /*0xcd*/ {},
    /*0xce*/ {},
    /*0xcf*/ {},
    /*0xd0*/ {},
    /*0xd1*/ {},
    /*0xd2*/ {},
    /*0xd3*/ {},
    /*0xd4*/ {},
    /*0xd5*/ {},
    /*0xd6*/ {},
    /*0xd7*/ {},
    /*0xd8*/ {},
    /*0xd9*/ {},
    /*0xda*/ {},
    /*0xdb*/ {},
    /*0xdc*/ {},
    /*0xdd*/ {},
    /*0xde*/ {},
    /*0xdf*/ {},
    /*0xe0*/ {},
    /*0xe1*/ {},
    /*0xe2*/ {},
    /*0xe3*/ {},
    /*0xe4*/ {},
    /*0xe5*/ {},
    /*0xe6*/ {},
    /*0xe7*/ {},
    /*0xe8*/ {},
    /*0xe9*/ {},
    /*0xea*/ {},
    /*0xeb*/ {},
    /*0xec*/ {},
    /*0xed*/ {},
    /*0xee*/ {},
    /*0xef*/ {},
    /*0xf0*/ {},
    /*0xf1*/ {},
    /*0xf2*/ {},
    /*0xf3*/ {},
    /*0xf4*/ {},
    /*0xf5*/ {},
    /*0xf6*/ {},
    /*0xf7*/ {},
    /*0xf8*/ {},
    /*0xf9*/ {},
    /*0xfa*/ {},
    /*0xfb*/ {},
    /*0xfc*/ {},
    /*0xfd*/ {},
    /*0xfe*/ {},
    /*0xff*/ {}};
