/* $Id$ */

/**
 * @since   February 2009
 */

#ifndef OKL4_PLATFORM_SH2007_CONSOLE_H
#define OKL4_PLATFORM_SH2007_CONSOLE_H

#define SCIF0_BASE              0xFFE0000
#define SCIF1_BASE              0xFFE1000

#define SCIF_MODE               0x00
#define SCIF_BITRATE            0x04
#define SCIF_CONTROL            0x08
#define SCIF_SEND_FIFO          0x0C
#define SCIF_STATUS             0x10
#define SCIF_RECV_FIFO          0x14
#define SCIF_FIFO_CTRL          0x18
#define SCIF_SEND_NUM           0x1C
#define SCIF_RECV_NUM           0x20
#define SCIF_PORT               0x24
#define SCIF_LINE_STAT          0x28
#define SCIF_ERROR              0x2C

#define SCIF_MODE_MODE          0x80
#define SCIF_MODE_CHR           0x40
#define SCIF_MODE_PE            0x20
#define SCIF_MODE_OE            0x10
#define SCIF_MODE_STOP          0x08
#define SCIF_MODE_PCK           0x00
#define SCIF_MODE_PCK4          0x01
#define SCIF_MODE_PCK16         0x02
#define SCIF_MODE_PCK64         0x03

#define SCIF_CONTROL_TIE        0x80
#define SCIF_CONTROL_RIE        0x40
#define SCIF_CONTROL_TE         0x20
#define SCIF_CONTROL_RE         0x10
#define SCIF_CONTROL_REIE       0x08
#define SCIF_CONTROL_CLKSRC     0x02
#define SCIF_CONTROL_CLKDIR     0x01

#define SCIF_STATUS_ER          0x80
#define SCIF_STATUS_TEND        0x40
#define SCIF_STATUS_TDFE        0x20
#define SCIF_STATUS_BRK         0x10
#define SCIF_STATUS_FER         0x08
#define SCIF_STATUS_PER         0x04
#define SCIF_STATUS_RDF         0x02
#define SCIF_STATUS_DR          0x01

#define SCIF_FIFO_CTRL_RTRG1    0x00
#define SCIF_FIFO_CTRL_RTRG16   0x40
#define SCIF_FIFO_CTRL_RTRG32   0x80
#define SCIF_FIFO_CTRL_RTRG48   0xC0
#define SCIF_FIFO_CTRL_TTRG32   0x00
#define SCIF_FIFO_CTRL_TTRG16   0x10
#define SCIF_FIFO_CTRL_TTRG2    0x20
#define SCIF_FIFO_CTRL_TTRG1    0x30
#define SCIF_FIFO_CTRL_MCE      0x08
#define SCIF_FIFO_CTRL_TFCL     0x04
#define SCIF_FIFO_CTRL_RFCL     0x02
#define SCIF_FIFO_CTRL_LOOP     0x01

#define SCIF_LINE_STAT_ORER     0x01

/*
 * SH2007:
 * Clock    33.333MHz
 * Ick      400MHz
 * DDRck    160MHz
 * Bck      100MHz
 * Pck      50MHz
 */

#define SCIF_BITRATE_9600       161     /* n: 0, Error rate: 0.47 */
#define SCIF_BITRATE_19200      80      /* n: 0, Error rate: 0.47 */
#define SCIF_BITRATE_38400      39      /* n: 0, Error rate: 1.72 */
#define SCIF_BITRATE_57600      26      /* n: 0, Error rate: 0.47 */
#define SCIF_BITRATE_115200     12      /* n: 0, Error rate: 4.33 */
#define SCIF_BITRATE_230400     5       /* n: 0, Error rate: 13.0 */
#define SCIF_BITRATE_460800     2       /* n: 0, Error rate: 13.0 */
#define SCIF_BITRATE_500000     2       /* n: 0, Error rate: 4.17 */
#define SCIF_BITRATE_576000     1       /* n: 0, Error rate: 35.6 */

#endif /* OKL4_PLATFORM_SH2007_CONSOLE_H */

