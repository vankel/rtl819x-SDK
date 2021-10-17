	.file	1 "wapiRandom.c"
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
	.section	.text.WapiMTgenSeed,"ax",@progbits
	.align	2
	.globl	WapiMTgenSeed
	.set	nomips16
	.ent	WapiMTgenSeed
	.type	WapiMTgenSeed, @function
WapiMTgenSeed:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$3,%hi(mt)
	li	$2,1			# 0x1
	sw	$4,%lo(mt)($3)
	li	$4,1812398080			# 0x6c070000
	addiu	$3,$3,%lo(mt)
	j	$L2
	ori	$4,$4,0x8965

$L3:
	lw	$5,-4($3)
	srl	$6,$5,30
	xor	$5,$6,$5
	mult	$5,$4
	mflo	$5
	addu	$5,$2,$5
	addiu	$2,$2,1
	sw	$5,0($3)
$L2:
	slt	$5,$2,624
	bne	$5,$0,$L3
	addiu	$3,$3,4

	lui	$3,%hi(mti)
	j	$31
	sw	$2,%lo(mti)($3)

	.set	macro
	.set	reorder
	.end	WapiMTgenSeed
	.size	WapiMTgenSeed, .-WapiMTgenSeed
	.section	.text.WapiMTgenrand,"ax",@progbits
	.align	2
	.globl	WapiMTgenrand
	.set	nomips16
	.ent	WapiMTgenrand
	.type	WapiMTgenrand, @function
WapiMTgenrand:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(mti)
	lw	$2,%lo(mti)($2)
	slt	$3,$2,624
	bne	$3,$0,$L7
	li	$3,625			# 0x271

	bne	$2,$3,$L8
	lui	$2,%hi(mt)

	li	$4,5489			# 0x1571
	sw	$4,%lo(mt)($2)
	li	$4,1812398080			# 0x6c070000
	addiu	$3,$2,%lo(mt)
	ori	$4,$4,0x8965
	j	$L9
	li	$2,1			# 0x1

$L10:
	lw	$5,-4($3)
	srl	$6,$5,30
	xor	$5,$6,$5
	mult	$5,$4
	mflo	$5
	addu	$5,$2,$5
	addiu	$2,$2,1
	sw	$5,0($3)
$L9:
	slt	$5,$2,624
	bne	$5,$0,$L10
	addiu	$3,$3,4

$L8:
	lui	$2,%hi(mt)
	li	$8,2147418112			# 0x7fff0000
	addiu	$2,$2,%lo(mt)
	lui	$6,%hi(mag01.40301)
	ori	$8,$8,0xffff
	addiu	$6,$6,%lo(mag01.40301)
	move	$7,$2
	addiu	$5,$2,908
	li	$4,-2147483648			# 0x80000000
$L11:
	lw	$9,0($2)
	lw	$3,4($2)
	and	$9,$9,$4
	lw	$10,1588($2)
	and	$3,$3,$8
	or	$3,$3,$9
	andi	$9,$3,0x1
	sll	$9,$9,2
	addu	$9,$9,$6
	srl	$3,$3,1
	lw	$9,0($9)
	xor	$3,$3,$10
	xor	$3,$3,$9
	sw	$3,0($2)
	addiu	$2,$2,4
	bne	$2,$5,$L11
	nop

	lui	$2,%hi(mt+908)
	li	$5,2147418112			# 0x7fff0000
	lui	$4,%hi(mag01.40301)
	addiu	$2,$2,%lo(mt+908)
	ori	$5,$5,0xffff
	addiu	$7,$7,2492
	addiu	$4,$4,%lo(mag01.40301)
	li	$6,-2147483648			# 0x80000000
$L12:
	lw	$8,0($2)
	lw	$3,4($2)
	and	$8,$8,$6
	lw	$9,-908($2)
	and	$3,$3,$5
	or	$3,$3,$8
	andi	$8,$3,0x1
	sll	$8,$8,2
	addu	$8,$8,$4
	srl	$3,$3,1
	lw	$8,0($8)
	xor	$3,$3,$9
	xor	$3,$3,$8
	sw	$3,0($2)
	addiu	$2,$2,4
	bne	$2,$7,$L12
	nop

	lui	$6,%hi(mt)
	addiu	$2,$6,%lo(mt)
	lw	$7,2492($2)
	lw	$3,%lo(mt)($6)
	li	$6,-2147483648			# 0x80000000
	and	$7,$7,$6
	lw	$6,1584($2)
	and	$3,$3,$5
	lui	$5,%hi(mti)
	or	$3,$3,$7
	sw	$0,%lo(mti)($5)
	andi	$5,$3,0x1
	sll	$5,$5,2
	addu	$4,$5,$4
	srl	$3,$3,1
	lw	$4,0($4)
	xor	$3,$3,$6
	xor	$3,$3,$4
	sw	$3,2492($2)
