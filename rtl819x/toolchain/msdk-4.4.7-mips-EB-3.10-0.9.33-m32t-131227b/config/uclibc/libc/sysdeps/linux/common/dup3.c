/* vi: set sw=4 ts=4: */
/*
 * dup3() for uClibc
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>
#include <unistd.h>

#if defined(__NR_dup3)
_syscall3(int, dup3, int, oldfd, int, newfd, int, flags)
#endif
