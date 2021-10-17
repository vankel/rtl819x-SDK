/* Suspend until termination of a requests.  Stub version.
   Copyright (C) 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */


/* We use an UGLY hack to prevent gcc from finding us cheating.  The
   implementations of aio_suspend and aio_suspend64 are identical and so
   we want to avoid code duplication by using aliases.  But gcc sees
   the different parameter lists and prints a warning.  We define here
   a function so that aio_suspend64 has no prototype.  */
#define aio_suspend64 XXX
#include <aio.h>
/* And undo the hack.  */
#undef aio_suspend64

#include <errno.h>
#include <sys/time.h>

#include <sys/syscall.h>
#include <limits.h>
#include <atomic.h>

/* Due to the requirement that aio_suspend be async-signal-safe, we cannot
 * use any locks, wait queues, etc. that would make it more efficient. The
 * only obviously-correct algorithm is to generate a wakeup every time any
 * aio operation finishes and have aio_suspend re-evaluate the completion
 * status of each aiocb it was waiting on. */

static volatile int seq;

extern long int syscall (long int __sysno, ...);

#define FUTEX_WAKE 1

static inline void __wake(volatile void *addr, int cnt, int priv)
{
	if (priv) priv = 128;
	if (cnt<0) cnt = INT_MAX;
	if (syscall(SYS_futex, addr, FUTEX_WAKE|priv, cnt) == -ENOSYS)
		syscall(SYS_futex, addr, FUTEX_WAKE, cnt);
}

int __timedwait(volatile int *, int,
	clockid_t, const struct timespec *,
	void (*cleanup)(void *), void *, int);

void __aio_wake(void)
{
	a_inc(&seq);
	__wake(&seq, -1, 1);
}

int aio_suspend(const struct aiocb *const cbs[], int cnt, const struct timespec *ts)
{
	int i, last, first=1, ret=0;
	struct timespec at;

	if (cnt<0) {
		errno = EINVAL;
		return -1;
	}

	for (;;) {
		last = seq;

		for (i=0; i<cnt; i++) {
			if (cbs[i] && cbs[i]->__err != EINPROGRESS)
				return 0;
		}

		if (first && ts) {
			clock_gettime(CLOCK_MONOTONIC, &at);
			at.tv_sec += ts->tv_sec;
			if ((at.tv_nsec += ts->tv_nsec) >= 1000000000) {
				at.tv_nsec -= 1000000000;
				at.tv_sec++;
			}
			first = 0;
		}

		ret = __timedwait(&seq, last, CLOCK_MONOTONIC,
			ts ? &at : 0, 0, 0, 1);

		if (ret == ETIMEDOUT) ret = EAGAIN;

		if (ret) {
			errno = ret;
			return -1;
		}
	}
}
weak_alias (aio_suspend, aio_suspend64)
