	.file	1 "rtl865x_asicBasic.c"
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
	li	$5,-1207959552			# 0xb8000000
	ori	$5,$5,0x2014
	j	$L2
	li	$6,30000			# 0x7530

$L5:
	bne	$2,$6,$L3
	addiu	$2,$2,1

	li	$3,-60			# 0xffffffc4
	li	$2,-1207959552			# 0xb8000000
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
	li	$2,-1207959552			# 0xb8000000
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
	lb	$4,0($16)
	jal	prom_putchar
	nop

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
	
	li	$3,-1149239296			# 0xbb800000
	ori	$3,$3,0x4d00
$L15:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L15
	lui	$2,%hi(_rtl8651_asicTableSize)

	sll	$3,$4,2
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$7,0($2)
	beq	$7,$0,$L16
	move	$2,$0

	li	$8,-1149239296			# 0xbb800000
	ori	$8,$8,0x4d20
	sll	$3,$2,2
$L28:
	addu	$3,$3,$8
	sw	$0,0($3)
	addiu	$2,$2,1
	sltu	$3,$2,$7
	bne	$3,$0,$L28
	sll	$3,$2,2

$L16:
	sltu	$2,$6,$5
	bne	$2,$0,$L18
	nop

	sll	$4,$4,16
	li	$2,-1157627904			# 0xbb000000
	addu	$4,$4,$2
	sll	$2,$5,5
	addu	$4,$4,$2
	li	$7,-1149239296			# 0xbb800000
	ori	$9,$7,0x4d08
	ori	$3,$7,0x4d00
	li	$8,9			# 0x9
	ori	$7,$7,0x4d04
$L22:
	sw	$4,0($9)
	sw	$8,0($3)
$L19:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L19
	nop

	lw	$2,0($7)
	andi	$2,$2,0x1
	beq	$2,$0,$L20
	addiu	$5,$5,1

	addiu	$5,$5,-1
	j	$31
	li	$2,-1			# 0xffffffff

$L20:
	sltu	$2,$6,$5
	beq	$2,$0,$L22
	addiu	$4,$4,32

$L18:
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
	bne	$6,$0,$L30
	li	$3,-1149239296			# 0xbb800000

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	panic_printk
	li	$6,368			# 0x170

$L31:
	j	$L31
	nop

$L30:
	ori	$3,$3,0x4d00
$L35:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L35
	lui	$2,%hi(_rtl8651_asicTableSize)

	sll	$3,$4,2
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$2,0($2)
	beq	$2,$0,$L32
	move	$2,$0

	li	$9,-1149239296			# 0xbb800000
	ori	$9,$9,0x4d20
	move	$7,$3
	lui	$3,%hi(_rtl8651_asicTableSize)
	addiu	$3,$3,%lo(_rtl8651_asicTableSize)
	addu	$3,$7,$3
	lw	$8,0($3)
$L33:
	sll	$3,$2,2
	addu	$3,$3,$9
	lw	$7,0($6)
	sw	$7,0($3)
	addiu	$2,$2,1
	sltu	$3,$2,$8
	bne	$3,$0,$L33
	addiu	$6,$6,4

