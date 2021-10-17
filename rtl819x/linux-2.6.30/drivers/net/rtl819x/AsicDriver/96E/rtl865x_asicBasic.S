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
	li	$2,30000			# 0x7530
	j	$L2
	li	$5,-1207959552			# 0xffffffffb8000000

$L4:
	bne	$2,$0,$L2
	li	$3,-60			# 0xffffffffffffffc4

	li	$2,-1207959552			# 0xffffffffb8000000
	sb	$3,8200($2)
	j	$31
	nop

$L2:
	lbu	$3,8212($5)
	nop
	andi	$3,$3,0x20
	beq	$3,$0,$L4
	addiu	$2,$2,-1

	andi	$4,$4,0x00ff
	li	$2,-1207959552			# 0xffffffffb8000000
	sb	$4,8192($2)
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
	beq	$5,$0,$L6
	nop

	lb	$2,0($4)
	nop
	beq	$2,$0,$L6
	addiu	$16,$4,1

	addu	$17,$4,$5
	li	$18,10			# 0xa
$L9:
	bne	$2,$18,$L8
	nop

	jal	prom_putchar
	li	$4,13			# 0xd

$L8:
	lb	$4,-1($16)
	jal	prom_putchar
	nop

	beq	$16,$17,$L6
	addiu	$16,$16,1

	lb	$2,-1($16)
	nop
	bne	$2,$0,$L9
	nop

$L6:
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
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"\012Assert Fail: %s %d\000"
	.align	2
$LC1:
	.ascii	"drivers/net/rtl819x/rtl865x/../AsicDriver/rtl865x_asicBa"
	.ascii	"sic.c\000"
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
	bne	$6,$0,$L12
	li	$3,-1149239296			# 0xffffffffbb800000

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	printk
	li	$6,405			# 0x195

$L13:
	j	$L13
	nop

$L12:
$L16:
	lw	$2,19712($3)
	nop
	andi	$2,$2,0x1
	bne	$2,$0,$L16
	lui	$2,%hi(_rtl8651_asicTableSize)

	sll	$3,$4,2
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$8,0($2)
	nop
	beq	$8,$0,$L14
	move	$2,$0

	li	$9,-1149239296			# 0xffffffffbb800000
	addiu	$9,$9,19744
$L15:
	sll	$3,$2,2
	addu	$3,$3,$9
	lw	$7,0($6)
	nop
	sw	$7,0($3)
	addiu	$2,$2,1
	sltu	$3,$2,$8
	bne	$3,$0,$L15
	addiu	$6,$6,4

$L14:
	sll	$4,$4,16
	li	$2,-1157627904			# 0xffffffffbb000000
	addu	$4,$4,$2
	sll	$5,$5,5
	addu	$5,$4,$5
	li	$2,-1149239296			# 0xffffffffbb800000
	sw	$5,19720($2)
	lw	$31,20($sp)
	nop
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	_rtl8651_asicTableAccessForward
	.size	_rtl8651_asicTableAccessForward, .-_rtl8651_asicTableAccessForward
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
	li	$3,-1207959552			# 0xffffffffb8000000
	lw	$5,0($3)
	li	$2,-65536			# 0xffffffffffff0000
	and	$2,$5,$2
	lw	$3,12($3)
	li	$5,-1073741824			# 0xffffffffc0000000
	bne	$2,$5,$L20
	andi	$3,$3,0xf

	li	$2,9			# 0x9
	beq	$3,$2,$L31
	li	$2,46			# 0x2e

	andi	$2,$3,0x8
	bne	$2,$0,$L22
	li	$2,46			# 0x2e

$L31:
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L22:
	li	$2,8			# 0x8
	bne	$3,$2,$L24
	li	$2,14			# 0xe

	sw	$2,0($4)
	j	$31
	move	$2,$0

$L24:
	addiu	$3,$3,-10
	sltu	$3,$3,2
	beq	$3,$0,$L25
	nop

	li	$2,10			# 0xa
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L25:
	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L20:
	li	$5,-2147483648			# 0xffffffff80000000
	bne	$2,$5,$L26
	nop

	li	$2,7			# 0x7
	bne	$3,$2,$L27
	li	$2,46			# 0x2e

	sw	$2,0($4)
	j	$31
	move	$2,$0

$L27:
	li	$2,15			# 0xf
	bne	$3,$2,$L28
	li	$2,14			# 0xe

	sw	$2,0($4)
	j	$31
	move	$2,$0

