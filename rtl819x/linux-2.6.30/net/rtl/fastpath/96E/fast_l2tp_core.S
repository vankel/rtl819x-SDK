	.file	1 "fast_l2tp_core.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.fastpath_l2tp_single_open,"ax",@progbits
	.align	2
	.globl	fastpath_l2tp_single_open
	.set	nomips16
	.ent	fastpath_l2tp_single_open
	.type	fastpath_l2tp_single_open, @function
fastpath_l2tp_single_open:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	move	$4,$5
	lui	$5,%hi(l2tp_read_proc)
	addiu	$5,$5,%lo(l2tp_read_proc)
	move	$6,$0
	j	single_open
	nop

	.set	macro
	.set	reorder
	.end	fastpath_l2tp_single_open
	.size	fastpath_l2tp_single_open, .-fastpath_l2tp_single_open
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"%c\012\000"
	.section	.text.l2tp_read_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	l2tp_read_proc
	.type	l2tp_read_proc, @function
l2tp_read_proc:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$2,%hi(fast_l2tp_fw)
	lw	$6,%lo(fast_l2tp_fw)($2)
	lui	$5,%hi($LC0)
	addiu	$5,$5,%lo($LC0)
	jal	seq_printf
	addiu	$6,$6,48

	lw	$31,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	l2tp_read_proc
	.size	l2tp_read_proc, .-l2tp_read_proc
	.section	.rodata.str1.4
	.align	2
$LC1:
	.ascii	"ppp0\000"
	.align	2
$LC2:
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

	beq	$2,$0,$L3
	nop

	lbu	$3,0($2)
	li	$5,1701			# 0x6a5
	andi	$3,$3,0xf
	sll	$3,$3,2
	addu	$3,$2,$3
	lhu	$4,0($3)
	nop
	bne	$4,$5,$L3
	nop

	lhu	$5,2($3)
	nop
	bne	$5,$4,$L3
	nop

	lhu	$4,8($3)
	nop
	sll	$5,$4,16
	sra	$5,$5,16
	bgez	$5,$L5
	andi	$4,$4,0x4000

	addiu	$5,$3,10
	addiu	$3,$3,12
	movn	$5,$3,$4
	lhu	$3,14($5)
	li	$4,4			# 0x4
	bne	$3,$4,$L8
	li	$4,11			# 0xb

	lui	$4,%hi(l2tpInfo)
	addiu	$4,$4,%lo(l2tpInfo)
	lhu	$3,20($4)
	lhu	$2,0($5)
	nop
	bne	$3,$2,$L3
	nop

	move	$5,$0
	jal	memset
	li	$6,40			# 0x28

	lui	$4,%hi($LC1)
	addiu	$4,$4,%lo($LC1)
	jal	rtl865x_setNetifType
	li	$5,2			# 0x2

	lui	$2,%hi(state)
	j	$L3
	sw	$0,%lo(state)($2)

$L8:
	bne	$3,$4,$L9
	li	$4,12			# 0xc

	lui	$18,%hi(state)
	lw	$2,%lo(state)($18)
	nop
	bne	$2,$0,$L3
	nop

	jal	rtl_get_skb_dev_name
	move	$4,$17

	lui	$5,%hi($LC2)
	move	$4,$2
	addiu	$5,$5,%lo($LC2)
	jal	memcmp
	li	$6,4			# 0x4

	bne	$2,$0,$L3
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
	j	$L3
	sw	$2,%lo(state)($18)

$L9:
	bne	$3,$4,$L3
	nop

	lhu	$3,2($5)
	nop
	beq	$3,$0,$L3
	lui	$16,%hi(state)

	lw	$3,%lo(state)($16)
	nop
	sltu	$3,$3,2
	beq	$3,$0,$L3
	lui	$17,%hi(l2tpInfo)

	lhu	$3,0($5)
	addiu	$17,$17,%lo(l2tpInfo)
	sh	$3,20($17)
	lhu	$3,2($5)
	nop
	sh	$3,22($17)
	lw	$3,12($2)
	nop
	sw	$3,16($17)
	lw	$4,16($2)
	jal	filter_addconnect
	sw	$4,12($17)

	li	$2,1			# 0x1
	lui	$4,%hi($LC1)
	sh	$2,38($17)
	addiu	$4,$4,%lo($LC1)
	jal	rtl865x_setNetifType
	li	$5,4			# 0x4

	li	$2,2			# 0x2
	j	$L3
	sw	$2,%lo(state)($16)