$L32:
	sll	$4,$4,16
	li	$2,-1157627904			# 0xbb000000
	addu	$4,$4,$2
	sll	$5,$5,5
	addu	$5,$4,$5
	li	$2,-1149239296			# 0xbb800000
	ori	$2,$2,0x4d08
	sw	$5,0($2)
	lw	$31,20($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	_rtl8651_asicTableAccessForward
	.size	_rtl8651_asicTableAccessForward, .-_rtl8651_asicTableAccessForward
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	"\012Lexra bus 1 master timeout, GISR2= 0x%08x, uptime= %"
	.ascii	"d seconds.\012\012\000"
	.section	.text.Lx1_check,"ax",@progbits
	.align	2
	.globl	Lx1_check
	.set	nomips16
	.ent	Lx1_check
	.type	Lx1_check, @function
Lx1_check:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	li	$2,-1207959552			# 0xb8000000
	ori	$2,$2,0x3024
	lw	$3,0($2)
	li	$2,134217728			# 0x8000000
	and	$2,$3,$2
	beq	$2,$0,$L41
	move	$2,$0

	li	$2,-1207959552			# 0xb8000000
	ori	$2,$2,0x3024
	lw	$16,0($2)
	jal	get_uptime_by_sec
	nop

	lui	$4,%hi($LC2)
	addiu	$4,$4,%lo($LC2)
	move	$5,$16
	jal	panic_printk
	move	$6,$2

	jal	machine_restart
	move	$4,$0

	move	$2,$0
$L41:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	Lx1_check
	.size	Lx1_check, .-Lx1_check
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
	li	$2,-1149239296			# 0xbb800000
	ori	$2,$2,0x610c
	lw	$4,0($2)
	li	$2,-1207894016			# 0xb8010000
	ori	$3,$2,0x4
	lw	$6,0($3)
	ori	$2,$2,0x20
	lw	$5,0($2)
	lui	$3,%hi(rtl819x_checkSwCoreTimer)
	lw	$2,%lo(rtl819x_checkSwCoreTimer)($3)
	addiu	$7,$2,1
	sw	$7,%lo(rtl819x_checkSwCoreTimer)($3)
	li	$3,-859045888			# 0xcccc0000
	ori	$3,$3,0xcccd
	multu	$2,$3
	mfhi	$3
	srl	$3,$3,4
	sll	$7,$3,2
	sll	$3,$3,4
	addu	$3,$7,$3
	bne	$2,$3,$L50
	li	$2,-4			# 0xfffffffc

	and	$6,$6,$2
	and	$5,$5,$2
	li	$2,65536			# 0x10000
	and	$4,$4,$2
	bne	$4,$0,$L44
	lui	$2,%hi(rtl819x_lastTxDesc)

	lui	$2,%hi(rtl819x_swHangCnt)
	j	$L45
	sw	$0,%lo(rtl819x_swHangCnt)($2)

$L44:
	lw	$2,%lo(rtl819x_lastTxDesc)($2)
	beq	$2,$0,$L46
	nop

	lui	$3,%hi(rtl819x_lastRxDesc)
	lw	$3,%lo(rtl819x_lastRxDesc)($3)
	bne	$3,$0,$L47
	nop

$L46:
	lui	$2,%hi(rtl819x_swHangCnt)
	j	$L45
	sw	$0,%lo(rtl819x_swHangCnt)($2)

$L47:
	bne	$6,$3,$L48
	nop

	bne	$5,$2,$L52
	lui	$2,%hi(rtl819x_swHangCnt)

	lui	$3,%hi(rtl819x_swHangCnt)
	lw	$2,%lo(rtl819x_swHangCnt)($3)
	addiu	$2,$2,1
	sw	$2,%lo(rtl819x_swHangCnt)($3)
	lui	$3,%hi(rtl819x_lastRxDesc)
	sw	$6,%lo(rtl819x_lastRxDesc)($3)
	lui	$3,%hi(rtl819x_lastTxDesc)
	sltu	$2,$2,3
	bne	$2,$0,$L50
	sw	$5,%lo(rtl819x_lastTxDesc)($3)

	j	$L51
	lui	$2,%hi(rtl819x_swHangCnt)

$L48:
	lui	$2,%hi(rtl819x_swHangCnt)
$L52:
	j	$L45
	sw	$0,%lo(rtl819x_swHangCnt)($2)

$L51:
	sw	$0,%lo(rtl819x_swHangCnt)($2)
	lui	$4,%hi($LC3)
	jal	panic_printk
	addiu	$4,$4,%lo($LC3)

	jal	machine_restart
	move	$4,$0

	j	$L50
	nop

$L45:
	lui	$2,%hi(rtl819x_lastRxDesc)
	sw	$6,%lo(rtl819x_lastRxDesc)($2)
	lui	$2,%hi(rtl819x_lastTxDesc)
	sw	$5,%lo(rtl819x_lastTxDesc)($2)
$L50:
	lw	$31,20($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	rtl819x_poll_sw
	.size	rtl819x_poll_sw, .-rtl819x_poll_sw
	.section	.text.check_8367r_connected,"ax",@progbits
	.align	2
	.globl	check_8367r_connected
	.set	nomips16
	.ent	check_8367r_connected
	.type	check_8367r_connected, @function
check_8367r_connected:
	.frame	$sp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$16,24($sp)
	li	$2,-1207959552			# 0xb8000000
	ori	$3,$2,0x40
	lw	$16,0($3)
	lw	$4,0($3)
	ori	$4,$4,0x18
	sw	$4,0($3)
	ori	$3,$2,0x351c
	lw	$5,0($3)
	li	$4,-24577			# 0xffff9fff
	and	$4,$5,$4
	sw	$4,0($3)
	ori	$2,$2,0x3524
	lw	$3,0($2)
	ori	$3,$3,0x6000
	sw	$3,0($2)
	li	$4,5			# 0x5
	li	$5,5			# 0x5
	jal	smi_init
	li	$6,6			# 0x6

	li	$4,5058			# 0x13c2
	jal	smi_write
	li	$5,585			# 0x249

	bne	$2,$0,$L55
	move	$2,$0

	li	$4,4864			# 0x1300
	jal	smi_read
	addiu	$5,$sp,16

	bne	$2,$0,$L54
	nop

	lw	$2,16($sp)
	xori	$2,$2,0x6000
	j	$L55
	sltu	$2,$2,1

$L54:
	move	$2,$0
$L55:
	li	$3,-1207959552			# 0xb8000000
	ori	$3,$3,0x40
	sw	$16,0($3)
	lw	$31,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	check_8367r_connected
	.size	check_8367r_connected, .-check_8367r_connected
	.section	.text.rtl865x_initAsicFun,"ax",@progbits
	.align	2
	.globl	rtl865x_initAsicFun
	.set	nomips16
	.ent	rtl865x_initAsicFun
	.type	rtl865x_initAsicFun, @function
rtl865x_initAsicFun:
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
	sw	$0,0($4)
	li	$3,-1207959552			# 0xb8000000
	lw	$4,0($3)
	li	$2,-65536			# 0xffff0000
	and	$2,$4,$2
	ori	$3,$3,0xc
	lw	$17,0($3)
	li	$3,-1073741824			# 0xc0000000
	bne	$2,$3,$L58
	andi	$17,$17,0xf

	li	$2,9			# 0x9
	beq	$17,$2,$L77
	li	$2,46			# 0x2e

	andi	$2,$17,0x8
	bne	$2,$0,$L60
	li	$2,46			# 0x2e

$L77:
	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L60:
	li	$2,8			# 0x8
	bne	$17,$2,$L62
	addiu	$17,$17,-10

	li	$2,14			# 0xe
	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L62:
	sltu	$17,$17,2
	beq	$17,$0,$L63
	nop

	li	$2,10			# 0xa
	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L63:
	li	$2,14			# 0xe
	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L58:
	li	$3,-2147483648			# 0x80000000
	bne	$2,$3,$L64
	nop

	li	$2,7			# 0x7
	bne	$17,$2,$L65
	li	$2,46			# 0x2e

	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L65:
	li	$2,15			# 0xf
	bne	$17,$2,$L66
	li	$2,3			# 0x3

	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L66:
	bne	$17,$2,$L67
	nop

	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L67:
	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L64:
	li	$3,-2120810496			# 0x81970000
	bne	$2,$3,$L68
	li	$3,-2004811776			# 0x88810000

	jal	check_8367r_connected
	nop

	beq	$2,$0,$L69
	li	$2,15			# 0xf

	lw	$2,0($16)
	ori	$2,$2,0xe
	j	$L70
	sw	$2,0($16)

$L69:
	beq	$17,$2,$L71
	li	$2,8			# 0x8

	beq	$17,$2,$L71
	li	$2,11			# 0xb

	bne	$17,$2,$L72
	li	$2,14			# 0xe

$L71:
	lw	$2,0($16)
	ori	$2,$2,0xe
	j	$L70
	sw	$2,0($16)

$L72:
	beq	$17,$2,$L73
	li	$2,9			# 0x9

	beq	$17,$2,$L73
	andi	$17,$17,0x8

	bne	$17,$0,$L74
	nop

$L73:
	lw	$2,0($16)
	ori	$2,$2,0x2e
	j	$L70
	sw	$2,0($16)

$L74:
	lw	$2,0($16)
	ori	$2,$2,0x3
	j	$L70
	sw	$2,0($16)

$L68:
	bne	$2,$3,$L75
	li	$2,14			# 0xe

	sw	$2,0($16)
	j	$L61
	move	$2,$0

$L70:
	lw	$2,0($16)
	bne	$2,$0,$L61
	move	$2,$0

$L75:
	li	$2,-1			# 0xffffffff
$L61:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl865x_initAsicFun
	.size	rtl865x_initAsicFun, .-rtl865x_initAsicFun
	.section	.rodata.str1.4
	.align	2
$LC4:
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
	li	$2,-1207959552			# 0xb8000000
	lw	$17,0($2)
	ori	$2,$2,0xc
	lw	$18,0($2)
	lui	$4,%hi(fun_enable)
	jal	rtl865x_initAsicFun
	addiu	$4,$4,%lo(fun_enable)

	li	$3,-1			# 0xffffffff
	beq	$2,$3,$L80
	li	$2,-1			# 0xffffffff

	li	$2,-65536			# 0xffff0000
	and	$17,$17,$2
	li	$2,-1073741824			# 0xc0000000
	bne	$17,$2,$L81
	andi	$18,$18,0xf

	li	$2,9			# 0x9
	bne	$16,$2,$L82
	nop

	beq	$18,$2,$L82
	andi	$18,$18,0x8

	beq	$18,$0,$L82
	lui	$4,%hi($LC4)

	addiu	$4,$4,%lo($LC4)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L80
	li	$2,-1			# 0xffffffff

$L81:
	li	$2,-2147483648			# 0x80000000
	bne	$17,$2,$L83
	li	$2,-2120810496			# 0x81970000

	li	$2,7			# 0x7
	bne	$16,$2,$L84
	nop

	beq	$18,$2,$L82
	lui	$4,%hi($LC4)

	addiu	$4,$4,%lo($LC4)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L80
	li	$2,-1			# 0xffffffff

$L84:
	li	$2,15			# 0xf
	bne	$16,$2,$L85
	nop

	beq	$18,$2,$L85
	li	$2,7			# 0x7

	beq	$18,$2,$L82
	li	$2,3			# 0x3

	beq	$18,$2,$L82
	li	$2,11			# 0xb

	beq	$18,$2,$L82
	lui	$4,%hi($LC4)

	addiu	$4,$4,%lo($LC4)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L80
	li	$2,-1			# 0xffffffff

$L85:
	li	$2,3			# 0x3
	bne	$16,$2,$L82
	nop

	beq	$18,$2,$L82
	li	$2,7			# 0x7

	beq	$18,$2,$L82
	lui	$4,%hi($LC4)

	addiu	$4,$4,%lo($LC4)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L80
	li	$2,-1			# 0xffffffff

$L83:
	bne	$17,$2,$L88
	move	$2,$0

	li	$2,8			# 0x8
	bne	$16,$2,$L89
	li	$2,11			# 0xb

	li	$2,15			# 0xf
	beq	$18,$2,$L86
	li	$2,11			# 0xb

	beq	$18,$2,$L82
	li	$2,8			# 0x8

	beq	$18,$2,$L82
	li	$2,14			# 0xe

	beq	$18,$2,$L82
	li	$2,9			# 0x9

	beq	$18,$2,$L88
	move	$2,$0

	beq	$18,$0,$L88
	lui	$4,%hi($LC4)

	addiu	$4,$4,%lo($LC4)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L80
	li	$2,-1			# 0xffffffff

$L86:
$L89:
	bne	$16,$2,$L88
	move	$2,$0

	li	$2,14			# 0xe
	beq	$18,$2,$L82
	li	$2,9			# 0x9

	beq	$18,$2,$L88
	move	$2,$0

	beq	$18,$0,$L88
	lui	$4,%hi($LC4)

	addiu	$4,$4,%lo($LC4)
	jal	early_console_write
	li	$5,27			# 0x1b

	j	$L80
	li	$2,-1			# 0xffffffff

$L82:
	move	$2,$0
$L80:
$L88:
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
	bne	$2,$0,$L91
	move	$17,$4

	lui	$16,%hi(fun_enable)
	jal	rtl865x_initAsicFun
	addiu	$4,$16,%lo(fun_enable)

	lw	$2,%lo(fun_enable)($16)
	sw	$2,0($17)
$L91:
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
	beq	$2,$0,$L94
	li	$2,1			# 0x1

	sll	$16,$2,$16
	andi	$2,$16,0xe22
	bne	$2,$0,$L95
	andi	$2,$16,0x4000

	bne	$2,$0,$L97
	andi	$16,$16,0x8

	bne	$16,$0,$L96
	li	$2,1			# 0x1

	j	$L99
	sw	$2,0($17)

$L97:
	lw	$2,16($sp)
	andi	$2,$2,0x20
	beq	$2,$0,$L98
	li	$2,1			# 0x1

	j	$L99
	sw	$2,0($17)

$L98:
	j	$L99
	sw	$0,0($17)

$L95:
	lw	$2,16($sp)
	andi	$2,$2,0x4
	beq	$2,$0,$L100
	li	$2,1			# 0x1

	j	$L99
	sw	$2,0($17)

$L100:
	j	$L99
	sw	$0,0($17)

$L96:
	lw	$2,16($sp)
	andi	$2,$2,0x2
	beq	$2,$0,$L101
	li	$2,1			# 0x1

	j	$L99
	sw	$2,0($17)

$L101:
	j	$L99
	sw	$0,0($17)

$L94:
	sw	$2,0($17)
$L99:
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

	lw	$2,16($sp)
	beq	$2,$0,$L106
	li	$2,-1			# 0xffffffff

	bne	$16,$0,$L107
	sll	$3,$17,16

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	panic_printk
	li	$6,601			# 0x259

$L108:
	j	$L108
	nop

$L107:
	li	$2,-1157627904			# 0xbb000000
	addu	$2,$3,$2
	sll	$18,$18,5
	addu	$18,$2,$18
	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L109
	li	$3,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L121
	li	$2,-1149239296			# 0xbb800000

	j	$L119
	sll	$3,$17,2

$L109:
	beq	$2,$3,$L110
	li	$3,4			# 0x4

	bne	$2,$3,$L119
	sll	$3,$17,2

$L110:
	li	$2,-1149239296			# 0xbb800000
$L121:
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,262144			# 0x40000
	or	$3,$4,$3
	sw	$3,0($2)
	sll	$3,$17,2
$L119:
	lui	$2,%hi(_rtl8651_asicTableSize)
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$2,0($2)
	beq	$2,$0,$L112
	move	$2,$0

	move	$17,$3
	lui	$3,%hi(_rtl8651_asicTableSize)
	addiu	$3,$3,%lo(_rtl8651_asicTableSize)
	addu	$17,$17,$3
	lw	$4,0($17)
$L113:
	lw	$3,0($18)
	sw	$3,0($16)
	addiu	$2,$2,1
	addiu	$18,$18,4
	sltu	$3,$2,$4
	bne	$3,$0,$L113
	addiu	$16,$16,4

$L112:
	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L114
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L115
	nop

	j	$L120
	move	$2,$0

$L114:
	li	$3,3			# 0x3
	beq	$2,$3,$L115
	li	$3,4			# 0x4

	bne	$2,$3,$L116
	nop

$L115:
	li	$2,-1149239296			# 0xbb800000
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,-327680			# 0xfffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,0($2)
	j	$L106
	move	$2,$0

$L116:
	move	$2,$0
$L106:
$L120:
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
	move	$17,$4
	move	$18,$5
	move	$16,$6
	jal	rtl865x_accessAsicTable
	addiu	$5,$sp,16

	lw	$2,16($sp)
	beq	$2,$0,$L124
	li	$2,-1			# 0xffffffff

	bne	$16,$0,$L125
	sll	$3,$17,16

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	lui	$5,%hi($LC1)
	addiu	$5,$5,%lo($LC1)
	jal	panic_printk
	li	$6,506			# 0x1fa

$L126:
	j	$L126
	nop

$L125:
	li	$2,-1157627904			# 0xbb000000
	addu	$2,$3,$2
	sll	$18,$18,5
	addu	$18,$2,$18
	li	$3,-1149239296			# 0xbb800000
	ori	$3,$3,0x4d00
$L127:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L127
	li	$14,10			# 0xa

	addiu	$12,$18,4
	addiu	$11,$18,8
	addiu	$10,$18,12
	addiu	$9,$18,16
	addiu	$8,$18,20
	addiu	$7,$18,24
	addiu	$6,$18,28
	addiu	$13,$sp,56
	addiu	$5,$sp,84
$L131:
	lw	$2,0($18)
	sw	$2,20($sp)
	lw	$3,0($12)
	sw	$3,24($sp)
	lw	$3,0($11)
	sw	$3,28($sp)
	lw	$3,0($10)
	sw	$3,32($sp)
	lw	$3,0($9)
	sw	$3,36($sp)
	lw	$3,0($8)
	sw	$3,40($sp)
	lw	$3,0($7)
	sw	$3,44($sp)
	lw	$3,0($6)
	sw	$3,48($sp)
	lw	$3,0($18)
	sw	$3,52($sp)
	lw	$4,0($12)
	sw	$4,56($sp)
	lw	$4,0($11)
	sw	$4,60($sp)
	lw	$4,0($10)
	sw	$4,64($sp)
	lw	$4,0($9)
	sw	$4,68($sp)
	lw	$4,0($8)
	sw	$4,72($sp)
	lw	$4,0($7)
	sw	$4,76($sp)
	lw	$4,0($6)
	bne	$3,$2,$L128
	sw	$4,80($sp)

	move	$2,$13
$L129:
	lw	$4,0($2)
	lw	$3,-32($2)
	bne	$4,$3,$L128
	addiu	$2,$2,4

	bne	$2,$5,$L129
	sll	$3,$17,2

	j	$L140
	lui	$2,%hi(_rtl8651_asicTableSize)

$L128:
	addiu	$14,$14,-1
	bne	$14,$0,$L131
	sll	$3,$17,2

	lui	$2,%hi(_rtl8651_asicTableSize)
$L140:
	addiu	$2,$2,%lo(_rtl8651_asicTableSize)
	addu	$2,$3,$2
	lw	$2,0($2)
	beq	$2,$0,$L132
	move	$2,$0

	addiu	$3,$sp,52
	sll	$17,$17,2
	lui	$4,%hi(_rtl8651_asicTableSize)
	addiu	$4,$4,%lo(_rtl8651_asicTableSize)
	addu	$17,$17,$4
	lw	$5,0($17)
$L133:
	lw	$4,0($3)
	sw	$4,0($16)
	addiu	$2,$2,1
	addiu	$3,$3,4
	sltu	$4,$2,$5
	bne	$4,$0,$L133
	addiu	$16,$16,4

$L132:
	move	$2,$0
$L124:
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
	beq	$2,$0,$L143
	li	$2,-1			# 0xffffffff

	li	$2,3			# 0x3
	bne	$18,$2,$L158
	lui	$2,%hi(RtkHomeGatewayChipNameID)

	lui	$2,%hi(mcastForceAddOpCnt)
	lw	$3,%lo(mcastForceAddOpCnt)($2)
	addiu	$3,$3,1
	sw	$3,%lo(mcastForceAddOpCnt)($2)
	lui	$2,%hi(RtkHomeGatewayChipNameID)
$L158:
	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L145
	li	$3,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L159
	li	$2,-1149239296			# 0xbb800000

	j	$L156
	move	$4,$18

$L145:
	beq	$2,$3,$L146
	li	$3,4			# 0x4

	bne	$2,$3,$L156
	move	$4,$18

$L146:
	li	$2,-1149239296			# 0xbb800000
$L159:
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,262144			# 0x40000
	or	$3,$4,$3
	sw	$3,0($2)
	move	$4,$2
	li	$3,524288			# 0x80000
$L148:
	lw	$2,0($4)
	and	$2,$2,$3
	beq	$2,$0,$L148
	nop

	move	$4,$18
$L156:
	move	$5,$16
	jal	_rtl8651_asicTableAccessForward
	move	$6,$17

	li	$3,9			# 0x9
	li	$2,-1149239296			# 0xbb800000
	ori	$2,$2,0x4d00
	sw	$3,0($2)
	move	$3,$2
$L149:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L149
	lui	$2,%hi(RtkHomeGatewayChipNameID)

	lw	$2,%lo(RtkHomeGatewayChipNameID)($2)
	li	$3,2			# 0x2
	bne	$2,$3,$L150
	nop

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L151
	nop

	j	$L157
	move	$2,$0

$L150:
	li	$3,3			# 0x3
	beq	$2,$3,$L151
	li	$3,4			# 0x4

	bne	$2,$3,$L152
	nop

$L151:
	li	$2,-1149239296			# 0xbb800000
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,-327680			# 0xfffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,0($2)
	j	$L143
	move	$2,$0

$L152:
	move	$2,$0
$L143:
$L157:
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
	beq	$2,$0,$L179
	li	$2,-1			# 0xffffffff

	move	$4,$18
	move	$5,$16
	jal	_rtl8651_asicTableAccessForward
	move	$6,$17

	lui	$2,%hi(RtkHomeGatewayChipNameID)
	lw	$4,%lo(RtkHomeGatewayChipNameID)($2)
	li	$2,2			# 0x2
	bne	$4,$2,$L162
	li	$2,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L180
	li	$2,-1149239296			# 0xbb800000

	j	$L182
	li	$3,3			# 0x3

$L162:
	beq	$4,$2,$L163
	li	$2,4			# 0x4

	bne	$4,$2,$L177
	li	$3,3			# 0x3

$L163:
	li	$2,-1149239296			# 0xbb800000
$L180:
	ori	$2,$2,0x4418
	lw	$5,0($2)
	li	$3,262144			# 0x40000
	or	$3,$5,$3
	sw	$3,0($2)
	move	$5,$2
	li	$3,524288			# 0x80000
$L165:
	lw	$2,0($5)
	and	$2,$2,$3
	beq	$2,$0,$L165
	nop

	li	$3,3			# 0x3
$L177:
	li	$2,-1149239296			# 0xbb800000
$L182:
	ori	$2,$2,0x4d00
	sw	$3,0($2)
	move	$3,$2
$L166:
	lw	$2,0($3)
	andi	$2,$2,0x1
	bne	$2,$0,$L166
	li	$2,-1149239296			# 0xbb800000

	ori	$2,$2,0x4d04
	lw	$2,0($2)
	andi	$2,$2,0x1
	beq	$2,$0,$L167
	li	$2,2			# 0x2

	bne	$4,$2,$L168
	li	$2,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	blez	$2,$L161
	nop

	j	$L178
	li	$2,-1149239296			# 0xbb800000

$L168:
	beq	$4,$2,$L178
	li	$2,-1149239296			# 0xbb800000

	li	$2,4			# 0x4
	bne	$4,$2,$L161
	nop

	li	$2,-1149239296			# 0xbb800000
$L178:
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,-327680			# 0xfffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,0($2)
	j	$L170
	li	$2,-1			# 0xffffffff

$L167:
	bne	$4,$2,$L171
	li	$2,3			# 0x3

	lui	$2,%hi(RtkHomeGatewayChipRevisionID)
	lw	$2,%lo(RtkHomeGatewayChipRevisionID)($2)
	bgtz	$2,$L172
	nop

	j	$L179
	move	$2,$0

$L171:
	beq	$4,$2,$L181
	li	$2,-1149239296			# 0xbb800000

	li	$2,4			# 0x4
	bne	$4,$2,$L173
	nop

$L172:
	li	$2,-1149239296			# 0xbb800000
$L181:
	ori	$2,$2,0x4418
	lw	$4,0($2)
	li	$3,-327680			# 0xfffb0000
	ori	$3,$3,0xffff
	and	$3,$4,$3
	sw	$3,0($2)
	j	$L170
	move	$2,$0

$L161:
	j	$L170
	li	$2,-1			# 0xffffffff

$L173:
	move	$2,$0
$L170:
$L179:
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
	.local	rtl819x_checkSwCoreTimer
	.comm	rtl819x_checkSwCoreTimer,4,4
	.local	rtl819x_swHangCnt
	.comm	rtl819x_swHangCnt,4,4
	.local	rtl819x_lastTxDesc
	.comm	rtl819x_lastTxDesc,4,4
	.local	rtl819x_lastRxDesc
	.comm	rtl819x_lastRxDesc,4,4
	.rdata
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
	.local	mcastForceAddOpCnt
	.comm	mcastForceAddOpCnt,4,4
	.section	.dram-fwd,"aw",@progbits
	.align	2
	.type	fun_enable, @object
	.size	fun_enable, 4
fun_enable:
	.space	4
	.globl	RtkHomeGatewayChipName
	.section	.bss,"aw",@nobits
	.align	2
	.type	RtkHomeGatewayChipName, @object
	.size	RtkHomeGatewayChipName, 16
RtkHomeGatewayChipName:
	.space	16
	.globl	RtkHomeGatewayChipNameID
	.align	2
	.type	RtkHomeGatewayChipNameID, @object
	.size	RtkHomeGatewayChipNameID, 4
RtkHomeGatewayChipNameID:
	.space	4
	.globl	RtkHomeGatewayChipRevisionID
	.align	2
	.type	RtkHomeGatewayChipRevisionID, @object
	.size	RtkHomeGatewayChipRevisionID, 4
RtkHomeGatewayChipRevisionID:
	.space	4
	.ident	"GCC: (GNU) 4.4.5-1.5.5p4"
