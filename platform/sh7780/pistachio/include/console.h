/**
 * @since   February 2009
 */

#ifndef OKL4_PLATFORM_SH7780_CONSOLE_H
#define OKL4_PLATFORM_SH7780_CONSOLE_H

#define SCIF0_BASE              0xFFE00000
#define SCIF1_BASE              0xFFE10000

typedef enum {
    SCIF_SCSMR =                0x00,
    SCIF_SCBRR =                0x04,
    SCIF_SCSCR =                0x08,
    SCIF_SCFTDR =               0x0C,
    SCIF_SCFSR =                0x10,
    SCIF_SCFRDR =               0x14,
    SCIF_SCFCR =                0x18,
    SCIF_SCTFDR =               0x1C,
    SCIF_SCRFDR =               0x20,
    SCIF_SCSPTR =               0x24,
    SCIF_SCLSR =                0x28,
    SCIF_SCRER =                0x2C,
} scif_off_e;

typedef enum {
    SCIF_SCSMR_CA =             0x80,
    SCIF_SCSMR_CHR =            0x40,
    SCIF_SCSMR_PE =             0x20,
    SCIF_SCSMR_OE =             0x10,
    SCIF_SCSMR_STOP =           0x08,
    SCIF_SCSMR_PCK =            0x00,
    SCIF_SCSMR_PCK4 =           0x01,
    SCIF_SCSMR_PCK16 =          0x02,
    SCIF_SCSMR_PCK64 =          0x03
} scif_scsmr_e;

typedef enum {
    SCIF_SCSCR_TIE =            0x80,
    SCIF_SCSCR_RIE =            0x40,
    SCIF_SCSCR_TE =             0x20,
    SCIF_SCSCR_RE =             0x10,
    SCIF_SCSCR_REIE =           0x08,
    SCIF_SCSCR_CKE1 =           0x02,
    SCIF_SCSCR_CKE0 =           0x01
} scif_scscr_e;

typedef enum {
    SCIF_SCFSR_ER =             0x80,
    SCIF_SCFSR_TEND =           0x40,
    SCIF_SCFSR_TDFE =           0x20,
    SCIF_SCFSR_BRK =            0x10,
    SCIF_SCFSR_FER =            0x08,
    SCIF_SCFSR_PER =            0x04,
    SCIF_SCFSR_RDF =            0x02,
    SCIF_SCFSR_DR =             0x01
} scif_scfsr_e;

typedef enum {
    SCIF_SCFCR_RTRG1 =          0x00,
    SCIF_SCFCR_RTRG16 =         0x40,
    SCIF_SCFCR_RTRG32 =         0x80,
    SCIF_SCFCR_RTRG48 =         0xC0,
    SCIF_SCFCR_TTRG32 =         0x00,
    SCIF_SCFCR_TTRG16 =         0x10,
    SCIF_SCFCR_TTRG2 =          0x20,
    SCIF_SCFCR_TTRG1 =          0x30,
    SCIF_SCFCR_MCE =            0x08,
    SCIF_SCFCR_TFCL =           0x04,
    SCIF_SCFCR_RFCL =           0x02,
    SCIF_SCFCR_LOOP =           0x01
} scif_scfcr_e;

typedef enum {
    SCIF_SCLSR_ORER =             0x01
} scif_sclsr_e;

/*
 * SH7780:
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

#endif /* OKL4_PLATFORM_SH7780_CONSOLE_H */