$L5:
	lui	$17,%hi(l2tpInfo)
	addiu	$16,$3,10
	addiu	$18,$17,%lo(l2tpInfo)
	addiu	$3,$3,12
	movn	$16,$3,$4
	lw	$4,12($18)
	jal	filter_checkConnect
	nop

	beq	$2,$0,$L12
	nop

	lw	$4,12($18)
	jal	filter_addconnect
	nop

$L12:
	lhu	$3,6($16)
	li	$2,49185			# 0xc021
	bne	$3,$2,$L3
	nop

	lbu	$3,8($16)
	li	$2,5			# 0x5
	bne	$3,$2,$L18
	li	$2,6			# 0x6

	addiu	$4,$17,%lo(l2tpInfo)
	move	$5,$0
	jal	memset
	li	$6,40			# 0x28

	lui	$2,%hi(state)
	sw	$0,%lo(state)($2)
	lbu	$3,8($16)
	li	$2,6			# 0x6
$L18:
	bne	$3,$2,$L14
	addiu	$4,$17,%lo(l2tpInfo)

	lhu	$3,20($4)
	lhu	$2,0($16)
	nop
	bne	$3,$2,$L14
	nop

	lhu	$3,22($4)
	lhu	$2,2($16)
	nop
	bne	$3,$2,$L14
	move	$5,$0

	jal	memset
	li	$6,40			# 0x28

	lui	$2,%hi(state)
	sw	$0,%lo(state)($2)
$L14:
	lbu	$3,8($16)
	li	$2,1			# 0x1
	bne	$3,$2,$L3
	nop

	addiu	$6,$16,8
	lhu	$5,10($16)
	li	$2,4			# 0x4
	j	$L15
	li	$4,5			# 0x5

$L17:
	lbu	$8,0($3)
	lbu	$7,1($3)
	bne	$8,$4,$L15
	addu	$2,$2,$7

	subu	$2,$2,$7
	lw	$3,2($3)
	lui	$2,%hi(l2tp_ppp_imagic)
	j	$L3
	sw	$3,%lo(l2tp_ppp_imagic)($2)

$L15:
	slt	$3,$2,$5
	bne	$3,$0,$L17
	addu	$3,$6,$2

$L3:
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
	.section	.text.fastpath_l2tp_single_write,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	fastpath_l2tp_single_write
	.type	fastpath_l2tp_single_write, @function
fastpath_l2tp_single_write:
	.frame	$sp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$16,24($sp)
	sltu	$4,$6,2
	move	$16,$6
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L20
	li	$2,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L20
	addiu	$4,$5,1
	.set	macro
	.set	reorder

	or	$3,$4,$5
	lw	$4,24($28)
	#nop
	and	$3,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L20
	addiu	$4,$sp,16
	.set	macro
	.set	reorder

	li	$6,1			# 0x1
 #APP
 # 596 "net/rtl/fastpath/fast_l2tp_core.c" 1
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
	bne	$6,$0,$L20
	li	$5,1			# 0x1
	.set	macro
	.set	reorder

	lb	$3,16($sp)
	lui	$2,%hi(fast_l2tp_fw)
	addiu	$3,$3,-48
	sw	$3,%lo(fast_l2tp_fw)($2)
	lui	$4,%hi(l2tpInfo)
	.set	noreorder
	.set	nomacro
	bne	$3,$5,$L21
	lui	$2,%hi(l2tp_tx_id_hook)
	.set	macro
	.set	reorder

	lui	$3,%hi(l2tp_tx_id)
	addiu	$3,$3,%lo(l2tp_tx_id)
	.set	noreorder
	.set	nomacro
	j	$L22
	sw	$3,%lo(l2tp_tx_id_hook)($2)
	.set	macro
	.set	reorder

