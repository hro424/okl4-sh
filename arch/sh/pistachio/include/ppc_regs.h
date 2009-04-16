#ifndef OKL4_ARCH_SH_PPC_REGS_H
#define OKL4_ARCH_SH_PPC_REGS_H

//XXX: CONFIDENTIAL START
#define REG_CCBR0               0xFF200800
#define REG_CCBR1               0xFF200804
#define REG_BCBR0               0xFF120800
#define REG_BCAR0               0xFF120804
#define REG_BCAMR0              0xFF120808
#define REG_BCMID0              0xFF12080C
#define REG_BCBR1               0xFF120810
#define REG_BCAR1               0xFF120814
#define REG_BCAMR1              0xFF120818
#define REG_BCMID1              0xFF12081C
#define REG_PMCTR0              0xFC100000
#define REG_PMCTR1              0xFC100004
#define REG_PMCTR2              0xFC100008
#define REG_PMCTR3              0xFC10000C
#define REG_PMCAT               0xFC100080

#define REG_CCBR_PCS_MASK       0x1F000000
#define REG_CCBR_PCS_0          0x00000000
#define REG_CCBR_PCS_1          0x01000000

#define REG_CCBR_PCE_MASK       0x001F0000
#define REG_CCBR_PCE_0          0x00000000
#define REG_CCBR_PCE_1          0x00010000

#define REG_CCBR_CIT_MASK       0x0000FFC0
#define REG_CCBR_CIT_CYCLE      0x00000000
#define REG_CCBR_CIT_CYCLE_PR   0x00008000
#define REG_CCBR_CIT_CYCLE_SRBL 0x0000A000
#define REG_CCBR_CIT_INST       0x00008080
#define REG_CCBR_CIT_INST_2     0x000080C0
#define REG_CCBR_CIT_BR         0x00008100
#define REG_CCBR_CIT_EXP        0x00008200
#define REG_CCBR_CIT_INT        0x00008240
#define REG_CCBR_CIT_UTLB_I     0x00008800
#define REG_CCBR_CIT_UTLB_D     0x00008880
#define REG_CCBR_CIT_ITLB       0x0000B800
#define REG_CCBR_CIT_I_MEMORY   0x00000B00
#define REG_CCBR_CIT_I_CACHE    0x00000B40
#define REG_CCBR_CIT_I_MISS     0x00000B80
#define REG_CCBR_CIT_D_MEMORY_R 0x00000C00
#define REG_CCBR_CIT_D_MEMORY_W 0x00000E00
#define REG_CCBR_CIT_D_CACHE_R  0x00000C40
#define REG_CCBR_CIT_D_CACHE_W  0x00000E40
#define REG_CCBR_CIT_D_MISS_R   0x00000C80
#define REG_CCBR_CIT_D_MISS_W   0x00000E80
#define REG_CCBR_CIT_D_WAIT_R   0x0000BD00
#define REG_CCBR_CIT_D_WAIT_W   0x0000BF00
#define REG_CCBR_CIT_D_WAIT_MISS_R  0x00000CC0
#define REG_CCBR_CIT_D_WAIT_MISS_W  0x00000EC0

#define REG_CCBR_DUC            0x00000008
#define REG_CCBR_CMDS           0x00000002
#define REG_CCBR_PPCE           0x00000001

#define REG_PMCAT_OVF3          0x08000000
#define REG_PMCAT_CNN3          0x04000000
#define REG_PMCAT_CLR3          0x02000000
#define REG_PMCAT_OVF2          0x00080000
#define REG_PMCAT_CLR2          0x00020000
#define REG_PMCAT_OVF1          0x00000800
#define REG_PMCAT_CNN1          0x00000400
#define REG_PMCAT_CLR1          0x00000200
#define REG_PMCAT_OVF0          0x00000008
#define REG_PMCAT_CLR0          0x00000002

class sh_ppc_cpu
{
public:
    static void enable(word_t reg) {
        word_t val = mapped_reg_read(reg);
        val |= REG_CCBR_PPCE;
        mapped_reg_write(reg, val);
    }

    static void disable(word_t reg) {
        word_t val = mapped_reg_read(reg);
        val &= ~REG_CCBR_PPCE;
        mapped_reg_write(reg, val);
    }

