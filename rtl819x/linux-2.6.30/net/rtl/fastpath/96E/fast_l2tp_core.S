	.file	1 "fast_l2tp_core.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
#APP
	.macro _ssnop; sll $0, $0, 1; .endm
	.macro _ehb; sll $0, $0, 3; .endm
	.macro mtc0_tlbw_hazard; nop; nop; .endm
	.macro tlbw_use_hazard; nop; nop; nop; .endm
	.macro tlb_probe_hazard; nop; nop; nop; .endm
	.macro irq_enable_hazard; _ssnop; _ssnop; _ssnop;; .endm
	.macro irq_disable_hazard; nop; nop; nop; .endm
	.macro back_to_back_c0_hazard; _ssnop; _ssnop; _ssnop;; .endm
		.macro	raw_local_irq_enable				
	.set	push						
	.set	reorder						
	.set	noat						
	mfc0	$1,$12						
	ori	$1,0x1f						
	xori	$1,0x1e						
	mtc0	$1,$12						
	irq_enable_hazard					
	.set	pop						
	.endm
		.macro	raw_local_irq_disable
	.set	push						
	.set	noat						
	mfc0	$1,$12						
	ori	$1,0x1f						
	xori	$1,0x1f						
	.set	noreorder					
	mtc0	$1,$12						
	irq_disable_hazard					
	.set	pop						
	.endm							

		.macro	raw_local_save_flags flags			
	.set	push						
	.set	reorder						
	mfc0	\flags, $12					
	.set	pop						
	.endm							

		.macro	raw_local_irq_save result			
	.set	push						
	.set	reorder						
	.set	noat						
	mfc0	\result, $12					
	ori	$1, \result, 0x1f				
	xori	$1, 0x1f					
	.set	noreorder					
	mtc0	$1, $12						
	irq_disable_hazard					
	.set	pop						
	.endm							

		.macro	raw_local_irq_restore flags			
	.set	push						
	.set	noreorder					
	.set	noat						
	mfc0	$1, $12						
	andi	\flags, 1					
	ori	$1, 0x1f					
	xori	$1, 0x1f					
	or	\flags, $1					
	mtc0	\flags, $12					
	irq_disable_hazard					
	.set	pop						
	.endm							

#NO_APP
	.section	.text.get_fast_l2tp_lastxmit,"ax",@progbits
	.align	2
	.globl	get_fast_l2tp_lastxmit
	.set	nomips16
	.ent	get_fast_l2tp_lastxmit
	.type	get_fast_l2tp_lastxmit, @function
get_fast_l2tp_lastxmit:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(l2tpInfo)
	addiu	$2,$2,%lo(l2tpInfo)
	lhu	$4,38($2)
	lw	$3,8($2)
	move	$2,$0
	xori	$4,$4,0x1
	j	$31
	movz	$2,$3,$4

	.set	macro
	.set	reorder
	.end	get_fast_l2tp_lastxmit
	.size	get_fast_l2tp_lastxmit, .-get_fast_l2tp_lastxmit
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"fast_l2tp\000"
	.section	.exit.text,"ax",@progbits
	.align	2
	.globl	fast_l2tp_exit
	.set	nomips16
	.ent	fast_l2tp_exit
	.type	fast_l2tp_exit, @function
fast_l2tp_exit:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$16,%hi(res1)
	lw	$5,%lo(res1)($16)
	nop
	beq	$5,$0,$L7
	nop

	lui	$4,%hi($LC0)
	jal	remove_proc_entry
	addiu	$4,$4,%lo($LC0)

	sw	$0,%lo(res1)($16)
$L7:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fast_l2tp_exit
	.size	fast_l2tp_exit, .-fast_l2tp_exit
	.section	.init.text,"ax",@progbits
	.align	2
	.globl	fast_l2tp_init
	.set	nomips16
	.ent	fast_l2tp_init
	.type	fast_l2tp_init, @function
