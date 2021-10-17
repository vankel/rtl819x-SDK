/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1992 Ross Biro
 * Copyright (C) Linus Torvalds
 * Copyright (C) 1994, 95, 96, 97, 98, 2000 Ralf Baechle
 * Copyright (C) 1996 David S. Miller
 * Kevin D. Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999 MIPS Technologies, Inc.
 * Copyright (C) 2000 Ulf Carlsson
 *
 * At this time Linux/MIPS64 only supports syscall tracing, even for 32-bit
 * binaries.
 */
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <linux/smp.h>
#include <linux/user.h>
#include <linux/security.h>
#include <linux/audit.h>
#include <linux/seccomp.h>

#include <asm/byteorder.h>
#include <asm/cpu.h>
#include <asm/rlxregs.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/bootinfo.h>
#include <asm/reg.h>

#ifdef CONFIG_CPU_HAS_WATCH
/* WMPU APIs for kernel space Watchpoint/Memory Protection. */

struct wmpu_reg_info wmpu_info;

static int ptrace_wmpu_reg_alloc(int *reg_idx)
{
	unsigned long wmpctl;
	unsigned long mask=0x10;
	int num;
	struct cpuinfo_mips *c = &current_cpu_data;

	*reg_idx = -1;
	wmpctl = read_lxc0_wmpctl();
	wmpctl =  wmpctl >> 16;

	/* Find a disable entry in the part of registers used for kernel space
	 * and return register index. */
	for (num = c->watch_reg_use_cnt; num < c->watch_reg_count; num++) {
		if((wmpctl & mask) == 0x0){
			*reg_idx = num;
			break;
		}
		mask = mask << 1;
	}

	if (*reg_idx < 0)
		return -EIO;
	else
		return 0;
}

static int ptrace_wmpu_mask(struct wmpu_addr *addr, int *wmpuhi, int *extra_mask)
{
	unsigned int wmpu_boundary = addr->end;
	unsigned int mask_bit=0x1;
	int size;
        int shift=1;

	if (addr->start > addr->end)
                return -EIO;

        size = ((int) (addr->end - addr->start));
        size = size >> 3;

        /* Set shift = 0 if size < 8 bytes */
        if (mask_bit > size)
                shift=0;
        else {
                while(mask_bit <= size) {
                        mask_bit = mask_bit << 1;
                        shift++;
                }
        }

	/* Make sure that wmpu can cover all the range */
        wmpu_boundary = wmpu_boundary | (mask_bit -1);
        if (wmpu_boundary < addr->end)
                shift++;

        if (shift <= 9)
                *wmpuhi = *wmpuhi | (mask_bit - 1);
        else {
                *wmpuhi = 0xff8;
                *extra_mask |= ((mask_bit << 3) - 1);
        }

	return 0;
}

static int ptrace_wmpu_set(struct wmpu_addr *addr, unsigned char mode)
{
	unsigned int wmpuhi = 0x0;
	unsigned int extra_mask = 0x0;
	unsigned int wmpctl=0x0;
	int err,reg_idx;

	err = ptrace_wmpu_mask(addr, &wmpuhi, &extra_mask);
	if (err) {
		printk("set wmpu mask error!\n");
		BUG();
	}

	err = ptrace_wmpu_reg_alloc(&reg_idx);
	if (err) {
		printk("wmpu register allocation error!\n");
                BUG();
	}

	wmpu_info.watchhi[reg_idx] = wmpuhi << 3;
	wmpu_info.watchlo[reg_idx] = (addr->start & 0xfffffff8) | addr->attr;
	wmpu_info.wmpxmask[reg_idx] = extra_mask;

	/* Set lo, hi, and extra_mask register */
	switch (reg_idx) {
	case 2:
		write_c0_watchlo2(wmpu_info.watchlo[2]);
		write_c0_watchhi2(0x40000000 | wmpu_info.watchhi[2]);
		write_lxc0_wmpxmask2(extra_mask);
		break;
	case 3:
		write_c0_watchlo3(wmpu_info.watchlo[3]);
		write_c0_watchhi3(0x40000000 | wmpu_info.watchhi[3]);
		write_lxc0_wmpxmask3(extra_mask);
		break;
	case 4:
		write_c0_watchlo4(wmpu_info.watchlo[4]);
		write_c0_watchhi4(0x40000000 | wmpu_info.watchhi[4]);
		write_lxc0_wmpxmask4(extra_mask);
		break;
	case 5:
		write_c0_watchlo5(wmpu_info.watchlo[5]);
		write_c0_watchhi5(0x40000000 | wmpu_info.watchhi[5]);
		write_lxc0_wmpxmask5(extra_mask);
		break;
	case 6:
		write_c0_watchlo6(wmpu_info.watchlo[6]);
		write_c0_watchhi6(0x40000000 | wmpu_info.watchhi[6]);
		write_lxc0_wmpxmask6(extra_mask);
		break;
	case 7:
		write_c0_watchlo7(wmpu_info.watchlo[7]);
		write_c0_watchhi7(0x40000000 | wmpu_info.watchhi[7]);
		write_lxc0_wmpxmask7(extra_mask);
		break;
	default:
		BUG();
		break;
	}

	/* Set wmpu ctrl register */
	wmpctl |= (0x1 << (reg_idx + 16)) | 0x2 | mode;
	set_lxc0_wmpctl(wmpctl);

	return reg_idx;
}

