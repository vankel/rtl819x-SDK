	.file	1 "rtl_nf_connGC.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.conntrack_dointvec,"ax",@progbits
	.align	2
	.globl	conntrack_dointvec
	.set	nomips16
	.ent	conntrack_dointvec
	.type	conntrack_dointvec, @function
conntrack_dointvec:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 24, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$17,32($sp)
	sw	$16,28($sp)
	lw	$2,56($sp)
	move	$16,$5
	jal	proc_dointvec
	sw	$2,16($sp)

	bne	$2,$0,$L2
	move	$17,$2

	beq	$16,$0,$L2
	lui	$2,%hi(nf_conntrack_max)

	lw	$2,%lo(nf_conntrack_max)($2)
	lui	$4,%hi(prot_limit)
	lui	$7,%hi(_prot_limit)
	addiu	$4,$4,%lo(prot_limit)
	move	$3,$0
	addiu	$7,$7,%lo(_prot_limit)
	li	$5,100			# 0x64
	li	$6,3			# 0x3
$L3:
	lw	$9,0($4)
	sll	$8,$3,2
	addu	$8,$7,$8
	mult	$2,$9
	addiu	$3,$3,1
	addiu	$4,$4,4
	mflo	$9
	divu	$0,$9,$5
	mflo	$10
	bne	$3,$6,$L3
	sw	$10,0($8)

	sll	$5,$2,2
	li	$3,5			# 0x5
	divu	$0,$5,$3
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	mflo	$5
	sw	$5,%lo(rtl_nf_conntrack_threshold)($4)
	subu	$5,$2,$5
	sltu	$5,$5,65
	bne	$5,$0,$L4
	addiu	$3,$2,-64

	sw	$3,%lo(rtl_nf_conntrack_threshold)($4)
$L4:
	beq	$2,$0,$L5
	lui	$16,%hi(rtl_newGC_session_status_flags)

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L5
	li	$2,3			# 0x3

	lw	$3,%lo(rtl_newGC_session_status_flags)($16)
	nop
	beq	$3,$2,$L2
	nop

	sw	$2,%lo(rtl_newGC_session_status_flags)($16)
	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$3,1
	j	$L2
	sw	$3,%lo(rtl_newGC_session_status_time)($2)

$L5:
	sw	$0,%lo(rtl_newGC_session_status_flags)($16)
$L2:
	move	$2,$17
	lw	$31,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	conntrack_dointvec
	.size	conntrack_dointvec, .-conntrack_dointvec
	.section	.text.conntrack_dointvec_minmax,"ax",@progbits
	.align	2
	.globl	conntrack_dointvec_minmax
	.set	nomips16
	.ent	conntrack_dointvec_minmax
	.type	conntrack_dointvec_minmax, @function
conntrack_dointvec_minmax:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 24, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$17,32($sp)
	sw	$16,28($sp)
	lw	$2,56($sp)
	move	$16,$5
	jal	proc_dointvec_minmax
	sw	$2,16($sp)

	bne	$2,$0,$L8
	move	$17,$2

	beq	$16,$0,$L8
	lui	$2,%hi(nf_conntrack_max)

	lw	$2,%lo(nf_conntrack_max)($2)
	lui	$4,%hi(prot_limit)
	lui	$7,%hi(_prot_limit)
	addiu	$4,$4,%lo(prot_limit)
	move	$3,$0
	addiu	$7,$7,%lo(_prot_limit)
	li	$5,100			# 0x64
	li	$6,3			# 0x3
$L9:
	lw	$9,0($4)
	sll	$8,$3,2
	addu	$8,$7,$8
	mult	$2,$9
	addiu	$3,$3,1
	addiu	$4,$4,4
	mflo	$9
	divu	$0,$9,$5
	mflo	$10
	bne	$3,$6,$L9
	sw	$10,0($8)

	sll	$5,$2,2
	li	$3,5			# 0x5
	divu	$0,$5,$3
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	mflo	$5
	sw	$5,%lo(rtl_nf_conntrack_threshold)($4)
	subu	$5,$2,$5
	sltu	$5,$5,65
	bne	$5,$0,$L10
	addiu	$3,$2,-64

	sw	$3,%lo(rtl_nf_conntrack_threshold)($4)
$L10:
	beq	$2,$0,$L11
	lui	$16,%hi(rtl_newGC_session_status_flags)

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L11
	li	$2,3			# 0x3

	lw	$3,%lo(rtl_newGC_session_status_flags)($16)
	nop
	beq	$3,$2,$L8
	nop

	sw	$2,%lo(rtl_newGC_session_status_flags)($16)
	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$3,1
	j	$L8
	sw	$3,%lo(rtl_newGC_session_status_time)($2)

$L11:
	sw	$0,%lo(rtl_newGC_session_status_flags)($16)
$L8:
	move	$2,$17
	lw	$31,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	conntrack_dointvec_minmax
	.size	conntrack_dointvec_minmax, .-conntrack_dointvec_minmax
	.section	.text.clean_from_lists,"ax",@progbits
	.align	2
	.globl	clean_from_lists
	.set	nomips16
	.ent	clean_from_lists
	.type	clean_from_lists, @function
clean_from_lists:
	.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$16,$4
	move	$17,$5
	jal	rtl_hlist_nulls_del_rcu
	move	$5,$0

	move	$4,$16
	jal	rtl_hlist_nulls_del_rcu
	li	$5,1			# 0x1

	move	$4,$16
	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0

	li	$3,6			# 0x6
	beq	$2,$3,$L16
	li	$3,17			# 0x11

	beq	$2,$3,$L17
	li	$3,1			# 0x1

	bne	$2,$3,$L14
	nop

	j	$L22
	nop