$L21:
	sw	$0,%lo(l2tp_tx_id_hook)($2)
	addiu	$2,$4,%lo(l2tpInfo)
	sh	$0,20($2)
$L22:
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
	move	$2,$16
$L20:
	lw	$31,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	fastpath_l2tp_single_write
	.size	fastpath_l2tp_single_write, .-fastpath_l2tp_single_write
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
	move	$16,$4
	lw	$4,%lo(l2tpInfo+4)($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L28
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
$L28:
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
	beq	$2,$0,$L30
	nop

	jal	rtl_get_dev_by_name
	nop

	lui	$3,%hi(l2tpInfo+4)
	sw	$2,%lo(l2tpInfo+4)($3)
$L30:
	lw	$31,20($sp)
	nop
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	set_l2tp_device
	.size	set_l2tp_device, .-set_l2tp_device
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
	beq	$2,$0,$L32
	move	$17,$4
	.set	macro
	.set	reorder

	lw	$4,4($16)
	#nop
	beq	$4,$0,$L32
	jal	rtl_get_ppp_dev_name
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
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
	bne	$3,$0,$L32
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
$L32:
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
	.section	.text.fast_l2tp_rx,"ax",@progbits
	.align	2
	.globl	fast_l2tp_rx
	.set	nomips16
	.ent	fast_l2tp_rx
	.type	fast_l2tp_rx, @function
fast_l2tp_rx:
	.frame	$sp,48,$31		# vars= 0, regs= 8/0, args= 16, gp= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$22,40($sp)
	sw	$21,36($sp)
	sw	$20,32($sp)
	sw	$19,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	jal	rtl_ip_hdr
	move	$16,$4

	move	$4,$16
	lbu	$18,0($2)
	jal	rtl_get_skb_data
	move	$21,$2

	move	$4,$16
	jal	rtl_get_skb_len
	move	$19,$2

	sltu	$2,$2,40
	andi	$18,$18,0xf
	bne	$2,$0,$L35
	move	$17,$0

	sll	$18,$18,2
	addu	$18,$21,$18
	lhu	$2,0($18)
	li	$3,1701			# 0x6a5
	bne	$2,$3,$L35
	nop

	lhu	$3,2($18)
	nop
	bne	$3,$2,$L56
	move	$2,$17

	lui	$20,%hi(l2tpInfo)
	lw	$2,%lo(l2tpInfo)($20)
	nop
	bne	$2,$0,$L36
	nop

	jal	l2tp_tx_id
	move	$4,$16

$L36:
	lbu	$2,28($19)
	li	$18,2			# 0x2
	move	$4,$19
	andi	$3,$2,0x40
	movz	$18,$0,$3
	andi	$2,$2,0x2
	addu	$18,$18,$2
	move	$5,$18
	jal	fast_l2tp_tx_lcp_echo_reply
	move	$6,$16

	li	$3,1			# 0x1
	beq	$2,$3,$L35
	li	$17,1			# 0x1

	addiu	$22,$20,%lo(l2tpInfo)
	lhu	$2,20($22)
	nop
	beq	$2,$0,$L35
	move	$17,$0

	lhu	$2,22($22)
	nop
	beq	$2,$0,$L35
	addu	$3,$19,$18

	lbu	$2,36($3)
	nop
	bne	$2,$0,$L35
	nop

	lbu	$4,37($3)
	li	$3,33			# 0x21
	bne	$4,$3,$L35
	move	$17,$2

	lui	$3,%hi(l2tpInfo)
	lw	$3,%lo(l2tpInfo)($3)
	nop
	beq	$3,$0,$L56
	move	$2,$17

	lw	$4,16($22)
	lw	$3,16($21)
	nop
	bne	$4,$3,$L56
	nop

	lw	$2,4($22)
	nop
	bne	$2,$0,$L57
	addiu	$2,$20,%lo(l2tpInfo)

	lui	$4,%hi($LC1)
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC1)

	sw	$2,4($22)
	addiu	$2,$20,%lo(l2tpInfo)
