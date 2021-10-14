#include "gb_proxy.h"

#define GB_INTERNAL // I solumnly swear I am up to no good
#include <Core/gb.h>

// from SameBoy's debugger.c (struct is not exposed)
struct GB_breakpoint_s {
    union {
        struct {
        uint16_t addr;
        uint16_t bank; /* -1 = any bank*/
        };
        uint32_t key; /* For sorting and comparing */
    };
    char *condition;
    bool is_jump_to;
};

static inline uint8_t *get_8_bit_gb_register_address(struct GB_gameboy_s *gb, sm83_register r) {
    switch(r) {
        case SM83_REG_A:
            return &gb->a;
        case SM83_REG_B:
            return &gb->b;
        case SM83_REG_C:
            return &gb->c;
        case SM83_REG_D:
            return &gb->d;
        case SM83_REG_E:
            return &gb->e;
        case SM83_REG_F:
            return &gb->f;
        default:
            return NULL;
    }
}
static inline uint16_t *get_16_bit_gb_register_address(struct GB_gameboy_s *gb, sm83_register r) {
    switch(r) {
        case SM83_REG_HL:
            return &gb->hl;
        case SM83_REG_PC:
            return &gb->pc;
        case SM83_REG_SP:
            return &gb->sp;
        case SM83_REG_BC:
            return &gb->bc;
        case SM83_REG_AF:
            return &gb->af;
        case SM83_REG_DE:
            return &gb->de;
        default:
            return NULL;
    }
}

static inline uint8_t get_8_bit_gb_register(const struct GB_gameboy_s *gb, sm83_register r) {
    return *get_8_bit_gb_register_address((struct GB_gameboy_s *)gb, r);
}
static inline uint16_t get_16_bit_gb_register(const struct GB_gameboy_s *gb, sm83_register r) {
    return *get_16_bit_gb_register_address((struct GB_gameboy_s *)gb, r);
}

static void set_8_bit_gb_register(struct GB_gameboy_s *gb, sm83_register r, uint8_t v) {
    *get_8_bit_gb_register_address(gb, r) = v;
}
static void set_16_bit_gb_register(struct GB_gameboy_s *gb, sm83_register r, uint16_t v) {
    *get_16_bit_gb_register_address(gb, r) = v;
}

uint16_t get_gb_register(const struct GB_gameboy_s *gb, sm83_register r) {
    switch(r) {
        case SM83_REG_A:
        case SM83_REG_B:
        case SM83_REG_C:
        case SM83_REG_D:
        case SM83_REG_E:
        case SM83_REG_F:
            return get_8_bit_gb_register(gb, r);
        case SM83_REG_HL:
        case SM83_REG_PC:
        case SM83_REG_SP:
        case SM83_REG_AF:
        case SM83_REG_BC:
        case SM83_REG_DE:
            return get_16_bit_gb_register(gb, r);
        default:
            abort();
    }
}

void set_gb_register(struct GB_gameboy_s *gb, sm83_register r, uint16_t v) {
    switch(r) {
        case SM83_REG_A:
        case SM83_REG_B:
        case SM83_REG_C:
        case SM83_REG_D:
        case SM83_REG_E:
        case SM83_REG_F:
            return set_8_bit_gb_register(gb, r, v);
        case SM83_REG_HL:
        case SM83_REG_PC:
        case SM83_REG_SP:
        case SM83_REG_AF:
        case SM83_REG_BC:
        case SM83_REG_DE:
            return set_16_bit_gb_register(gb, r, v);
        default:
            abort();
    }
}

uint32_t get_gb_backtrace_size(const struct GB_gameboy_s *gb) {
    return gb->backtrace_size;
}
uint16_t get_gb_backtrace_address(const struct GB_gameboy_s *gb, uint32_t bt) {
    return gb->backtrace_returns[bt].addr;
}

uint32_t get_gb_breakpoint_size(const struct GB_gameboy_s *gb) {
    return gb->n_breakpoints;
}
uint16_t get_gb_breakpoint_address(const struct GB_gameboy_s *gb, uint32_t bt) {
    return gb->breakpoints[bt].addr;
}