fast_l2tp_init:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	move	$5,$0
	jal	create_proc_entry
	move	$6,$0

	lui	$3,%hi(res1)
	beq	$2,$0,$L10
	sw	$2,%lo(res1)($3)

	lui	$3,%hi(l2tp_write_proc)
	addiu	$3,$3,%lo(l2tp_write_proc)
	sw	$3,68($2)
	lui	$3,%hi(l2tp_read_proc)
	addiu	$3,$3,%lo(l2tp_read_proc)
	sw	$3,64($2)
$L10:
	lui	$2,%hi(l2tp_tx_id_hook)
	sw	$0,%lo(l2tp_tx_id_hook)($2)
	lui	$4,%hi(l2tpInfo)
	lui	$2,%hi(fast_l2tp_fw)
	addiu	$4,$4,%lo(l2tpInfo)
	move	$5,$0
	li	$6,40			# 0x28
	sw	$0,%lo(fast_l2tp_fw)($2)
	lui	$2,%hi(state)
	jal	memset
	sw	$0,%lo(state)($2)

	lw	$31,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fast_l2tp_init
	.size	fast_l2tp_init, .-fast_l2tp_init
	.section	.text.l2tp_write_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	l2tp_write_proc
	.type	l2tp_write_proc, @function
l2tp_write_proc:
	.frame	$sp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$16,24($sp)
	sltu	$3,$6,2
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L13
	move	$16,$6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L16
	li	$2,-14			# 0xfffffff2
	.set	macro
	.set	reorder

	lw	$3,24($28)
	addiu	$4,$5,1
	or	$2,$4,$5
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L13
	addiu	$4,$sp,16
	.set	macro
	.set	reorder

	li	$6,1			# 0x1
#APP
 # 600 "net/rtl/fastpath/fast_l2tp_core.c" 1
	.set	noreorder
	jal	__copy_user
	.set	noat
	addu	$1, $5, $6
	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
	.set	noreorder
	.set	nomacro
	bne	$6,$0,$L13
	lui	$2,%hi(fast_l2tp_fw)
	.set	macro
	.set	reorder

	lb	$3,16($sp)
	li	$4,1			# 0x1
	addiu	$3,$3,-48
	sw	$3,%lo(fast_l2tp_fw)($2)
	.set	noreorder
	.set	nomacro
	bne	$3,$4,$L14
	lui	$2,%hi(l2tp_tx_id_hook)
	.set	macro
	.set	reorder

	lui	$3,%hi(l2tp_tx_id)
	addiu	$3,$3,%lo(l2tp_tx_id)
	.set	noreorder
	.set	nomacro
	j	$L15
	sw	$3,%lo(l2tp_tx_id_hook)($2)
	.set	macro
	.set	reorder

$L14:
	sw	$0,%lo(l2tp_tx_id_hook)($2)
	lui	$2,%hi(l2tpInfo+20)
	sh	$0,%lo(l2tpInfo+20)($2)
$L15:
	lui	$4,%hi(l2tpInfo)
	addiu	$4,$4,%lo(l2tpInfo)
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,40			# 0x28
	.set	macro
	.set	reorder

	lui	$2,%hi(state)
	sw	$0,%lo(state)($2)
	.set	noreorder
	.set	nomacro
	j	$L16
	move	$2,$16
	.set	macro
	.set	reorder

$L13:
	li	$2,-14			# 0xfffffff2
$L16:
	lw	$31,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	l2tp_write_proc
	.size	l2tp_write_proc, .-l2tp_write_proc
	.section	.rodata.str1.4
	.align	2
$LC1:
	.ascii	"%c\012\000"
	.section	.text.l2tp_read_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	l2tp_read_proc
	.type	l2tp_read_proc, @function