$L16:
	jal	arch_local_irq_save
	nop

	lui	$3,%hi(prot_counters)
	addiu	$3,$3,%lo(prot_counters)
	lw	$4,4($3)
	nop
	addiu	$4,$4,-1
	j	$L21
	sw	$4,4($3)

$L17:
	jal	arch_local_irq_save
	nop

	lui	$3,%hi(prot_counters)
	addiu	$3,$3,%lo(prot_counters)
	lw	$4,8($3)
	nop
	addiu	$4,$4,-1
	j	$L21
	sw	$4,8($3)

$L22:
	jal	arch_local_irq_save
	nop

	lui	$3,%hi(prot_counters)
	lw	$4,%lo(prot_counters)($3)
	nop
	addiu	$4,$4,-1
	sw	$4,%lo(prot_counters)($3)
$L21:
	jal	arch_local_irq_restore
	move	$4,$2

$L14:
	move	$4,$16
	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0

	li	$3,6			# 0x6
	beq	$2,$3,$L23
	move	$4,$16

	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0

	li	$3,17			# 0x11
	bne	$2,$3,$L19
	move	$4,$16

$L23:
	jal	clean_from_lists_hooks
	move	$5,$17

	jal	rtl_list_del
	move	$4,$16

	lui	$2,%hi(nf_conntrack_max)
	lw	$2,%lo(nf_conntrack_max)($2)
	nop
	beq	$2,$0,$L20
	lui	$18,%hi(rtl_newGC_session_status_flags)

	jal	rtl_gc_threshold_check
	move	$4,$17

	bne	$2,$0,$L20
	li	$2,3			# 0x3

	lw	$3,%lo(rtl_newGC_session_status_flags)($18)
	nop
	beq	$3,$2,$L24
	move	$4,$16

	sw	$2,%lo(rtl_newGC_session_status_flags)($18)
	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$3,1
	j	$L24
	sw	$3,%lo(rtl_newGC_session_status_time)($2)

$L20:
	sw	$0,%lo(rtl_newGC_session_status_flags)($18)
$L19:
	move	$4,$16
$L24:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	nf_ct_remove_expectations
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	clean_from_lists
	.size	clean_from_lists, .-clean_from_lists
	.section	.text.rtl_death_action,"ax",@progbits
	.align	2
	.globl	rtl_death_action
	.set	nomips16
	.ent	rtl_death_action
	.type	rtl_death_action, @function