$L57:
	lw	$5,4($2)
	nop
	beq	$5,$0,$L35
	move	$17,$0

	addiu	$18,$18,38
	addu	$19,$19,$18
	lbu	$2,9($19)
	li	$3,6			# 0x6
	beq	$2,$3,$L40
	li	$3,17			# 0x11

	bne	$2,$3,$L56
	move	$2,$17

$L40:
	jal	rtl_set_skb_dev
	move	$4,$16

	move	$5,$18
	jal	skb_pull
	move	$4,$16

	jal	rtl_skb_reset_network_header
	move	$4,$16

	addiu	$20,$20,%lo(l2tpInfo)
	jal	rtl_skb_reset_transport_header
	move	$4,$16

	lw	$4,4($20)
	nop
	beq	$4,$0,$L35
	move	$17,$0

	jal	rtl_get_ppp_dev_priv
	nop

	beq	$2,$0,$L56
	move	$2,$17

	lw	$4,4($20)
	jal	rtl_get_ppp_dev_priv
	nop

	move	$4,$16
	jal	rtl_get_skb_len
	move	$18,$2

	move	$6,$2
	move	$4,$18
	jal	rtl_inc_ppp_stats
	move	$5,$0

$L35:
	move	$2,$17
$L56:
	lw	$31,44($sp)
	lw	$22,40($sp)
	lw	$21,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	fast_l2tp_rx
	.size	fast_l2tp_rx, .-fast_l2tp_rx
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
	lui	$3,%hi(l2tpInfo)
	addiu	$3,$3,%lo(l2tpInfo)
	lhu	$2,38($3)
	lw	$3,8($3)
	xori	$2,$2,0x1
	movn	$3,$0,$2
	j	$31
	move	$2,$3

	.set	macro
	.set	reorder
	.end	get_fast_l2tp_lastxmit
	.size	get_fast_l2tp_lastxmit, .-get_fast_l2tp_lastxmit
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
	beq	$4,$3,$L62
	move	$2,$0

	jal	rtl_ip_hdr
	move	$4,$16

	lw	$2,12($2)
	li	$3,-256			# 0xffffffffffffff00
	and	$2,$3,$2
	li	$3,-172032000			# 0xfffffffff5bf0000
	ori	$3,$3,0xc000
	addu	$2,$2,$3
	sltu	$2,$0,$2
$L62:
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
	.frame	$sp,80,$31		# vars= 40, regs= 6/0, args= 16, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-80
	sw	$31,76($sp)
	sw	$20,72($sp)
	sw	$19,68($sp)
	sw	$18,64($sp)
	sw	$17,60($sp)
	sw	$16,56($sp)
	lui	$2,%hi(fast_l2tp_fw)
	lw	$3,%lo(fast_l2tp_fw)($2)
	move	$16,$4
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L65
	move	$2,$0
	.set	macro
	.set	reorder

	lui	$17,%hi(l2tpInfo)
	addiu	$3,$17,%lo(l2tpInfo)
	lhu	$5,20($3)
	#nop
	beq	$5,$0,$L65
	lhu	$5,22($3)
	#nop
	beq	$5,$0,$L65
	lw	$5,%lo(l2tpInfo)($17)
	#nop
	beq	$5,$0,$L65
	lhu	$5,38($3)
	li	$3,1			# 0x1
	bne	$5,$3,$L65
	jal	rtl_ip_hdr
	move	$4,$16
	lhu	$18,2($2)
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_headroom
	move	$19,$2
	.set	macro
	.set	reorder

	sltu	$2,$2,52
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L80
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
	bne	$2,$0,$L80
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
	beq	$2,$0,$L81
	move	$4,$16
	.set	macro
	.set	reorder

