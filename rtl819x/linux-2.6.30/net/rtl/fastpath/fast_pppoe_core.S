	.file	1 "fast_pppoe_core.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.fastpath_pppoe_single_write,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	fastpath_pppoe_single_write
	.type	fastpath_pppoe_single_write, @function
fastpath_pppoe_single_write:
	.frame	$sp,16,$31		# vars= 8, regs= 1/0, args= 0, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-16
	sw	$31,12($sp)
	sltu	$4,$6,2
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L2
	move	$2,$6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$5,$0,$L3
	li	$2,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	addiu	$4,$5,1
	or	$3,$4,$5
	lw	$4,24($28)
	and	$3,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L2
	move	$4,$sp
	.set	macro
	.set	reorder

	li	$6,1			# 0x1
 #APP
 # 65 "net/rtl/fastpath/fast_pppoe_core.c" 1
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
	bne	$6,$0,$L2
	lb	$4,0($sp)
	.set	macro
	.set	reorder

	lui	$3,%hi(fast_pppoe_fw)
	addiu	$4,$4,-48
	.set	noreorder
	.set	nomacro
	j	$L3
	sw	$4,%lo(fast_pppoe_fw)($3)
	.set	macro
	.set	reorder

$L2:
	li	$2,-14			# 0xfffffffffffffff2
