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
	move	$3,$6
	sltu	$6,$6,2
	.set	noreorder
	.set	nomacro
	bne	$6,$0,$L2
	li	$2,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L2
	addiu	$6,$5,1
	.set	macro
	.set	reorder

	or	$4,$6,$5
	lw	$6,24($28)
	#nop
	and	$4,$4,$6
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L2
	move	$4,$sp
	.set	macro
	.set	reorder

	li	$6,1			# 0x1
 #APP
 # 68 "net/rtl/fastpath/fast_pppoe_core.c" 1
	.set	noreorder
	jal	__copy_user
	.set	noat
	addu	$1, $5, $6
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	bne	$6,$0,$L2
	lb	$4,0($sp)
	lui	$2,%hi(fast_pppoe_fw)
	addiu	$4,$4,-48
	sw	$4,%lo(fast_pppoe_fw)($2)
	move	$2,$3
$L2:
	lw	$31,12($sp)
	#nop
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
	.frame	$sp,64,$31		# vars= 8, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
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
	lw	$fp,88($sp)
	lui	$20,%hi(fast_pppoe_info+54)
	lui	$19,%hi(fast_pppoe_info)
	move	$17,$4
	move	$16,$5
	move	$23,$6
	addiu	$20,$20,%lo(fast_pppoe_info+54)
	move	$18,$0
	li	$21,88			# 0x58
	addiu	$19,$19,%lo(fast_pppoe_info)
$L16:
	lhu	$2,0($20)
	#nop
	beq	$2,$0,$L8
	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L9
	mult	$18,$21
	.set	macro
	.set	reorder

	move	$4,$17
	mflo	$2
	addiu	$3,$2,16
	addu	$3,$19,$3
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($3)
1:	lbu	$1,($4)
	addiu	$3,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($3)
	nop
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L10
	addu	$2,$19,$2
	.set	macro
	.set	reorder