$L7:
	lui	$3,%hi(mti)
	lw	$2,%lo(mti)($3)
	sll	$5,$2,2
	addiu	$4,$2,1
	lui	$2,%hi(mt)
	addiu	$2,$2,%lo(mt)
	addu	$2,$5,$2
	lw	$2,0($2)
	sw	$4,%lo(mti)($3)
	li	$3,-1658060800			# 0x9d2c0000
	srl	$4,$2,11
	xor	$4,$4,$2
	sll	$2,$4,7
	ori	$3,$3,0x5680
	and	$3,$2,$3
	li	$2,-272236544			# 0xefc60000
	xor	$3,$3,$4
	sll	$4,$3,15
	and	$2,$4,$2
	xor	$2,$2,$3
	srl	$3,$2,18
	j	$31
	xor	$2,$3,$2

	.set	macro
	.set	reorder
	.end	WapiMTgenrand
	.size	WapiMTgenrand, .-WapiMTgenrand
	.section	.text.GenerateRandomData,"ax",@progbits
	.align	2
	.globl	GenerateRandomData
	.set	nomips16
	.ent	GenerateRandomData
	.type	GenerateRandomData, @function
GenerateRandomData:
	.frame	$sp,88,$31		# vars= 40, regs= 7/0, args= 16, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-88
	sw	$31,84($sp)
	sw	$21,80($sp)
	sw	$20,76($sp)
	sw	$19,72($sp)
	sw	$18,68($sp)
	sw	$17,64($sp)
	sw	$16,60($sp)
	lui	$2,%hi(jiffies)
	move	$20,$4
	lw	$4,%lo(jiffies)($2)
	jal	srandom32
	move	$16,$5

	jal	random32
	nop

	lui	$4,%hi(mt)
	sw	$2,%lo(mt)($4)
	addiu	$3,$4,%lo(mt)
	li	$4,1812398080			# 0x6c070000
	li	$2,1			# 0x1
	j	$L17
	ori	$4,$4,0x8965

$L18:
	lw	$5,-4($3)
	srl	$6,$5,30
	xor	$5,$6,$5
	mult	$5,$4
	mflo	$5
	addu	$5,$2,$5
	addiu	$2,$2,1
	sw	$5,0($3)
$L17:
	slt	$5,$2,624
	bne	$5,$0,$L18
	addiu	$3,$3,4

	srl	$3,$16,2
	addiu	$17,$sp,20
	move	$5,$0
	andi	$19,$16,0x3
	addiu	$4,$3,1
	move	$18,$0
	movn	$3,$4,$19
	move	$4,$17
	addiu	$21,$sp,16
	move	$19,$3
	sll	$6,$3,2
	lui	$3,%hi(mti)
	jal	memset
	sw	$2,%lo(mti)($3)

	j	$L24
	slt	$2,$18,$19

$L22:
	jal	WapiMTgenrand
	nop

	move	$4,$17
	li	$6,4			# 0x4
	move	$5,$21
	sw	$2,16($sp)
	jal	memcpy
	addiu	$17,$17,4

	slt	$2,$18,$19
$L24:
	bne	$2,$0,$L22
	addiu	$18,$18,1

	move	$4,$20
	addiu	$5,$sp,20
	jal	memcpy
	move	$6,$16

	lw	$31,84($sp)
	lw	$21,80($sp)
	lw	$20,76($sp)
	lw	$19,72($sp)
	lw	$18,68($sp)
	lw	$17,64($sp)
	lw	$16,60($sp)
	j	$31
	addiu	$sp,$sp,88

	.set	macro
	.set	reorder
	.end	GenerateRandomData
	.size	GenerateRandomData, .-GenerateRandomData
	.data
	.align	2
	.type	mti, @object
	.size	mti, 4
mti:
	.word	625
	.rdata
	.align	2
	.type	mag01.40301, @object
	.size	mag01.40301, 8
mag01.40301:
	.word	0
	.word	-1727483681
	.local	mt
	.comm	mt,2496,4
	.ident	"GCC: (GNU) 4.4.5-1.5.5p4"
