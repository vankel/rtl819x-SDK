/* Copyright (C) 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#if __ARM_ARCH < 6

#include "pthreadP.h"

int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  __asm__ ("mov	r1, #1\n"
       "1:\n\t"
       "swp	r2, r1, [%0]\n\t"
       "teq	r2, #0\n\t"
       "bne	1b\n\t"
       : /* no output constraint.  */
       : "r" (lock)
       : "r1", "r2", "memory"
      );
  return 0;
}

#else

#include "pthreadP.h"

int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  while (atomic_compare_and_exchange_val_acq (lock, 1, 0) != 0)
    while (*lock != 0)
      ;

  return 0;
}
#endif
