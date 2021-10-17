	.file	1 "fast_pptp_core.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.pptp_conn_single_open,"ax",@progbits
	.align	2
	.globl	pptp_conn_single_open
	.set	nomips16
	.ent	pptp_conn_single_open
	.type	pptp_conn_single_open, @function
pptp_conn_single_open:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	move	$4,$5
	lui	$5,%hi(pptpconn_read_proc)
	addiu	$5,$5,%lo(pptpconn_read_proc)
	move	$6,$0
	j	single_open
	nop

	.set	macro
	.set	reorder
	.end	pptp_conn_single_open
	.size	pptp_conn_single_open, .-pptp_conn_single_open
	.section	.text.fastpath_pptp_single_open,"ax",@progbits
	.align	2
	.globl	fastpath_pptp_single_open
	.set	nomips16
	.ent	fastpath_pptp_single_open
	.type	fastpath_pptp_single_open, @function
fastpath_pptp_single_open:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	move	$4,$5
	lui	$5,%hi(ppfw_read_proc)
	addiu	$5,$5,%lo(ppfw_read_proc)
	move	$6,$0
	j	single_open
	nop

	.set	macro
	.set	reorder
	.end	fastpath_pptp_single_open
	.size	fastpath_pptp_single_open, .-fastpath_pptp_single_open
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"%d\012\000"
	.section	.text.pptpconn_read_proc,"ax",@progbits
	.align	2
	.globl	pptpconn_read_proc
	.set	nomips16
	.ent	pptpconn_read_proc
	.type	pptpconn_read_proc, @function
pptpconn_read_proc:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$2,%hi(pptp_conn_check)
	lw	$6,%lo(pptp_conn_check)($2)
	lui	$5,%hi($LC0)
	jal	seq_printf
	addiu	$5,$5,%lo($LC0)

	lw	$31,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	pptpconn_read_proc
	.size	pptpconn_read_proc, .-pptpconn_read_proc
	.section	.text.ppfw_read_proc,"ax",@progbits
	.align	2
	.globl	ppfw_read_proc
	.set	nomips16
	.ent	ppfw_read_proc
	.type	ppfw_read_proc, @function
ppfw_read_proc:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$2,%hi(fast_pptp_fw)
	lw	$6,%lo(fast_pptp_fw)($2)
	lui	$5,%hi($LC0)
	jal	seq_printf
	addiu	$5,$5,%lo($LC0)

	lw	$31,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	ppfw_read_proc
	.size	ppfw_read_proc, .-ppfw_read_proc
	.section	.text.sync_tx_pptp_gre_seqno,"ax",@progbits
	.align	2
	.globl	sync_tx_pptp_gre_seqno
	.set	nomips16
	.ent	sync_tx_pptp_gre_seqno
	.type	sync_tx_pptp_gre_seqno, @function
sync_tx_pptp_gre_seqno:
	.frame	$sp,40,$31		# vars= 8, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$18,$4
	.set	macro
	.set	reorder

	move	$16,$2
	lui	$2,%hi(fast_pptp_fw)
	lw	$2,%lo(fast_pptp_fw)($2)
	#nop
	beq	$2,$0,$L5
	jal	arch_local_irq_save
	lbu	$3,23($16)
	move	$17,$2
	li	$2,47			# 0x2f
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L17
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	move	$4,$18
	.set	macro
	.set	reorder

	sltu	$2,$2,35
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L17
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$18
	.set	macro
	.set	reorder

	lbu	$3,14($16)
	#nop
	andi	$3,$3,0xf
	sll	$3,$3,2
	addiu	$3,$3,14
	addu	$2,$2,$3
	lbu	$3,2($2)
	lbu	$4,1($2)
	sb	$3,16($sp)
	lbu	$3,3($2)
	andi	$4,$4,0x7
	sb	$3,17($sp)
	li	$3,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$4,$3,$L17
	move	$4,$17
	.set	macro
	.set	reorder

	lhu	$4,16($sp)
	li	$3,34827			# 0x880b
	bne	$4,$3,$L7
	lbu	$3,0($2)
	#nop
	andi	$3,$3,0x10
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L8
	move	$4,$0
	.set	macro
	.set	reorder

	lui	$3,%hi(pptpAccInfo)
	addiu	$3,$3,%lo(pptpAccInfo)
	lhu	$6,18($16)
	lhu	$5,46($3)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$6,$5,$L8
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

	lhu	$6,6($2)
	lhu	$5,18($3)
	#nop
	bne	$6,$5,$L8
	lw	$6,32($3)
	lw	$5,30($16)
	#nop
	bne	$6,$5,$L8
	lw	$4,8($2)
	#nop
	srl	$5,$4,24
	sb	$5,48($3)
	srl	$5,$4,16
	sb	$5,49($3)
	sb	$4,51($3)
	srl	$5,$4,8
	lw	$4,36($3)
	sb	$5,50($3)
	addiu	$5,$4,1
	sw	$5,36($3)
	srl	$3,$4,24
	sb	$3,8($2)
	srl	$3,$4,16
	sb	$3,9($2)
	srl	$3,$4,8
	sw	$4,20($sp)
	sb	$4,11($2)
	sb	$3,10($2)
	move	$4,$0
