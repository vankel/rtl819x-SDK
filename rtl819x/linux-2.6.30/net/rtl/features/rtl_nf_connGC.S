	.file	1 "rtl_nf_connGC.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.rtl_nf_conn_GC_init,"ax",@progbits
	.align	2
	.globl	rtl_nf_conn_GC_init
	.set	nomips16
	.ent	rtl_nf_conn_GC_init
	.type	rtl_nf_conn_GC_init, @function
rtl_nf_conn_GC_init:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$3,%hi(Tcp_State_Hash_Head+4)
	lui	$4,%hi(Tcp_State_Hash_Head+92)
	addiu	$3,$3,%lo(Tcp_State_Hash_Head+4)
	j	$L2
	addiu	$4,$4,%lo(Tcp_State_Hash_Head+92)

$L4:
	beq	$2,$0,$L2
	addiu	$3,$3,8

	sw	$2,4($2)
	sw	$2,0($2)
$L2:
	bnel	$3,$4,$L4
	lw	$2,0($3)

	lui	$3,%hi(Udp_State_Hash_Head+4)
	lui	$4,%hi(Udp_State_Hash_Head+20)
	addiu	$3,$3,%lo(Udp_State_Hash_Head+4)
	j	$L5
	addiu	$4,$4,%lo(Udp_State_Hash_Head+20)

$L7:
	beq	$2,$0,$L5
	addiu	$3,$3,8

	sw	$2,4($2)
	sw	$2,0($2)
$L5:
	bnel	$3,$4,$L7
	lw	$2,0($3)

	lui	$4,%hi(prot_limit)
	addiu	$4,$4,%lo(prot_limit)
	move	$5,$0
	jal	memset
	li	$6,12			# 0xc

	lui	$5,%hi(prot_counters)
	lui	$4,%hi(_prot_limit)
	addiu	$5,$5,%lo(prot_counters)
	addiu	$4,$4,%lo(_prot_limit)
	move	$2,$0
	li	$3,12			# 0xc
$L8:
	addu	$7,$5,$2
	addu	$6,$4,$2
	addiu	$2,$2,4
	sw	$0,0($7)
	bne	$2,$3,$L8
	sw	$0,0($6)

	li	$4,10			# 0xa
	lui	$3,%hi(drop_priority_max_idx)
	sw	$4,%lo(drop_priority_max_idx)($3)
	lui	$3,%hi(rtl_newGC_session_status_flags)
	lui	$2,%hi(prot_limit)
	sw	$0,%lo(rtl_newGC_session_status_flags)($3)
	li	$3,2			# 0x2
	addiu	$7,$2,%lo(prot_limit)
	sw	$3,%lo(prot_limit)($2)
	li	$2,90			# 0x5a
	sw	$2,4($7)
	lui	$8,%hi(_prot_limit)
	li	$2,60			# 0x3c
	sw	$2,8($7)
	addiu	$8,$8,%lo(_prot_limit)
	move	$3,$0
	lui	$6,%hi(nf_conntrack_max)
	li	$4,100			# 0x64
	li	$5,12			# 0xc
$L9:
	addu	$2,$7,$3
	lw	$10,0($2)
	lw	$2,%lo(nf_conntrack_max)($6)
	addu	$9,$8,$3
	addiu	$3,$3,4
	mul	$10,$10,$2
	divu	$0,$10,$4
	mflo	$10
	bne	$3,$5,$L9
	sw	$10,0($9)

	sll	$5,$2,2
	li	$3,5			# 0x5
	divu	$0,$5,$3
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	mflo	$5
	subu	$3,$2,$5
	sltu	$3,$3,65
	bne	$3,$0,$L10
	sw	$5,%lo(rtl_nf_conntrack_threshold)($4)

	addiu	$3,$2,-64
	sw	$3,%lo(rtl_nf_conntrack_threshold)($4)
$L10:
	beq	$2,$0,$L17
	lui	$2,%hi(rtl_newGC_session_status_flags)

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L11
	lui	$2,%hi(rtl_newGC_session_status_flags)

	lw	$4,%lo(rtl_newGC_session_status_flags)($2)
	li	$3,3			# 0x3
	beq	$4,$3,$L12
	lui	$4,%hi(jiffies)

	lw	$4,%lo(jiffies)($4)
	sw	$3,%lo(rtl_newGC_session_status_flags)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$4,1
	j	$L12
	sw	$3,%lo(rtl_newGC_session_status_time)($2)

$L11:
$L17:
	sw	$0,%lo(rtl_newGC_session_status_flags)($2)
$L12:
	jal	rtl_nf_conn_GC_init_hooks
	nop

	lw	$31,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	rtl_nf_conn_GC_init
	.size	rtl_nf_conn_GC_init, .-rtl_nf_conn_GC_init
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

	li	$4,-268435456			# 0xfffffffff0000000
	and	$5,$21,$4
	li	$3,-536870912			# 0xffffffffe0000000
	beql	$5,$3,$L21
	li	$2,-1			# 0xffffffffffffffff

	and	$4,$2,$4
	beql	$4,$3,$L21
	li	$2,-1			# 0xffffffffffffffff

	bne	$23,$2,$L23
	sltu	$2,$20,1024

	beq	$21,$22,$L21
	li	$2,-1			# 0xffffffffffffffff

	sltu	$2,$20,1024
