/*
 * Copyright (c) 2006, National ICT Australia
 */
/*
 * Copyright (c) 2007 Open Kernel Labs, Inc. (Copyright Holder).
 * All rights reserved.
 *
 * 1. Redistribution and use of OKL4 (Software) in source and binary
 * forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 *     (a) Redistributions of source code must retain this clause 1
 *         (including paragraphs (a), (b) and (c)), clause 2 and clause 3
 *         (Licence Terms) and the above copyright notice.
 *
 *     (b) Redistributions in binary form must reproduce the above
 *         copyright notice and the Licence Terms in the documentation and/or
 *         other materials provided with the distribution.
 *
 *     (c) Redistributions in any form must be accompanied by information on
 *         how to obtain complete source code for:
 *        (i) the Software; and
 *        (ii) all accompanying software that uses (or is intended to
 *        use) the Software whether directly or indirectly.  Such source
 *        code must:
 *        (iii) either be included in the distribution or be available
 *        for no more than the cost of distribution plus a nominal fee;
 *        and
 *        (iv) be licensed by each relevant holder of copyright under
 *        either the Licence Terms (with an appropriate copyright notice)
 *        or the terms of a licence which is approved by the Open Source
 *        Initative.  For an executable file, "complete source code"
 *        means the source code for all modules it contains and includes
 *        associated build and other files reasonably required to produce
 *        the executable.
 *
 * 2. THIS SOFTWARE IS PROVIDED ``AS IS'' AND, TO THE EXTENT PERMITTED BY
 * LAW, ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED.  WHERE ANY WARRANTY IS
 * IMPLIED AND IS PREVENTED BY LAW FROM BEING DISCLAIMED THEN TO THE
 * EXTENT PERMISSIBLE BY LAW: (A) THE WARRANTY IS READ DOWN IN FAVOUR OF
 * THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
 * PARTICIPANT) AND (B) ANY LIMITATIONS PERMITTED BY LAW (INCLUDING AS TO
 * THE EXTENT OF THE WARRANTY AND THE REMEDIES AVAILABLE IN THE EVENT OF
 * BREACH) ARE DEEMED PART OF THIS LICENCE IN A FORM MOST FAVOURABLE TO
 * THE COPYRIGHT HOLDER (AND, IN THE CASE OF A PARTICIPANT, THAT
 * PARTICIPANT). IN THE LICENCE TERMS, "PARTICIPANT" INCLUDES EVERY
 * PERSON WHO HAS CONTRIBUTED TO THE SOFTWARE OR WHO HAS BEEN INVOLVED IN
 * THE DISTRIBUTION OR DISSEMINATION OF THE SOFTWARE.
 *
 * 3. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ANY OTHER PARTICIPANT BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <okl4/env.h>

#if defined SERIAL_DRIVER
#include <serial/serial.h>
#endif /* SERIAL_DRIVER */

#include "../threadsafety.h"
#include "../stream.h"

extern struct __file __stdin;
extern struct __file __stdout;
extern struct __file __stderr;

static size_t
null_write(const void *data, long int position, size_t count, void *handle)
{
    return count;
}

static size_t
null_read(void *data, long int position, size_t count, void *handle)
{
    return 0;
}

static void
null_init(void)
{
    __stdin.read_fn = null_read;
    __stdout.write_fn = null_write;
    __stderr.write_fn = null_write;
}

#if defined SERIAL_DRIVER
static void
serial_stdio_init(void)
{
    __stdin.read_fn = serial_read;
    __stdout.write_fn = serial_write;
    __stderr.write_fn = serial_write;
}
#endif

static size_t
init_write(const void *data, long int position, size_t count, void *handle)
{
    /* If serial initialisation failed, fall back to /dev/null serial. */
#if defined SERIAL_DRIVER
    if (serial_init() == 0) {
        serial_stdio_init();
    } else {
#else
    if (1) {
#endif
        null_init();
    }

    /* Call the write function and return its result */
    return __stdout.write_fn(data, position, count, handle);
}

static size_t
init_read(void *data, long int position, size_t count, void *handle)
{
    /* If serial initialisation failed, fall back to /dev/null serial. */
#if defined SERIAL_DRIVER
    if (serial_init() == 0) {
        serial_stdio_init();
    } else {
#else
    if (1) {
#endif
        null_init();
    }

    /* Call the read function and return its result */
    return __stdin.read_fn(data, position, count, handle);
}

struct __file __stdin = {
    NULL,
    init_read,
    NULL,
    NULL,
    NULL,
    _IONBF,
    NULL,
    0,
    0
};

struct __file __stdout = {
    NULL,
    NULL,
    init_write,
    NULL,
    NULL,
    _IONBF,
    NULL,
    0,
    0
};

struct __file __stderr = {
    NULL,
    NULL,
    init_write,
    NULL,
    NULL,
    _IONBF,
    NULL,
    0,
    0
};

FILE *stdin = &__stdin;
FILE *stdout = &__stdout;
FILE *stderr = &__stderr;