$L28:
	li	$2,3			# 0x3
	bne	$3,$2,$L29
	nop

	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L29:
	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L26:
	li	$3,-2120876032			# 0xffffffff81960000
	bne	$2,$3,$L30
	nop

	li	$2,14			# 0xe
	sw	$2,0($4)
	j	$31
	move	$2,$0

$L30:
	j	$31
	li	$2,-1			# 0xffffffffffffffff

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
	nop
	sw	$3,0($4)
	lw	$2,%lo(fun_enable)($2)
	nop
	bne	$2,$0,$L33
	move	$17,$4

	lui	$16,%hi(fun_enable)
	jal	rtl865x_initAsicFun
	addiu	$4,$16,%lo(fun_enable)

	lw	$2,%lo(fun_enable)($16)
	nop
	sw	$2,0($17)
$L33:
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
	.section	.rodata.str1.4
	.align	2
$LC2:
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
	lw	$18,12($2)
	lui	$4,%hi(fun_enable)
	jal	rtl865x_initAsicFun
	addiu	$4,$4,%lo(fun_enable)

	li	$3,-1			# 0xffffffffffffffff
	beq	$2,$3,$L51
	li	$2,-1			# 0xffffffffffffffff

	li	$2,-65536			# 0xffffffffffff0000
	and	$17,$17,$2
	li	$2,-1073741824			# 0xffffffffc0000000
	bne	$17,$2,$L36
	andi	$18,$18,0xf

	li	$2,9			# 0x9
	bne	$16,$2,$L40
	nop

	beq	$18,$2,$L41
	andi	$18,$18,0x8

	beq	$18,$0,$L42
	lui	$4,%hi($LC2)

	addiu	$4,$4,%lo($LC2)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L35
	li	$2,-1			# 0xffffffffffffffff

$L36:
	li	$2,-2147483648			# 0xffffffff80000000
	bne	$17,$2,$L51
	move	$2,$0

	li	$2,7			# 0x7
	bne	$16,$2,$L37
	nop

	beq	$18,$2,$L44
	lui	$4,%hi($LC2)

	addiu	$4,$4,%lo($LC2)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L35
	li	$2,-1			# 0xffffffffffffffff

$L37:
	li	$2,15			# 0xf
	bne	$16,$2,$L38
	nop

	beq	$18,$2,$L38
	li	$2,7			# 0x7

	beq	$18,$2,$L51
	move	$2,$0

	li	$2,3			# 0x3
	beq	$18,$2,$L51
	move	$2,$0

	li	$2,11			# 0xb
	beq	$18,$2,$L47
	lui	$4,%hi($LC2)

	addiu	$4,$4,%lo($LC2)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L35
	li	$2,-1			# 0xffffffffffffffff

$L38:
	li	$2,3			# 0x3
	bne	$16,$2,$L48
	nop

	beq	$18,$2,$L51
	move	$2,$0

	li	$2,7			# 0x7
	beq	$18,$2,$L50
	lui	$4,%hi($LC2)

	addiu	$4,$4,%lo($LC2)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L35
	li	$2,-1			# 0xffffffffffffffff

$L40:
	j	$L35
	move	$2,$0

$L41:
	j	$L35
	move	$2,$0

$L42:
	j	$L35
	move	$2,$0

$L44:
	j	$L35
	move	$2,$0

$L47:
	j	$L35
	move	$2,$0

$L48:
	j	$L35
	move	$2,$0

$L50:
	move	$2,$0
$L35:
$L51:
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
	beq	$2,$0,$L53
	li	$2,1			# 0x1

	sll	$16,$2,$16
	andi	$2,$16,0xe22
	bne	$2,$0,$L54
	andi	$2,$16,0x4000

	bne	$2,$0,$L56
	andi	$16,$16,0x8

	bne	$16,$0,$L55
	li	$2,1			# 0x1

	j	$L58
	sw	$2,0($17)

$L56:
	lw	$2,16($sp)
	nop
	andi	$2,$2,0x20
	beq	$2,$0,$L57
	li	$2,1			# 0x1

	j	$L58
	sw	$2,0($17)

$L57:
	j	$L58
	sw	$0,0($17)

$L54:
	lw	$2,16($sp)
	nop
	andi	$2,$2,0x4
	beq	$2,$0,$L59
	li	$2,1			# 0x1

	j	$L58
	sw	$2,0($17)

$L59:
	j	$L58
	sw	$0,0($17)

