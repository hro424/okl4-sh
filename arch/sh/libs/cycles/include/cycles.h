#ifndef OKL4_ARCH_SH_CYCLES_CYCLES_H
#define OKL4_ARCH_SH_CYCLES_CYCLES_H

#include <inttypes.h>

#define __HAVE_SH_PPC

void setup_PPC(void);
void restart_PPC(void);
void stop_PPC(void);
uint64_t get_PPC(void);

#endif // OKL4_ARCH_SH_CYCLES_CYCLES_H