$L8:
	lb	$3,1($2)
	#nop
	.set	noreorder
	.set	nomacro
	bgez	$3,$L9
	lui	$3,%hi(pptpAccInfo)
	.set	macro
	.set	reorder

	addiu	$3,$3,%lo(pptpAccInfo)
	lhu	$6,6($2)
	lhu	$5,18($3)
	#nop
	bne	$6,$5,$L7
	lw	$6,32($3)
	lw	$5,30($16)
	#nop
	bne	$6,$5,$L7
	lw	$3,40($3)
	#nop
	srl	$5,$3,24
	sb	$5,12($2)
	srl	$5,$3,16
	sb	$5,13($2)
	srl	$5,$3,8
	sw	$3,20($sp)
	sb	$5,14($2)
	sb	$3,15($2)
$L9:
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L7
	lui	$3,%hi(pptpAccInfo)
	.set	macro
	.set	reorder

	addiu	$3,$3,%lo(pptpAccInfo)
	lhu	$2,44($3)
	#nop
	addiu	$2,$2,1
	andi	$2,$2,0xffff
	sh	$2,44($3)
	sh	$2,18($16)
	lw	$5,14($16)
	lw	$6,18($16)
	sh	$0,24($16)
	lw	$4,22($16)
	srl	$7,$5,22
	addu	$5,$6,$5
	sltu	$6,$5,$6
	addu	$5,$5,$4
	addu	$6,$5,$6
	lw	$5,26($16)
	sltu	$4,$6,$4
	addiu	$2,$16,14
	addu	$6,$6,$5
	addu	$4,$6,$4
	andi	$7,$7,0x3c
	sltu	$3,$4,$5
	addu	$7,$2,$7
	addu	$3,$3,$4
	addiu	$2,$16,30
$L10:
	lw	$4,0($2)
	addiu	$2,$2,4
	addu	$3,$4,$3
	sltu	$4,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$7,$L10
	addu	$3,$4,$3
	.set	macro
	.set	reorder

	move	$2,$3
 #APP
 # 78 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
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
	sh	$2,24($16)
$L7:
	move	$4,$17
$L17:
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	arch_local_irq_restore
	addiu	$sp,$sp,40
	.set	macro
	.set	reorder

$L5:
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,40
	.set	macro
	.set	reorder

	.end	sync_tx_pptp_gre_seqno
	.size	sync_tx_pptp_gre_seqno, .-sync_tx_pptp_gre_seqno
	.section	.text.is_pptp_device,"ax",@progbits
	.align	2
	.globl	is_pptp_device
	.set	nomips16
	.ent	is_pptp_device
	.type	is_pptp_device, @function
