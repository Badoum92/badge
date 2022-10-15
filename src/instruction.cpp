#include "instruction.h"

#include <cstdio>

#include "gameboy.h"

/* --- String helpers --- */

static std::string instr_r_str(Gameboy&, const Instr& instr, const char* mnemonic)
{
    char buf[32] = {};
    sprintf(buf, "%s %s", mnemonic, reg_str[instr.r1]);
    return buf;
}

static std::string instr_mr_str(Gameboy&, const Instr& instr, const char* mnemonic)
{
    char buf[32] = {};
    sprintf(buf, "%s (%s)", mnemonic, reg_str[instr.r1]);
    return buf;
}

static std::string instr_r_r_str(Gameboy&, const Instr& instr, const char* mnemonic)
{
    char buf[32] = {};
    sprintf(buf, "%s %s, %s", mnemonic, reg_str[instr.r1], reg_str[instr.r2]);
    return buf;
}

static std::string instr_mr_r_str(Gameboy&, const Instr& instr, const char* mnemonic)
{
    char buf[32] = {};
    sprintf(buf, "%s (%s), %s", mnemonic, reg_str[instr.r1], reg_str[instr.r2]);
    return buf;
}

static std::string instr_r_mr_str(Gameboy&, const Instr& instr, const char* mnemonic)
{
    char buf[32] = {};
    sprintf(buf, "%s %s, (%s)", mnemonic, reg_str[instr.r1], reg_str[instr.r2]);
    return buf;
}

static std::string instr_r_d8_str(Gameboy& gb, const Instr& instr, const char* mnemonic)
{
    char buf[32] = {};
    uint8_t data = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    sprintf(buf, "%s %s, 0x%02x", mnemonic, reg_str[instr.r1], data);
    return buf;
}

static std::string instr_d8_str(Gameboy& gb, const Instr&, const char* mnemonic)
{
    char buf[32] = {};
    uint8_t data = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    sprintf(buf, "%s 0x%02x", mnemonic, data);
    return buf;
}

/* 0xCB Instructions */

static uint8_t _rlc(Gameboy& gb, uint8_t data)
{
    gb.cpu.flag_z(data == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x80);
    return (data << 1) | ((data & 0x80) != 0);
}

static uint32_t instr_rlc_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.get_reg8(instr.r1);
    gb.cpu.set_reg(instr.r1, _rlc(gb, data));
    return 2;
}

static std::string instr_rlc_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "RLC");
}

static uint32_t instr_rlc_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));
    gb.memory.write(gb.cpu.read_reg(instr.r1), _rlc(gb, data));
    return 4;
}

static std::string instr_rlc_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "RLC");
}

static uint8_t _rrc(Gameboy& gb, uint8_t data)
{
    gb.cpu.flag_z(data == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x01);
    return (data >> 1) | ((data & 0x01) << 7);
}

static uint32_t instr_rrc_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.get_reg8(instr.r1);
    gb.cpu.set_reg(instr.r1, _rrc(gb, data));
    return 2;
}

static std::string instr_rrc_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "RRC");
}

static uint32_t instr_rrc_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));
    gb.memory.write(gb.cpu.read_reg(instr.r1), _rrc(gb, data));
    return 4;
}

static std::string instr_rrc_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "RRC");
}

static uint8_t _rl(Gameboy& gb, uint8_t data)
{
    uint8_t c = gb.cpu.flag_c();
    gb.cpu.flag_c(data & 0x80);
    data = (data << 1) | (c != 0);
    gb.cpu.flag_z(data == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    return data;
}

static uint32_t instr_rl_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.get_reg8(instr.r1);
    gb.cpu.set_reg(instr.r1, _rl(gb, data));
    return 2;
}

static std::string instr_rl_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "RL");
}

static uint32_t instr_rl_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));
    gb.memory.write(gb.cpu.read_reg(instr.r1), _rl(gb, data));
    return 4;
}

static std::string instr_rl_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "RL");
}

static uint8_t _rr(Gameboy& gb, uint8_t data)
{
    uint8_t c = gb.cpu.flag_c();
    gb.cpu.flag_c(data & 0x01);
    data = (data >> 1) | (c << 7);
    gb.cpu.flag_z(data == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    return data;
}

static uint32_t instr_rr_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.get_reg8(instr.r1);
    gb.cpu.set_reg(instr.r1, _rr(gb, data));
    return 2;
}

static std::string instr_rr_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "RR");
}

static uint32_t instr_rr_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));
    gb.memory.write(gb.cpu.read_reg(instr.r1), _rr(gb, data));
    return 4;
}

static std::string instr_rr_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "RR");
}

static uint8_t _sla(Gameboy& gb, uint8_t data)
{
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x80);
    data = data << 1;
    gb.cpu.flag_z(data == 0);
    return data;
}

static uint32_t instr_sla_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.read_reg(instr.r1);
    gb.cpu.set_reg(instr.r1, _sla(gb, data));
    return 2;
}

static std::string instr_sla_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "SLA");
}

static uint32_t instr_sla_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));
    gb.memory.write(gb.cpu.read_reg(instr.r1), _sla(gb, data));
    return 4;
}

static std::string instr_sla_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "SLA");
}

static uint8_t _sra(Gameboy& gb, uint8_t data)
{
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x01);
    data = (data >> 1) | (data & 0x80);
    gb.cpu.flag_z(data == 0);
    return data;
}

static uint32_t instr_sra_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.read_reg(instr.r1);
    gb.cpu.set_reg(instr.r1, _sra(gb, data));
    return 2;
}

static std::string instr_sra_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "SRA");
}

static uint32_t instr_sra_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));
    gb.memory.write(gb.cpu.read_reg(instr.r1), _sra(gb, data));
    return 4;
}

static std::string instr_sra_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "SRA");
}

static uint8_t _srl(Gameboy& gb, uint8_t data)
{
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x01);
    data = data >> 1;
    gb.cpu.flag_z(data == 0);
    return data;
}

static uint32_t instr_srl_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.read_reg(instr.r1);
    gb.cpu.set_reg(instr.r1, _srl(gb, data));
    return 2;
}

static std::string instr_srl_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "SRL");
}

static uint32_t instr_srl_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));
    gb.memory.write(gb.cpu.read_reg(instr.r1), _srl(gb, data));
    return 4;
}

static std::string instr_srl_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "SRL");
}

static uint8_t _swap(Gameboy& gb, uint8_t data)
{
    gb.cpu.flag_z(data == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(0);
    data = ((data & 0xf0) >> 4) | ((data & 0x0f) << 4);
    return data;
}

static uint32_t instr_swap_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.read_reg(instr.r1);
    gb.cpu.set_reg(instr.r1, _swap(gb, data));
    return 2;
}

static std::string instr_swap_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "SWAP");
}

static uint32_t instr_swap_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));
    gb.memory.write(gb.cpu.read_reg(instr.r1), _swap(gb, data));
    return 4;
}

static std::string instr_swap_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "SWAP");
}