$L55:
	lw	$2,16($sp)
	nop
	andi	$2,$2,0x2
	beq	$2,$0,$L60
	li	$2,1			# 0x1

	j	$L58
	sw	$2,0($17)

$L60:
	j	$L58
	sw	$0,0($17)

$L53:
	sw	$2,0($17)
$L58:
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
	nop
	beq	$2,$0,$L74
	move	$4,$18

	move	$5,$16
	jal	_rtl8651_asicTableAccessForward
	move	$6,$17

	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L64
	li	$3,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	nop
	bgtz	$2,$L80
	li	$2,-1149239296			# 0xffffffffbb800000

	j	$L81
	nop

$L64:
	beq	$2,$3,$L65
	li	$3,4			# 0x4

	bne	$2,$3,$L79
	li	$3,3			# 0x3

$L65:
	li	$2,-1149239296			# 0xffffffffbb800000
$L80:
	lw	$4,17432($2)
	li	$3,262144			# 0x40000
	or	$3,$4,$3
	sw	$3,17432($2)
	li	$4,-1149239296			# 0xffffffffbb800000
	li	$3,524288			# 0x80000
$L67:
	lw	$2,17432($4)
	nop
	and	$2,$2,$3
	beq	$2,$0,$L67
	nop

	li	$3,3			# 0x3
$L79:
	li	$2,-1149239296			# 0xffffffffbb800000
$L81:
	sw	$3,19712($2)
	li	$3,-1149239296			# 0xffffffffbb800000
$L68:
	lw	$2,19712($3)
	nop
	andi	$2,$2,0x1
	bne	$2,$0,$L68
	li	$2,-1149239296			# 0xffffffffbb800000

	lw	$2,19716($2)
	nop
	andi	$2,$2,0x1
	beq	$2,$0,$L69
	lui	$2,%hi(RtkHomeGatewayChipNameID)

	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L70
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	nop
	bgtz	$2,$L71
	nop

	j	$L63
	li	$2,-1			# 0xffffffffffffffff

$L70:
	li	$3,3			# 0x3
	beq	$2,$3,$L71
	li	$3,4			# 0x4

	bne	$2,$3,$L75
	nop

$L71:
	li	$2,-1149239296			# 0xffffffffbb800000
	lw	$4,17432($2)
	li	$3,-327680			# 0xfffffffffffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,17432($2)
	j	$L63
	li	$2,-1			# 0xffffffffffffffff

$L69:
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L72
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	nop
	bgtz	$2,$L73
	nop

	j	$L63
	move	$2,$0

$L72:
	li	$3,3			# 0x3
	beq	$2,$3,$L73
	li	$3,4			# 0x4

	bne	$2,$3,$L76
	nop

$L73:
	li	$2,-1149239296			# 0xffffffffbb800000
	lw	$4,17432($2)
	li	$3,-327680			# 0xfffffffffffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,17432($2)
	j	$L63
	move	$2,$0

$L74:
	j	$L63
	li	$2,-1			# 0xffffffffffffffff

$L75:
	j	$L63
	li	$2,-1			# 0xffffffffffffffff

$L76:
	move	$2,$0
$L63:
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
	lw	$2,%lo(mcastForceAddOpCnt)($2)
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	_rtl865x_getForceAddMcastOpCnt
	.size	_rtl865x_getForceAddMcastOpCnt, .-_rtl865x_getForceAddMcastOpCnt
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

	lw	$2,16($sp)
	nop
	beq	$2,$0,$L98
	li	$2,-1			# 0xffffffffffffffff

	li	$2,3			# 0x3
	bne	$18,$2,$L99
	lui	$2,%hi(RtkHomeGatewayChipNameID)

	lui	$2,%hi(mcastForceAddOpCnt)
	lw	$3,%lo(mcastForceAddOpCnt)($2)
	nop
	addiu	$3,$3,1
	sw	$3,%lo(mcastForceAddOpCnt)($2)
	lui	$2,%hi(RtkHomeGatewayChipNameID)
$L99:
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L86
	li	$3,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	nop
	bgtz	$2,$L100
	li	$2,-1149239296			# 0xffffffffbb800000

	j	$L97
	move	$4,$18

$L86:
	beq	$2,$3,$L87
	li	$3,4			# 0x4

	bne	$2,$3,$L97
	move	$4,$18

$L87:
	li	$2,-1149239296			# 0xffffffffbb800000
