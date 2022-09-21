#include "instruction.h"

#include <cstdio>

#include "gameboy.h"

static void instr_nop(Gameboy&, const Instruction&)
{
    printf("nop\n");
}

static void instr_jp(Gameboy& gb, const Instruction& instruction)
{
    printf("jp\n");
    if (!gb.cpu.check_condition(instruction.cond))
    {
        return;
    }
    gb.cpu.pc = instruction.data;
}

static void instr_xor(Gameboy& gb, const Instruction& instruction)
{
    printf("xor\n");
    gb.cpu.a() ^= instruction.data;
    gb.cpu.flag_z(gb.cpu.a() == 0);
}

static void instr_ld16(Gameboy& gb, const Instruction& instruction)
{
    printf("ld16\n");
    if (instruction.memory_dst != 0)
    {
        gb.memory.write(instruction.memory_dst, low_bits(instruction.data));
        gb.memory.write(instruction.memory_dst + 1, high_bits(instruction.data));
    }
    else
    {
        gb.cpu.get_reg16(instruction.reg1) = instruction.data;
    }
}

Instruction Instruction::instructions[0x100] = {
    /*0x00*/ {Type::NOP, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, instr_nop},
    /*0x01*/ {Type::LD, Addressing::R_D16, Register::BC, Register::NONE, Condition::NONE, instr_ld16},
    /*0x02*/ {Type::LD, Addressing::MR_R, Register::BC, Register::A, Condition::NONE, nullptr},
    /*0x03*/ {Type::INC, Addressing::R, Register::BC, Register::NONE, Condition::NONE, nullptr},
    /*0x04*/ {Type::INC, Addressing::R, Register::B, Register::NONE, Condition::NONE, nullptr},
    /*0x05*/ {Type::DEC, Addressing::R, Register::B, Register::NONE, Condition::NONE, nullptr},
    /*0x06*/ {Type::LD, Addressing::R_D8, Register::B, Register::NONE, Condition::NONE, nullptr},
    /*0x07*/ {Type::RLCA, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x08*/ {Type::LD, Addressing::A16_R, Register::SP, Register::NONE, Condition::NONE, nullptr},
    /*0x09*/ {Type::ADD, Addressing::R_R, Register::HL, Register::BC, Condition::NONE, nullptr},
    /*0x0a*/ {Type::LD, Addressing::R_MR, Register::A, Register::BC, Condition::NONE, nullptr},
    /*0x0b*/ {Type::DEC, Addressing::R, Register::BC, Register::NONE, Condition::NONE, nullptr},
    /*0x0c*/ {Type::INC, Addressing::R, Register::C, Register::NONE, Condition::NONE, nullptr},
    /*0x0d*/ {Type::DEC, Addressing::R, Register::C, Register::NONE, Condition::NONE, nullptr},
    /*0x0e*/ {Type::LD, Addressing::R_D8, Register::C, Register::NONE, Condition::NONE, nullptr},
    /*0x0f*/ {Type::RRCA, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x10*/ {Type::STOP, Addressing::D8, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x11*/ {Type::LD, Addressing::R_D16, Register::DE, Register::NONE, Condition::NONE, nullptr},
    /*0x12*/ {Type::LD, Addressing::MR_R, Register::DE, Register::A, Condition::NONE, nullptr},
    /*0x13*/ {Type::INC, Addressing::R, Register::DE, Register::NONE, Condition::NONE, nullptr},
    /*0x14*/ {Type::INC, Addressing::R, Register::D, Register::NONE, Condition::NONE, nullptr},
    /*0x15*/ {Type::DEC, Addressing::R, Register::D, Register::NONE, Condition::NONE, nullptr},
    /*0x16*/ {Type::LD, Addressing::R_D8, Register::D, Register::NONE, Condition::NONE, nullptr},
    /*0x17*/ {Type::RLA, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x18*/ {Type::JR, Addressing::R8, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x19*/ {Type::ADD, Addressing::R_R, Register::HL, Register::DE, Condition::NONE, nullptr},
    /*0x1a*/ {Type::LD, Addressing::R_MR, Register::A, Register::DE, Condition::NONE, nullptr},
    /*0x1b*/ {Type::DEC, Addressing::R, Register::DE, Register::NONE, Condition::NONE, nullptr},
    /*0x1c*/ {Type::INC, Addressing::R, Register::E, Register::NONE, Condition::NONE, nullptr},
    /*0x1d*/ {Type::DEC, Addressing::R, Register::E, Register::NONE, Condition::NONE, nullptr},
    /*0x1e*/ {Type::LD, Addressing::R_D8, Register::E, Register::NONE, Condition::NONE, nullptr},
    /*0x1f*/ {Type::RRA, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x20*/ {Type::JR, Addressing::R8, Register::NONE, Register::NONE, Condition::NZ, nullptr},
    /*0x21*/ {Type::LD, Addressing::R_D16, Register::HL, Register::NONE, Condition::NONE, instr_ld16},
    /*0x22*/ {Type::LD, Addressing::HLI_R, Register::HL, Register::A, Condition::NONE, nullptr},
    /*0x23*/ {Type::INC, Addressing::R, Register::HL, Register::NONE, Condition::NONE, nullptr},
    /*0x24*/ {Type::INC, Addressing::R, Register::H, Register::NONE, Condition::NONE, nullptr},
    /*0x25*/ {Type::DEC, Addressing::R, Register::H, Register::NONE, Condition::NONE, nullptr},
    /*0x26*/ {Type::LD, Addressing::R_D8, Register::H, Register::NONE, Condition::NONE, nullptr},
    /*0x27*/ {Type::DAA, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x28*/ {Type::JR, Addressing::R8, Register::NONE, Register::NONE, Condition::Z, nullptr},
    /*0x29*/ {Type::ADD, Addressing::R_R, Register::HL, Register::HL, Condition::NONE, nullptr},
    /*0x2a*/ {Type::LD, Addressing::R_HLI, Register::A, Register::HL, Condition::NONE, nullptr},
    /*0x2b*/ {Type::DEC, Addressing::R, Register::HL, Register::NONE, Condition::NONE, nullptr},
    /*0x2c*/ {Type::INC, Addressing::R, Register::L, Register::NONE, Condition::NONE, nullptr},
    /*0x2d*/ {Type::DEC, Addressing::R, Register::L, Register::NONE, Condition::NONE, nullptr},
    /*0x2e*/ {Type::LD, Addressing::R_D8, Register::L, Register::NONE, Condition::NONE, nullptr},
    /*0x2f*/ {Type::CPL, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x30*/ {Type::JR, Addressing::R8, Register::NONE, Register::NONE, Condition::NC, nullptr},
    /*0x31*/ {Type::LD, Addressing::R_D16, Register::SP, Register::NONE, Condition::NONE, nullptr},
    /*0x32*/ {Type::LD, Addressing::HLD_R, Register::HL, Register::A, Condition::NONE, nullptr},
    /*0x33*/ {Type::INC, Addressing::R, Register::SP, Register::NONE, Condition::NONE, nullptr},
    /*0x34*/ {Type::INC, Addressing::MR, Register::HL, Register::NONE, Condition::NONE, nullptr},
    /*0x35*/ {Type::DEC, Addressing::MR, Register::HL, Register::NONE, Condition::NONE, nullptr},
    /*0x36*/ {Type::LD, Addressing::MR_D8, Register::HL, Register::NONE, Condition::NONE, nullptr},
    /*0x37*/ {Type::SCF, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x38*/ {Type::JR, Addressing::R8, Register::NONE, Register::NONE, Condition::C, nullptr},
    /*0x39*/ {Type::ADD, Addressing::R_R, Register::HL, Register::SP, Condition::NONE, nullptr},
    /*0x3a*/ {Type::LD, Addressing::R_HLD, Register::A, Register::HL, Condition::NONE, nullptr},
    /*0x3b*/ {Type::DEC, Addressing::R, Register::SP, Register::NONE, Condition::NONE, nullptr},
    /*0x3c*/ {Type::INC, Addressing::R, Register::A, Register::NONE, Condition::NONE, nullptr},
    /*0x3d*/ {Type::DEC, Addressing::R, Register::A, Register::NONE, Condition::NONE, nullptr},
    /*0x3e*/ {Type::LD, Addressing::R_D8, Register::A, Register::NONE, Condition::NONE, nullptr},
    /*0x3f*/ {Type::CCF, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x40*/ {Type::LD, Addressing::R_R, Register::B, Register::B, Condition::NONE, nullptr},
    /*0x41*/ {Type::LD, Addressing::R_R, Register::B, Register::C, Condition::NONE, nullptr},
    /*0x42*/ {Type::LD, Addressing::R_R, Register::B, Register::D, Condition::NONE, nullptr},
    /*0x43*/ {Type::LD, Addressing::R_R, Register::B, Register::E, Condition::NONE, nullptr},
    /*0x44*/ {Type::LD, Addressing::R_R, Register::B, Register::H, Condition::NONE, nullptr},
    /*0x45*/ {Type::LD, Addressing::R_R, Register::B, Register::L, Condition::NONE, nullptr},
    /*0x46*/ {Type::LD, Addressing::R_MR, Register::B, Register::HL, Condition::NONE, nullptr},
    /*0x47*/ {Type::LD, Addressing::R_R, Register::B, Register::A, Condition::NONE, nullptr},
    /*0x48*/ {Type::LD, Addressing::R_R, Register::C, Register::B, Condition::NONE, nullptr},
    /*0x49*/ {Type::LD, Addressing::R_R, Register::C, Register::C, Condition::NONE, nullptr},
    /*0x4a*/ {Type::LD, Addressing::R_R, Register::C, Register::D, Condition::NONE, nullptr},
    /*0x4b*/ {Type::LD, Addressing::R_R, Register::C, Register::E, Condition::NONE, nullptr},
    /*0x4c*/ {Type::LD, Addressing::R_R, Register::C, Register::H, Condition::NONE, nullptr},
    /*0x4d*/ {Type::LD, Addressing::R_MR, Register::C, Register::L, Condition::NONE, nullptr},
    /*0x4e*/ {Type::LD, Addressing::R_R, Register::C, Register::HL, Condition::NONE, nullptr},
    /*0x4f*/ {Type::LD, Addressing::R_R, Register::C, Register::A, Condition::NONE, nullptr},
    /*0x50*/ {Type::LD, Addressing::R_R, Register::D, Register::B, Condition::NONE, nullptr},
    /*0x51*/ {Type::LD, Addressing::R_R, Register::D, Register::C, Condition::NONE, nullptr},
    /*0x52*/ {Type::LD, Addressing::R_R, Register::D, Register::D, Condition::NONE, nullptr},
    /*0x53*/ {Type::LD, Addressing::R_R, Register::D, Register::E, Condition::NONE, nullptr},
    /*0x54*/ {Type::LD, Addressing::R_R, Register::D, Register::H, Condition::NONE, nullptr},
    /*0x55*/ {Type::LD, Addressing::R_R, Register::D, Register::L, Condition::NONE, nullptr},
    /*0x56*/ {Type::LD, Addressing::R_MR, Register::D, Register::HL, Condition::NONE, nullptr},
    /*0x57*/ {Type::LD, Addressing::R_R, Register::D, Register::A, Condition::NONE, nullptr},
    /*0x58*/ {Type::LD, Addressing::R_R, Register::E, Register::B, Condition::NONE, nullptr},
    /*0x59*/ {Type::LD, Addressing::R_R, Register::E, Register::C, Condition::NONE, nullptr},
    /*0x5a*/ {Type::LD, Addressing::R_R, Register::E, Register::D, Condition::NONE, nullptr},
    /*0x5b*/ {Type::LD, Addressing::R_R, Register::E, Register::E, Condition::NONE, nullptr},
    /*0x5c*/ {Type::LD, Addressing::R_R, Register::E, Register::H, Condition::NONE, nullptr},
    /*0x5d*/ {Type::LD, Addressing::R_R, Register::E, Register::L, Condition::NONE, nullptr},
    /*0x5e*/ {Type::LD, Addressing::R_MR, Register::E, Register::HL, Condition::NONE, nullptr},
    /*0x5f*/ {Type::LD, Addressing::R_R, Register::E, Register::A, Condition::NONE, nullptr},
    /*0x60*/ {Type::LD, Addressing::R_R, Register::H, Register::B, Condition::NONE, nullptr},
    /*0x61*/ {Type::LD, Addressing::R_R, Register::H, Register::C, Condition::NONE, nullptr},
    /*0x62*/ {Type::LD, Addressing::R_R, Register::H, Register::D, Condition::NONE, nullptr},
    /*0x63*/ {Type::LD, Addressing::R_R, Register::H, Register::E, Condition::NONE, nullptr},
    /*0x64*/ {Type::LD, Addressing::R_R, Register::H, Register::H, Condition::NONE, nullptr},
    /*0x65*/ {Type::LD, Addressing::R_R, Register::H, Register::L, Condition::NONE, nullptr},
    /*0x66*/ {Type::LD, Addressing::R_MR, Register::H, Register::HL, Condition::NONE, nullptr},
    /*0x67*/ {Type::LD, Addressing::R_R, Register::H, Register::A, Condition::NONE, nullptr},
    /*0x68*/ {Type::LD, Addressing::R_R, Register::L, Register::B, Condition::NONE, nullptr},
    /*0x69*/ {Type::LD, Addressing::R_R, Register::L, Register::C, Condition::NONE, nullptr},
    /*0x6a*/ {Type::LD, Addressing::R_R, Register::L, Register::D, Condition::NONE, nullptr},
    /*0x6b*/ {Type::LD, Addressing::R_R, Register::L, Register::E, Condition::NONE, nullptr},
    /*0x6c*/ {Type::LD, Addressing::R_R, Register::L, Register::H, Condition::NONE, nullptr},
    /*0x6d*/ {Type::LD, Addressing::R_R, Register::L, Register::L, Condition::NONE, nullptr},
    /*0x6e*/ {Type::LD, Addressing::R_MR, Register::L, Register::HL, Condition::NONE, nullptr},
    /*0x6f*/ {Type::LD, Addressing::R_R, Register::L, Register::A, Condition::NONE, nullptr},
    /*0x70*/ {Type::LD, Addressing::MR_R, Register::HL, Register::B, Condition::NONE, nullptr},
    /*0x71*/ {Type::LD, Addressing::MR_R, Register::HL, Register::C, Condition::NONE, nullptr},
    /*0x72*/ {Type::LD, Addressing::MR_R, Register::HL, Register::D, Condition::NONE, nullptr},
    /*0x73*/ {Type::LD, Addressing::MR_R, Register::HL, Register::E, Condition::NONE, nullptr},
    /*0x74*/ {Type::LD, Addressing::MR_R, Register::HL, Register::H, Condition::NONE, nullptr},
    /*0x75*/ {Type::LD, Addressing::MR_R, Register::HL, Register::L, Condition::NONE, nullptr},
    /*0x76*/ {Type::HALT, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0x77*/ {Type::LD, Addressing::MR_R, Register::HL, Register::A, Condition::NONE, nullptr},
    /*0x78*/ {Type::LD, Addressing::R_R, Register::A, Register::B, Condition::NONE, nullptr},
    /*0x79*/ {Type::LD, Addressing::R_R, Register::A, Register::C, Condition::NONE, nullptr},
    /*0x7a*/ {Type::LD, Addressing::R_R, Register::A, Register::D, Condition::NONE, nullptr},
    /*0x7b*/ {Type::LD, Addressing::R_R, Register::A, Register::E, Condition::NONE, nullptr},
    /*0x7c*/ {Type::LD, Addressing::R_R, Register::A, Register::H, Condition::NONE, nullptr},
    /*0x7d*/ {Type::LD, Addressing::R_R, Register::A, Register::L, Condition::NONE, nullptr},
    /*0x7e*/ {Type::LD, Addressing::R_MR, Register::A, Register::HL, Condition::NONE, nullptr},
    /*0x7f*/ {Type::LD, Addressing::R_R, Register::A, Register::A, Condition::NONE, nullptr},
    /*0x80*/ {Type::ADD, Addressing::R_R, Register::A, Register::B, Condition::NONE, nullptr},
    /*0x81*/ {Type::ADD, Addressing::R_R, Register::A, Register::C, Condition::NONE, nullptr},
    /*0x82*/ {Type::ADD, Addressing::R_R, Register::A, Register::D, Condition::NONE, nullptr},
    /*0x83*/ {Type::ADD, Addressing::R_R, Register::A, Register::E, Condition::NONE, nullptr},
    /*0x84*/ {Type::ADD, Addressing::R_R, Register::A, Register::H, Condition::NONE, nullptr},
    /*0x85*/ {Type::ADD, Addressing::R_R, Register::A, Register::L, Condition::NONE, nullptr},
    /*0x86*/ {Type::ADD, Addressing::R_MR, Register::A, Register::HL, Condition::NONE, nullptr},
    /*0x87*/ {Type::ADD, Addressing::R_R, Register::A, Register::A, Condition::NONE, nullptr},
    /*0x88*/ {Type::ADC, Addressing::R_R, Register::A, Register::B, Condition::NONE, nullptr},
    /*0x89*/ {Type::ADC, Addressing::R_R, Register::A, Register::C, Condition::NONE, nullptr},
    /*0x8a*/ {Type::ADC, Addressing::R_R, Register::A, Register::D, Condition::NONE, nullptr},
    /*0x8b*/ {Type::ADC, Addressing::R_R, Register::A, Register::E, Condition::NONE, nullptr},
    /*0x8c*/ {Type::ADC, Addressing::R_R, Register::A, Register::H, Condition::NONE, nullptr},
    /*0x8d*/ {Type::ADC, Addressing::R_R, Register::A, Register::L, Condition::NONE, nullptr},
    /*0x8e*/ {Type::ADC, Addressing::R_MR, Register::A, Register::HL, Condition::NONE, nullptr},
    /*0x8f*/ {Type::ADC, Addressing::R_R, Register::A, Register::A, Condition::NONE, nullptr},
    /*0x90*/ {Type::SUB, Addressing::R, Register::B, Register::NONE, Condition::NONE, nullptr},
    /*0x91*/ {Type::SUB, Addressing::R, Register::C, Register::NONE, Condition::NONE, nullptr},
    /*0x92*/ {Type::SUB, Addressing::R, Register::D, Register::NONE, Condition::NONE, nullptr},
    /*0x93*/ {Type::SUB, Addressing::R, Register::E, Register::NONE, Condition::NONE, nullptr},
    /*0x94*/ {Type::SUB, Addressing::R, Register::H, Register::NONE, Condition::NONE, nullptr},
    /*0x95*/ {Type::SUB, Addressing::R, Register::L, Register::NONE, Condition::NONE, nullptr},
    /*0x96*/ {Type::SUB, Addressing::MR, Register::HL, Register::NONE, Condition::NONE, nullptr},
    /*0x97*/ {Type::SUB, Addressing::R, Register::A, Register::NONE, Condition::NONE, nullptr},
    /*0x98*/ {Type::SBC, Addressing::R_R, Register::A, Register::B, Condition::NONE, nullptr},
    /*0x99*/ {Type::SBC, Addressing::R_R, Register::A, Register::C, Condition::NONE, nullptr},
    /*0x9a*/ {Type::SBC, Addressing::R_R, Register::A, Register::D, Condition::NONE, nullptr},
    /*0x9b*/ {Type::SBC, Addressing::R_R, Register::A, Register::E, Condition::NONE, nullptr},
    /*0x9c*/ {Type::SBC, Addressing::R_R, Register::A, Register::H, Condition::NONE, nullptr},
    /*0x9d*/ {Type::SBC, Addressing::R_R, Register::A, Register::L, Condition::NONE, nullptr},
    /*0x9e*/ {Type::SBC, Addressing::R_MR, Register::A, Register::HL, Condition::NONE, nullptr},
    /*0x9f*/ {Type::SBC, Addressing::R_R, Register::A, Register::A, Condition::NONE, nullptr},
    /*0xa0*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa1*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa2*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa3*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa4*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa5*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa6*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa7*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa8*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xa9*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xaa*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xab*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xac*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xad*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xae*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xaf*/ {Type::XOR, Addressing::R, Register::A, Register::NONE, Condition::NONE, instr_xor},
    /*0xb0*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb1*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb2*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb3*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb4*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb5*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb6*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb7*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb8*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xb9*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xba*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xbb*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xbc*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xbd*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xbe*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xbf*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc0*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc1*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc2*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc3*/ {Type::JP, Addressing::D16, Register::NONE, Register::NONE, Condition::NONE, instr_jp},
    /*0xc4*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc5*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc6*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc7*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc8*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xc9*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xca*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xcb*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xcc*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xcd*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xce*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xcf*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd0*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd1*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd2*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd3*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd4*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd5*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd6*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd7*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd8*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xd9*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xda*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xdb*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xdc*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xdd*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xde*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xdf*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe0*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe1*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe2*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe3*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe4*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe5*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe6*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe7*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe8*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xe9*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xea*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xeb*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xec*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xed*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xee*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xef*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf0*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf1*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf2*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf3*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf4*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf5*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf6*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf7*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf8*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xf9*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xfa*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xfb*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xfc*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xfd*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xfe*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr},
    /*0xff*/ {Type::NONE, Addressing::NONE, Register::NONE, Register::NONE, Condition::NONE, nullptr}
};
// clang-format on

std::string to_string(const Instruction& instruction)
{
    char buf[64];
    size_t offset = 0;

    if (instruction.func == nullptr)
    {
        offset += sprintf(buf + offset, "!");
    }

    if (instruction.type == Instruction::Type::NONE)
    {
        return "!?";
    }

    offset += sprintf(buf + offset, "%s", Instruction::type_str[instruction.type]);

    switch (instruction.addressing)
    {
    case Instruction::Addressing::R:
        offset += sprintf(buf + offset, " %s", Instruction::register_str[instruction.reg1]);
        break;
    case Instruction::Addressing::R_A8:
    case Instruction::Addressing::R_A16:
        offset += sprintf(buf + offset, " %s, [0x%04x]", Instruction::register_str[instruction.reg1], instruction.data);
        break;
    case Instruction::Addressing::R_D16:
    case Instruction::Addressing::R_D8:
        offset += sprintf(buf + offset, " %s, %d", Instruction::register_str[instruction.reg1], instruction.data);
        break;
    case Instruction::Addressing::R8:
        offset += sprintf(buf + offset, " %+d", *(int8_t*)&instruction.data);
        break;
    case Instruction::Addressing::R_HLD:
        offset += sprintf(buf + offset, " %s, [HL-]", Instruction::register_str[instruction.reg1]);
        break;
    case Instruction::Addressing::R_HLI:
        offset += sprintf(buf + offset, " %s, [HL+]", Instruction::register_str[instruction.reg1]);
        break;
    case Instruction::Addressing::R_MR:
        offset += sprintf(buf + offset, " %s, [%s]", Instruction::register_str[instruction.reg1],
                          Instruction::register_str[instruction.reg2]);
        break;
    case Instruction::Addressing::R_R:
        offset += sprintf(buf + offset, " %s, %s", Instruction::register_str[instruction.reg1],
                          Instruction::register_str[instruction.reg2]);
        break;
    case Instruction::Addressing::HL_SPR8:
        offset += sprintf(buf + offset, " HL, SP%+d", *(int8_t*)&instruction.data);
        break;
    case Instruction::Addressing::MR:
        offset += sprintf(buf + offset, " [%s]", Instruction::register_str[instruction.reg1]);
        break;
    case Instruction::Addressing::MR_D8:
        offset += sprintf(buf + offset, " [%s], %d", Instruction::register_str[instruction.reg1], instruction.data);
        break;
    case Instruction::Addressing::MR_R:
        offset += sprintf(buf + offset, " [%s], %s", Instruction::register_str[instruction.reg1],
                          Instruction::register_str[instruction.reg2]);
        break;
    case Instruction::Addressing::D8:
        offset += sprintf(buf + offset, " %d", instruction.data);
        break;
    case Instruction::Addressing::D16: {
        if (instruction.type == Instruction::Type::JP)
        {
            offset += sprintf(buf + offset, " 0x%04x", instruction.data);
        }
        else
        {
            offset += sprintf(buf + offset, " %d", instruction.data);
        }
        break;
    }
    case Instruction::Addressing::A8_R:
    case Instruction::Addressing::A16_R:
        offset += sprintf(buf + offset, " [0x%04x], %s", instruction.memory_dst, Instruction::register_str[instruction.reg1]);
        break;
    case Instruction::Addressing::HLI_R:
        offset += sprintf(buf + offset, " [HL+], %s", Instruction::register_str[instruction.reg2]);
        break;
    case Instruction::Addressing::HLD_R:
        offset += sprintf(buf + offset, " [HL-], %s", Instruction::register_str[instruction.reg2]);
        break;
    default:
        break;
    }

    return buf;
}