    static void config(word_t reg, word_t cit) {
        word_t val = mapped_reg_read(reg);
        val &= ~REG_CCBR_CIT_MASK;
        val |= cit;
        mapped_reg_write(reg, val);
    }

    static void set_direct_mode(word_t reg) {
        word_t val = mapped_reg_read(reg);
        val |= REG_CCBR_CMDS;
        mapped_reg_write(reg, val);
    }

    static void set_trigger_mode(word_t reg, word_t start, word_t stop) {
        word_t val = mapped_reg_read(reg);
        val &= ~REG_CCBR_CMDS;
        val |= REG_CCBR_PCE_1;
        mapped_reg_write(reg, val);

        do {
            mapped_reg_write(REG_UBC_CBR0, REG_UBC_CBR_RW_R | REG_UBC_CBR_CE);
        } while (mapped_reg_read(REG_UBC_CBR0) != (REG_UBC_CBR_RW_R | REG_UBC_CBR_CE));
        do {
            mapped_reg_write(REG_UBC_CAR0, start);
        } while (mapped_reg_read(REG_UBC_CAR0) != start);
        do {
            mapped_reg_write(REG_UBC_CAMR0, 0);
        } while (mapped_reg_read(REG_UBC_CAMR0) != 0);

        do {
            mapped_reg_write(REG_UBC_CBR1, REG_UBC_CBR_RW_R | REG_UBC_CBR_CE);
        } while (mapped_reg_read(REG_UBC_CBR1) != (REG_UBC_CBR_RW_R | REG_UBC_CBR_CE));
        do {
            mapped_reg_write(REG_UBC_CAR1, stop);
        } while (mapped_reg_read(REG_UBC_CAR1) != stop);
        do {
            mapped_reg_write(REG_UBC_CAMR1, 0);
        } while (mapped_reg_read(REG_UBC_CAMR1) != 0);
    }

    static void start(word_t reg) {
        word_t conf = mapped_reg_read(reg);
        if (conf & REG_CCBR_CMDS) {
            conf |= REG_CCBR_DUC;
            mapped_reg_write(reg, conf);
        }
    }

    static void stop(word_t reg) {
        word_t conf = mapped_reg_read(reg);
        if (conf & REG_CCBR_CMDS) {
            conf &= ~REG_CCBR_DUC;
            mapped_reg_write(reg, conf);
        }
    }

    static void restart(word_t reg) {
        reset(reg);
        start(reg);
    }

    static bool is_counting(word_t reg) {
        word_t val = mapped_reg_read(reg);
        return (val & REG_CCBR_DUC != 0);
    }


    static u64_t get_counter(word_t reg) {
        u64_t val = 0;
        if (reg == REG_CCBR0) {
            val = (u64_t)mapped_reg_read(REG_PMCTR0);
        }
        else if (reg == REG_CCBR1) {
            val = (u64_t)mapped_reg_read(REG_PMCTR1);
        }
        return val;
    }

    static void get_counter(u32_t* counter0, u32_t* counter1) {
        *counter0 = mapped_reg_read(REG_PMCTR0);
        *counter1 = mapped_reg_read(REG_PMCTR1);
    }

    static void reset(word_t reg) {
        word_t val = mapped_reg_read(REG_PMCAT);
        if (reg == REG_CCBR0) {
            if (val & REG_PMCAT_CNN1) {
                val |= REG_PMCAT_CLR1;
            }
            val |= REG_PMCAT_CLR0;
        }
        else if (reg == REG_CCBR1) {
            val |= REG_PMCAT_CLR1;
        }
        mapped_reg_write(REG_PMCAT, val);
    }

    static void expand_counter() {
        disable(REG_CCBR1);
        word_t attr = mapped_reg_read(REG_PMCAT);
        attr |= REG_PMCAT_CNN1;
        mapped_reg_write(REG_PMCAT, attr);
    }

    static void separate_counter() {
        word_t val = mapped_reg_read(REG_PMCAT);
        val &= ~REG_PMCAT_CNN1;
        mapped_reg_write(REG_PMCAT, val);
    }
};

//XXX: CONFIDENTIAL END

#endif // OKL4_ARCH_SH_PPC_REGS_H