static void _bit(Gameboy& gb, uint8_t data, size_t n)
{
    gb.cpu.flag_z(bit(data, n) == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(1);
}

#define INSTR_BIT_N(N)                                                                                                 \
    static uint32_t instr_bit_##N##_r8(Gameboy& gb, const Instr& instr)                                                \
    {                                                                                                                  \
        uint8_t data = gb.cpu.read_reg(instr.r1);                                                                      \
        _bit(gb, data, N);                                                                                             \
        return 2;                                                                                                      \
    }                                                                                                                  \
    static std::string instr_bit_##N##_r8_str(Gameboy& gb, const Instr& instr)                                         \
    {                                                                                                                  \
        return instr_r_str(gb, instr, "BIT " #N);                                                                      \
    }                                                                                                                  \
    static uint32_t instr_bit_##N##_mr(Gameboy& gb, const Instr& instr)                                                \
    {                                                                                                                  \
        uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));                                                      \
        _bit(gb, data, N);                                                                                             \
        return 3;                                                                                                      \
    }                                                                                                                  \
    static std::string instr_bit_##N##_mr_str(Gameboy& gb, const Instr& instr)                                         \
    {                                                                                                                  \
        return instr_mr_str(gb, instr, "BIT " #N);                                                                     \
    }

INSTR_BIT_N(0)
INSTR_BIT_N(1)
INSTR_BIT_N(2)
INSTR_BIT_N(3)
INSTR_BIT_N(4)
INSTR_BIT_N(5)
INSTR_BIT_N(6)
INSTR_BIT_N(7)

#define INSTR_RES_N(N)                                                                                                 \
    static uint32_t instr_res_##N##_r8(Gameboy& gb, const Instr& instr)                                                \
    {                                                                                                                  \
        uint8_t data = gb.cpu.read_reg(instr.r1);                                                                      \
        set_bit(data, N, 0);                                                                                           \
        gb.cpu.set_reg(instr.r1, data);                                                                                \
        return 2;                                                                                                      \
    }                                                                                                                  \
    static std::string instr_res_##N##_r8_str(Gameboy& gb, const Instr& instr)                                         \
    {                                                                                                                  \
        return instr_r_str(gb, instr, "RES " #N);                                                                      \
    }                                                                                                                  \
    static uint32_t instr_res_##N##_mr(Gameboy& gb, const Instr& instr)                                                \
    {                                                                                                                  \
        uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));                                                      \
        set_bit(data, N, 0);                                                                                           \
        gb.memory.write(gb.cpu.read_reg(instr.r1), data);                                                              \
        return 3;                                                                                                      \
    }                                                                                                                  \
    static std::string instr_res_##N##_mr_str(Gameboy& gb, const Instr& instr)                                         \
    {                                                                                                                  \
        return instr_mr_str(gb, instr, "RES " #N);                                                                     \
    }

INSTR_RES_N(0)
INSTR_RES_N(1)
INSTR_RES_N(2)
INSTR_RES_N(3)
INSTR_RES_N(4)
INSTR_RES_N(5)
INSTR_RES_N(6)
INSTR_RES_N(7)

#define INSTR_SET_N(N)                                                                                                 \
    static uint32_t instr_set_##N##_r8(Gameboy& gb, const Instr& instr)                                                \
    {                                                                                                                  \
        uint8_t data = gb.cpu.read_reg(instr.r1);                                                                      \
        set_bit(data, N, 1);                                                                                           \
        gb.cpu.set_reg(instr.r1, data);                                                                                \
        return 2;                                                                                                      \
    }                                                                                                                  \
    static std::string instr_set_##N##_r8_str(Gameboy& gb, const Instr& instr)                                         \
    {                                                                                                                  \
        return instr_r_str(gb, instr, "SET " #N);                                                                      \
    }                                                                                                                  \
    static uint32_t instr_set_##N##_mr(Gameboy& gb, const Instr& instr)                                                \
    {                                                                                                                  \
        uint8_t data = gb.memory.read(gb.cpu.read_reg(instr.r1));                                                      \
        set_bit(data, N, 1);                                                                                           \
        gb.memory.write(gb.cpu.read_reg(instr.r1), data);                                                              \
        return 3;                                                                                                      \
    }                                                                                                                  \
    static std::string instr_set_##N##_mr_str(Gameboy& gb, const Instr& instr)                                         \
    {                                                                                                                  \
        return instr_mr_str(gb, instr, "SET " #N);                                                                     \
    }

INSTR_SET_N(0)
INSTR_SET_N(1)
INSTR_SET_N(2)
INSTR_SET_N(3)
INSTR_SET_N(4)
INSTR_SET_N(5)
INSTR_SET_N(6)
INSTR_SET_N(7)

// clang-format off
Instr cb_instructions[0x100] = {
    {0x00, instr_rlc_r8, instr_rlc_r8_str, Reg::B},
    {0x01, instr_rlc_r8, instr_rlc_r8_str, Reg::C},
    {0x02, instr_rlc_r8, instr_rlc_r8_str, Reg::D},
    {0x03, instr_rlc_r8, instr_rlc_r8_str, Reg::E},
    {0x04, instr_rlc_r8, instr_rlc_r8_str, Reg::H},
    {0x05, instr_rlc_r8, instr_rlc_r8_str, Reg::L},
    {0x06, instr_rlc_mr, instr_rlc_mr_str, Reg::HL},
    {0x07, instr_rlc_r8, instr_rlc_r8_str, Reg::A},
    {0x08, instr_rrc_r8, instr_rrc_r8_str, Reg::B},
    {0x09, instr_rrc_r8, instr_rrc_r8_str, Reg::C},
    {0x0a, instr_rrc_r8, instr_rrc_r8_str, Reg::D},
    {0x0b, instr_rrc_r8, instr_rrc_r8_str, Reg::E},
    {0x0c, instr_rrc_r8, instr_rrc_r8_str, Reg::H},
    {0x0d, instr_rrc_r8, instr_rrc_r8_str, Reg::L},
    {0x0e, instr_rrc_mr, instr_rrc_mr_str, Reg::HL},
    {0x0f, instr_rrc_r8, instr_rrc_r8_str, Reg::A},
    {0x10, instr_rl_r8, instr_rl_r8_str, Reg::B},
    {0x11, instr_rl_r8, instr_rl_r8_str, Reg::C},
    {0x12, instr_rl_r8, instr_rl_r8_str, Reg::D},
    {0x13, instr_rl_r8, instr_rl_r8_str, Reg::E},
    {0x14, instr_rl_r8, instr_rl_r8_str, Reg::H},
    {0x15, instr_rl_r8, instr_rl_r8_str, Reg::L},
    {0x16, instr_rl_mr, instr_rl_mr_str, Reg::HL},
    {0x17, instr_rl_r8, instr_rl_r8_str, Reg::A},
    {0x18, instr_rr_r8, instr_rr_r8_str, Reg::B},
    {0x19, instr_rr_r8, instr_rr_r8_str, Reg::C},
    {0x1a, instr_rr_r8, instr_rr_r8_str, Reg::D},
    {0x1b, instr_rr_r8, instr_rr_r8_str, Reg::E},
    {0x1c, instr_rr_r8, instr_rr_r8_str, Reg::H},
    {0x1d, instr_rr_r8, instr_rr_r8_str, Reg::L},
    {0x1e, instr_rr_mr, instr_rr_mr_str, Reg::HL},
    {0x1f, instr_rr_r8, instr_rr_r8_str, Reg::A},
    {0x20, instr_sla_r8, instr_sla_r8_str, Reg::B},
    {0x21, instr_sla_r8, instr_sla_r8_str, Reg::C},
    {0x22, instr_sla_r8, instr_sla_r8_str, Reg::D},
    {0x23, instr_sla_r8, instr_sla_r8_str, Reg::E},
    {0x24, instr_sla_r8, instr_sla_r8_str, Reg::H},
    {0x25, instr_sla_r8, instr_sla_r8_str, Reg::L},
    {0x26, instr_sla_mr, instr_sla_mr_str, Reg::HL},
    {0x27, instr_sla_r8, instr_sla_r8_str, Reg::A},
    {0x28, instr_sra_r8, instr_sra_r8_str, Reg::B},
    {0x29, instr_sra_r8, instr_sra_r8_str, Reg::C},
    {0x2a, instr_sra_r8, instr_sra_r8_str, Reg::D},
    {0x2b, instr_sra_r8, instr_sra_r8_str, Reg::E},
    {0x2c, instr_sra_r8, instr_sra_r8_str, Reg::H},
    {0x2d, instr_sra_r8, instr_sra_r8_str, Reg::L},
    {0x2e, instr_sra_mr, instr_sra_mr_str, Reg::HL},
    {0x2f, instr_sra_r8, instr_sra_r8_str, Reg::A},
    {0x30, instr_swap_r8, instr_swap_r8_str, Reg::B},
    {0x31, instr_swap_r8, instr_swap_r8_str, Reg::C},
    {0x32, instr_swap_r8, instr_swap_r8_str, Reg::D},
    {0x33, instr_swap_r8, instr_swap_r8_str, Reg::E},
    {0x34, instr_swap_r8, instr_swap_r8_str, Reg::H},
    {0x35, instr_swap_r8, instr_swap_r8_str, Reg::L},
    {0x36, instr_swap_mr, instr_swap_mr_str, Reg::HL},
    {0x37, instr_swap_r8, instr_swap_r8_str, Reg::A},
    {0x38, instr_srl_r8, instr_srl_r8_str, Reg::B},
    {0x39, instr_srl_r8, instr_srl_r8_str, Reg::C},
    {0x3a, instr_srl_r8, instr_srl_r8_str, Reg::D},
    {0x3b, instr_srl_r8, instr_srl_r8_str, Reg::E},
    {0x3c, instr_srl_r8, instr_srl_r8_str, Reg::H},
    {0x3d, instr_srl_r8, instr_srl_r8_str, Reg::L},
    {0x3e, instr_srl_mr, instr_srl_mr_str, Reg::HL},
    {0x3f, instr_srl_r8, instr_srl_r8_str, Reg::A},
    {0x40, instr_bit_0_r8, instr_bit_0_r8_str, Reg::B},
    {0x41, instr_bit_0_r8, instr_bit_0_r8_str, Reg::C},
    {0x42, instr_bit_0_r8, instr_bit_0_r8_str, Reg::D},
    {0x43, instr_bit_0_r8, instr_bit_0_r8_str, Reg::E},
    {0x44, instr_bit_0_r8, instr_bit_0_r8_str, Reg::H},
    {0x45, instr_bit_0_r8, instr_bit_0_r8_str, Reg::L},
    {0x46, instr_bit_0_mr, instr_bit_0_mr_str, Reg::HL},
    {0x47, instr_bit_0_r8, instr_bit_0_r8_str, Reg::A},
    {0x48, instr_bit_1_r8, instr_bit_1_r8_str, Reg::B},
    {0x49, instr_bit_1_r8, instr_bit_1_r8_str, Reg::C},
    {0x4a, instr_bit_1_r8, instr_bit_1_r8_str, Reg::D},
    {0x4b, instr_bit_1_r8, instr_bit_1_r8_str, Reg::E},
    {0x4c, instr_bit_1_r8, instr_bit_1_r8_str, Reg::H},
    {0x4d, instr_bit_1_r8, instr_bit_1_r8_str, Reg::L},
    {0x4e, instr_bit_1_mr, instr_bit_1_mr_str, Reg::HL},
    {0x4f, instr_bit_1_r8, instr_bit_1_r8_str, Reg::A},
    {0x50, instr_bit_2_r8, instr_bit_2_r8_str, Reg::B},
    {0x51, instr_bit_2_r8, instr_bit_2_r8_str, Reg::C},
    {0x52, instr_bit_2_r8, instr_bit_2_r8_str, Reg::D},
    {0x53, instr_bit_2_r8, instr_bit_2_r8_str, Reg::E},
    {0x54, instr_bit_2_r8, instr_bit_2_r8_str, Reg::H},
    {0x55, instr_bit_2_r8, instr_bit_2_r8_str, Reg::L},
    {0x56, instr_bit_2_mr, instr_bit_2_mr_str, Reg::HL},
    {0x57, instr_bit_2_r8, instr_bit_2_r8_str, Reg::A},
    {0x58, instr_bit_3_r8, instr_bit_3_r8_str, Reg::B},
    {0x59, instr_bit_3_r8, instr_bit_3_r8_str, Reg::C},
    {0x5a, instr_bit_3_r8, instr_bit_3_r8_str, Reg::D},
    {0x5b, instr_bit_3_r8, instr_bit_3_r8_str, Reg::E},
    {0x5c, instr_bit_3_r8, instr_bit_3_r8_str, Reg::H},
    {0x5d, instr_bit_3_r8, instr_bit_3_r8_str, Reg::L},
    {0x5e, instr_bit_3_mr, instr_bit_3_mr_str, Reg::HL},
    {0x5f, instr_bit_3_r8, instr_bit_3_r8_str, Reg::A},
    {0x60, instr_bit_4_r8, instr_bit_4_r8_str, Reg::B},
    {0x61, instr_bit_4_r8, instr_bit_4_r8_str, Reg::C},
    {0x62, instr_bit_4_r8, instr_bit_4_r8_str, Reg::D},
    {0x63, instr_bit_4_r8, instr_bit_4_r8_str, Reg::E},
    {0x64, instr_bit_4_r8, instr_bit_4_r8_str, Reg::H},
    {0x65, instr_bit_4_r8, instr_bit_4_r8_str, Reg::L},
    {0x66, instr_bit_4_mr, instr_bit_4_mr_str, Reg::HL},
    {0x67, instr_bit_4_r8, instr_bit_4_r8_str, Reg::A},
    {0x68, instr_bit_5_r8, instr_bit_5_r8_str, Reg::B},
    {0x69, instr_bit_5_r8, instr_bit_5_r8_str, Reg::C},
    {0x6a, instr_bit_5_r8, instr_bit_5_r8_str, Reg::D},
    {0x6b, instr_bit_5_r8, instr_bit_5_r8_str, Reg::E},
    {0x6c, instr_bit_5_r8, instr_bit_5_r8_str, Reg::H},
    {0x6d, instr_bit_5_r8, instr_bit_5_r8_str, Reg::L},
    {0x6e, instr_bit_5_mr, instr_bit_5_mr_str, Reg::HL},
    {0x6f, instr_bit_5_r8, instr_bit_5_r8_str, Reg::A},
    {0x70, instr_bit_6_r8, instr_bit_6_r8_str, Reg::B},
    {0x71, instr_bit_6_r8, instr_bit_6_r8_str, Reg::C},
    {0x72, instr_bit_6_r8, instr_bit_6_r8_str, Reg::D},
    {0x73, instr_bit_6_r8, instr_bit_6_r8_str, Reg::E},
    {0x74, instr_bit_6_r8, instr_bit_6_r8_str, Reg::H},
    {0x75, instr_bit_6_r8, instr_bit_6_r8_str, Reg::L},
    {0x76, instr_bit_6_mr, instr_bit_6_mr_str, Reg::HL},
    {0x77, instr_bit_6_r8, instr_bit_6_r8_str, Reg::A},
    {0x78, instr_bit_7_r8, instr_bit_7_r8_str, Reg::B},
    {0x79, instr_bit_7_r8, instr_bit_7_r8_str, Reg::C},
    {0x7a, instr_bit_7_r8, instr_bit_7_r8_str, Reg::D},
    {0x7b, instr_bit_7_r8, instr_bit_7_r8_str, Reg::E},
    {0x7c, instr_bit_7_r8, instr_bit_7_r8_str, Reg::H},
    {0x7d, instr_bit_7_r8, instr_bit_7_r8_str, Reg::L},
    {0x7e, instr_bit_7_mr, instr_bit_7_mr_str, Reg::HL},
    {0x7f, instr_bit_7_r8, instr_bit_7_r8_str, Reg::A},
    {0x80, instr_res_0_r8, instr_res_0_r8_str, Reg::B},
    {0x81, instr_res_0_r8, instr_res_0_r8_str, Reg::C},
    {0x82, instr_res_0_r8, instr_res_0_r8_str, Reg::D},
    {0x83, instr_res_0_r8, instr_res_0_r8_str, Reg::E},
    {0x84, instr_res_0_r8, instr_res_0_r8_str, Reg::H},
    {0x85, instr_res_0_r8, instr_res_0_r8_str, Reg::L},
    {0x86, instr_res_0_mr, instr_res_0_mr_str, Reg::HL},
    {0x87, instr_res_0_r8, instr_res_0_r8_str, Reg::A},
    {0x88, instr_res_1_r8, instr_res_1_r8_str, Reg::B},
    {0x89, instr_res_1_r8, instr_res_1_r8_str, Reg::C},
    {0x8a, instr_res_1_r8, instr_res_1_r8_str, Reg::D},
    {0x8b, instr_res_1_r8, instr_res_1_r8_str, Reg::E},
    {0x8c, instr_res_1_r8, instr_res_1_r8_str, Reg::H},
    {0x8d, instr_res_1_r8, instr_res_1_r8_str, Reg::L},
    {0x8e, instr_res_1_mr, instr_res_1_mr_str, Reg::HL},
    {0x8f, instr_res_1_r8, instr_res_1_r8_str, Reg::A},
    {0x90, instr_res_2_r8, instr_res_2_r8_str, Reg::B},
    {0x91, instr_res_2_r8, instr_res_2_r8_str, Reg::C},
    {0x92, instr_res_2_r8, instr_res_2_r8_str, Reg::D},
    {0x93, instr_res_2_r8, instr_res_2_r8_str, Reg::E},
    {0x94, instr_res_2_r8, instr_res_2_r8_str, Reg::H},
    {0x95, instr_res_2_r8, instr_res_2_r8_str, Reg::L},
    {0x96, instr_res_2_mr, instr_res_2_mr_str, Reg::HL},
    {0x97, instr_res_2_r8, instr_res_2_r8_str, Reg::A},
    {0x98, instr_res_3_r8, instr_res_3_r8_str, Reg::B},
    {0x99, instr_res_3_r8, instr_res_3_r8_str, Reg::C},
    {0x9a, instr_res_3_r8, instr_res_3_r8_str, Reg::D},
    {0x9b, instr_res_3_r8, instr_res_3_r8_str, Reg::E},
    {0x9c, instr_res_3_r8, instr_res_3_r8_str, Reg::H},
    {0x9d, instr_res_3_r8, instr_res_3_r8_str, Reg::L},
    {0x9e, instr_res_3_mr, instr_res_3_mr_str, Reg::HL},
    {0x9f, instr_res_3_r8, instr_res_3_r8_str, Reg::A},
    {0xa0, instr_res_4_r8, instr_res_4_r8_str, Reg::B},
    {0xa1, instr_res_4_r8, instr_res_4_r8_str, Reg::C},
    {0xa2, instr_res_4_r8, instr_res_4_r8_str, Reg::D},
    {0xa3, instr_res_4_r8, instr_res_4_r8_str, Reg::E},
    {0xa4, instr_res_4_r8, instr_res_4_r8_str, Reg::H},
    {0xa5, instr_res_4_r8, instr_res_4_r8_str, Reg::L},
    {0xa6, instr_res_4_mr, instr_res_4_mr_str, Reg::HL},
    {0xa7, instr_res_4_r8, instr_res_4_r8_str, Reg::A},
    {0xa8, instr_res_5_r8, instr_res_5_r8_str, Reg::B},
    {0xa9, instr_res_5_r8, instr_res_5_r8_str, Reg::C},
    {0xaa, instr_res_5_r8, instr_res_5_r8_str, Reg::D},
    {0xab, instr_res_5_r8, instr_res_5_r8_str, Reg::E},
    {0xac, instr_res_5_r8, instr_res_5_r8_str, Reg::H},
    {0xad, instr_res_5_r8, instr_res_5_r8_str, Reg::L},
    {0xae, instr_res_5_mr, instr_res_5_mr_str, Reg::HL},
    {0xaf, instr_res_5_r8, instr_res_5_r8_str, Reg::A},
    {0xb0, instr_res_6_r8, instr_res_6_r8_str, Reg::B},
    {0xb1, instr_res_6_r8, instr_res_6_r8_str, Reg::C},
    {0xb2, instr_res_6_r8, instr_res_6_r8_str, Reg::D},
    {0xb3, instr_res_6_r8, instr_res_6_r8_str, Reg::E},
    {0xb4, instr_res_6_r8, instr_res_6_r8_str, Reg::H},
    {0xb5, instr_res_6_r8, instr_res_6_r8_str, Reg::L},
    {0xb6, instr_res_6_mr, instr_res_6_mr_str, Reg::HL},
    {0xb7, instr_res_6_r8, instr_res_6_r8_str, Reg::A},
    {0xb8, instr_res_7_r8, instr_res_7_r8_str, Reg::B},
    {0xb9, instr_res_7_r8, instr_res_7_r8_str, Reg::C},
    {0xba, instr_res_7_r8, instr_res_7_r8_str, Reg::D},
    {0xbb, instr_res_7_r8, instr_res_7_r8_str, Reg::E},
    {0xbc, instr_res_7_r8, instr_res_7_r8_str, Reg::H},
    {0xbd, instr_res_7_r8, instr_res_7_r8_str, Reg::L},
    {0xbe, instr_res_7_mr, instr_res_7_mr_str, Reg::HL},
    {0xbf, instr_res_7_r8, instr_res_7_r8_str, Reg::A},
    {0xc0, instr_set_0_r8, instr_set_0_r8_str, Reg::B},
    {0xc1, instr_set_0_r8, instr_set_0_r8_str, Reg::C},
    {0xc2, instr_set_0_r8, instr_set_0_r8_str, Reg::D},
    {0xc3, instr_set_0_r8, instr_set_0_r8_str, Reg::E},
    {0xc4, instr_set_0_r8, instr_set_0_r8_str, Reg::H},
    {0xc5, instr_set_0_r8, instr_set_0_r8_str, Reg::L},
    {0xc6, instr_set_0_mr, instr_set_0_mr_str, Reg::HL},
    {0xc7, instr_set_0_r8, instr_set_0_r8_str, Reg::A},
    {0xc8, instr_set_1_r8, instr_set_1_r8_str, Reg::B},
    {0xc9, instr_set_1_r8, instr_set_1_r8_str, Reg::C},
    {0xca, instr_set_1_r8, instr_set_1_r8_str, Reg::D},
    {0xcb, instr_set_1_r8, instr_set_1_r8_str, Reg::E},
    {0xcc, instr_set_1_r8, instr_set_1_r8_str, Reg::H},
    {0xcd, instr_set_1_r8, instr_set_1_r8_str, Reg::L},
    {0xce, instr_set_1_mr, instr_set_1_mr_str, Reg::HL},
    {0xcf, instr_set_1_r8, instr_set_1_r8_str, Reg::A},
    {0xd0, instr_set_2_r8, instr_set_2_r8_str, Reg::B},
    {0xd1, instr_set_2_r8, instr_set_2_r8_str, Reg::C},
    {0xd2, instr_set_2_r8, instr_set_2_r8_str, Reg::D},
    {0xd3, instr_set_2_r8, instr_set_2_r8_str, Reg::E},
    {0xd4, instr_set_2_r8, instr_set_2_r8_str, Reg::H},
    {0xd5, instr_set_2_r8, instr_set_2_r8_str, Reg::L},
    {0xd6, instr_set_2_mr, instr_set_2_mr_str, Reg::HL},
    {0xd7, instr_set_2_r8, instr_set_2_r8_str, Reg::A},
    {0xd8, instr_set_3_r8, instr_set_3_r8_str, Reg::B},
    {0xd9, instr_set_3_r8, instr_set_3_r8_str, Reg::C},
    {0xda, instr_set_3_r8, instr_set_3_r8_str, Reg::D},
    {0xdb, instr_set_3_r8, instr_set_3_r8_str, Reg::E},
    {0xdc, instr_set_3_r8, instr_set_3_r8_str, Reg::H},
    {0xdd, instr_set_3_r8, instr_set_3_r8_str, Reg::L},
    {0xde, instr_set_3_mr, instr_set_3_mr_str, Reg::HL},
    {0xdf, instr_set_3_r8, instr_set_3_r8_str, Reg::A},
    {0xe0, instr_set_4_r8, instr_set_4_r8_str, Reg::B},
    {0xe1, instr_set_4_r8, instr_set_4_r8_str, Reg::C},
    {0xe2, instr_set_4_r8, instr_set_4_r8_str, Reg::D},
    {0xe3, instr_set_4_r8, instr_set_4_r8_str, Reg::E},
    {0xe4, instr_set_4_r8, instr_set_4_r8_str, Reg::H},
    {0xe5, instr_set_4_r8, instr_set_4_r8_str, Reg::L},
    {0xe6, instr_set_4_mr, instr_set_4_mr_str, Reg::HL},
    {0xe7, instr_set_4_r8, instr_set_4_r8_str, Reg::A},
    {0xe8, instr_set_5_r8, instr_set_5_r8_str, Reg::B},
    {0xe9, instr_set_5_r8, instr_set_5_r8_str, Reg::C},
    {0xea, instr_set_5_r8, instr_set_5_r8_str, Reg::D},
    {0xeb, instr_set_5_r8, instr_set_5_r8_str, Reg::E},
    {0xec, instr_set_5_r8, instr_set_5_r8_str, Reg::H},
    {0xed, instr_set_5_r8, instr_set_5_r8_str, Reg::L},
    {0xee, instr_set_5_mr, instr_set_5_mr_str, Reg::HL},
    {0xef, instr_set_5_r8, instr_set_5_r8_str, Reg::A},
    {0xf0, instr_set_6_r8, instr_set_6_r8_str, Reg::B},
    {0xf1, instr_set_6_r8, instr_set_6_r8_str, Reg::C},
    {0xf2, instr_set_6_r8, instr_set_6_r8_str, Reg::D},
    {0xf3, instr_set_6_r8, instr_set_6_r8_str, Reg::E},
    {0xf4, instr_set_6_r8, instr_set_6_r8_str, Reg::H},
    {0xf5, instr_set_6_r8, instr_set_6_r8_str, Reg::L},
    {0xf6, instr_set_6_mr, instr_set_6_mr_str, Reg::HL},
    {0xf7, instr_set_6_r8, instr_set_6_r8_str, Reg::A},
    {0xf8, instr_set_7_r8, instr_set_7_r8_str, Reg::B},
    {0xf9, instr_set_7_r8, instr_set_7_r8_str, Reg::C},
    {0xfa, instr_set_7_r8, instr_set_7_r8_str, Reg::D},
    {0xfb, instr_set_7_r8, instr_set_7_r8_str, Reg::E},
    {0xfc, instr_set_7_r8, instr_set_7_r8_str, Reg::H},
    {0xfd, instr_set_7_r8, instr_set_7_r8_str, Reg::L},
    {0xfe, instr_set_7_mr, instr_set_7_mr_str, Reg::HL},
    {0xff, instr_set_7_r8, instr_set_7_r8_str, Reg::A}
};
// clang-format on

/* Instructions */

static uint32_t instr_nop(Gameboy&, const Instr&)
{
    return 1;
}

static std::string instr_nop_str(Gameboy&, const Instr&)
{
    return "NOP";
}

static uint32_t instr_ld_r16_d16(Gameboy& gb, const Instr& instr)
{
    uint16_t data = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    gb.cpu.set_reg(instr.r1, data);
    return 3;
}

static std::string instr_ld_r16_d16_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint16_t data = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    sprintf(buf, "LD %s 0x%04x", reg_str[instr.r1], data);
    return buf;
}

static uint32_t instr_ld_mr_r8(Gameboy& gb, const Instr& instr)
{
    gb.memory.write(gb.cpu.get_reg16(instr.r1), gb.cpu.get_reg8(instr.r2));
    return 2;
}

static std::string instr_ld_mr_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_r_str(gb, instr, "LD");
}

static uint32_t instr_inc_r16(Gameboy& gb, const Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.cpu.get_reg16(instr.r1) + 1);
    return 2;
}