$L100:
	lw	$4,17432($2)
	li	$3,262144			# 0x40000
	or	$3,$4,$3
	sw	$3,17432($2)
	li	$4,-1149239296			# 0xffffffffbb800000
	li	$3,524288			# 0x80000
$L89:
	lw	$2,17432($4)
	nop
	and	$2,$2,$3
	beq	$2,$0,$L89
	nop

	move	$4,$18
$L97:
	move	$5,$16
	jal	_rtl8651_asicTableAccessForward
	move	$6,$17

	li	$3,9			# 0x9
	li	$2,-1149239296			# 0xffffffffbb800000
	sw	$3,19712($2)
	li	$3,-1149239296			# 0xffffffffbb800000
$L90:
	lw	$2,19712($3)
	nop
	andi	$2,$2,0x1
	bne	$2,$0,$L90
	lui	$2,%hi(RtkHomeGatewayChipNameID)

	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L91
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	nop
	bgtz	$2,$L92
	nop

	j	$L84
	move	$2,$0

$L91:
	li	$3,3			# 0x3
	beq	$2,$3,$L92
	li	$3,4			# 0x4

	bne	$2,$3,$L94
	nop

$L92:
	li	$2,-1149239296			# 0xffffffffbb800000
	lw	$4,17432($2)
	li	$3,-327680			# 0xfffffffffffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,17432($2)
	j	$L84
	move	$2,$0

$L94:
	move	$2,$0
$L84:
$L98:
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
	.section	.text._rtl8651_readAsicEntry,"ax",@progbits
	.align	2
	.globl	_rtl8651_readAsicEntry
	.set	nomips16
	.ent	_rtl8651_readAsicEntry
	.type	_rtl8651_readAsicEntry, @function
_rtl8651_readAsicEntry:
	.frame	$sp,104,$31		# vars= 72, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-104
	sw	$31,100($sp)
	sw	$18,96($sp)
	sw	$17,92($sp)
	sw	$16,88($sp)
	move	$18,$4
	move	$17,$5
	move	$16,$6
	jal	rtl865x_accessAsicTable
	addiu	$5,$sp,16

	lw	$2,16($sp)
	nop
	beq	$2,$0,$L119
	li	$2,-1			# 0xffffffffffffffff

	bne	$16,$0,$L103
	sll	$3,$18,16

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	printk
	li	$6,577			# 0x241

$L104:
	j	$L104
	nop

$L103:
	li	$2,-1157627904			# 0xffffffffbb000000
	addu	$2,$3,$2
	sll	$17,$17,5
	addu	$17,$2,$17
	li	$3,-1149239296			# 0xffffffffbb800000
$L105:
	lw	$2,19712($3)
	nop
	andi	$2,$2,0x1
	bne	$2,$0,$L105
	li	$14,10			# 0xa

	lw	$13,0($17)
	lw	$12,4($17)
	lw	$11,8($17)
	lw	$10,12($17)
	lw	$9,16($17)
	lw	$8,20($17)
	lw	$7,24($17)
	lw	$6,28($17)
	addiu	$5,$sp,84
$L110:
	sw	$13,20($sp)
	sw	$12,24($sp)
	sw	$11,28($sp)
	sw	$10,32($sp)
	sw	$9,36($sp)
	sw	$8,40($sp)
	sw	$7,44($sp)
	sw	$6,48($sp)
	sw	$13,52($sp)
	sw	$12,56($sp)
	sw	$11,60($sp)
	sw	$10,64($sp)
	sw	$9,68($sp)
	sw	$8,72($sp)
	sw	$7,76($sp)
	sw	$6,80($sp)
	addiu	$2,$sp,56
$L108:
	lw	$4,0($2)
	lw	$3,-32($2)
	nop
	bne	$4,$3,$L106
	addiu	$2,$2,4

	bne	$2,$5,$L108
	nop

	j	$L118
	sll	$18,$18,2

$L106:
	addiu	$14,$14,-1
	bne	$14,$0,$L110
	nop

	sll	$18,$18,2
$L118:
	lui	$2,%hi(_rtl8651_asicTableSize)
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$18,$18,$2
	lw	$5,0($18)
	nop
	beq	$5,$0,$L113
	addiu	$3,$sp,52

	move	$2,$0
$L111:
	lw	$4,0($3)
	nop
	sw	$4,0($16)
	addiu	$2,$2,1
	addiu	$3,$3,4
	sltu	$4,$2,$5
	bne	$4,$0,$L111
	addiu	$16,$16,4

	j	$L102
	move	$2,$0

