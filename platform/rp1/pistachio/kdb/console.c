/* $Id$ */

/**
 * @since   February 2009
 * @see     SH hardware manual
 */

#include <console.h>
#include <soc/soc.h>
#include <kernel/arch/registers.h>

static word_t   scif_base;

INLINE void
scif_write(word_t reg, u16_t var)
{
    mapped_reg_write16(scif_base + reg, var);
}

INLINE u16_t
scif_read(word_t reg)
{
    return mapped_reg_read16(scif_base + reg);
}

INLINE void
scif_write8(word_t reg, u8_t var)
{
    mapped_reg_write8(scif_base + reg, var);
}

INLINE u8_t
scif_read8(word_t reg)
{
    return mapped_reg_read8(scif_base + reg);
}

/**
 * Writes a byte of data to serial line.
 */
void
soc_console_putc(char c)
{
    u16_t   reg;

    do {
        /* Read the status */
        reg = scif_read(SCIF_SCFSR);
    } while (!(reg & SCIF_SCFSR_TDFE));

    scif_write8(SCIF_SCFTDR, c);

    reg &= ~(SCIF_SCFSR_TDFE | SCIF_SCFSR_TEND);
    scif_write(SCIF_SCFSR, reg);

    do {
        reg = scif_read(SCIF_SCFSR);
    } while (!(reg & SCIF_SCFSR_TEND));

    /*
    reg = scif_read(SCIF_SCSCR);
    reg &= ~SCIF_SCSCR_TE;
    scif_write(SCIF_SCSCR, reg);
    */

    if (c == '\n') {
        soc_console_putc('\r');
    }
}

/**
 * Reads a byte of data from serial line.
 */
int
soc_console_getc(bool can_block)
{
    u16_t   reg;
    int     c;

    reg = scif_read(SCIF_SCFSR);

    /* Check ER, DR, BRK, ORER -> Clear them */
    if (reg & SCIF_SCFSR_ER ||
        reg & SCIF_SCFSR_DR ||
        reg & SCIF_SCFSR_BRK) {
        //TODO: Error handling
        reg &= ~(SCIF_SCFSR_ER | SCIF_SCFSR_DR | SCIF_SCFSR_BRK);
        scif_write(SCIF_SCFSR, reg);
        return -1;
    }

    reg = scif_read(SCIF_SCLSR);
    if (reg & SCIF_SCLSR_ORER) {
        //TODO: Error handing
        reg &= ~SCIF_SCLSR_ORER;
        scif_write(SCIF_SCLSR, reg);
        return -1;
    }

    /* Check if the buffer exceeds the limit */
    reg = scif_read(SCIF_SCFSR);
    if (!(reg & SCIF_SCFSR_RDF)) {
        return -1;
    }

    /* Read it in */
    c = (int)scif_read8(SCIF_SCFRDR);
    /* Clear RDF */
    reg &= ~SCIF_SCFSR_RDF;
    scif_write(SCIF_SCFSR, reg);

    /*
    reg = scif_read(SCIF_SCSCR);
    reg &= ~SCIF_SCSCR_RE;
    scif_write(SCIF_SCSCR, reg);
    */

    return c;
}

static void
soc_serial_init(word_t base)
{
    scif_base = base;

    scif_write(SCIF_SCSCR, 0);
    scif_write(SCIF_SCFCR, SCIF_SCFCR_TFCL | SCIF_SCFCR_RFCL);
    scif_write(SCIF_SCSMR, SCIF_SCSMR_PCK);
    scif_write8(SCIF_SCBRR, SCIF_BITRATE_115200);

    // Wait for one clock

    scif_write(SCIF_SCFCR, SCIF_SCFCR_RTRG1 | SCIF_SCFCR_TTRG32);
    scif_write(SCIF_SCSCR, SCIF_SCSCR_TE | SCIF_SCSCR_RE);
}

void
soc_kdb_init()
{
    soc_serial_init(SCIF0_BASE);
}

