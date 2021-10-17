	.file	1 "wapiRandom.c"
	.section .mdebug.abi32
	.previous
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
	.data
	.align	2
	.type	mti, @object
	.size	mti, 4
mti:
	.word	625
	.section	.text.WapiMTgenSeed,"ax",@progbits
	.align	2
	.globl	WapiMTgenSeed
	.ent	WapiMTgenSeed
	.type	WapiMTgenSeed, @function
WapiMTgenSeed:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$3,%hi(mt)
	lui	$7,%hi(mti)
	addiu	$2,$3,%lo(mt)
	addiu	$5,$2,4
	li	$2,1812398080			# 0x6c070000
	ori	$6,$2,0x8965
	li	$2,1			# 0x1
	sw	$4,%lo(mt)($3)
	li	$4,1			# 0x1
	sw	$2,%lo(mti)($7)
$L5:
	lw	$2,-4($5)
	srl	$3,$2,30
	xor	$2,$2,$3
	mult	$2,$6
	mflo	$2
	addu	$2,$2,$4
	addiu	$4,$4,1
	slt	$3,$4,624
	sw	$2,0($5)
	bne	$3,$0,$L5
	addiu	$5,$5,4

	li	$2,624			# 0x270
	j	$31
	sw	$2,%lo(mti)($7)

	.set	macro
	.set	reorder
	.end	WapiMTgenSeed
	.data
	.align	2
	.type	mag01.0, @object
	.size	mag01.0, 8
mag01.0:
	.word	0
	.word	-1727483681
	.section	.text.WapiMTgenrand,"ax",@progbits
	.align	2
	.globl	WapiMTgenrand
	.ent	WapiMTgenrand
	.type	WapiMTgenrand, @function
WapiMTgenrand:
	.set	nomips16
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$16,16($sp)
	lui	$16,%hi(mti)
	lw	$3,%lo(mti)($16)
	slt	$2,$3,624
	bne	$2,$0,$L10
	sw	$31,20($sp)

	li	$2,625			# 0x271
	bne	$3,$2,$L24
	li	$2,2147418112			# 0x7fff0000

	jal	WapiMTgenSeed
	li	$4,5489			# 0x1571

	li	$2,2147418112			# 0x7fff0000
$L24:
	li	$7,-2147483648			# 0x80000000
	ori	$10,$2,0xffff
	lui	$2,%hi(mag01.0)
	addiu	$9,$2,%lo(mag01.0)
	lui	$2,%hi(mt)
	addiu	$5,$2,%lo(mt)
	li	$8,226			# 0xe2
$L15:
	lw	$3,0($5)
	addiu	$8,$8,-1
	lw	$2,4($5)
	and	$3,$3,$7
	and	$2,$2,$10
	or	$6,$3,$2
	andi	$3,$6,0x1
	srl	$4,$6,1
	sll	$3,$3,2
	lw	$2,1588($5)
	addu	$3,$3,$9
	lw	$3,0($3)
	xor	$2,$2,$4
	xor	$2,$2,$3
	sw	$2,0($5)
	bgez	$8,$L15
	addiu	$5,$5,4

	li	$2,2147418112			# 0x7fff0000
	li	$8,227			# 0xe3
	ori	$11,$2,0xffff
	lui	$2,%hi(mag01.0)
	addiu	$10,$2,%lo(mag01.0)
	lui	$2,%hi(mt)
	addiu	$2,$2,%lo(mt)
	li	$9,-2147483648			# 0x80000000
	addiu	$7,$2,908