$L3:
	lw	$31,12($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,16
	.set	macro
	.set	reorder

	.end	fastpath_pppoe_single_write
	.size	fastpath_pppoe_single_write, .-fastpath_pppoe_single_write
	.section	.text.find_pppoe_info,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	find_pppoe_info
	.type	find_pppoe_info, @function
find_pppoe_info:
	.frame	$sp,72,$31		# vars= 16, regs= 10/0, args= 16, gp= 0
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
	lw	$20,88($sp)
	lw	$19,92($sp)
	lw	$18,96($sp)
	andi	$6,$6,0xffff
	lui	$23,%hi(fast_pppoe_info+54)
	lui	$22,%hi(fast_pppoe_info)
	move	$17,$4
	move	$16,$5
	sw	$6,16($sp)
	addiu	$23,$23,%lo(fast_pppoe_info+54)
	addiu	$22,$22,%lo(fast_pppoe_info)
	move	$21,$0
	li	$fp,88			# 0x58
$L15:
	lhu	$2,0($23)
	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L20
	addiu	$21,$21,1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L8
	mul	$2,$21,$fp
	.set	macro
	.set	reorder

	move	$5,$17
	addiu	$4,$2,16
	addu	$4,$22,$4
 #APP
 # 77 "/home/janice_yu/branch-rtl819x-sdk-v3.4.9.2/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$3,($4)
1:	lbu	$1,($5)
	addiu	$4,1
	bne	$1,$3,2f
	addiu	$5,1
	bnez	$3,1b
	lbu	$3,($4)
	move	$3,$1
2:	subu	$3,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L9
	addu	$2,$22,$2
	.set	macro
	.set	reorder

$L8:
	.set	noreorder
	.set	nomacro
	beq	$16,$0,$L10
	mul	$3,$21,$fp
	.set	macro
	.set	reorder

	move	$5,$16
	addu	$2,$3,$22
	move	$4,$2
 #APP
 # 77 "/home/janice_yu/branch-rtl819x-sdk-v3.4.9.2/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$3,($4)
1:	lbu	$1,($5)
	addiu	$4,1
	bne	$1,$3,2f
	addiu	$5,1
	bnez	$3,1b
	lbu	$3,($4)
	move	$3,$1
2:	subu	$3,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L21
	lw	$31,68($sp)
	.set	macro
	.set	reorder

$L10:
	.set	noreorder
	.set	nomacro
	beq	$18,$0,$L11
	move	$5,$18
	.set	macro
	.set	reorder

	mul	$3,$21,$fp
	li	$6,6			# 0x6
	sw	$7,20($sp)
	addu	$4,$22,$3
	addiu	$4,$4,38
	.set	noreorder
	.set	nomacro
	jal	memcmp
	sw	$3,24($sp)
	.set	macro
	.set	reorder

	lw	$3,24($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L18
	lw	$7,20($sp)
	.set	macro
	.set	reorder

$L11:
	.set	noreorder
	.set	nomacro
	beq	$19,$0,$L12
	mul	$3,$21,$fp
	.set	macro
	.set	reorder

	move	$5,$19
	li	$6,6			# 0x6
	sw	$7,20($sp)
	addiu	$4,$3,32
	addu	$4,$22,$4
	.set	noreorder
	.set	nomacro
	jal	memcmp
	sw	$3,24($sp)
	.set	macro
	.set	reorder

	lw	$3,24($sp)
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L12
	lw	$7,20($sp)
	.set	macro
	.set	reorder

$L18:
	.set	noreorder
	.set	nomacro
	j	$L9
	addu	$2,$22,$3
	.set	macro
	.set	reorder

$L12:
	beq	$20,$0,$L13
	lw	$2,-6($23)
	.set	noreorder
	.set	nomacro
	beq	$2,$20,$L22
	li	$3,88			# 0x58
	.set	macro
	.set	reorder

$L13:
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L23
	lw	$2,16($sp)
	.set	macro
	.set	reorder

	lw	$2,-10($23)
	.set	noreorder
	.set	nomacro
	beq	$2,$7,$L22
	li	$3,88			# 0x58
	.set	macro
	.set	reorder

	lw	$2,16($sp)
$L23:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L7
	lw	$3,16($sp)
	.set	macro
	.set	reorder

	lhu	$2,-2($23)
	.set	noreorder
	.set	nomacro
	bnel	$2,$3,$L20
	addiu	$21,$21,1
	.set	macro
	.set	reorder

	li	$3,88			# 0x58
$L22:
	mul	$4,$21,$3
	lui	$2,%hi(fast_pppoe_info)
	addiu	$2,$2,%lo(fast_pppoe_info)
	.set	noreorder
	.set	nomacro
	j	$L9
	addu	$2,$4,$2
	.set	macro
	.set	reorder

$L7:
	addiu	$21,$21,1
$L20:
	li	$2,16			# 0x10
	.set	noreorder
	.set	nomacro
	bne	$21,$2,$L15
	addiu	$23,$23,88
	.set	macro
	.set	reorder

	move	$2,$0
$L9:
	lw	$31,68($sp)
$L21:
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

	.end	find_pppoe_info
	.size	find_pppoe_info, .-find_pppoe_info
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"ppp\000"
	.section	.text.fast_pppoe_xmit,"ax",@progbits
	.align	2
	.globl	fast_pppoe_xmit
	.set	nomips16
	.ent	fast_pppoe_xmit
	.type	fast_pppoe_xmit, @function
fast_pppoe_xmit:
	.frame	$sp,56,$31		# vars= 0, regs= 5/0, args= 32, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$19,48($sp)
	sw	$18,44($sp)
	sw	$17,40($sp)
	sw	$16,36($sp)
	lui	$2,%hi(fast_pppoe_fw)
	lw	$2,%lo(fast_pppoe_fw)($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L25
	move	$16,$4
	.set	macro
	.set	reorder

	lui	$2,%hi(gQosEnabled)
	lw	$2,%lo(gQosEnabled)($2)
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L26
	move	$2,$0
	.set	macro
	.set	reorder

	jal	rtl_get_skb_dev
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L26
	move	$2,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_dev
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$3,%hi($LC0)
	addiu	$3,$3,%lo($LC0)
	li	$4,3			# 0x3
 #APP
 # 108 "/home/janice_yu/branch-rtl819x-sdk-v3.4.9.2/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$5,($2)
	beqz	$4,2f
	lbu	$1,($3)
	subu	$4,1
	bne	$1,$5,3f
	addiu	$2,1
	bnez	$5,1b
	addiu	$3,1
2:
	move	$5,$1
3:	subu	$5,$1
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L26
	move	$2,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_headroom
	move	$4,$16
	.set	macro
	.set	reorder

	sltu	$2,$2,22
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L26
	move	$2,$0
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
	bne	$2,$0,$L26
	move	$2,$0
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
	bne	$2,$0,$L26
	move	$2,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_dev
	move	$4,$16
	.set	macro
	.set	reorder

	move	$4,$2
	move	$5,$0
	move	$6,$0
	move	$7,$0
	sw	$0,16($sp)
	sw	$0,20($sp)
	.set	noreorder
	.set	nomacro
	jal	find_pppoe_info
	sw	$0,24($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L25
	move	$17,$2
	.set	macro
	.set	reorder

	lb	$2,16($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L26
	move	$2,$0
	.set	macro
	.set	reorder

	lb	$2,0($17)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L26
	move	$2,$0
	.set	macro
	.set	reorder

	lhu	$2,52($17)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L26
	move	$2,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_dev
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	move	$4,$16
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	move	$19,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L25
	move	$18,$2
	.set	macro
	.set	reorder

	li	$5,2			# 0x2
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_push
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$16
	.set	macro
	.set	reorder

	li	$3,33			# 0x21
	sh	$3,0($2)
	li	$5,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_push
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$16
	.set	macro
	.set	reorder

	lbu	$3,0($2)
	li	$4,1			# 0x1
	sb	$0,1($2)
	ins	$3,$4,4,4
	ins	$3,$4,0,4
	sb	$3,0($2)
	lbu	$3,52($17)
	addiu	$19,$19,2
	andi	$19,$19,0xffff
	sb	$3,2($2)
	lbu	$4,53($17)
	srl	$3,$19,8
	sb	$3,4($2)
	sb	$4,3($2)
	sb	$19,5($2)
	move	$5,$18
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dev
	move	$4,$16
	.set	macro
	.set	reorder

	li	$5,2			# 0x2
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_push
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$16
	.set	macro
	.set	reorder

	li	$3,-30620			# 0xffffffffffff8864
	sh	$3,0($2)
	li	$5,12			# 0xc
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_push
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$16
	.set	macro
	.set	reorder

	move	$4,$2
	addiu	$5,$17,38
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$16
	.set	macro
	.set	reorder

	lw	$5,396($18)
	addiu	$4,$2,6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	ip_finish_output4
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	j	$L26
	sw	$3,60($17)
	.set	macro
	.set	reorder

$L25:
	move	$2,$0
$L26:
	lw	$31,52($sp)
	lw	$19,48($sp)
	lw	$18,44($sp)
	lw	$17,40($sp)
	lw	$16,36($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,56
	.set	macro
	.set	reorder

	.end	fast_pppoe_xmit
	.size	fast_pppoe_xmit, .-fast_pppoe_xmit
	.section	.text.get_pppoe_last_rx_tx,"ax",@progbits
	.align	2
	.globl	get_pppoe_last_rx_tx
	.set	nomips16
	.ent	get_pppoe_last_rx_tx
	.type	get_pppoe_last_rx_tx, @function
get_pppoe_last_rx_tx:
	.frame	$sp,40,$31		# vars= 0, regs= 1/0, args= 32, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	lw	$2,56($sp)
	andi	$6,$6,0xffff
	sw	$2,16($sp)
	lw	$2,60($sp)
	sw	$2,20($sp)
	lw	$2,64($sp)
	jal	find_pppoe_info
	sw	$2,24($sp)

	beq	$2,$0,$L29
	lw	$3,68($sp)

	lw	$4,56($2)
	sw	$4,0($3)
	lw	$3,60($2)
	lw	$2,72($sp)
	sw	$3,0($2)
$L29:
	lw	$31,36($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	get_pppoe_last_rx_tx
	.size	get_pppoe_last_rx_tx, .-get_pppoe_last_rx_tx
	.section	.text.check_and_restore_pppoe_hdr,"ax",@progbits
	.align	2
	.globl	check_and_restore_pppoe_hdr
	.set	nomips16
	.ent	check_and_restore_pppoe_hdr
	.type	check_and_restore_pppoe_hdr, @function
check_and_restore_pppoe_hdr:
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
	lui	$2,%hi(fast_pppoe_fw)
	lw	$2,%lo(fast_pppoe_fw)($2)
	beq	$2,$0,$L33
	move	$16,$4

	jal	rtl_get_skb_pppoe_flag
	nop

	beq	$2,$0,$L34
	lw	$31,28($sp)

	li	$5,8			# 0x8
	jal	rtl_skb_push
	move	$4,$16

	jal	rtl_get_skb_data
	move	$4,$16

	move	$5,$2
	jal	rtl_set_skb_network_header
	move	$4,$16

	jal	rtl_get_skb_data
	move	$4,$16

	move	$5,$2
	jal	rtl_set_skb_transport_header
	move	$4,$16

	jal	rtl_get_skb_dev
	move	$4,$16

	lw	$3,128($2)
	move	$4,$16
	addiu	$3,$3,-1
	jal	rtl_get_skb_dev
	sw	$3,128($2)

	lw	$18,136($2)
	move	$4,$16
	jal	rtl_get_skb_len
	move	$17,$2

	subu	$2,$18,$2
	move	$4,$16
	jal	rtl_get_skb_rx_dev
	sw	$2,136($17)

	move	$5,$2
	jal	rtl_set_skb_dev
	move	$4,$16

	move	$4,$16
	jal	rtl_set_skb_protocol
	li	$5,34916			# 0x8864

	move	$4,$16
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	move	$5,$0
	j	rtl_set_skb_pppoe_flag
	addiu	$sp,$sp,32

$L33:
	lw	$31,28($sp)
$L34:
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	check_and_restore_pppoe_hdr
	.size	check_and_restore_pppoe_hdr, .-check_and_restore_pppoe_hdr
	.section	.text.check_and_pull_pppoe_hdr,"ax",@progbits
	.align	2
	.globl	check_and_pull_pppoe_hdr
	.set	nomips16
	.ent	check_and_pull_pppoe_hdr
	.type	check_and_pull_pppoe_hdr, @function
check_and_pull_pppoe_hdr:
	.frame	$sp,64,$31		# vars= 8, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-64
	sw	$31,60($sp)
	sw	$fp,56($sp)
	sw	$23,52($sp)
	sw	$22,48($sp)
	sw	$21,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	lui	$2,%hi(fast_pppoe_fw)
	lw	$2,%lo(fast_pppoe_fw)($2)
	beq	$2,$0,$L36
	move	$16,$4

	jal	rtl_set_skb_pppoe_flag
	move	$5,$0

	jal	rtl_skb_mac_header
	move	$4,$16

	beq	$2,$0,$L36
	move	$17,$2

	lhu	$3,12($2)
	li	$2,34916			# 0x8864
	bne	$3,$2,$L40
	move	$2,$0

	lhu	$3,20($17)
	li	$2,33			# 0x21
	bne	$3,$2,$L40
	move	$2,$0

	lhu	$3,16($17)
	lui	$19,%hi(fast_pppoe_info+54)
	lui	$21,%hi(fast_pppoe_info)
	addiu	$19,$19,%lo(fast_pppoe_info+54)
	addiu	$21,$21,%lo(fast_pppoe_info)
	addiu	$fp,$17,6
	move	$18,$0
	li	$23,88			# 0x58
	li	$22,16			# 0x10
$L39:
	lhu	$2,0($19)
	beql	$2,$0,$L44
	addiu	$18,$18,1

	lhu	$2,-2($19)
	bnel	$2,$3,$L44
	addiu	$18,$18,1

	mul	$20,$18,$23
	move	$5,$fp
	li	$6,6			# 0x6
	sw	$3,16($sp)
	addu	$4,$21,$20
	jal	memcmp
	addiu	$4,$4,38

	bne	$2,$0,$L37
	lw	$3,16($sp)

	addu	$20,$21,$20
	bne	$20,$0,$L43
	move	$2,$0

	j	$L45
	lw	$31,60($sp)

$L37:
	addiu	$18,$18,1
$L44:
	bne	$18,$22,$L39
	addiu	$19,$19,88

	j	$L40
	move	$2,$0

$L43:
	jal	rtl_get_dev_by_name
	addiu	$4,$20,16

	beq	$2,$0,$L36
	move	$18,$2

	addiu	$17,$17,22
	move	$5,$17
	jal	rtl_set_skb_network_header
	move	$4,$16

	move	$5,$17
	jal	rtl_set_skb_transport_header
	move	$4,$16

	li	$5,8			# 0x8
	jal	rtl_skb_pull
	move	$4,$16

	jal	rtl_get_skb_dev
	move	$4,$16

	move	$5,$2
	jal	rtl_set_skb_rx_dev
	move	$4,$16

	move	$4,$16
	jal	rtl_set_skb_protocol
	li	$5,2048			# 0x800

	move	$5,$18
	jal	rtl_set_skb_dev
	move	$4,$16

	jal	rtl_get_skb_dev
	move	$4,$16

	lw	$3,128($2)
	move	$4,$16
	addiu	$3,$3,1
	jal	rtl_get_skb_dev
	sw	$3,128($2)

	lw	$18,136($2)
	move	$4,$16
	jal	rtl_get_skb_len
	move	$17,$2

	addu	$2,$2,$18
	sw	$2,136($17)
	move	$4,$16
	jal	rtl_set_skb_pppoe_flag
	li	$5,1			# 0x1

	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	li	$2,1			# 0x1
	j	$L40
	sw	$3,56($20)

$L36:
	move	$2,$0
$L40:
	lw	$31,60($sp)
$L45:
	lw	$fp,56($sp)
	lw	$23,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,64

	.set	macro
	.set	reorder
	.end	check_and_pull_pppoe_hdr
	.size	check_and_pull_pppoe_hdr, .-check_and_pull_pppoe_hdr
	.section	.text.set_pppoe_info,"ax",@progbits
	.align	2
	.globl	set_pppoe_info
	.set	nomips16
	.ent	set_pppoe_info
	.type	set_pppoe_info, @function
set_pppoe_info:
	.frame	$sp,72,$31		# vars= 0, regs= 9/0, args= 32, gp= 0
	.mask	0x80ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-72
	sw	$31,68($sp)
	sw	$23,64($sp)
	sw	$22,60($sp)
	sw	$21,56($sp)
	sw	$20,52($sp)
	sw	$19,48($sp)
	sw	$18,44($sp)
	sw	$17,40($sp)
	sw	$16,36($sp)
	lw	$18,88($sp)
	lw	$16,92($sp)
	lw	$17,96($sp)
	move	$19,$5
	andi	$22,$6,0xffff
	move	$5,$0
	move	$6,$0
	sw	$18,16($sp)
	sw	$16,20($sp)
	sw	$17,24($sp)
	move	$20,$4
	.set	noreorder
	.set	nomacro
	jal	find_pppoe_info
	move	$21,$7
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L47
	move	$23,$2
	.set	macro
	.set	reorder

	lui	$3,%hi(fast_pppoe_info+54)
	addiu	$3,$3,%lo(fast_pppoe_info+54)
	move	$2,$0
	li	$4,16			# 0x10
	lhu	$5,0($3)
$L61:
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L48
	addiu	$3,$3,88
	.set	macro
	.set	reorder

	li	$3,88			# 0x58
	mul	$4,$2,$3
	lui	$23,%hi(fast_pppoe_info)
	addiu	$23,$23,%lo(fast_pppoe_info)
	addu	$23,$4,$23
	.set	noreorder
	.set	nomacro
	bne	$23,$0,$L47
	lw	$31,68($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L62
	lw	$23,64($sp)
	.set	macro
	.set	reorder

$L48:
	addiu	$2,$2,1
	.set	noreorder
	.set	nomacro
	bnel	$2,$4,$L61
	lhu	$5,0($3)
	.set	macro
	.set	reorder

	lui	$23,%hi(fast_pppoe_info)
	addiu	$4,$23,%lo(fast_pppoe_info)
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,1408			# 0x580
	.set	macro
	.set	reorder

	addiu	$23,$23,%lo(fast_pppoe_info)
$L47:
	.set	noreorder
	.set	nomacro
	bnel	$22,$0,$L51
	sh	$22,52($23)
	.set	macro
	.set	reorder

$L51:
	.set	noreorder
	.set	nomacro
	bnel	$21,$0,$L52
	sw	$21,44($23)
	.set	macro
	.set	reorder

$L52:
	.set	noreorder
	.set	nomacro
	bnel	$18,$0,$L53
	sw	$18,48($23)
	.set	macro
	.set	reorder

$L53:
	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L54
	addiu	$2,$23,16
	.set	macro
	.set	reorder

 #APP
 # 27 "/home/janice_yu/branch-rtl819x-sdk-v3.4.9.2/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($20)
	addiu	$20,1
	sb	$1,($2)
	bnez	$1,1b
	addiu	$2,1
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
$L54:
	.set	noreorder
	.set	nomacro
	beq	$19,$0,$L55
	move	$2,$23
	.set	macro
	.set	reorder

 #APP
 # 27 "/home/janice_yu/branch-rtl819x-sdk-v3.4.9.2/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($19)
	addiu	$19,1
	sb	$1,($2)
	bnez	$1,1b
	addiu	$2,1
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
$L55:
	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L56
	move	$5,$17
	.set	macro
	.set	reorder

	addiu	$4,$23,38
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

$L56:
	.set	noreorder
	.set	nomacro
	beq	$16,$0,$L57
	move	$5,$16
	.set	macro
	.set	reorder

	addiu	$4,$23,32
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

$L57:
	li	$2,1			# 0x1
	sh	$2,54($23)
	lw	$31,68($sp)
	lw	$23,64($sp)
$L62:
	lw	$22,60($sp)
	lw	$21,56($sp)
	lw	$20,52($sp)
	lw	$19,48($sp)
	lw	$18,44($sp)
	lw	$17,40($sp)
	lw	$16,36($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,72
	.set	macro
	.set	reorder

	.end	set_pppoe_info
	.size	set_pppoe_info, .-set_pppoe_info
	.section	.text.clear_pppoe_info,"ax",@progbits
	.align	2
	.globl	clear_pppoe_info
	.set	nomips16
	.ent	clear_pppoe_info
	.type	clear_pppoe_info, @function
clear_pppoe_info:
	.frame	$sp,56,$31		# vars= 0, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$fp,48($sp)
	sw	$23,44($sp)
	sw	$22,40($sp)
	sw	$21,36($sp)
	sw	$20,32($sp)
	sw	$19,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	lui	$16,%hi(fast_pppoe_info)
	lw	$22,76($sp)
	lw	$21,80($sp)
	addiu	$16,$16,%lo(fast_pppoe_info)
	move	$20,$4
	move	$19,$5
	andi	$fp,$6,0xffff
	move	$17,$0
	move	$18,$16
	li	$23,16			# 0x10
$L69:
	lhu	$2,54($16)
	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L72
	addiu	$17,$17,1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L65
	li	$2,88			# 0x58
	.set	macro
	.set	reorder

	mul	$2,$17,$2
	move	$3,$20
	addiu	$2,$2,16
	addu	$2,$18,$2
 #APP
 # 77 "/home/janice_yu/branch-rtl819x-sdk-v3.4.9.2/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$4,($2)
1:	lbu	$1,($3)
	addiu	$2,1
	bne	$1,$4,2f
	addiu	$3,1
	bnez	$4,1b
	lbu	$4,($2)
	move	$4,$1
2:	subu	$4,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L65
	move	$4,$16
	.set	macro
	.set	reorder

	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

$L65:
	.set	noreorder
	.set	nomacro
	beq	$19,$0,$L66
	li	$2,88			# 0x58
	.set	macro
	.set	reorder

	mul	$3,$17,$2
	addu	$2,$3,$18
	move	$3,$19
 #APP
 # 77 "/home/janice_yu/branch-rtl819x-sdk-v3.4.9.2/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$4,($2)
1:	lbu	$1,($3)
	addiu	$2,1
	bne	$1,$4,2f
	addiu	$3,1
	bnez	$4,1b
	lbu	$4,($2)
	move	$4,$1
2:	subu	$4,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L66
	move	$4,$16
	.set	macro
	.set	reorder

	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

$L66:
	.set	noreorder
	.set	nomacro
	beq	$21,$0,$L67
	li	$4,88			# 0x58
	.set	macro
	.set	reorder

	mul	$2,$17,$4
	move	$5,$21
	li	$6,6			# 0x6
	addu	$4,$2,$18
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$4,$4,38
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L67
	move	$4,$16
	.set	macro
	.set	reorder

	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

$L67:
	.set	noreorder
	.set	nomacro
	beq	$22,$0,$L68
	li	$4,88			# 0x58
	.set	macro
	.set	reorder

	mul	$4,$17,$4
	move	$5,$22
	li	$6,6			# 0x6
	addiu	$4,$4,32
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addu	$4,$18,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L73
	lhu	$2,52($16)
	.set	macro
	.set	reorder

	move	$4,$16
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

$L68:
	lhu	$2,52($16)
$L73:
	.set	noreorder
	.set	nomacro
	bnel	$2,$fp,$L72
	addiu	$17,$17,1
	.set	macro
	.set	reorder

	move	$4,$16
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

	addiu	$17,$17,1
$L72:
	.set	noreorder
	.set	nomacro
	bne	$17,$23,$L69
	addiu	$16,$16,88
	.set	macro
	.set	reorder

	lw	$31,52($sp)
	lw	$fp,48($sp)
	lw	$23,44($sp)
	lw	$22,40($sp)
	lw	$21,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,56
	.set	macro
	.set	reorder

	.end	clear_pppoe_info
	.size	clear_pppoe_info, .-clear_pppoe_info
	.section	.rodata.str1.4
	.align	2
$LC1:
	.ascii	"fast_pppoe\000"
	.section	.exit.text,"ax",@progbits
	.align	2
	.globl	fast_pppoe_exit
	.set	nomips16
	.ent	fast_pppoe_exit
	.type	fast_pppoe_exit, @function
fast_pppoe_exit:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$16,%hi(fast_pppoe_proc)
	lw	$2,%lo(fast_pppoe_proc)($16)
	beq	$2,$0,$L75
	lui	$4,%hi($LC1)

	lui	$5,%hi(proc_root)
	addiu	$4,$4,%lo($LC1)
	jal	remove_proc_entry
	addiu	$5,$5,%lo(proc_root)

	sw	$0,%lo(fast_pppoe_proc)($16)
$L75:
	lw	$31,20($sp)
	lw	$16,16($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fast_pppoe_exit
	.size	fast_pppoe_exit, .-fast_pppoe_exit
	.section	.init.text,"ax",@progbits
	.align	2
	.globl	fast_pppoe_init
	.set	nomips16
	.ent	fast_pppoe_init
	.type	fast_pppoe_init, @function
fast_pppoe_init:
	.frame	$sp,32,$31		# vars= 0, regs= 1/0, args= 24, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	lui	$4,%hi($LC1)
	lui	$6,%hi(proc_root)
	lui	$7,%hi(fastpath_pppoe_proc_fops)
	addiu	$7,$7,%lo(fastpath_pppoe_proc_fops)
	addiu	$4,$4,%lo($LC1)
	addiu	$6,$6,%lo(proc_root)
	move	$5,$0
	jal	proc_create_data
	sw	$0,16($sp)

	lui	$4,%hi(fast_pppoe_info)
	lui	$3,%hi(fast_pppoe_proc)
	addiu	$4,$4,%lo(fast_pppoe_info)
	move	$5,$0
	li	$6,1408			# 0x580
	jal	memset
	sw	$2,%lo(fast_pppoe_proc)($3)

	lw	$31,28($sp)
	li	$3,1			# 0x1
	lui	$2,%hi(fast_pppoe_fw)
	sw	$3,%lo(fast_pppoe_fw)($2)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	fast_pppoe_init
	.size	fast_pppoe_init, .-fast_pppoe_init
	.section	.text.fastpath_pppoe_single_open,"ax",@progbits
	.align	2
	.globl	fastpath_pppoe_single_open
	.set	nomips16
	.ent	fastpath_pppoe_single_open
	.type	fastpath_pppoe_single_open, @function
fastpath_pppoe_single_open:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	move	$4,$5
	lui	$5,%hi(fast_pppoe_read_proc)
	addiu	$5,$5,%lo(fast_pppoe_read_proc)
	move	$6,$0
	j	single_open
	nop

	.set	macro
	.set	reorder
	.end	fastpath_pppoe_single_open
	.size	fastpath_pppoe_single_open, .-fastpath_pppoe_single_open
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	"fast pppoe enable:%c\012\000"
	.align	2
$LC3:
	.ascii	"[%d] null,null,0\012\000"
	.align	2
$LC4:
	.ascii	"[%d] %s,%s,%d,0x%x:%x:%x:%x:%x:%x\012\000"
	.section	.text.fast_pppoe_read_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	fast_pppoe_read_proc
	.type	fast_pppoe_read_proc, @function
fast_pppoe_read_proc:
	.frame	$sp,88,$31		# vars= 0, regs= 9/0, args= 48, gp= 0
	.mask	0x80ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-88
	sw	$31,84($sp)
	sw	$23,80($sp)
	sw	$22,76($sp)
	sw	$21,72($sp)
	sw	$20,68($sp)
	sw	$19,64($sp)
	sw	$18,60($sp)
	sw	$17,56($sp)
	sw	$16,52($sp)
	lui	$2,%hi(fast_pppoe_fw)
	lw	$6,%lo(fast_pppoe_fw)($2)
	lui	$5,%hi($LC2)
	addiu	$5,$5,%lo($LC2)
	addiu	$6,$6,48
	lui	$16,%hi(fast_pppoe_info+54)
	lui	$18,%hi(fast_pppoe_info)
	lui	$23,%hi($LC4)
	lui	$22,%hi($LC3)
	move	$19,$4
	jal	seq_printf
	addiu	$16,$16,%lo(fast_pppoe_info+54)

	addiu	$18,$18,%lo(fast_pppoe_info)
	addiu	$23,$23,%lo($LC4)
	addiu	$22,$22,%lo($LC3)
	move	$17,$0
	li	$21,88			# 0x58
	li	$20,16			# 0x10
	lhu	$2,0($16)
$L89:
	move	$4,$19
	move	$5,$22
	bne	$2,$0,$L82
	move	$6,$17

	jal	seq_printf
	addiu	$17,$17,1

	j	$L88
	addiu	$16,$16,88

$L82:
	mul	$2,$17,$21
	lbu	$7,-12($16)
	lhu	$12,-2($16)
	lbu	$11,-16($16)
	lbu	$10,-15($16)
	lbu	$9,-14($16)
	lbu	$8,-13($16)
	lbu	$3,-11($16)
	sw	$7,40($sp)
	move	$4,$19
	move	$5,$23
	sw	$12,20($sp)
	sw	$11,24($sp)
	sw	$10,28($sp)
	sw	$9,32($sp)
	sw	$8,36($sp)
	sw	$3,44($sp)
	addiu	$7,$2,16
	addu	$7,$18,$7
	addu	$2,$18,$2
	jal	seq_printf
	sw	$2,16($sp)

	addiu	$17,$17,1
	addiu	$16,$16,88
$L88:
	bnel	$17,$20,$L89
	lhu	$2,0($16)

	lw	$31,84($sp)
	lw	$23,80($sp)
	lw	$22,76($sp)
	lw	$21,72($sp)
	lw	$20,68($sp)
	lw	$19,64($sp)
	lw	$18,60($sp)
	lw	$17,56($sp)
	lw	$16,52($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,88

	.set	macro
	.set	reorder
	.end	fast_pppoe_read_proc
	.size	fast_pppoe_read_proc, .-fast_pppoe_read_proc
	.globl	fast_pppoe_fw
	.section	.bss.fast_pppoe_fw,"aw",@nobits
	.align	2
	.type	fast_pppoe_fw, @object
	.size	fast_pppoe_fw, 4
fast_pppoe_fw:
	.space	4
	.globl	fast_pppoe_proc
	.section	.bss.fast_pppoe_proc,"aw",@nobits
	.align	2
	.type	fast_pppoe_proc, @object
	.size	fast_pppoe_proc, 4
fast_pppoe_proc:
	.space	4
	.globl	fastpath_pppoe_proc_fops
	.section	.data.fastpath_pppoe_proc_fops,"aw",@progbits
	.align	2
	.type	fastpath_pppoe_proc_fops, @object
	.size	fastpath_pppoe_proc_fops, 108
fastpath_pppoe_proc_fops:
	.space	4
	.word	seq_lseek
	.word	seq_read
	.word	fastpath_pppoe_single_write
	.space	28
	.word	fastpath_pppoe_single_open
	.space	4
	.word	single_release
	.space	52
	.globl	fast_pppoe_info
	.section	.bss.fast_pppoe_info,"aw",@nobits
	.align	2
	.type	fast_pppoe_info, @object
	.size	fast_pppoe_info, 1408
fast_pppoe_info:
	.space	1408
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 1459) 4.4.7"