$L113:
	move	$2,$0
$L102:
$L119:
	lw	$31,100($sp)
	lw	$18,96($sp)
	lw	$17,92($sp)
	lw	$16,88($sp)
	j	$31
	addiu	$sp,$sp,104

	.set	macro
	.set	reorder
	.end	_rtl8651_readAsicEntry
	.size	_rtl8651_readAsicEntry, .-_rtl8651_readAsicEntry
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
	move	$18,$4
	move	$17,$5
	move	$16,$6
	jal	rtl865x_accessAsicTable
	addiu	$5,$sp,16

	lw	$2,16($sp)
	nop
	beq	$2,$0,$L135
	li	$2,-1			# 0xffffffffffffffff

	bne	$16,$0,$L122
	sll	$3,$18,16

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	printk
	li	$6,721			# 0x2d1

$L123:
	j	$L123
	nop

$L122:
	li	$2,-1157627904			# 0xffffffffbb000000
	addu	$2,$3,$2
	sll	$17,$17,5
	addu	$17,$2,$17
	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L124
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	nop
	bgtz	$2,$L136
	li	$2,-1149239296			# 0xffffffffbb800000

	j	$L134
	sll	$18,$18,2

$L124:
	li	$3,3			# 0x3
	beq	$2,$3,$L125
	li	$3,4			# 0x4

	bne	$2,$3,$L126
	nop

$L125:
	li	$2,-1149239296			# 0xffffffffbb800000
$L136:
	lw	$4,17432($2)
	li	$3,262144			# 0x40000
	or	$3,$4,$3
	sw	$3,17432($2)
$L126:
	sll	$18,$18,2
$L134:
	lui	$2,%hi(_rtl8651_asicTableSize)
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$18,$18,$2
	lw	$5,0($18)
	nop
	beq	$5,$0,$L127
	move	$2,$16

	move	$3,$0
	subu	$16,$17,$16
$L128:
	addu	$4,$16,$2
	lw	$4,0($4)
	nop
	sw	$4,0($2)
	addiu	$3,$3,1
	sltu	$4,$3,$5
	bne	$4,$0,$L128
	addiu	$2,$2,4

$L127:
	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L129
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	nop
	bgtz	$2,$L130
	nop

	j	$L121
	move	$2,$0

$L129:
	li	$3,3			# 0x3
	beq	$2,$3,$L130
	li	$3,4			# 0x4

	bne	$2,$3,$L132
	nop

$L130:
	li	$2,-1149239296			# 0xffffffffbb800000
	lw	$4,17432($2)
	li	$3,-327680			# 0xfffffffffffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,17432($2)
	j	$L121
	move	$2,$0

$L132:
	move	$2,$0
$L121:
$L135:
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
$L138:
	lw	$2,19712($3)
	nop
	andi	$2,$2,0x1
	bne	$2,$0,$L138
	lui	$2,%hi(_rtl8651_asicTableSize)

	sll	$3,$4,2
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$7,0($2)
	nop
	beq	$7,$0,$L139
	move	$2,$0

	li	$8,-1149239296			# 0xffffffffbb800000
	addiu	$8,$8,19744
	sll	$3,$2,2
$L150:
	addu	$3,$3,$8
	sw	$0,0($3)
	addiu	$2,$2,1
	sltu	$3,$2,$7
	bne	$3,$0,$L150
	sll	$3,$2,2

$L139:
	sltu	$2,$6,$5
	bne	$2,$0,$L144
	nop

	sll	$4,$4,16
	li	$2,-1157627904			# 0xffffffffbb000000
	addu	$4,$4,$2
	sll	$2,$5,5
	addu	$4,$4,$2
	li	$7,-1149239296			# 0xffffffffbb800000
	addiu	$3,$7,19712
	li	$8,9			# 0x9
$L143:
	sw	$4,19720($7)
	sw	$8,0($3)
$L142:
	lw	$2,0($3)
	nop
	andi	$2,$2,0x1
	bne	$2,$0,$L142
	nop

	lw	$2,19716($7)
	nop
	andi	$2,$2,0x1
	bne	$2,$0,$L145
	nop

	addiu	$5,$5,1
	sltu	$2,$6,$5
	beq	$2,$0,$L143
	addiu	$4,$4,32

	j	$31
	move	$2,$0