is_pptp_device:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$2,%hi(pptpAccInfo+56)
	move	$16,$4
	lw	$4,%lo(pptpAccInfo+56)($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L19
	move	$2,$0
	.set	macro
	.set	reorder

	jal	rtl_get_ppp_dev_name
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
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
$L19:
	lw	$31,20($sp)
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,24
	.set	macro
	.set	reorder

	.end	is_pptp_device
	.size	is_pptp_device, .-is_pptp_device
	.section	.text.set_pptp_device,"ax",@progbits
	.align	2
	.globl	set_pptp_device
	.set	nomips16
	.ent	set_pptp_device
	.type	set_pptp_device, @function
set_pptp_device:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$2,%hi(fast_pptp_fw)
	lw	$2,%lo(fast_pptp_fw)($2)
	nop
	beq	$2,$0,$L21
	nop

	jal	rtl_get_dev_by_name
	nop

	lui	$3,%hi(pptpAccInfo+56)
	sw	$2,%lo(pptpAccInfo+56)($3)
$L21:
	lw	$31,20($sp)
	nop
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	set_pptp_device
	.size	set_pptp_device, .-set_pptp_device
	.section	.text.Check_GRE_rx_net_device,"ax",@progbits
	.align	2
	.globl	Check_GRE_rx_net_device
	.set	nomips16
	.ent	Check_GRE_rx_net_device
	.type	Check_GRE_rx_net_device, @function
Check_GRE_rx_net_device:
	.frame	$sp,48,$31		# vars= 0, regs= 7/0, args= 16, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$21,40($sp)
	sw	$20,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$17,%hi(pptpAccInfo)
	jal	rtl_ip_hdr
	move	$19,$4

	move	$16,$2
	move	$4,$19
	jal	rtl_get_skb_data
	addiu	$17,$17,%lo(pptpAccInfo)

	lbu	$21,0($16)
	lw	$18,8($17)
	move	$20,$2
	andi	$21,$21,0xf
	beq	$18,$0,$L24
	move	$16,$0

	jal	rtl_get_skb_dev
	move	$4,$19

	bne	$18,$2,$L27
	move	$2,$16

	sll	$21,$21,2
	addu	$20,$20,$21
	lhu	$16,6($20)
	lhu	$2,30($17)
	nop
	xor	$16,$16,$2
	sltu	$16,$16,1
$L24:
	move	$2,$16
$L27:
	lw	$31,44($sp)
	lw	$21,40($sp)
	lw	$20,36($sp)
	lw	$19,32($sp)
	lw	$18,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	Check_GRE_rx_net_device
	.size	Check_GRE_rx_net_device, .-Check_GRE_rx_net_device
	.section	.text.fast_pptp_filter,"ax",@progbits
	.align	2
	.globl	fast_pptp_filter
	.set	nomips16
	.ent	fast_pptp_filter
	.type	fast_pptp_filter, @function
fast_pptp_filter:
	.frame	$sp,40,$31		# vars= 0, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	jal	rtl_ip_hdr
	move	$17,$4

	move	$4,$17
	jal	rtl_get_skb_len
	move	$19,$2

	sltu	$2,$2,56
	bne	$2,$0,$L28
	li	$2,6			# 0x6

	lbu	$3,0($19)
	lbu	$4,9($19)
	nop
	bne	$4,$2,$L28
	andi	$3,$3,0xf

	sll	$3,$3,2
	addu	$3,$19,$3
	lhu	$4,0($3)
	li	$2,1723			# 0x6bb
	bne	$4,$2,$L28
	nop

	lw	$2,12($3)
	nop
	srl	$2,$2,28
	sll	$2,$2,2
	addu	$2,$3,$2
	lhu	$4,2($2)
	li	$3,1			# 0x1
	bne	$4,$3,$L28
	li	$5,8			# 0x8

	lhu	$3,8($2)
	nop
	bne	$3,$5,$L30
	nop

	lbu	$3,16($2)
	nop
	bne	$3,$4,$L28
	lui	$16,%hi(pptpAccInfo)

	addiu	$3,$16,%lo(pptpAccInfo)
	lw	$4,36($3)
	nop
	beq	$4,$0,$L31
	nop

	lw	$4,20($3)
	nop
	beq	$4,$0,$L31
	nop

	lw	$4,32($3)
	nop
	beq	$4,$0,$L31
	nop

	lhu	$4,18($3)
	lhu	$3,12($2)
	nop
	bne	$4,$3,$L28
	addiu	$3,$16,%lo(pptpAccInfo)

	lhu	$4,30($3)
	lhu	$3,14($2)
	nop
	bne	$4,$3,$L28
	nop

$L31:
	li	$18,1			# 0x1
	lui	$3,%hi(pptp_tcp_finished)
	sw	$18,%lo(pptp_tcp_finished)($3)
	lhu	$3,12($2)
	addiu	$16,$16,%lo(pptpAccInfo)
	move	$4,$17
	sh	$3,18($16)
	lhu	$2,14($2)
	nop
	sh	$2,30($16)
	lw	$2,16($19)
	nop
	sw	$2,20($16)
	lw	$2,12($19)
	sw	$18,36($16)
	jal	rtl_get_skb_dev
	sw	$2,32($16)

	move	$4,$17
	jal	rtl_get_skb_data
	sw	$2,8($16)

	lui	$4,%hi(pptpAccInfo+24)
	addiu	$5,$2,-8
	li	$6,6			# 0x6
	jal	memcpy
	addiu	$4,$4,%lo(pptpAccInfo+24)

	jal	rtl_get_skb_data
	move	$4,$17

	lui	$4,%hi(pptpAccInfo+12)
	addiu	$4,$4,%lo(pptpAccInfo+12)
	addiu	$5,$2,-14
	jal	memcpy
	li	$6,6			# 0x6

	j	$L28
	sw	$18,4($16)

$L30:
	li	$4,13			# 0xd
	bne	$3,$4,$L28
	lui	$4,%hi(pptpAccInfo)

	addiu	$3,$4,%lo(pptpAccInfo)
	lw	$5,36($3)
	nop
	beq	$5,$0,$L32
	nop

	lw	$5,20($3)
	nop
	beq	$5,$0,$L32
	nop

	lw	$5,32($3)
	nop
	beq	$5,$0,$L32
	nop

	lhu	$3,18($3)
	lhu	$2,12($2)
	nop
	bne	$3,$2,$L28
	nop

$L32:
	lui	$2,%hi(pptp_tcp_finished)
	addiu	$4,$4,%lo(pptpAccInfo)
	sw	$0,%lo(pptp_tcp_finished)($2)
	sw	$0,4($4)
$L28:
	lw	$31,36($sp)
	lw	$19,32($sp)
	lw	$18,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fast_pptp_filter
	.size	fast_pptp_filter, .-fast_pptp_filter
	.section	.text.fast_pptp_to_lan,"ax",@progbits
	.align	2
	.globl	fast_pptp_to_lan
	.set	nomips16
	.ent	fast_pptp_to_lan
	.type	fast_pptp_to_lan, @function
fast_pptp_to_lan:
	.frame	$sp,56,$31		# vars= 8, regs= 7/0, args= 16, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$21,48($sp)
	sw	$20,44($sp)
	sw	$19,40($sp)
	sw	$18,36($sp)
	sw	$17,32($sp)
	sw	$16,28($sp)
	lw	$16,0($4)
	lui	$18,%hi(pptpAccInfo)
	move	$17,$4
	addiu	$19,$18,%lo(pptpAccInfo)
	jal	rtl_get_skb_dev
	move	$4,$16

	lw	$3,8($19)
	nop
	bne	$3,$2,$L34
	move	$21,$0

	jal	rtl_ip_hdr
	move	$4,$16

	lbu	$3,9($2)
	li	$2,47			# 0x2f
	bne	$3,$2,$L60
	move	$2,$21

	jal	rtl_get_skb_len
	move	$4,$16

	sltu	$2,$2,20
	bne	$2,$0,$L60
	move	$2,$21

	jal	rtl_ip_hdr
	move	$4,$16

	move	$20,$2
	jal	rtl_get_skb_data
	move	$4,$16

	lbu	$3,0($20)
	nop
	andi	$3,$3,0xf
	sll	$3,$3,2
	addu	$3,$2,$3
	lbu	$4,1($3)
	li	$2,1			# 0x1
	andi	$4,$4,0x7
	bne	$4,$2,$L60
	move	$2,$21

	lhu	$4,2($3)
	li	$2,34827			# 0x880b
	bne	$4,$2,$L60
	move	$2,$21

	lhu	$4,6($3)
	lhu	$2,30($19)
	nop
	bne	$4,$2,$L60
	move	$2,$21

	lbu	$4,0($3)
	nop
	andi	$4,$4,0x10
	beq	$4,$0,$L35
	li	$2,8			# 0x8

	lw	$2,8($3)
	nop
	sw	$2,40($19)
	li	$2,12			# 0xc
$L35:
	lb	$5,1($3)
	addiu	$4,$2,4
	slt	$5,$5,0
	movn	$2,$4,$5
	lhu	$4,4($3)
	nop
	beq	$4,$0,$L34
	move	$21,$0

	addu	$3,$3,$2
	lbu	$5,0($3)
	nop
	bne	$5,$0,$L37
	li	$6,255			# 0xff

	addiu	$3,$3,1
	j	$L38
	li	$4,1			# 0x1

$L37:
	bne	$5,$6,$L38
	move	$4,$0

	lbu	$6,1($3)
	li	$5,3			# 0x3
	bne	$6,$5,$L38
	nop

	lbu	$4,2($3)
	nop
	beq	$4,$0,$L39
	li	$4,2			# 0x2

	j	$L38
	addiu	$3,$3,2

$L39:
	addiu	$3,$3,3
	li	$4,3			# 0x3
$L38:
	lbu	$19,0($3)
	li	$3,33			# 0x21
	beq	$19,$3,$L40
	li	$3,253			# 0xfd

	bne	$19,$3,$L34
	move	$21,$0

$L40:
	lbu	$5,0($20)
	addiu	$4,$4,1
	andi	$5,$5,0xf
	sll	$5,$5,2
	addu	$5,$5,$2
	li	$2,33			# 0x21
	beq	$19,$2,$L41
	addu	$5,$5,$4

	addiu	$20,$18,%lo(pptpAccInfo)
	lw	$4,56($20)
	nop
	beq	$4,$0,$L34
	move	$21,$0

	jal	rtl_get_ppp_dev_priv
	sw	$5,16($sp)

	lw	$5,16($sp)
	beq	$2,$0,$L34
	addiu	$5,$5,-2

	jal	skb_pull
	move	$4,$16

	jal	rtl_get_skb_data
	move	$4,$16

	sb	$0,0($2)
	sb	$19,1($2)
	lw	$4,56($20)
	jal	rtl_get_ppp_dev_priv
	li	$21,-2			# 0xfffffffffffffffe

	move	$4,$2
	move	$5,$16
	jal	rtl_ppp_receive_nonmp_frame
	li	$6,1			# 0x1

	beq	$2,$0,$L34
	move	$16,$2

	sw	$2,0($17)
	j	$L42
	li	$19,1			# 0x1

$L41:
	jal	skb_pull
	move	$4,$16

	move	$19,$0
$L42:
	jal	rtl_get_skb_data
	move	$4,$16

	andi	$17,$2,0x3
	beq	$17,$0,$L43
	nop

	jal	rtl_skb_headroom
	move	$4,$16

	sltu	$2,$2,$17
	bne	$2,$0,$L43
	nop

	jal	rtl_get_skb_data
	move	$4,$16

	move	$4,$16
	jal	rtl_get_skb_data
	subu	$20,$2,$17

	move	$4,$16
	jal	rtl_get_skb_len
	sw	$2,16($sp)

	lw	$5,16($sp)
	move	$4,$20
	jal	memmove
	move	$6,$2

	move	$4,$16
	move	$5,$17
	jal	rtl_set_skb_data
	li	$6,1			# 0x1

	move	$4,$16
	move	$5,$17
	jal	rtl_set_skb_tail
	li	$6,1			# 0x1

$L43:
	jal	rtl_skb_reset_network_header
	move	$4,$16

	addiu	$17,$18,%lo(pptpAccInfo)
	jal	rtl_skb_reset_transport_header
	move	$4,$16

	lw	$4,56($17)
	nop
	beq	$4,$0,$L34
	move	$21,$0

	bne	$19,$0,$L44
	nop

	jal	rtl_get_ppp_dev_priv
	nop

	beq	$2,$0,$L44
	nop

	lw	$4,56($17)
	jal	rtl_get_ppp_dev_priv
	nop

	move	$4,$16
	jal	rtl_get_skb_len
	move	$17,$2

	addiu	$6,$2,-2
	move	$4,$17
	jal	rtl_inc_ppp_stats
	move	$5,$0

$L44:
	addiu	$18,$18,%lo(pptpAccInfo)
	lw	$5,56($18)
	move	$4,$16
	jal	rtl_set_skb_dev
	li	$21,1			# 0x1

$L34:
	move	$2,$21
$L60:
	lw	$31,52($sp)
	lw	$21,48($sp)
	lw	$20,44($sp)
	lw	$19,40($sp)
	lw	$18,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	fast_pptp_to_lan
	.size	fast_pptp_to_lan, .-fast_pptp_to_lan
	.section	.text.get_fastpptp_lastxmit,"ax",@progbits
	.align	2
	.globl	get_fastpptp_lastxmit
	.set	nomips16
	.ent	get_fastpptp_lastxmit
	.type	get_fastpptp_lastxmit, @function
get_fastpptp_lastxmit:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	lui	$2,%hi(fast_pptp_fw)
	lw	$3,%lo(fast_pptp_fw)($2)
	nop
	beq	$3,$0,$L65
	move	$2,$0

	lui	$3,%hi(pptpAccInfo)
	addiu	$2,$3,%lo(pptpAccInfo)
	lw	$2,4($2)
	lw	$3,%lo(pptpAccInfo)($3)
	xori	$2,$2,0x1
	movn	$3,$0,$2
	move	$2,$3
$L65:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	get_fastpptp_lastxmit
	.size	get_fastpptp_lastxmit, .-get_fastpptp_lastxmit
	.section	.rodata.str1.4
	.align	2
$LC1:
	.ascii	"ppp0\000"
	.align	2
$LC2:
	.ascii	"RTL\000"
	.align	2
$LC3:
	.ascii	"pptpInfo.ppp0_dev == NULL\012\000"
	.align	2
$LC4:
	.ascii	"pptpInfo.ppp0_dev->priv == NULL\012\000"
	.section	.text.fast_pptp_to_wan,"ax",@progbits
	.align	2
	.globl	fast_pptp_to_wan
	.set	nomips16
	.ent	fast_pptp_to_wan
	.type	fast_pptp_to_wan, @function
fast_pptp_to_wan:
	.frame	$sp,96,$31		# vars= 48, regs= 8/0, args= 16, gp= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-96
	sw	$31,92($sp)
	sw	$22,88($sp)
	sw	$21,84($sp)
	sw	$20,80($sp)
	sw	$19,76($sp)
	sw	$18,72($sp)
	sw	$17,68($sp)
	sw	$16,64($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$18,$4
	.set	macro
	.set	reorder

	lw	$2,12($2)
	move	$4,$18
	li	$17,-16777216			# 0xffffffffff000000
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	and	$17,$17,$2
	.set	macro
	.set	reorder

	lw	$16,12($2)
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$18
	.set	macro
	.set	reorder

	lw	$3,12($2)
	li	$2,167772160			# 0xa000000
	.set	noreorder
	.set	nomacro
	bne	$17,$2,$L67
	li	$2,16711680			# 0xff0000
	.set	macro
	.set	reorder

	and	$16,$16,$2
	li	$2,4194304			# 0x400000
	.set	noreorder
	.set	nomacro
	bne	$16,$2,$L67
	andi	$3,$3,0xff00
	.set	macro
	.set	reorder

	li	$2,16384			# 0x4000
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L85
	lui	$19,%hi(pptpAccInfo)
	.set	macro
	.set	reorder

	lui	$2,%hi(pptp_conn_check)
	lw	$3,%lo(pptp_conn_check)($2)
	li	$2,3			# 0x3
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L68
	move	$21,$0
	.set	macro
	.set	reorder

$L67:
	lui	$19,%hi(pptpAccInfo)
$L85:
	addiu	$16,$19,%lo(pptpAccInfo)
	lw	$4,56($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L68
	move	$21,$0
	.set	macro
	.set	reorder

	jal	rtl_get_ppp_dev_name
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L84
	move	$2,$21
	.set	macro
	.set	reorder

	lw	$4,56($16)
	jal	rtl_get_ppp_dev_name
	lui	$3,%hi($LC1)
	addiu	$3,$3,%lo($LC1)
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$22,($2)
1:	lbu	$1,($3)
	addiu	$2,1
	bne	$1,$22,2f
	addiu	$3,1
	bnez	$22,1b
	lbu	$22,($2)
	nop
	move	$22,$1
2:	subu	$22,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$22,$0,$L84
	move	$2,$21
	.set	macro
	.set	reorder

	lw	$2,4($16)
	#nop
	beq	$2,$0,$L69
	lw	$4,56($16)
	#nop
	beq	$4,$0,$L69
	jal	rtl_get_ppp_dev_priv
	beq	$2,$0,$L69
	lw	$4,56($16)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_ppp_dev_priv
	move	$21,$22
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_vj_check
	move	$17,$2
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L84
	move	$2,$21
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$18
	.set	macro
	.set	reorder

	lbu	$20,1($2)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_ppp_xmit_pending
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L84
	move	$2,$21
	.set	macro
	.set	reorder

	lui	$5,%hi($LC2)
	li	$6,3			# 0x3
	move	$4,$18
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_cb
	addiu	$5,$5,%lo($LC2)
	.set	macro
	.set	reorder

	lw	$5,56($16)
	move	$4,$18
	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_start_xmit
	li	$21,1			# 0x1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_ppp_xmit_pending
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L84
	move	$2,$21
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_ppp_xmit_pending
	move	$4,$17
	.set	macro
	.set	reorder

	move	$16,$2
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_ppp_xmit_pending
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_headroom
	move	$4,$16
	.set	macro
	.set	reorder

	sltu	$2,$2,50
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L86
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
	bne	$2,$0,$L86
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
	beq	$2,$0,$L87
	move	$4,$16
	.set	macro
	.set	reorder

$L86:
	.set	noreorder
	.set	nomacro
	jal	skb_realloc_headroom
	li	$5,50			# 0x32
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L68
	move	$21,$0
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	consume_skb
	sw	$2,56($sp)
	.set	macro
	.set	reorder

	lw	$2,56($sp)
	#nop
	move	$16,$2
	move	$4,$16
$L87:
	.set	noreorder
	.set	nomacro
	jal	skb_push
	li	$5,50			# 0x32
	.set	macro
	.set	reorder

	lui	$5,%hi(pptpAccInfo+24)
	move	$4,$2
	addiu	$5,$5,%lo(pptpAccInfo+24)
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	move	$17,$2
	.set	macro
	.set	reorder

	lui	$5,%hi(pptpAccInfo+12)
	li	$6,6			# 0x6
	addiu	$4,$17,6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$5,%lo(pptpAccInfo+12)
	.set	macro
	.set	reorder

	li	$2,8			# 0x8
	sb	$2,12($17)
	sb	$0,13($17)
	lw	$3,32($sp)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1073741824			# 0x40000000
	or	$2,$2,$3
	li	$3,-251723776			# 0xfffffffff0ff0000
	ori	$3,$3,0xffff
	and	$2,$2,$3
	li	$3,83886080			# 0x5000000
	or	$2,$2,$3
	sw	$2,32($sp)
	li	$2,16384			# 0x4000
	addiu	$17,$19,%lo(pptpAccInfo)
	sh	$2,38($sp)
	li	$2,47			# 0x2f
	sb	$2,41($sp)
	lw	$2,32($17)
	move	$5,$0
	move	$4,$16
	sw	$2,48($sp)
	lw	$2,20($17)
	sb	$20,33($sp)
	sw	$2,44($sp)
	li	$2,64			# 0x40
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$2,40($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	move	$4,$16
	.set	macro
	.set	reorder

	lhu	$3,44($17)
	addiu	$2,$2,-14
	sh	$2,34($sp)
	addiu	$3,$3,1
	andi	$3,$3,0xffff
	sh	$3,44($17)
	sh	$3,36($sp)
	lw	$6,32($sp)
	lw	$7,36($sp)
	sh	$0,42($sp)
	lw	$5,40($sp)
	srl	$8,$6,22
	addu	$6,$7,$6
	sltu	$7,$6,$7
	addu	$6,$6,$5
	addu	$7,$6,$7
	lw	$6,44($sp)
	sltu	$5,$7,$5
	addiu	$2,$sp,32
	addu	$7,$7,$6
	addu	$5,$7,$5
	andi	$8,$8,0x3c
	sltu	$4,$5,$6
	addu	$8,$2,$8
	addu	$4,$4,$5
	addiu	$2,$sp,48
$L72:
	lw	$5,0($2)
	addiu	$2,$2,4
	addu	$4,$5,$4
	sltu	$5,$4,$5
	.set	noreorder
	.set	nomacro
	bne	$2,$8,$L72
	addu	$4,$5,$4
	.set	macro
	.set	reorder

	addiu	$17,$19,%lo(pptpAccInfo)
	sh	$3,46($17)
	move	$2,$4
	move	$4,$16
 #APP
 # 78 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
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
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sh	$2,42($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,14
	addiu	$5,$sp,32
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,20			# 0x14
	.set	macro
	.set	reorder

	li	$2,48			# 0x30
	sb	$2,16($sp)
	li	$2,-127			# 0xffffffffffffff81
	sb	$2,17($sp)
	move	$4,$16
	li	$2,-30709			# 0xffffffffffff880b
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sh	$2,18($sp)
	.set	macro
	.set	reorder

	addiu	$2,$2,-50
	sh	$2,20($sp)
	lhu	$2,18($17)
	move	$4,$16
	li	$21,1			# 0x1
	sh	$2,22($sp)
	lw	$2,36($17)
	#nop
	sw	$2,24($sp)
	addiu	$2,$2,1
	sw	$2,36($17)
	lw	$2,40($17)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,34
	li	$6,16			# 0x10
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,16
	.set	macro
	.set	reorder

	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	lw	$5,8($17)
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dev
	sw	$2,%lo(pptpAccInfo)($19)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	dev_queue_xmit
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L84
	move	$2,$21
	.set	macro
	.set	reorder

$L69:
	addiu	$19,$19,%lo(pptpAccInfo)
	lw	$4,56($19)
	#nop
	bne	$4,$0,$L73
	lui	$4,%hi($LC3)
	.set	noreorder
	.set	nomacro
	jal	printk
	addiu	$4,$4,%lo($LC3)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L68
	move	$21,$0
	.set	macro
	.set	reorder

$L73:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_ppp_dev_priv
	move	$21,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L84
	move	$2,$21
	.set	macro
	.set	reorder

	lui	$4,%hi($LC4)
	.set	noreorder
	.set	nomacro
	jal	printk
	addiu	$4,$4,%lo($LC4)
	.set	macro
	.set	reorder

$L68:
	move	$2,$21
$L84:
	lw	$31,92($sp)
	lw	$22,88($sp)
	lw	$21,84($sp)
	lw	$20,80($sp)
	lw	$19,76($sp)
	lw	$18,72($sp)
	lw	$17,68($sp)
	lw	$16,64($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,96
	.set	macro
	.set	reorder

	.end	fast_pptp_to_wan
	.size	fast_pptp_to_wan, .-fast_pptp_to_wan
	.section	.text.fast_pptp_sync_rx_seq,"ax",@progbits
	.align	2
	.globl	fast_pptp_sync_rx_seq
	.set	nomips16
	.ent	fast_pptp_sync_rx_seq
	.type	fast_pptp_sync_rx_seq, @function
fast_pptp_sync_rx_seq:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	jal	rtl_ip_hdr
	nop

	lbu	$4,21($2)
	li	$3,1			# 0x1
	andi	$4,$4,0x7
	bne	$4,$3,$L88
	li	$3,34827			# 0x880b

	lhu	$4,22($2)
	nop
	bne	$4,$3,$L88
	nop

	lbu	$3,20($2)
	nop
	andi	$3,$3,0x10
	beq	$3,$0,$L90
	lui	$3,%hi(pptpAccInfo)

	addiu	$3,$3,%lo(pptpAccInfo)
	lw	$4,52($3)
	nop
	sw	$4,28($2)
	addiu	$4,$4,1
	sw	$4,52($3)
$L90:
	lb	$3,21($2)
	nop
	bgez	$3,$L88
	lui	$3,%hi(pptpAccInfo+48)

	lw	$3,%lo(pptpAccInfo+48)($3)
	nop
	sw	$3,32($2)
$L88:
	lw	$31,20($sp)
	nop
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fast_pptp_sync_rx_seq
	.size	fast_pptp_sync_rx_seq, .-fast_pptp_sync_rx_seq
	.section	.text.pptpconn_write_proc,"ax",@progbits
	.align	2
	.globl	pptpconn_write_proc
	.set	nomips16
	.ent	pptpconn_write_proc
	.type	pptpconn_write_proc, @function
pptpconn_write_proc:
	.frame	$sp,208,$31		# vars= 200, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-208
	sw	$31,204($sp)
	sltu	$4,$6,2
	bne	$4,$0,$L92
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L92
	addu	$2,$5,$6
	.set	macro
	.set	reorder

	or	$4,$5,$6
	or	$2,$4,$2
	lw	$4,24($28)
	#nop
	and	$2,$2,$4
	bne	$2,$0,$L92
	move	$4,$sp
 #APP
 # 913 "net/rtl/fastpath/fast_pptp_core.c" 1
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
	beq	$6,$0,$L95
	move	$5,$0
	.set	macro
	.set	reorder

	j	$L92
$L94:
	addiu	$2,$2,1
	mflo	$5
	addiu	$5,$5,-48
	.set	noreorder
	.set	nomacro
	j	$L93
	addu	$5,$5,$3
	.set	macro
	.set	reorder

$L95:
	move	$2,$sp
	li	$4,10			# 0xa
$L93:
	lb	$3,0($2)
	#nop
	addiu	$6,$3,-48
	andi	$6,$6,0x00ff
	sltu	$6,$6,10
	.set	noreorder
	.set	nomacro
	bne	$6,$0,$L94
	mult	$5,$4
	.set	macro
	.set	reorder

	lui	$2,%hi(pptp_conn_check)
	sw	$5,%lo(pptp_conn_check)($2)
$L92:
	lw	$31,204($sp)
	li	$2,-14			# 0xfffffffffffffff2
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,208
	.set	macro
	.set	reorder

	.end	pptpconn_write_proc
	.size	pptpconn_write_proc, .-pptpconn_write_proc
	.section	.text.pptp_conn_single_write,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	pptp_conn_single_write
	.type	pptp_conn_single_write, @function
pptp_conn_single_write:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	pptpconn_write_proc
	nop

	.set	macro
	.set	reorder
	.end	pptp_conn_single_write
	.size	pptp_conn_single_write, .-pptp_conn_single_write
	.section	.text.ppfw_write_proc,"ax",@progbits
	.align	2
	.globl	ppfw_write_proc
	.set	nomips16
	.ent	ppfw_write_proc
	.type	ppfw_write_proc, @function
ppfw_write_proc:
	.frame	$sp,40,$31		# vars= 8, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	sltu	$2,$6,2
	move	$18,$5
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L98
	li	$17,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	addiu	$4,$6,32
	li	$5,32			# 0x20
	.set	noreorder
	.set	nomacro
	jal	__kmalloc
	sw	$6,16($sp)
	.set	macro
	.set	reorder

	lw	$6,16($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L98
	move	$16,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$18,$0,$L99
	or	$3,$18,$6
	.set	macro
	.set	reorder

	addu	$2,$18,$6
	or	$2,$3,$2
	lw	$3,24($28)
	#nop
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L99
	move	$17,$6
	.set	macro
	.set	reorder

	move	$4,$16
	move	$5,$18
 #APP
 # 939 "net/rtl/fastpath/fast_pptp_core.c" 1
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
	beq	$6,$0,$L105
	move	$2,$16
	.set	macro
	.set	reorder

	j	$L99
$L101:
	addiu	$2,$2,1
	mflo	$5
	addiu	$5,$5,-48
	.set	noreorder
	.set	nomacro
	j	$L100
	addu	$5,$5,$3
	.set	macro
	.set	reorder

$L105:
	move	$5,$0
	li	$4,10			# 0xa
$L100:
	lb	$3,0($2)
	#nop
	addiu	$6,$3,-48
	andi	$6,$6,0x00ff
	sltu	$6,$6,10
	.set	noreorder
	.set	nomacro
	bne	$6,$0,$L101
	mult	$5,$4
	.set	macro
	.set	reorder

	lui	$18,%hi(fast_pptp_fw)
	lui	$4,%hi(pptpAccInfo)
	sw	$5,%lo(fast_pptp_fw)($18)
	addiu	$4,$4,%lo(pptpAccInfo)
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,64			# 0x40
	.set	macro
	.set	reorder

	lw	$3,%lo(fast_pptp_fw)($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L102
	move	$2,$0
	.set	macro
	.set	reorder

	lui	$2,%hi(sync_tx_pptp_gre_seqno)
	addiu	$2,$2,%lo(sync_tx_pptp_gre_seqno)
$L102:
	lui	$3,%hi(sync_tx_pptp_gre_seqno_hook)
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	kfree
	sw	$2,%lo(sync_tx_pptp_gre_seqno_hook)($3)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L107
	move	$2,$17
	.set	macro
	.set	reorder

$L99:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	li	$17,-14			# 0xfffffffffffffff2
$L98:
	move	$2,$17
$L107:
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,40
	.set	macro
	.set	reorder

	.end	ppfw_write_proc
	.size	ppfw_write_proc, .-ppfw_write_proc
	.section	.text.fastpath_pptp_single_write,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	fastpath_pptp_single_write
	.type	fastpath_pptp_single_write, @function
fastpath_pptp_single_write:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	ppfw_write_proc
	nop

	.set	macro
	.set	reorder
	.end	fastpath_pptp_single_write
	.size	fastpath_pptp_single_write, .-fastpath_pptp_single_write
	.section	.rodata.str1.4
	.align	2
$LC5:
	.ascii	"fast_pptp\000"
	.align	2
$LC6:
	.ascii	"pptp_conn_ck\000"
	.section	.init.text,"ax",@progbits
	.align	2
	.globl	fast_pptp_init
	.set	nomips16
	.ent	fast_pptp_init
	.type	fast_pptp_init, @function
fast_pptp_init:
	.frame	$sp,32,$31		# vars= 0, regs= 2/0, args= 24, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$16,24($sp)
	lui	$16,%hi(proc_root)
	lui	$4,%hi($LC5)
	lui	$7,%hi(fastpath_pptp_proc_fops)
	addiu	$6,$16,%lo(proc_root)
	addiu	$4,$4,%lo($LC5)
	move	$5,$0
	addiu	$7,$7,%lo(fastpath_pptp_proc_fops)
	jal	proc_create_data
	sw	$0,16($sp)

	lui	$4,%hi($LC6)
	lui	$7,%hi(pptp_conn_proc_fops)
	lui	$3,%hi(res1)
	addiu	$6,$16,%lo(proc_root)
	addiu	$7,$7,%lo(pptp_conn_proc_fops)
	addiu	$4,$4,%lo($LC6)
	move	$5,$0
	sw	$2,%lo(res1)($3)
	jal	proc_create_data
	sw	$0,16($sp)

	lui	$4,%hi(pptpAccInfo)
	lui	$3,%hi(res_check_pptp)
	addiu	$4,$4,%lo(pptpAccInfo)
	move	$5,$0
	li	$6,64			# 0x40
	jal	memset
	sw	$2,%lo(res_check_pptp)($3)

	lw	$31,28($sp)
	lw	$16,24($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	fast_pptp_init
	.size	fast_pptp_init, .-fast_pptp_init
	.section	.exit.text,"ax",@progbits
	.align	2
	.globl	fast_pptp_exit
	.set	nomips16
	.ent	fast_pptp_exit
	.type	fast_pptp_exit, @function
fast_pptp_exit:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$16,%hi(res1)
	lw	$2,%lo(res1)($16)
	nop
	beq	$2,$0,$L111
	lui	$4,%hi($LC5)

	lui	$5,%hi(proc_root)
	addiu	$4,$4,%lo($LC5)
	jal	remove_proc_entry
	addiu	$5,$5,%lo(proc_root)

	sw	$0,%lo(res1)($16)
$L111:
	lui	$16,%hi(res_check_pptp)
	lw	$2,%lo(res_check_pptp)($16)
	nop
	beq	$2,$0,$L110
	lui	$4,%hi($LC6)

	lui	$5,%hi(proc_root)
	addiu	$4,$4,%lo($LC6)
	jal	remove_proc_entry
	addiu	$5,$5,%lo(proc_root)

	sw	$0,%lo(res_check_pptp)($16)
$L110:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fast_pptp_exit
	.size	fast_pptp_exit, .-fast_pptp_exit
	.globl	pptp_conn_proc_fops
	.section	.data.pptp_conn_proc_fops,"aw",@progbits
	.align	2
	.type	pptp_conn_proc_fops, @object
	.size	pptp_conn_proc_fops, 108
pptp_conn_proc_fops:
	.space	4
	.word	seq_lseek
	.word	seq_read
	.word	pptp_conn_single_write
	.space	28
	.word	pptp_conn_single_open
	.space	4
	.word	single_release
	.space	52
	.globl	fastpath_pptp_proc_fops
	.section	.data.fastpath_pptp_proc_fops,"aw",@progbits
	.align	2
	.type	fastpath_pptp_proc_fops, @object
	.size	fastpath_pptp_proc_fops, 108
fastpath_pptp_proc_fops:
	.space	4
	.word	seq_lseek
	.word	seq_read
	.word	fastpath_pptp_single_write
	.space	28
	.word	fastpath_pptp_single_open
	.space	4
	.word	single_release
	.space	52
	.globl	fast_pptp_fw
	.section	.bss.fast_pptp_fw,"aw",@nobits
	.align	2
	.type	fast_pptp_fw, @object
	.size	fast_pptp_fw, 4
fast_pptp_fw:
	.space	4
	.globl	pptp_tcp_finished
	.section	.bss.pptp_tcp_finished,"aw",@nobits
	.align	2
	.type	pptp_tcp_finished, @object
	.size	pptp_tcp_finished, 4
pptp_tcp_finished:
	.space	4
	.section	.bss.pptpAccInfo,"aw",@nobits
	.align	2
	.type	pptpAccInfo, @object
	.size	pptpAccInfo, 64
pptpAccInfo:
	.space	64
	.section	.bss.pptp_conn_check,"aw",@nobits
	.align	2
	.type	pptp_conn_check, @object
	.size	pptp_conn_check, 4
pptp_conn_check:
	.space	4
	.section	.bss.res1,"aw",@nobits
	.align	2
	.type	res1, @object
	.size	res1, 4
res1:
	.space	4
	.section	.bss.res_check_pptp,"aw",@nobits
	.align	2
	.type	res_check_pptp, @object
	.size	res_check_pptp, 4
res_check_pptp:
	.space	4
	.ident	"GCC: (Realtek RSDK-4.6.4 Build 2080) 4.6.4"