$L80:
	.set	noreorder
	.set	nomacro
	jal	skb_realloc_headroom
	li	$5,52			# 0x34
	.set	macro
	.set	reorder

	move	$20,$2
	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L65
	move	$2,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	consume_skb
	move	$4,$16
	.set	macro
	.set	reorder

	move	$16,$20
	move	$4,$16
$L81:
	lbu	$19,1($19)
	.set	noreorder
	.set	nomacro
	jal	skb_push
	li	$5,52			# 0x34
	.set	macro
	.set	reorder

	lui	$5,%hi(l2tpInfo+24)
	li	$6,14			# 0xe
	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$5,%lo(l2tpInfo+24)
	.set	macro
	.set	reorder

	lw	$3,36($sp)
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
	sw	$2,36($sp)
	li	$2,16384			# 0x4000
	sh	$2,42($sp)
	li	$2,17			# 0x11
	sb	$2,45($sp)
	addiu	$2,$17,%lo(l2tpInfo)
	lw	$3,12($2)
	lw	$2,16($2)
	move	$5,$0
	move	$4,$16
	sw	$2,48($sp)
	li	$2,64			# 0x40
	sw	$3,52($sp)
	sb	$2,44($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$19,37($sp)
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
	sh	$0,40($sp)
	lw	$5,36($sp)
	lw	$6,40($sp)
	sh	$0,46($sp)
	lw	$4,44($sp)
	srl	$7,$5,22
	addu	$5,$6,$5
	sltu	$6,$5,$6
	addu	$5,$5,$4
	addu	$6,$5,$6
	lw	$5,48($sp)
	sltu	$4,$6,$4
	addiu	$2,$sp,36
	addu	$6,$6,$5
	addu	$4,$6,$4
	andi	$7,$7,0x3c
	sltu	$3,$4,$5
	addu	$7,$2,$7
	addu	$3,$3,$4
	addiu	$2,$sp,52
$L68:
	lw	$4,0($2)
	addiu	$2,$2,4
	addu	$3,$4,$3
	sltu	$4,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$7,$L68
	addu	$3,$4,$3
	.set	macro
	.set	reorder

	move	$4,$16
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
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sh	$2,46($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,14
	addiu	$5,$sp,36
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,20			# 0x14
	.set	macro
	.set	reorder

	addiu	$18,$18,18
	li	$2,1701			# 0x6a5
	sh	$2,16($sp)
	sh	$2,18($sp)
	sh	$18,20($sp)
	li	$2,2			# 0x2
	addiu	$18,$17,%lo(l2tpInfo)
	sh	$2,24($sp)
	lhu	$2,20($18)
	move	$4,$16
	sh	$0,22($sp)
	sh	$2,26($sp)
	lhu	$2,22($18)
	#nop
	sh	$2,28($sp)
	li	$2,-253			# 0xffffffffffffff03
	sh	$2,30($sp)
	li	$2,33			# 0x21
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sh	$2,32($sp)
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

	lw	$5,%lo(l2tpInfo)($17)
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
	beq	$2,$3,$L69
	lui	$2,%hi(jiffies)
	.set	macro
	.set	reorder

	lw	$2,%lo(jiffies)($2)
	#nop
	sw	$2,8($18)
$L69:
	addiu	$17,$17,%lo(l2tpInfo)
	lw	$4,4($17)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L82
	lui	$2,%hi(gQosEnabled)
	.set	macro
	.set	reorder

	jal	rtl_get_ppp_dev_priv
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L82
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
$L82:
	lw	$2,%lo(gQosEnabled)($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L71
	move	$4,$16
	.set	macro
	.set	reorder

	jal	dev_queue_xmit
	.set	noreorder
	.set	nomacro
	j	$L65
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L71:
	jal	rtl_call_skb_ndo_start_xmit
	li	$2,1			# 0x1
$L65:
	lw	$31,76($sp)
	lw	$20,72($sp)
	lw	$19,68($sp)
	lw	$18,64($sp)
	lw	$17,60($sp)
	lw	$16,56($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,80
	.set	macro
	.set	reorder

	.end	fast_l2tp_to_wan
	.size	fast_l2tp_to_wan, .-fast_l2tp_to_wan
	.section	.rodata.str1.4
	.align	2
$LC3:
	.ascii	"fast_l2tp\000"
	.section	.init.text,"ax",@progbits
	.align	2
	.globl	fast_l2tp_init
	.set	nomips16
	.ent	fast_l2tp_init
	.type	fast_l2tp_init, @function
fast_l2tp_init:
	.frame	$sp,32,$31		# vars= 0, regs= 1/0, args= 24, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	lui	$4,%hi($LC3)
	lui	$6,%hi(proc_root)
	lui	$7,%hi(fastpath_l2tp_proc_fops)
	addiu	$7,$7,%lo(fastpath_l2tp_proc_fops)
	addiu	$4,$4,%lo($LC3)
	move	$5,$0
	addiu	$6,$6,%lo(proc_root)
	jal	proc_create_data
	sw	$0,16($sp)

	lui	$3,%hi(res1)
	sw	$2,%lo(res1)($3)
	lui	$2,%hi(l2tp_tx_id_hook)
	sw	$0,%lo(l2tp_tx_id_hook)($2)
	lui	$4,%hi(l2tpInfo)
	lui	$2,%hi(fast_l2tp_fw)
	sw	$0,%lo(fast_l2tp_fw)($2)
	addiu	$4,$4,%lo(l2tpInfo)
	lui	$2,%hi(state)
	move	$5,$0
	li	$6,40			# 0x28
	jal	memset
	sw	$0,%lo(state)($2)

	lw	$31,28($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	fast_l2tp_init
	.size	fast_l2tp_init, .-fast_l2tp_init
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
	lw	$2,%lo(res1)($16)
	nop
	beq	$2,$0,$L84
	lui	$4,%hi($LC3)

	lui	$5,%hi(proc_root)
	addiu	$4,$4,%lo($LC3)
	jal	remove_proc_entry
	addiu	$5,$5,%lo(proc_root)

	sw	$0,%lo(res1)($16)
$L84:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fast_l2tp_exit
	.size	fast_l2tp_exit, .-fast_l2tp_exit
	.globl	fastpath_l2tp_proc_fops
	.section	.data.fastpath_l2tp_proc_fops,"aw",@progbits
	.align	2
	.type	fastpath_l2tp_proc_fops, @object
	.size	fastpath_l2tp_proc_fops, 108
fastpath_l2tp_proc_fops:
	.space	4
	.word	seq_lseek
	.word	seq_read
	.word	fastpath_l2tp_single_write
	.space	28
	.word	fastpath_l2tp_single_open
	.space	4
	.word	single_release
	.space	52
	.globl	l2tp_ppp_imagic
	.section	.bss.l2tp_ppp_imagic,"aw",@nobits
	.align	2
	.type	l2tp_ppp_imagic, @object
	.size	l2tp_ppp_imagic, 4
l2tp_ppp_imagic:
	.space	4
	.globl	state
	.section	.bss.state,"aw",@nobits
	.align	2
	.type	state, @object
	.size	state, 4
state:
	.space	4
	.globl	l2tpInfo
	.section	.bss.l2tpInfo,"aw",@nobits
	.align	2
	.type	l2tpInfo, @object
	.size	l2tpInfo, 40
l2tpInfo:
	.space	40
	.globl	fast_l2tp_fw
	.section	.bss.fast_l2tp_fw,"aw",@nobits
	.align	2
	.type	fast_l2tp_fw, @object
	.size	fast_l2tp_fw, 4
fast_l2tp_fw:
	.space	4
	.globl	l2tp_tx_id_hook
	.section	.bss.l2tp_tx_id_hook,"aw",@nobits
	.align	2
	.type	l2tp_tx_id_hook, @object
	.size	l2tp_tx_id_hook, 4
l2tp_tx_id_hook:
	.space	4
	.section	.bss.res1,"aw",@nobits
	.align	2
	.type	res1, @object
	.size	res1, 4
res1:
	.space	4
	.ident	"GCC: (Realtek RSDK-4.6.4 Build 2080) 4.6.4"