static std::string instr_inc_r16_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "INC");
}

static uint32_t instr_inc_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.get_reg8(instr.r1);
    uint8_t res = data + 1;
    gb.cpu.set_reg(instr.r1, res);
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h((data & 0x0f) == 0x0f);
    return 1;
}

static std::string instr_inc_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "INC");
}

static uint32_t instr_dec_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.cpu.get_reg8(instr.r1);
    gb.cpu.set_reg(instr.r1, data - 1);
    gb.cpu.flag_z(data - 1 == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((data & 0x0f) == 0);
    return 1;
}

static std::string instr_dec_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "DEC");
}

static uint32_t instr_ld_r8_d8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.cpu.set_reg(instr.r1, data);
    return 2;
}

static std::string instr_ld_r8_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_d8_str(gb, instr, "LD");
}

static uint32_t instr_rlca(Gameboy& gb, const Instr&)
{
    uint8_t data = gb.cpu.a();
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x80);
    data = (data << 1) | ((data & 0x80) != 0);
    gb.cpu.a() = data;
    return 1;
}

static std::string instr_rlca_str(Gameboy&, const Instr&)
{
    return "RLCA";
}

static uint32_t instr_ld_a16_r16(Gameboy& gb, const Instr& instr)
{
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    gb.memory.write16(addr, gb.cpu.get_reg16(instr.r1));
    return 5;
}

