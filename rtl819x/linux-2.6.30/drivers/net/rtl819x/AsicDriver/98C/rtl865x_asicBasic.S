	.file	1 "rtl865x_asicBasic.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.prom_putchar,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	prom_putchar
	.type	prom_putchar, @function
prom_putchar:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	li	$2,1			# 0x1
	li	$5,-1207959552			# 0xffffffffb8000000
	ori	$5,$5,0x2014
	j	$L2
	li	$6,30000			# 0x7530

$L5:
	bne	$2,$6,$L3
	addiu	$2,$2,1

	li	$3,-60			# 0xffffffffffffffc4
	li	$2,-1207959552			# 0xffffffffb8000000
	ori	$2,$2,0x2008
	sb	$3,0($2)
	j	$31
	nop

$L3:
$L2:
	lbu	$3,0($5)
	andi	$3,$3,0x20
	beq	$3,$0,$L5
	nop

	andi	$4,$4,0x00ff
	li	$2,-1207959552			# 0xffffffffb8000000
	ori	$2,$2,0x2000
	sb	$4,0($2)
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	prom_putchar
	.size	prom_putchar, .-prom_putchar
	.section	.text.early_console_write,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	early_console_write
	.type	early_console_write, @function
early_console_write:
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
	beq	$5,$0,$L11
	move	$16,$4

	lb	$2,0($4)
	beq	$2,$0,$L11
	addiu	$17,$5,-1

	li	$18,10			# 0xa
$L10:
	bne	$2,$18,$L9
	nop

	jal	prom_putchar
	li	$4,13			# 0xd

$L9:
	jal	prom_putchar
	lb	$4,0($16)

	beq	$17,$0,$L11
	addiu	$16,$16,1

	lb	$2,0($16)
	bne	$2,$0,$L10
	addiu	$17,$17,-1

$L11:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	early_console_write
	.size	early_console_write, .-early_console_write
	.section	.text.rtl865x_initAsicFun,"ax",@progbits
	.align	2
	.globl	rtl865x_initAsicFun
	.set	nomips16
	.ent	rtl865x_initAsicFun
	.type	rtl865x_initAsicFun, @function
rtl865x_initAsicFun:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	sw	$0,0($4)
	li	$2,-1207959552			# 0xffffffffb8000000
	lw	$5,0($2)
	li	$3,-65536			# 0xffffffffffff0000
	and	$3,$5,$3
	ori	$2,$2,0xc
	lw	$2,0($2)
	li	$5,-1073741824			# 0xffffffffc0000000
	bne	$3,$5,$L13
	andi	$2,$2,0xf

	li	$3,9			# 0x9
	beq	$2,$3,$L14
	andi	$3,$2,0x8

	bne	$3,$0,$L15
	li	$3,8			# 0x8

$L14:
	li	$2,46			# 0x2e
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L15:
	bne	$2,$3,$L17
	nop

	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L17:
	addiu	$2,$2,-10
	sltu	$2,$2,2
	beq	$2,$0,$L18
	nop

	li	$2,10			# 0xa
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L18:
	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L13:
	li	$5,-2147483648			# 0xffffffff80000000
	bne	$3,$5,$L19
	nop

	li	$3,7			# 0x7
	bne	$2,$3,$L20
	nop

	li	$2,46			# 0x2e
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L20:
	li	$3,15			# 0xf
	bne	$2,$3,$L21
	nop

	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L21:
	li	$3,3			# 0x3
	bne	$2,$3,$L22
	nop

	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L22:
	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L19:
	li	$5,-2120744960			# 0xffffffff81980000
	bne	$3,$5,$L25
	li	$2,-1			# 0xffffffffffffffff

	li	$2,46			# 0x2e
	sw	$2,0($4)
	move	$2,$0
$L25:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	rtl865x_initAsicFun
	.size	rtl865x_initAsicFun, .-rtl865x_initAsicFun
	.section	.text.rtl865x_getAsicFun,"ax",@progbits
	.align	2
	.globl	rtl865x_getAsicFun
	.set	nomips16
	.ent	rtl865x_getAsicFun
	.type	rtl865x_getAsicFun, @function
rtl865x_getAsicFun:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$2,%hi(fun_enable)
	lw	$3,%lo(fun_enable)($2)
	sw	$3,0($4)
	lw	$2,%lo(fun_enable)($2)
	bne	$2,$0,$L27
	move	$17,$4

	lui	$16,%hi(fun_enable)
	jal	rtl865x_initAsicFun
	addiu	$4,$16,%lo(fun_enable)

	lw	$2,%lo(fun_enable)($16)
	sw	$2,0($17)
