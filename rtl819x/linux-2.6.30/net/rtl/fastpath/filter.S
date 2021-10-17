	.file	1 "filter.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.str2hexnum,"ax",@progbits
	.align	2
	.globl	str2hexnum
	.set	nomips16
	.ent	str2hexnum
	.type	str2hexnum, @function
str2hexnum:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	j	$L2
	move	$2,$0

$L7:
	bne	$6,$0,$L2
	addu	$2,$5,$8

	addiu	$2,$3,-65
	sltu	$2,$2,6
	bne	$7,$0,$L3
	addiu	$5,$3,-87

	bne	$2,$0,$L5
	addiu	$5,$3,-55

	j	$31
	li	$2,-1			# 0xffffffffffffffff

$L5:
$L3:
	addu	$2,$5,$8
$L2:
	lbu	$3,0($4)
	sll	$8,$2,4
	addiu	$4,$4,1
	addiu	$5,$3,-48
	addiu	$7,$3,-97
	sltu	$7,$7,6
	bne	$3,$0,$L7
	sltu	$6,$5,10

	j	$31
	nop

	.set	macro
	.set	reorder
	.end	str2hexnum
	.size	str2hexnum, .-str2hexnum
	.section	.text.scan_table,"ax",@progbits
	.align	2
	.globl	scan_table
	.set	nomips16
	.ent	scan_table
	.type	scan_table, @function
scan_table:
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
	lui	$2,%hi(url_filter_flag)
	move	$17,$6
	sw	$0,%lo(url_filter_flag)($2)
	move	$18,$4
	move	$22,$5
	beq	$6,$0,$L11
	move	$21,$7

	lw	$2,28($6)
	beq	$2,$0,$L19
	li	$2,-1			# 0xffffffffffffffff

	lw	$2,24($6)
	sltu	$2,$2,2
	bne	$2,$0,$L12
	addiu	$20,$6,8

	lw	$16,8($6)
	beq	$16,$20,$L30
	lui	$2,%hi(url_filter_mode)

	lw	$2,16($16)
$L32:
	andi	$3,$2,0x1
	bne	$3,$0,$L13
	move	$19,$16

	lw	$3,20($16)
	bnel	$22,$3,$L29
	lw	$16,0($16)

$L13:
	andi	$3,$2,0x200
	beql	$3,$0,$L15
	andi	$2,$2,0x100

	andi	$2,$2,0x2
	beq	$2,$0,$L14
	lw	$6,0($21)

	beq	$6,$0,$L11
	move	$4,$18

	lw	$5,20($16)
	jal	scan_table
	move	$7,$6

	bne	$2,$0,$L19
	li	$2,1			# 0x1

	j	$L29
	lw	$16,0($16)

$L15:
	bnel	$2,$0,$L31
	lw	$2,16($16)

	lw	$2,28($17)
	move	$4,$18
	jalr	$2
	addiu	$5,$16,28

	beql	$2,$0,$L29
	lw	$16,0($16)

	lw	$2,16($16)
$L31:
	andi	$3,$2,0x2
	bnel	$3,$0,$L18
	lw	$6,0($21)

	andi	$3,$2,0x1
	beql	$3,$0,$L19
	li	$2,1			# 0x1

	xori	$2,$2,0x8
	j	$L19
	ext	$2,$2,3,1

$L18:
	beq	$6,$0,$L11
	move	$4,$18

	lw	$5,20($16)
	jal	scan_table
	move	$7,$6

	beql	$2,$0,$L29
	lw	$16,0($16)

	lw	$2,16($16)
	andi	$4,$2,0x1
	beq	$4,$0,$L20
	lui	$3,%hi(url_filter_mode)

	lw	$4,%lo(url_filter_mode)($3)
	srl	$3,$2,3
	bne	$4,$0,$L19
	andi	$2,$3,0x1

	xori	$2,$3,0x1
	j	$L19
	andi	$2,$2,0x1

$L20:
	lw	$2,%lo(url_filter_mode)($3)
	j	$L19
	sltu	$2,$2,1

$L14:
	lw	$16,0($16)
$L29:
	bnel	$16,$20,$L32
	lw	$2,16($16)

$L23 = .
	bne	$19,$0,$L24
	lui	$2,%hi(url_filter_flag)

$L12:
	lui	$2,%hi(url_filter_mode)
$L30:
	lw	$2,%lo(url_filter_mode)($2)
	bnel	$2,$0,$L19
	li	$2,1			# 0x1

	j	$L19
	li	$2,-1			# 0xffffffffffffffff

$L24:
	lw	$2,%lo(url_filter_flag)($2)
	bnel	$2,$0,$L16
	li	$2,1			# 0x1

	lw	$3,16($19)
	andi	$2,$3,0x1
	ext	$4,$3,3,1
	movz	$4,$0,$2
	j	$L19
	move	$2,$4

$L11:
	li	$2,-1			# 0xffffffffffffffff
$L16:
$L19:
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
	.end	scan_table
	.size	scan_table, .-scan_table
	.section	.text.strtoint,"ax",@progbits
	.align	2
	.globl	strtoint
	.set	nomips16
	.ent	strtoint
	.type	strtoint, @function
strtoint:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	move	$2,$0
	j	$L34
	li	$6,10			# 0xa

$L36:
	beq	$7,$0,$L38
	nop

	mul	$3,$2,$6
	addu	$2,$3,$5
$L34:
	lb	$3,0($4)
	addiu	$4,$4,1
	addiu	$5,$3,-48
	bne	$3,$0,$L36
	sltu	$7,$5,10

$L38:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	strtoint
	.size	strtoint, .-strtoint
	.section	.text.filter_addconnect,"ax",@progbits
	.align	2
	.globl	filter_addconnect
	.set	nomips16
	.ent	filter_addconnect
	.type	filter_addconnect, @function
filter_addconnect:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	andi	$3,$4,0xff
	lui	$2,%hi(ConnectedIp)
	sll	$3,$3,2
	addiu	$2,$2,%lo(ConnectedIp)
	addu	$2,$3,$2
	j	$31
	sw	$4,0($2)

	.set	macro
	.set	reorder
	.end	filter_addconnect
	.size	filter_addconnect, .-filter_addconnect
	.section	.text.filter_delconnect,"ax",@progbits
	.align	2
	.globl	filter_delconnect
	.set	nomips16
	.ent	filter_delconnect
	.type	filter_delconnect, @function
filter_delconnect:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	andi	$4,$4,0x00ff
	lui	$2,%hi(ConnectedIp)
	sll	$4,$4,2
	addiu	$2,$2,%lo(ConnectedIp)
	addu	$4,$4,$2
	j	$31
	sw	$0,0($4)

	.set	macro
	.set	reorder
	.end	filter_delconnect
	.size	filter_delconnect, .-filter_delconnect
	.section	.text.filter_checkConnect,"ax",@progbits
	.align	2
	.globl	filter_checkConnect
	.set	nomips16
	.ent	filter_checkConnect
	.type	filter_checkConnect, @function
filter_checkConnect:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	andi	$3,$4,0xff
	lui	$2,%hi(ConnectedIp)
	sll	$3,$3,2
	addiu	$2,$2,%lo(ConnectedIp)
	addu	$2,$3,$2
	lw	$3,0($2)
	beq	$3,$0,$L47
	li	$2,1			# 0x1

	xor	$2,$3,$4
	sltu	$2,$0,$2
$L47:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	filter_checkConnect
	.size	filter_checkConnect, .-filter_checkConnect
	.section	.text.register_customRspHook,"ax",@progbits
	.align	2
	.globl	register_customRspHook
	.set	nomips16
	.ent	register_customRspHook
	.type	register_customRspHook, @function
register_customRspHook:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	beq	$4,$0,$L49
	lui	$2,%hi(fast_path_cusRsp401_func)

	j	$L50
	sw	$4,%lo(fast_path_cusRsp401_func)($2)

$L49:
	lui	$3,%hi(GenerateHTTP401)
	addiu	$3,$3,%lo(GenerateHTTP401)
	sw	$3,%lo(fast_path_cusRsp401_func)($2)
$L50:
	beq	$5,$0,$L51
	lui	$2,%hi(fast_path_cusRspTCPFinAck_func)

	j	$L52
	sw	$5,%lo(fast_path_cusRspTCPFinAck_func)($2)

$L51:
	lui	$3,%hi(GenerateTCPFINACK)
	addiu	$3,$3,%lo(GenerateTCPFINACK)
	sw	$3,%lo(fast_path_cusRspTCPFinAck_func)($2)
$L52:
	beq	$6,$0,$L53
	lui	$2,%hi(fast_path_cusRspTCPEndAck_func)

	j	$31
	sw	$6,%lo(fast_path_cusRspTCPEndAck_func)($2)

$L53:
	lui	$3,%hi(GenerateTCPENDACK)
	addiu	$3,$3,%lo(GenerateTCPENDACK)
	j	$31
	sw	$3,%lo(fast_path_cusRspTCPEndAck_func)($2)

	.set	macro
	.set	reorder
	.end	register_customRspHook
	.size	register_customRspHook, .-register_customRspHook
	.section	.text.unregister_customRspHook,"ax",@progbits
	.align	2
	.globl	unregister_customRspHook
	.set	nomips16
	.ent	unregister_customRspHook
	.type	unregister_customRspHook, @function
unregister_customRspHook:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(fast_path_cusRsp401_func)
	sw	$0,%lo(fast_path_cusRsp401_func)($2)
	lui	$2,%hi(fast_path_cusRspTCPFinAck_func)
	sw	$0,%lo(fast_path_cusRspTCPFinAck_func)($2)
	lui	$2,%hi(fast_path_cusRspTCPEndAck_func)
	j	$31
	sw	$0,%lo(fast_path_cusRspTCPEndAck_func)($2)

	.set	macro
	.set	reorder
	.end	unregister_customRspHook
	.size	unregister_customRspHook, .-unregister_customRspHook
	.section	.text.dos_single_open,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	dos_single_open
	.type	dos_single_open, @function
dos_single_open:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	move	$4,$5
	lui	$5,%hi(dos_read_proc)
	addiu	$5,$5,%lo(dos_read_proc)
	move	$6,$0
	j	single_open
	nop

	.set	macro
	.set	reorder
	.end	dos_single_open
	.size	dos_single_open, .-dos_single_open
	.section	.text.filter_table_single_open,"ax",@progbits
	.align	2
	.globl	filter_table_single_open
	.set	nomips16
	.ent	filter_table_single_open
	.type	filter_table_single_open, @function
filter_table_single_open:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	move	$4,$5
	lui	$5,%hi(filter_table_read_proc)
	addiu	$5,$5,%lo(filter_table_read_proc)
	move	$6,$0
	j	single_open
	nop

	.set	macro
	.set	reorder
	.end	filter_table_single_open
	.size	filter_table_single_open, .-filter_table_single_open
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"%s\012\000"
	.section	.text.dos_read_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	dos_read_proc
	.type	dos_read_proc, @function
dos_read_proc:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$5,%hi($LC0)
	lui	$6,%hi(dos_flag)
	addiu	$5,$5,%lo($LC0)
	jal	seq_printf
	addiu	$6,$6,%lo(dos_flag)

	lw	$31,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	dos_read_proc
	.size	dos_read_proc, .-dos_read_proc
	.section	.text.filter_table_head_init,"ax",@progbits
	.align	2
	.globl	filter_table_head_init
	.set	nomips16
	.ent	filter_table_head_init
	.type	filter_table_head_init, @function
