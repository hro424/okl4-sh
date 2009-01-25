/* $Id$ */

/**
 * @since   December 2008
 */

#include <stdarg.h>
#include <debug.h>

int do_printf(const char* format_p, va_list args);

extern "C" void
kernel_enter_kdebug(const char* s)
{
    enter_kdebug(s);
}

extern "C" void
kernel_fatal_error(const char* s)
{
#if defined(CONFIG_KDB_CONS)
    enter_kdebug(s);
#endif
    panic(s);
}

#if defined(CONFIG_KDB_CONS)
extern "C" int
kernel_printf(const char* format, ...)
{
    va_list args;
    int     res;

    va_start(args, format);
    res = do_printf(format, args);
    va_end(args);

    return res;
}
#endif