$L27:
	move	$2,$0
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl865x_getAsicFun
	.size	rtl865x_getAsicFun, .-rtl865x_getAsicFun
	.section	.text.rtl865x_accessAsicTable,"ax",@progbits
	.align	2
	.globl	rtl865x_accessAsicTable
	.set	nomips16
	.ent	rtl865x_accessAsicTable
	.type	rtl865x_accessAsicTable, @function
rtl865x_accessAsicTable:
	.frame	$sp,40,$31		# vars= 8, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$17,32($sp)
	sw	$16,28($sp)
	move	$16,$4
	move	$17,$5
	jal	rtl865x_getAsicFun
	addiu	$4,$sp,16

	sltu	$2,$16,15
	beq	$2,$0,$L30
	li	$2,1			# 0x1

	sll	$16,$2,$16
	andi	$2,$16,0xe22
	bne	$2,$0,$L31
	lw	$2,16($sp)

	andi	$2,$16,0x4000
	bne	$2,$0,$L33
	lw	$2,16($sp)

	andi	$16,$16,0x8
	bne	$16,$0,$L32
	nop

	j	$L39
	li	$2,1			# 0x1

$L33:
	andi	$2,$2,0x20
	beq	$2,$0,$L34
	li	$2,1			# 0x1

	j	$L35
	sw	$2,0($17)

$L34:
	j	$L35
	sw	$0,0($17)

$L31:
	andi	$2,$2,0x4
	beq	$2,$0,$L36
	li	$2,1			# 0x1

	j	$L35
	sw	$2,0($17)

$L36:
	j	$L35
	sw	$0,0($17)

$L32:
	andi	$2,$2,0x2
	beq	$2,$0,$L37
	li	$2,1			# 0x1

	j	$L35
	sw	$2,0($17)

$L37:
	j	$L35
	sw	$0,0($17)

$L30:
$L39:
	sw	$2,0($17)
$L35:
	move	$2,$0
	lw	$31,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	rtl865x_accessAsicTable
	.size	rtl865x_accessAsicTable, .-rtl865x_accessAsicTable
	.section	.text._rtl865x_getForceAddMcastOpCnt,"ax",@progbits
	.align	2
	.globl	_rtl865x_getForceAddMcastOpCnt
	.set	nomips16
	.ent	_rtl865x_getForceAddMcastOpCnt
	.type	_rtl865x_getForceAddMcastOpCnt, @function
_rtl865x_getForceAddMcastOpCnt:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(mcastForceAddOpCnt)
	j	$31
	lw	$2,%lo(mcastForceAddOpCnt)($2)

	.set	macro
	.set	reorder
	.end	_rtl865x_getForceAddMcastOpCnt
	.size	_rtl865x_getForceAddMcastOpCnt, .-_rtl865x_getForceAddMcastOpCnt
	.section	.text._rtl8198C_getForceAddMcastv6OpCnt,"ax",@progbits
	.align	2
	.globl	_rtl8198C_getForceAddMcastv6OpCnt
	.set	nomips16
	.ent	_rtl8198C_getForceAddMcastv6OpCnt
	.type	_rtl8198C_getForceAddMcastv6OpCnt, @function
_rtl8198C_getForceAddMcastv6OpCnt:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(mcastForceAddOpCnt6)
	j	$31
	lw	$2,%lo(mcastForceAddOpCnt6)($2)

	.set	macro
	.set	reorder
	.end	_rtl8198C_getForceAddMcastv6OpCnt
	.size	_rtl8198C_getForceAddMcastv6OpCnt, .-_rtl8198C_getForceAddMcastv6OpCnt
	.section	.text._rtl8651_delAsicEntry,"ax",@progbits
	.align	2
	.globl	_rtl8651_delAsicEntry
	.set	nomips16
	.ent	_rtl8651_delAsicEntry
	.type	_rtl8651_delAsicEntry, @function
_rtl8651_delAsicEntry:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	li	$3,-1149239296			# 0xffffffffbb800000
	ori	$3,$3,0x4d00
$L45:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L45
	lui	$2,%hi(_rtl8651_asicTableSize)

	sll	$3,$4,2
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$7,0($2)
	beq	$7,$0,$L46
	move	$2,$0

	li	$8,-1149239296			# 0xffffffffbb800000
	ori	$8,$8,0x4d20
	sll	$3,$2,2