filter_table_head_init:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$16,%hi(table_list_head)
	addiu	$4,$16,%lo(table_list_head)
	move	$5,$0
	jal	memset
	li	$6,32			# 0x20

	lui	$3,%hi(table_list_head+8)
	addiu	$2,$16,%lo(table_list_head)
	addiu	$3,$3,%lo(table_list_head+8)
	li	$4,1			# 0x1
	sw	$4,24($2)
	sw	$3,12($2)
	sw	$2,4($2)
	sw	$3,8($2)
	sw	$0,16($2)
	sw	$0,20($2)
	sw	$0,28($2)
	sw	$2,%lo(table_list_head)($16)
	lw	$31,20($sp)
	lw	$16,16($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_table_head_init
	.size	filter_table_head_init, .-filter_table_head_init
	.section	.text.check_esc_end,"ax",@progbits
	.align	2
	.globl	check_esc_end
	.set	nomips16
	.ent	check_esc_end
	.type	check_esc_end, @function
check_esc_end:
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
	jal	strlen
	seb	$17,$5

	addu	$16,$16,$2
	move	$3,$0
	j	$L67
	addiu	$2,$2,-1

$L69:
	addiu	$2,$2,-1
	bne	$4,$17,$L68
	addiu	$16,$16,-1

	addiu	$3,$3,1
$L67:
	bgezl	$2,$L69
	lb	$4,-1($16)

$L68:
	li	$2,2			# 0x2
	div	$0,$3,$2
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	mfhi	$2
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	check_esc_end
	.size	check_esc_end, .-check_esc_end
	.section	.text.unregister_customRspStr,"ax",@progbits
	.align	2
	.globl	unregister_customRspStr
	.set	nomips16
	.ent	unregister_customRspStr
	.type	unregister_customRspStr, @function
unregister_customRspStr:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$16,%hi(block_message)
	lw	$4,%lo(block_message)($16)
	beq	$4,$0,$L74
	lw	$31,20($sp)

	jal	kfree
	nop

	sw	$0,%lo(block_message)($16)
	lw	$31,20($sp)
$L74:
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	unregister_customRspStr
	.size	unregister_customRspStr, .-unregister_customRspStr
	.section	.text.filter_table_regist,"ax",@progbits
	.align	2
	.globl	filter_table_regist
	.set	nomips16
	.ent	filter_table_regist
	.type	filter_table_regist, @function
filter_table_regist:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$2,%hi(table_list_head+24)
	lw	$2,%lo(table_list_head+24)($2)
	move	$17,$4
	bne	$2,$0,$L76
	move	$16,$5

	jal	filter_table_head_init
	nop

$L76:
	lui	$2,%hi(kmalloc_caches+20)
	lw	$4,%lo(kmalloc_caches+20)($2)
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0

	move	$3,$2
	li	$4,1			# 0x1
	addiu	$2,$2,8
	sw	$4,24($3)
	sw	$2,12($3)
	sw	$3,0($3)
	sw	$3,4($3)
	sw	$2,8($3)
	sw	$17,16($3)
	sw	$16,28($3)
	sw	$0,20($3)
	lui	$5,%hi(table_list_head)
	lw	$6,%lo(table_list_head)($5)
	addiu	$4,$5,%lo(table_list_head)
	move	$2,$0
	sw	$3,4($6)
	sw	$6,0($3)
	sw	$4,4($3)
	lw	$6,24($4)
	sw	$3,%lo(table_list_head)($5)
	addiu	$3,$6,1
	sw	$3,24($4)
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	filter_table_regist
	.size	filter_table_regist, .-filter_table_regist
	.section	.text.rtl_schedule_filter_fastpath,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	rtl_schedule_filter_fastpath
	.type	rtl_schedule_filter_fastpath, @function
rtl_schedule_filter_fastpath:
	.frame	$sp,40,$31		# vars= 16, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	addiu	$4,$sp,16
	jal	do_gettimeofday
	sw	$5,24($sp)

	lw	$3,16($sp)
	li	$2,65536			# 0x10000
	ori	$2,$2,0x5180
	div	$0,$3,$2
	li	$6,7			# 0x7
	lw	$5,24($sp)
	lw	$2,8($5)
	mflo	$4
	addiu	$4,$4,4
	div	$0,$4,$6
	li	$6,1			# 0x1
	mfhi	$4
	sll	$4,$6,$4
	ori	$4,$4,0x80
	and	$2,$4,$2
	beq	$2,$0,$L82
	move	$2,$0

	li	$2,60			# 0x3c
	div	$0,$3,$2
	li	$8,3600			# 0xe10
	li	$7,24			# 0x18
	lw	$4,0($5)
	mflo	$6
	div	$0,$3,$8
	mflo	$3
	div	$0,$3,$7
	mfhi	$7
	div	$0,$6,$2
	mfhi	$9
	mul	$3,$7,$2
	bne	$4,$0,$L80
	addu	$7,$3,$9

	lw	$3,4($5)
	bne	$3,$0,$L81
	li	$2,1			# 0x1

	j	$L84
	lw	$31,36($sp)

$L80:
	sltu	$4,$7,$4
	bnel	$4,$0,$L79
	move	$2,$0

$L81:
	lw	$2,4($5)
	sltu	$7,$2,$7
	xori	$2,$7,0x1
$L79:
$L82:
	lw	$31,36($sp)
$L84:
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	rtl_schedule_filter_fastpath
	.size	rtl_schedule_filter_fastpath, .-rtl_schedule_filter_fastpath
	.section	.text.rtl_ip_range_filter_fastpath,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	rtl_ip_range_filter_fastpath
	.type	rtl_ip_range_filter_fastpath, @function
rtl_ip_range_filter_fastpath:
	.frame	$sp,32,$31		# vars= 8, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	jal	rtl_ip_hdr
	sw	$5,16($sp)

	lw	$5,16($sp)
	lw	$3,12($2)
	lw	$4,0($5)
	sltu	$4,$3,$4
	bne	$4,$0,$L87
	move	$2,$0

	lw	$2,4($5)
	sltu	$2,$2,$3
	xori	$2,$2,0x1
$L87:
	lw	$31,28($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl_ip_range_filter_fastpath
	.size	rtl_ip_range_filter_fastpath, .-rtl_ip_range_filter_fastpath
	.section	.rodata.str1.4
	.align	2
$LC1:
	.ascii	"filter_table\000"
	.section	.text.filter_exit_fastpath,"ax",@progbits
	.align	2
	.globl	filter_exit_fastpath
	.set	nomips16
	.ent	filter_exit_fastpath
	.type	filter_exit_fastpath, @function
filter_exit_fastpath:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$16,%hi(res12)
	lw	$2,%lo(res12)($16)
	beq	$2,$0,$L92
	lui	$4,%hi($LC1)

	lui	$5,%hi(proc_root)
	addiu	$4,$4,%lo($LC1)
	jal	remove_proc_entry
	addiu	$5,$5,%lo(proc_root)

	sw	$0,%lo(res12)($16)
$L92:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_exit_fastpath
	.size	filter_exit_fastpath, .-filter_exit_fastpath
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	"enable_dos\000"
	.section	.exit.text,"ax",@progbits
	.align	2
	.globl	filter_exit
	.set	nomips16
	.ent	filter_exit
	.type	filter_exit, @function
filter_exit:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$16,%hi(res3)
	lw	$5,%lo(res3)($16)
	beql	$5,$0,$L96
	lui	$4,%hi(dos_timer)

	lui	$4,%hi($LC2)
	jal	remove_proc_entry
	addiu	$4,$4,%lo($LC2)

	sw	$0,%lo(res3)($16)
	lui	$4,%hi(dos_timer)
$L96:
	jal	del_timer_sync
	addiu	$4,$4,%lo(dos_timer)

	lw	$31,20($sp)
	lw	$16,16($sp)
	j	filter_exit_fastpath
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_exit
	.size	filter_exit, .-filter_exit
	.section	.text.filter_table_init,"ax",@progbits
	.align	2
	.globl	filter_table_init
	.set	nomips16
	.ent	filter_table_init
	.type	filter_table_init, @function
filter_table_init:
	.frame	$sp,32,$31		# vars= 0, regs= 1/0, args= 24, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	jal	filter_table_head_init
	nop

	lui	$4,%hi($LC1)
	lui	$6,%hi(proc_root)
	lui	$7,%hi(filter_table_proc_fops)
	addiu	$4,$4,%lo($LC1)
	addiu	$6,$6,%lo(proc_root)
	addiu	$7,$7,%lo(filter_table_proc_fops)
	move	$5,$0
	jal	proc_create_data
	sw	$0,16($sp)

	lw	$31,28($sp)
	lui	$3,%hi(res12)
	sw	$2,%lo(res12)($3)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	filter_table_init
	.size	filter_table_init, .-filter_table_init
	.section	.text.filter_init_fastpath,"ax",@progbits
	.align	2
	.globl	filter_init_fastpath
	.set	nomips16
	.ent	filter_init_fastpath
	.type	filter_init_fastpath, @function
filter_init_fastpath:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	li	$3,1			# 0x1
	lui	$2,%hi(filter_rule_num)
	jal	filter_table_init
	sw	$3,%lo(filter_rule_num)($2)

	lw	$31,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_init_fastpath
	.size	filter_init_fastpath, .-filter_init_fastpath
	.section	.init.text,"ax",@progbits
	.align	2
	.globl	filter_init
	.set	nomips16
	.ent	filter_init
	.type	filter_init, @function
filter_init:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 24, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$17,32($sp)
	sw	$16,28($sp)
	lui	$2,%hi(item)
	sw	$0,%lo(item)($2)
	lui	$4,%hi($LC2)
	lui	$2,%hi(none_dos_drop_pkt_cnt)
	lui	$6,%hi(proc_root)
	lui	$7,%hi(dos_proc_fops)
	addiu	$4,$4,%lo($LC2)
	addiu	$6,$6,%lo(proc_root)
	addiu	$7,$7,%lo(dos_proc_fops)
	move	$5,$0
	lui	$16,%hi(dos_timer)
	sw	$0,%lo(none_dos_drop_pkt_cnt)($2)
	lui	$2,%hi(url_count)
	sw	$0,16($sp)
	jal	proc_create_data
	sb	$0,%lo(url_count)($2)

	move	$7,$0
	move	$6,$0
	addiu	$4,$16,%lo(dos_timer)
	lui	$3,%hi(res3)
	move	$5,$0
	jal	init_timer_key
	sw	$2,%lo(res3)($3)

	lui	$3,%hi(jiffies)
	lw	$2,%lo(jiffies)($3)
	lw	$5,%lo(jiffies)($3)
	addiu	$16,$16,%lo(dos_timer)
	addiu	$2,$2,100
	sw	$2,8($16)
	lui	$2,%hi(dos_timer_fn)
	lui	$17,%hi(_tcpDosScanBitmap)
	move	$4,$16
	addiu	$5,$5,100
	addiu	$2,$2,%lo(dos_timer_fn)
	sw	$2,16($16)
	jal	mod_timer
	sw	$0,20($16)

	addiu	$4,$17,%lo(_tcpDosScanBitmap)
	move	$5,$0
	jal	memset
	li	$6,64			# 0x40

	addiu	$3,$17,%lo(_tcpDosScanBitmap)
	li	$2,1			# 0x1
	sb	$2,3($3)
	sb	$2,63($3)
	sb	$2,58($3)
	sb	$2,41($3)
	sb	$2,40($3)
	sb	$2,33($3)
	sb	$2,32($3)
	sb	$2,9($3)
	sb	$2,8($3)
	jal	filter_init_fastpath
	sb	$2,%lo(_tcpDosScanBitmap)($17)

	jal	igmp_delete_init_netlink
	nop

	lw	$31,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	filter_init
	.size	filter_init, .-filter_init
	.section	.rodata.str1.4
	.align	2
$LC3:
	.ascii	"DoS: Whole System SYN Flood Attack\012\000"
	.align	2
$LC4:
	.ascii	"DoS: Whole System FIN Flood Attack\012\000"
	.align	2
$LC5:
	.ascii	"DoS: Whole System UDP Flood Attack\012\000"
	.align	2
$LC6:
	.ascii	"DoS: Whole System ICMP Flood Attack\012\000"
	.align	2
$LC7:
	.ascii	"DoS: Per-source SYN Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC8:
	.ascii	"DoS: Per-source FIN Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC9:
	.ascii	"DoS: Per-source UDP Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC10:
	.ascii	"DoS: Per-source ICMP Flood Attack source=%u.%u.%u.%u des"
	.ascii	"tination=%u.%u.%u.%u\012\000"
	.align	2
$LC11:
	.ascii	"DoS: Port Scan Attack source=%u.%u.%u.%u destination=%u."
	.ascii	"%u.%u.%u\012\000"
	.align	2
$LC12:
	.ascii	"DoS: Tcp Scan Attack source=%u.%u.%u.%u destination=%u.%"
	.ascii	"u.%u.%u\012\000"
	.align	2
$LC13:
	.ascii	"DoS: Tcp SYN With Data Attack source=%u.%u.%u.%u destina"
	.ascii	"tion=%u.%u.%u.%u\012\000"
	.align	2
$LC14:
	.ascii	"DoS: IP Land Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC15:
	.ascii	"DoS: UdpEchoChargen Attack source=%u.%u.%u.%u destinatio"
	.ascii	"n=%u.%u.%u.%u\012\000"
	.align	2
$LC16:
	.ascii	"DoS: UdpBomb Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC17:
	.ascii	"DoS: PingOfDeath Attack source=%u.%u.%u.%u destination=%"
	.ascii	"u.%u.%u.%u\012\000"
	.align	2
$LC18:
	.ascii	"DoS: IcmpSmurf Attack source=%u.%u.%u.%u destination=%u."
	.ascii	"%u.%u.%u\012\000"
	.align	2
$LC19:
	.ascii	"DoS: IpSpoof Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC20:
	.ascii	"DoS: TearDrop Attack source=%u.%u.%u.%u destination=%u.%"
	.ascii	"u.%u.%u\012\000"
	.section	.text.dos_timer_fn,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	dos_timer_fn
	.type	dos_timer_fn, @function
dos_timer_fn:
	.frame	$sp,48,$31		# vars= 0, regs= 2/0, args= 40, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$16,40($sp)
	lui	$2,%hi(item)
	lw	$2,%lo(item)($2)
	beql	$2,$0,$L145
	lui	$2,%hi(none_dos_drop_pkt_cnt)

	lui	$3,%hi(block_count)
	lui	$4,%hi(block_time)
	lw	$4,%lo(block_time)($4)
	lw	$5,%lo(block_count)($3)
	slt	$4,$5,$4
	bnel	$4,$0,$L146
	lui	$3,%hi(block)

	lui	$4,%hi(block)
	sw	$0,%lo(block)($4)
	sw	$0,%lo(block_count)($3)
	lui	$3,%hi(block)
$L146:
	lw	$4,%lo(block)($3)
	li	$3,1			# 0x1
	bne	$4,$3,$L147
	andi	$3,$2,0x1e

	lui	$3,%hi(block_count)
	lw	$4,%lo(block_count)($3)
	addiu	$4,$4,1
	sw	$4,%lo(block_count)($3)
	andi	$3,$2,0x1e
$L147:
	beql	$3,$0,$L148
	lui	$2,%hi(LogFlag)

	lui	$13,%hi(dos_pkt+36828)
	lui	$3,%hi(dos_pkt)
	addiu	$13,$13,%lo(dos_pkt+36828)
	addiu	$3,$3,%lo(dos_pkt)
	move	$24,$0
	move	$4,$0
	move	$5,$0
	move	$6,$0
	li	$12,1			# 0x1
	andi	$11,$2,0x2
	andi	$10,$2,0x4
	andi	$9,$2,0x8
	andi	$8,$2,0x10
$L113:
	lb	$7,0($3)
	bne	$7,$12,$L108
	nop

	beq	$11,$0,$L109
	nop

	lw	$7,4($3)
	slt	$14,$0,$7
	mul	$15,$14,$7
	j	$L109
	addu	$6,$15,$6

$L140:
	slt	$14,$0,$7
	mul	$15,$14,$7
	j	$L110
	addu	$5,$15,$5

$L141:
	slt	$14,$0,$7
	mul	$15,$14,$7
	j	$L111
	addu	$4,$15,$4

$L142:
	slt	$14,$0,$7
	mul	$15,$14,$7
	addu	$24,$15,$24
$L108:
	bne	$3,$13,$L113
	addiu	$3,$3,36

	lui	$3,%hi(whole_syn_threshold)
	lw	$3,%lo(whole_syn_threshold)($3)
	slt	$6,$3,$6
	beql	$6,$0,$L149
	lui	$3,%hi(whole_fin_threshold)

	andi	$6,$2,0x2
	beql	$6,$0,$L149
	lui	$3,%hi(whole_fin_threshold)

	blez	$3,$L114
	lui	$3,%hi(LogFlag)

	lw	$6,%lo(LogFlag)($3)
	ori	$6,$6,0x2
	sw	$6,%lo(LogFlag)($3)
$L114:
	lui	$3,%hi(whole_fin_threshold)
$L149:
	lw	$3,%lo(whole_fin_threshold)($3)
	slt	$5,$3,$5
	beql	$5,$0,$L150
	lui	$3,%hi(whole_udp_threshold)

	andi	$5,$2,0x4
	beql	$5,$0,$L150
	lui	$3,%hi(whole_udp_threshold)

	blez	$3,$L115
	lui	$3,%hi(LogFlag)

	lw	$5,%lo(LogFlag)($3)
	ori	$5,$5,0x4
	sw	$5,%lo(LogFlag)($3)
$L115:
	lui	$3,%hi(whole_udp_threshold)
$L150:
	lw	$3,%lo(whole_udp_threshold)($3)
	slt	$4,$3,$4
	beql	$4,$0,$L151
	lui	$3,%hi(whole_icmp_threshold)

	andi	$4,$2,0x8
	beql	$4,$0,$L151
	lui	$3,%hi(whole_icmp_threshold)

	blez	$3,$L116
	lui	$3,%hi(LogFlag)

	lw	$4,%lo(LogFlag)($3)
	ori	$4,$4,0x8
	sw	$4,%lo(LogFlag)($3)
$L116:
	lui	$3,%hi(whole_icmp_threshold)
$L151:
	lw	$3,%lo(whole_icmp_threshold)($3)
	slt	$4,$3,$24
	beql	$4,$0,$L148
	lui	$2,%hi(LogFlag)

	andi	$2,$2,0x10
	beq	$2,$0,$L148
	lui	$2,%hi(LogFlag)

	blez	$3,$L148
	lw	$3,%lo(LogFlag)($2)

	ori	$3,$3,0x10
	sw	$3,%lo(LogFlag)($2)
	lui	$2,%hi(LogFlag)
$L148:
	lw	$16,%lo(LogFlag)($2)
	andi	$2,$16,0x2
	beq	$2,$0,$L152
	andi	$2,$16,0x4

	lui	$4,%hi($LC3)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC3)

	andi	$2,$16,0x4
$L152:
	beq	$2,$0,$L153
	andi	$2,$16,0x8

	lui	$4,%hi($LC4)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC4)

	andi	$2,$16,0x8
$L153:
	beq	$2,$0,$L154
	andi	$2,$16,0x10

	lui	$4,%hi($LC5)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC5)

	andi	$2,$16,0x10
$L154:
	beq	$2,$0,$L155
	andi	$2,$16,0x20

	lui	$4,%hi($LC6)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC6)

	andi	$2,$16,0x20
$L155:
	beq	$2,$0,$L156
	andi	$2,$16,0x40

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,5($2)
	lbu	$7,6($2)
	lbu	$10,7($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+4)
	lui	$2,%hi(attack_daddr+4)
	lbu	$8,5($3)
	lbu	$9,%lo(attack_daddr+4)($2)
	lbu	$5,%lo(attack_saddr+4)($4)
	lbu	$2,7($3)
	lbu	$3,6($3)
	lui	$4,%hi($LC7)
	addiu	$4,$4,%lo($LC7)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x40
$L156:
	beq	$2,$0,$L157
	andi	$2,$16,0x80

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,9($2)
	lbu	$7,10($2)
	lbu	$10,11($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+8)
	lui	$2,%hi(attack_daddr+8)
	lbu	$8,9($3)
	lbu	$9,%lo(attack_daddr+8)($2)
	lbu	$5,%lo(attack_saddr+8)($4)
	lbu	$2,11($3)
	lbu	$3,10($3)
	lui	$4,%hi($LC8)
	addiu	$4,$4,%lo($LC8)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x80
$L157:
	beq	$2,$0,$L158
	andi	$2,$16,0x100

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,13($2)
	lbu	$7,14($2)
	lbu	$10,15($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+12)
	lui	$2,%hi(attack_daddr+12)
	lbu	$8,13($3)
	lbu	$9,%lo(attack_daddr+12)($2)
	lbu	$5,%lo(attack_saddr+12)($4)
	lbu	$2,15($3)
	lbu	$3,14($3)
	lui	$4,%hi($LC9)
	addiu	$4,$4,%lo($LC9)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x100
$L158:
	beq	$2,$0,$L159
	andi	$2,$16,0x200

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,17($2)
	lbu	$7,18($2)
	lbu	$10,19($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+16)
	lui	$2,%hi(attack_daddr+16)
	lbu	$8,17($3)
	lbu	$9,%lo(attack_daddr+16)($2)
	lbu	$5,%lo(attack_saddr+16)($4)
	lbu	$2,19($3)
	lbu	$3,18($3)
	lui	$4,%hi($LC10)
	addiu	$4,$4,%lo($LC10)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x200
$L159:
	beq	$2,$0,$L160
	andi	$2,$16,0x8000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,21($2)
	lbu	$7,22($2)
	lbu	$10,23($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+20)
	lui	$2,%hi(attack_daddr+20)
	lbu	$8,21($3)
	lbu	$9,%lo(attack_daddr+20)($2)
	lbu	$5,%lo(attack_saddr+20)($4)
	lbu	$2,23($3)
	lbu	$3,22($3)
	lui	$4,%hi($LC11)
	addiu	$4,$4,%lo($LC11)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x8000
$L160:
	beq	$2,$0,$L161
	ext	$2,$16,16,1

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,45($2)
	lbu	$7,46($2)
	lbu	$10,47($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+44)
	lui	$2,%hi(attack_daddr+44)
	lbu	$8,45($3)
	lbu	$9,%lo(attack_daddr+44)($2)
	lbu	$5,%lo(attack_saddr+44)($4)
	lbu	$2,47($3)
	lbu	$3,46($3)
	lui	$4,%hi($LC12)
	addiu	$4,$4,%lo($LC12)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	ext	$2,$16,16,1
$L161:
	beq	$2,$0,$L162
	andi	$2,$16,0x800

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,49($2)
	lbu	$7,50($2)
	lbu	$10,51($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+48)
	lui	$2,%hi(attack_daddr+48)
	lbu	$8,49($3)
	lbu	$9,%lo(attack_daddr+48)($2)
	lbu	$5,%lo(attack_saddr+48)($4)
	lbu	$2,51($3)
	lbu	$3,50($3)
	lui	$4,%hi($LC13)
	addiu	$4,$4,%lo($LC13)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x800
$L162:
	beq	$2,$0,$L163
	ext	$2,$16,18,1

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,29($2)
	lbu	$7,30($2)
	lbu	$10,31($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+28)
	lui	$2,%hi(attack_daddr+28)
	lbu	$8,29($3)
	lbu	$9,%lo(attack_daddr+28)($2)
	lbu	$5,%lo(attack_saddr+28)($4)
	lbu	$2,31($3)
	lbu	$3,30($3)
	lui	$4,%hi($LC14)
	addiu	$4,$4,%lo($LC14)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	ext	$2,$16,18,1
$L163:
	beq	$2,$0,$L164
	ext	$2,$16,17,1

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,57($2)
	lbu	$7,58($2)
	lbu	$10,59($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+56)
	lui	$2,%hi(attack_daddr+56)
	lbu	$8,57($3)
	lbu	$9,%lo(attack_daddr+56)($2)
	lbu	$5,%lo(attack_saddr+56)($4)
	lbu	$2,59($3)
	lbu	$3,58($3)
	lui	$4,%hi($LC15)
	addiu	$4,$4,%lo($LC15)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	ext	$2,$16,17,1
$L164:
	beq	$2,$0,$L165
	andi	$2,$16,0x4000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,53($2)
	lbu	$7,54($2)
	lbu	$10,55($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+52)
	lui	$2,%hi(attack_daddr+52)
	lbu	$8,53($3)
	lbu	$9,%lo(attack_daddr+52)($2)
	lbu	$5,%lo(attack_saddr+52)($4)
	lbu	$2,55($3)
	lbu	$3,54($3)
	lui	$4,%hi($LC16)
	addiu	$4,$4,%lo($LC16)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x4000
$L165:
	beq	$2,$0,$L166
	andi	$2,$16,0x400

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,41($2)
	lbu	$7,42($2)
	lbu	$10,43($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+40)
	lui	$2,%hi(attack_daddr+40)
	lbu	$8,41($3)
	lbu	$9,%lo(attack_daddr+40)($2)
	lbu	$5,%lo(attack_saddr+40)($4)
	lbu	$2,43($3)
	lbu	$3,42($3)
	lui	$4,%hi($LC17)
	addiu	$4,$4,%lo($LC17)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x400
$L166:
	beq	$2,$0,$L167
	andi	$2,$16,0x1000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,25($2)
	lbu	$7,26($2)
	lbu	$10,27($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+24)
	lui	$2,%hi(attack_daddr+24)
	lbu	$8,25($3)
	lbu	$9,%lo(attack_daddr+24)($2)
	lbu	$5,%lo(attack_saddr+24)($4)
	lbu	$2,27($3)
	lbu	$3,26($3)
	lui	$4,%hi($LC18)
	addiu	$4,$4,%lo($LC18)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x1000
$L167:
	beql	$2,$0,$L168
	andi	$16,$16,0x2000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,33($2)
	lbu	$7,34($2)
	lbu	$10,35($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+32)
	lui	$2,%hi(attack_daddr+32)
	lbu	$8,33($3)
	lbu	$9,%lo(attack_daddr+32)($2)
	lbu	$5,%lo(attack_saddr+32)($4)
	lbu	$2,35($3)
	lbu	$3,34($3)
	lui	$4,%hi($LC19)
	addiu	$4,$4,%lo($LC19)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$16,$16,0x2000
$L168:
	beq	$16,$0,$L169
	lui	$2,%hi(LogFlag)

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lui	$3,%hi(attack_daddr)
	lbu	$6,37($2)
	lbu	$7,38($2)
	lbu	$10,39($2)
	addiu	$3,$3,%lo(attack_daddr)
	lui	$4,%hi(attack_saddr+36)
	lui	$2,%hi(attack_daddr+36)
	lbu	$8,37($3)
	lbu	$9,%lo(attack_daddr+36)($2)
	lbu	$5,%lo(attack_saddr+36)($4)
	lbu	$2,39($3)
	lbu	$3,38($3)
	lui	$4,%hi($LC20)
	addiu	$4,$4,%lo($LC20)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	lui	$2,%hi(LogFlag)
$L169:
	sw	$0,%lo(LogFlag)($2)
	lui	$3,%hi(dos_pkt+36864)
	lui	$2,%hi(dos_pkt)
	addiu	$3,$3,%lo(dos_pkt+36864)
	addiu	$2,$2,%lo(dos_pkt)
	sb	$0,0($2)
$L170:
	sw	$0,24($2)
	sw	$0,4($2)
	sw	$0,8($2)
	sw	$0,12($2)
	sw	$0,16($2)
	sw	$0,20($2)
	addiu	$2,$2,36
	bnel	$2,$3,$L170
	sb	$0,0($2)

	lui	$2,%hi(none_dos_drop_pkt_cnt)
$L145:
	lw	$2,%lo(none_dos_drop_pkt_cnt)($2)
	sltu	$3,$2,3001
	bne	$3,$0,$L136
	lui	$3,%hi(item_backup)

	lui	$2,%hi(item)
	lw	$3,%lo(item)($2)
	beql	$3,$0,$L171
	lui	$2,%hi(none_dos_drop_pkt_cnt)

	lui	$4,%hi(item_backup)
	sw	$3,%lo(item_backup)($4)
	j	$L137
	sw	$0,%lo(item)($2)

$L136:
	lw	$4,%lo(item_backup)($3)
	beq	$4,$0,$L138
	lui	$5,%hi(item)

	sw	$4,%lo(item)($5)
	sw	$0,%lo(item_backup)($3)