static std::string instr_ld_a16_r16_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    sprintf(buf, "LD (0x%04x) %s", addr, reg_str[instr.r1]);
    return buf;
}

static uint32_t instr_add_r16_r16(Gameboy& gb, const Instr& instr)
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

static std::string instr_add_r16_r16_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_r_str(gb, instr, "ADD");
}

static uint32_t instr_ld_r8_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.get_reg16(instr.r2));
    gb.cpu.set_reg(instr.r1, data);
    return 2;
}

static std::string instr_ld_r8_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_mr_str(gb, instr, "LD");
}

static uint32_t instr_dec_r16(Gameboy& gb, const Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.cpu.get_reg16(instr.r1) - 1);
    return 2;
}

static std::string instr_dec_r16_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "DEC");
}

static uint32_t instr_rrca(Gameboy& gb, const Instr&)
{
    uint8_t data = gb.cpu.a();
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(data & 0x01);
    data = (data >> 1) | ((data & 0x01) << 7);
    gb.cpu.a() = data;
    return 1;
}

static std::string instr_rrca_str(Gameboy&, const Instr&)
{
    return "RRCA";
}

static uint32_t instr_stop(Gameboy& gb, const Instr&)
{
    gb.cpu.pc += 1;
    gb.cpu.stopped = true;
    return 1;
}

static std::string instr_stop_str(Gameboy&, const Instr&)
{
    return "STOP";
}

static uint32_t instr_rla(Gameboy& gb, const Instr&)
{
    uint8_t c = gb.cpu.flag_c();
    gb.cpu.flag_c(gb.cpu.a() & 0x80);
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.a() = (gb.cpu.a() << 1) | (c != 0);
    return 1;
}

static std::string instr_rla_str(Gameboy&, const Instr&)
{
    return "RLA";
}

static uint32_t instr_jr_s8(Gameboy& gb, const Instr& instr)
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

static std::string instr_jr_s8_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    int8_t s8 = bit_cast<int8_t>(gb.memory.read(gb.cpu.pc));
    gb.cpu.pc += 1;
    if (instr.cond == Cond::NONE)
    {
        sprintf(buf, "JR 0x%04x", gb.cpu.pc + s8);
    }
    else
    {
        sprintf(buf, "JR %s, 0x%04x", cond_str[instr.cond], gb.cpu.pc + s8);
    }
    return buf;
}

static uint32_t instr_rra(Gameboy& gb, const Instr&)
{
    uint8_t c = gb.cpu.flag_c();
    gb.cpu.flag_c(gb.cpu.a() & 0x01);
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.a() = (gb.cpu.a() >> 1) | (c << 7);
    return 1;
}

static std::string instr_rra_str(Gameboy&, const Instr&)
{
    return "RRA";
}