$L58:
	addu	$3,$3,$8
	sw	$0,0($3)
	addiu	$2,$2,1
	sltu	$3,$2,$7
	bne	$3,$0,$L58
	sll	$3,$2,2

$L46:
	sltu	$2,$6,$5
	bne	$2,$0,$L48
	nop

	sll	$4,$4,16
	li	$2,-1157627904			# 0xffffffffbb000000
	addu	$4,$4,$2
	sll	$2,$5,5
	addu	$4,$4,$2
	li	$7,-1149239296			# 0xffffffffbb800000
	ori	$9,$7,0x4d08
	ori	$3,$7,0x4d00
	li	$8,9			# 0x9
	ori	$7,$7,0x4d04
$L52:
	sw	$4,0($9)
	sw	$8,0($3)
$L49:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L49
	nop

	lw	$2,0($7)
	andi	$2,$2,0x1
	beq	$2,$0,$L50
	addiu	$5,$5,1

	addiu	$5,$5,-1
	j	$31
	li	$2,-1			# 0xffffffffffffffff

$L50:
	sltu	$2,$6,$5
	beq	$2,$0,$L52
	addiu	$4,$4,32

$L48:
	j	$31
	move	$2,$0

	.set	macro
	.set	reorder
	.end	_rtl8651_delAsicEntry
	.size	_rtl8651_delAsicEntry, .-_rtl8651_delAsicEntry
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"\012Assert Fail: %s %d\000"
	.align	2
$LC1:
	.ascii	"drivers/net/rtl819x/rtl865x/../AsicDriver/rtl865x_asicBa"
	.ascii	"sic.c\000"
	.section	.text._rtl8651_readAsicEntryStopTLU,"ax",@progbits
	.align	2
	.globl	_rtl8651_readAsicEntryStopTLU
	.set	nomips16
	.ent	_rtl8651_readAsicEntryStopTLU
	.type	_rtl8651_readAsicEntryStopTLU, @function
_rtl8651_readAsicEntryStopTLU:
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
	move	$17,$4
	move	$18,$5
	move	$16,$6
	jal	rtl865x_accessAsicTable
	addiu	$5,$sp,16

	lw	$3,16($sp)
	beq	$3,$0,$L61
	li	$2,-1			# 0xffffffffffffffff

	bne	$16,$0,$L62
	sll	$3,$17,16

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	printk
	li	$6,721			# 0x2d1

$L63:
	j	$L63
	nop

$L62:
	li	$2,-1157627904			# 0xffffffffbb000000
	addu	$2,$3,$2
	sll	$18,$18,5
	addu	$18,$2,$18
	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L64
	li	$3,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L78
	li	$2,-1149239296			# 0xffffffffbb800000

	j	$L76
	sll	$3,$17,2

$L64:
	beq	$2,$3,$L65
	li	$3,4			# 0x4

	bne	$2,$3,$L76
	sll	$3,$17,2

$L65:
	li	$2,-1149239296			# 0xffffffffbb800000
$L78:
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,262144			# 0x40000
	or	$3,$4,$3
	sw	$3,0($2)
	sll	$3,$17,2
$L76:
	lui	$2,%hi(_rtl8651_asicTableSize)
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$2,0($2)
	beq	$2,$0,$L67
	move	$2,$0

	move	$17,$3
	lui	$3,%hi(_rtl8651_asicTableSize)
	addiu	$3,$3,%lo(_rtl8651_asicTableSize)
	addu	$17,$17,$3
	lw	$4,0($17)
$L70:
	sltu	$3,$2,8
	beq	$3,$0,$L68
	addiu	$3,$18,32736

	lw	$3,0($18)
	j	$L69
	sw	$3,0($16)

$L68:
	lw	$3,0($3)
	sw	$3,0($16)
$L69:
	addiu	$2,$2,1
	addiu	$18,$18,4
	sltu	$3,$2,$4
	bne	$3,$0,$L70
	addiu	$16,$16,4

$L67:
	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L71
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L72
	nop

	j	$L77
	move	$2,$0

$L71:
	li	$3,3			# 0x3
	beq	$2,$3,$L72
	li	$3,4			# 0x4

	bne	$2,$3,$L73
	nop

$L72:
	li	$2,-1149239296			# 0xffffffffbb800000
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,-327680			# 0xfffffffffffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,0($2)
	j	$L61
	move	$2,$0

$L73:
	move	$2,$0