int ptrace_wmpu_wp(struct wmpu_addr *addr)
{
	return  ptrace_wmpu_set(addr, MODE_WP);
}

int ptrace_wmpu_mp(struct wmpu_addr *addr)
{
	return ptrace_wmpu_set(addr, MODE_MP);
}

void ptrace_wmpu_clear(int reg_idx)
{

	switch (reg_idx) {
	case 2:
		write_c0_watchlo2(0);
		write_c0_watchhi2(0);
		write_lxc0_wmpxmask2(0);
		clear_lxc0_wmpctl(WMPCTLF_EE2);
		break;
	case 3:
		write_c0_watchlo3(0);
		write_c0_watchhi3(0);
		write_lxc0_wmpxmask3(0);
		clear_lxc0_wmpctl(WMPCTLF_EE3);
		break;
	case 4:
		write_c0_watchlo4(0);
		write_c0_watchhi4(0);
		write_lxc0_wmpxmask4(0);
		clear_lxc0_wmpctl(WMPCTLF_EE4);
		break;
	case 5:
		write_c0_watchlo5(0);
		write_c0_watchhi5(0);
		write_lxc0_wmpxmask5(0);
		clear_lxc0_wmpctl(WMPCTLF_EE5);
		break;
	case 6:
		write_c0_watchlo6(0);
		write_c0_watchhi6(0);
		write_lxc0_wmpxmask6(0);
		clear_lxc0_wmpctl(WMPCTLF_EE6);
		break;
	case 7:
		write_c0_watchlo7(0);
		write_c0_watchhi7(0);
		write_lxc0_wmpxmask7(0);
		clear_lxc0_wmpctl(WMPCTLF_EE7);
		break;
	default:
		printk("wmpu clear fail!\n");
		BUG();
		break;
	}
}

#endif

/*
 * Called by kernel/ptrace.c when detaching..
 *
 * Make sure single step bits etc are not set.
 */
void ptrace_disable(struct task_struct *child)
{
	#if defined(CONFIG_CPU_HAS_WATCH)
	/* Don't load the watchpoint registers for the ex-child. */
	clear_tsk_thread_flag(child, TIF_LOAD_WATCH);
	#endif
}

/*
 * Read a general register set.  We always use the 64-bit format, even
 * for 32-bit kernels and for 32-bit processes on a 64-bit kernel.
 * Registers are sign extended to fill the available space.
 */
int ptrace_getregs(struct task_struct *child, __s64 __user *data)
{
	struct pt_regs *regs;
	int i;

	if (!access_ok(VERIFY_WRITE, data, 38 * 8))
		return -EIO;

	regs = task_pt_regs(child);

	for (i = 0; i < 32; i++)
		__put_user((long)regs->regs[i], data + i);
	__put_user((long)regs->lo, data + EF_LO - EF_R0);
	__put_user((long)regs->hi, data + EF_HI - EF_R0);
	__put_user((long)regs->cp0_epc, data + EF_CP0_EPC - EF_R0);
	__put_user((long)regs->cp0_badvaddr, data + EF_CP0_BADVADDR - EF_R0);
	__put_user((long)regs->cp0_status, data + EF_CP0_STATUS - EF_R0);
	__put_user((long)regs->cp0_cause, data + EF_CP0_CAUSE - EF_R0);

	return 0;
}

/*
 * Write a general register set.  As for PTRACE_GETREGS, we always use
 * the 64-bit format.  On a 32-bit kernel only the lower order half
 * (according to endianness) will be used.
 */
