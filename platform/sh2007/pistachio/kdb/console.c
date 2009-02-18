/* $Id$ */

/**
 * @since   February 2009
 * @see     SH hardware manual
 */

#include <console.h>
#include <soc/soc.h>

#define SCIF0_BASE          0xFFE0000
#define SCIF1_BASE          0xFFE1000

#define SCIF_BITRATE        0x04
#define SCIF_CONTROL        0x08
#define SCIF_SEND_FIFO      0x0C
#define SCIF_STATUS         0x10
#define SCIF_RECV_FIFO      0x14
#define SCIF_FIFO_CTRL      0x18
#define SCIF_SEND_NUM       0x1C
#define SCIF_RECV_NUM       0x20
#define SCIF_PORT           0x24
#define SCIF_LINE_STAT      0x28
#define SCIF_ERROR          0x2C


//static word_t   scif_base;

void
soc_console_putc(char c)
{
    //mapped_reg_write(scif_base + SCIF_SEND_FIFO, c);

    if (c == '\n') {
        soc_console_putc('\r');
    }
}

int
soc_console_getc(bool can_block)
{
    //char    c;

    //c = mapped_reg_read(scif_base + SCIF_RECV_FIFO);
    return 0;
}

static void
soc_serial_init(word_t base)
{
    //mapped_reg_write(base, 0);
}

void
soc_kdb_init()
{
    soc_serial_init(SCIF0_BASE);
}