$L61:
$L77:
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	_rtl8651_readAsicEntryStopTLU
	.size	_rtl8651_readAsicEntryStopTLU, .-_rtl8651_readAsicEntryStopTLU
	.section	.text._rtl8651_readAsicEntry,"ax",@progbits
	.align	2
	.globl	_rtl8651_readAsicEntry
	.set	nomips16
	.ent	_rtl8651_readAsicEntry
	.type	_rtl8651_readAsicEntry, @function
_rtl8651_readAsicEntry:
	.frame	$sp,136,$31		# vars= 96, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-136
	sw	$31,132($sp)
	sw	$19,128($sp)
	sw	$18,124($sp)
	sw	$17,120($sp)
	sw	$16,116($sp)
	move	$17,$4
	move	$18,$5
	move	$16,$6
	jal	rtl865x_accessAsicTable
	addiu	$5,$sp,16

	lw	$3,16($sp)
	beq	$3,$0,$L81
	li	$2,-1			# 0xffffffffffffffff

	bne	$16,$0,$L82
	sll	$6,$17,16

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	printk
	li	$6,577			# 0x241

$L83:
	j	$L83
	nop

$L82:
	li	$2,-1157627904			# 0xffffffffbb000000
	addu	$6,$6,$2
	sll	$18,$18,5
	addu	$18,$6,$18
	li	$3,-1149239296			# 0xffffffffbb800000
	ori	$3,$3,0x4d00
$L84:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L84
	lui	$2,%hi(_rtl8651_asicTableSize)

	lw	$15,0($18)
	lw	$14,4($18)
	lw	$13,8($18)
	lw	$12,12($18)
	lw	$11,16($18)
	lw	$10,20($18)
	lw	$9,24($18)
	lw	$8,28($18)
	sll	$3,$17,2
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$19,0($2)
	li	$25,10			# 0xa
	addiu	$24,$sp,20
	addiu	$5,$sp,108
	sltu	$7,$19,9
	li	$6,65536			# 0x10000
	addu	$6,$18,$6
	addiu	$18,$sp,68
	move	$2,$24
$L86:
	sw	$15,0($2)
	sw	$14,4($2)
	sw	$13,8($2)
	sw	$12,12($2)
	sw	$11,16($2)
	sw	$10,20($2)
	sw	$9,24($2)
	bne	$7,$0,$L85
	sw	$8,28($2)

	lw	$3,-32768($6)
	sw	$3,32($2)
	lw	$3,-32764($6)
	sw	$3,36($2)
	lw	$3,-32760($6)
	sw	$3,40($2)
$L85:
	addiu	$2,$2,44
	bne	$2,$5,$L86
	lw	$3,64($sp)

	lw	$2,20($sp)
	bne	$3,$2,$L87
	move	$2,$18

$L88:
	lw	$4,0($2)
	lw	$3,-44($2)
	bne	$4,$3,$L87
	addiu	$2,$2,4

	bne	$5,$2,$L88
	nop

	j	$L89
	nop

$L87:
	addiu	$25,$25,-1
	bne	$25,$0,$L86
	move	$2,$24

$L89:
	beq	$19,$0,$L91
	move	$2,$0

	addiu	$3,$sp,64
	move	$4,$0
	sll	$17,$17,2
	lui	$5,%hi(_rtl8651_asicTableSize)
	addiu	$5,$5,%lo(_rtl8651_asicTableSize)
	addu	$17,$17,$5
	lw	$6,0($17)
$L92:
	sll	$4,$4,2
	addu	$4,$16,$4
	lw	$5,0($3)
	sw	$5,0($4)
	addiu	$2,$2,1
	move	$4,$2
	sltu	$5,$2,$6
	bne	$5,$0,$L92
	addiu	$3,$3,4

$L91:
	move	$2,$0
$L81:
	lw	$31,132($sp)
	lw	$19,128($sp)
	lw	$18,124($sp)
	lw	$17,120($sp)
	lw	$16,116($sp)
	j	$31
	addiu	$sp,$sp,136

	.set	macro
	.set	reorder
	.end	_rtl8651_readAsicEntry
	.size	_rtl8651_readAsicEntry, .-_rtl8651_readAsicEntry
	.section	.text._rtl8651_asicTableAccessForward,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	_rtl8651_asicTableAccessForward
	.type	_rtl8651_asicTableAccessForward, @function
_rtl8651_asicTableAccessForward:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	bne	$6,$0,$L100
	li	$3,-1149239296			# 0xffffffffbb800000

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	printk
	li	$6,405			# 0x195