l2tp_read_proc:
	.frame	$sp,40,$31		# vars= 8, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	lui	$2,%hi(fast_l2tp_fw)
	lw	$2,%lo(fast_l2tp_fw)($2)
	move	$17,$5
	lui	$5,%hi($LC1)
	move	$16,$6
	addiu	$5,$5,%lo($LC1)
	addiu	$6,$2,48
	sw	$7,16($sp)
	jal	sprintf
	move	$18,$4

	lw	$7,16($sp)
	nop
	addu	$3,$7,$16
	slt	$3,$3,$2
	bne	$3,$0,$L19
	li	$4,1			# 0x1

	lw	$3,56($sp)
	nop
	sw	$4,0($3)
$L19:
	subu	$2,$2,$16
	slt	$3,$7,$2
	addu	$16,$18,$16
	sw	$16,0($17)
	movz	$7,$2,$3
	slt	$2,$7,0
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	movn	$7,$0,$2
	move	$2,$7
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	l2tp_read_proc
	.size	l2tp_read_proc, .-l2tp_read_proc
	.section	.text.check_for_fast_l2tp_to_wan,"ax",@progbits
	.align	2
	.globl	check_for_fast_l2tp_to_wan
	.set	nomips16
	.ent	check_for_fast_l2tp_to_wan
	.type	check_for_fast_l2tp_to_wan, @function
check_for_fast_l2tp_to_wan:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	jal	rtl_ip_hdr
	move	$16,$4

	lbu	$4,9($2)
	li	$3,1			# 0x1
	beq	$4,$3,$L23
	move	$2,$0

	jal	rtl_ip_hdr
	move	$4,$16

	lw	$3,12($2)
	li	$2,-256			# 0xffffff00
	and	$2,$3,$2
	li	$3,-172032000			# 0xf5bf0000
	ori	$3,$3,0xc000
	addu	$2,$2,$3
	sltu	$2,$0,$2
$L23:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	check_for_fast_l2tp_to_wan
	.size	check_for_fast_l2tp_to_wan, .-check_for_fast_l2tp_to_wan
	.section	.text.fast_l2tp_to_wan,"ax",@progbits
	.align	2
	.globl	fast_l2tp_to_wan
	.set	nomips16
	.ent	fast_l2tp_to_wan
	.type	fast_l2tp_to_wan, @function