$L23:
	bne	$2,$0,$L21
	li	$2,-1			# 0xffffffffffffffff

	sltu	$2,$19,1024
	bne	$2,$0,$L21
	li	$2,-1			# 0xffffffffffffffff

	sltu	$2,$18,1024
	bne	$2,$0,$L21
	li	$2,-1			# 0xffffffffffffffff

	sltu	$2,$17,1024
	bnel	$2,$0,$L21
	li	$2,-1			# 0xffffffffffffffff

	li	$3,8080			# 0x1f90
	beq	$20,$3,$L21
	li	$2,-1			# 0xffffffffffffffff

	beq	$19,$3,$L21
	nop

	beq	$18,$3,$L24
	lw	$31,52($sp)

	bne	$17,$3,$L24
	move	$2,$0

	li	$2,-1			# 0xffffffffffffffff
$L21:
	lw	$31,52($sp)
$L24:
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
	.section	.text.__nf_ct_refresh_acct_proto,"ax",@progbits
	.align	2
	.globl	__nf_ct_refresh_acct_proto
	.set	nomips16
	.ent	__nf_ct_refresh_acct_proto
	.type	__nf_ct_refresh_acct_proto, @function
__nf_ct_refresh_acct_proto:
	.frame	$sp,64,$31		# vars= 8, regs= 7/0, args= 24, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-64
	sw	$31,60($sp)
	sw	$21,56($sp)
	sw	$20,52($sp)
	sw	$19,48($sp)
	sw	$18,44($sp)
	sw	$17,40($sp)
	sw	$16,36($sp)
	lui	$17,%hi(nf_conntrack_lock)
	move	$16,$4
	addiu	$4,$17,%lo(nf_conntrack_lock)
	move	$19,$5
	move	$18,$6
	move	$21,$7
	sw	$0,24($sp)
	jal	_raw_spin_lock_bh
	lbu	$20,87($sp)

	move	$4,$16
	jal	rtl_test_bit
	li	$5,10			# 0xa

	beq	$2,$0,$L35
	addiu	$4,$17,%lo(nf_conntrack_lock)

	move	$4,$16
	jal	rtl_test_bit
	li	$5,3			# 0x3

	li	$3,-1			# 0xffffffffffffffff
	bne	$2,$3,$L28
	move	$5,$21

	jal	rtl_new_gc_set_ct_timeout_expires
	move	$4,$16

	li	$2,2048			# 0x800
	j	$L29
	sw	$2,24($sp)

$L28:
	jal	rtl_del_ct_timer
	move	$4,$16

	beq	$2,$0,$L38
	lw	$7,80($sp)

	lui	$2,%hi(jiffies)
	lw	$5,%lo(jiffies)($2)
	move	$4,$16
	jal	rtl_new_gc_set_ct_timeout_expires
	addu	$5,$21,$5

	jal	rtl_add_ct_timer
	move	$4,$16

	li	$2,2048			# 0x800
	sw	$2,24($sp)
	li	$2,6			# 0x6
	beq	$20,$2,$L30
	li	$2,17			# 0x11

	bne	$20,$2,$L38
	lw	$7,80($sp)

	j	$L37
	nop

$L30:
	lw	$6,92($sp)
	move	$4,$16
	j	$L34
	li	$5,1			# 0x1

$L37:
	jal	rtl_new_gc_get_ct_udp_status
	move	$4,$16

	andi	$2,$2,0x2
	beq	$2,$0,$L32
	move	$4,$16

	li	$5,2			# 0x2
	j	$L34
	li	$6,1			# 0x1

$L32:
	li	$5,2			# 0x2
	move	$6,$0
$L34:
	jal	rtl_list_move_tail
	nop

$L29:
	lw	$7,80($sp)
$L38:
	move	$4,$16
	addiu	$2,$sp,24
	move	$5,$19
	move	$6,$18
	jal	__nf_ct_refresh_acct_proto_hooks
	sw	$2,16($sp)

	lui	$4,%hi(nf_conntrack_lock)
	addiu	$4,$4,%lo(nf_conntrack_lock)