$L138:
	beq	$2,$0,$L172
	lui	$2,%hi(jiffies)

$L137:
	lui	$2,%hi(none_dos_drop_pkt_cnt)
$L171:
	sw	$0,%lo(none_dos_drop_pkt_cnt)($2)
	lui	$2,%hi(jiffies)
$L172:
	lw	$5,%lo(jiffies)($2)
	lw	$31,44($sp)
	lw	$16,40($sp)
	lui	$4,%hi(dos_timer)
	addiu	$4,$4,%lo(dos_timer)
	addiu	$5,$5,100
	j	mod_timer
	addiu	$sp,$sp,48

$L109:
	bnel	$10,$0,$L140
	lw	$7,8($3)

$L110:
	bnel	$9,$0,$L141
	lw	$7,12($3)

$L111:
	bnel	$8,$0,$L142
	lw	$7,16($3)

	j	$L108
	nop

	.set	macro
	.set	reorder
	.end	dos_timer_fn
	.size	dos_timer_fn, .-dos_timer_fn
	.section	.rodata.str1.4
	.align	2
$LC21:
	.ascii	"br0\000"
	.align	2
$LC22:
	.ascii	"ppp0\000"
	.align	2
$LC23:
	.ascii	"eth1\000"
	.align	2
$LC24:
	.ascii	"wlan0\000"
	.section	.text.need_filter,"ax",@progbits
	.align	2
	.globl	need_filter
	.set	nomips16
	.ent	need_filter
	.type	need_filter, @function