int ptrace_setregs(struct task_struct *child, __s64 __user *data)
{
	struct pt_regs *regs;
	int i;

	if (!access_ok(VERIFY_READ, data, 38 * 8))
		return -EIO;

	regs = task_pt_regs(child);

	for (i = 0; i < 32; i++)
		__get_user(regs->regs[i], data + i);
	__get_user(regs->lo, data + EF_LO - EF_R0);
	__get_user(regs->hi, data + EF_HI - EF_R0);
	__get_user(regs->cp0_epc, data + EF_CP0_EPC - EF_R0);

	/* badvaddr, status, and cause may not be written.  */

	return 0;
}
#if defined(CONFIG_CPU_HAS_WATCH)
int ptrace_get_watch_regs(struct task_struct *child,
			  struct pt_watch_regs __user *addr)
{
	enum pt_watch_style style;
	int i;

	if (!cpu_has_watch || current_cpu_data.watch_reg_use_cnt == 0)
		return -EIO;
	if (!access_ok(VERIFY_WRITE, addr, sizeof(struct pt_watch_regs)))
		return -EIO;

#ifdef CONFIG_32BIT
	style = pt_watch_style_rlx32;
#define WATCH_STYLE rlx32
#else
	style = pt_watch_style_rlx64;
#define WATCH_STYLE rlx64
#endif

	__put_user(style, &addr->style);
	__put_user(current_cpu_data.watch_reg_use_cnt,
		   &addr->WATCH_STYLE.num_valid);
	for (i = 0; i < current_cpu_data.watch_reg_use_cnt; i++) {
		__put_user(child->thread.watch.rlx3264.watchlo[i],
			   &addr->WATCH_STYLE.watchlo[i]);
		__put_user(child->thread.watch.rlx3264.watchhi[i] & 0xfff,
			   &addr->WATCH_STYLE.watchhi[i]);
		__put_user(current_cpu_data.watch_reg_masks[i],
			   &addr->WATCH_STYLE.watch_masks[i]);
	}
	for (; i < 8; i++) {
		__put_user(0, &addr->WATCH_STYLE.watchlo[i]);
		__put_user(0, &addr->WATCH_STYLE.watchhi[i]);
		__put_user(0, &addr->WATCH_STYLE.watch_masks[i]);
	}
	return 0;
}


int ptrace_set_watch_regs(struct task_struct *child,
			  struct pt_watch_regs __user *addr)
{
	int i;
	int watch_active = 0;
	unsigned long lt[NUM_WATCH_REGS];
	u16 ht[NUM_WATCH_REGS];

	if (!cpu_has_watch || current_cpu_data.watch_reg_use_cnt == 0)
		return -EIO;
	if (!access_ok(VERIFY_READ, addr, sizeof(struct pt_watch_regs)))
		return -EIO;
	/* Check the values. */
	for (i = 0; i < current_cpu_data.watch_reg_use_cnt; i++) {
		__get_user(lt[i], &addr->WATCH_STYLE.watchlo[i]);
#ifdef CONFIG_32BIT
		if (lt[i] & __UA_LIMIT)
			return -EINVAL;
#else
		if (test_tsk_thread_flag(child, TIF_32BIT_ADDR)) {
			if (lt[i] & 0xffffffff80000000UL)
				return -EINVAL;
		} else {
			if (lt[i] & __UA_LIMIT)
				return -EINVAL;
		}
#endif
		__get_user(ht[i], &addr->WATCH_STYLE.watchhi[i]);
		if (ht[i] & ~0xff8)
			return -EINVAL;
	}
	/* Install them. */
	for (i = 0; i < current_cpu_data.watch_reg_use_cnt; i++) {
		if (lt[i] & 7)
			watch_active = 1;
		child->thread.watch.rlx3264.watchlo[i] = lt[i];
		/* Set the G bit. */
		child->thread.watch.rlx3264.watchhi[i] = ht[i];
	}


	if (watch_active)
		set_tsk_thread_flag(child, TIF_LOAD_WATCH);
	else
		clear_tsk_thread_flag(child, TIF_LOAD_WATCH);
	return 0;
}

#endif/*CONFIG_CPU_HAS_WATCH*/