$L9:
	.set	noreorder
	.set	nomacro
	beq	$16,$0,$L11
	mult	$18,$21
	.set	macro
	.set	reorder

	move	$4,$16
	mflo	$2
	addu	$2,$19,$2
	move	$3,$2
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($3)
1:	lbu	$1,($4)
	addiu	$3,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($3)
	nop
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	beq	$5,$0,$L10
$L11:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L12
	mult	$18,$21
	.set	macro
	.set	reorder

	move	$5,$fp
	li	$6,6			# 0x6
	sw	$7,16($sp)
	mflo	$22
	addu	$4,$19,$22
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$4,$4,38
	.set	macro
	.set	reorder

	lw	$7,16($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L10
	addu	$2,$19,$22
	.set	macro
	.set	reorder

$L12:
	lw	$3,84($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L13
	mult	$18,$21
	.set	macro
	.set	reorder

	move	$5,$3
	li	$6,6			# 0x6
	sw	$7,16($sp)
	mflo	$22
	addiu	$4,$22,32
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addu	$4,$19,$4
	.set	macro
	.set	reorder

	lw	$7,16($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L10
	addu	$2,$19,$22
	.set	macro
	.set	reorder

$L13:
	lw	$3,80($sp)
	#nop
	beq	$3,$0,$L14
	lw	$2,-6($20)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L20
	li	$2,88			# 0x58
	.set	macro
	.set	reorder

$L14:
	beq	$7,$0,$L15
	lw	$2,-10($20)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$7,$L20
	li	$2,88			# 0x58
	.set	macro
	.set	reorder

$L15:
	beq	$23,$0,$L8
	lhu	$2,-2($20)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$23,$L21
	addiu	$18,$18,1
	.set	macro
	.set	reorder

	addiu	$18,$18,-1
	li	$2,88			# 0x58
$L20:
	mult	$18,$2
	lui	$2,%hi(fast_pppoe_info)
	addiu	$2,$2,%lo(fast_pppoe_info)
	mflo	$18
	.set	noreorder
	.set	nomacro
	j	$L10
	addu	$2,$2,$18
	.set	macro
	.set	reorder

$L8:
	addiu	$18,$18,1
$L21:
	li	$2,16			# 0x10
	.set	noreorder
	.set	nomacro
	bne	$18,$2,$L16
	addiu	$20,$20,88
	.set	macro
	.set	reorder

	move	$2,$0
$L10:
	lw	$31,60($sp)
	lw	$fp,56($sp)
	lw	$23,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,64
	.set	macro
	.set	reorder

	.end	find_pppoe_info
	.size	find_pppoe_info, .-find_pppoe_info
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
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"fast pppoe enable:%c\012\000"
	.align	2
$LC1:
	.ascii	"[%d] null,null,0\012\000"
	.align	2
$LC2:
	.ascii	"[%d] %s,%s,%d,0x%x:%x:%x:%x:%x:%x\012\000"
	.section	.text.fast_pppoe_read_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	fast_pppoe_read_proc
	.type	fast_pppoe_read_proc, @function
fast_pppoe_read_proc:
	.frame	$sp,80,$31		# vars= 0, regs= 7/0, args= 48, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-80
	sw	$31,76($sp)
	sw	$21,72($sp)
	sw	$20,68($sp)
	sw	$19,64($sp)
	sw	$18,60($sp)
	sw	$17,56($sp)
	sw	$16,52($sp)
	lui	$2,%hi(fast_pppoe_fw)
	lw	$6,%lo(fast_pppoe_fw)($2)
	lui	$5,%hi($LC0)
	addiu	$5,$5,%lo($LC0)
	addiu	$6,$6,48
	lui	$16,%hi(fast_pppoe_info+54)
	lui	$18,%hi(fast_pppoe_info)
	lui	$21,%hi($LC2)
	move	$19,$4
	jal	seq_printf
	addiu	$16,$16,%lo(fast_pppoe_info+54)

	move	$17,$0
	addiu	$18,$18,%lo(fast_pppoe_info)
	addiu	$21,$21,%lo($LC2)
	lui	$20,%hi($LC1)
$L26:
	lhu	$2,0($16)
	nop
	bne	$2,$0,$L24
	li	$2,88			# 0x58

	move	$4,$19
	addiu	$5,$20,%lo($LC1)
	jal	seq_printf
	move	$6,$17

	j	$L28
	addiu	$17,$17,1

$L24:
	mult	$17,$2
	move	$4,$19
	move	$5,$21
	move	$6,$17
	mflo	$2
	addiu	$7,$2,16
	addu	$2,$18,$2
	sw	$2,16($sp)
	lhu	$2,-2($16)
	addu	$7,$18,$7
	sw	$2,20($sp)
	lbu	$2,-16($16)
	nop
	sw	$2,24($sp)
	lbu	$2,-15($16)
	nop
	sw	$2,28($sp)
	lbu	$2,-14($16)
	nop
	sw	$2,32($sp)
	lbu	$2,-13($16)
	nop
	sw	$2,36($sp)
	lbu	$2,-12($16)
	nop
	sw	$2,40($sp)
	lbu	$2,-11($16)
	jal	seq_printf
	sw	$2,44($sp)

	addiu	$17,$17,1
$L28:
	li	$2,16			# 0x10
	bne	$17,$2,$L26
	addiu	$16,$16,88

	lw	$31,76($sp)
	lw	$21,72($sp)
	lw	$20,68($sp)
	lw	$19,64($sp)
	lw	$18,60($sp)
	lw	$17,56($sp)
	lw	$16,52($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,80

	.set	macro
	.set	reorder
	.end	fast_pppoe_read_proc
	.size	fast_pppoe_read_proc, .-fast_pppoe_read_proc
	.section	.rodata.str1.4
	.align	2
$LC3:
	.ascii	"fast_pppoe\000"
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
	lui	$4,%hi($LC3)
	lui	$6,%hi(proc_root)
	lui	$7,%hi(fastpath_pppoe_proc_fops)
	addiu	$7,$7,%lo(fastpath_pppoe_proc_fops)
	addiu	$4,$4,%lo($LC3)
	move	$5,$0
	addiu	$6,$6,%lo(proc_root)
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
	nop
	beq	$2,$0,$L31
	lui	$4,%hi($LC3)

	lui	$5,%hi(proc_root)
	addiu	$4,$4,%lo($LC3)
	jal	remove_proc_entry
	addiu	$5,$5,%lo(proc_root)

	sw	$0,%lo(fast_pppoe_proc)($16)
$L31:
	lw	$31,20($sp)
	lw	$16,16($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fast_pppoe_exit
	.size	fast_pppoe_exit, .-fast_pppoe_exit
	.section	.text.clear_pppoe_info,"ax",@progbits
	.align	2
	.globl	clear_pppoe_info
	.set	nomips16
	.ent	clear_pppoe_info
	.type	clear_pppoe_info, @function
clear_pppoe_info:
	.frame	$sp,64,$31		# vars= 8, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
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
	lw	$21,84($sp)
	lw	$20,88($sp)
	andi	$6,$6,0xffff
	lui	$19,%hi(fast_pppoe_info+54)
	lui	$16,%hi(fast_pppoe_info)
	move	$23,$4
	move	$22,$5
	sw	$6,16($sp)
	addiu	$19,$19,%lo(fast_pppoe_info+54)
	move	$17,$0
	li	$18,88			# 0x58
	addiu	$16,$16,%lo(fast_pppoe_info)
$L38:
	lhu	$2,0($19)
	#nop
	beq	$2,$0,$L33
	.set	noreorder
	.set	nomacro
	beq	$23,$0,$L34
	mult	$17,$18
	.set	macro
	.set	reorder

	move	$3,$23
	mflo	$4
	addiu	$2,$4,16
	addu	$2,$16,$2
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($2)
1:	lbu	$1,($3)
	addiu	$2,1
	bne	$1,$5,2f
	addiu	$3,1
	bnez	$5,1b
	lbu	$5,($2)
	nop
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L34
	addu	$4,$16,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

$L34:
	.set	noreorder
	.set	nomacro
	beq	$22,$0,$L35
	mult	$17,$18
	.set	macro
	.set	reorder

	move	$3,$22
	mflo	$4
	addu	$4,$16,$4
	move	$2,$4
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($2)
1:	lbu	$1,($3)
	addiu	$2,1
	bne	$1,$5,2f
	addiu	$3,1
	bnez	$5,1b
	lbu	$5,($2)
	nop
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	bne	$5,$0,$L35
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

$L35:
	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L36
	mult	$17,$18
	.set	macro
	.set	reorder

	move	$5,$20
	li	$6,6			# 0x6
	mflo	$fp
	addu	$4,$16,$fp
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$4,$4,38
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L36
	addu	$4,$16,$fp
	.set	macro
	.set	reorder

	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

$L36:
	.set	noreorder
	.set	nomacro
	beq	$21,$0,$L37
	mult	$17,$18
	.set	macro
	.set	reorder

	move	$5,$21
	li	$6,6			# 0x6
	mflo	$fp
	addiu	$4,$fp,32
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addu	$4,$16,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L37
	addu	$4,$16,$fp
	.set	macro
	.set	reorder

	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	memset
	li	$6,88			# 0x58
	.set	macro
	.set	reorder

$L37:
	lhu	$2,-2($19)
	lw	$3,16($sp)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L33
	mult	$17,$18
	.set	macro
	.set	reorder

	move	$5,$0
	li	$6,88			# 0x58
	mflo	$4
	.set	noreorder
	.set	nomacro
	jal	memset
	addu	$4,$16,$4
	.set	macro
	.set	reorder

$L33:
	addiu	$17,$17,1
	li	$2,16			# 0x10
	.set	noreorder
	.set	nomacro
	bne	$17,$2,$L38
	addiu	$19,$19,88
	.set	macro
	.set	reorder

	lw	$31,60($sp)
	lw	$fp,56($sp)
	lw	$23,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,64
	.set	macro
	.set	reorder

	.end	clear_pppoe_info
	.size	clear_pppoe_info, .-clear_pppoe_info
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
	move	$20,$4
	move	$21,$7
	sw	$18,16($sp)
	sw	$16,20($sp)
	.set	noreorder
	.set	nomacro
	jal	find_pppoe_info
	sw	$17,24($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L41
	move	$23,$2
	.set	macro
	.set	reorder

	lui	$3,%hi(fast_pppoe_info+54)
	addiu	$3,$3,%lo(fast_pppoe_info+54)
	move	$2,$0
	li	$4,16			# 0x10
$L44:
	lhu	$5,0($3)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L42
	addiu	$2,$2,1
	.set	macro
	.set	reorder

	addiu	$2,$2,-1
	li	$23,88			# 0x58
	mult	$2,$23
	lui	$23,%hi(fast_pppoe_info)
	addiu	$23,$23,%lo(fast_pppoe_info)
	mflo	$2
	addu	$23,$23,$2
	bne	$23,$0,$L41
	j	$L45
$L42:
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L44
	addiu	$3,$3,88
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
$L41:
	beq	$22,$0,$L46
	sh	$22,52($23)
$L46:
	beq	$21,$0,$L47
	sw	$21,44($23)
$L47:
	beq	$18,$0,$L48
	sw	$18,48($23)
$L48:
	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L49
	addiu	$2,$23,16
	.set	macro
	.set	reorder

 #APP
 # 30 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
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
$L49:
	.set	noreorder
	.set	nomacro
	beq	$19,$0,$L50
	move	$2,$23
	.set	macro
	.set	reorder

 #APP
 # 30 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
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
$L50:
	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L51
	addiu	$4,$23,38
	.set	macro
	.set	reorder

	move	$5,$17
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

$L51:
	.set	noreorder
	.set	nomacro
	beq	$16,$0,$L52
	addiu	$4,$23,32
	.set	macro
	.set	reorder

	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

$L52:
	li	$2,1			# 0x1
	sh	$2,54($23)
$L45:
	lw	$31,68($sp)
	lw	$23,64($sp)
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
	nop
	sw	$2,20($sp)
	lw	$2,64($sp)
	jal	find_pppoe_info
	sw	$2,24($sp)

	beq	$2,$0,$L55
	nop

	lw	$4,56($2)
	lw	$3,68($sp)
	nop
	sw	$4,0($3)
	lw	$3,60($2)
	lw	$2,72($sp)
	nop
	sw	$3,0($2)
$L55:
	lw	$31,36($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	get_pppoe_last_rx_tx
	.size	get_pppoe_last_rx_tx, .-get_pppoe_last_rx_tx
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
	move	$16,$4
	beq	$2,$0,$L57
	move	$18,$0

	jal	rtl_set_skb_pppoe_flag
	move	$5,$0

	jal	rtl_skb_mac_header
	move	$4,$16

	beq	$2,$0,$L57
	move	$17,$2

	lhu	$3,12($2)
	li	$2,34916			# 0x8864
	bne	$3,$2,$L69
	move	$2,$18

	lhu	$3,20($17)
	li	$2,33			# 0x21
	bne	$3,$2,$L69
	move	$2,$18

	lhu	$3,16($17)
	lui	$19,%hi(fast_pppoe_info+54)
	lui	$21,%hi(fast_pppoe_info)
	addiu	$fp,$17,6
	addiu	$19,$19,%lo(fast_pppoe_info+54)
	li	$23,88			# 0x58
	addiu	$21,$21,%lo(fast_pppoe_info)
	li	$22,16			# 0x10
$L60:
	lhu	$2,0($19)
	nop
	beq	$2,$0,$L58
	nop

	lhu	$2,-2($19)
	nop
	bne	$2,$3,$L58
	mult	$18,$23

	move	$5,$fp
	li	$6,6			# 0x6
	sw	$3,16($sp)
	mflo	$20
	addu	$4,$21,$20
	jal	memcmp
	addiu	$4,$4,38

	lw	$3,16($sp)
	bne	$2,$0,$L70
	addiu	$18,$18,1

	addu	$20,$21,$20
	bne	$20,$0,$L68
	move	$18,$0

	j	$L69
	move	$2,$18

$L58:
	addiu	$18,$18,1
$L70:
	bne	$18,$22,$L60
	addiu	$19,$19,88

	j	$L57
	move	$18,$0

$L68:
	jal	rtl_get_dev_by_name
	addiu	$4,$20,16

	beq	$2,$0,$L57
	move	$19,$2

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

	move	$5,$19
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
	lw	$2,%lo(jiffies)($2)
	li	$18,1			# 0x1
	sw	$2,56($20)
$L57:
	move	$2,$18
$L69:
	lw	$31,60($sp)
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
	nop
	beq	$2,$0,$L71
	move	$16,$4

	jal	rtl_get_skb_pppoe_flag
	nop

	beq	$2,$0,$L71
	move	$4,$16

	jal	rtl_skb_push
	li	$5,8			# 0x8

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

$L71:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	check_and_restore_pppoe_hdr
	.size	check_and_restore_pppoe_hdr, .-check_and_restore_pppoe_hdr
	.section	.rodata.str1.4
	.align	2
$LC4:
	.ascii	"ppp\000"
	.section	.text.fast_pppoe_xmit,"ax",@progbits
	.align	2
	.globl	fast_pppoe_xmit
	.set	nomips16
	.ent	fast_pppoe_xmit
	.type	fast_pppoe_xmit, @function
fast_pppoe_xmit:
	.frame	$sp,56,$31		# vars= 0, regs= 6/0, args= 32, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$20,48($sp)
	sw	$19,44($sp)
	sw	$18,40($sp)
	sw	$17,36($sp)
	sw	$16,32($sp)
	lui	$2,%hi(fast_pppoe_fw)
	lw	$2,%lo(fast_pppoe_fw)($2)
	move	$16,$4
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L74
	move	$17,$0
	.set	macro
	.set	reorder

	lui	$2,%hi(gQosEnabled)
	lw	$2,%lo(gQosEnabled)($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L87
	move	$2,$17
	.set	macro
	.set	reorder

	jal	rtl_get_skb_dev
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L87
	move	$2,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_dev
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$3,%hi($LC4)
	addiu	$3,$3,%lo($LC4)
	li	$4,3			# 0x3
 #APP
 # 111 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
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
	nop
	move	$5,$1
3:	subu	$5,$1
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L87
	move	$2,$17
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
	bne	$2,$0,$L87
	move	$2,$17
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
	bne	$2,$0,$L87
	move	$2,$17
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
	bne	$2,$0,$L87
	move	$2,$17
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
	beq	$2,$0,$L74
	move	$18,$2
	.set	macro
	.set	reorder

	lb	$2,16($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L87
	move	$2,$17
	.set	macro
	.set	reorder

	lb	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L87
	move	$2,$17
	.set	macro
	.set	reorder

	lhu	$2,52($18)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L87
	move	$2,$17
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

	move	$4,$18
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	move	$20,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L74
	move	$19,$2
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

	li	$3,17			# 0x11
	sb	$3,0($2)
	sb	$0,1($2)
	lhu	$3,52($18)
	addiu	$20,$20,2
	andi	$20,$20,0xffff
	srl	$4,$3,8
	sb	$3,3($2)
	srl	$3,$20,8
	sb	$3,4($2)
	sb	$4,2($2)
	sb	$20,5($2)
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dev
	move	$5,$19
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
	addiu	$5,$18,38
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

	lw	$5,392($19)
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
	lw	$2,%lo(jiffies)($2)
	li	$17,1			# 0x1
	sw	$2,60($18)
$L74:
	move	$2,$17
$L87:
	lw	$31,52($sp)
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,56
	.set	macro
	.set	reorder

	.end	fast_pppoe_xmit
	.size	fast_pppoe_xmit, .-fast_pppoe_xmit
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
	.globl	fast_pppoe_proc
	.section	.bss.fast_pppoe_proc,"aw",@nobits
	.align	2
	.type	fast_pppoe_proc, @object
	.size	fast_pppoe_proc, 4
fast_pppoe_proc:
	.space	4
	.globl	fast_pppoe_info
	.section	.bss.fast_pppoe_info,"aw",@nobits
	.align	2
	.type	fast_pppoe_info, @object
	.size	fast_pppoe_info, 1408
fast_pppoe_info:
	.space	1408
	.globl	fast_pppoe_fw
	.section	.bss.fast_pppoe_fw,"aw",@nobits
	.align	2
	.type	fast_pppoe_fw, @object
	.size	fast_pppoe_fw, 4
fast_pppoe_fw:
	.space	4
	.ident	"GCC: (Realtek RSDK-4.6.4 Build 2080) 4.6.4"