need_filter:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	lui	$17,%hi($LC21)
	.set	macro
	.set	reorder

	addiu	$4,$17,%lo($LC21)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	move	$16,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L193
	lui	$17,%hi($LC22)
	.set	macro
	.set	reorder

	lw	$2,372($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L174
	addiu	$17,$17,%lo($LC21)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L175
	lw	$2,12($2)
	.set	macro
	.set	reorder

$L177:
	move	$3,$17
 #APP
 # 77 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($3)
1:	lbu	$1,($4)
	addiu	$3,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($3)
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beql	$5,$0,$L176
	lw	$3,28($2)
	.set	macro
	.set	reorder

	lw	$2,8($2)
$L175:
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L177
	addiu	$4,$2,43
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L193
	lui	$17,%hi($LC22)
	.set	macro
	.set	reorder

$L176:
	lw	$4,16($16)
	.set	noreorder
	.set	nomacro
	beq	$4,$3,$L178
	move	$2,$0
	.set	macro
	.set	reorder

$L174:
	lui	$17,%hi($LC22)
$L193:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$17,%lo($LC22)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L179
	lui	$17,%hi($LC23)
	.set	macro
	.set	reorder

	lw	$2,372($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L180
	addiu	$17,$17,%lo($LC22)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L181
	lw	$3,12($2)
	.set	macro
	.set	reorder

$L183:
	move	$2,$17
 #APP
 # 77 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($2)
1:	lbu	$1,($4)
	addiu	$2,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($2)
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beql	$5,$0,$L182
	lw	$4,16($16)
	.set	macro
	.set	reorder

	lw	$3,8($3)
$L181:
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L183
	addiu	$4,$3,43
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L178
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L182:
	.set	noreorder
	.set	nomacro
	j	$L192
	lw	$2,24($3)
	.set	macro
	.set	reorder

$L179:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$17,%lo($LC23)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L184
	lui	$17,%hi($LC24)
	.set	macro
	.set	reorder

	lw	$2,372($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L180
	addiu	$17,$17,%lo($LC23)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L185
	lw	$3,12($2)
	.set	macro
	.set	reorder

$L187:
	move	$2,$17
 #APP
 # 77 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($2)
1:	lbu	$1,($4)
	addiu	$2,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($2)
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beql	$5,$0,$L194
	lw	$4,16($16)
	.set	macro
	.set	reorder

	lw	$3,8($3)
$L185:
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L187
	addiu	$4,$3,43
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L178
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L184:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$17,%lo($LC24)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L178
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

	lw	$2,372($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L180
	addiu	$17,$17,%lo($LC24)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L188
	lw	$3,12($2)
	.set	macro
	.set	reorder

$L190:
	move	$2,$17
 #APP
 # 77 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($2)
1:	lbu	$1,($4)
	addiu	$2,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($2)
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beql	$5,$0,$L189
	lw	$4,16($16)
	.set	macro
	.set	reorder

	lw	$3,8($3)
$L188:
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L190
	addiu	$4,$3,43
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L178
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L189:
$L194:
	lw	$2,28($3)
$L192:
	xor	$2,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L178
	sltu	$2,$0,$2
	.set	macro
	.set	reorder

$L180:
	li	$2,1			# 0x1
$L178:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	need_filter
	.size	need_filter, .-need_filter
	.section	.rodata.str1.4
	.align	2
$LC25:
	.ascii	"dos_filter: alloc skb fail!\012\000"
	.section	.text.GenerateTCPENDACK,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	GenerateTCPENDACK
	.type	GenerateTCPENDACK, @function
GenerateTCPENDACK:
	.frame	$sp,128,$31		# vars= 72, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-128
	sw	$31,124($sp)
	sw	$fp,120($sp)
	sw	$23,116($sp)
	sw	$22,112($sp)
	sw	$21,108($sp)
	sw	$20,104($sp)
	sw	$19,100($sp)
	sw	$18,96($sp)
	sw	$17,92($sp)
	sw	$16,88($sp)
	move	$19,$4
	lui	$4,%hi($LC21)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC21)
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,80($sp)
	.set	macro
	.set	reorder

	lw	$18,0($2)
	move	$17,$2
	lui	$2,%hi(endStateClientPort)
	ext	$18,$18,24,4
	sll	$18,$18,2
	addu	$18,$17,$18
	lhu	$3,%lo(endStateClientPort)($2)
	lhu	$2,0($18)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L196
	lw	$5,80($sp)
	.set	macro
	.set	reorder

	lui	$2,%hi(endStateClientSeq)
	lw	$3,%lo(endStateClientSeq)($2)
	lw	$2,4($18)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L220
	lw	$31,124($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$17,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L196
	move	$16,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_hh_from_neigh
	move	$4,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L216
	move	$4,$19
	.set	macro
	.set	reorder

	j	$L219
$L199:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($2)	# atomic_sub_return	
	subu	$4, $3, 1			
	sc	$4, 0($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$4,$0,$L199
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L220
	lw	$31,124($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L220
	lw	$31,124($sp)
	.set	macro
	.set	reorder

$L216:
	li	$5,96			# 0x60
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	skb_copy_expand
	li	$7,32			# 0x20
	.set	macro
	.set	reorder

	move	$20,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L200
	addiu	$21,$16,24
	.set	macro
	.set	reorder

 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
$L201:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($21)	# atomic_sub_return	
	subu	$2, $3, 1			
	sc	$2, 0($21)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$2,$0,$L201
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L222
	lui	$4,%hi($LC25)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC25)
$L222:
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC25)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L220
	lw	$31,124($sp)
	.set	macro
	.set	reorder

$L200:
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$2
	.set	macro
	.set	reorder

	lw	$5,0($2)
	move	$4,$20
	addiu	$23,$sp,24
	ext	$5,$5,24,4
	addiu	$5,$5,5
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	sll	$5,$5,2
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	skb_put
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_hh_from_neigh
	move	$4,$16
	.set	macro
	.set	reorder

	move	$5,$2
	addiu	$4,$sp,72
	.set	noreorder
	.set	nomacro
	jal	rtl_get_lock_from_hh
	move	$22,$2
	.set	macro
	.set	reorder

	lw	$2,72($sp)
	move	$4,$23
	sw	$2,24($sp)
	lw	$2,76($sp)
	.set	noreorder
	.set	nomacro
	jal	_raw_read_lock_bh
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$22
	.set	noreorder
	.set	nomacro
	jal	rtl_get_data_from_hh
	move	$fp,$2
	.set	macro
	.set	reorder

	move	$5,$2
	li	$6,16			# 0x10
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$fp,-16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	_raw_read_unlock_bh
	move	$4,$23
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_len_from_hh
	move	$4,$22
	.set	macro
	.set	reorder

	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	skb_push
	move	$4,$20
	.set	macro
	.set	reorder

 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
$L203:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($21)	# atomic_sub_return	
	subu	$2, $3, 1			
	sc	$2, 0($21)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$2,$0,$L203
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L223
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	move	$4,$20
$L223:
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$5,$0
	.set	macro
	.set	reorder

	lhu	$3,2($18)
	lw	$2,64($sp)
	li	$4,5			# 0x5
	sh	$3,52($sp)
	lhu	$3,0($18)
	ins	$2,$4,28,4
	addiu	$21,$sp,52
	sh	$3,54($sp)
	lw	$5,4($18)
	lw	$6,8($18)
	sw	$2,64($sp)
	sb	$0,13($21)
	lw	$2,64($sp)
	li	$3,1			# 0x1
	addiu	$5,$5,1
	ins	$2,$0,18,1
	ins	$2,$3,20,1
	lui	$3,%hi(url_cache+12)
	lhu	$3,%lo(url_cache+12)($3)
	ins	$2,$0,16,1
	move	$4,$20
	sw	$2,64($sp)
	sw	$5,60($sp)
	sh	$3,66($sp)
	sw	$6,56($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sh	$0,70($sp)
	.set	macro
	.set	reorder

	lw	$3,12($18)
	li	$16,20			# 0x14
	addiu	$18,$2,-34
	srl	$3,$3,28
	sll	$3,$3,2
	.set	noreorder
	.set	nomacro
	j	$L217
	li	$5,8			# 0x8
	.set	macro
	.set	reorder

$L209:
	lbu	$2,0($4)
	addiu	$4,$16,1
	sltu	$7,$2,2
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L206
	addu	$6,$21,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L217
	move	$16,$4
	.set	macro
	.set	reorder

$L206:
	lbu	$4,0($6)
	.set	noreorder
	.set	nomacro
	bne	$2,$5,$L207
	addiu	$23,$16,2
	.set	macro
	.set	reorder

	addu	$23,$21,$23
	addiu	$22,$sp,16
	addiu	$16,$16,6
	addu	$21,$21,$16
	move	$4,$22
	move	$5,$23
	addiu	$16,$sp,20
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$16
	move	$5,$21
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$23
	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$21
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L221
	addiu	$16,$sp,52
	.set	macro
	.set	reorder

$L207:
	addu	$16,$16,$4
$L217:
	sltu	$2,$16,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L209
	addu	$4,$21,$16
	.set	macro
	.set	reorder

	addiu	$16,$sp,52
$L221:
	move	$5,$18
	sh	$0,68($sp)
	move	$6,$0
	move	$4,$16
	lw	$22,16($17)
	lw	$21,12($17)
	.set	noreorder
	.set	nomacro
	jal	csum_partial
	andi	$18,$18,0xffff
	.set	macro
	.set	reorder

	move	$4,$20
	addiu	$18,$18,6
 #APP
 # 136 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
		.set	push		# csum_tcpudp_nofold
	.set	noat		
	addu	$22, $21		
	sltu	$1, $22, $21	
	addu	$22, $1		
	addu	$22, $18		
	sltu	$1, $22, $18	
	addu	$22, $1		
	addu	$22, $2		
	sltu	$1, $22, $2	
	addu	$22, $1		
	.set	pop
 # 0 "" 2
 # 78 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
		.set	push		# csum_fold
	.set	noat		
	sll	$1, $22, 16	
	addu	$22, $1		
	sltu	$1, $22, $1	
	srl	$22, $22, 16	
	addu	$22, $1		
	xori	$22, 0xffff	
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sh	$22,68($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,34
	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,20			# 0x14
	.set	macro
	.set	reorder

	lw	$2,32($sp)
	li	$3,4			# 0x4
	sh	$0,38($sp)
	ins	$2,$3,28,4
	li	$3,5			# 0x5
	ins	$2,$3,24,4
	sw	$2,32($sp)
	li	$2,6			# 0x6
	sb	$2,41($sp)
	sb	$0,33($sp)
	lw	$3,12($17)
	lui	$16,%hi(url_cache)
	addiu	$2,$16,%lo(url_cache)
	sw	$3,48($sp)
	lw	$3,16($17)
	lbu	$2,14($2)
	move	$4,$19
	move	$5,$0
	sw	$3,44($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$2,40($sp)
	.set	macro
	.set	reorder

	lhu	$2,%lo(url_cache)($16)
	li	$3,40			# 0x28
	sh	$3,34($sp)
	addiu	$2,$2,1
	addiu	$3,$sp,32
	sh	$2,36($sp)
	sh	$0,42($sp)
	lw	$7,4($3)
	lw	$6,32($sp)
	lw	$4,8($3)
	lw	$5,44($sp)
	addu	$8,$7,$6
	sltu	$7,$8,$7
	addu	$8,$8,$4
	addu	$7,$8,$7
	sltu	$4,$7,$4
	addu	$7,$7,$5
	addu	$4,$7,$4
	ext	$6,$6,24,4
	sll	$6,$6,2
	sltu	$2,$4,$5
	addu	$6,$3,$6
	addu	$2,$2,$4
	addiu	$3,$sp,48
$L210:
	lw	$4,0($3)
	addiu	$3,$3,4
	addu	$2,$2,$4
	sltu	$4,$2,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L210
	addu	$2,$4,$2
	.set	macro
	.set	reorder

	move	$4,$20
 #APP
 # 78 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
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

	lui	$2,%hi(url_cache+16)
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	dev_queue_xmit
	sb	$0,%lo(url_cache+16)($2)
	.set	macro
	.set	reorder

$L196:
	lw	$31,124($sp)
$L220:
	lw	$fp,120($sp)
	lw	$23,116($sp)
	lw	$22,112($sp)
	lw	$21,108($sp)
	lw	$20,104($sp)
	lw	$19,100($sp)
	lw	$18,96($sp)
	lw	$17,92($sp)
	lw	$16,88($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,128
	.set	macro
	.set	reorder

$L219:
 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L199
	addiu	$2,$16,24
	.set	macro
	.set	reorder

	.end	GenerateTCPENDACK
	.size	GenerateTCPENDACK, .-GenerateTCPENDACK
	.section	.text.GenerateTCPFINACK,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	GenerateTCPFINACK
	.type	GenerateTCPFINACK, @function
GenerateTCPFINACK:
	.frame	$sp,128,$31		# vars= 72, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-128
	sw	$31,124($sp)
	sw	$fp,120($sp)
	sw	$23,116($sp)
	sw	$22,112($sp)
	sw	$21,108($sp)
	sw	$20,104($sp)
	sw	$19,100($sp)
	sw	$18,96($sp)
	sw	$17,92($sp)
	sw	$16,88($sp)
	move	$19,$4
	lui	$4,%hi($LC21)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC21)
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,80($sp)
	.set	macro
	.set	reorder

	lw	$18,0($2)
	move	$17,$2
	lui	$2,%hi(finStateClientPort)
	ext	$18,$18,24,4
	sll	$18,$18,2
	addu	$18,$17,$18
	lhu	$3,%lo(finStateClientPort)($2)
	lhu	$2,0($18)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L225
	lw	$5,80($sp)
	.set	macro
	.set	reorder

	lui	$2,%hi(finStateClientSeq)
	lw	$3,%lo(finStateClientSeq)($2)
	lw	$2,4($18)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L249
	lw	$31,124($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$17,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L225
	move	$16,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_hh_from_neigh
	move	$4,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L245
	move	$4,$19
	.set	macro
	.set	reorder

	j	$L248
$L228:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($2)	# atomic_sub_return	
	subu	$4, $3, 1			
	sc	$4, 0($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$4,$0,$L228
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L249
	lw	$31,124($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L249
	lw	$31,124($sp)
	.set	macro
	.set	reorder

$L245:
	li	$5,96			# 0x60
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	skb_copy_expand
	li	$7,32			# 0x20
	.set	macro
	.set	reorder

	move	$20,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L229
	addiu	$21,$16,24
	.set	macro
	.set	reorder

 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
$L230:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($21)	# atomic_sub_return	
	subu	$2, $3, 1			
	sc	$2, 0($21)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$2,$0,$L230
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L251
	lui	$4,%hi($LC25)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC25)
$L251:
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC25)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L249
	lw	$31,124($sp)
	.set	macro
	.set	reorder

$L229:
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$2
	.set	macro
	.set	reorder

	lw	$5,0($2)
	move	$4,$20
	addiu	$23,$sp,24
	ext	$5,$5,24,4
	addiu	$5,$5,5
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	sll	$5,$5,2
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	skb_put
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_hh_from_neigh
	move	$4,$16
	.set	macro
	.set	reorder

	move	$5,$2
	addiu	$4,$sp,72
	.set	noreorder
	.set	nomacro
	jal	rtl_get_lock_from_hh
	move	$22,$2
	.set	macro
	.set	reorder

	lw	$2,72($sp)
	move	$4,$23
	sw	$2,24($sp)
	lw	$2,76($sp)
	.set	noreorder
	.set	nomacro
	jal	_raw_read_lock_bh
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$22
	.set	noreorder
	.set	nomacro
	jal	rtl_get_data_from_hh
	move	$fp,$2
	.set	macro
	.set	reorder

	move	$5,$2
	li	$6,16			# 0x10
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$fp,-16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	_raw_read_unlock_bh
	move	$4,$23
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_len_from_hh
	move	$4,$22
	.set	macro
	.set	reorder

	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	skb_push
	move	$4,$20
	.set	macro
	.set	reorder

 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
$L232:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($21)	# atomic_sub_return	
	subu	$2, $3, 1			
	sc	$2, 0($21)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$2,$0,$L232
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L252
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	move	$4,$20
$L252:
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$5,$0
	.set	macro
	.set	reorder

	lhu	$3,2($18)
	lw	$2,64($sp)
	li	$4,5			# 0x5
	sh	$3,52($sp)
	lhu	$3,0($18)
	ins	$2,$4,28,4
	addiu	$21,$sp,52
	sh	$3,54($sp)
	lw	$5,4($18)
	lw	$6,8($18)
	sw	$2,64($sp)
	sb	$0,13($21)
	lw	$2,64($sp)
	lui	$3,%hi(url_cache+12)
	li	$4,1			# 0x1
	lhu	$3,%lo(url_cache+12)($3)
	ins	$2,$0,18,1
	ins	$2,$4,20,1
	ins	$2,$4,16,1
	move	$4,$20
	sw	$2,64($sp)
	sw	$5,60($sp)
	sh	$3,66($sp)
	sw	$6,56($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sh	$0,70($sp)
	.set	macro
	.set	reorder

	lw	$3,12($18)
	li	$16,20			# 0x14
	addiu	$18,$2,-34
	srl	$3,$3,28
	sll	$3,$3,2
	.set	noreorder
	.set	nomacro
	j	$L246
	li	$5,8			# 0x8
	.set	macro
	.set	reorder

$L238:
	lbu	$2,0($4)
	addiu	$4,$16,1
	sltu	$7,$2,2
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L235
	addu	$6,$21,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L246
	move	$16,$4
	.set	macro
	.set	reorder

$L235:
	lbu	$4,0($6)
	.set	noreorder
	.set	nomacro
	bne	$2,$5,$L236
	addiu	$23,$16,2
	.set	macro
	.set	reorder

	addu	$23,$21,$23
	addiu	$22,$sp,16
	addiu	$16,$16,6
	addu	$21,$21,$16
	move	$4,$22
	move	$5,$23
	addiu	$16,$sp,20
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$16
	move	$5,$21
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$23
	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$21
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L250
	lhu	$3,54($sp)
	.set	macro
	.set	reorder

$L236:
	addu	$16,$16,$4
$L246:
	sltu	$2,$16,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L238
	addu	$4,$21,$16
	.set	macro
	.set	reorder

	lhu	$3,54($sp)
$L250:
	lui	$2,%hi(endStateClientPort)
	addiu	$16,$sp,52
	sh	$3,%lo(endStateClientPort)($2)
	lw	$3,60($sp)
	lui	$2,%hi(endStateClientSeq)
	move	$5,$18
	sw	$3,%lo(endStateClientSeq)($2)
	sh	$0,68($sp)
	move	$6,$0
	move	$4,$16
	lw	$22,16($17)
	lw	$21,12($17)
	.set	noreorder
	.set	nomacro
	jal	csum_partial
	andi	$18,$18,0xffff
	.set	macro
	.set	reorder

	move	$4,$20
	addiu	$18,$18,6
 #APP
 # 136 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
		.set	push		# csum_tcpudp_nofold
	.set	noat		
	addu	$22, $21		
	sltu	$1, $22, $21	
	addu	$22, $1		
	addu	$22, $18		
	sltu	$1, $22, $18	
	addu	$22, $1		
	addu	$22, $2		
	sltu	$1, $22, $2	
	addu	$22, $1		
	.set	pop
 # 0 "" 2
 # 78 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
		.set	push		# csum_fold
	.set	noat		
	sll	$1, $22, 16	
	addu	$22, $1		
	sltu	$1, $22, $1	
	srl	$22, $22, 16	
	addu	$22, $1		
	xori	$22, 0xffff	
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sh	$22,68($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,34
	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,20			# 0x14
	.set	macro
	.set	reorder

	lw	$2,32($sp)
	li	$3,4			# 0x4
	sh	$0,38($sp)
	ins	$2,$3,28,4
	li	$3,5			# 0x5
	ins	$2,$3,24,4
	sw	$2,32($sp)
	li	$2,6			# 0x6
	sb	$2,41($sp)
	sb	$0,33($sp)
	lw	$3,12($17)
	lui	$16,%hi(url_cache)
	addiu	$2,$16,%lo(url_cache)
	sw	$3,48($sp)
	lw	$3,16($17)
	lbu	$2,14($2)
	move	$4,$19
	move	$5,$0
	sw	$3,44($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$2,40($sp)
	.set	macro
	.set	reorder

	lhu	$2,%lo(url_cache)($16)
	li	$3,40			# 0x28
	sh	$3,34($sp)
	addiu	$2,$2,1
	addiu	$3,$sp,32
	sh	$2,36($sp)
	sh	$0,42($sp)
	lw	$7,4($3)
	lw	$6,32($sp)
	lw	$4,8($3)
	lw	$5,44($sp)
	addu	$8,$7,$6
	sltu	$7,$8,$7
	addu	$8,$8,$4
	addu	$7,$8,$7
	sltu	$4,$7,$4
	addu	$7,$7,$5
	addu	$4,$7,$4
	ext	$6,$6,24,4
	sll	$6,$6,2
	sltu	$2,$4,$5
	addu	$6,$3,$6
	addu	$2,$2,$4
	addiu	$3,$sp,48
$L239:
	lw	$4,0($3)
	addiu	$3,$3,4
	addu	$2,$2,$4
	sltu	$4,$2,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L239
	addu	$2,$4,$2
	.set	macro
	.set	reorder

	move	$4,$20
 #APP
 # 78 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
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

	lui	$2,%hi(url_cache)
	addiu	$2,$2,%lo(url_cache)
	li	$3,1			# 0x1
	move	$4,$20
	sb	$3,16($2)
	.set	noreorder
	.set	nomacro
	jal	dev_queue_xmit
	sb	$0,15($2)
	.set	macro
	.set	reorder

$L225:
	lw	$31,124($sp)
$L249:
	lw	$fp,120($sp)
	lw	$23,116($sp)
	lw	$22,112($sp)
	lw	$21,108($sp)
	lw	$20,104($sp)
	lw	$19,100($sp)
	lw	$18,96($sp)
	lw	$17,92($sp)
	lw	$16,88($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,128
	.set	macro
	.set	reorder

$L248:
 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L228
	addiu	$2,$16,24
	.set	macro
	.set	reorder

	.end	GenerateTCPFINACK
	.size	GenerateTCPFINACK, .-GenerateTCPFINACK
	.section	.rodata.str1.4
	.align	2
$LC26:
	.ascii	"HTTP/1.1 401 The web site is blocked by administrator\015"
	.ascii	"\012Server: Embedded HTTP Server 2.00\015\012Connection:"
	.ascii	" close\015\012\015\012<HTML><HEAD><TITLE>401 The web sit"
	.ascii	"e is blocked by administrator</TITLE></HEAD>\012<BODY BG"
	.ascii	"COLOR=\"#ffffff\"><H4>401 The web site is blocked by adm"
	.ascii	"inistrator</H4></BODY></HTML>\012\000"
	.section	.text.GenerateHTTP401,"ax",@progbits
	.align	2
	.globl	GenerateHTTP401
	.set	nomips16
	.ent	GenerateHTTP401
	.type	GenerateHTTP401, @function
GenerateHTTP401:
	.frame	$sp,192,$31		# vars= 136, regs= 9/0, args= 16, gp= 0
	.mask	0x80ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-192
	sw	$31,188($sp)
	sw	$23,184($sp)
	sw	$22,180($sp)
	sw	$21,176($sp)
	sw	$20,172($sp)
	sw	$19,168($sp)
	sw	$18,164($sp)
	sw	$17,160($sp)
	sw	$16,156($sp)
	move	$17,$4
	lui	$4,%hi($LC21)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC21)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,144($sp)
	.set	macro
	.set	reorder

	lw	$5,144($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$2,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L254
	move	$16,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_hh_from_neigh
	move	$4,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L280
	lui	$2,%hi(block_message)
	.set	macro
	.set	reorder

	j	$L286
$L257:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($2)	# atomic_sub_return	
	subu	$4, $3, 1			
	sc	$4, 0($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$4,$0,$L257
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L287
	lw	$31,188($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L287
	lw	$31,188($sp)
	.set	macro
	.set	reorder

$L280:
	lw	$4,%lo(block_message)($2)
	bne	$4,$0,$L282
	lui	$4,%hi($LC26)
	addiu	$4,$4,%lo($LC26)
$L282:
	jal	strlen
	move	$6,$2
	move	$4,$17
	li	$5,96			# 0x60
	.set	noreorder
	.set	nomacro
	jal	skb_copy_expand
	li	$7,32			# 0x20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L260
	move	$19,$2
	.set	macro
	.set	reorder

 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	addiu	$2,$16,24
$L261:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($2)	# atomic_sub_return	
	subu	$4, $3, 1			
	sc	$4, 0($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$4,$0,$L261
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L289
	lui	$4,%hi($LC25)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$4,%hi($LC25)
$L289:
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC25)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L287
	lw	$31,188($sp)
	.set	macro
	.set	reorder

$L260:
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$2
	.set	macro
	.set	reorder

	move	$18,$2
	lw	$2,0($2)
	move	$4,$19
	ext	$2,$2,24,4
	sll	$20,$2,2
	addu	$20,$18,$20
	lw	$3,12($20)
	srl	$3,$3,28
	addu	$2,$3,$2
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	sll	$5,$2,2
	.set	macro
	.set	reorder

	lui	$2,%hi(block_message)
	lw	$4,%lo(block_message)($2)
	bne	$4,$0,$L283
	lui	$4,%hi($LC26)
	addiu	$4,$4,%lo($LC26)
$L283:
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$22,$sp,24
	.set	macro
	.set	reorder

	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	skb_put
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_hh_from_neigh
	move	$4,$16
	.set	macro
	.set	reorder

	move	$5,$2
	addiu	$4,$sp,136
	.set	noreorder
	.set	nomacro
	jal	rtl_get_lock_from_hh
	move	$21,$2
	.set	macro
	.set	reorder

	lw	$2,136($sp)
	move	$4,$22
	sw	$2,24($sp)
	lw	$2,140($sp)
	.set	noreorder
	.set	nomacro
	jal	_raw_read_lock_bh
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$19
	.set	macro
	.set	reorder

	move	$4,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_get_data_from_hh
	move	$23,$2
	.set	macro
	.set	reorder

	move	$5,$2
	li	$6,16			# 0x10
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$23,-16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	_raw_read_unlock_bh
	move	$4,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_len_from_hh
	move	$4,$21
	.set	macro
	.set	reorder

	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	skb_push
	move	$4,$19
	.set	macro
	.set	reorder

 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	addiu	$2,$16,24
$L265:
 #APP
 # 207 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
		.set	mips3				
	ll	$3, 0($2)	# atomic_sub_return	
	subu	$4, $3, 1			
	sc	$4, 0($2)				
	.set	mips0				

 # 0 "" 2
 #NO_APP
	beq	$4,$0,$L265
 #APP
 # 228 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L290
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	move	$5,$0
$L290:
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$4,$19
	.set	macro
	.set	reorder

	lw	$16,0($18)
	lw	$2,12($18)
	lw	$3,16($18)
	ext	$16,$16,24,4
	sll	$16,$16,2
	sw	$3,12($18)
	sw	$2,16($18)
	addu	$16,$18,$16
	lhu	$2,2($16)
	lhu	$3,0($16)
	lw	$22,12($16)
	sh	$2,0($16)
	sh	$3,2($16)
	lui	$2,%hi(block_message)
	lw	$5,%lo(block_message)($2)
	srl	$22,$22,28
	sll	$22,$22,2
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L267
	addu	$22,$16,$22
	.set	macro
	.set	reorder

	move	$4,$5
	.set	noreorder
	.set	nomacro
	jal	strlen
	sw	$5,144($sp)
	.set	macro
	.set	reorder

	addiu	$6,$2,1
	move	$4,$22
	.set	noreorder
	.set	nomacro
	j	$L284
	lw	$5,144($sp)
	.set	macro
	.set	reorder

$L267:
	lui	$21,%hi($LC26)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$21,%lo($LC26)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$22
	addiu	$5,$21,%lo($LC26)
$L284:
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$21,-4			# 0xfffffffffffffffc
	.set	macro
	.set	reorder

	lw	$2,8($16)
	move	$4,$17
	lw	$17,4($16)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sw	$2,4($16)
	.set	macro
	.set	reorder

	lw	$5,12($20)
	lw	$3,0($18)
	srl	$5,$5,28
	ext	$3,$3,24,4
	addu	$5,$5,$3
	mul	$3,$5,$21
	move	$4,$19
	addu	$5,$3,$17
	addu	$2,$5,$2
	sw	$2,8($16)
	lui	$2,%hi(url_cache+12)
	lhu	$2,%lo(url_cache+12)($2)
	sb	$0,13($16)
	li	$3,1			# 0x1
	sh	$2,14($16)
	lw	$2,12($16)
	sh	$0,18($16)
	li	$17,20			# 0x14
	ins	$2,$0,18,1
	ins	$2,$3,20,1
	ins	$2,$3,19,1
	ins	$2,$0,16,1
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sw	$2,12($16)
	.set	macro
	.set	reorder

	lw	$4,0($18)
	lw	$3,12($20)
	li	$5,8			# 0x8
	ext	$20,$4,24,4
	mul	$20,$20,$21
	srl	$3,$3,28
	sll	$3,$3,2
	addiu	$20,$20,-14
	.set	noreorder
	.set	nomacro
	j	$L281
	addu	$20,$20,$2
	.set	macro
	.set	reorder

$L273:
	lbu	$2,0($4)
	addiu	$4,$17,1
	sltu	$7,$2,2
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L270
	addu	$6,$16,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L281
	move	$17,$4
	.set	macro
	.set	reorder

$L270:
	lbu	$4,0($6)
	.set	noreorder
	.set	nomacro
	bne	$2,$5,$L271
	addiu	$23,$17,2
	.set	macro
	.set	reorder

	addu	$23,$16,$23
	addiu	$21,$sp,16
	addiu	$17,$17,6
	move	$4,$21
	move	$5,$23
	addu	$17,$16,$17
	addiu	$22,$sp,20
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$22
	move	$5,$17
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$23
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$17
	move	$5,$21
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L288
	move	$5,$20
	.set	macro
	.set	reorder

$L271:
	addu	$17,$17,$4
$L281:
	sltu	$2,$17,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L273
	addu	$4,$16,$17
	.set	macro
	.set	reorder

	move	$5,$20
$L288:
	sh	$0,16($16)
	move	$6,$0
	move	$4,$16
	andi	$20,$20,0xffff
	lw	$21,16($18)
	lw	$17,12($18)
	.set	noreorder
	.set	nomacro
	jal	csum_partial
	addiu	$20,$20,6
	.set	macro
	.set	reorder

	move	$4,$19
 #APP
 # 136 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
		.set	push		# csum_tcpudp_nofold
	.set	noat		
	addu	$21, $17		
	sltu	$1, $21, $17	
	addu	$21, $1		
	addu	$21, $20		
	sltu	$1, $21, $20	
	addu	$21, $1		
	addu	$21, $2		
	sltu	$1, $21, $2	
	addu	$21, $1		
	.set	pop
 # 0 "" 2
 # 78 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
		.set	push		# csum_fold
	.set	noat		
	sll	$1, $21, 16	
	addu	$21, $1		
	sltu	$1, $21, $1	
	srl	$21, $21, 16	
	addu	$21, $1		
	xori	$21, 0xffff	
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sh	$21,16($16)
	.set	macro
	.set	reorder

	addiu	$2,$2,-14
	lui	$3,%hi(url_cache)
	sh	$2,2($18)
	addiu	$2,$3,%lo(url_cache)
	lbu	$2,14($2)
	sh	$0,6($18)
	lw	$7,0($18)
	sb	$2,8($18)
	lhu	$2,%lo(url_cache)($3)
	sh	$0,10($18)
	lw	$4,8($18)
	addiu	$2,$2,2
	sh	$2,4($18)
	lw	$6,4($18)
	lw	$5,12($18)
	ext	$8,$7,24,4
	addu	$7,$6,$7
	sltu	$6,$7,$6
	addu	$7,$7,$4
	addu	$6,$7,$6
	sltu	$4,$6,$4
	addu	$6,$6,$5
	addu	$4,$6,$4
	sltu	$3,$4,$5
	sll	$6,$8,2
	addu	$3,$3,$4
	addu	$5,$18,$6
	addiu	$2,$18,16
$L274:
	lw	$4,0($2)
	addiu	$2,$2,4
	addu	$3,$3,$4
	sltu	$4,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$5,$L274
	addu	$3,$4,$3
	.set	macro
	.set	reorder

	move	$2,$3
	move	$4,$19
	li	$3,1			# 0x1
 #APP
 # 78 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/checksum.h" 1
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
	sh	$2,10($18)
	lui	$2,%hi(url_cache+15)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sb	$3,%lo(url_cache+15)($2)
	.set	macro
	.set	reorder

	move	$5,$2
	addiu	$4,$sp,32
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,54			# 0x36
	.set	macro
	.set	reorder

	lhu	$3,2($16)
	lui	$2,%hi(finStateClientPort)
	move	$4,$19
	sh	$3,%lo(finStateClientPort)($2)
	lw	$3,8($16)
	lui	$2,%hi(finStateClientSeq)
	.set	noreorder
	.set	nomacro
	jal	dev_queue_xmit
	sw	$3,%lo(finStateClientSeq)($2)
	.set	macro
	.set	reorder

$L254:
	lw	$31,188($sp)
$L287:
	lw	$23,184($sp)
	lw	$22,180($sp)
	lw	$21,176($sp)
	lw	$20,172($sp)
	lw	$19,168($sp)
	lw	$18,164($sp)
	lw	$17,160($sp)
	lw	$16,156($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,192
	.set	macro
	.set	reorder

$L286:
 #APP
 # 185 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L257
	addiu	$2,$16,24
	.set	macro
	.set	reorder

	.end	GenerateHTTP401
	.size	GenerateHTTP401, .-GenerateHTTP401
	.section	.rodata.str1.4
	.align	2
$LC27:
	.ascii	"%s\000"
	.section	.text.register_customRspStr,"ax",@progbits
	.align	2
	.globl	register_customRspStr
	.set	nomips16
	.ent	register_customRspStr
	.type	register_customRspStr, @function
register_customRspStr:
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
	jal	unregister_customRspStr
	lui	$17,%hi(block_message)

	beql	$16,$0,$L292
	lui	$16,%hi($LC26)

	jal	strlen
	move	$4,$16

	addiu	$4,$2,1
	jal	__kmalloc
	li	$5,208			# 0xd0

	beq	$2,$0,$L294
	sw	$2,%lo(block_message)($17)

	lui	$5,%hi($LC27)
	move	$4,$2
	addiu	$5,$5,%lo($LC27)
	j	$L295
	move	$6,$16

$L292:
	jal	strlen
	addiu	$4,$16,%lo($LC26)

	move	$4,$2
	jal	__kmalloc
	li	$5,208			# 0xd0

	beq	$2,$0,$L294
	sw	$2,%lo(block_message)($17)

	lui	$5,%hi($LC27)
	move	$4,$2
	addiu	$5,$5,%lo($LC27)
	addiu	$6,$16,%lo($LC26)
$L295:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	sprintf
	addiu	$sp,$sp,32

$L294:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	register_customRspStr
	.size	register_customRspStr, .-register_customRspStr
	.section	.text.strtok,"ax",@progbits
	.align	2
	.globl	strtok
	.set	nomips16
	.ent	strtok
	.type	strtok, @function
strtok:
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	move	$17,$5
	bne	$4,$0,$L298
	move	$16,$4

	lui	$2,%hi(___strtok)
	lw	$16,%lo(___strtok)($2)
	beq	$16,$0,$L303
	move	$2,$16

$L298:
	move	$4,$16
	jal	strspn
	move	$5,$17

	addu	$16,$16,$2
	lb	$2,0($16)
	bnel	$2,$0,$L300
	move	$5,$17

	lui	$2,%hi(___strtok)
	sw	$0,%lo(___strtok)($2)
	j	$L299
	move	$16,$0

$L300:
	jal	strpbrk
	move	$4,$16

	beq	$2,$0,$L304
	lui	$3,%hi(___strtok)

	lb	$3,0($2)
	beq	$3,$0,$L304
	lui	$3,%hi(___strtok)

	sb	$0,0($2)
	addiu	$2,$2,1
$L304:
	sw	$2,%lo(___strtok)($3)
$L299:
	move	$2,$16
$L303:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	strtok
	.size	strtok, .-strtok
	.section	.rodata.str1.4
	.align	2
$LC28:
	.ascii	" \000"
	.section	.text.dos_single_write,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	dos_single_write
	.type	dos_single_write, @function
dos_single_write:
	.frame	$sp,136,$31		# vars= 80, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-136
	sw	$31,132($sp)
	sw	$fp,128($sp)
	sw	$23,124($sp)
	sw	$22,120($sp)
	sw	$21,116($sp)
	sw	$20,112($sp)
	sw	$19,108($sp)
	sw	$18,104($sp)
	sw	$17,100($sp)
	sw	$16,96($sp)
	sltu	$3,$6,2
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L306
	move	$16,$6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L324
	li	$2,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	addiu	$3,$5,80
	or	$2,$3,$5
	lw	$3,24($28)
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L306
	lui	$2,%hi(dos_flag)
	.set	macro
	.set	reorder

	addiu	$4,$2,%lo(dos_flag)
	li	$6,80			# 0x50
 #APP
 # 3048 "net/rtl/fastpath/filter.c" 1
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
	bne	$6,$0,$L306
	addiu	$4,$sp,16
	.set	macro
	.set	reorder

	addiu	$2,$2,%lo(dos_flag)
	li	$5,80			# 0x50
	move	$3,$4
 #APP
 # 52 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($2)
	subu	$5,1
	sb	$1,($3)
	beqz	$1,2f
	addiu	$3,1
	bnez	$5,1b
	addiu	$2,1
2:
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	li	$18,1			# 0x1
	lui	$fp,%hi(per_fin_threshold)
	lui	$23,%hi(per_syn_threshold)
	lui	$22,%hi(whole_icmp_threshold)
	lui	$21,%hi(whole_udp_threshold)
	lui	$20,%hi(whole_fin_threshold)
	.set	noreorder
	.set	nomacro
	j	$L307
	lui	$19,%hi(whole_syn_threshold)
	.set	macro
	.set	reorder

$L323:
	move	$4,$17
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	simple_strtol
	move	$6,$0
	.set	macro
	.set	reorder

	addiu	$3,$18,-1
	andi	$3,$3,0x00ff
	sltu	$4,$3,13
	.set	noreorder
	.set	nomacro
	beql	$4,$0,$L326
	addiu	$18,$18,1
	.set	macro
	.set	reorder

	lui	$4,%hi($L322)
	sll	$3,$3,2
	addiu	$4,$4,%lo($L322)
	addu	$3,$4,$3
	lw	$3,0($3)
	j	$3
	.section	.rodata.dos_single_write,"a",@progbits
	.align	2
	.align	2
$L322:
	.word	$L309
	.word	$L310
	.word	$L311
	.word	$L312
	.word	$L313
	.word	$L314
	.word	$L315
	.word	$L316
	.word	$L317
	.word	$L318
	.word	$L319
	.word	$L320
	.word	$L321
	.section	.text.dos_single_write
$L309:
	lui	$3,%hi(op_mode)
	.set	noreorder
	.set	nomacro
	j	$L308
	sb	$2,%lo(op_mode)($3)
	.set	macro
	.set	reorder

$L310:
	move	$4,$17
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	simple_strtol
	li	$6,16			# 0x10
	.set	macro
	.set	reorder

	lui	$4,%hi(lan_addr)
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(lan_addr)($4)
	.set	macro
	.set	reorder

$L311:
	move	$4,$17
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	simple_strtol
	li	$6,16			# 0x10
	.set	macro
	.set	reorder

	lui	$3,%hi(lan_mask)
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(lan_mask)($3)
	.set	macro
	.set	reorder

$L312:
	lui	$4,%hi(item)
	sw	$2,%lo(item)($4)
	lui	$2,%hi(item_backup)
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$0,%lo(item_backup)($2)
	.set	macro
	.set	reorder

$L313:
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(whole_syn_threshold)($19)
	.set	macro
	.set	reorder

$L314:
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(whole_fin_threshold)($20)
	.set	macro
	.set	reorder

$L315:
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(whole_udp_threshold)($21)
	.set	macro
	.set	reorder

$L316:
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(whole_icmp_threshold)($22)
	.set	macro
	.set	reorder

$L317:
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(per_syn_threshold)($23)
	.set	macro
	.set	reorder

$L318:
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(per_fin_threshold)($fp)
	.set	macro
	.set	reorder

$L319:
	lui	$3,%hi(per_udp_threshold)
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(per_udp_threshold)($3)
	.set	macro
	.set	reorder

$L320:
	lui	$4,%hi(per_icmp_threshold)
	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$2,%lo(per_icmp_threshold)($4)
	.set	macro
	.set	reorder

$L321:
	lui	$3,%hi(block_time)
	sw	$2,%lo(block_time)($3)
$L308:
	addiu	$18,$18,1
$L326:
	andi	$18,$18,0x00ff
	move	$4,$0
$L307:
	lui	$5,%hi($LC28)
	.set	noreorder
	.set	nomacro
	jal	strtok
	addiu	$5,$5,%lo($LC28)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L323
	move	$17,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L324
	move	$2,$16
	.set	macro
	.set	reorder

$L306:
	li	$2,-14			# 0xfffffffffffffff2
$L324:
	lw	$31,132($sp)
	lw	$fp,128($sp)
	lw	$23,124($sp)
	lw	$22,120($sp)
	lw	$21,116($sp)
	lw	$20,112($sp)
	lw	$19,108($sp)
	lw	$18,104($sp)
	lw	$17,100($sp)
	lw	$16,96($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,136
	.set	macro
	.set	reorder

	.end	dos_single_write
	.size	dos_single_write, .-dos_single_write
	.section	.rodata.str1.4
	.align	2
$LC29:
	.ascii	"[ip_range_filter]%d.%d.%d.%d ===> %d.%d.%d.%d  [Blocked]"
	.ascii	"\012\000"
	.align	2
$LC30:
	.ascii	"[mac_filter]%d.%d.%d.%d(%02x:%02x:%02x:%02x:%02x:%02x) ="
	.ascii	"==> %d.%d.%d.%d [Blocked]\012\000"
	.align	2
$LC31:
	.ascii	"[url_key_filter]%d.%d.%d.%d ==> %d.%d.%d.%d(%s)  [Blocke"
	.ascii	"d]\012\000"
	.align	2
$LC32:
	.ascii	"[schedule_filter]%d.%d.%d.%d ===> %d.%d.%d.%d  [Blocked]"
	.ascii	"\012\000"
	.align	2
$LC33:
	.ascii	"Unkown Type [Blocked]\012\000"
	.section	.text.log_fastFilter,"ax",@progbits
	.align	2
	.globl	log_fastFilter
	.set	nomips16
	.ent	log_fastFilter
	.type	log_fastFilter, @function
log_fastFilter:
	.frame	$sp,184,$31		# vars= 8, regs= 9/0, args= 136, gp= 0
	.mask	0x80ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-184
	sw	$31,180($sp)
	sw	$23,176($sp)
	sw	$22,172($sp)
	sw	$21,168($sp)
	sw	$20,164($sp)
	sw	$19,160($sp)
	sw	$18,156($sp)
	sw	$17,152($sp)
	sw	$16,148($sp)
	move	$17,$5
	move	$18,$4
	jal	do_gettimeofday
	addiu	$4,$sp,136

	jal	rtl_ip_hdr
	move	$4,$17

	move	$4,$17
	jal	rtl_eth_hdr
	move	$16,$2

	li	$3,2			# 0x2
	beq	$18,$3,$L330
	nop

	slt	$2,$18,3
	beq	$2,$0,$L333
	li	$2,3			# 0x3

	li	$2,1			# 0x1
	bne	$18,$2,$L355
	lui	$4,%hi($LC33)

	j	$L352
	nop

$L333:
	beq	$18,$2,$L331
	li	$2,4			# 0x4

	bne	$18,$2,$L328
	lui	$4,%hi($LC33)

	j	$L353
	nop

$L352:
	beql	$16,$0,$L354
	lw	$31,180($sp)

	lw	$3,16($16)
	lui	$4,%hi($LC29)
	lw	$6,12($16)
	andi	$2,$3,0xff
	j	$L349
	addiu	$4,$4,%lo($LC29)

$L330:
	beq	$2,$0,$L354
	lw	$31,180($sp)

	beql	$16,$0,$L356
	lw	$23,176($sp)

	move	$4,$17
	jal	rtl_eth_hdr
	lw	$18,12($16)

	move	$4,$17
	jal	rtl_eth_hdr
	move	$23,$2

	move	$4,$17
	jal	rtl_eth_hdr
	move	$22,$2

	move	$4,$17
	jal	rtl_eth_hdr
	move	$21,$2

	move	$4,$17
	jal	rtl_eth_hdr
	move	$20,$2

	move	$4,$17
	jal	rtl_eth_hdr
	move	$19,$2

	andi	$4,$18,0xff
	lw	$3,16($16)
	addiu	$23,$23,84
	sw	$4,16($sp)
	srl	$5,$18,24
	ext	$6,$18,16,8
	ext	$7,$18,8,8
	move	$4,$0
	j	$L336
	addiu	$8,$sp,20

$L335:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L336:
	sltu	$9,$4,14
	addu	$11,$23,$4
	bne	$9,$0,$L335
	addu	$10,$8,$4

	addiu	$22,$22,98
	move	$4,$0
	j	$L338
	addiu	$8,$sp,36

$L337:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L338:
	sltu	$9,$4,14
	addu	$11,$22,$4
	bne	$9,$0,$L337
	addu	$10,$8,$4

	addiu	$21,$21,112
	move	$4,$0
	j	$L340
	addiu	$8,$sp,52

$L339:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L340:
	sltu	$9,$4,14
	addu	$11,$21,$4
	bne	$9,$0,$L339
	addu	$10,$8,$4

	addiu	$20,$20,126
	move	$4,$0
	j	$L342
	addiu	$8,$sp,68

$L341:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L342:
	sltu	$9,$4,14
	addu	$11,$20,$4
	bne	$9,$0,$L341
	addu	$10,$8,$4

	addiu	$19,$19,140
	move	$4,$0
	j	$L344
	addiu	$8,$sp,84

$L343:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L344:
	sltu	$9,$4,14
	addu	$11,$19,$4
	bne	$9,$0,$L343
	addu	$10,$8,$4

	addiu	$2,$2,154
	move	$4,$0
	j	$L346
	addiu	$8,$sp,100

$L345:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L346:
	sltu	$9,$4,14
	addu	$11,$2,$4
	bne	$9,$0,$L345
	addu	$10,$8,$4

	lui	$4,%hi($LC30)
	srl	$9,$3,24
	ext	$8,$3,16,8
	andi	$2,$3,0xff
	addiu	$4,$4,%lo($LC30)
	ext	$3,$3,8,8
	sw	$9,116($sp)
	sw	$8,120($sp)
	sw	$3,124($sp)
	jal	scrlog_printk
	sw	$2,128($sp)

	j	$L354
	lw	$31,180($sp)

$L331:
	beql	$16,$0,$L354
	lw	$31,180($sp)

	lw	$3,16($16)
	lw	$6,12($16)
	lui	$4,%hi($LC31)
	andi	$2,$3,0xff
	sw	$2,32($sp)
	lui	$2,%hi(log_info)
	andi	$10,$6,0xff
	srl	$9,$3,24
	ext	$8,$3,16,8
	addiu	$2,$2,%lo(log_info)
	ext	$3,$3,8,8
	ext	$7,$6,8,8
	srl	$5,$6,24
	addiu	$4,$4,%lo($LC31)
	ext	$6,$6,16,8
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	sw	$2,36($sp)
$L348:
	jal	scrlog_printk
	nop

	j	$L354
	lw	$31,180($sp)

$L353:
	beql	$16,$0,$L354
	lw	$31,180($sp)

	lw	$3,16($16)
	lw	$6,12($16)
	lui	$4,%hi($LC32)
	andi	$2,$3,0xff
	addiu	$4,$4,%lo($LC32)
$L349:
	andi	$10,$6,0xff
	srl	$9,$3,24
	ext	$8,$3,16,8
	ext	$3,$3,8,8
	ext	$7,$6,8,8
	srl	$5,$6,24
	sw	$10,16($sp)
	ext	$6,$6,16,8
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	j	$L348
	sw	$2,32($sp)

$L328:
$L355:
	jal	printk
	addiu	$4,$4,%lo($LC33)

	lw	$31,180($sp)
$L354:
	lw	$23,176($sp)
$L356:
	lw	$22,172($sp)
	lw	$21,168($sp)
	lw	$20,164($sp)
	lw	$19,160($sp)
	lw	$18,156($sp)
	lw	$17,152($sp)
	lw	$16,148($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,184

	.set	macro
	.set	reorder
	.end	log_fastFilter
	.size	log_fastFilter, .-log_fastFilter
	.section	.text.do_filter,"ax",@progbits
	.align	2
	.globl	do_filter
	.set	nomips16
	.ent	do_filter
	.type	do_filter, @function
do_filter:
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
	lui	$2,%hi(table_list_head)
	move	$17,$4
	addiu	$20,$2,%lo(table_list_head)
	lw	$16,%lo(table_list_head)($2)
	li	$19,-1			# 0xffffffffffffffff
	j	$L358
	li	$18,1			# 0x1

$L363:
	jal	scan_table
	nop

	beql	$2,$19,$L362
	move	$2,$0

	bnel	$2,$18,$L360
	lw	$16,0($16)

	lui	$2,%hi(log_enable)
	lw	$3,%lo(log_enable)($2)
	beq	$3,$0,$L362
	li	$2,1			# 0x1

	lw	$4,16($16)
	jal	log_fastFilter
	move	$5,$17

	j	$L362
	li	$2,1			# 0x1

$L360:
$L358:
	move	$6,$16
	move	$7,$16
	move	$4,$17
	bne	$16,$20,$L363
	move	$5,$0

	move	$2,$0
$L362:
	lw	$31,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	do_filter
	.size	do_filter, .-do_filter
	.section	.text.FilterWeb_v2,"ax",@progbits
	.align	2
	.globl	FilterWeb_v2
	.set	nomips16
	.ent	FilterWeb_v2
	.type	FilterWeb_v2, @function
FilterWeb_v2:
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

	jal	need_filter
	move	$4,$16

	bne	$2,$0,$L366
	lw	$31,20($sp)

	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

$L366:
	move	$4,$16
	lw	$16,16($sp)
	j	do_filter
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	FilterWeb_v2
	.size	FilterWeb_v2, .-FilterWeb_v2
	.section	.text.filter_enter,"ax",@progbits
	.align	2
	.globl	filter_enter
	.set	nomips16
	.ent	filter_enter
	.type	filter_enter, @function
filter_enter:
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
	lui	$2,%hi(item)
	lw	$2,%lo(item)($2)
	bne	$2,$0,$L370
	move	$17,$4

	lui	$2,%hi(table_list_head+24)
	lw	$2,%lo(table_list_head+24)($2)
	sltu	$2,$2,2
	beq	$2,$0,$L370
	lui	$3,%hi(none_dos_drop_pkt_cnt)

	lw	$4,%lo(none_dos_drop_pkt_cnt)($3)
	j	$L445
	li	$2,1			# 0x1

$L370:
	jal	rtl_get_skb_type
	move	$4,$17

	bne	$2,$0,$L371
	li	$2,4			# 0x4

	jal	rtl_get_skb_protocol
	move	$4,$17

	li	$3,2048			# 0x800
	bne	$2,$3,$L371
	li	$2,4			# 0x4

	lui	$2,%hi(table_list_head+24)
	lw	$2,%lo(table_list_head+24)($2)
	sltu	$2,$2,2
	bne	$2,$0,$L454
	lui	$2,%hi(item)

	jal	FilterWeb_v2
	move	$4,$17

	beq	$2,$0,$L374
	lui	$3,%hi(fast_path_cusRsp401_func)

	j	$L451
	lw	$2,%lo(fast_path_cusRsp401_func)($3)

$L374:
	lw	$2,%lo(fast_path_cusRsp401_func)($3)
	beq	$2,$0,$L373
	lui	$3,%hi(url_cache)

	addiu	$3,$3,%lo(url_cache)
	lbu	$4,15($3)
	li	$2,1			# 0x1
	bnel	$4,$2,$L376
	lbu	$3,16($3)

	lui	$2,%hi(fast_path_cusRspTCPFinAck_func)
	j	$L451
	lw	$2,%lo(fast_path_cusRspTCPFinAck_func)($2)

$L376:
	bne	$3,$2,$L454
	lui	$2,%hi(item)

	lui	$2,%hi(fast_path_cusRspTCPEndAck_func)
	lw	$2,%lo(fast_path_cusRspTCPEndAck_func)($2)
$L451:
	beql	$2,$0,$L371
	move	$2,$0

	jalr	$2
	move	$4,$17

	j	$L371
	move	$2,$0

$L373:
	lui	$2,%hi(item)
$L454:
	lw	$2,%lo(item)($2)
	andi	$2,$2,0x1
	beq	$2,$0,$L377
	nop

	jal	rtl_ip_hdr
	move	$4,$17

	lui	$4,%hi($LC22)
	addiu	$4,$4,%lo($LC22)
	lw	$20,0($2)
	jal	rtl_get_dev_by_name
	move	$16,$2

	lui	$4,%hi($LC23)
	addiu	$4,$4,%lo($LC23)
	lui	$18,%hi(wan_ppp_dev)
	sw	$2,%lo(wan_ppp_dev)($18)
	jal	rtl_get_dev_by_name
	lui	$19,%hi(wan_dev)

	move	$4,$17
	ext	$20,$20,24,4
	jal	rtl_get_skb_dev
	sw	$2,%lo(wan_dev)($19)

	beq	$2,$0,$L377
	nop

	jal	rtl_get_skb_dev
	move	$4,$17

	lw	$3,%lo(wan_dev)($19)
	beq	$3,$2,$L455
	lui	$2,%hi(item)

	jal	rtl_get_skb_dev
	move	$4,$17

	lw	$3,%lo(wan_ppp_dev)($18)
	bne	$3,$2,$L377
	lui	$2,%hi(item)

$L455:
	lw	$3,%lo(item)($2)
	sll	$20,$20,2
	andi	$2,$3,0x3fe
	beq	$2,$0,$L379
	addu	$20,$16,$20

	lw	$4,12($16)
	lui	$2,%hi(ConnectedIp)
	addiu	$2,$2,%lo(ConnectedIp)
	andi	$5,$4,0xff
	sll	$5,$5,2
	addu	$2,$5,$2
	lw	$5,0($2)
	andi	$7,$4,0x3ff
	li	$2,36			# 0x24
	mul	$8,$7,$2
	lui	$6,%hi(dos_pkt)
	addiu	$6,$6,%lo(dos_pkt)
	bne	$4,$5,$L380
	addu	$2,$8,$6

	lw	$5,24($2)
	bnel	$5,$4,$L456
	lw	$6,24($2)

	j	$L379
	sb	$0,0($2)

$L380:
	lw	$6,24($2)
$L456:
	beq	$6,$4,$L381
	lb	$5,0($2)

	bnel	$5,$0,$L404
	move	$2,$0

	lui	$5,%hi(cur_p_pkt)
	sw	$2,%lo(cur_p_pkt)($5)
	sw	$4,24($2)
	li	$4,1			# 0x1
	j	$L382
	sb	$4,0($2)

$L381:
	li	$4,1			# 0x1
	bnel	$5,$4,$L404
	move	$2,$0

	lui	$4,%hi(cur_p_pkt)
	j	$L382
	sw	$2,%lo(cur_p_pkt)($4)

$L439:
	lui	$2,%hi(cur_p_pkt)
	lw	$5,%lo(cur_p_pkt)($2)
	li	$2,6			# 0x6
	bne	$4,$2,$L385
	move	$2,$0

	lbu	$4,13($20)
	li	$2,2			# 0x2
	andi	$4,$4,0x3f
	bne	$4,$2,$L385
	move	$2,$0

	lui	$2,%hi(block)
	lw	$4,%lo(block)($2)
	li	$2,1			# 0x1
	bnel	$4,$2,$L457
	lw	$4,4($5)

	lui	$2,%hi(attack_saddr+4)
	lw	$6,%lo(attack_saddr+4)($2)
	lw	$4,12($16)
	beq	$6,$4,$L385
	li	$2,-1			# 0xffffffffffffffff

	lw	$4,4($5)
$L457:
	lui	$2,%hi(per_syn_threshold)
	addiu	$4,$4,1
	sw	$4,4($5)
	lw	$2,%lo(per_syn_threshold)($2)
	slt	$4,$2,$4
	beql	$4,$0,$L385
	move	$2,$0

	andi	$4,$3,0x20
	beql	$4,$0,$L385
	move	$2,$0

	blez	$2,$L383
	lui	$4,%hi(attack_saddr+4)

	lw	$5,12($16)
	lui	$2,%hi(LogFlag)
	sw	$5,%lo(attack_saddr+4)($4)
	lw	$6,16($16)
	lw	$4,%lo(LogFlag)($2)
	lui	$5,%hi(attack_daddr+4)
	sw	$6,%lo(attack_daddr+4)($5)
	ori	$4,$4,0x20
	li	$6,1			# 0x1
	lui	$5,%hi(block)
	sw	$6,%lo(block)($5)
	sw	$4,%lo(LogFlag)($2)
$L383:
	move	$2,$0
$L385:
	andi	$4,$3,0x44
	beq	$4,$0,$L458
	andi	$4,$3,0x88

	lbu	$5,9($16)
	lui	$4,%hi(cur_p_pkt)
	lw	$6,%lo(cur_p_pkt)($4)
	li	$4,6			# 0x6
	bne	$5,$4,$L389
	move	$4,$0

	lbu	$4,13($20)
	li	$5,1			# 0x1
	andi	$4,$4,0x3f
	bnel	$4,$5,$L389
	move	$4,$0

	lui	$5,%hi(block)
	lw	$5,%lo(block)($5)
	bnel	$5,$4,$L459
	lw	$5,8($6)

	lui	$4,%hi(attack_saddr+8)
	lw	$7,%lo(attack_saddr+8)($4)
	lw	$5,12($16)
	beq	$7,$5,$L389
	li	$4,-1			# 0xffffffffffffffff

	lw	$5,8($6)
$L459:
	lui	$4,%hi(per_fin_threshold)
	addiu	$5,$5,1
	sw	$5,8($6)
	lw	$4,%lo(per_fin_threshold)($4)
	slt	$5,$4,$5
	beql	$5,$0,$L389
	move	$4,$0

	andi	$5,$3,0x40
	beql	$5,$0,$L389
	move	$4,$0

	blez	$4,$L387
	lui	$5,%hi(attack_saddr+8)

	lw	$6,12($16)
	lui	$4,%hi(LogFlag)
	sw	$6,%lo(attack_saddr+8)($5)
	lw	$7,16($16)
	lw	$5,%lo(LogFlag)($4)
	lui	$6,%hi(attack_daddr+8)
	sw	$7,%lo(attack_daddr+8)($6)
	ori	$5,$5,0x40
	li	$7,1			# 0x1
	lui	$6,%hi(block)
	sw	$7,%lo(block)($6)
	sw	$5,%lo(LogFlag)($4)
$L387:
	move	$4,$0
$L389:
	or	$2,$2,$4
	andi	$4,$3,0x88
$L458:
	beq	$4,$0,$L460
	andi	$4,$3,0x110

	lbu	$5,9($16)
	lui	$4,%hi(cur_p_pkt)
	lw	$6,%lo(cur_p_pkt)($4)
	li	$4,17			# 0x11
	bne	$5,$4,$L393
	move	$4,$0

	lui	$4,%hi(block)
	lw	$5,%lo(block)($4)
	li	$4,1			# 0x1
	bnel	$5,$4,$L461
	lw	$5,12($6)

	lui	$4,%hi(attack_saddr+12)
	lw	$7,%lo(attack_saddr+12)($4)
	lw	$5,12($16)
	beq	$7,$5,$L393
	li	$4,-1			# 0xffffffffffffffff

	lw	$5,12($6)
$L461:
	lui	$4,%hi(per_udp_threshold)
	addiu	$5,$5,1
	sw	$5,12($6)
	lw	$4,%lo(per_udp_threshold)($4)
	slt	$5,$4,$5
	beql	$5,$0,$L393
	move	$4,$0

	andi	$5,$3,0x80
	beql	$5,$0,$L393
	move	$4,$0

	blez	$4,$L391
	lui	$5,%hi(attack_saddr+12)

	lw	$6,12($16)
	lui	$4,%hi(LogFlag)
	sw	$6,%lo(attack_saddr+12)($5)
	lw	$7,16($16)
	lw	$5,%lo(LogFlag)($4)
	lui	$6,%hi(attack_daddr+12)
	sw	$7,%lo(attack_daddr+12)($6)
	ori	$5,$5,0x80
	li	$7,1			# 0x1
	lui	$6,%hi(block)
	sw	$7,%lo(block)($6)
	sw	$5,%lo(LogFlag)($4)
$L391:
	move	$4,$0
$L393:
	or	$2,$2,$4
	andi	$4,$3,0x110
$L460:
	beq	$4,$0,$L462
	andi	$4,$3,0x200

	lbu	$4,9($16)
	lui	$5,%hi(cur_p_pkt)
	lw	$6,%lo(cur_p_pkt)($5)
	li	$5,1			# 0x1
	bnel	$4,$5,$L397
	move	$4,$0

	lui	$5,%hi(block)
	lw	$5,%lo(block)($5)
	bnel	$5,$4,$L463
	lw	$5,16($6)

	lui	$4,%hi(attack_saddr+16)
	lw	$7,%lo(attack_saddr+16)($4)
	lw	$5,12($16)
	beq	$7,$5,$L397
	li	$4,-1			# 0xffffffffffffffff

	lw	$5,16($6)
$L463:
	lui	$4,%hi(per_icmp_threshold)
	addiu	$5,$5,1
	sw	$5,16($6)
	lw	$4,%lo(per_icmp_threshold)($4)
	slt	$5,$4,$5
	beql	$5,$0,$L397
	move	$4,$0

	andi	$5,$3,0x100
	beql	$5,$0,$L397
	move	$4,$0

	blez	$4,$L395
	lui	$5,%hi(attack_saddr+16)

	lw	$6,12($16)
	lui	$4,%hi(LogFlag)
	sw	$6,%lo(attack_saddr+16)($5)
	lw	$7,16($16)
	lw	$5,%lo(LogFlag)($4)
	lui	$6,%hi(attack_daddr+16)
	sw	$7,%lo(attack_daddr+16)($6)
	ori	$5,$5,0x100
	li	$7,1			# 0x1
	lui	$6,%hi(block)
	sw	$7,%lo(block)($6)
	sw	$5,%lo(LogFlag)($4)
$L395:
	move	$4,$0
$L397:
	or	$2,$2,$4
	andi	$4,$3,0x200
$L462:
	beql	$4,$0,$L464
	ext	$4,$3,22,1

	lbu	$6,9($16)
	lui	$4,%hi(cur_p_pkt)
	li	$5,6			# 0x6
	bne	$6,$5,$L466
	lw	$4,%lo(cur_p_pkt)($4)

	lhu	$5,32($4)
	beq	$5,$0,$L465
	nop

	lhu	$6,2($20)
	beq	$6,$5,$L465
	nop

	lw	$5,20($4)
	addiu	$5,$5,1
	sw	$5,20($4)
	lhu	$5,32($4)
$L465:
	bnel	$5,$0,$L466
	lbu	$6,9($16)

	lhu	$5,2($20)
	sh	$5,32($4)
	lbu	$6,9($16)
$L466:
	li	$5,17			# 0x11
	bne	$6,$5,$L467
	ext	$5,$3,23,1

	lhu	$5,32($4)
	beq	$5,$0,$L468
	nop

	lhu	$6,2($20)
	beq	$6,$5,$L468
	nop

	lw	$5,20($4)
	addiu	$5,$5,1
	sw	$5,20($4)
	lhu	$5,32($4)
$L468:
	bne	$5,$0,$L467
	ext	$5,$3,23,1

	lhu	$5,2($20)
	sh	$5,32($4)
	ext	$5,$3,23,1
$L467:
	beq	$5,$0,$L403
	lw	$4,20($4)

	j	$L447
	slt	$4,$4,11

$L403:
	slt	$4,$4,201
$L447:
	bne	$4,$0,$L464
	ext	$4,$3,22,1

	lui	$4,%hi(LogFlag)
	lw	$5,%lo(LogFlag)($4)
	ori	$5,$5,0x200
	sw	$5,%lo(LogFlag)($4)
	lw	$5,12($16)
	lui	$4,%hi(attack_saddr+20)
	sw	$5,%lo(attack_saddr+20)($4)
	lw	$5,16($16)
	lui	$4,%hi(attack_daddr+20)
	sw	$5,%lo(attack_daddr+20)($4)
	ext	$4,$3,22,1
$L464:
	bne	$4,$0,$L469
	andi	$4,$3,0x8000

$L379:
	move	$2,$0
$L404:
	andi	$4,$3,0x8000
$L469:
	beq	$4,$0,$L470
	li	$4,65536			# 0x10000

	lbu	$5,9($16)
	li	$4,6			# 0x6
	bne	$5,$4,$L407
	move	$4,$0

	lbu	$5,13($20)
	lui	$4,%hi(_tcpDosScanBitmap)
	addiu	$4,$4,%lo(_tcpDosScanBitmap)
	andi	$5,$5,0x3f
	addu	$4,$5,$4
	lb	$4,0($4)
	beql	$4,$0,$L406
	move	$4,$0

	lui	$4,%hi(LogFlag)
	lw	$5,%lo(LogFlag)($4)
	ori	$5,$5,0x8000
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+44)
	li	$4,-1			# 0xffffffffffffffff
	sw	$6,%lo(attack_saddr+44)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+44)
	sw	$6,%lo(attack_daddr+44)($5)
$L406:
$L407:
	or	$2,$2,$4
	li	$4,65536			# 0x10000
$L470:
	and	$5,$3,$4
	beql	$5,$0,$L471
	andi	$4,$3,0x800

	lbu	$6,9($16)
	li	$5,6			# 0x6
	bnel	$6,$5,$L411
	move	$4,$0

	lbu	$6,13($20)
	li	$5,2			# 0x2
	andi	$6,$6,0x3f
	bnel	$6,$5,$L411
	move	$4,$0

	lw	$6,0($16)
	lw	$5,12($20)
	lhu	$7,2($16)
	ext	$6,$6,24,4
	sll	$6,$6,2
	srl	$5,$5,28
	subu	$6,$7,$6
	sll	$5,$5,2
	bne	$6,$5,$L472
	lui	$5,%hi(LogFlag)

	lhu	$5,6($16)
	andi	$5,$5,0x2000
	beql	$5,$0,$L409
	move	$4,$0

	lui	$5,%hi(LogFlag)
$L472:
	lw	$6,%lo(LogFlag)($5)
	or	$4,$6,$4
	sw	$4,%lo(LogFlag)($5)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+48)
	li	$4,-1			# 0xffffffffffffffff
	sw	$6,%lo(attack_saddr+48)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+48)
	sw	$6,%lo(attack_daddr+48)($5)
$L409:
$L411:
	or	$2,$2,$4
	andi	$4,$3,0x800
$L471:
	beq	$4,$0,$L473
	ext	$4,$3,18,1

	lw	$6,12($16)
	lw	$5,16($16)
	bne	$6,$5,$L414
	move	$4,$0

	lui	$4,%hi(LogFlag)
	lw	$5,%lo(LogFlag)($4)
	ori	$5,$5,0x800
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+28)
	li	$4,-1			# 0xffffffffffffffff
	sw	$6,%lo(attack_saddr+28)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+28)
	sw	$6,%lo(attack_daddr+28)($5)
$L414:
	or	$2,$2,$4
	ext	$4,$3,18,1
$L473:
	beq	$4,$0,$L474
	li	$5,131072			# 0x20000

	lbu	$5,9($16)
	li	$4,17			# 0x11
	bne	$5,$4,$L418
	move	$4,$0

	lhu	$4,2($20)
	li	$7,7			# 0x7
	beql	$4,$7,$L475
	lui	$4,%hi(LogFlag)

	beq	$4,$5,$L417
	li	$6,19			# 0x13

	beq	$4,$6,$L475
	lui	$4,%hi(LogFlag)

	lhu	$4,0($20)
	beql	$4,$7,$L475
	lui	$4,%hi(LogFlag)

	beql	$4,$5,$L475
	lui	$4,%hi(LogFlag)

	bnel	$4,$6,$L416
	move	$4,$0

$L417:
	lui	$4,%hi(LogFlag)
$L475:
	lw	$6,%lo(LogFlag)($4)
	li	$5,262144			# 0x40000
	or	$5,$6,$5
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+56)
	li	$4,-1			# 0xffffffffffffffff
	sw	$6,%lo(attack_saddr+56)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+56)
	sw	$6,%lo(attack_daddr+56)($5)
$L416:
$L418:
	or	$2,$2,$4
	li	$5,131072			# 0x20000
$L474:
	and	$4,$3,$5
	beq	$4,$0,$L476
	andi	$4,$3,0x4000

	lbu	$6,9($16)
	li	$4,17			# 0x11
	bne	$6,$4,$L421
	move	$4,$0

	lhu	$4,6($16)
	andi	$4,$4,0x3fff
	bne	$4,$0,$L421
	move	$4,$0

	lw	$4,0($16)
	lhu	$7,2($16)
	lhu	$6,4($20)
	ext	$4,$4,24,4
	sll	$4,$4,2
	subu	$4,$7,$4
	slt	$4,$6,$4
	beql	$4,$0,$L420
	move	$4,$0

	lui	$4,%hi(LogFlag)
	lw	$6,%lo(LogFlag)($4)
	or	$5,$6,$5
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+52)
	li	$4,-1			# 0xffffffffffffffff
	sw	$6,%lo(attack_saddr+52)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+52)
	sw	$6,%lo(attack_daddr+52)($5)
$L420:
$L421:
	or	$2,$2,$4
	andi	$4,$3,0x4000
$L476:
	beq	$4,$0,$L477
	andi	$4,$3,0x400

	lhu	$4,6($16)
	andi	$5,$4,0x2000
	bnel	$5,$0,$L424
	move	$4,$0

	andi	$4,$4,0x1fff
	beql	$4,$0,$L424
	move	$4,$0

	lw	$6,0($16)
	lhu	$5,2($16)
	sll	$4,$4,3
	ext	$6,$6,24,4
	sll	$6,$6,2
	subu	$4,$4,$6
	addu	$4,$4,$5
	li	$5,65536			# 0x10000
	sltu	$4,$4,$5
	bnel	$4,$0,$L423
	move	$4,$0

	lui	$4,%hi(LogFlag)
	lw	$5,%lo(LogFlag)($4)
	ori	$5,$5,0x4000
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+40)
	li	$4,-1			# 0xffffffffffffffff
	sw	$6,%lo(attack_saddr+40)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+40)
	sw	$6,%lo(attack_daddr+40)($5)
$L423:
$L424:
	or	$2,$2,$4
	andi	$4,$3,0x400
$L477:
	beql	$4,$0,$L478
	andi	$4,$3,0x1000

	lw	$4,0($16)
	lbu	$6,9($16)
	li	$5,1			# 0x1
	bne	$6,$5,$L426
	ext	$4,$4,24,4

	sll	$4,$4,2
	addu	$4,$16,$4
	lbu	$5,0($4)
	li	$4,8			# 0x8
	bne	$5,$4,$L427
	move	$4,$0

	lui	$4,%hi(lan_addr)
	lw	$6,%lo(lan_addr)($4)
	lui	$5,%hi(lan_mask)
	lw	$4,12($16)
	lw	$5,%lo(lan_mask)($5)
	xor	$4,$6,$4
	and	$4,$4,$5
	bnel	$4,$0,$L427
	move	$4,$0

	lui	$4,%hi(LogFlag)
	lw	$5,%lo(LogFlag)($4)
	ori	$5,$5,0x400
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+24)
	li	$4,-1			# 0xffffffffffffffff
	sw	$6,%lo(attack_saddr+24)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+24)
	j	$L427
	sw	$6,%lo(attack_daddr+24)($5)

$L426:
	move	$4,$0
$L427:
	or	$2,$2,$4
	andi	$4,$3,0x1000
$L478:
	beql	$4,$0,$L479
	andi	$3,$3,0x2000

	lui	$4,%hi(lan_addr)
	lw	$6,%lo(lan_addr)($4)
	lui	$5,%hi(lan_mask)
	lw	$4,12($16)
	lw	$5,%lo(lan_mask)($5)
	xor	$4,$6,$4
	and	$4,$4,$5
	bne	$4,$0,$L430
	move	$4,$0

	lbu	$5,9($16)
	li	$4,1			# 0x1
	beq	$5,$4,$L429
	lui	$4,%hi(LogFlag)

	lw	$5,%lo(LogFlag)($4)
	ori	$5,$5,0x1000
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+32)
	li	$4,-1			# 0xffffffffffffffff
	sw	$6,%lo(attack_saddr+32)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+32)
	j	$L430
	sw	$6,%lo(attack_daddr+32)($5)

$L429:
	move	$4,$0
$L430:
	or	$2,$2,$4
	andi	$3,$3,0x2000
$L479:
	beq	$3,$0,$L431
	lui	$3,%hi(ConnectedIp)

	lw	$4,12($16)
	addiu	$3,$3,%lo(ConnectedIp)
	andi	$5,$4,0xff
	sll	$5,$5,2
	addu	$3,$5,$3
	lw	$5,0($3)
	andi	$7,$4,0x3ff
	li	$3,36			# 0x24
	mul	$8,$7,$3
	lui	$6,%hi(dos_pkt)
	addiu	$6,$6,%lo(dos_pkt)
	bne	$4,$5,$L432
	addu	$3,$8,$6

	lw	$5,24($3)
	bnel	$5,$4,$L480
	lw	$6,24($3)

	j	$L433
	sb	$0,0($3)

$L432:
	lw	$6,24($3)
$L480:
	beq	$6,$4,$L434
	lb	$5,0($3)

	bnel	$5,$0,$L438
	move	$4,$0

	lui	$5,%hi(cur_p_pkt)
	sw	$3,%lo(cur_p_pkt)($5)
	sw	$4,24($3)
	li	$4,1			# 0x1
	j	$L435
	sb	$4,0($3)

$L434:
	li	$4,1			# 0x1
	bne	$5,$4,$L438
	move	$4,$0

	lui	$4,%hi(cur_p_pkt)
	j	$L435
	sw	$3,%lo(cur_p_pkt)($4)

$L440:
	bne	$4,$0,$L438
	move	$4,$0

	lhu	$4,6($16)
$L453:
	andi	$6,$4,0x3fff
	beql	$6,$0,$L438
	move	$4,$0

	lhu	$6,30($3)
	sll	$7,$4,3
	andi	$7,$7,0xffff
	slt	$7,$7,$6
	bnel	$7,$0,$L436
	lui	$3,%hi(LogFlag)

	andi	$4,$4,0x2000
	bnel	$4,$0,$L437
	sh	$5,28($3)

	sh	$0,30($3)
	j	$L438
	sh	$0,28($3)

$L437:
	lw	$5,0($16)
	lhu	$7,2($16)
	move	$4,$0
	ext	$5,$5,24,4
	andi	$5,$5,0x00ff
	addu	$6,$7,$6
	sll	$5,$5,2
	subu	$6,$6,$5
	j	$L438
	sh	$6,30($3)

$L436:
	lw	$4,%lo(LogFlag)($3)
	ori	$4,$4,0x2000
	sw	$4,%lo(LogFlag)($3)
	lw	$5,12($16)
	lui	$3,%hi(attack_saddr+36)
	li	$4,-1			# 0xffffffffffffffff
	sw	$5,%lo(attack_saddr+36)($3)
	lw	$5,16($16)
	lui	$3,%hi(attack_daddr+36)
	j	$L438
	sw	$5,%lo(attack_daddr+36)($3)

$L433:
	move	$4,$0
$L438:
	or	$2,$2,$4
$L431:
	bne	$2,$0,$L375
	move	$2,$0

$L377:
	jal	rtl_ip_hdr
	move	$4,$17

	lbu	$3,9($2)
	li	$2,1			# 0x1
	beq	$3,$2,$L372
	lui	$3,%hi(none_dos_drop_pkt_cnt)

	lw	$4,%lo(none_dos_drop_pkt_cnt)($3)
$L445:
	addiu	$4,$4,1
	j	$L371
	sw	$4,%lo(none_dos_drop_pkt_cnt)($3)

$L372:
	li	$2,4			# 0x4
$L375:
$L371:
	lw	$31,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,40

$L382:
	andi	$2,$3,0x22
	bnel	$2,$0,$L439
	lbu	$4,9($16)

	j	$L385
	move	$2,$0

$L435:
	lui	$3,%hi(cur_p_pkt)
	lw	$3,%lo(cur_p_pkt)($3)
	lhu	$5,4($16)
	lhu	$4,28($3)
	bne	$5,$4,$L440
	nop

	j	$L453
	lhu	$4,6($16)

	.set	macro
	.set	reorder
	.end	filter_enter
	.size	filter_enter, .-filter_enter
	.section	.rodata.str1.4
	.align	2
$LC34:
	.ascii	"type=%d num=%d\012\000"
	.align	2
$LC35:
	.ascii	"index=%d\011\000"
	.align	2
$LC36:
	.ascii	"mac=%02x:%02x:%02x:%02x:%02x:%02x\012\000"
	.align	2
$LC37:
	.ascii	"index=%d\011\011\000"
	.align	2
$LC38:
	.ascii	"url=%s\012\000"
	.align	2
$LC39:
	.ascii	"start=%x\011\000"
	.align	2
$LC40:
	.ascii	"end=%x\011\000"
	.align	2
$LC41:
	.ascii	"day=%x\012\000"
	.align	2
$LC42:
	.ascii	"unknow table type\012\000"
	.align	2
$LC43:
	.ascii	"Mode:%d %s\012\000"
	.section	.text.filter_table_read_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	filter_table_read_proc
	.type	filter_table_read_proc, @function
filter_table_read_proc:
	.frame	$sp,80,$31		# vars= 8, regs= 10/0, args= 32, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-80
	sw	$31,76($sp)
	sw	$fp,72($sp)
	sw	$23,68($sp)
	sw	$22,64($sp)
	sw	$21,60($sp)
	sw	$20,56($sp)
	sw	$19,52($sp)
	sw	$18,48($sp)
	sw	$17,44($sp)
	sw	$16,40($sp)
	lui	$16,%hi(kmalloc_caches)
	addiu	$16,$16,%lo(kmalloc_caches)
	sw	$4,80($sp)
	lw	$4,36($16)
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0

	beq	$2,$0,$L482
	move	$17,$2

	move	$4,$2
	move	$5,$0
	jal	memset
	li	$6,512			# 0x200

	lw	$4,32($16)
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0

	move	$16,$2
	beq	$2,$0,$L499
	move	$4,$17

	move	$4,$2
	move	$5,$0
	jal	memset
	li	$6,256			# 0x100

	lui	$fp,%hi($LC37)
	lui	$2,%hi(table_list_head)
	lui	$23,%hi($LC38)
	lui	$20,%hi($LC35)
	lui	$22,%hi($LC39)
	lui	$21,%hi($LC40)
	addiu	$fp,$fp,%lo($LC37)
	addiu	$23,$23,%lo($LC38)
	addiu	$20,$20,%lo($LC35)
	addiu	$22,$22,%lo($LC39)
	addiu	$21,$21,%lo($LC40)
	j	$L484
	lw	$18,%lo(table_list_head)($2)

$L496:
	lw	$6,16($18)
	lui	$5,%hi($LC34)
	move	$4,$17
	addiu	$7,$7,-1
	jal	sprintf
	addiu	$5,$5,%lo($LC34)

	lw	$2,16($18)
	li	$3,3			# 0x3
	beql	$2,$3,$L487
	lw	$19,8($18)

	li	$3,4			# 0x4
	beql	$2,$3,$L488
	lw	$19,8($18)

	li	$3,2			# 0x2
	bnel	$2,$3,$L498
	lui	$5,%hi($LC42)

	lw	$19,8($18)
	j	$L489
	addiu	$3,$18,8

$L490:
	lw	$6,20($19)
	jal	sprintf
	sw	$3,36($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lbu	$2,30($19)
	lbu	$6,28($19)
	lbu	$7,29($19)
	sw	$2,16($sp)
	lbu	$2,31($19)
	lui	$5,%hi($LC36)
	move	$4,$16
	sw	$2,20($sp)
	lbu	$2,32($19)
	addiu	$5,$5,%lo($LC36)
	sw	$2,24($sp)
	lbu	$2,33($19)
	jal	sprintf
	sw	$2,28($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,36($sp)
$L489:
	move	$4,$16
	bne	$19,$3,$L490
	move	$5,$20

	j	$L484
	lw	$18,0($18)

$L487:
	j	$L492
	addiu	$3,$18,8

$L493:
	lw	$6,20($19)
	jal	sprintf
	sw	$3,36($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	addiu	$6,$19,28
	move	$4,$16
	jal	sprintf
	move	$5,$23

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,36($sp)
$L492:
	move	$4,$16
	bne	$19,$3,$L493
	move	$5,$fp

	j	$L484
	lw	$18,0($18)

$L488:
	j	$L494
	addiu	$3,$18,8

$L495:
	lw	$6,20($19)
	jal	sprintf
	sw	$3,36($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$6,28($19)
	move	$4,$16
	jal	sprintf
	move	$5,$22

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$2,32($sp)
	move	$4,$16
	move	$5,$21
	jal	sprintf
	lw	$6,4($2)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$2,32($sp)
	lui	$5,%hi($LC41)
	move	$4,$16
	lw	$6,8($2)
	jal	sprintf
	addiu	$5,$5,%lo($LC41)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,36($sp)
$L494:
	addiu	$2,$19,28
	move	$4,$16
	move	$5,$20
	bne	$19,$3,$L495
	sw	$2,32($sp)

	j	$L484
	lw	$18,0($18)

$L498:
	move	$4,$16
	jal	sprintf
	addiu	$5,$5,%lo($LC42)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$18,0($18)
$L484:
	lui	$3,%hi(table_list_head)
	addiu	$3,$3,%lo(table_list_head)
	bnel	$18,$3,$L496
	lw	$7,24($18)

	lui	$2,%hi(url_filter_mode)
	lw	$6,%lo(url_filter_mode)($2)
	lw	$4,80($sp)
	lui	$5,%hi($LC43)
	addiu	$5,$5,%lo($LC43)
	jal	seq_printf
	move	$7,$17

	jal	kfree
	move	$4,$17

	move	$4,$16
$L499:
	jal	kfree
	nop

$L482:
	lw	$31,76($sp)
	lw	$fp,72($sp)
	lw	$23,68($sp)
	lw	$22,64($sp)
	lw	$21,60($sp)
	lw	$20,56($sp)
	lw	$19,52($sp)
	lw	$18,48($sp)
	lw	$17,44($sp)
	lw	$16,40($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,80

	.set	macro
	.set	reorder
	.end	filter_table_read_proc
	.size	filter_table_read_proc, .-filter_table_read_proc
	.section	.text.filter_item_regist,"ax",@progbits
	.align	2
	.globl	filter_item_regist
	.set	nomips16
	.ent	filter_item_regist
	.type	filter_item_regist, @function
filter_item_regist:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lui	$2,%hi(table_list_head)
	addiu	$3,$2,%lo(table_list_head)
	j	$L501
	lw	$2,%lo(table_list_head)($2)

$L505:
	bnel	$6,$4,$L502
	lw	$2,0($2)

	lw	$3,8($2)
	addiu	$6,$2,8
	sw	$5,4($3)
	sw	$3,0($5)
	sw	$6,4($5)
	lw	$3,24($2)
	sw	$5,8($2)
	addiu	$3,$3,1
	sw	$3,24($2)
	li	$2,3			# 0x3
	bne	$4,$2,$L504
	move	$2,$0

	li	$2,2			# 0x2
	bnel	$3,$2,$L503
	move	$2,$0

	jal	Add_Pattern_ACL_For_ContentFilter
	nop

	j	$L504
	move	$2,$0

$L502:
$L501:
	bnel	$2,$3,$L505
	lw	$6,16($2)

	li	$2,-1			# 0xffffffffffffffff
$L503:
$L504:
	lw	$31,20($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_item_regist
	.size	filter_item_regist, .-filter_item_regist
	.section	.text.filter_table_flush,"ax",@progbits
	.align	2
	.globl	filter_table_flush
	.set	nomips16
	.ent	filter_table_flush
	.type	filter_table_flush, @function
filter_table_flush:
	.frame	$sp,56,$31		# vars= 0, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
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
	lui	$18,%hi(table_list_head)
	lw	$16,%lo(table_list_head)($18)
	li	$20,1048576			# 0x100000
	li	$19,2097152			# 0x200000
	addiu	$18,$18,%lo(table_list_head)
	ori	$20,$20,0x100
	ori	$19,$19,0x200
	lw	$17,0($16)
	li	$22,3			# 0x3
	j	$L508
	li	$21,1			# 0x1

$L512:
	addiu	$fp,$16,8
	j	$L509
	lw	$23,0($2)

$L510:
	lw	$3,4($2)
	lw	$5,0($2)
	sw	$3,4($5)
	sw	$5,0($3)
	sw	$20,0($2)
	jal	kfree
	sw	$19,4($2)

	lw	$3,24($16)
	move	$2,$23
	addiu	$3,$3,-1
	sw	$3,24($16)
	lw	$23,0($23)
$L509:
	bne	$2,$fp,$L510
	move	$4,$2

	lw	$2,16($16)
	bnel	$2,$22,$L514
	lw	$2,4($16)

	lw	$2,24($16)
	bnel	$2,$21,$L514
	lw	$2,4($16)

	jal	Del_Pattern_ACL_For_ContentFilter
	nop

	lw	$2,4($16)
$L514:
	lw	$3,0($16)
	move	$4,$16
	sw	$2,4($3)
	sw	$3,0($2)
	sw	$20,0($16)
	jal	kfree
	sw	$19,4($16)

	lw	$2,24($18)
	move	$16,$17
	addiu	$2,$2,-1
	sw	$2,24($18)
	lw	$17,0($17)
$L508:
	bnel	$16,$18,$L512
	lw	$2,8($16)

	li	$3,1			# 0x1
	lui	$2,%hi(filter_rule_num)
	sw	$3,%lo(filter_rule_num)($2)
	lw	$31,52($sp)
	lui	$2,%hi(log_enable)
	lw	$fp,48($sp)
	lw	$23,44($sp)
	lw	$22,40($sp)
	lw	$21,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	sw	$0,%lo(log_enable)($2)
	lui	$2,%hi(url_filter_mode)
	sw	$0,%lo(url_filter_mode)($2)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	filter_table_flush
	.size	filter_table_flush, .-filter_table_flush
	.section	.rodata.str1.4
	.align	2
$LC44:
	.ascii	"init\000"
	.align	2
$LC45:
	.ascii	"flush\000"
	.align	2
$LC46:
	.ascii	"enable_log\000"
	.align	2
$LC47:
	.ascii	"white\000"
	.align	2
$LC48:
	.ascii	"black\000"
	.align	2
$LC49:
	.ascii	"add:\000"
	.align	2
$LC50:
	.ascii	"#\000"
	.align	2
$LC51:
	.ascii	";\000"
	.section	.text.filter_table_single_write,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	filter_table_single_write
	.type	filter_table_single_write, @function
filter_table_single_write:
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
	lui	$2,%hi(kmalloc_caches+40)
	lw	$4,%lo(kmalloc_caches+40)($2)
	move	$19,$5
	li	$5,208			# 0xd0
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	move	$17,$6
	.set	macro
	.set	reorder

	move	$16,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L517
	li	$18,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	move	$5,$0
	li	$6,1024			# 0x400
	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	memset
	lui	$20,%hi(get_info_from_usr_space)
	.set	macro
	.set	reorder

	addiu	$4,$20,%lo(get_info_from_usr_space)
	.set	noreorder
	.set	nomacro
	jal	strlen
	sw	$16,20($sp)
	.set	macro
	.set	reorder

	move	$6,$2
	addiu	$4,$20,%lo(get_info_from_usr_space)
	.set	noreorder
	.set	nomacro
	jal	memset
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$19,$0,$L518
	addu	$2,$19,$17
	.set	macro
	.set	reorder

	lw	$3,24($28)
	or	$2,$2,$19
	or	$2,$2,$17
	and	$2,$2,$3
	move	$18,$17
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L520
	move	$6,$17
	.set	macro
	.set	reorder

	addiu	$4,$20,%lo(get_info_from_usr_space)
	move	$5,$19
 #APP
 # 702 "net/rtl/fastpath/filter_v2.c" 1
	.set	noreorder
	jal	__copy_user
	.set	noat
	addu	$1, $5, $6
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
$L520:
	bne	$6,$0,$L518
	.set	noreorder
	.set	nomacro
	beql	$17,$0,$L551
	lui	$4,%hi(get_info_from_usr_space)
	.set	macro
	.set	reorder

	lui	$3,%hi(get_info_from_usr_space)
	addiu	$3,$3,%lo(get_info_from_usr_space)
	move	$2,$16
 #APP
 # 52 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($3)
	subu	$17,1
	sb	$1,($2)
	beqz	$1,2f
	addiu	$2,1
	bnez	$17,1b
	addiu	$3,1
2:
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	lui	$4,%hi(get_info_from_usr_space)
$L551:
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$4,%lo(get_info_from_usr_space)
	.set	macro
	.set	reorder

	addu	$2,$16,$2
	sb	$0,-1($2)
	lw	$19,20($sp)
	lui	$17,%hi($LC44)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC44)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC44)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L522
	lw	$19,20($sp)
	.set	macro
	.set	reorder

	lui	$17,%hi($LC28)
	addiu	$19,$sp,20
	addiu	$5,$17,%lo($LC28)
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	strsep
	lui	$20,%hi(filter_tlb)
	.set	macro
	.set	reorder

	addiu	$17,$17,%lo($LC28)
	addiu	$20,$20,%lo(filter_tlb)
	lw	$2,20($sp)
	move	$5,$17
$L552:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L518
	move	$4,$19
	.set	macro
	.set	reorder

	jal	strsep
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	sll	$3,$2,3
	addu	$3,$20,$3
	lw	$5,4($3)
	.set	noreorder
	.set	nomacro
	jal	filter_table_regist
	move	$4,$2
	.set	macro
	.set	reorder

	lw	$2,20($sp)
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L552
	move	$5,$17
	.set	macro
	.set	reorder

	j	$L524
$L522:
	lui	$17,%hi($LC45)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC45)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC45)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L525
	lw	$19,20($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	filter_table_flush
	move	$4,$0
	.set	macro
	.set	reorder

	j	$L524
$L525:
	lui	$17,%hi($LC46)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC46)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC46)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L526
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	lui	$2,%hi(log_enable)
	.set	noreorder
	.set	nomacro
	j	$L524
	sw	$3,%lo(log_enable)($2)
	.set	macro
	.set	reorder

$L526:
	lw	$19,20($sp)
	lui	$17,%hi($LC47)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC47)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC47)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L527
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	lui	$2,%hi(url_filter_mode)
	.set	noreorder
	.set	nomacro
	j	$L524
	sw	$3,%lo(url_filter_mode)($2)
	.set	macro
	.set	reorder