long arch_ptrace(struct task_struct *child, long request, long addr, long data)
{
	int ret;

	switch (request) {
	/* when I and D space are separate, these will need to be fixed. */
	case PTRACE_PEEKTEXT: /* read word at location addr. */
	case PTRACE_PEEKDATA:
		ret = generic_ptrace_peekdata(child, addr, data);
		break;

	/* Read the word at location addr in the USER area. */
	case PTRACE_PEEKUSR: {
		struct pt_regs *regs;
		unsigned long tmp = 0;

		regs = task_pt_regs(child);
		ret = 0;  /* Default return value. */

		switch (addr) {
		case 0 ... 31:
			tmp = regs->regs[addr];
			break;
		case PC:
			tmp = regs->cp0_epc;
			break;
		case CAUSE:
			tmp = regs->cp0_cause;
			break;
		case BADVADDR:
			tmp = regs->cp0_badvaddr;
			break;
		case MMHI:
			tmp = regs->hi;
			break;
		case MMLO:
			tmp = regs->lo;
			break;
		default:
			tmp = 0;
			ret = -EIO;
			goto out;
		}
		ret = put_user(tmp, (unsigned long __user *) data);
		break;
	}

	/* when I and D space are separate, this will have to be fixed. */
	case PTRACE_POKETEXT: /* write the word at location addr. */
	case PTRACE_POKEDATA:
		ret = generic_ptrace_pokedata(child, addr, data);
		break;

	case PTRACE_POKEUSR: {
		struct pt_regs *regs;
		ret = 0;
		regs = task_pt_regs(child);

		switch (addr) {
		case 0 ... 31:
			regs->regs[addr] = data;
			break;
		case PC:
			regs->cp0_epc = data;
			break;
		case MMHI:
			regs->hi = data;
			break;
		case MMLO:
			regs->lo = data;
			break;
		default:
			/* The rest are not allowed. */
			ret = -EIO;
			break;
		}
		break;
		}

	case PTRACE_GETREGS:
		ret = ptrace_getregs(child, (__s64 __user *) data);
		break;

	case PTRACE_SETREGS:
		ret = ptrace_setregs(child, (__s64 __user *) data);
		break;

	case PTRACE_SYSCALL: /* continue and stop at next (return from) syscall */
	case PTRACE_CONT: { /* restart after signal. */
		ret = -EIO;
		if (!valid_signal(data))
			break;
		if (request == PTRACE_SYSCALL) {
			set_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
		}
		else {
			clear_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
		}
		child->exit_code = data;
		wake_up_process(child);
		ret = 0;
		break;
	}

	/*
	 * make the child exit.  Best I can do is send it a sigkill.
	 * perhaps it should be put in the status that it wants to
	 * exit.
	 */
	case PTRACE_KILL:
		ret = 0;
		if (child->exit_state == EXIT_ZOMBIE)	/* already dead */
			break;
		child->exit_code = SIGKILL;
		wake_up_process(child);
		break;

	case PTRACE_GET_THREAD_AREA:
		ret = put_user(task_thread_info(child)->tp_value,
				(unsigned long __user *) data);
		break;
#if defined(CONFIG_CPU_HAS_WATCH)
	case PTRACE_GET_WATCH_REGS:
		ret = ptrace_get_watch_regs(child,
					(struct pt_watch_regs __user *) addr);
		break;

	case PTRACE_SET_WATCH_REGS:
                 /*return error if wmpu is in memory protection mode */
                 if(read_lxc0_wmpctl() & 0x1)
                 {
                      printk("WMPU run in memory protection mode. Fail to set watch registers!\n");
                      return -EIO;
                 }
		ret = ptrace_set_watch_regs(child,
					(struct pt_watch_regs __user *) addr);
		break;
#endif
	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}
 out:
	return ret;
}

static inline int audit_arch(void)
{
	int arch = EM_MIPS;
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	arch |=  __AUDIT_ARCH_LE;
#endif
	return arch;
}

/*
 * Notification of system call entry/exit
 * - triggered by current->work.syscall_trace
 */
asmlinkage void do_syscall_trace(struct pt_regs *regs, int entryexit)
{
	/* do the secure computing check first */
	if (!entryexit)
		secure_computing(regs->regs[0]);

	if (unlikely(current->audit_context) && entryexit)
		audit_syscall_exit(AUDITSC_RESULT(regs->regs[2]),
		                   regs->regs[2]);

	if (!(current->ptrace & PT_PTRACED))
		goto out;

	if (!test_thread_flag(TIF_SYSCALL_TRACE))
		goto out;

	/* The 0x80 provides a way for the tracing parent to distinguish
	   between a syscall stop and SIGTRAP delivery */
	ptrace_notify(SIGTRAP | ((current->ptrace & PT_TRACESYSGOOD) ?
	                         0x80 : 0));

	/*
	 * this isn't the same as continuing with a signal, but it will do
	 * for normal use.  strace only continues with a signal if the
	 * stopping signal is not SIGTRAP.  -brl
	 */
	if (current->exit_code) {
		send_sig(current->exit_code, current, 1);
		current->exit_code = 0;
	}

out:
	if (unlikely(current->audit_context) && !entryexit)
		audit_syscall_entry(audit_arch(), regs->regs[0],
				    regs->regs[4], regs->regs[5],
				    regs->regs[6], regs->regs[7]);
}