static uint32_t instr_ld_hli_r8(Gameboy& gb, const Instr& instr)
{
    gb.memory.write(gb.cpu.hl(), gb.cpu.get_reg8(instr.r1));
    gb.cpu.hl() += 1;
    return 2;
}

static std::string instr_ld_hli_r8_str(Gameboy&, const Instr& instr)
{
    char buf[32] = {};
    sprintf(buf, "LD (HL+), %s", reg_str[instr.r1]);
    return buf;
}

static uint32_t instr_daa(Gameboy& gb, const Instr&)
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

static std::string instr_daa_str(Gameboy&, const Instr&)
{
    return "DAA";
}

static uint32_t instr_ld_r8_hli(Gameboy& gb, const Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.memory.read(gb.cpu.hl()));
    gb.cpu.hl() += 1;
    return 2;
}

static std::string instr_ld_r8_hli_str(Gameboy&, const Instr& instr)
{
    char buf[32] = {};
    sprintf(buf, "LD %s, (HL+)", reg_str[instr.r1]);
    return buf;
}

static uint32_t instr_cpl(Gameboy& gb, const Instr&)
{
    gb.cpu.a() ^= 0xff;
    gb.cpu.flag_n(1);
    gb.cpu.flag_h(1);
    return 1;
}

static std::string instr_cpl_str(Gameboy&, const Instr&)
{
    return "CPL";
}

static uint32_t instr_ld_hld_r8(Gameboy& gb, const Instr& instr)
{
    gb.memory.write(gb.cpu.hl(), gb.cpu.get_reg8(instr.r1));
    gb.cpu.hl() -= 1;
    return 2;
}

static std::string instr_ld_hld_r8_str(Gameboy&, const Instr& instr)
{
    char buf[32] = {};
    sprintf(buf, "LD (HL-), %s", reg_str[instr.r1]);
    return buf;
}

static uint32_t instr_inc_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    uint8_t res = data + 1;
    gb.memory.write(gb.cpu.get_reg16(instr.r1), res);
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h((data & 0x0f) == 0x0f);
    return 3;
}

static std::string instr_inc_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "INC");
}

static uint32_t instr_dec_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    gb.memory.write(gb.cpu.get_reg16(instr.r1), data - 1);
    gb.cpu.flag_z(data - 1 == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((data & 0x0f) == 0);
    return 3;
}

static std::string instr_dec_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "DEC");
}

static uint32_t instr_ld_mr_d8(Gameboy& gb, const Instr& instr)
{
    uint8_t data = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.memory.write(gb.cpu.get_reg16(instr.r1), data);
    return 3;
}

static std::string instr_ld_mr_d8_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint8_t data = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    sprintf(buf, "LD (%s), 0x%02x", reg_str[instr.r1], data);
    return buf;
}

static uint32_t instr_scf(Gameboy& gb, const Instr&)
{
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(1);
    return 1;
}

static std::string instr_scf_str(Gameboy&, const Instr&)
{
    return "SCF";
}

static uint32_t instr_ld_r8_hld(Gameboy& gb, const Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.memory.read(gb.cpu.hl()));
    gb.cpu.hl() -= 1;
    return 2;
}

static std::string instr_ld_r8_hld_str(Gameboy&, const Instr& instr)
{
    char buf[32] = {};
    sprintf(buf, "LD %s, (HL-)", reg_str[instr.r1]);
    return buf;
}

static uint32_t instr_ccf(Gameboy& gb, const Instr&)
{
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(0);
    gb.cpu.flag_c(gb.cpu.flag_c() ^ 1);
    return 1;
}

static std::string instr_ccf_str(Gameboy&, const Instr&)
{
    return "CCF";
}

static uint32_t instr_ld_r8_r8(Gameboy& gb, const Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.cpu.get_reg8(instr.r2));
    return 1;
}

static std::string instr_ld_r8_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_r_str(gb, instr, "LD");
}

static uint32_t instr_halt(Gameboy& gb, const Instr&)
{
    gb.cpu.halted = true;
    return 1;
}

static std::string instr_halt_str(Gameboy&, const Instr&)
{
    return "HALT";
}

static void _instr_add(Gameboy& gb, uint16_t x, uint16_t y, Reg r)
{
    uint16_t res = x + y;
    gb.cpu.set_reg(r, (uint8_t)res);
    gb.cpu.flag_z((uint8_t)res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(((x & 0x0f) + (y & 0x0f)) & 0x10);
    gb.cpu.flag_c(res > 0xff);
}

static uint32_t instr_add_r8_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.cpu.get_reg8(instr.r2);
    _instr_add(gb, x, y, instr.r1);
    return 1;
}

static std::string instr_add_r8_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_r_str(gb, instr, "ADD");
}

static uint32_t instr_add_r8_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r2));
    _instr_add(gb, x, y, instr.r1);
    return 2;
}

static std::string instr_add_r8_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_mr_str(gb, instr, "ADD");
}

static uint32_t instr_add_r8_d8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_add(gb, x, y, instr.r1);
    return 2;
}

static std::string instr_add_r8_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_d8_str(gb, instr, "ADD");
}

static void _instr_adc(Gameboy& gb, uint16_t x, uint16_t y, Reg r)
{
    uint16_t res = x + y + gb.cpu.flag_c();
    gb.cpu.set_reg(r, (uint8_t)res);
    gb.cpu.flag_z((uint8_t)res == 0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h(((x & 0x0f) + (y & 0x0f) + gb.cpu.flag_c()) & 0x10);
    gb.cpu.flag_c(res > 0xff);
}

static uint32_t instr_adc_r8_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.cpu.get_reg8(instr.r2);
    _instr_adc(gb, x, y, instr.r1);
    return 1;
}

static std::string instr_adc_r8_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_r_str(gb, instr, "ADC");
}

static uint32_t instr_adc_r8_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r2));
    _instr_adc(gb, x, y, instr.r1);
    return 2;
}

static std::string instr_adc_r8_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_mr_str(gb, instr, "ADC");
}

static uint32_t instr_adc_r8_d8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_adc(gb, x, y, instr.r1);
    return 2;
}

static std::string instr_adc_r8_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_d8_str(gb, instr, "ADC");
}

static void _instr_sub(Gameboy& gb, uint16_t x, uint16_t y)
{
    uint16_t res = x - y;
    gb.cpu.a() = (uint8_t)res;
    gb.cpu.flag_z((uint8_t)res == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((x & 0x0f) < (y & 0x0f));
    gb.cpu.flag_c(x < y);
}

static uint32_t instr_sub_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_sub(gb, x, y);
    return 1;
}

static std::string instr_sub_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "SUB");
}

static uint32_t instr_sub_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_sub(gb, x, y);
    return 2;
}

static std::string instr_sub_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "SUB");
}

static uint32_t instr_sub_d8(Gameboy& gb, const Instr&)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_sub(gb, x, y);
    return 2;
}

static std::string instr_sub_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_d8_str(gb, instr, "SUB");
}

static void _instr_sbc(Gameboy& gb, uint16_t x, uint16_t y, Reg r)
{
    uint16_t res = x - y - gb.cpu.flag_c();
    gb.cpu.set_reg(r, (uint8_t)res);
    gb.cpu.flag_z((uint8_t)res == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((x ^ y ^ res) & 0x10);
    gb.cpu.flag_c(res & 0x100);
}

static uint32_t instr_sbc_r8_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.cpu.get_reg8(instr.r2);
    _instr_sbc(gb, x, y, instr.r1);
    return 1;
}

static std::string instr_sbc_r8_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_r_str(gb, instr, "SBC");
}

static uint32_t instr_sbc_r8_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r2));
    _instr_sbc(gb, x, y, instr.r1);
    return 2;
}

static std::string instr_sbc_r8_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_mr_str(gb, instr, "SBC");
}

static uint32_t instr_sbc_r8_d8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.get_reg8(instr.r1);
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_sbc(gb, x, y, instr.r1);
    return 2;
}

static std::string instr_sbc_r8_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_d8_str(gb, instr, "SBC");
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

static uint32_t instr_and_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_and(gb, x, y);
    return 1;
}

static std::string instr_and_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "AND");
}

static uint32_t instr_and_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_and(gb, x, y);
    return 2;
}

static std::string instr_and_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "AND");
}

static uint32_t instr_and_d8(Gameboy& gb, const Instr&)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_and(gb, x, y);
    return 2;
}

static std::string instr_and_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_d8_str(gb, instr, "AND");
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

static uint32_t instr_xor_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_xor(gb, x, y);
    return 1;
}

static std::string instr_xor_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "XOR");
}

static uint32_t instr_xor_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_xor(gb, x, y);
    return 2;
}

static std::string instr_xor_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "XOR");
}

static uint32_t instr_xor_d8(Gameboy& gb, const Instr&)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_xor(gb, x, y);
    return 2;
}

static std::string instr_xor_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_d8_str(gb, instr, "XOR");
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

static uint32_t instr_or_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_or(gb, x, y);
    return 1;
}

static std::string instr_or_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "OR");
}

static uint32_t instr_or_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_or(gb, x, y);
    return 2;
}

static std::string instr_or_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "OR");
}

static uint32_t instr_or_d8(Gameboy& gb, const Instr&)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_or(gb, x, y);
    return 2;
}

static std::string instr_or_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_d8_str(gb, instr, "OR");
}

static void _instr_cp(Gameboy& gb, uint16_t x, uint16_t y)
{
    uint16_t res = x - y;
    gb.cpu.flag_z(res == 0);
    gb.cpu.flag_n(1);
    gb.cpu.flag_h((x & 0x0f) < (y & 0x0f));
    gb.cpu.flag_c(x < y);
}

static uint32_t instr_cp_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.cpu.get_reg8(instr.r1);
    _instr_cp(gb, x, y);
    return 1;
}

