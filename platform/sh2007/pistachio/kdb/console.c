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
        reg = scif_read(SCIF_STATUS);
    } while (!(reg & SCIF_STATUS_TDFE));

    scif_write8(SCIF_SEND_FIFO, c);

    reg &= ~(SCIF_STATUS_TDFE | SCIF_STATUS_TEND);
    scif_write(SCIF_STATUS, reg);

    do {
        reg = scif_read(SCIF_STATUS);
    } while (!(reg & SCIF_STATUS_TEND));

    reg = scif_read(SCIF_CONTROL);
    reg &= ~SCIF_CONTROL_TE;
    scif_write(SCIF_CONTROL, reg);

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

    reg = scif_read(SCIF_STATUS);

    /* Check ER, DR, BRK, ORER -> Clear them */
    if (reg & SCIF_STATUS_ER ||
        reg & SCIF_STATUS_DR ||
        reg & SCIF_STATUS_BRK) {
        //TODO: Error handling
        reg &= ~(SCIF_STATUS_ER | SCIF_STATUS_DR | SCIF_STATUS_BRK);
        scif_write(SCIF_STATUS, reg);
        return -1;
    }

    reg = scif_read(SCIF_LINE_STAT);
    if (reg & SCIF_LINE_STAT_ORER) {
        //TODO: Error handing
        reg &= ~SCIF_LINE_STAT_ORER;
        scif_write(SCIF_LINE_STAT, reg);
        return -1;
    }

    /* Check if the buffer exceeds the limit */
    reg = scif_read(SCIF_STATUS);
    if (!(reg & SCIF_STATUS_RDF)) {
        return -1;
    }

    /* Read it in */
    c = (int)scif_read8(SCIF_RECV_FIFO);
    /* Clear RDF */
    reg &= ~SCIF_STATUS_RDF;
    scif_write(SCIF_STATUS, reg);

    reg = scif_read(SCIF_CONTROL);
    reg &= ~SCIF_CONTROL_RE;
    scif_write(SCIF_CONTROL, reg);

    return c;
}

static void
soc_serial_init(word_t base)
{
    scif_base = base;

    scif_write(SCIF_CONTROL, 0);
    scif_write(SCIF_FIFO_CTRL, SCIF_FIFO_CTRL_TFCL | SCIF_FIFO_CTRL_RFCL);
    scif_write(SCIF_MODE, SCIF_MODE_PCK);
    scif_write8(SCIF_BITRATE, SCIF_BITRATE_57600);

    // Wait for one clock

    scif_write(SCIF_FIFO_CTRL, SCIF_FIFO_CTRL_RTRG1 | SCIF_FIFO_CTRL_TTRG32);
    scif_write(SCIF_CONTROL, SCIF_CONTROL_TE | SCIF_CONTROL_RE);
}

void
soc_kdb_init()
{
    soc_serial_init(SCIF0_BASE);
}