fast_l2tp_to_wan:
	.frame	$sp,88,$31		# vars= 48, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-88
	sw	$31,84($sp)
	sw	$19,80($sp)
	sw	$18,76($sp)
	sw	$17,72($sp)
	sw	$16,68($sp)
	lui	$2,%hi(fast_l2tp_fw)
	lw	$2,%lo(fast_l2tp_fw)($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L26
	move	$16,$4
	.set	macro
	.set	reorder

	lui	$3,%hi(l2tpInfo)
	addiu	$2,$3,%lo(l2tpInfo)
	lhu	$5,20($2)
	#nop
	beq	$5,$0,$L26
	lhu	$5,22($2)
	#nop
	beq	$5,$0,$L26
	lw	$3,%lo(l2tpInfo)($3)
	#nop
	beq	$3,$0,$L26
	lhu	$3,38($2)
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L33
	move	$2,$0
	.set	macro
	.set	reorder

	jal	rtl_ip_hdr
	move	$4,$16
	lhu	$19,2($2)
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_headroom
	move	$17,$2
	.set	macro
	.set	reorder

	sltu	$2,$2,52
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L37
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_cloned
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L37
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_shared
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L38
	li	$5,52			# 0x34
	.set	macro
	.set	reorder

	move	$4,$16
$L37:
	.set	noreorder
	.set	nomacro
	jal	skb_realloc_headroom
	li	$5,52			# 0x34
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L26
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	consume_skb
	sw	$2,56($sp)
	.set	macro
	.set	reorder

	lw	$2,56($sp)
	#nop
	move	$16,$2
	li	$5,52			# 0x34
$L38:
	lbu	$17,1($17)
	.set	noreorder
	.set	nomacro
	jal	skb_push
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$5,%hi(l2tpInfo+24)
	move	$4,$2
	addiu	$5,$5,%lo(l2tpInfo+24)
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,14			# 0xe
	.set	macro
	.set	reorder

	lw	$2,36($sp)
	li	$6,268369920			# 0xfff0000
	ori	$6,$6,0xffff
	and	$6,$2,$6
	lui	$3,%hi(l2tpInfo)
	li	$2,1073741824			# 0x40000000
	li	$4,-251723776			# 0xf0ff0000
	addiu	$3,$3,%lo(l2tpInfo)
	or	$6,$6,$2
	ori	$4,$4,0xffff
	lw	$2,16($3)
	and	$6,$6,$4
	li	$4,83886080			# 0x5000000
	lw	$3,12($3)
	or	$6,$6,$4
	sw	$6,36($sp)
	li	$6,16384			# 0x4000
	move	$5,$0
	move	$4,$16
	sh	$6,42($sp)
	sw	$2,48($sp)
	li	$6,17			# 0x11
	li	$2,64			# 0x40
	sb	$6,45($sp)
	sw	$3,52($sp)
	sb	$2,44($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$17,37($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	move	$4,$16
	.set	macro
	.set	reorder

	addiu	$2,$2,-14
	sh	$2,38($sp)
	addiu	$3,$sp,36
	sh	$0,40($sp)
	sh	$0,46($sp)
	lw	$7,4($3)
	lw	$6,36($sp)
	lw	$4,8($3)
	lw	$5,48($sp)
	addu	$8,$7,$6
	sltu	$7,$8,$7
	addu	$8,$8,$4
	addu	$7,$8,$7
	sltu	$4,$7,$4
	addu	$7,$7,$5
	addu	$4,$7,$4
	srl	$6,$6,22
	andi	$6,$6,0x3c
	sltu	$2,$4,$5
	addu	$6,$3,$6
	addu	$2,$2,$4
	addiu	$3,$sp,52
$L29:
	lw	$4,0($3)
	addiu	$3,$3,4
	addu	$2,$2,$4
	sltu	$4,$2,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L29
	addu	$2,$4,$2
	.set	macro
	.set	reorder

	move	$4,$16
#APP
 # 83 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
		.set	push		# csum_fold
	.set	noat		
	sll	$1, $2, 16	
	addu	$2, $1		
	sltu	$1, $2, $1	
	srl	$2, $2, 16	
	addu	$2, $1		
	xori	$2, 0xffff	
	.set	pop
 # 0 "" 2
#NO_APP
	lui	$18,%hi(l2tpInfo)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sh	$2,46($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,14
	addiu	$5,$sp,36
	li	$6,20			# 0x14
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$17,$18,%lo(l2tpInfo)
	.set	macro
	.set	reorder

	lhu	$3,22($17)
	lhu	$5,20($17)
	li	$2,1701			# 0x6a5
	sh	$3,28($sp)
	li	$3,-253			# 0xffffff03
	li	$6,2			# 0x2
	move	$4,$16
	sh	$3,30($sp)
	addiu	$19,$19,18
	li	$3,33			# 0x21
	sh	$3,32($sp)
	sh	$6,24($sp)
	sh	$5,26($sp)
	sh	$2,18($sp)
	sh	$19,20($sp)
	sh	$2,16($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sh	$0,22($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,34
	li	$6,18			# 0x12
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,16
	.set	macro
	.set	reorder

	lw	$5,%lo(l2tpInfo)($18)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dev
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	timeoutCheck_skipp_pkt
	addiu	$4,$2,52
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L30
	lui	$2,%hi(jiffies)
	.set	macro
	.set	reorder

	lw	$2,%lo(jiffies)($2)
	#nop
	sw	$2,8($17)
$L30:
	lui	$17,%hi(l2tpInfo)
	addiu	$17,$17,%lo(l2tpInfo)
	lw	$4,4($17)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L39
	lui	$2,%hi(gQosEnabled)
	.set	macro
	.set	reorder

	jal	rtl_get_ppp_dev_priv
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L39
	lui	$2,%hi(gQosEnabled)
	.set	macro
	.set	reorder

	lw	$4,4($17)
	jal	rtl_get_ppp_dev_priv
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	move	$17,$2
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_inc_ppp_stats
	li	$5,1			# 0x1
	.set	macro
	.set	reorder

	lui	$2,%hi(gQosEnabled)
$L39:
	lw	$2,%lo(gQosEnabled)($2)
	#nop
	beq	$2,$0,$L32
	.set	noreorder
	.set	nomacro
	jal	dev_queue_xmit
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L33
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L32:
	.set	noreorder
	.set	nomacro
	jal	rtl_call_skb_ndo_start_xmit
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L33
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L26:
	move	$2,$0
$L33:
	lw	$31,84($sp)
	lw	$19,80($sp)
	lw	$18,76($sp)
	lw	$17,72($sp)
	lw	$16,68($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,88
	.set	macro
	.set	reorder

	.end	fast_l2tp_to_wan
	.size	fast_l2tp_to_wan, .-fast_l2tp_to_wan
	.section	.text.set_l2tp_device,"ax",@progbits
	.align	2
	.globl	set_l2tp_device
	.set	nomips16
	.ent	set_l2tp_device
	.type	set_l2tp_device, @function
set_l2tp_device:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$2,%hi(fast_l2tp_fw)
	lw	$2,%lo(fast_l2tp_fw)($2)
	nop
	beq	$2,$0,$L42
	nop

	jal	rtl_get_dev_by_name
	nop

	lui	$3,%hi(l2tpInfo+4)
	sw	$2,%lo(l2tpInfo+4)($3)
$L42:
	lw	$31,20($sp)
	nop
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	set_l2tp_device
	.size	set_l2tp_device, .-set_l2tp_device
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	"ppp0\000"
	.align	2
$LC3:
	.ascii	"eth1\000"
	.section	.text.l2tp_tx_id,"ax",@progbits
	.align	2
	.globl	l2tp_tx_id
	.set	nomips16
	.ent	l2tp_tx_id
	.type	l2tp_tx_id, @function
l2tp_tx_id:
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
	jal	rtl_get_skb_data
	move	$17,$4

	move	$4,$17
	jal	rtl_ip_hdr
	move	$16,$2

	lbu	$18,0($2)
	li	$4,1701			# 0x6a5
	andi	$18,$18,0xf
	sll	$18,$18,2
	addu	$18,$2,$18
	lhu	$3,0($18)
	nop
	bne	$3,$4,$L57
	nop

	lhu	$5,2($18)
	nop
	bne	$5,$3,$L57
	addiu	$4,$18,8

	lhu	$3,8($18)
	nop
	sll	$5,$3,16
	sra	$5,$5,16
	bgez	$5,$L45
	andi	$3,$3,0x4000

	bne	$3,$0,$L46
	nop

	lhu	$5,2($4)
	j	$L47
	addiu	$18,$18,10

$L46:
	lhu	$5,4($4)
	addiu	$18,$18,12
$L47:
	lhu	$3,14($18)
	li	$4,4			# 0x4
	bne	$3,$4,$L48
	li	$4,11			# 0xb

	lui	$4,%hi(l2tpInfo)
	addiu	$4,$4,%lo(l2tpInfo)
	lhu	$2,20($4)
	nop
	bne	$2,$5,$L57
	nop

	move	$5,$0
	jal	memset
	li	$6,40			# 0x28

	lui	$4,%hi($LC2)
	addiu	$4,$4,%lo($LC2)
	jal	rtl865x_setNetifType
	li	$5,2			# 0x2

	lui	$2,%hi(state)
	j	$L57
	sw	$0,%lo(state)($2)

$L48:
	bne	$3,$4,$L49
	li	$4,12			# 0xc

	lui	$18,%hi(state)
	lw	$2,%lo(state)($18)
	nop
	bne	$2,$0,$L57
	nop

	jal	rtl_get_skb_dev_name
	move	$4,$17

	lui	$5,%hi($LC3)
	move	$4,$2
	addiu	$5,$5,%lo($LC3)
	jal	memcmp
	li	$6,4			# 0x4

	bne	$2,$0,$L57
	nop

	jal	rtl_get_skb_dev
	move	$4,$17

	lui	$4,%hi(l2tpInfo+24)
	lui	$3,%hi(l2tpInfo)
	addiu	$4,$4,%lo(l2tpInfo+24)
	addiu	$5,$16,-8
	li	$6,6			# 0x6
	jal	memcpy
	sw	$2,%lo(l2tpInfo)($3)

	lui	$4,%hi(l2tpInfo+30)
	addiu	$4,$4,%lo(l2tpInfo+30)
	addiu	$5,$16,-14
	jal	memcpy
	li	$6,6			# 0x6

	lui	$4,%hi(l2tpInfo+36)
	addiu	$4,$4,%lo(l2tpInfo+36)
	addiu	$5,$16,-2
	jal	memcpy
	li	$6,2			# 0x2

	li	$2,1			# 0x1
	j	$L57
	sw	$2,%lo(state)($18)

$L49:
	bne	$3,$4,$L57
	nop

	lhu	$4,2($18)
	nop
	beq	$4,$0,$L57
	addiu	$3,$18,2

	lui	$16,%hi(state)
	lw	$4,%lo(state)($16)
	nop
	sltu	$4,$4,2
	beq	$4,$0,$L57
	lui	$17,%hi(l2tpInfo)

	lhu	$4,0($18)
	addiu	$17,$17,%lo(l2tpInfo)
	sh	$4,20($17)
	lhu	$3,0($3)
	nop
	sh	$3,22($17)
	lw	$3,12($2)
	nop
	sw	$3,16($17)
	lw	$2,16($2)
	nop
	move	$4,$2
	jal	filter_addconnect
	sw	$2,12($17)

	li	$2,1			# 0x1
	lui	$4,%hi($LC2)
	addiu	$4,$4,%lo($LC2)
	li	$5,4			# 0x4
	jal	rtl865x_setNetifType
	sh	$2,38($17)

	li	$2,2			# 0x2
	j	$L57
	sw	$2,%lo(state)($16)

$L45:
	lui	$17,%hi(l2tpInfo)
	addiu	$17,$17,%lo(l2tpInfo)
	lw	$4,12($17)
	addiu	$2,$18,12
	addiu	$18,$18,10
	movn	$18,$2,$3
	jal	filter_checkConnect
	addiu	$16,$18,6

	beq	$2,$0,$L52
	nop

	lw	$4,12($17)
	jal	filter_addconnect
	nop

$L52:
	lhu	$3,0($16)
	li	$2,49185			# 0xc021
	bne	$3,$2,$L57
	nop

	lbu	$3,2($16)
	li	$2,6			# 0x6
	bne	$3,$2,$L58
	addiu	$17,$16,2

	lui	$4,%hi(l2tpInfo)
	addiu	$4,$4,%lo(l2tpInfo)
	lhu	$3,20($4)
	lhu	$2,0($18)
	nop
	bne	$3,$2,$L53
	nop

	lhu	$2,2($18)
	lhu	$3,22($4)
	nop
	bne	$3,$2,$L53
	move	$5,$0

	jal	memset
	li	$6,40			# 0x28

	lui	$2,%hi(state)
	sw	$0,%lo(state)($2)
$L53:
	lbu	$3,2($16)
$L58:
	li	$2,1			# 0x1
	bne	$3,$2,$L57
	nop

	lhu	$5,2($17)
	li	$2,4			# 0x4
	j	$L54
	li	$4,5			# 0x5

$L56:
	lbu	$7,0($3)
	lbu	$6,1($3)
	bne	$7,$4,$L54
	addu	$2,$2,$6

	subu	$2,$2,$6
	lw	$3,2($3)
	lui	$2,%hi(l2tp_ppp_imagic)
	j	$L57
	sw	$3,%lo(l2tp_ppp_imagic)($2)

$L54:
	slt	$6,$2,$5
	bne	$6,$0,$L56
	addu	$3,$17,$2

$L57:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	l2tp_tx_id
	.size	l2tp_tx_id, .-l2tp_tx_id
	.section	.text.fast_l2tp_rx,"ax",@progbits
	.align	2
	.globl	fast_l2tp_rx
	.set	nomips16
	.ent	fast_l2tp_rx
	.type	fast_l2tp_rx, @function
fast_l2tp_rx:
	.frame	$sp,40,$31		# vars= 0, regs= 6/0, args= 16, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$20,32($sp)
	sw	$19,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	jal	rtl_ip_hdr
	move	$16,$4

	move	$4,$16
	lbu	$17,0($2)
	jal	rtl_get_skb_data
	move	$19,$2

	move	$4,$16
	jal	rtl_get_skb_len
	move	$18,$2

	sltu	$2,$2,40
	bne	$2,$0,$L60
	andi	$17,$17,0xf

	sll	$17,$17,2
	addu	$17,$19,$17
	lhu	$2,0($17)
	li	$3,1701			# 0x6a5
	bne	$2,$3,$L66
	move	$3,$0

	lhu	$3,2($17)
	nop
	bne	$3,$2,$L60
	lui	$2,%hi(l2tpInfo)

	lw	$2,%lo(l2tpInfo)($2)
	nop
	bne	$2,$0,$L61
	nop

	jal	l2tp_tx_id
	move	$4,$16

$L61:
	lbu	$2,28($18)
	li	$3,2			# 0x2
	move	$4,$18
	andi	$17,$2,0x40
	movz	$3,$0,$17
	andi	$2,$2,0x2
	addu	$17,$3,$2
	move	$5,$17
	jal	fast_l2tp_tx_lcp_echo_reply
	move	$6,$16

	li	$4,1			# 0x1
	beq	$2,$4,$L66
	li	$3,1			# 0x1

	lui	$2,%hi(l2tpInfo)
	addiu	$20,$2,%lo(l2tpInfo)
	lhu	$3,20($20)
	nop
	beq	$3,$0,$L66
	move	$3,$0

	lhu	$3,22($20)
	nop
	beq	$3,$0,$L60
	addu	$3,$18,$17

	lbu	$4,36($3)
	nop
	bne	$4,$0,$L60
	nop

	lbu	$4,37($3)
	li	$3,33			# 0x21
	bne	$4,$3,$L66
	move	$3,$0

	lw	$2,%lo(l2tpInfo)($2)
	nop
	beq	$2,$0,$L66
	nop

	lw	$2,16($19)
	lw	$3,16($20)
	nop
	bne	$3,$2,$L66
	move	$3,$0

	lw	$2,4($20)
	nop
	bne	$2,$0,$L70
	lui	$2,%hi(l2tpInfo+4)

	lui	$4,%hi($LC2)
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC2)

	sw	$2,4($20)
	lui	$2,%hi(l2tpInfo+4)
$L70:
	lw	$5,%lo(l2tpInfo+4)($2)
	nop
	beq	$5,$0,$L60
	addu	$18,$18,$17

	lbu	$2,47($18)
	li	$3,6			# 0x6
	beq	$2,$3,$L68
	li	$3,17			# 0x11

	bne	$2,$3,$L66
	move	$3,$0

$L68:
	jal	rtl_set_skb_dev
	move	$4,$16

	addiu	$5,$17,38
	jal	skb_pull
	move	$4,$16

	lui	$17,%hi(l2tpInfo)
	jal	rtl_skb_reset_network_header
	move	$4,$16

	addiu	$17,$17,%lo(l2tpInfo)
	jal	rtl_skb_reset_transport_header
	move	$4,$16

	lw	$4,4($17)
	nop
	beq	$4,$0,$L66
	move	$3,$0

	jal	rtl_get_ppp_dev_priv
	nop

	beq	$2,$0,$L66
	move	$3,$0

	lw	$4,4($17)
	jal	rtl_get_ppp_dev_priv
	nop

	move	$4,$16
	jal	rtl_get_skb_len
	move	$17,$2

	move	$6,$2
	move	$4,$17
	jal	rtl_inc_ppp_stats
	move	$5,$0

$L60:
	move	$3,$0
$L66:
	lw	$31,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	move	$2,$3
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fast_l2tp_rx
	.size	fast_l2tp_rx, .-fast_l2tp_rx
	.section	.text.event_ppp_dev_down,"ax",@progbits
	.align	2
	.globl	event_ppp_dev_down
	.set	nomips16
	.ent	event_ppp_dev_down
	.type	event_ppp_dev_down, @function
event_ppp_dev_down:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$16,%hi(l2tpInfo)
	addiu	$16,$16,%lo(l2tpInfo)
	lhu	$2,38($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L73
	move	$17,$4
	.set	macro
	.set	reorder

	lw	$4,4($16)
	#nop
	beq	$4,$0,$L73
	jal	rtl_get_ppp_dev_name
#APP
 # 75 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$3,($2)
1:	lbu	$1,($17)
	addiu	$2,1
	bne	$1,$3,2f
	addiu	$17,1
	bnez	$3,1b
	lbu	$3,($2)
	nop
	move	$3,$1
2:	subu	$3,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L73
	move	$4,$16
	.set	macro
	.set	reorder

	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,40			# 0x28
	.set	macro
	.set	reorder

	lui	$2,%hi(state)
	sw	$0,%lo(state)($2)
$L73:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	event_ppp_dev_down
	.size	event_ppp_dev_down, .-event_ppp_dev_down
	.section	.text.is_l2tp_device,"ax",@progbits
	.align	2
	.globl	is_l2tp_device
	.set	nomips16
	.ent	is_l2tp_device
	.type	is_l2tp_device, @function
is_l2tp_device:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$2,%hi(l2tpInfo+4)
	lw	$3,%lo(l2tpInfo+4)($2)
	move	$16,$4
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L76
	move	$2,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_ppp_dev_name
	move	$4,$3
	.set	macro
	.set	reorder

#APP
 # 75 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$3,($2)
1:	lbu	$1,($16)
	addiu	$2,1
	bne	$1,$3,2f
	addiu	$16,1
	bnez	$3,1b
	lbu	$3,($2)
	nop
	move	$3,$1
2:	subu	$3,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
	sltu	$2,$3,1
$L76:
	lw	$31,20($sp)
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,24
	.set	macro
	.set	reorder

	.end	is_l2tp_device
	.size	is_l2tp_device, .-is_l2tp_device
	.globl	l2tpInfo
	.section	.bss,"aw",@nobits
	.align	2
	.type	l2tpInfo, @object
	.size	l2tpInfo, 40
l2tpInfo:
	.space	40
	.globl	l2tp_ppp_imagic
	.align	2
	.type	l2tp_ppp_imagic, @object
	.size	l2tp_ppp_imagic, 4
l2tp_ppp_imagic:
	.space	4
	.local	res1
	.comm	res1,4,4
	.globl	l2tp_tx_id_hook
	.align	2
	.type	l2tp_tx_id_hook, @object
	.size	l2tp_tx_id_hook, 4
l2tp_tx_id_hook:
	.space	4
	.globl	fast_l2tp_fw
	.align	2
	.type	fast_l2tp_fw, @object
	.size	fast_l2tp_fw, 4
fast_l2tp_fw:
	.space	4
	.globl	state
	.align	2
	.type	state, @object
	.size	state, 4
state:
	.space	4
	.ident	"GCC: (GNU) 4.4.5-1.5.5p2"