static std::string instr_cp_r8_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "CP");
}

static uint32_t instr_cp_mr(Gameboy& gb, const Instr& instr)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.get_reg16(instr.r1));
    _instr_cp(gb, x, y);
    return 2;
}

static std::string instr_cp_mr_str(Gameboy& gb, const Instr& instr)
{
    return instr_mr_str(gb, instr, "CP");
}

static uint32_t instr_cp_d8(Gameboy& gb, const Instr&)
{
    uint8_t x = gb.cpu.a();
    uint8_t y = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    _instr_cp(gb, x, y);
    return 2;
}

static std::string instr_cp_d8_str(Gameboy& gb, const Instr& instr)
{
    return instr_d8_str(gb, instr, "CP");
}

static uint32_t instr_ret(Gameboy& gb, const Instr& instr)
{
    if (!gb.cpu.check_condition(instr.cond))
    {
        return 2;
    }
    gb.cpu.pc = gb.memory.read16(gb.cpu.sp);
    gb.cpu.sp += 2;
    return instr.cond == Cond::NONE ? 4 : 5;
}

static std::string instr_ret_str(Gameboy&, const Instr& instr)
{
    char buf[32] = {};
    if (instr.cond == Cond::NONE)
    {
        sprintf(buf, "RET");
    }
    else
    {
        sprintf(buf, "RET %s", cond_str[instr.cond]);
    }
    return buf;
}

static uint32_t instr_pop(Gameboy& gb, const Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.memory.read16(gb.cpu.sp));
    gb.cpu.sp += 2;
    gb.cpu.f() = gb.cpu.f() & 0xf0;
    return 3;
}

static std::string instr_pop_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "POP");
}

static uint32_t instr_jp_a16(Gameboy& gb, const Instr& instr)
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

static std::string instr_jp_a16_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    if (instr.cond == Cond::NONE)
    {
        sprintf(buf, "JP 0x%04x", addr);
    }
    else
    {
        sprintf(buf, "JP %s, 0x%04x", cond_str[instr.cond], addr);
    }
    return buf;
}

static uint32_t instr_call(Gameboy& gb, const Instr& instr)
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

static std::string instr_call_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    if (instr.cond == Cond::NONE)
    {
        sprintf(buf, "CALL 0x%04x", addr);
    }
    else
    {
        sprintf(buf, "CALL %s, 0x%04x", cond_str[instr.cond], addr);
    }
    return buf;
}

static uint32_t instr_push(Gameboy& gb, const Instr& instr)
{
    gb.cpu.sp -= 2;
    gb.memory.write16(gb.cpu.sp, gb.cpu.get_reg16(instr.r1));
    return 4;
}

static std::string instr_push_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_str(gb, instr, "PUSH");
}

static uint32_t instr_rst(Gameboy& gb, const Instr& instr)
{
    uint16_t addr = [](uint8_t opcode) {
        switch (opcode)
        {
        case 0xc7:
            return 0x00;
        case 0xcf:
            return 0x08;
        case 0xd7:
            return 0x10;
        case 0xdf:
            return 0x18;
        case 0xe7:
            return 0x20;
        case 0xef:
            return 0x28;
        case 0xf7:
            return 0x30;
        case 0xff:
            return 0x38;
        default:
            ASSERT(!"Unreachable");
            return 0;
        }
    }(instr.opcode);
    gb.cpu.sp -= 2;
    gb.memory.write16(gb.cpu.sp, gb.cpu.pc);
    gb.cpu.pc = addr;
    return 4;
}

static std::string instr_rst_str(Gameboy& gb, const Instr&)
{
    char buf[32] = {};
    uint8_t addr = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    sprintf(buf, "RST 0x%02xH", addr);
    return buf;
}

static uint32_t instr_prefix_cb(Gameboy& gb, const Instr&)
{
    uint8_t cb_opcode = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    Instr instr = cb_instructions[cb_opcode];
    return instr.exec(gb, instr) + 1;
}

static std::string instr_prefix_cb_str(Gameboy& gb, const Instr&)
{
    uint8_t cb_opcode = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    Instr instr = cb_instructions[cb_opcode];
    return instr.to_string(gb, instr);
}

static uint32_t instr_reti(Gameboy& gb, const Instr& instr)
{
    gb.cpu.ime = true;
    return instr_ret(gb, instr);
}

static std::string instr_reti_str(Gameboy&, const Instr&)
{
    return "RETI";
}

static uint32_t instr_ldh_a8_r8(Gameboy& gb, const Instr& instr)
{
    uint8_t port = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.memory.write(0xff00 + port, gb.cpu.get_reg8(instr.r1));
    return 3;
}

static std::string instr_ldh_a8_r8_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint8_t addr = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    sprintf(buf, "LDH (0xff00 + 0x%02x), %s", addr, reg_str[instr.r1]);
    return buf;
}

static uint32_t instr_ldh_r8_a8(Gameboy& gb, const Instr& instr)
{
    uint8_t port = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    gb.cpu.set_reg(instr.r1, gb.memory.read(0xff00 + port));
    return 3;
}

static std::string instr_ldh_r8_a8_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint8_t addr = gb.memory.read(gb.cpu.pc);
    gb.cpu.pc += 1;
    sprintf(buf, "LDH %s, (0xff00 + 0x%02x)", reg_str[instr.r1], addr);
    return buf;
}

static uint32_t instr_ldh_mr_r8(Gameboy& gb, const Instr& instr)
{
    gb.memory.write(0xff00 + gb.cpu.get_reg8(instr.r1), gb.cpu.get_reg8(instr.r2));
    return 2;
}

static std::string instr_ldh_mr_r8_str(Gameboy&, const Instr& instr)
{
    char buf[32] = {};
    sprintf(buf, "LDH (0xff00 + %s), %s", reg_str[instr.r1], reg_str[instr.r2]);
    return buf;
}

static uint32_t instr_ldh_r8_mr(Gameboy& gb, const Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.memory.read(0xff00 + gb.cpu.get_reg8(instr.r2)));
    return 2;
}

static std::string instr_ldh_r8_mr_str(Gameboy&, const Instr& instr)
{
    char buf[32] = {};
    sprintf(buf, "LDH %s, (0xff00 + %s)", reg_str[instr.r1], reg_str[instr.r2]);
    return buf;
}

static uint32_t instr_ld_a16_r8(Gameboy& gb, const Instr& instr)
{
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    gb.memory.write(addr, gb.cpu.get_reg8(instr.r1));
    return 4;
}

static std::string instr_ld_a16_r8_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    sprintf(buf, "LD (0x%04x), %s", addr, reg_str[instr.r1]);
    return buf;
}

static uint32_t instr_ld_r8_a16(Gameboy& gb, const Instr& instr)
{
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    gb.cpu.set_reg(instr.r1, gb.memory.read(addr));
    return 4;
}

static std::string instr_ld_r8_a16_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    uint16_t addr = gb.memory.read16(gb.cpu.pc);
    gb.cpu.pc += 2;
    sprintf(buf, "LD %s, (0x%04x)", reg_str[instr.r1], addr);
    return buf;
}

static uint32_t instr_add_r16_s8(Gameboy& gb, const Instr& instr)
{
    int8_t data = bit_cast<int8_t>(gb.memory.read(gb.cpu.pc));
    gb.cpu.pc += 1;
    int32_t r16 = gb.cpu.get_reg16(instr.r1);
    int32_t res = r16 + data;
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h((r16 ^ data ^ res) & 0x0010);
    gb.cpu.flag_c((r16 ^ data ^ res) & 0x0100);
    gb.cpu.set_reg(instr.r1, res);
    return 4;
}

static std::string instr_add_r16_s8_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    int16_t s8 = bit_cast<int8_t>(gb.memory.read(gb.cpu.pc));
    gb.cpu.pc += 1;
    char sign = ' ';
    if (s8 < 0)
    {
        sign = '-';
        s8 = -s8;
    }
    sprintf(buf, "ADD %s, %c0x%02x", reg_str[instr.r1], sign, s8);
    return buf;
}

static uint32_t instr_jp_r16(Gameboy& gb, const Instr& instr)
{
    gb.cpu.pc = gb.cpu.get_reg16(instr.r1);
    return 1;
}

static std::string instr_jp_r16_str(Gameboy&, const Instr& instr)
{
    char buf[32] = {};
    sprintf(buf, "JP %s", reg_str[instr.r1]);
    return buf;
}

static uint32_t instr_ei(Gameboy& gb, const Instr&)
{
    gb.cpu.enable_interrupts = true;
    return 1;
}

static std::string instr_ei_str(Gameboy&, const Instr&)
{
    return "EI";
}

static uint32_t instr_di(Gameboy& gb, const Instr&)
{
    gb.cpu.ime = false;
    return 1;
}

static std::string instr_di_str(Gameboy&, const Instr&)
{
    return "DI";
}

static uint32_t instr_ld_r16_r16s8(Gameboy& gb, const Instr& instr)
{
    int8_t data = bit_cast<int8_t>(gb.memory.read(gb.cpu.pc));
    gb.cpu.pc += 1;
    uint16_t r16 = gb.cpu.get_reg16(instr.r2);
    int32_t res = r16 + data;
    gb.cpu.flag_z(0);
    gb.cpu.flag_n(0);
    gb.cpu.flag_h((r16 ^ data ^ res) & 0x0010);
    gb.cpu.flag_c((r16 ^ data ^ res) & 0x0100);
    gb.cpu.set_reg(instr.r1, res);
    return 3;
}

static std::string instr_ld_r16_r16s8_str(Gameboy& gb, const Instr& instr)
{
    char buf[32] = {};
    int16_t s8 = bit_cast<int8_t>(gb.memory.read(gb.cpu.pc));
    gb.cpu.pc += 1;
    char sign = '+';
    if (s8 < 0)
    {
        sign = '-';
        s8 = -s8;
    }
    sprintf(buf, "LD %s, %s %c 0x%02x", reg_str[instr.r1], reg_str[instr.r2], sign, s8);
    return buf;
}