$L35:
	jal	_raw_spin_unlock_bh
	nop

	lw	$31,60($sp)
	lw	$21,56($sp)
	lw	$20,52($sp)
	lw	$19,48($sp)
	lw	$18,44($sp)
	lw	$17,40($sp)
	lw	$16,36($sp)
	j	$31
	addiu	$sp,$sp,64

	.set	macro
	.set	reorder
	.end	__nf_ct_refresh_acct_proto
	.size	__nf_ct_refresh_acct_proto, .-__nf_ct_refresh_acct_proto
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
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	move	$16,$5
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_ip_hdr
	move	$17,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L57
	lw	$31,28($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_skb_protocol
	move	$4,$17
	.set	macro
	.set	reorder

	li	$3,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L42
	li	$3,17			# 0x11
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L43
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L57
	lw	$31,28($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L56
	lui	$2,%hi(prot_counters)
	.set	macro
	.set	reorder

$L42:
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_tcp_state
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$3,%hi(Tcp_State_Hash_Head)
	sll	$2,$2,3
	addiu	$3,$3,%lo(Tcp_State_Hash_Head)
	addu	$2,$3,$2
	lw	$2,4($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L44
	move	$4,$16
	.set	macro
	.set	reorder

	li	$5,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	rtl_list_add_tail
	move	$6,$0
	.set	macro
	.set	reorder

$L44:
	lui	$2,%hi(prot_counters)
	addiu	$2,$2,%lo(prot_counters)
$L48:
 #APP
 # 68 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 4($2)		# atomic_add	
	addu	$3, 1				
	sc	$3, 4($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L57
	lw	$31,28($sp)
	.set	macro
	.set	reorder

	j	$L48
$L43:
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_udp_status
	move	$4,$16
	.set	macro
	.set	reorder

	andi	$2,$2,0x2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L45
	move	$4,$16
	.set	macro
	.set	reorder

	li	$5,2			# 0x2
	.set	noreorder
	.set	nomacro
	j	$L54
	li	$6,1			# 0x1
	.set	macro
	.set	reorder

$L45:
	li	$5,2			# 0x2
	move	$6,$0
$L54:
	jal	rtl_list_add_tail
	lui	$2,%hi(prot_counters)
	addiu	$2,$2,%lo(prot_counters)
$L49:
 #APP
 # 68 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 8($2)		# atomic_add	
	addu	$3, 1				
	sc	$3, 8($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L57
	lw	$31,28($sp)
	.set	macro
	.set	reorder

	j	$L49
$L56:
$L50:
 #APP
 # 68 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, %lo(prot_counters)($2)		# atomic_add	
	addu	$3, 1				
	sc	$3, %lo(prot_counters)($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L50
	lw	$31,28($sp)
	.set	macro
	.set	reorder

$L57:
	lw	$17,24($sp)
	lw	$16,20($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	rtl_connGC_addList
	.size	rtl_connGC_addList, .-rtl_connGC_addList
	.section	.text.rtl_death_action,"ax",@progbits
	.align	2
	.globl	rtl_death_action
	.set	nomips16
	.ent	rtl_death_action
	.type	rtl_death_action, @function
rtl_death_action:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	li	$3,-1			# 0xffffffffffffffff
	lw	$2,104($4)
	move	$16,$4
	addiu	$2,$4,104
$L59:
 #APP
 # 93 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/bitops.h" 1
		ll	$4, 0($2)		# set_bit	
	ins	$4, $3, 9, 1			
	sc	$4, 0($2)				

 # 0 "" 2
 #NO_APP
	beq	$4,$0,$L59
	.set	noreorder
	.set	nomacro
	jal	nf_ct_delete_from_lists
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$16,$0,$L65
	lw	$31,20($sp)
	.set	macro
	.set	reorder

 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
$L61:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($16)	# atomic_sub_return	
	subu	$2, $3, 1			
	sc	$2, 0($16)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$2,$0,$L61
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L62
	lw	$31,20($sp)
	.set	macro
	.set	reorder

	move	$4,$16
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	nf_conntrack_destroy
	addiu	$sp,$sp,24
	.set	macro
	.set	reorder

$L62:
$L65:
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,24
	.set	macro
	.set	reorder

	.end	rtl_death_action
	.size	rtl_death_action, .-rtl_death_action
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
	.set	noreorder
	.set	nomacro
	
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
	lui	$2,%hi(_prot_limit+4)
	lw	$3,%lo(_prot_limit+4)($2)
	lui	$2,%hi(prot_counters+4)
	lw	$2,%lo(prot_counters+4)($2)
	slt	$2,$3,$2
	beq	$2,$0,$L122
	lui	$2,%hi(_prot_limit+8)

	lui	$4,%hi(nf_conntrack_lock)
	addiu	$4,$4,%lo(nf_conntrack_lock)
	lui	$19,%hi(drop_priority)
	jal	_raw_spin_lock_bh
	lui	$23,%hi(Tcp_State_Hash_Head)

	addiu	$19,$19,%lo(drop_priority)
	addiu	$23,$23,%lo(Tcp_State_Hash_Head)
	move	$18,$0
	lui	$22,%hi(drop_priority_max_idx)
	j	$L68
	li	$21,-1			# 0xffffffffffffffff

$L75:
	sltu	$3,$2,10
	beql	$3,$0,$L123
	addiu	$18,$18,1

	sll	$2,$2,3
	addu	$2,$23,$2
	lw	$20,4($2)
	lw	$16,0($20)
	beql	$16,$20,$L123
	addiu	$18,$18,1

	lw	$17,0($16)
$L74:
	addiu	$16,$16,-144
	lb	$2,152($16)
	bne	$2,$21,$L124
	nop

	jal	__conntrack_drop_check
	move	$4,$16

	sb	$2,152($16)
	lb	$2,152($16)
$L124:
	bnel	$2,$0,$L125
	move	$16,$17

	jal	rtl_del_ct_timer
	move	$4,$16

	bnel	$2,$0,$L126
	lui	$4,%hi(nf_conntrack_lock)

	move	$16,$17
$L125:
	beq	$17,$20,$L69
	lw	$2,0($17)

	j	$L74
	move	$17,$2

$L69:
	addiu	$18,$18,1
$L123:
	addiu	$19,$19,4
$L68:
	lw	$2,%lo(drop_priority_max_idx)($22)
	slt	$2,$18,$2
	bnel	$2,$0,$L75
	lbu	$2,0($19)

	lui	$4,%hi(nf_conntrack_lock)
	jal	_raw_spin_unlock_bh
	addiu	$4,$4,%lo(nf_conntrack_lock)

	lui	$2,%hi(_prot_limit+8)
$L122:
	lw	$3,%lo(_prot_limit+8)($2)
	lui	$2,%hi(prot_counters+8)
	lw	$2,%lo(prot_counters+8)($2)
	slt	$2,$3,$2
	beq	$2,$0,$L127
	lui	$4,%hi(nf_conntrack_lock)

	addiu	$4,$4,%lo(nf_conntrack_lock)
	lui	$19,%hi(drop_priority)
	jal	_raw_spin_lock_bh
	lui	$23,%hi(Udp_State_Hash_Head)

	addiu	$19,$19,%lo(drop_priority)
	addiu	$23,$23,%lo(Udp_State_Hash_Head)
	move	$18,$0
	lui	$22,%hi(drop_priority_max_idx)
	j	$L78
	li	$21,-1			# 0xffffffffffffffff

$L84:
	sltu	$3,$2,11
	bnel	$3,$0,$L128
	addiu	$18,$18,1

	addiu	$2,$2,-11
	sll	$2,$2,3
	addu	$2,$23,$2
	lw	$20,4($2)
	lw	$16,0($20)
	beql	$16,$20,$L128
	addiu	$18,$18,1

	lw	$17,0($16)
$L83:
	addiu	$16,$16,-144
	lb	$2,152($16)
	bne	$2,$21,$L129
	nop

	jal	__conntrack_drop_check
	move	$4,$16

	sb	$2,152($16)
	lb	$2,152($16)
$L129:
	bnel	$2,$0,$L130
	move	$16,$17

	jal	rtl_del_ct_timer
	move	$4,$16

	bnel	$2,$0,$L126
	lui	$4,%hi(nf_conntrack_lock)

	move	$16,$17
$L130:
	beq	$17,$20,$L79
	lw	$2,0($17)

	j	$L83
	move	$17,$2

$L79:
	addiu	$18,$18,1
$L128:
	addiu	$19,$19,4
$L78:
	lw	$2,%lo(drop_priority_max_idx)($22)
	slt	$2,$18,$2
	bnel	$2,$0,$L84
	lbu	$2,0($19)

	lui	$4,%hi(nf_conntrack_lock)
	jal	_raw_spin_unlock_bh
	addiu	$4,$4,%lo(nf_conntrack_lock)

	lui	$4,%hi(nf_conntrack_lock)
$L127:
	lui	$18,%hi(drop_priority+2)
	addiu	$4,$4,%lo(nf_conntrack_lock)
	jal	_raw_spin_lock_bh
	addiu	$18,$18,%lo(drop_priority+2)

	move	$19,$18
	move	$17,$0
	li	$fp,-1			# 0xffffffffffffffff
	lui	$23,%hi(jiffies)
	li	$22,2			# 0x2
	j	$L85
	li	$21,12			# 0xc

$L99:
	sltu	$3,$2,10
	beql	$3,$0,$L86
	addiu	$2,$2,-11

	lui	$3,%hi(Tcp_State_Hash_Head)
	sll	$2,$2,3
	addiu	$3,$3,%lo(Tcp_State_Hash_Head)
	addu	$2,$3,$2
	lw	$2,4($2)
	sw	$2,24($sp)
	lw	$16,0($2)
	beql	$16,$2,$L131
	addiu	$17,$17,1

	lw	$20,0($16)
	sw	$0,28($sp)
$L93:
	lw	$3,28($sp)
	addiu	$16,$16,-144
	lb	$2,152($16)
	addiu	$3,$3,1
	bne	$2,$fp,$L88
	sw	$3,28($sp)

	jal	__conntrack_drop_check
	move	$4,$16

	sb	$2,152($16)
$L88:
	lb	$2,152($16)
	bne	$2,$0,$L132
	lw	$3,24($sp)

	jal	rtl_get_ct_timer_expires
	move	$4,$16

	lw	$4,%lo(jiffies)($23)
	lhu	$3,0($19)
	subu	$4,$2,$4
	srl	$4,$4,7
	sltu	$3,$3,$4
	bne	$3,$0,$L133
	lw	$3,28($sp)

	jal	rtl_del_ct_timer
	move	$4,$16

	beq	$2,$0,$L133
	lw	$3,28($sp)

	li	$2,3			# 0x3
	move	$4,$16
	move	$5,$17
	move	$6,$0
	li	$7,1			# 0x1
	jal	__drop_one_conntrack_process_hooks1
	sw	$2,16($sp)

	li	$3,1			# 0x1
	beq	$2,$3,$L126
	lui	$4,%hi(nf_conntrack_lock)

	beq	$2,$22,$L92
	li	$3,3			# 0x3

	move	$4,$16
	move	$5,$17
	move	$6,$0
	li	$7,1			# 0x1
	jal	__drop_one_conntrack_process_hooks2
	sw	$3,16($sp)

	lw	$3,28($sp)
$L133:
	slt	$2,$3,129
	beq	$2,$0,$L87
	lw	$3,24($sp)

$L132:
	beq	$20,$3,$L87
	lw	$2,0($20)

	move	$16,$20
	j	$L93
	move	$20,$2

$L86:
	lui	$3,%hi(Udp_State_Hash_Head)
	sll	$2,$2,3
	addiu	$3,$3,%lo(Udp_State_Hash_Head)
	addu	$2,$3,$2
	lw	$2,4($2)
	sw	$2,24($sp)
	lw	$16,0($2)
	beql	$16,$2,$L131
	addiu	$17,$17,1

	lw	$20,0($16)
	sw	$0,28($sp)
$L98:
	lw	$3,28($sp)
	addiu	$16,$16,-144
	lb	$2,152($16)
	addiu	$3,$3,1
	bne	$2,$fp,$L94
	sw	$3,28($sp)

	jal	__conntrack_drop_check
	move	$4,$16

	sb	$2,152($16)
$L94:
	lb	$2,152($16)
	bne	$2,$0,$L134
	lw	$3,24($sp)

	jal	rtl_get_ct_timer_expires
	move	$4,$16

	lw	$4,%lo(jiffies)($23)
	lhu	$3,0($19)
	subu	$4,$2,$4
	srl	$4,$4,7
	sltu	$3,$3,$4
	bne	$3,$0,$L135
	lw	$3,28($sp)

	jal	rtl_del_ct_timer
	move	$4,$16

	beq	$2,$0,$L135
	lw	$3,28($sp)

	move	$4,$16
	move	$5,$17
	move	$6,$0
	li	$7,1			# 0x1
	jal	__drop_one_conntrack_process_hooks1
	sw	$21,16($sp)

	li	$3,1			# 0x1
	beq	$2,$3,$L126
	lui	$4,%hi(nf_conntrack_lock)

	beq	$2,$22,$L92
	move	$4,$16

	move	$5,$17
	move	$6,$0
	li	$7,1			# 0x1
	jal	__drop_one_conntrack_process_hooks2
	sw	$21,16($sp)

	lw	$3,28($sp)
$L135:
	slt	$2,$3,129
	beq	$2,$0,$L87
	lw	$3,24($sp)

$L134:
	beq	$20,$3,$L87
	lw	$2,0($20)

	move	$16,$20
	j	$L98
	move	$20,$2

$L87:
	addiu	$17,$17,1
$L131:
	addiu	$19,$19,4
$L85:
	lui	$3,%hi(drop_priority_max_idx)
	lw	$2,%lo(drop_priority_max_idx)($3)
	slt	$2,$17,$2
	bnel	$2,$0,$L99
	lbu	$2,-2($19)

	lui	$16,%hi(nf_conntrack_lock)
	jal	_raw_spin_unlock_bh
	addiu	$4,$16,%lo(nf_conntrack_lock)

	jal	_raw_spin_lock_bh
	addiu	$4,$16,%lo(nf_conntrack_lock)

	move	$17,$0
	li	$fp,-1			# 0xffffffffffffffff
	lui	$23,%hi(jiffies)
	li	$22,1			# 0x1
	li	$21,12			# 0xc
	j	$L100
	li	$20,3			# 0x3

$L113:
	sltu	$3,$2,10
	beql	$3,$0,$L101
	addiu	$2,$2,-11

	lui	$3,%hi(Tcp_State_Hash_Head)
	sll	$2,$2,3
	addiu	$3,$3,%lo(Tcp_State_Hash_Head)
	addu	$2,$3,$2
	lw	$2,4($2)
	sw	$2,24($sp)
	lw	$16,0($2)
	beql	$16,$2,$L136
	addiu	$17,$17,1

	lw	$19,0($16)
	sw	$0,28($sp)
$L107:
	lw	$3,28($sp)
	addiu	$16,$16,-144
	lb	$2,152($16)
	addiu	$3,$3,1
	bne	$2,$fp,$L103
	sw	$3,28($sp)

	jal	__conntrack_drop_check
	move	$4,$16

	sb	$2,152($16)
$L103:
	lb	$2,152($16)
	bne	$2,$0,$L137
	lw	$3,24($sp)

	jal	rtl_get_ct_timer_expires
	move	$4,$16

	lw	$4,%lo(jiffies)($23)
	lhu	$3,0($18)
	subu	$4,$2,$4
	srl	$4,$4,9
	sltu	$3,$3,$4
	bne	$3,$0,$L138
	lw	$3,28($sp)

	jal	rtl_del_ct_timer
	move	$4,$16

	beq	$2,$0,$L138
	lw	$3,28($sp)

	move	$4,$16
	move	$5,$17
	li	$6,2			# 0x2
	move	$7,$0
	jal	__drop_one_conntrack_process_hooks1
	sw	$20,16($sp)

	beq	$2,$22,$L117
	li	$3,2			# 0x2

	beq	$2,$3,$L92
	move	$4,$16

	move	$5,$17
	li	$6,2			# 0x2
	move	$7,$0
	jal	__drop_one_conntrack_process_hooks2
	sw	$20,16($sp)

	lw	$3,28($sp)
$L138:
	slt	$2,$3,129
	beq	$2,$0,$L102
	lw	$3,24($sp)

$L137:
	beq	$19,$3,$L102
	lw	$2,0($19)

	move	$16,$19
	j	$L107
	move	$19,$2

$L101:
	lui	$3,%hi(Udp_State_Hash_Head)
	sll	$2,$2,3
	addiu	$3,$3,%lo(Udp_State_Hash_Head)
	addu	$2,$3,$2
	lw	$2,4($2)
	sw	$2,24($sp)
	lw	$16,0($2)
	beql	$16,$2,$L136
	addiu	$17,$17,1

	lw	$19,0($16)
	sw	$0,28($sp)
$L112:
	lw	$3,28($sp)
	addiu	$16,$16,-144
	lb	$2,152($16)
	addiu	$3,$3,1
	bne	$2,$fp,$L108
	sw	$3,28($sp)

	jal	__conntrack_drop_check
	move	$4,$16

	sb	$2,152($16)
$L108:
	lb	$2,152($16)
	bne	$2,$0,$L139
	lw	$3,24($sp)

	jal	rtl_get_ct_timer_expires
	move	$4,$16

	lw	$4,%lo(jiffies)($23)
	lhu	$3,0($18)
	subu	$4,$2,$4
	srl	$4,$4,9
	sltu	$3,$3,$4
	bne	$3,$0,$L140
	lw	$3,28($sp)

	jal	rtl_del_ct_timer
	move	$4,$16

	beq	$2,$0,$L140
	lw	$3,28($sp)

	move	$4,$16
	move	$5,$17
	li	$6,2			# 0x2
	move	$7,$0
	jal	__drop_one_conntrack_process_hooks1
	sw	$21,16($sp)

	bne	$2,$22,$L111
	li	$3,2			# 0x2

$L117:
	lui	$4,%hi(nf_conntrack_lock)
$L126:
	jal	_raw_spin_unlock_bh
	addiu	$4,$4,%lo(nf_conntrack_lock)

	jal	rtl_death_action
	move	$4,$16

	j	$L120
	lui	$2,%hi(rtl_newGC_session_status_flags)

$L111:
	beq	$2,$3,$L92
	move	$4,$16

	move	$5,$17
	li	$6,2			# 0x2
	move	$7,$0
	jal	__drop_one_conntrack_process_hooks2
	sw	$21,16($sp)

	lw	$3,28($sp)
$L140:
	slt	$2,$3,129
	beq	$2,$0,$L102
	lw	$3,24($sp)

$L139:
	beq	$19,$3,$L102
	lw	$2,0($19)

	move	$16,$19
	j	$L112
	move	$19,$2

$L102:
	addiu	$17,$17,1
$L136:
	addiu	$18,$18,4
$L100:
	lui	$3,%hi(drop_priority_max_idx)
	lw	$2,%lo(drop_priority_max_idx)($3)
	slt	$2,$17,$2
	bnel	$2,$0,$L113
	lbu	$2,-2($18)

	lui	$4,%hi(nf_conntrack_lock)
	jal	_raw_spin_unlock_bh
	addiu	$4,$4,%lo(nf_conntrack_lock)

	lui	$3,%hi(rtl_newGC_session_status_flags)
	lw	$5,%lo(rtl_newGC_session_status_flags)($3)
	li	$4,1			# 0x1
	bne	$5,$4,$L119
	move	$2,$0

	j	$L121
	lw	$31,68($sp)

$L92:
	lui	$2,%hi(rtl_newGC_session_status_flags)
$L120:
	lw	$3,%lo(rtl_newGC_session_status_flags)($2)
	bnel	$3,$0,$L76
	li	$2,1			# 0x1

	lui	$3,%hi(jiffies)
	lw	$3,%lo(jiffies)($3)
	li	$4,3			# 0x3
	sw	$4,%lo(rtl_newGC_session_status_flags)($2)
	addiu	$3,$3,1
	lui	$2,%hi(rtl_newGC_session_status_time)
	j	$L73
	sw	$3,%lo(rtl_newGC_session_status_time)($2)

$L119:
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	sw	$4,%lo(rtl_newGC_session_status_flags)($3)
	addiu	$3,$2,200
	lui	$2,%hi(rtl_newGC_session_status_time)
	sw	$3,%lo(rtl_newGC_session_status_time)($2)
	j	$L76
	move	$2,$0

$L73:
	li	$2,1			# 0x1
$L76:
	lw	$31,68($sp)
$L121:
	lw	$fp,64($sp)
	lw	$23,60($sp)
	lw	$22,56($sp)
	lw	$21,52($sp)
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	j	$31
	addiu	$sp,$sp,72

	.set	macro
	.set	reorder
	.end	drop_one_conntrack
	.size	drop_one_conntrack, .-drop_one_conntrack
	.section	.text.clean_from_lists,"ax",@progbits
	.align	2
	.globl	clean_from_lists
	.set	nomips16
	.ent	clean_from_lists
	.type	clean_from_lists, @function
clean_from_lists:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	move	$16,$4
	move	$17,$5
	.set	noreorder
	.set	nomacro
	jal	rtl_hlist_nulls_del_rcu
	move	$5,$0
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_hlist_nulls_del_rcu
	li	$5,1			# 0x1
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0
	.set	macro
	.set	reorder

	li	$3,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L144
	li	$3,17			# 0x11
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L145
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L158
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L157
	lui	$2,%hi(prot_counters)
	.set	macro
	.set	reorder

$L144:
	lui	$2,%hi(prot_counters)
	addiu	$2,$2,%lo(prot_counters)
$L150:
 #APP
 # 111 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 4($2)		# atomic_sub	
	subu	$3, 1				
	sc	$3, 4($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L158
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L150
$L145:
	lui	$2,%hi(prot_counters)
	addiu	$2,$2,%lo(prot_counters)
$L151:
 #APP
 # 111 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 8($2)		# atomic_sub	
	subu	$3, 1				
	sc	$3, 8($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L158
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L151
$L157:
$L152:
 #APP
 # 111 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, %lo(prot_counters)($2)		# atomic_sub	
	subu	$3, 1				
	sc	$3, %lo(prot_counters)($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L152
	move	$4,$16
	.set	macro
	.set	reorder

$L158:
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0
	.set	macro
	.set	reorder

	li	$3,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L146
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0
	.set	macro
	.set	reorder

	li	$3,17			# 0x11
	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L159
	move	$4,$16
	.set	macro
	.set	reorder

$L146:
	move	$5,$17
	.set	noreorder
	.set	nomacro
	jal	clean_from_lists_hooks
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_list_del
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$2,%hi(nf_conntrack_max)
	lw	$2,%lo(nf_conntrack_max)($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L160
	lui	$2,%hi(rtl_newGC_session_status_flags)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_gc_threshold_check
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L148
	lui	$2,%hi(rtl_newGC_session_status_flags)
	.set	macro
	.set	reorder

	lw	$4,%lo(rtl_newGC_session_status_flags)($2)
	li	$3,3			# 0x3
	.set	noreorder
	.set	nomacro
	beq	$4,$3,$L147
	lui	$4,%hi(jiffies)
	.set	macro
	.set	reorder

	lw	$4,%lo(jiffies)($4)
	sw	$3,%lo(rtl_newGC_session_status_flags)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$4,1
	.set	noreorder
	.set	nomacro
	j	$L147
	sw	$3,%lo(rtl_newGC_session_status_time)($2)
	.set	macro
	.set	reorder

$L148:
$L160:
	sw	$0,%lo(rtl_newGC_session_status_flags)($2)
$L147:
	move	$4,$16
$L159:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	.set	noreorder
	.set	nomacro
	j	nf_ct_remove_expectations
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	clean_from_lists
	.size	clean_from_lists, .-clean_from_lists
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

	bne	$2,$0,$L162
	move	$17,$2

	beq	$16,$0,$L162
	lui	$8,%hi(_prot_limit)

	lui	$7,%hi(prot_limit)
	addiu	$8,$8,%lo(_prot_limit)
	addiu	$7,$7,%lo(prot_limit)
	move	$3,$0
	lui	$6,%hi(nf_conntrack_max)
	li	$4,100			# 0x64
	li	$5,12			# 0xc
$L163:
	addu	$2,$7,$3
	lw	$10,0($2)
	lw	$2,%lo(nf_conntrack_max)($6)
	addu	$9,$8,$3
	addiu	$3,$3,4
	mul	$10,$10,$2
	divu	$0,$10,$4
	mflo	$10
	bne	$3,$5,$L163
	sw	$10,0($9)

	sll	$5,$2,2
	li	$3,5			# 0x5
	divu	$0,$5,$3
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	mflo	$5
	subu	$3,$2,$5
	sltu	$3,$3,65
	bne	$3,$0,$L164
	sw	$5,%lo(rtl_nf_conntrack_threshold)($4)

	addiu	$3,$2,-64
	sw	$3,%lo(rtl_nf_conntrack_threshold)($4)
$L164:
	beq	$2,$0,$L168
	lui	$2,%hi(rtl_newGC_session_status_flags)

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L165
	lui	$2,%hi(rtl_newGC_session_status_flags)

	lw	$4,%lo(rtl_newGC_session_status_flags)($2)
	li	$3,3			# 0x3
	beq	$4,$3,$L162
	lui	$4,%hi(jiffies)

	lw	$4,%lo(jiffies)($4)
	sw	$3,%lo(rtl_newGC_session_status_flags)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$4,1
	j	$L162
	sw	$3,%lo(rtl_newGC_session_status_time)($2)

$L165:
$L168:
	sw	$0,%lo(rtl_newGC_session_status_flags)($2)
$L162:
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

	bne	$2,$0,$L170
	move	$17,$2

	beq	$16,$0,$L170
	lui	$8,%hi(_prot_limit)

	lui	$7,%hi(prot_limit)
	addiu	$8,$8,%lo(_prot_limit)
	addiu	$7,$7,%lo(prot_limit)
	move	$3,$0
	lui	$6,%hi(nf_conntrack_max)
	li	$4,100			# 0x64
	li	$5,12			# 0xc
$L171:
	addu	$2,$7,$3
	lw	$10,0($2)
	lw	$2,%lo(nf_conntrack_max)($6)
	addu	$9,$8,$3
	addiu	$3,$3,4
	mul	$10,$10,$2
	divu	$0,$10,$4
	mflo	$10
	bne	$3,$5,$L171
	sw	$10,0($9)

	sll	$5,$2,2
	li	$3,5			# 0x5
	divu	$0,$5,$3
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	mflo	$5
	subu	$3,$2,$5
	sltu	$3,$3,65
	bne	$3,$0,$L172
	sw	$5,%lo(rtl_nf_conntrack_threshold)($4)

	addiu	$3,$2,-64
	sw	$3,%lo(rtl_nf_conntrack_threshold)($4)
$L172:
	beq	$2,$0,$L176
	lui	$2,%hi(rtl_newGC_session_status_flags)

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L173
	lui	$2,%hi(rtl_newGC_session_status_flags)

	lw	$4,%lo(rtl_newGC_session_status_flags)($2)
	li	$3,3			# 0x3
	beq	$4,$3,$L170
	lui	$4,%hi(jiffies)

	lw	$4,%lo(jiffies)($4)
	sw	$3,%lo(rtl_newGC_session_status_flags)($2)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$4,1
	j	$L170
	sw	$3,%lo(rtl_newGC_session_status_time)($2)

$L173:
$L176:
	sw	$0,%lo(rtl_newGC_session_status_flags)($2)
$L170:
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
	.globl	conntrack_min
	.section	.bss.conntrack_min,"aw",@nobits
	.align	2
	.type	conntrack_min, @object
	.size	conntrack_min, 4
conntrack_min:
	.space	4
	.globl	conntrack_max
	.section	.data.conntrack_max,"aw",@progbits
	.align	2
	.type	conntrack_max, @object
	.size	conntrack_max, 4
conntrack_max:
	.word	100
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
	.section	.bss.prot_counters,"aw",@nobits
	.align	2
	.type	prot_counters, @object
	.size	prot_counters, 12
prot_counters:
	.space	12
	.section	.bss._prot_limit,"aw",@nobits
	.align	2
	.type	_prot_limit, @object
	.size	_prot_limit, 12
_prot_limit:
	.space	12
	.globl	drop_priority_max_idx
	.section	.bss.drop_priority_max_idx,"aw",@nobits
	.align	2
	.type	drop_priority_max_idx, @object
	.size	drop_priority_max_idx, 4
drop_priority_max_idx:
	.space	4
	.globl	rtl_newGC_session_status_flags
	.section	.bss.rtl_newGC_session_status_flags,"aw",@nobits
	.align	2
	.type	rtl_newGC_session_status_flags, @object
	.size	rtl_newGC_session_status_flags, 4
rtl_newGC_session_status_flags:
	.space	4
	.globl	rtl_newGC_session_status_time
	.section	.bss.rtl_newGC_session_status_time,"aw",@nobits
	.align	2
	.type	rtl_newGC_session_status_time, @object
	.size	rtl_newGC_session_status_time, 4
rtl_newGC_session_status_time:
	.space	4
	.globl	prot_limit
	.section	.bss.prot_limit,"aw",@nobits
	.align	2
	.type	prot_limit, @object
	.size	prot_limit, 12
prot_limit:
	.space	12
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
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 1459) 4.4.7"