$L144:
	j	$31
	move	$2,$0

$L145:
	j	$31
	li	$2,-1			# 0xffffffffffffffff

	.set	macro
	.set	reorder
	.end	_rtl8651_delAsicEntry
	.size	_rtl8651_delAsicEntry, .-_rtl8651_delAsicEntry
	.section	.rodata.str1.4
	.align	2
$LC3:
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
	lw	$4,24844($2)
	li	$2,-1207894016			# 0xffffffffb8010000
	lw	$6,4($2)
	lw	$5,32($2)
	lui	$3,%hi(rtl819x_checkSwCoreTimer)
	lw	$2,%lo(rtl819x_checkSwCoreTimer)($3)
	nop
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
	bne	$2,$3,$L151
	li	$2,-4			# 0xfffffffffffffffc

	and	$6,$6,$2
	and	$5,$5,$2
	li	$2,65536			# 0x10000
	and	$4,$4,$2
	bne	$4,$0,$L153
	lui	$2,%hi(rtl819x_lastTxDesc)

	lui	$2,%hi(rtl819x_swHangCnt)
	j	$L154
	sw	$0,%lo(rtl819x_swHangCnt)($2)

$L153:
	lw	$2,%lo(rtl819x_lastTxDesc)($2)
	nop
	beq	$2,$0,$L155
	nop

	lui	$3,%hi(rtl819x_lastRxDesc)
	lw	$3,%lo(rtl819x_lastRxDesc)($3)
	nop
	bne	$3,$0,$L156
	nop

$L155:
	lui	$2,%hi(rtl819x_swHangCnt)
	j	$L154
	sw	$0,%lo(rtl819x_swHangCnt)($2)

$L156:
	bne	$6,$3,$L157
	nop

	bne	$5,$2,$L158
	lui	$2,%hi(rtl819x_swHangCnt)

	lw	$3,%lo(rtl819x_swHangCnt)($2)
	nop
	addiu	$3,$3,1
	j	$L154
	sw	$3,%lo(rtl819x_swHangCnt)($2)

$L157:
	lui	$2,%hi(rtl819x_swHangCnt)
$L158:
	sw	$0,%lo(rtl819x_swHangCnt)($2)
$L154:
	lui	$2,%hi(rtl819x_lastRxDesc)
	sw	$6,%lo(rtl819x_lastRxDesc)($2)
	lui	$2,%hi(rtl819x_lastTxDesc)
	sw	$5,%lo(rtl819x_lastTxDesc)($2)
	lui	$2,%hi(rtl819x_swHangCnt)
	lw	$2,%lo(rtl819x_swHangCnt)($2)
	nop
	sltu	$2,$2,3
	bne	$2,$0,$L151
	lui	$2,%hi(rtl819x_swHangCnt)

	sw	$0,%lo(rtl819x_swHangCnt)($2)
	lui	$4,%hi($LC3)
	jal	printk
	addiu	$4,$4,%lo($LC3)

	jal	machine_restart
	move	$4,$0

$L151:
	lw	$31,20($sp)
	nop
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	rtl819x_poll_sw
	.size	rtl819x_poll_sw, .-rtl819x_poll_sw
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
	.globl	RtkHomeGatewayChipName
	.section	.bss.RtkHomeGatewayChipName,"aw",@nobits
	.align	2
	.type	RtkHomeGatewayChipName, @object
	.size	RtkHomeGatewayChipName, 16
RtkHomeGatewayChipName:
	.space	16
	.section	.dram-fwd,"aw",@progbits
	.align	2
	.type	fun_enable, @object
	.size	fun_enable, 4
fun_enable:
	.space	4
	.section	.rodata._rtl8651_asicTableSize,"a",@progbits
	.align	2
	.type	_rtl8651_asicTableSize, @object
	.size	_rtl8651_asicTableSize, 64
_rtl8651_asicTableSize:
	.word	2
	.word	1
	.word	2
	.word	3
	.word	5
	.word	3
	.word	3
	.word	3
	.word	4
	.word	3
	.word	3
	.word	1
	.word	8
	.word	1
	.word	3
	.word	1
	.section	.bss.mcastForceAddOpCnt,"aw",@nobits
	.align	2
	.type	mcastForceAddOpCnt, @object
	.size	mcastForceAddOpCnt, 4
mcastForceAddOpCnt:
	.space	4
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
	.ident	"GCC: (Realtek RSDK-4.6.4 Build 2080) 4.6.4"