static uint32_t instr_ld_r16_r16(Gameboy& gb, const Instr& instr)
{
    gb.cpu.set_reg(instr.r1, gb.cpu.get_reg16(instr.r2));
    return 2;
}

static std::string instr_ld_r16_r16_str(Gameboy& gb, const Instr& instr)
{
    return instr_r_r_str(gb, instr, "LD");
}

// clang-format off
Instr instructions[0x100] = {
    {0x00, instr_nop, instr_nop_str},
    {0x01, instr_ld_r16_d16, instr_ld_r16_d16_str, Reg::BC},
    {0x02, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::BC, Reg::A},
    {0x03, instr_inc_r16, instr_inc_r16_str, Reg::BC},
    {0x04, instr_inc_r8, instr_inc_r8_str, Reg::B},
    {0x05, instr_dec_r8, instr_dec_r8_str, Reg::B},
    {0x06, instr_ld_r8_d8, instr_ld_r8_d8_str, Reg::B},
    {0x07, instr_rlca, instr_rlca_str},
    {0x08, instr_ld_a16_r16, instr_ld_a16_r16_str, Reg::SP},
    {0x09, instr_add_r16_r16, instr_add_r16_r16_str, Reg::HL, Reg::BC},
    {0x0a, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::A, Reg::BC},
    {0x0b, instr_dec_r16, instr_dec_r16_str, Reg::BC},
    {0x0c, instr_inc_r8, instr_inc_r8_str, Reg::C},
    {0x0d, instr_dec_r8, instr_dec_r8_str, Reg::C},
    {0x0e, instr_ld_r8_d8, instr_ld_r8_d8_str, Reg::C},
    {0x0f, instr_rrca, instr_rrca_str},
    {0x10, instr_stop, instr_stop_str},
    {0x11, instr_ld_r16_d16, instr_ld_r16_d16_str, Reg::DE},
    {0x12, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::DE, Reg::A},
    {0x13, instr_inc_r16, instr_inc_r16_str, Reg::DE},
    {0x14, instr_inc_r8, instr_inc_r8_str, Reg::D},
    {0x15, instr_dec_r8, instr_dec_r8_str, Reg::D},
    {0x16, instr_ld_r8_d8, instr_ld_r8_d8_str, Reg::D},
    {0x17, instr_rla, instr_rla_str},
    {0x18, instr_jr_s8, instr_jr_s8_str},
    {0x19, instr_add_r16_r16, instr_add_r16_r16_str, Reg::HL, Reg::DE},
    {0x1a, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::A, Reg::DE},
    {0x1b, instr_dec_r16, instr_dec_r16_str, Reg::DE},
    {0x1c, instr_inc_r8, instr_inc_r8_str, Reg::E},
    {0x1d, instr_dec_r8, instr_dec_r8_str, Reg::E},
    {0x1e, instr_ld_r8_d8, instr_ld_r8_d8_str, Reg::E},
    {0x1f, instr_rra, instr_rra_str},
    {0x20, instr_jr_s8, instr_jr_s8_str, Reg::NONE, Reg::NONE, Cond::NZ},
    {0x21, instr_ld_r16_d16, instr_ld_r16_d16_str, Reg::HL},
    {0x22, instr_ld_hli_r8, instr_ld_hli_r8_str, Reg::A},
    {0x23, instr_inc_r16, instr_inc_r16_str, Reg::HL},
    {0x24, instr_inc_r8, instr_inc_r8_str, Reg::H},
    {0x25, instr_dec_r8, instr_dec_r8_str, Reg::H},
    {0x26, instr_ld_r8_d8, instr_ld_r8_d8_str, Reg::H},
    {0x27, instr_daa, instr_daa_str},
    {0x28, instr_jr_s8, instr_jr_s8_str, Reg::NONE, Reg::NONE, Cond::Z},
    {0x29, instr_add_r16_r16, instr_add_r16_r16_str, Reg::HL, Reg::HL},
    {0x2a, instr_ld_r8_hli, instr_ld_r8_hli_str, Reg::A},
    {0x2b, instr_dec_r16, instr_dec_r16_str, Reg::HL},
    {0x2c, instr_inc_r8, instr_inc_r8_str, Reg::L},
    {0x2d, instr_dec_r8, instr_dec_r8_str, Reg::L},
    {0x2e, instr_ld_r8_d8, instr_ld_r8_d8_str, Reg::L},
    {0x2f, instr_cpl, instr_cpl_str},
    {0x30, instr_jr_s8, instr_jr_s8_str, Reg::NONE, Reg::NONE, Cond::NC},
    {0x31, instr_ld_r16_d16, instr_ld_r16_d16_str, Reg::SP},
    {0x32, instr_ld_hld_r8, instr_ld_hld_r8_str, Reg::A},
    {0x33, instr_inc_r16, instr_inc_r16_str, Reg::SP},
    {0x34, instr_inc_mr, instr_inc_mr_str, Reg::HL},
    {0x35, instr_dec_mr, instr_dec_mr_str, Reg::HL},
    {0x36, instr_ld_mr_d8, instr_ld_mr_d8_str, Reg::HL},
    {0x37, instr_scf, instr_scf_str},
    {0x38, instr_jr_s8, instr_jr_s8_str, Reg::NONE, Reg::NONE, Cond::C},
    {0x39, instr_add_r16_r16, instr_add_r16_r16_str, Reg::HL, Reg::SP},
    {0x3a, instr_ld_r8_hld, instr_ld_r8_hld_str, Reg::A},
    {0x3b, instr_dec_r16, instr_dec_r16_str, Reg::SP},
    {0x3c, instr_inc_r8, instr_inc_r8_str, Reg::A},
    {0x3d, instr_dec_r8, instr_dec_r8_str, Reg::A},
    {0x3e, instr_ld_r8_d8, instr_ld_r8_d8_str, Reg::A},
    {0x3f, instr_ccf, instr_ccf_str},
    {0x40, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::B, Reg::B},
    {0x41, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::B, Reg::C},
    {0x42, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::B, Reg::D},
    {0x43, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::B, Reg::E},
    {0x44, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::B, Reg::H},
    {0x45, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::B, Reg::L},
    {0x46, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::B, Reg::HL},
    {0x47, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::B, Reg::A},
    {0x48, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::C, Reg::B},
    {0x49, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::C, Reg::C},
    {0x4a, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::C, Reg::D},
    {0x4b, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::C, Reg::E},
    {0x4c, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::C, Reg::H},
    {0x4d, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::C, Reg::L},
    {0x4e, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::C, Reg::HL},
    {0x4f, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::C, Reg::A},
    {0x50, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::D, Reg::B},
    {0x51, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::D, Reg::C},
    {0x52, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::D, Reg::D},
    {0x53, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::D, Reg::E},
    {0x54, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::D, Reg::H},
    {0x55, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::D, Reg::L},
    {0x56, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::D, Reg::HL},
    {0x57, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::D, Reg::A},
    {0x58, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::E, Reg::B},
    {0x59, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::E, Reg::C},
    {0x5a, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::E, Reg::D},
    {0x5b, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::E, Reg::E},
    {0x5c, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::E, Reg::H},
    {0x5d, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::E, Reg::L},
    {0x5e, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::E, Reg::HL},
    {0x5f, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::E, Reg::A},
    {0x60, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::H, Reg::B},
    {0x61, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::H, Reg::C},
    {0x62, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::H, Reg::D},
    {0x63, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::H, Reg::E},
    {0x64, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::H, Reg::H},
    {0x65, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::H, Reg::L},
    {0x66, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::H, Reg::HL},
    {0x67, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::H, Reg::A},
    {0x68, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::L, Reg::B},
    {0x69, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::L, Reg::C},
    {0x6a, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::L, Reg::D},
    {0x6b, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::L, Reg::E},
    {0x6c, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::L, Reg::H},
    {0x6d, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::L, Reg::L},
    {0x6e, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::L, Reg::HL},
    {0x6f, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::L, Reg::A},
    {0x70, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::HL, Reg::B},
    {0x71, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::HL, Reg::C},
    {0x72, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::HL, Reg::D},
    {0x73, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::HL, Reg::E},
    {0x74, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::HL, Reg::H},
    {0x75, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::HL, Reg::L},
    {0x76, instr_halt, instr_halt_str},
    {0x77, instr_ld_mr_r8, instr_ld_mr_r8_str, Reg::HL, Reg::A},
    {0x78, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::A, Reg::B},
    {0x79, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::A, Reg::C},
    {0x7a, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::A, Reg::D},
    {0x7b, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::A, Reg::E},
    {0x7c, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::A, Reg::H},
    {0x7d, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::A, Reg::L},
    {0x7e, instr_ld_r8_mr, instr_ld_r8_mr_str, Reg::A, Reg::HL},
    {0x7f, instr_ld_r8_r8, instr_ld_r8_r8_str, Reg::A, Reg::A},
    {0x80, instr_add_r8_r8, instr_add_r8_r8_str, Reg::A, Reg::B},
    {0x81, instr_add_r8_r8, instr_add_r8_r8_str, Reg::A, Reg::C},
    {0x82, instr_add_r8_r8, instr_add_r8_r8_str, Reg::A, Reg::D},
    {0x83, instr_add_r8_r8, instr_add_r8_r8_str, Reg::A, Reg::E},
    {0x84, instr_add_r8_r8, instr_add_r8_r8_str, Reg::A, Reg::H},
    {0x85, instr_add_r8_r8, instr_add_r8_r8_str, Reg::A, Reg::L},
    {0x86, instr_add_r8_mr, instr_add_r8_mr_str, Reg::A, Reg::HL},
    {0x87, instr_add_r8_r8, instr_add_r8_r8_str, Reg::A, Reg::A},
    {0x88, instr_adc_r8_r8, instr_adc_r8_r8_str, Reg::A, Reg::B},
    {0x89, instr_adc_r8_r8, instr_adc_r8_r8_str, Reg::A, Reg::C},
    {0x8a, instr_adc_r8_r8, instr_adc_r8_r8_str, Reg::A, Reg::D},
    {0x8b, instr_adc_r8_r8, instr_adc_r8_r8_str, Reg::A, Reg::E},
    {0x8c, instr_adc_r8_r8, instr_adc_r8_r8_str, Reg::A, Reg::H},
    {0x8d, instr_adc_r8_r8, instr_adc_r8_r8_str, Reg::A, Reg::L},
    {0x8e, instr_adc_r8_mr, instr_adc_r8_mr_str, Reg::A, Reg::HL},
    {0x8f, instr_adc_r8_r8, instr_adc_r8_r8_str, Reg::A, Reg::A},
    {0x90, instr_sub_r8, instr_sub_r8_str, Reg::B},
    {0x91, instr_sub_r8, instr_sub_r8_str, Reg::C},
    {0x92, instr_sub_r8, instr_sub_r8_str, Reg::D},
    {0x93, instr_sub_r8, instr_sub_r8_str, Reg::E},
    {0x94, instr_sub_r8, instr_sub_r8_str, Reg::H},
    {0x95, instr_sub_r8, instr_sub_r8_str, Reg::L},
    {0x96, instr_sub_mr, instr_sub_mr_str, Reg::HL},
    {0x97, instr_sub_r8, instr_sub_r8_str, Reg::A},
    {0x98, instr_sbc_r8_r8, instr_sbc_r8_r8_str, Reg::A, Reg::B},
    {0x99, instr_sbc_r8_r8, instr_sbc_r8_r8_str, Reg::A, Reg::C},
    {0x9a, instr_sbc_r8_r8, instr_sbc_r8_r8_str, Reg::A, Reg::D},
    {0x9b, instr_sbc_r8_r8, instr_sbc_r8_r8_str, Reg::A, Reg::E},
    {0x9c, instr_sbc_r8_r8, instr_sbc_r8_r8_str, Reg::A, Reg::H},
    {0x9d, instr_sbc_r8_r8, instr_sbc_r8_r8_str, Reg::A, Reg::L},
    {0x9e, instr_sbc_r8_mr, instr_sbc_r8_mr_str, Reg::A, Reg::HL},
    {0x9f, instr_sbc_r8_r8, instr_sbc_r8_r8_str, Reg::A, Reg::A},
    {0xa0, instr_and_r8, instr_and_r8_str, Reg::B},
    {0xa1, instr_and_r8, instr_and_r8_str, Reg::C},
    {0xa2, instr_and_r8, instr_and_r8_str, Reg::D},
    {0xa3, instr_and_r8, instr_and_r8_str, Reg::E},
    {0xa4, instr_and_r8, instr_and_r8_str, Reg::H},
    {0xa5, instr_and_r8, instr_and_r8_str, Reg::L},
    {0xa6, instr_and_mr, instr_and_mr_str, Reg::HL},
    {0xa7, instr_and_r8, instr_and_r8_str, Reg::A},
    {0xa8, instr_xor_r8, instr_xor_r8_str, Reg::B},
    {0xa9, instr_xor_r8, instr_xor_r8_str, Reg::C},
    {0xaa, instr_xor_r8, instr_xor_r8_str, Reg::D},
    {0xab, instr_xor_r8, instr_xor_r8_str, Reg::E},
    {0xac, instr_xor_r8, instr_xor_r8_str, Reg::H},
    {0xad, instr_xor_r8, instr_xor_r8_str, Reg::L},
    {0xae, instr_xor_mr, instr_xor_mr_str, Reg::HL},
    {0xaf, instr_xor_r8, instr_xor_r8_str, Reg::A},
    {0xb0, instr_or_r8, instr_or_r8_str, Reg::B},
    {0xb1, instr_or_r8, instr_or_r8_str, Reg::C},
    {0xb2, instr_or_r8, instr_or_r8_str, Reg::D},
    {0xb3, instr_or_r8, instr_or_r8_str, Reg::E},
    {0xb4, instr_or_r8, instr_or_r8_str, Reg::H},
    {0xb5, instr_or_r8, instr_or_r8_str, Reg::L},
    {0xb6, instr_or_mr, instr_or_mr_str, Reg::HL},
    {0xb7, instr_or_r8, instr_or_r8_str, Reg::A},
    {0xb8, instr_cp_r8, instr_cp_r8_str, Reg::B},
    {0xb9, instr_cp_r8, instr_cp_r8_str, Reg::C},
    {0xba, instr_cp_r8, instr_cp_r8_str, Reg::D},
    {0xbb, instr_cp_r8, instr_cp_r8_str, Reg::E},
    {0xbc, instr_cp_r8, instr_cp_r8_str, Reg::H},
    {0xbd, instr_cp_r8, instr_cp_r8_str, Reg::L},
    {0xbe, instr_cp_mr, instr_cp_mr_str, Reg::HL},
    {0xbf, instr_cp_r8, instr_cp_r8_str, Reg::A},
    {0xc0, instr_ret, instr_ret_str, Reg::NONE, Reg::NONE, Cond::NZ},
    {0xc1, instr_pop, instr_pop_str, Reg::BC},
    {0xc2, instr_jp_a16, instr_jp_a16_str, Reg::NONE, Reg::NONE, Cond::NZ},
    {0xc3, instr_jp_a16, instr_jp_a16_str},
    {0xc4, instr_call, instr_call_str, Reg::NONE, Reg::NONE, Cond::NZ},
    {0xc5, instr_push, instr_push_str, Reg::BC},
    {0xc6, instr_add_r8_d8, instr_add_r8_d8_str, Reg::A},
    {0xc7, instr_rst, instr_rst_str},
    {0xc8, instr_ret, instr_ret_str, Reg::NONE, Reg::NONE, Cond::Z},
    {0xc9, instr_ret, instr_ret_str},
    {0xca, instr_jp_a16, instr_jp_a16_str, Reg::NONE, Reg::NONE, Cond::Z},
    {0xcb, instr_prefix_cb, instr_prefix_cb_str},
    {0xcc, instr_call, instr_call_str, Reg::NONE, Reg::NONE, Cond::Z},
    {0xcd, instr_call, instr_call_str},
    {0xce, instr_adc_r8_d8, instr_adc_r8_d8_str, Reg::A},
    {0xcf, instr_rst, instr_rst_str},
    {0xd0, instr_ret, instr_ret_str, Reg::NONE, Reg::NONE, Cond::NC},
    {0xd1, instr_pop, instr_pop_str, Reg::DE},
    {0xd2, instr_jp_a16, instr_jp_a16_str, Reg::NONE, Reg::NONE, Cond::NC},
    {0xd3},
    {0xd4, instr_call, instr_call_str, Reg::NONE, Reg::NONE, Cond::NC},
    {0xd5, instr_push, instr_push_str, Reg::DE},
    {0xd6, instr_sub_d8, instr_sub_d8_str},
    {0xd7, instr_rst, instr_rst_str},
    {0xd8, instr_ret, instr_ret_str, Reg::NONE, Reg::NONE, Cond::C},
    {0xd9, instr_reti, instr_reti_str},
    {0xda, instr_jp_a16, instr_jp_a16_str, Reg::NONE, Reg::NONE, Cond::C},
    {0xdb},
    {0xdc, instr_call, instr_call_str, Reg::NONE, Reg::NONE, Cond::C},
    {0xdd},
    {0xde, instr_sbc_r8_d8, instr_sbc_r8_d8_str, Reg::A},
    {0xdf, instr_rst, instr_rst_str},
    {0xe0, instr_ldh_a8_r8, instr_ldh_a8_r8_str, Reg::A},
    {0xe1, instr_pop, instr_pop_str, Reg::HL},
    {0xe2, instr_ldh_mr_r8, instr_ldh_mr_r8_str, Reg::C, Reg::A},
    {0xe3},
    {0xe4},
    {0xe5, instr_push, instr_push_str, Reg::HL},
    {0xe6, instr_and_d8, instr_and_d8_str},
    {0xe7, instr_rst, instr_rst_str},
    {0xe8, instr_add_r16_s8, instr_add_r16_s8_str, Reg::SP},
    {0xe9, instr_jp_r16, instr_jp_r16_str, Reg::HL},
    {0xea, instr_ld_a16_r8, instr_ld_a16_r8_str, Reg::A},
    {0xeb},
    {0xec},
    {0xed},
    {0xee, instr_xor_d8, instr_xor_d8_str},
    {0xef, instr_rst, instr_rst_str},
    {0xf0, instr_ldh_r8_a8, instr_ldh_r8_a8_str, Reg::A},
    {0xf1, instr_pop, instr_pop_str, Reg::AF},
    {0xf2, instr_ldh_r8_mr, instr_ldh_r8_mr_str, Reg::A, Reg::C},
    {0xf3, instr_di, instr_di_str},
    {0xf4},
    {0xf5, instr_push, instr_push_str, Reg::AF},
    {0xf6, instr_or_d8, instr_or_d8_str},
    {0xf7, instr_rst, instr_rst_str},
    {0xf8, instr_ld_r16_r16s8, instr_ld_r16_r16s8_str, Reg::HL, Reg::SP},
    {0xf9, instr_ld_r16_r16, instr_ld_r16_r16_str, Reg::SP, Reg::HL},
    {0xfa, instr_ld_r8_a16, instr_ld_r8_a16_str, Reg::A},
    {0xfb, instr_ei, instr_ei_str},
    {0xfc},
    {0xfd},
    {0xfe, instr_cp_d8, instr_cp_d8_str},
    {0xff, instr_rst, instr_rst_str}
};
// clang-format off