$L527:
	lw	$19,20($sp)
	lui	$17,%hi($LC48)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC48)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC48)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L528
	lui	$2,%hi(url_filter_mode)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L524
	sw	$0,%lo(url_filter_mode)($2)
	.set	macro
	.set	reorder

$L528:
	lw	$19,20($sp)
	lui	$17,%hi($LC49)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC49)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC49)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L524
	lw	$19,20($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC49)
	.set	macro
	.set	reorder

	addu	$2,$19,$2
	sw	$2,20($sp)
	lb	$2,0($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L518
	addiu	$17,$sp,20
	.set	macro
	.set	reorder

	lui	$5,%hi($LC50)
	addiu	$5,$5,%lo($LC50)
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$4,$17
	.set	macro
	.set	reorder

	lw	$3,20($sp)
	beq	$3,$0,$L518
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	li	$3,-1			# 0xffffffffffffffff
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L518
	lui	$5,%hi($LC28)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$5,%lo($LC28)
	.set	macro
	.set	reorder

	lw	$3,20($sp)
	beq	$3,$0,$L518
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	li	$17,-1			# 0xffffffffffffffff
	.set	noreorder
	.set	nomacro
	beq	$2,$17,$L518
	addiu	$23,$sp,20
	.set	macro
	.set	reorder

	lui	$21,%hi($LC28)
	move	$4,$23
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$21,%lo($LC28)
	.set	macro
	.set	reorder

	lw	$3,20($sp)
	beq	$3,$0,$L518
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$17,$L518
	move	$fp,$2
	.set	macro
	.set	reorder

	lui	$22,%hi($LC51)
	addiu	$22,$22,%lo($LC51)
	addiu	$21,$21,%lo($LC28)
	addiu	$20,$sp,16
	lui	$19,%hi(filter_rule_num)
	move	$4,$23
$L556:
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L529
	sw	$2,16($sp)
	.set	macro
	.set	reorder

$L531:
	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$4,$17
	.set	macro
	.set	reorder

	addu	$2,$17,$2
	li	$3,59			# 0x3b
	sb	$3,0($2)
	move	$4,$23
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$22
	.set	macro
	.set	reorder

	lw	$2,20($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L553
	move	$4,$20
	.set	macro
	.set	reorder

$L529:
	lw	$4,16($sp)
	.set	noreorder
	.set	nomacro
	jal	check_esc_end
	li	$5,92			# 0x5c
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L531
	lw	$17,16($sp)
	.set	macro
	.set	reorder

	move	$4,$20
$L553:
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$21
	.set	macro
	.set	reorder

	move	$17,$2
	lw	$2,16($sp)
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L554
	move	$4,$17
	.set	macro
	.set	reorder

	j	$L518
$L533:
	jal	strlen
	addu	$2,$17,$2
	li	$5,32			# 0x20
	sb	$5,0($2)
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$21
	.set	macro
	.set	reorder

	move	$4,$17
$L554:
	.set	noreorder
	.set	nomacro
	jal	check_esc_end
	li	$5,92			# 0x5c
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L533
	move	$4,$17
	.set	macro
	.set	reorder

	move	$2,$17
	.set	noreorder
	.set	nomacro
	j	$L534
	move	$4,$0
	.set	macro
	.set	reorder

$L538:
	.set	noreorder
	.set	nomacro
	bnel	$3,$5,$L555
	addiu	$4,$4,1
	.set	macro
	.set	reorder

	move	$3,$2
	.set	noreorder
	.set	nomacro
	j	$L536
	move	$5,$4
	.set	macro
	.set	reorder

$L537:
	addiu	$5,$5,1
	addiu	$3,$3,1
$L536:
	lb	$6,1($3)
	.set	noreorder
	.set	nomacro
	bnel	$6,$0,$L537
	sb	$6,0($3)
	.set	macro
	.set	reorder

	addu	$5,$17,$5
	sb	$0,0($5)
	addiu	$4,$4,1
$L555:
	addiu	$2,$2,1
$L534:
	lb	$3,0($2)
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L538
	li	$5,92			# 0x5c
	.set	macro
	.set	reorder

	lui	$2,%hi(kmalloc_caches)
	addiu	$2,$2,%lo(kmalloc_caches)
	lw	$4,4($2)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L524
	move	$5,$2
	.set	macro
	.set	reorder

	addiu	$2,$2,8
	sw	$2,12($5)
	sw	$5,0($5)
	sw	$5,4($5)
	sw	$2,8($5)
	sw	$0,24($5)
	lw	$2,%lo(filter_rule_num)($19)
	move	$4,$17
	sw	$fp,16($5)
	sw	$2,20($5)
	.set	noreorder
	.set	nomacro
	jal	strlen
	sw	$5,24($sp)
	.set	macro
	.set	reorder

	sltu	$3,$2,41
	lw	$5,24($sp)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L549
	li	$2,39			# 0x27
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L542
	lw	$5,24($sp)
	.set	macro
	.set	reorder

$L549:
	addiu	$3,$5,28
	move	$4,$2
 #APP
 # 52 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($17)
	subu	$4,1
	sb	$1,($3)
	beqz	$1,2f
	addiu	$3,1
	bnez	$4,1b
	addiu	$17,1
2:
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
$L542:
	addu	$2,$5,$2
	sb	$0,28($2)
	.set	noreorder
	.set	nomacro
	jal	filter_item_regist
	li	$4,3			# 0x3
	.set	macro
	.set	reorder

	lw	$3,%lo(filter_rule_num)($19)
	lw	$2,20($sp)
	addiu	$3,$3,1
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L524
	sw	$3,%lo(filter_rule_num)($19)
	.set	macro
	.set	reorder

	lb	$2,0($2)
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L556
	move	$4,$23
	.set	macro
	.set	reorder

$L524:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L550
	move	$2,$18
	.set	macro
	.set	reorder

$L518:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	li	$18,-14			# 0xfffffffffffffff2
$L517:
	move	$2,$18
$L550:
	lw	$31,68($sp)
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

	.end	filter_table_single_write
	.size	filter_table_single_write, .-filter_table_single_write
	.section	.text.rtl_mac_filter_fastpath,"ax",@progbits
	.align	2
	.globl	rtl_mac_filter_fastpath
	.set	nomips16
	.ent	rtl_mac_filter_fastpath
	.type	rtl_mac_filter_fastpath, @function
rtl_mac_filter_fastpath:
	.frame	$sp,48,$31		# vars= 8, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$19,40($sp)
	sw	$18,36($sp)
	sw	$17,32($sp)
	sw	$16,28($sp)
	move	$17,$0
	move	$16,$4
	li	$18,6			# 0x6
	addu	$2,$5,$17
$L563:
	move	$4,$16
	lbu	$19,0($2)
	jal	rtl_skb_mac_header
	sw	$5,16($sp)

	addu	$2,$2,$17
	lbu	$2,6($2)
	addiu	$17,$17,1
	beq	$19,$2,$L558
	lw	$5,16($sp)

	j	$L559
	move	$2,$0

$L558:
	bne	$17,$18,$L563
	addu	$2,$5,$17

	li	$2,1			# 0x1
$L559:
	lw	$31,44($sp)
	lw	$19,40($sp)
	lw	$18,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	j	$31
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	rtl_mac_filter_fastpath
	.size	rtl_mac_filter_fastpath, .-rtl_mac_filter_fastpath
	.section	.text.T.1337,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	T.1337
	.type	T.1337, @function
T.1337:
	.frame	$sp,72,$31		# vars= 16, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
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
	lw	$18,88($sp)
	lw	$16,92($sp)
	move	$17,$4
	move	$19,$5
	sw	$0,0($16)
	move	$8,$6
	sw	$0,0($18)
	move	$22,$4
	move	$20,$0
	move	$21,$0
	subu	$3,$5,$7
	li	$fp,13			# 0xd
	j	$L565
	li	$23,10			# 0xa

$L575:
	bne	$2,$fp,$L566
	nop

	andi	$2,$20,0x1
	beq	$2,$0,$L568
	addiu	$20,$20,1

	j	$L570
	move	$20,$0

$L566:
	bne	$2,$23,$L567
	andi	$2,$20,0x1

	beq	$2,$0,$L567
	addiu	$20,$20,1

$L568:
	slt	$2,$20,4
	beq	$2,$0,$L569
	move	$4,$22

	j	$L578
	move	$5,$8

$L567:
	move	$20,$0
$L570:
	move	$4,$22
	move	$5,$8
$L578:
	move	$6,$7
	sw	$3,24($sp)
	sw	$7,20($sp)
	jal	memcmp
	sw	$8,16($sp)

	addiu	$22,$22,1
	lw	$3,24($sp)
	lw	$7,20($sp)
	beq	$2,$0,$L571
	lw	$8,16($sp)

	j	$L565
	addiu	$21,$21,1

$L571:
	addu	$21,$21,$7
	sw	$21,0($18)
	addu	$17,$17,$21
	move	$2,$0
	j	$L572
	li	$3,13			# 0xd

$L573:
	bne	$5,$0,$L569
	addiu	$17,$17,1

	addiu	$2,$2,1
$L572:
	lb	$4,0($17)
	addu	$5,$2,$21
	bne	$4,$3,$L573
	sltu	$5,$19,$5

	sw	$2,0($16)
	j	$L574
	li	$2,1			# 0x1

$L565:
	sltu	$2,$3,$21
	beql	$2,$0,$L575
	lb	$2,0($22)

$L569:
	move	$2,$0
$L574:
	lw	$31,68($sp)
	lw	$fp,64($sp)
	lw	$23,60($sp)
	lw	$22,56($sp)
	lw	$21,52($sp)
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	j	$31
	addiu	$sp,$sp,72

	.set	macro
	.set	reorder
	.end	T.1337
	.size	T.1337, .-T.1337
	.section	.rodata.str1.4
	.align	2
$LC52:
	.ascii	"GET \000"
	.align	2
$LC53:
	.ascii	"Host: \000"
	.section	.text.rtl_url_filter_fastpath,"ax",@progbits
	.align	2
	.globl	rtl_url_filter_fastpath
	.set	nomips16
	.ent	rtl_url_filter_fastpath
	.type	rtl_url_filter_fastpath, @function
rtl_url_filter_fastpath:
	.frame	$sp,72,$31		# vars= 16, regs= 7/0, args= 24, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-72
	sw	$31,68($sp)
	sw	$21,64($sp)
	sw	$20,60($sp)
	sw	$19,56($sp)
	sw	$18,52($sp)
	sw	$17,48($sp)
	sw	$16,44($sp)
	move	$16,$4
	move	$4,$5
	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$18,$5
	.set	macro
	.set	reorder

	sltu	$2,$2,2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L582
	move	$17,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$16
	.set	macro
	.set	reorder

	lbu	$3,9($2)
	move	$19,$2
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L582
	move	$17,$0
	.set	macro
	.set	reorder

	lw	$21,0($19)
	lui	$2,%hi(kmalloc_caches+44)
	lw	$4,%lo(kmalloc_caches+44)($2)
	ext	$21,$21,24,4
	sll	$21,$21,2
	addu	$21,$19,$21
	lw	$17,12($21)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0
	.set	macro
	.set	reorder

	srl	$17,$17,28
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L580
	move	$16,$2
	.set	macro
	.set	reorder

	lw	$20,0($19)
	sll	$17,$17,2
	addu	$17,$21,$17
	lui	$5,%hi($LC52)
	addiu	$5,$5,%lo($LC52)
	move	$4,$17
	li	$6,4			# 0x4
	ext	$20,$20,24,4
	.set	noreorder
	.set	nomacro
	jal	memcmp
	lhu	$19,2($19)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L585
	li	$2,-8			# 0xfffffffffffffff8
	.set	macro
	.set	reorder

	mul	$3,$20,$2
	lui	$6,%hi($LC53)
	addiu	$2,$sp,28
	addiu	$6,$6,%lo($LC53)
	move	$4,$17
	li	$7,6			# 0x6
	sw	$2,20($sp)
	addu	$20,$3,$19
	move	$5,$20
	addiu	$19,$sp,24
	.set	noreorder
	.set	nomacro
	jal	T.1337
	sw	$19,16($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L585
	lw	$4,28($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L584
	lw	$3,24($sp)
	.set	macro
	.set	reorder

	addu	$3,$17,$3
	move	$2,$16
 #APP
 # 52 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($3)
	subu	$4,1
	sb	$1,($2)
	beqz	$1,2f
	addiu	$2,1
	bnez	$4,1b
	addiu	$3,1
2:
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
$L584:
	lw	$2,28($sp)
	lui	$6,%hi($LC52)
	move	$5,$20
	addu	$2,$16,$2
	sb	$0,0($2)
	addiu	$6,$6,%lo($LC52)
	addiu	$2,$sp,32
	move	$4,$17
	li	$7,4			# 0x4
	sw	$19,16($sp)
	.set	noreorder
	.set	nomacro
	jal	T.1337
	sw	$2,20($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L585
	lw	$3,32($sp)
	.set	macro
	.set	reorder

	addiu	$3,$3,-9
	.set	noreorder
	.set	nomacro
	bgtz	$3,$L586
	sw	$3,32($sp)
	.set	macro
	.set	reorder

$L585:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L582
	move	$17,$0
	.set	macro
	.set	reorder

$L586:
	lw	$2,28($sp)
	lw	$4,24($sp)
	addu	$2,$16,$2
	addu	$17,$17,$4
 #APP
 # 52 "/home/winfred_wang/trunk_code/branch-rtl819x-sdk-v3.4.9_backup/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($17)
	subu	$3,1
	sb	$1,($2)
	beqz	$1,2f
	addiu	$2,1
	bnez	$3,1b
	addiu	$17,1
2:
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	lw	$2,32($sp)
	lw	$3,28($sp)
	move	$4,$16
	addu	$2,$16,$2
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	jal	strlen
	sb	$0,0($2)
	.set	macro
	.set	reorder

	move	$4,$18
	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$17,$2
	.set	macro
	.set	reorder

	sltu	$17,$17,$2
	.set	noreorder
	.set	nomacro
	bne	$17,$0,$L588
	move	$20,$2
	.set	macro
	.set	reorder

	addu	$19,$16,$2
	.set	noreorder
	.set	nomacro
	j	$L589
	move	$17,$16
	.set	macro
	.set	reorder

$L591:
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$19,$19,1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L589
	addiu	$17,$17,1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L590
	li	$17,1			# 0x1
	.set	macro
	.set	reorder

$L589:
	lb	$2,0($19)
	move	$4,$17
	move	$5,$18
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L591
	move	$6,$20
	.set	macro
	.set	reorder

$L588:
	move	$17,$0
$L590:
	lui	$6,%hi($LC27)
	lui	$4,%hi(log_info)
	addiu	$6,$6,%lo($LC27)
	addiu	$4,$4,%lo(log_info)
	li	$5,32			# 0x20
	.set	noreorder
	.set	nomacro
	jal	snprintf
	move	$7,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$2,%hi(url_filter_mode)
	lw	$2,%lo(url_filter_mode)($2)
	li	$3,1			# 0x1
	.set	noreorder
	.set	nomacro
	bnel	$2,$3,$L595
	move	$2,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L592
	lui	$3,%hi(url_filter_flag)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L582
	li	$17,1			# 0x1
	.set	macro
	.set	reorder

$L592:
	.set	noreorder
	.set	nomacro
	j	$L582
	sw	$2,%lo(url_filter_flag)($3)
	.set	macro
	.set	reorder

$L580:
	move	$17,$0
$L582:
	move	$2,$17
$L595:
	lw	$31,68($sp)
	lw	$21,64($sp)
	lw	$20,60($sp)
	lw	$19,56($sp)
	lw	$18,52($sp)
	lw	$17,48($sp)
	lw	$16,44($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,72
	.set	macro
	.set	reorder

	.end	rtl_url_filter_fastpath
	.size	rtl_url_filter_fastpath, .-rtl_url_filter_fastpath
	.globl	filter_tlb
	.section	.data.filter_tlb,"aw",@progbits
	.align	2
	.type	filter_tlb, @object
	.size	filter_tlb, 40
filter_tlb:
	.word	0
	.word	0
	.word	1
	.word	rtl_ip_range_filter_fastpath
	.word	2
	.word	rtl_mac_filter_fastpath
	.word	3
	.word	rtl_url_filter_fastpath
	.word	4
	.word	rtl_schedule_filter_fastpath
	.globl	filter_table_proc_fops
	.section	.data.filter_table_proc_fops,"aw",@progbits
	.align	2
	.type	filter_table_proc_fops, @object
	.size	filter_table_proc_fops, 108
filter_table_proc_fops:
	.space	4
	.word	seq_lseek
	.word	seq_read
	.word	filter_table_single_write
	.space	28
	.word	filter_table_single_open
	.space	4
	.word	single_release
	.space	52
	.globl	dos_proc_fops
	.section	.data.dos_proc_fops,"aw",@progbits
	.align	2
	.type	dos_proc_fops, @object
	.size	dos_proc_fops, 108
dos_proc_fops:
	.space	4
	.word	seq_lseek
	.word	seq_read
	.word	dos_single_write
	.space	28
	.word	dos_single_open
	.space	4
	.word	single_release
	.space	52
	.globl	fast_path_cusRsp401_func
	.section	.bss.fast_path_cusRsp401_func,"aw",@nobits
	.align	2
	.type	fast_path_cusRsp401_func, @object
	.size	fast_path_cusRsp401_func, 4
fast_path_cusRsp401_func:
	.space	4
	.globl	fast_path_cusRspTCPFinAck_func
	.section	.bss.fast_path_cusRspTCPFinAck_func,"aw",@nobits
	.align	2
	.type	fast_path_cusRspTCPFinAck_func, @object
	.size	fast_path_cusRspTCPFinAck_func, 4
fast_path_cusRspTCPFinAck_func:
	.space	4
	.globl	fast_path_cusRspTCPEndAck_func
	.section	.bss.fast_path_cusRspTCPEndAck_func,"aw",@nobits
	.align	2
	.type	fast_path_cusRspTCPEndAck_func, @object
	.size	fast_path_cusRspTCPEndAck_func, 4
fast_path_cusRspTCPEndAck_func:
	.space	4
	.section	.bss.item,"aw",@nobits
	.align	2
	.type	item, @object
	.size	item, 4
item:
	.space	4
	.section	.bss.item_backup,"aw",@nobits
	.align	2
	.type	item_backup, @object
	.size	item_backup, 4
item_backup:
	.space	4
	.section	.bss.block_time,"aw",@nobits
	.align	2
	.type	block_time, @object
	.size	block_time, 4
block_time:
	.space	4
	.section	.bss.log_enable,"aw",@nobits
	.align	2
	.type	log_enable, @object
	.size	log_enable, 4
log_enable:
	.space	4
	.section	.bss.url_filter_mode,"aw",@nobits
	.align	2
	.type	url_filter_mode, @object
	.size	url_filter_mode, 4
url_filter_mode:
	.space	4
	.section	.bss.res3,"aw",@nobits
	.align	2
	.type	res3, @object
	.size	res3, 4
res3:
	.space	4
	.section	.bss.none_dos_drop_pkt_cnt,"aw",@nobits
	.align	2
	.type	none_dos_drop_pkt_cnt, @object
	.size	none_dos_drop_pkt_cnt, 4
none_dos_drop_pkt_cnt:
	.space	4
	.section	.bss.url_count,"aw",@nobits
	.type	url_count, @object
	.size	url_count, 1
url_count:
	.space	1
	.section	.bss.block_count,"aw",@nobits
	.align	2
	.type	block_count, @object
	.size	block_count, 4
block_count:
	.space	4
	.section	.bss.block,"aw",@nobits
	.align	2
	.type	block, @object
	.size	block, 4
block:
	.space	4
	.section	.bss.url_cache,"aw",@nobits
	.align	2
	.type	url_cache, @object
	.size	url_cache, 20
url_cache:
	.space	20
	.section	.bss.endStateClientPort,"aw",@nobits
	.align	1
	.type	endStateClientPort, @object
	.size	endStateClientPort, 2
endStateClientPort:
	.space	2
	.section	.bss.endStateClientSeq,"aw",@nobits
	.align	2
	.type	endStateClientSeq, @object
	.size	endStateClientSeq, 4
endStateClientSeq:
	.space	4
	.section	.bss.finStateClientPort,"aw",@nobits
	.align	1
	.type	finStateClientPort, @object
	.size	finStateClientPort, 2
finStateClientPort:
	.space	2
	.section	.bss.finStateClientSeq,"aw",@nobits
	.align	2
	.type	finStateClientSeq, @object
	.size	finStateClientSeq, 4
finStateClientSeq:
	.space	4
	.section	.bss.block_message,"aw",@nobits
	.align	2
	.type	block_message, @object
	.size	block_message, 4
block_message:
	.space	4
	.section	.bss.res12,"aw",@nobits
	.align	2
	.type	res12, @object
	.size	res12, 4
res12:
	.space	4
	.section	.bss.url_filter_flag,"aw",@nobits
	.align	2
	.type	url_filter_flag, @object
	.size	url_filter_flag, 4
url_filter_flag:
	.space	4
	.section	.bss.get_info_from_usr_space,"aw",@nobits
	.align	2
	.type	get_info_from_usr_space, @object
	.size	get_info_from_usr_space, 1024
get_info_from_usr_space:
	.space	1024
	.section	.bss.log_info,"aw",@nobits
	.align	2
	.type	log_info, @object
	.size	log_info, 32
log_info:
	.space	32
	.section	.bss.filter_rule_num,"aw",@nobits
	.align	2
	.type	filter_rule_num, @object
	.size	filter_rule_num, 4
filter_rule_num:
	.space	4
	.section	.bss.LogFlag,"aw",@nobits
	.align	2
	.type	LogFlag, @object
	.size	LogFlag, 4
LogFlag:
	.space	4
	.section	.bss.ConnectedIp,"aw",@nobits
	.align	2
	.type	ConnectedIp, @object
	.size	ConnectedIp, 1024
ConnectedIp:
	.space	1024
	.section	.bss.dos_pkt,"aw",@nobits
	.align	2
	.type	dos_pkt, @object
	.size	dos_pkt, 36864
dos_pkt:
	.space	36864
	.section	.bss.cur_p_pkt,"aw",@nobits
	.align	2
	.type	cur_p_pkt, @object
	.size	cur_p_pkt, 4
cur_p_pkt:
	.space	4
	.section	.bss.dos_timer,"aw",@nobits
	.align	2
	.type	dos_timer, @object
	.size	dos_timer, 28
dos_timer:
	.space	28
	.section	.bss._tcpDosScanBitmap,"aw",@nobits
	.align	2
	.type	_tcpDosScanBitmap, @object
	.size	_tcpDosScanBitmap, 64
_tcpDosScanBitmap:
	.space	64
	.section	.bss.op_mode,"aw",@nobits
	.type	op_mode, @object
	.size	op_mode, 1
op_mode:
	.space	1
	.section	.bss.whole_syn_threshold,"aw",@nobits
	.align	2
	.type	whole_syn_threshold, @object
	.size	whole_syn_threshold, 4
whole_syn_threshold:
	.space	4
	.section	.bss.whole_fin_threshold,"aw",@nobits
	.align	2
	.type	whole_fin_threshold, @object
	.size	whole_fin_threshold, 4
whole_fin_threshold:
	.space	4
	.section	.bss.whole_udp_threshold,"aw",@nobits
	.align	2
	.type	whole_udp_threshold, @object
	.size	whole_udp_threshold, 4
whole_udp_threshold:
	.space	4
	.section	.bss.whole_icmp_threshold,"aw",@nobits
	.align	2
	.type	whole_icmp_threshold, @object
	.size	whole_icmp_threshold, 4
whole_icmp_threshold:
	.space	4
	.section	.bss.per_syn_threshold,"aw",@nobits
	.align	2
	.type	per_syn_threshold, @object
	.size	per_syn_threshold, 4
per_syn_threshold:
	.space	4
	.section	.bss.per_fin_threshold,"aw",@nobits
	.align	2
	.type	per_fin_threshold, @object
	.size	per_fin_threshold, 4
per_fin_threshold:
	.space	4
	.section	.bss.per_udp_threshold,"aw",@nobits
	.align	2
	.type	per_udp_threshold, @object
	.size	per_udp_threshold, 4
per_udp_threshold:
	.space	4
	.section	.bss.per_icmp_threshold,"aw",@nobits
	.align	2
	.type	per_icmp_threshold, @object
	.size	per_icmp_threshold, 4
per_icmp_threshold:
	.space	4
	.section	.bss.lan_addr,"aw",@nobits
	.align	2
	.type	lan_addr, @object
	.size	lan_addr, 4
lan_addr:
	.space	4
	.section	.bss.lan_mask,"aw",@nobits
	.align	2
	.type	lan_mask, @object
	.size	lan_mask, 4
lan_mask:
	.space	4
	.section	.bss.attack_saddr,"aw",@nobits
	.align	2
	.type	attack_saddr, @object
	.size	attack_saddr, 60
attack_saddr:
	.space	60
	.section	.bss.attack_daddr,"aw",@nobits
	.align	2
	.type	attack_daddr, @object
	.size	attack_daddr, 60
attack_daddr:
	.space	60
	.section	.bss.dos_flag,"aw",@nobits
	.align	2
	.type	dos_flag, @object
	.size	dos_flag, 80
dos_flag:
	.space	80
	.globl	table_list_head
	.section	.bss.table_list_head,"aw",@nobits
	.align	2
	.type	table_list_head, @object
	.size	table_list_head, 32
table_list_head:
	.space	32
	.globl	wan_dev
	.section	.bss.wan_dev,"aw",@nobits
	.align	2
	.type	wan_dev, @object
	.size	wan_dev, 4
wan_dev:
	.space	4
	.globl	wan_ppp_dev
	.section	.bss.wan_ppp_dev,"aw",@nobits
	.align	2
	.type	wan_ppp_dev, @object
	.size	wan_ppp_dev, 4
wan_ppp_dev:
	.space	4
	.globl	___strtok
	.section	.bss.___strtok,"aw",@nobits
	.align	2
	.type	___strtok, @object
	.size	___strtok, 4
___strtok:
	.space	4
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 1459) 4.4.7"