rtl_death_action:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	move	$16,$4
	lw	$2,100($4)
	addiu	$5,$16,100
	.set	noreorder
	.set	nomacro
	jal	__mips_set_bit
	li	$4,9			# 0x9
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	nf_ct_delete_from_lists
	move	$4,$16
	.set	macro
	.set	reorder

	beq	$16,$0,$L25
 #APP
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$17,0($16)
	move	$4,$2
	addiu	$17,$17,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$17,0($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$17,$0,$L25
	move	$4,$16
	.set	macro
	.set	reorder

	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	.set	noreorder
	.set	nomacro
	j	nf_conntrack_destroy
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

$L25:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	rtl_death_action
	.size	rtl_death_action, .-rtl_death_action
	.section	.text.rtl_connGC_addList,"ax",@progbits
	.align	2
	.globl	rtl_connGC_addList
	.set	nomips16
	.ent	rtl_connGC_addList
	.type	rtl_connGC_addList, @function
rtl_connGC_addList:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	move	$16,$4
	jal	rtl_new_gc_ip_hdr
	move	$17,$5

	beq	$2,$0,$L28
	nop

	jal	rtl_new_gc_get_skb_protocol
	move	$4,$16

	li	$3,6			# 0x6
	beq	$2,$3,$L30
	li	$3,17			# 0x11

	beq	$2,$3,$L31
	li	$3,1			# 0x1

	bne	$2,$3,$L28
	nop

	j	$L37
	nop

$L30:
	jal	rtl_new_gc_get_ct_tcp_state
	move	$4,$17

	lui	$3,%hi(Tcp_State_Hash_Head)
	sll	$2,$2,3
	addiu	$3,$3,%lo(Tcp_State_Hash_Head)
	addu	$2,$3,$2
	lw	$2,4($2)
	nop
	beq	$2,$0,$L32
	li	$5,1			# 0x1

	move	$4,$17
	jal	rtl_list_add_tail
	move	$6,$0

$L32:
	jal	arch_local_irq_save
	nop

	lui	$3,%hi(prot_counters)
	addiu	$3,$3,%lo(prot_counters)
	lw	$4,4($3)
	nop
	addiu	$4,$4,1
	j	$L36
	sw	$4,4($3)

$L31:
	jal	rtl_new_gc_get_ct_udp_status
	move	$4,$17

	andi	$2,$2,0x2
	move	$4,$17
	li	$5,2			# 0x2
	bne	$2,$0,$L35
	li	$6,1			# 0x1

	move	$6,$0
$L35:
	jal	rtl_list_add_tail
	nop

	jal	arch_local_irq_save
	nop

	lui	$3,%hi(prot_counters)
	addiu	$3,$3,%lo(prot_counters)
	lw	$4,8($3)
	nop
	addiu	$4,$4,1
	j	$L36
	sw	$4,8($3)

$L37:
	jal	arch_local_irq_save
	nop

	lui	$3,%hi(prot_counters)
	lw	$4,%lo(prot_counters)($3)
	nop
	addiu	$4,$4,1
	sw	$4,%lo(prot_counters)($3)
$L36:
	jal	arch_local_irq_restore
	move	$4,$2

$L28:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl_connGC_addList
	.size	rtl_connGC_addList, .-rtl_connGC_addList
	.section	.text.__nf_ct_refresh_acct_proto,"ax",@progbits
	.align	2
	.globl	__nf_ct_refresh_acct_proto
	.set	nomips16
	.ent	__nf_ct_refresh_acct_proto
	.type	__nf_ct_refresh_acct_proto, @function
__nf_ct_refresh_acct_proto:
	.frame	$sp,56,$31		# vars= 8, regs= 6/0, args= 24, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$20,48($sp)
	sw	$19,44($sp)
	sw	$18,40($sp)
	sw	$17,36($sp)
	sw	$16,32($sp)
	move	$16,$4
	move	$18,$5
	move	$17,$6
	move	$20,$7
	lbu	$19,79($sp)
	.set	noreorder
	.set	nomacro
	jal	local_bh_disable
	sw	$0,24($sp)
	.set	macro
	.set	reorder

	li	$5,10			# 0xa
	.set	noreorder
	.set	nomacro
	jal	rtl_test_bit
	move	$4,$16
	.set	macro
	.set	reorder

	beq	$2,$0,$L47
$L39:
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_test_bit
	li	$5,3			# 0x3
	.set	macro
	.set	reorder

	li	$3,-1			# 0xffffffffffffffff
	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L41
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_set_ct_timeout_expires
	move	$5,$20
	.set	macro
	.set	reorder

	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	j	$L42
	sw	$2,24($sp)
	.set	macro
	.set	reorder

$L41:
	jal	rtl_del_ct_timer
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L42
	lui	$2,%hi(jiffies)
	.set	macro
	.set	reorder

	lw	$5,%lo(jiffies)($2)
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_set_ct_timeout_expires
	addu	$5,$20,$5
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_add_ct_timer
	move	$4,$16
	.set	macro
	.set	reorder

	li	$2,2048			# 0x800
	sw	$2,24($sp)
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$19,$2,$L43
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$19,$2,$L42
	j	$L48
$L43:
	move	$4,$16
	lw	$6,84($sp)
	.set	noreorder
	.set	nomacro
	j	$L46
	li	$5,1			# 0x1
	.set	macro
	.set	reorder

$L48:
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_udp_status
	move	$4,$16
	.set	macro
	.set	reorder

	andi	$2,$2,0x2
	move	$4,$16
	li	$5,2			# 0x2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L46
	li	$6,1			# 0x1
	.set	macro
	.set	reorder

	move	$6,$0
$L46:
	jal	rtl_list_move_tail
$L42:
	lw	$7,72($sp)
	addiu	$2,$sp,24
	move	$4,$16
	move	$5,$18
	move	$6,$17
	.set	noreorder
	.set	nomacro
	jal	__nf_ct_refresh_acct_proto_hooks
	sw	$2,16($sp)
	.set	macro
	.set	reorder

$L47:
	jal	local_bh_enable
	lw	$31,52($sp)
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,56
	.set	macro
	.set	reorder

	.end	__nf_ct_refresh_acct_proto
	.size	__nf_ct_refresh_acct_proto, .-__nf_ct_refresh_acct_proto
	.section	.text.__conntrack_drop_check,"ax",@progbits
	.align	2
	.globl	__conntrack_drop_check
	.set	nomips16
	.ent	__conntrack_drop_check
	.type	__conntrack_drop_check, @function
__conntrack_drop_check:
	.frame	$sp,56,$31		# vars= 0, regs= 9/0, args= 16, gp= 0
	.mask	0x80ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$23,48($sp)
	sw	$22,44($sp)
	sw	$21,40($sp)
	sw	$20,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	move	$5,$0
	jal	rtl_new_gc_get_ct_protonum
	move	$16,$4

	move	$4,$16
	move	$5,$0
	jal	rtl_new_gc_get_ct_port_by_dir
	move	$6,$0

	move	$4,$16
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_new_gc_get_ct_port_by_dir
	move	$20,$2

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_new_gc_get_ct_port_by_dir
	move	$19,$2

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_new_gc_get_ct_port_by_dir
	move	$18,$2

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_new_gc_get_ct_ip_by_dir
	move	$17,$2

	move	$4,$16
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_new_gc_get_ct_ip_by_dir
	move	$23,$2

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_new_gc_get_ct_ip_by_dir
	move	$21,$2

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_new_gc_get_ct_ip_by_dir
	move	$22,$2

	li	$5,-268435456			# 0xfffffffff0000000
	and	$6,$21,$5
	li	$4,-536870912			# 0xffffffffe0000000
	move	$3,$2
	beq	$6,$4,$L50
	li	$2,-1			# 0xffffffffffffffff

	and	$5,$3,$5
	beq	$5,$4,$L50
	nop

	bne	$23,$3,$L63
	sltu	$3,$20,1024

	beq	$21,$22,$L50
	nop

$L63:
	bne	$3,$0,$L50
	li	$2,-1			# 0xffffffffffffffff

	sltu	$3,$19,1024
	bne	$3,$0,$L50
	sltu	$3,$18,1024

	bne	$3,$0,$L50
	sltu	$3,$17,1024

	bne	$3,$0,$L50
	li	$3,8080			# 0x1f90

	beq	$20,$3,$L50
	nop

	beq	$19,$3,$L50
	nop

	beq	$18,$3,$L50
	xori	$17,$17,0x1f90

	movn	$2,$0,$17
$L50:
	lw	$31,52($sp)
	lw	$23,48($sp)
	lw	$22,44($sp)
	lw	$21,40($sp)
	lw	$20,36($sp)
	lw	$19,32($sp)
	lw	$18,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	__conntrack_drop_check
	.size	__conntrack_drop_check, .-__conntrack_drop_check
	.section	.text.drop_one_conntrack,"ax",@progbits
	.align	2
	.globl	drop_one_conntrack
	.set	nomips16
	.ent	drop_one_conntrack
	.type	drop_one_conntrack, @function
drop_one_conntrack:
	.frame	$sp,72,$31		# vars= 8, regs= 10/0, args= 24, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-72
	sw	$31,68($sp)
	sw	$fp,64($sp)
	sw	$23,60($sp)
	sw	$22,56($sp)
	sw	$21,52($sp)
	sw	$20,48($sp)
	sw	$19,44($sp)
	sw	$18,40($sp)
	sw	$17,36($sp)
	sw	$16,32($sp)
	lui	$fp,%hi(_prot_limit)
	addiu	$2,$fp,%lo(_prot_limit)
	lui	$23,%hi(prot_counters)
	lw	$3,4($2)
	addiu	$2,$23,%lo(prot_counters)
	lw	$2,4($2)
	#nop
	slt	$2,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L65
	lui	$20,%hi(drop_priority_max_idx)
	.set	macro
	.set	reorder

	jal	local_bh_disable
	lui	$21,%hi(drop_priority)
	addiu	$21,$21,%lo(drop_priority)
	.set	noreorder
	.set	nomacro
	j	$L66
	move	$19,$0
	.set	macro
	.set	reorder

$L73:
	lbu	$2,0($21)
	#nop
	sltu	$3,$2,10
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L67
	lui	$3,%hi(Tcp_State_Hash_Head)
	.set	macro
	.set	reorder

	sll	$2,$2,3
	addiu	$3,$3,%lo(Tcp_State_Hash_Head)
	addu	$2,$3,$2
	lw	$22,4($2)
	#nop
	lw	$16,0($22)
	#nop
	beq	$16,$22,$L67
	lw	$17,0($16)
	j	$L72
$L122:
	move	$17,$2
$L72:
	lb	$2,8($16)
	li	$4,-1			# 0xffffffffffffffff
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L141
	addiu	$18,$16,-140
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	__conntrack_drop_check
	move	$4,$18
	.set	macro
	.set	reorder

	sb	$2,8($16)
	lb	$2,8($16)
	#nop
$L141:
	bne	$2,$0,$L69
	.set	noreorder
	.set	nomacro
	jal	rtl_del_ct_timer
	move	$4,$18
	.set	macro
	.set	reorder

	bne	$2,$0,$L133
$L69:
	lw	$2,0($17)
	.set	noreorder
	.set	nomacro
	bne	$17,$22,$L122
	move	$16,$17
	.set	macro
	.set	reorder

$L67:
	addiu	$19,$19,1
	addiu	$21,$21,4
$L66:
	lw	$2,%lo(drop_priority_max_idx)($20)
	#nop
	slt	$2,$19,$2
	bne	$2,$0,$L73
	jal	local_bh_enable
$L65:
	addiu	$fp,$fp,%lo(_prot_limit)
	addiu	$23,$23,%lo(prot_counters)
	lw	$3,8($fp)
	lw	$2,8($23)
	#nop
	slt	$2,$3,$2
	beq	$2,$0,$L74
	jal	local_bh_disable
	lui	$21,%hi(drop_priority)
	lui	$fp,%hi(Udp_State_Hash_Head)
	addiu	$21,$21,%lo(drop_priority)
	move	$19,$0
	addiu	$fp,$fp,%lo(Udp_State_Hash_Head)
	.set	noreorder
	.set	nomacro
	j	$L75
	li	$23,-1			# 0xffffffffffffffff
	.set	macro
	.set	reorder

$L81:
	lbu	$2,0($21)
	#nop
	sltu	$3,$2,11
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L76
	addiu	$2,$2,-11
	.set	macro
	.set	reorder

	sll	$2,$2,3
	addu	$2,$fp,$2
	lw	$22,4($2)
	#nop
	lw	$16,0($22)
	#nop
	beq	$16,$22,$L76
	lw	$17,0($16)
	j	$L80
$L123:
	move	$17,$2
$L80:
	lb	$2,8($16)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$23,$L142
	addiu	$18,$16,-140
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	__conntrack_drop_check
	move	$4,$18
	.set	macro
	.set	reorder

	sb	$2,8($16)
	lb	$2,8($16)
	#nop
$L142:
	bne	$2,$0,$L78
	.set	noreorder
	.set	nomacro
	jal	rtl_del_ct_timer
	move	$4,$18
	.set	macro
	.set	reorder

	beq	$2,$0,$L78
$L133:
	jal	local_bh_enable
	.set	noreorder
	.set	nomacro
	jal	rtl_death_action
	move	$4,$18
	.set	macro
	.set	reorder

	lui	$2,%hi(rtl_newGC_session_status_flags)
	lw	$3,%lo(rtl_newGC_session_status_flags)($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L79
	li	$3,3			# 0x3
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L71
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L78:
	lw	$2,0($17)
	.set	noreorder
	.set	nomacro
	bne	$17,$22,$L123
	move	$16,$17
	.set	macro
	.set	reorder

$L76:
	addiu	$19,$19,1
	addiu	$21,$21,4
$L75:
	lw	$2,%lo(drop_priority_max_idx)($20)
	#nop
	slt	$2,$19,$2
	bne	$2,$0,$L81
	jal	local_bh_enable
	j	$L74
$L79:
	sw	$3,%lo(rtl_newGC_session_status_flags)($2)
	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$3,1
	sw	$3,%lo(rtl_newGC_session_status_time)($2)
	.set	noreorder
	.set	nomacro
	j	$L71
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L74:
	jal	local_bh_disable
	lui	$17,%hi(drop_priority+2)
	addiu	$17,$17,%lo(drop_priority+2)
	move	$19,$17
	move	$16,$0
	.set	noreorder
	.set	nomacro
	j	$L82
	lui	$fp,%hi(jiffies)
	.set	macro
	.set	reorder

$L99:
	lbu	$2,-2($19)
	#nop
	sltu	$3,$2,10
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L83
	lui	$3,%hi(Udp_State_Hash_Head)
	.set	macro
	.set	reorder

	lui	$3,%hi(Tcp_State_Hash_Head)
	addiu	$3,$3,%lo(Tcp_State_Hash_Head)
	sll	$2,$2,3
	addu	$2,$3,$2
	lw	$23,4($2)
	#nop
	lw	$3,0($23)
	#nop
	beq	$3,$23,$L91
	lw	$21,0($3)
	.set	noreorder
	.set	nomacro
	j	$L90
	move	$22,$0
	.set	macro
	.set	reorder

$L124:
	move	$21,$2
$L90:
	lb	$2,8($3)
	li	$4,-1			# 0xffffffffffffffff
	addiu	$18,$3,-140
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L143
	addiu	$22,$22,1
	.set	macro
	.set	reorder

	move	$4,$18
	.set	noreorder
	.set	nomacro
	jal	__conntrack_drop_check
	sw	$3,24($sp)
	.set	macro
	.set	reorder

	lw	$3,24($sp)
	#nop
	sb	$2,8($3)
	lb	$2,8($3)
	#nop
$L143:
	bne	$2,$0,$L86
	.set	noreorder
	.set	nomacro
	jal	rtl_get_ct_timer_expires
	move	$4,$18
	.set	macro
	.set	reorder

	lw	$3,%lo(jiffies)($fp)
	lhu	$4,0($19)
	subu	$3,$2,$3
	srl	$3,$3,7
	sltu	$3,$4,$3
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L137
	slt	$2,$22,129
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_del_ct_timer
	move	$4,$18
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L137
	slt	$2,$22,129
	.set	macro
	.set	reorder

	li	$2,3			# 0x3
	move	$4,$18
	move	$5,$16
	move	$6,$0
	li	$7,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	__drop_one_conntrack_process_hooks1
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	beq	$2,$3,$L129
$L88:
	li	$3,2			# 0x2
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L101
	li	$4,3			# 0x3
	.set	macro
	.set	reorder

	sw	$4,16($sp)
	move	$5,$16
	move	$4,$18
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	__drop_one_conntrack_process_hooks2
	li	$7,1			# 0x1
	.set	macro
	.set	reorder

	slt	$2,$22,129
$L137:
	beq	$2,$0,$L91
$L86:
	lw	$2,0($21)
	.set	noreorder
	.set	nomacro
	bne	$21,$23,$L124
	move	$3,$21
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L134
	addiu	$16,$16,1
	.set	macro
	.set	reorder

$L83:
	addiu	$2,$2,-11
	addiu	$3,$3,%lo(Udp_State_Hash_Head)
	sll	$2,$2,3
	addu	$2,$3,$2
	lw	$23,4($2)
	#nop
	lw	$3,0($23)
	#nop
	beq	$3,$23,$L91
	lw	$21,0($3)
	.set	noreorder
	.set	nomacro
	j	$L98
	move	$22,$0
	.set	macro
	.set	reorder

$L125:
	move	$21,$2
$L98:
	lb	$2,8($3)
	li	$4,-1			# 0xffffffffffffffff
	addiu	$18,$3,-140
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L144
	addiu	$22,$22,1
	.set	macro
	.set	reorder

	move	$4,$18
	.set	noreorder
	.set	nomacro
	jal	__conntrack_drop_check
	sw	$3,24($sp)
	.set	macro
	.set	reorder

	lw	$3,24($sp)
	#nop
	sb	$2,8($3)
	lb	$2,8($3)
	#nop
$L144:
	bne	$2,$0,$L94
	.set	noreorder
	.set	nomacro
	jal	rtl_get_ct_timer_expires
	move	$4,$18
	.set	macro
	.set	reorder

	lw	$3,%lo(jiffies)($fp)
	lhu	$4,0($19)
	subu	$3,$2,$3
	srl	$3,$3,7
	sltu	$3,$4,$3
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L138
	slt	$2,$22,129
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_del_ct_timer
	move	$4,$18
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L138
	slt	$2,$22,129
	.set	macro
	.set	reorder

	li	$2,12			# 0xc
	move	$4,$18
	move	$5,$16
	move	$6,$0
	li	$7,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	__drop_one_conntrack_process_hooks1
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	beq	$2,$3,$L129
$L96:
	li	$3,2			# 0x2
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L101
	li	$4,12			# 0xc
	.set	macro
	.set	reorder

	sw	$4,16($sp)
	move	$5,$16
	move	$4,$18
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	__drop_one_conntrack_process_hooks2
	li	$7,1			# 0x1
	.set	macro
	.set	reorder

	slt	$2,$22,129
$L138:
	beq	$2,$0,$L91
$L94:
	lw	$2,0($21)
	.set	noreorder
	.set	nomacro
	bne	$21,$23,$L125
	move	$3,$21
	.set	macro
	.set	reorder

$L91:
	addiu	$16,$16,1
$L134:
	addiu	$19,$19,4
$L82:
	lw	$2,%lo(drop_priority_max_idx)($20)
	#nop
	slt	$2,$16,$2
	bne	$2,$0,$L99
	jal	local_bh_enable
	jal	local_bh_disable
	lui	$fp,%hi(Udp_State_Hash_Head)
	move	$16,$0
	addiu	$fp,$fp,%lo(Udp_State_Hash_Head)
	.set	noreorder
	.set	nomacro
	j	$L100
	lui	$23,%hi(jiffies)
	.set	macro
	.set	reorder

$L118:
	lbu	$2,-2($17)
	#nop
	sltu	$3,$2,10
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L102
	lui	$3,%hi(Tcp_State_Hash_Head)
	.set	macro
	.set	reorder

	addiu	$3,$3,%lo(Tcp_State_Hash_Head)
	sll	$2,$2,3
	addu	$2,$3,$2
	lw	$22,4($2)
	#nop
	lw	$3,0($22)
	#nop
	beq	$3,$22,$L110
	lw	$19,0($3)
	.set	noreorder
	.set	nomacro
	j	$L109
	move	$21,$0
	.set	macro
	.set	reorder

$L126:
	move	$19,$2
$L109:
	lb	$2,8($3)
	li	$4,-1			# 0xffffffffffffffff
	addiu	$18,$3,-140
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L145
	addiu	$21,$21,1
	.set	macro
	.set	reorder

	move	$4,$18
	.set	noreorder
	.set	nomacro
	jal	__conntrack_drop_check
	sw	$3,24($sp)
	.set	macro
	.set	reorder

	lw	$3,24($sp)
	#nop
	sb	$2,8($3)
	lb	$2,8($3)
	#nop
$L145:
	bne	$2,$0,$L105
	.set	noreorder
	.set	nomacro
	jal	rtl_get_ct_timer_expires
	move	$4,$18
	.set	macro
	.set	reorder

	lw	$3,%lo(jiffies)($23)
	lhu	$4,0($17)
	subu	$3,$2,$3
	srl	$3,$3,9
	sltu	$3,$4,$3
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L139
	slt	$2,$21,129
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_del_ct_timer
	move	$4,$18
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L139
	slt	$2,$21,129
	.set	macro
	.set	reorder

	li	$2,3			# 0x3
	move	$4,$18
	move	$5,$16
	li	$6,2			# 0x2
	move	$7,$0
	.set	noreorder
	.set	nomacro
	jal	__drop_one_conntrack_process_hooks1
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	beq	$2,$3,$L129
$L107:
	li	$3,2			# 0x2
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L101
	li	$4,3			# 0x3
	.set	macro
	.set	reorder

	sw	$4,16($sp)
	move	$5,$16
	move	$4,$18
	li	$6,2			# 0x2
	.set	noreorder
	.set	nomacro
	jal	__drop_one_conntrack_process_hooks2
	move	$7,$0
	.set	macro
	.set	reorder

	slt	$2,$21,129
$L139:
	beq	$2,$0,$L110
$L105:
	lw	$2,0($19)
	.set	noreorder
	.set	nomacro
	bne	$19,$22,$L126
	move	$3,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L135
	addiu	$16,$16,1
	.set	macro
	.set	reorder

$L102:
	addiu	$2,$2,-11
	sll	$2,$2,3
	addu	$2,$fp,$2
	lw	$22,4($2)
	#nop
	lw	$3,0($22)
	#nop
	beq	$3,$22,$L110
	lw	$19,0($3)
	.set	noreorder
	.set	nomacro
	j	$L117
	move	$21,$0
	.set	macro
	.set	reorder

$L127:
	move	$19,$2
$L117:
	lb	$2,8($3)
	li	$4,-1			# 0xffffffffffffffff
	addiu	$18,$3,-140
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L146
	addiu	$21,$21,1
	.set	macro
	.set	reorder

	move	$4,$18
	.set	noreorder
	.set	nomacro
	jal	__conntrack_drop_check
	sw	$3,24($sp)
	.set	macro
	.set	reorder

	lw	$3,24($sp)
	#nop
	sb	$2,8($3)
	lb	$2,8($3)
	#nop
$L146:
	bne	$2,$0,$L113
	.set	noreorder
	.set	nomacro
	jal	rtl_get_ct_timer_expires
	move	$4,$18
	.set	macro
	.set	reorder

	lw	$3,%lo(jiffies)($23)
	lhu	$4,0($17)
	subu	$3,$2,$3
	srl	$3,$3,9
	sltu	$3,$4,$3
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L140
	slt	$2,$21,129
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_del_ct_timer
	move	$4,$18
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L140
	slt	$2,$21,129
	.set	macro
	.set	reorder

	li	$2,12			# 0xc
	move	$4,$18
	move	$5,$16
	li	$6,2			# 0x2
	move	$7,$0
	.set	noreorder
	.set	nomacro
	jal	__drop_one_conntrack_process_hooks1
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	bne	$2,$3,$L115
$L129:
	jal	local_bh_enable
	.set	noreorder
	.set	nomacro
	jal	rtl_death_action
	move	$4,$18
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L136
	lui	$3,%hi(rtl_newGC_session_status_flags)
	.set	macro
	.set	reorder

$L115:
	li	$3,2			# 0x2
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L101
	li	$4,12			# 0xc
	.set	macro
	.set	reorder

	sw	$4,16($sp)
	move	$5,$16
	move	$4,$18
	li	$6,2			# 0x2
	.set	noreorder
	.set	nomacro
	jal	__drop_one_conntrack_process_hooks2
	move	$7,$0
	.set	macro
	.set	reorder

	slt	$2,$21,129
$L140:
	beq	$2,$0,$L110
$L113:
	lw	$2,0($19)
	.set	noreorder
	.set	nomacro
	bne	$19,$22,$L127
	move	$3,$19
	.set	macro
	.set	reorder

$L110:
	addiu	$16,$16,1
$L135:
	addiu	$17,$17,4
$L100:
	lw	$2,%lo(drop_priority_max_idx)($20)
	#nop
	slt	$2,$16,$2
	bne	$2,$0,$L118
	jal	local_bh_enable
	lui	$2,%hi(rtl_newGC_session_status_flags)
	lw	$4,%lo(rtl_newGC_session_status_flags)($2)
	li	$3,1			# 0x1
	bne	$4,$3,$L119
	.set	noreorder
	.set	nomacro
	j	$L71
	move	$2,$0
	.set	macro
	.set	reorder

$L101:
	lui	$3,%hi(rtl_newGC_session_status_flags)
$L136:
	lw	$4,%lo(rtl_newGC_session_status_flags)($3)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L71
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

	li	$4,3			# 0x3
	sw	$4,%lo(rtl_newGC_session_status_flags)($3)
	lui	$3,%hi(jiffies)
	lw	$4,%lo(jiffies)($3)
	lui	$3,%hi(rtl_newGC_session_status_time)
	addiu	$4,$4,1
	.set	noreorder
	.set	nomacro
	j	$L71
	sw	$4,%lo(rtl_newGC_session_status_time)($3)
	.set	macro
	.set	reorder

$L119:
	sw	$3,%lo(rtl_newGC_session_status_flags)($2)
	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$3,200
	sw	$3,%lo(rtl_newGC_session_status_time)($2)
	move	$2,$0
$L71:
	lw	$31,68($sp)
	lw	$fp,64($sp)
	lw	$23,60($sp)
	lw	$22,56($sp)
	lw	$21,52($sp)
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,72
	.set	macro
	.set	reorder

	.end	drop_one_conntrack
	.size	drop_one_conntrack, .-drop_one_conntrack
	.section	.text.rtl_nf_conn_GC_init,"ax",@progbits
	.align	2
	.globl	rtl_nf_conn_GC_init
	.set	nomips16
	.ent	rtl_nf_conn_GC_init
	.type	rtl_nf_conn_GC_init, @function
rtl_nf_conn_GC_init:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$4,%hi(Tcp_State_Hash_Head)
	move	$3,$0
	li	$5,11			# 0xb
	j	$L148
	addiu	$4,$4,%lo(Tcp_State_Hash_Head)

$L150:
	addu	$2,$4,$2
	lw	$2,4($2)
	nop
	beq	$2,$0,$L149
	nop

	sw	$2,0($2)
	sw	$2,4($2)
$L149:
	addiu	$3,$3,1
$L148:
	bne	$3,$5,$L150
	sll	$2,$3,3

	lui	$3,%hi(Udp_State_Hash_Head)
	addiu	$3,$3,%lo(Udp_State_Hash_Head)
	lw	$2,4($3)
	nop
	beq	$2,$0,$L151
	nop

	sw	$2,0($2)
	sw	$2,4($2)
$L151:
	lw	$2,12($3)
	nop
	beq	$2,$0,$L158
	lui	$16,%hi(prot_limit)

	sw	$2,0($2)
	sw	$2,4($2)
$L158:
	addiu	$4,$16,%lo(prot_limit)
	move	$5,$0
	jal	memset
	li	$6,12			# 0xc

	lui	$2,%hi(prot_counters)
	sw	$0,%lo(prot_counters)($2)
	addiu	$2,$2,%lo(prot_counters)
	li	$3,10			# 0xa
	sw	$0,4($2)
	sw	$0,8($2)
	lui	$2,%hi(drop_priority_max_idx)
	sw	$3,%lo(drop_priority_max_idx)($2)
	li	$2,2			# 0x2
	sw	$2,%lo(prot_limit)($16)
	addiu	$16,$16,%lo(prot_limit)
	li	$2,90			# 0x5a
	sw	$2,4($16)
	li	$2,60			# 0x3c
	sw	$2,8($16)
	lui	$2,%hi(nf_conntrack_max)
	lui	$6,%hi(_prot_limit)
	lw	$2,%lo(nf_conntrack_max)($2)
	sw	$0,%lo(_prot_limit)($6)
	lui	$17,%hi(rtl_newGC_session_status_flags)
	addiu	$6,$6,%lo(_prot_limit)
	sw	$0,4($6)
	sw	$0,8($6)
	sw	$0,%lo(rtl_newGC_session_status_flags)($17)
	move	$3,$0
	li	$4,100			# 0x64
	li	$5,3			# 0x3
$L153:
	lw	$8,0($16)
	sll	$7,$3,2
	addu	$7,$6,$7
	mult	$2,$8
	addiu	$3,$3,1
	addiu	$16,$16,4
	mflo	$8
	divu	$0,$8,$4
	mflo	$9
	bne	$3,$5,$L153
	sw	$9,0($7)

	sll	$5,$2,2
	li	$3,5			# 0x5
	divu	$0,$5,$3
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	mflo	$5
	sw	$5,%lo(rtl_nf_conntrack_threshold)($4)
	subu	$5,$2,$5
	sltu	$5,$5,65
	bne	$5,$0,$L154
	addiu	$3,$2,-64

	sw	$3,%lo(rtl_nf_conntrack_threshold)($4)
$L154:
	beq	$2,$0,$L155
	nop

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L155
	li	$2,3			# 0x3

	lw	$3,%lo(rtl_newGC_session_status_flags)($17)
	nop
	beq	$3,$2,$L156
	lui	$3,%hi(rtl_newGC_session_status_flags)

	sw	$2,%lo(rtl_newGC_session_status_flags)($3)
	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$3,1
	j	$L156
	sw	$3,%lo(rtl_newGC_session_status_time)($2)

$L155:
	sw	$0,%lo(rtl_newGC_session_status_flags)($17)
$L156:
	jal	rtl_nf_conn_GC_init_hooks
	nop

	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl_nf_conn_GC_init
	.size	rtl_nf_conn_GC_init, .-rtl_nf_conn_GC_init
	.globl	prot_limit
	.section	.bss.prot_limit,"aw",@nobits
	.align	2
	.type	prot_limit, @object
	.size	prot_limit, 12
prot_limit:
	.space	12
	.globl	drop_priority_max_idx
	.section	.bss.drop_priority_max_idx,"aw",@nobits
	.align	2
	.type	drop_priority_max_idx, @object
	.size	drop_priority_max_idx, 4
drop_priority_max_idx:
	.space	4
	.globl	drop_priority
	.section	.data.drop_priority,"aw",@progbits
	.align	2
	.type	drop_priority, @object
	.size	drop_priority, 40
drop_priority:
	.byte	8
	.space	1
	.half	60
	.byte	6
	.space	1
	.half	30
	.byte	5
	.space	1
	.half	60
	.byte	7
	.space	1
	.half	120
	.byte	4
	.space	1
	.half	120
	.byte	11
	.space	1
	.half	85
	.byte	1
	.space	1
	.half	110
	.byte	2
	.space	1
	.half	30
	.byte	12
	.space	1
	.half	10
	.byte	3
	.space	1
	.half	120
	.globl	Udp_State_Hash_Head
	.section	.data.Udp_State_Hash_Head,"aw",@progbits
	.align	2
	.type	Udp_State_Hash_Head, @object
	.size	Udp_State_Hash_Head, 16
Udp_State_Hash_Head:
	.byte	11
	.space	3
	.word	udp_unreply_list
	.byte	12
	.space	3
	.word	udp_assured_list
	.globl	Tcp_State_Hash_Head
	.section	.data.Tcp_State_Hash_Head,"aw",@progbits
	.align	2
	.type	Tcp_State_Hash_Head, @object
	.size	Tcp_State_Hash_Head, 88
Tcp_State_Hash_Head:
	.word	0
	.word	0
	.word	1
	.word	syn_sent_list
	.word	2
	.word	syn_recv_list
	.word	3
	.word	established_list
	.word	4
	.word	fin_wait_list
	.word	5
	.word	close_wait_list
	.word	6
	.word	last_ack_list
	.word	7
	.word	time_wait_list
	.word	8
	.word	close_list
	.word	9
	.word	listen_list
	.word	10
	.word	0
	.globl	rtl_newGC_session_status_time
	.section	.dram-fwd,"aw",@progbits
	.align	2
	.type	rtl_newGC_session_status_time, @object
	.size	rtl_newGC_session_status_time, 4
rtl_newGC_session_status_time:
	.space	4
	.globl	rtl_newGC_session_status_flags
	.align	2
	.type	rtl_newGC_session_status_flags, @object
	.size	rtl_newGC_session_status_flags, 4
rtl_newGC_session_status_flags:
	.space	4
	.globl	conntrack_max
	.section	.data.conntrack_max,"aw",@progbits
	.align	2
	.type	conntrack_max, @object
	.size	conntrack_max, 4
conntrack_max:
	.word	100
	.globl	conntrack_min
	.section	.bss.conntrack_min,"aw",@nobits
	.align	2
	.type	conntrack_min, @object
	.size	conntrack_min, 4
conntrack_min:
	.space	4
	.section	.bss._prot_limit,"aw",@nobits
	.align	2
	.type	_prot_limit, @object
	.size	_prot_limit, 12
_prot_limit:
	.space	12
	.section	.bss.prot_counters,"aw",@nobits
	.align	2
	.type	prot_counters, @object
	.size	prot_counters, 12
prot_counters:
	.space	12
	.section	.data.udp_unreply_list,"aw",@progbits
	.align	2
	.type	udp_unreply_list, @object
	.size	udp_unreply_list, 8
udp_unreply_list:
	.word	udp_unreply_list
	.word	udp_unreply_list
	.section	.data.udp_assured_list,"aw",@progbits
	.align	2
	.type	udp_assured_list, @object
	.size	udp_assured_list, 8
udp_assured_list:
	.word	udp_assured_list
	.word	udp_assured_list
	.section	.data.syn_sent_list,"aw",@progbits
	.align	2
	.type	syn_sent_list, @object
	.size	syn_sent_list, 8
syn_sent_list:
	.word	syn_sent_list
	.word	syn_sent_list
	.section	.data.syn_recv_list,"aw",@progbits
	.align	2
	.type	syn_recv_list, @object
	.size	syn_recv_list, 8
syn_recv_list:
	.word	syn_recv_list
	.word	syn_recv_list
	.section	.data.established_list,"aw",@progbits
	.align	2
	.type	established_list, @object
	.size	established_list, 8
established_list:
	.word	established_list
	.word	established_list
	.section	.data.fin_wait_list,"aw",@progbits
	.align	2
	.type	fin_wait_list, @object
	.size	fin_wait_list, 8
fin_wait_list:
	.word	fin_wait_list
	.word	fin_wait_list
	.section	.data.close_wait_list,"aw",@progbits
	.align	2
	.type	close_wait_list, @object
	.size	close_wait_list, 8
close_wait_list:
	.word	close_wait_list
	.word	close_wait_list
	.section	.data.last_ack_list,"aw",@progbits
	.align	2
	.type	last_ack_list, @object
	.size	last_ack_list, 8
last_ack_list:
	.word	last_ack_list
	.word	last_ack_list
	.section	.data.time_wait_list,"aw",@progbits
	.align	2
	.type	time_wait_list, @object
	.size	time_wait_list, 8
time_wait_list:
	.word	time_wait_list
	.word	time_wait_list
	.section	.data.close_list,"aw",@progbits
	.align	2
	.type	close_list, @object
	.size	close_list, 8
close_list:
	.word	close_list
	.word	close_list
	.section	.data.listen_list,"aw",@progbits
	.align	2
	.type	listen_list, @object
	.size	listen_list, 8
listen_list:
	.word	listen_list
	.word	listen_list
	.ident	"GCC: (Realtek RSDK-4.6.4 Build 2080) 4.6.4"