$L101:
	j	$L101
	nop

$L100:
	ori	$3,$3,0x4d00
$L105:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L105
	lui	$2,%hi(_rtl8651_asicTableSize)

	sll	$3,$4,2
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$2,0($2)
	beq	$2,$0,$L102
	move	$2,$0

	li	$9,-1149239296			# 0xffffffffbb800000
	ori	$9,$9,0x4d20
	move	$7,$3
	lui	$3,%hi(_rtl8651_asicTableSize)
	addiu	$3,$3,%lo(_rtl8651_asicTableSize)
	addu	$3,$7,$3
	lw	$8,0($3)
$L103:
	sll	$3,$2,2
	addu	$3,$3,$9
	lw	$7,0($6)
	sw	$7,0($3)
	addiu	$2,$2,1
	sltu	$3,$2,$8
	bne	$3,$0,$L103
	addiu	$6,$6,4

$L102:
	sll	$4,$4,16
	li	$2,-1157627904			# 0xffffffffbb000000
	addu	$4,$4,$2
	sll	$5,$5,5
	addu	$5,$4,$5
	li	$2,-1149239296			# 0xffffffffbb800000
	ori	$2,$2,0x4d08
	sw	$5,0($2)
	lw	$31,20($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	_rtl8651_asicTableAccessForward
	.size	_rtl8651_asicTableAccessForward, .-_rtl8651_asicTableAccessForward
	.section	.text._rtl8651_forceAddAsicEntry,"ax",@progbits
	.align	2
	.globl	_rtl8651_forceAddAsicEntry
	.set	nomips16
	.ent	_rtl8651_forceAddAsicEntry
	.type	_rtl8651_forceAddAsicEntry, @function
_rtl8651_forceAddAsicEntry:
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
	move	$18,$4
	move	$16,$5
	move	$17,$6
	jal	rtl865x_accessAsicTable
	addiu	$5,$sp,16

	lw	$3,16($sp)
	beq	$3,$0,$L110
	li	$2,-1			# 0xffffffffffffffff

	li	$2,3			# 0x3
	bne	$18,$2,$L111
	li	$2,21			# 0x15

	lui	$2,%hi(mcastForceAddOpCnt)
	lw	$3,%lo(mcastForceAddOpCnt)($2)
	addiu	$3,$3,1
	j	$L112
	sw	$3,%lo(mcastForceAddOpCnt)($2)

$L111:
	bne	$18,$2,$L126
	lui	$2,%hi(RtkHomeGatewayChipNameID)

	lui	$2,%hi(mcastForceAddOpCnt6)
	lw	$3,%lo(mcastForceAddOpCnt6)($2)
	addiu	$3,$3,1
	sw	$3,%lo(mcastForceAddOpCnt6)($2)
$L112:
	lui	$2,%hi(RtkHomeGatewayChipNameID)
$L126:
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L113
	li	$3,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L127
	li	$2,-1149239296			# 0xffffffffbb800000

	j	$L124
	move	$4,$18

$L113:
	beq	$2,$3,$L114
	li	$3,4			# 0x4

	bne	$2,$3,$L124
	move	$4,$18

$L114:
	li	$2,-1149239296			# 0xffffffffbb800000
$L127:
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,262144			# 0x40000
	or	$3,$4,$3
	sw	$3,0($2)
	move	$4,$2
	li	$3,524288			# 0x80000
$L116:
	lw	$2,0($4)
	and	$2,$2,$3
	beq	$2,$0,$L116
	nop

	move	$4,$18
$L124:
	move	$5,$16
	jal	_rtl8651_asicTableAccessForward
	move	$6,$17

	li	$3,9			# 0x9
	li	$2,-1149239296			# 0xffffffffbb800000
	ori	$2,$2,0x4d00
	sw	$3,0($2)
	move	$3,$2
$L117:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L117
	lui	$2,%hi(RtkHomeGatewayChipNameID)

	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L118
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L119
	nop

	j	$L125
	move	$2,$0

$L118:
	li	$3,3			# 0x3
	beq	$2,$3,$L119
	li	$3,4			# 0x4

	bne	$2,$3,$L120
	nop

$L119:
	li	$2,-1149239296			# 0xffffffffbb800000
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,-327680			# 0xfffffffffffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,0($2)
	j	$L110
	move	$2,$0

$L120:
	move	$2,$0
$L110:
$L125:
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	_rtl8651_forceAddAsicEntry
	.size	_rtl8651_forceAddAsicEntry, .-_rtl8651_forceAddAsicEntry
	.section	.text._rtl8651_addAsicEntry,"ax",@progbits
	.align	2
	.globl	_rtl8651_addAsicEntry
	.set	nomips16
	.ent	_rtl8651_addAsicEntry
	.type	_rtl8651_addAsicEntry, @function
_rtl8651_addAsicEntry:
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
	move	$18,$4
	move	$16,$5
	move	$17,$6
	jal	rtl865x_accessAsicTable
	addiu	$5,$sp,16

	lw	$2,16($sp)
	beq	$2,$0,$L147
	li	$2,-1			# 0xffffffffffffffff

	move	$4,$18
	move	$5,$16
	jal	_rtl8651_asicTableAccessForward
	move	$6,$17

	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$4,%lo(RtkHomeGatewayChipNameID)($2)
	li	$2,2			# 0x2
	bne	$4,$2,$L130
	li	$2,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L148
	li	$2,-1149239296			# 0xffffffffbb800000

	j	$L150
	li	$3,3			# 0x3

$L130:
	beq	$4,$2,$L131
	li	$2,4			# 0x4

	bne	$4,$2,$L145
	li	$3,3			# 0x3

$L131:
	li	$2,-1149239296			# 0xffffffffbb800000
$L148:
	ori	$2,$2,0x4418
	lw	$5,0($2)
	li	$3,262144			# 0x40000
	or	$3,$5,$3
	sw	$3,0($2)
	move	$5,$2
	li	$3,524288			# 0x80000
$L133:
	lw	$2,0($5)
	and	$2,$2,$3
	beq	$2,$0,$L133
	nop

	li	$3,3			# 0x3
$L145:
	li	$2,-1149239296			# 0xffffffffbb800000
$L150:
	ori	$2,$2,0x4d00
	sw	$3,0($2)
	move	$3,$2
$L134:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L134
	li	$2,-1149239296			# 0xffffffffbb800000

	ori	$2,$2,0x4d04
	lw	$2,0($2)
	andi	$2,$2,0x1
	beq	$2,$0,$L135
	li	$2,2			# 0x2

	bne	$4,$2,$L136
	li	$2,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	blez	$2,$L129
	nop

	j	$L146
	li	$2,-1149239296			# 0xffffffffbb800000

$L136:
	beq	$4,$2,$L146
	li	$2,-1149239296			# 0xffffffffbb800000

	li	$2,4			# 0x4
	bne	$4,$2,$L129
	nop

	li	$2,-1149239296			# 0xffffffffbb800000
$L146:
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,-327680			# 0xfffffffffffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,0($2)
	j	$L138
	li	$2,-1			# 0xffffffffffffffff

$L135:
	bne	$4,$2,$L139
	li	$2,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L140
	nop

	j	$L147
	move	$2,$0

$L139:
	beq	$4,$2,$L149
	li	$2,-1149239296			# 0xffffffffbb800000

	li	$2,4			# 0x4
	bne	$4,$2,$L141
	nop

$L140:
	li	$2,-1149239296			# 0xffffffffbb800000
$L149:
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,-327680			# 0xfffffffffffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,0($2)
	j	$L138
	move	$2,$0

$L129:
	j	$L138
	li	$2,-1			# 0xffffffffffffffff

$L141:
	move	$2,$0
$L138:
$L147:
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	_rtl8651_addAsicEntry
	.size	_rtl8651_addAsicEntry, .-_rtl8651_addAsicEntry
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	".........................................\012\000"
	.section	.text.rtl819x_poll_sw,"ax",@progbits
	.align	2
	.globl	rtl819x_poll_sw
	.set	nomips16
	.ent	rtl819x_poll_sw
	.type	rtl819x_poll_sw, @function
rtl819x_poll_sw:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	li	$2,-1149239296			# 0xffffffffbb800000
	ori	$2,$2,0x610c
	lw	$4,0($2)
	li	$2,-1207894016			# 0xffffffffb8010000
	ori	$3,$2,0x4
	lw	$6,0($3)
	ori	$2,$2,0x20
	lw	$5,0($2)
	lui	$3,%hi(rtl819x_checkSwCoreTimer)
	lw	$2,%lo(rtl819x_checkSwCoreTimer)($3)
	addiu	$7,$2,1
	sw	$7,%lo(rtl819x_checkSwCoreTimer)($3)
	li	$3,-859045888			# 0xffffffffcccc0000
	ori	$3,$3,0xcccd
	multu	$2,$3
	mfhi	$3
	srl	$3,$3,4
	sll	$7,$3,2
	sll	$3,$3,4
	addu	$3,$7,$3
	bne	$2,$3,$L159
	li	$2,-4			# 0xfffffffffffffffc

	and	$6,$6,$2
	ext	$4,$4,16,1
	bne	$4,$0,$L153
	and	$5,$5,$2

	lui	$2,%hi(rtl819x_swHangCnt)
	j	$L154
	sw	$0,%lo(rtl819x_swHangCnt)($2)

$L153:
	lui	$2,%hi(rtl819x_lastTxDesc)
	lw	$2,%lo(rtl819x_lastTxDesc)($2)
	beq	$2,$0,$L155
	nop

	lui	$3,%hi(rtl819x_lastRxDesc)
	lw	$3,%lo(rtl819x_lastRxDesc)($3)
	bne	$3,$0,$L156
	nop

$L155:
	lui	$2,%hi(rtl819x_swHangCnt)
	j	$L154
	sw	$0,%lo(rtl819x_swHangCnt)($2)

$L156:
	bne	$6,$3,$L157
	nop

	bne	$5,$2,$L162
	lui	$2,%hi(rtl819x_swHangCnt)

	lui	$3,%hi(rtl819x_swHangCnt)
	lw	$2,%lo(rtl819x_swHangCnt)($3)
	addiu	$2,$2,1
	sw	$2,%lo(rtl819x_swHangCnt)($3)
	lui	$3,%hi(rtl819x_lastRxDesc)
	sw	$6,%lo(rtl819x_lastRxDesc)($3)
	lui	$3,%hi(rtl819x_lastTxDesc)
	sltu	$2,$2,3
	bne	$2,$0,$L159
	sw	$5,%lo(rtl819x_lastTxDesc)($3)

	j	$L160
	lui	$2,%hi(rtl819x_swHangCnt)

$L157:
	lui	$2,%hi(rtl819x_swHangCnt)
$L162:
	j	$L154
	sw	$0,%lo(rtl819x_swHangCnt)($2)

$L160:
	sw	$0,%lo(rtl819x_swHangCnt)($2)
	lui	$4,%hi($LC2)
	jal	printk
	addiu	$4,$4,%lo($LC2)

	jal	machine_restart
	move	$4,$0

	j	$L161
	lw	$31,20($sp)

$L154:
	lui	$2,%hi(rtl819x_lastRxDesc)
	sw	$6,%lo(rtl819x_lastRxDesc)($2)
	lui	$2,%hi(rtl819x_lastTxDesc)
	sw	$5,%lo(rtl819x_lastTxDesc)($2)
$L159:
	lw	$31,20($sp)
$L161:
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	rtl819x_poll_sw
	.size	rtl819x_poll_sw, .-rtl819x_poll_sw
	.section	.rodata.str1.4
	.align	2
$LC3:
	.ascii	"init switch core failed!!!\012\000"
	.section	.text.bsp_swcore_init,"ax",@progbits
	.align	2
	.globl	bsp_swcore_init
	.set	nomips16
	.ent	bsp_swcore_init
	.type	bsp_swcore_init, @function
bsp_swcore_init:
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
	li	$2,-1207959552			# 0xffffffffb8000000
	lw	$17,0($2)
	ori	$2,$2,0xc
	lw	$18,0($2)
	jal	ado_refine
	nop

	lui	$4,%hi(fun_enable)
	jal	rtl865x_initAsicFun
	addiu	$4,$4,%lo(fun_enable)

	li	$4,-1			# 0xffffffffffffffff
	beq	$2,$4,$L165
	li	$3,-1			# 0xffffffffffffffff

	li	$2,-65536			# 0xffffffffffff0000
	and	$17,$17,$2
	li	$2,-1073741824			# 0xffffffffc0000000
	bne	$17,$2,$L166
	andi	$18,$18,0xf

	li	$2,9			# 0x9
	bne	$16,$2,$L167
	nop

	beq	$18,$2,$L167
	andi	$18,$18,0x8

	beq	$18,$0,$L167
	lui	$4,%hi($LC3)

	addiu	$4,$4,%lo($LC3)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L165
	li	$3,-1			# 0xffffffffffffffff

$L166:
	li	$2,-2147483648			# 0xffffffff80000000
	bne	$17,$2,$L165
	move	$3,$0

	li	$2,7			# 0x7
	bne	$16,$2,$L168
	nop

	beq	$18,$2,$L165
	lui	$4,%hi($LC3)

	addiu	$4,$4,%lo($LC3)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L165
	li	$3,-1			# 0xffffffffffffffff

$L168:
	li	$2,15			# 0xf
	bne	$16,$2,$L169
	nop

	beq	$18,$2,$L169
	li	$2,7			# 0x7

	beq	$18,$2,$L167
	li	$2,3			# 0x3

	beq	$18,$2,$L167
	li	$2,11			# 0xb

	beq	$18,$2,$L167
	lui	$4,%hi($LC3)

	addiu	$4,$4,%lo($LC3)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L165
	li	$3,-1			# 0xffffffffffffffff

$L169:
	li	$2,3			# 0x3
	bne	$16,$2,$L165
	move	$3,$0

	beq	$18,$2,$L171
	li	$2,7			# 0x7

	beq	$18,$2,$L172
	lui	$4,%hi($LC3)

	addiu	$4,$4,%lo($LC3)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L165
	li	$3,-1			# 0xffffffffffffffff

$L167:
	move	$3,$0
$L165:
$L171:
$L172:
	move	$2,$3
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	bsp_swcore_init
	.size	bsp_swcore_init, .-bsp_swcore_init
	.section	.bss.rtl819x_checkSwCoreTimer,"aw",@nobits
	.align	2
	.type	rtl819x_checkSwCoreTimer, @object
	.size	rtl819x_checkSwCoreTimer, 4
rtl819x_checkSwCoreTimer:
	.space	4
	.section	.bss.rtl819x_swHangCnt,"aw",@nobits
	.align	2
	.type	rtl819x_swHangCnt, @object
	.size	rtl819x_swHangCnt, 4
rtl819x_swHangCnt:
	.space	4
	.section	.bss.rtl819x_lastTxDesc,"aw",@nobits
	.align	2
	.type	rtl819x_lastTxDesc, @object
	.size	rtl819x_lastTxDesc, 4
rtl819x_lastTxDesc:
	.space	4
	.section	.bss.rtl819x_lastRxDesc,"aw",@nobits
	.align	2
	.type	rtl819x_lastRxDesc, @object
	.size	rtl819x_lastRxDesc, 4
rtl819x_lastRxDesc:
	.space	4
	.section	.rodata._rtl8651_asicTableSize,"a",@progbits
	.align	2
	.type	_rtl8651_asicTableSize, @object
	.size	_rtl8651_asicTableSize, 88
_rtl8651_asicTableSize:
	.word	2
	.word	1
	.word	3
	.word	3
	.word	5
	.word	3
	.word	3
	.word	3
	.word	4
	.word	3
	.word	3
	.word	1
	.word	11
	.word	1
	.word	3
	.word	1
	.word	9
	.word	6
	.word	6
	.word	1
	.word	3
	.word	9
	.section	.bss.mcastForceAddOpCnt,"aw",@nobits
	.align	2
	.type	mcastForceAddOpCnt, @object
	.size	mcastForceAddOpCnt, 4
mcastForceAddOpCnt:
	.space	4
	.section	.bss.mcastForceAddOpCnt6,"aw",@nobits
	.align	2
	.type	mcastForceAddOpCnt6, @object
	.size	mcastForceAddOpCnt6, 4
mcastForceAddOpCnt6:
	.space	4
	.section	.bss.fun_enable,"aw",@nobits
	.align	2
	.type	fun_enable, @object
	.size	fun_enable, 4
fun_enable:
	.space	4
	.globl	RtkHomeGatewayChipName
	.section	.bss.RtkHomeGatewayChipName,"aw",@nobits
	.align	2
	.type	RtkHomeGatewayChipName, @object
	.size	RtkHomeGatewayChipName, 16
RtkHomeGatewayChipName:
	.space	16
	.globl	RtkHomeGatewayChipNameID
	.section	.bss.RtkHomeGatewayChipNameID,"aw",@nobits
	.align	2
	.type	RtkHomeGatewayChipNameID, @object
	.size	RtkHomeGatewayChipNameID, 4
RtkHomeGatewayChipNameID:
	.space	4
	.globl	RtkHomeGatewayChipRevisionID
	.section	.bss.RtkHomeGatewayChipRevisionID,"aw",@nobits
	.align	2
	.type	RtkHomeGatewayChipRevisionID, @object
	.size	RtkHomeGatewayChipRevisionID, 4
RtkHomeGatewayChipRevisionID:
	.space	4
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 1459) 4.4.7"