$L19:
	lw	$3,0($7)
	addiu	$8,$8,1
	lw	$2,4($7)
	slt	$5,$8,623
	and	$3,$3,$9
	and	$2,$2,$11
	or	$6,$3,$2
	andi	$3,$6,0x1
	srl	$4,$6,1
	sll	$3,$3,2
	lw	$2,-908($7)
	addu	$3,$3,$10
	lw	$3,0($3)
	xor	$2,$2,$4
	xor	$2,$2,$3
	sw	$2,0($7)
	bne	$5,$0,$L19
	addiu	$7,$7,4

	lui	$2,%hi(mt)
	addiu	$5,$2,%lo(mt)
	lw	$4,%lo(mt)($2)
	li	$2,2147418112			# 0x7fff0000
	lw	$3,2492($5)
	ori	$2,$2,0xffff
	and	$4,$4,$2
	li	$2,-2147483648			# 0x80000000
	and	$3,$3,$2
	lui	$2,%hi(mag01.0)
	or	$6,$3,$4
	addiu	$2,$2,%lo(mag01.0)
	andi	$3,$6,0x1
	srl	$4,$6,1
	sll	$3,$3,2
	addu	$3,$3,$2
	lw	$2,1584($5)
	lw	$3,0($3)
	xor	$2,$2,$4
	xor	$2,$2,$3
	sw	$2,2492($5)
	sw	$0,%lo(mti)($16)
$L10:
	lui	$5,%hi(mti)
	lui	$2,%hi(mt)
	lw	$4,%lo(mti)($5)
	addiu	$2,$2,%lo(mt)
	lw	$31,20($sp)
	sll	$3,$4,2
	addiu	$4,$4,1
	addu	$3,$3,$2
	lw	$6,0($3)
	lw	$16,16($sp)
	addiu	$sp,$sp,24
	srl	$2,$6,11
	xor	$6,$6,$2
	li	$2,-1658060800			# 0x9d2c0000
	sll	$3,$6,7
	ori	$2,$2,0x5680
	and	$3,$3,$2
	xor	$6,$6,$3
	li	$3,-272236544			# 0xefc60000
	sll	$2,$6,15
	and	$2,$2,$3
	xor	$6,$6,$2
	srl	$2,$6,18
	xor	$2,$6,$2
	j	$31
	sw	$4,%lo(mti)($5)

	.set	macro
	.set	reorder
	.end	WapiMTgenrand
	.section	.text.GenerateRandomData,"ax",@progbits
	.align	2
	.globl	GenerateRandomData
	.ent	GenerateRandomData
	.type	GenerateRandomData, @function
GenerateRandomData:
	.set	nomips16
	.frame	$sp,80,$31		# vars= 40, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-80
	lui	$2,%hi(jiffies)
	sw	$31,72($sp)
	sw	$19,68($sp)
	move	$19,$4
	sw	$18,64($sp)
	move	$18,$5
	sw	$17,60($sp)
	move	$17,$0
	sw	$16,56($sp)
	lw	$4,%lo(jiffies)($2)
	jal	srandom32
	nop

	jal	random32
	nop

	jal	WapiMTgenSeed
	move	$4,$2

	srl	$3,$18,2
	addiu	$4,$3,1
	andi	$2,$18,0x3
	move	$16,$3
	move	$5,$0
	movn	$16,$4,$2		#conditional move  
	addiu	$4,$sp,16
	jal	memset
	sll	$6,$16,2

	beq	$16,$0,$L34
	move	$4,$19

$L31:
	jal	WapiMTgenrand
	nop

	sll	$4,$17,2
	addiu	$3,$sp,16
	addiu	$17,$17,1
	addu	$4,$3,$4
	addiu	$5,$sp,48
	li	$6,4			# 0x4
	jal	memcpy
	sw	$2,48($sp)

	slt	$2,$17,$16
	bne	$2,$0,$L31
	move	$4,$19

$L34:
	move	$6,$18
	jal	memcpy
	addiu	$5,$sp,16

	lw	$31,72($sp)
	lw	$19,68($sp)
	lw	$18,64($sp)
	lw	$17,60($sp)
	lw	$16,56($sp)
	j	$31
	addiu	$sp,$sp,80

	.set	macro
	.set	reorder
	.end	GenerateRandomData
	.local	mt
	.comm	mt,2496,4
	.ident	"GCC: (GNU) 3.4.6-1.3.6"
