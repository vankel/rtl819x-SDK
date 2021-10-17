	.file	1 "fastpath_core.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.cache_timeout,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	cache_timeout
	.type	cache_timeout, @function
cache_timeout:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	lbu	$2,0($4)
	nop
	beq	$2,$0,$L5
	nop

	lw	$2,4($4)
	lw	$5,8($4)
	lhu	$3,2($4)
	sb	$0,0($4)
	xor	$5,$5,$2
	lbu	$2,1($4)
	srl	$6,$5,16
	xor	$3,$3,$5
	xor	$3,$3,$6
	srl	$5,$3,8
	xor	$2,$3,$2
	lui	$3,%hi(max_udp_frag_entry)
	xor	$2,$2,$5
	lw	$5,%lo(max_udp_frag_entry)($3)
	lw	$6,44($4)
	addiu	$5,$5,-1
	and	$2,$2,$5
	lw	$5,48($4)
	beq	$6,$0,$L3
	lui	$3,%hi(udp_cache_table)

	j	$L4
	sw	$5,48($6)

$L3:
	li	$7,12			# 0xc
	mult	$2,$7
	lw	$6,%lo(udp_cache_table)($3)
	nop
	lw	$6,0($6)
	mflo	$7
	addu	$6,$6,$7
	sw	$5,4($6)
$L4:
	lw	$5,48($4)
	lw	$6,44($4)
	nop
	sw	$6,0($5)
	li	$5,12			# 0xc
	mult	$2,$5
	lw	$3,%lo(udp_cache_table)($3)
	nop
	lw	$3,0($3)
	mflo	$2
	addu	$2,$3,$2
	lw	$3,8($2)
	nop
	addiu	$3,$3,-1
	sw	$3,8($2)
	lui	$2,%hi(udp_cache_list_free)
	addiu	$2,$2,%lo(udp_cache_list_free)
	lw	$3,4($2)
	sw	$0,52($4)
	sw	$3,56($4)
	sw	$4,0($3)
	lw	$3,8($2)
	addiu	$4,$4,52
	sw	$4,4($2)
	addiu	$3,$3,1
	sw	$3,8($2)
$L5:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	cache_timeout
	.size	cache_timeout, .-cache_timeout
	.section	.text.negative_cache_timeout,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	negative_cache_timeout
	.type	negative_cache_timeout, @function
negative_cache_timeout:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	lw	$2,4($4)
	lw	$5,8($4)
	lbu	$3,1($4)
	sb	$0,0($4)
	xor	$5,$5,$2
	lhu	$2,2($4)
	srl	$6,$5,16
	xor	$2,$2,$5
	xor	$2,$2,$6
	srl	$5,$2,8
	xor	$3,$2,$3
	lui	$2,%hi(max_udp_frag_entry)
	xor	$3,$3,$5
	lw	$5,%lo(max_udp_frag_entry)($2)
	lw	$6,40($4)
	addiu	$5,$5,-1
	and	$3,$3,$5
	lw	$5,44($4)
	beq	$6,$0,$L7
	lui	$2,%hi(negative_cache_table)

	j	$L8
	sw	$5,44($6)

$L7:
	li	$6,12			# 0xc
	mult	$3,$6
	lw	$7,%lo(negative_cache_table)($2)
	mflo	$6
	addu	$6,$7,$6
	sw	$5,4($6)
$L8:
	lw	$5,44($4)
	lw	$6,40($4)
	nop
	sw	$6,0($5)
	li	$5,12			# 0xc
	mult	$3,$5
	lw	$2,%lo(negative_cache_table)($2)
	mflo	$3
	addu	$3,$2,$3
	lw	$2,8($3)
	nop
	addiu	$2,$2,-1
	sw	$2,8($3)
	lui	$2,%hi(negative_cache_list_free)
	addiu	$2,$2,%lo(negative_cache_list_free)
	lw	$3,4($2)
	sw	$0,48($4)
	sw	$3,52($4)
	sw	$4,0($3)
	lw	$3,8($2)
	addiu	$4,$4,48
	sw	$4,4($2)
	addiu	$3,$3,1
	j	$31
	sw	$3,8($2)

	.set	macro
	.set	reorder
	.end	negative_cache_timeout
	.size	negative_cache_timeout, .-negative_cache_timeout
	.section	.text.mark_path_invalid,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	mark_path_invalid
	.type	mark_path_invalid, @function
mark_path_invalid:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	srl	$9,$4,8
	xor	$9,$9,$4
	srl	$2,$6,16
	xor	$9,$9,$6
	xor	$9,$9,$2
	lhu	$3,18($sp)
	srl	$2,$5,4
	xor	$9,$9,$2
	lui	$2,%hi(path_table_list_max)
	xor	$9,$9,$7
	lw	$2,%lo(path_table_list_max)($2)
	lui	$8,%hi(table_path)
	xor	$9,$9,$3
	lw	$10,%lo(table_path)($8)
	srl	$11,$9,12
	xor	$9,$11,$9
	addiu	$2,$2,-1
	and	$11,$9,$2
	lw	$9,0($10)
	li	$10,12			# 0xc
	mult	$11,$10
	mflo	$10
	addu	$9,$9,$10
	lw	$9,0($9)
	j	$L10
	li	$10,255			# 0xff

$L14:
	lw	$11,12($9)
	nop
	bne	$11,$5,$L11
	nop

	lw	$11,8($9)
	nop
	bne	$11,$7,$L11
	nop

	lw	$11,16($9)
	nop
	bne	$11,$4,$L11
	nop

	lw	$11,20($9)
	nop
	bne	$11,$6,$L11
	nop

	lbu	$11,0($9)
	nop
	bne	$11,$10,$L11
	nop

	lbu	$11,2($9)
	li	$10,1			# 0x1
	bne	$11,$10,$L12
	sb	$0,0($9)

	lw	$10,24($9)
	lhu	$9,30($9)
	j	$L13
	nop

$L12:
	lw	$10,32($9)
	lhu	$9,38($9)
	j	$L13
	nop

$L11:
	lw	$9,64($9)
	nop
$L10:
	bne	$9,$0,$L14
	nop

$L23:
	j	$31
	nop

$L13:
	beq	$10,$0,$L23
	li	$12,1			# 0x1

	bne	$11,$12,$L16
	srl	$11,$4,16

	srl	$11,$6,8
	xor	$11,$11,$6
	xor	$3,$11,$3
	srl	$11,$7,4
	xor	$3,$3,$11
	xor	$3,$3,$10
	xor	$9,$3,$9
	srl	$10,$10,16
	xor	$9,$9,$10
	srl	$10,$9,12
	j	$L21
	xor	$9,$10,$9

$L16:
	xor	$11,$11,$4
	xor	$11,$11,$5
	xor	$3,$11,$3
	xor	$3,$3,$10
	srl	$10,$10,8
	xor	$10,$3,$10
	srl	$9,$9,4
	xor	$9,$10,$9
	srl	$3,$9,12
	xor	$9,$3,$9
$L21:
	and	$2,$9,$2
	lw	$3,%lo(table_path)($8)
	li	$8,12			# 0xc
	mult	$2,$8
	lw	$3,0($3)
	mflo	$2
	addu	$3,$3,$2
	lw	$2,0($3)
	j	$L18
	li	$3,255			# 0xff

$L20:
	lw	$8,24($2)
	nop
	bne	$8,$6,$L19
	nop

	lw	$8,28($2)
	nop
	bne	$8,$7,$L19
	nop

	lw	$8,32($2)
	nop
	bne	$8,$4,$L19
	nop

	lw	$8,36($2)
	nop
	bne	$8,$5,$L19
	nop

	lbu	$8,0($2)
	nop
	bne	$8,$3,$L19
	nop

	sb	$0,0($2)
$L19:
	lw	$2,64($2)
	nop
$L18:
	bne	$2,$0,$L20
	nop

	j	$31
	nop

	.set	macro
	.set	reorder
	.end	mark_path_invalid
	.size	mark_path_invalid, .-mark_path_invalid
	.section	.text.udp_fragCache_init,"ax",@progbits
	.align	2
	.globl	udp_fragCache_init
	.set	nomips16
	.ent	udp_fragCache_init
	.type	udp_fragCache_init, @function
udp_fragCache_init:
	.frame	$sp,48,$31		# vars= 8, regs= 6/0, args= 16, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	lui	$19,%hi(kmalloc_caches)
	addiu	$2,$19,%lo(kmalloc_caches)
	move	$16,$4
	lw	$4,20($2)
	li	$5,32			# 0x20
	jal	kmem_cache_alloc
	lui	$17,%hi(udp_cache_table)

	move	$18,$2
	sw	$2,%lo(udp_cache_table)($17)
	beq	$18,$0,$L25
	move	$2,$0

	li	$4,12			# 0xc
	mult	$16,$4
	lui	$20,%hi(udp_cache_list_free)
	addiu	$2,$20,%lo(udp_cache_list_free)
	sw	$0,8($2)
	sw	$2,4($2)
	li	$5,32			# 0x20
	lui	$2,%hi(max_udp_frag_entry)
	sw	$0,%lo(udp_cache_list_free)($20)
	sw	$16,%lo(max_udp_frag_entry)($2)
	jal	__kmalloc
	mflo	$4

	sw	$2,0($18)
	lw	$2,%lo(udp_cache_table)($17)
	nop
	lw	$3,0($2)
	nop
	beq	$3,$0,$L25
	li	$2,-1			# 0xffffffffffffffff

	move	$2,$0
	j	$L26
	li	$4,12			# 0xc

$L27:
	lw	$3,%lo(udp_cache_table)($17)
	addiu	$2,$2,1
	lw	$6,0($3)
	mflo	$5
	addu	$6,$6,$5
	sw	$0,8($6)
	lw	$3,0($3)
	nop
	addu	$3,$3,$5
	sw	$0,0($3)
	lw	$3,%lo(udp_cache_table)($17)
	nop
	lw	$3,0($3)
	nop
	addu	$5,$3,$5
	sw	$5,4($5)
$L26:
	slt	$3,$2,$16
	bne	$3,$0,$L27
	mult	$2,$4

	lui	$18,%hi(cache_timeout)
	move	$17,$0
	j	$L28
	addiu	$18,$18,%lo(cache_timeout)

$L29:
	addiu	$2,$19,%lo(kmalloc_caches)
	lw	$4,24($2)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	beq	$2,$0,$L32
	addiu	$4,$2,16

	move	$5,$0
	move	$6,$0
	move	$7,$0
	jal	init_timer_key
	sw	$2,16($sp)

	addiu	$3,$20,%lo(udp_cache_list_free)
	lw	$2,16($sp)
	lw	$4,4($3)
	addiu	$17,$17,1
	sw	$2,36($2)
	sw	$18,32($2)
	sw	$0,52($2)
	sw	$4,56($2)
	sw	$2,0($4)
	addiu	$2,$2,52
	sw	$2,4($3)
	lw	$2,8($3)
	nop
	addiu	$2,$2,1
	sw	$2,8($3)
$L28:
	slt	$2,$17,$16
	bne	$2,$0,$L29
	li	$2,1			# 0x1

	j	$L25
	nop

$L32:
	move	$2,$0
$L25:
	lw	$31,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	udp_fragCache_init
	.size	udp_fragCache_init, .-udp_fragCache_init
	.section	.text.negative_fragCache_init,"ax",@progbits
	.align	2
	.globl	negative_fragCache_init
	.set	nomips16
	.ent	negative_fragCache_init
	.type	negative_fragCache_init, @function
negative_fragCache_init:
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
	lui	$18,%hi(kmalloc_caches)
	addiu	$2,$18,%lo(kmalloc_caches)
	lw	$4,40($2)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	move	$4,$2
	lui	$3,%hi(negative_cache_table)
	sw	$2,%lo(negative_cache_table)($3)
	beq	$4,$0,$L34
	move	$2,$0

	lui	$19,%hi(negative_cache_list_free)
	addiu	$2,$19,%lo(negative_cache_list_free)
	sw	$0,8($2)
	sw	$2,4($2)
	sw	$0,%lo(negative_cache_list_free)($19)
	move	$2,$0
	li	$5,12			# 0xc
	li	$4,64			# 0x40
$L35:
	mult	$2,$5
	lw	$7,%lo(negative_cache_table)($3)
	addiu	$2,$2,1
	mflo	$6
	addu	$7,$7,$6
	sw	$0,0($7)
	sw	$0,8($7)
	lw	$7,%lo(negative_cache_table)($3)
	nop
	addu	$6,$7,$6
	bne	$2,$4,$L35
	sw	$6,4($6)

	lui	$17,%hi(negative_cache_timeout)
	li	$16,64			# 0x40
	addiu	$17,$17,%lo(negative_cache_timeout)
$L36:
	addiu	$2,$18,%lo(kmalloc_caches)
	lw	$4,24($2)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	beq	$2,$0,$L38
	addiu	$4,$2,12

	move	$5,$0
	move	$6,$0
	move	$7,$0
	jal	init_timer_key
	sw	$2,16($sp)

	addiu	$3,$19,%lo(negative_cache_list_free)
	lw	$2,16($sp)
	lw	$4,4($3)
	addiu	$16,$16,-1
	sw	$2,32($2)
	sw	$17,28($2)
	sw	$0,48($2)
	sw	$4,52($2)
	sw	$2,0($4)
	addiu	$2,$2,48
	sw	$2,4($3)
	lw	$2,8($3)
	nop
	addiu	$2,$2,1
	bne	$16,$0,$L36
	sw	$2,8($3)

	j	$L34
	li	$2,1			# 0x1

$L38:
	move	$2,$0
$L34:
	lw	$31,44($sp)
	lw	$19,40($sp)
	lw	$18,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	j	$31
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	negative_fragCache_init
	.size	negative_fragCache_init, .-negative_fragCache_init
	.section	.text.rtk_addArp,"ax",@progbits
	.align	2
	.globl	rtk_addArp
	.set	nomips16
	.ent	rtk_addArp
	.type	rtk_addArp, @function
rtk_addArp:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0

	.set	macro
	.set	reorder
	.end	rtk_addArp
	.size	rtk_addArp, .-rtk_addArp
	.section	.text.rtk_modifyArp,"ax",@progbits
	.align	2
	.globl	rtk_modifyArp
	.set	nomips16
	.ent	rtk_modifyArp
	.type	rtk_modifyArp, @function
rtk_modifyArp:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0

	.set	macro
	.set	reorder
	.end	rtk_modifyArp
	.size	rtk_modifyArp, .-rtk_modifyArp
	.section	.text.rtk_delArp,"ax",@progbits
	.align	2
	.globl	rtk_delArp
	.set	nomips16
	.ent	rtk_delArp
	.type	rtk_delArp, @function
rtk_delArp:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	li	$2,-4			# 0xfffffffffffffffc

	.set	macro
	.set	reorder
	.end	rtk_delArp
	.size	rtk_delArp, .-rtk_delArp
	.section	.text.rtk_addRoute,"ax",@progbits
	.align	2
	.globl	rtk_addRoute
	.set	nomips16
	.ent	rtk_addRoute
	.type	rtk_addRoute, @function
rtk_addRoute:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0

	.set	macro
	.set	reorder
	.end	rtk_addRoute
	.size	rtk_addRoute, .-rtk_addRoute
	.section	.text.rtk_modifyRoute,"ax",@progbits
	.align	2
	.globl	rtk_modifyRoute
	.set	nomips16
	.ent	rtk_modifyRoute
	.type	rtk_modifyRoute, @function
rtk_modifyRoute:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0

	.set	macro
	.set	reorder
	.end	rtk_modifyRoute
	.size	rtk_modifyRoute, .-rtk_modifyRoute
	.section	.text.rtk_delRoute,"ax",@progbits
	.align	2
	.globl	rtk_delRoute
	.set	nomips16
	.ent	rtk_delRoute
	.type	rtk_delRoute, @function
rtk_delRoute:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	li	$2,-4			# 0xfffffffffffffffc

	.set	macro
	.set	reorder
	.end	rtk_delRoute
	.size	rtk_delRoute, .-rtk_delRoute
	.section	.text.rtk_addSession,"ax",@progbits
	.align	2
	.globl	rtk_addSession
	.set	nomips16
	.ent	rtk_addSession
	.type	rtk_addSession, @function
rtk_addSession:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0

	.set	macro
	.set	reorder
	.end	rtk_addSession
	.size	rtk_addSession, .-rtk_addSession
	.section	.text.rtk_delSession,"ax",@progbits
	.align	2
	.globl	rtk_delSession
	.set	nomips16
	.ent	rtk_delSession
	.type	rtk_delSession, @function
rtk_delSession:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	j	$31
	move	$2,$0

	.set	macro
	.set	reorder
	.end	rtk_delSession
	.size	rtk_delSession, .-rtk_delSession
	.section	.text.rtk_delNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_delNaptConnection
	.set	nomips16
	.ent	rtk_delNaptConnection
	.type	rtk_delNaptConnection, @function
rtk_delNaptConnection:
	.frame	$sp,40,$31		# vars= 8, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$17,32($sp)
	sw	$16,28($sp)
	lw	$3,0($4)
	move	$16,$4
	li	$2,17			# 0x11
	xori	$3,$3,0x2
	li	$17,6			# 0x6
	jal	arch_local_irq_save
	movn	$17,$2,$3

	lw	$4,20($16)
	jal	filter_delconnect
	sw	$2,16($sp)

	lw	$4,4($16)
	lw	$7,8($16)
	lw	$3,20($16)
	lw	$6,24($16)
	xor	$9,$7,$4
	lw	$5,0($16)
	xor	$9,$9,$3
	srl	$8,$4,16
	xor	$9,$9,$6
	xor	$9,$9,$8
	srl	$8,$3,16
	xor	$9,$9,$8
	andi	$8,$5,0xffff
	xor	$9,$9,$8
	srl	$8,$9,12
	xor	$9,$8,$9
	lui	$8,%hi(napt_table_list_max)
	lw	$8,%lo(napt_table_list_max)($8)
	lui	$10,%hi(table_napt)
	lw	$2,16($sp)
	addiu	$8,$8,-1
	and	$9,$9,$8
	li	$8,12			# 0xc
	mult	$9,$8
	lw	$8,%lo(table_napt)($10)
	nop
	lw	$8,0($8)
	mflo	$9
	addu	$8,$8,$9
	lw	$8,0($8)
	j	$L51
	nop

$L75:
	lw	$11,4($8)
	nop
	bne	$11,$5,$L52
	nop

	lw	$11,8($8)
	nop
	bne	$11,$4,$L52
	nop

	lw	$11,12($8)
	nop
	bne	$11,$7,$L52
	nop

	lw	$12,16($8)
	lw	$11,12($16)
	nop
	bne	$12,$11,$L52
	nop

	lw	$12,20($8)
	lw	$11,16($16)
	nop
	bne	$12,$11,$L52
	nop

	lw	$11,24($8)
	nop
	bne	$11,$3,$L52
	nop

	lw	$11,28($8)
	nop
	bne	$11,$6,$L52
	nop

	lw	$4,36($8)
	sb	$0,0($8)
	lw	$3,40($8)
	beq	$4,$0,$L53
	nop

	j	$L54
	sw	$3,40($4)

$L53:
	lw	$4,%lo(table_napt)($10)
	nop
	lw	$4,0($4)
	nop
	addu	$4,$4,$9
	sw	$3,4($4)
$L54:
	lw	$3,40($8)
	lw	$4,36($8)
	nop
	sw	$4,0($3)
	lw	$3,%lo(table_napt)($10)
	nop
	lw	$3,0($3)
	nop
	addu	$9,$3,$9
	lw	$3,8($9)
	nop
	addiu	$3,$3,-1
	sw	$3,8($9)
	lw	$5,44($8)
	lw	$4,48($8)
	beq	$5,$0,$L55
	lui	$3,%hi(napt_list_inuse)

	j	$L56
	sw	$4,48($5)

$L55:
	addiu	$5,$3,%lo(napt_list_inuse)
	sw	$4,4($5)
$L56:
	lw	$4,48($8)
	lw	$5,44($8)
	addiu	$3,$3,%lo(napt_list_inuse)
	sw	$5,0($4)
	lw	$4,8($3)
	sw	$0,44($8)
	addiu	$4,$4,-1
	sw	$4,8($3)
	lui	$3,%hi(napt_list_free)
	addiu	$3,$3,%lo(napt_list_free)
	lw	$4,4($3)
	nop
	sw	$4,48($8)
	sw	$8,0($4)
	lw	$4,8($3)
	addiu	$8,$8,44
	sw	$8,4($3)
	addiu	$4,$4,1
	sw	$4,8($3)
	lw	$5,4($16)
	lw	$4,20($16)
	lw	$8,24($16)
	lw	$7,8($16)
	xor	$10,$4,$5
	srl	$3,$5,8
	xor	$10,$10,$8
	xor	$10,$10,$3
	srl	$3,$4,16
	xor	$10,$10,$3
	srl	$3,$7,4
	xor	$10,$10,$3
	lui	$3,%hi(path_table_list_max)
	lw	$6,%lo(path_table_list_max)($3)
	xor	$10,$10,$17
	srl	$3,$10,12
	xor	$10,$3,$10
	addiu	$6,$6,-1
	and	$10,$10,$6
	li	$3,12			# 0xc
	mult	$10,$3
	lui	$3,%hi(table_path)
	lw	$9,%lo(table_path)($3)
	nop
	lw	$9,0($9)
	mflo	$10
	addu	$9,$9,$10
	lw	$9,0($9)
	j	$L57
	li	$11,1			# 0x1

$L64:
	lw	$13,4($9)
	lw	$12,0($16)
	nop
	bne	$13,$12,$L58
	nop

	lbu	$12,2($9)
	nop
	bne	$12,$11,$L58
	nop

	lw	$12,16($9)
	nop
	bne	$12,$5,$L58
	nop

	lw	$12,12($9)
	nop
	bne	$12,$7,$L58
	nop

	lw	$13,24($9)
	lw	$12,12($16)
	nop
	bne	$13,$12,$L58
	nop

	lw	$13,28($9)
	lw	$12,16($16)
	nop
	bne	$13,$12,$L58
	nop

	lw	$12,20($9)
	nop
	bne	$12,$4,$L58
	nop

	lw	$12,8($9)
	nop
	bne	$12,$8,$L58
	nop

	lw	$5,64($9)
	sb	$0,0($9)
	lw	$4,68($9)
	beq	$5,$0,$L59
	nop

	j	$L60
	sw	$4,68($5)

$L59:
	lw	$5,%lo(table_path)($3)
	nop
	lw	$5,0($5)
	nop
	addu	$5,$5,$10
	sw	$4,4($5)
$L60:
	lw	$4,68($9)
	lw	$5,64($9)
	nop
	sw	$5,0($4)
	lw	$4,%lo(table_path)($3)
	nop
	lw	$4,0($4)
	nop
	addu	$10,$4,$10
	lw	$4,8($10)
	nop
	addiu	$4,$4,-1
	sw	$4,8($10)
	lw	$7,72($9)
	lw	$5,76($9)
	beq	$7,$0,$L61
	lui	$4,%hi(path_list_inuse)

	j	$L62
	sw	$5,76($7)

$L61:
	addiu	$7,$4,%lo(path_list_inuse)
	sw	$5,4($7)
$L62:
	lw	$5,76($9)
	lw	$7,72($9)
	addiu	$4,$4,%lo(path_list_inuse)
	sw	$7,0($5)
	lw	$5,8($4)
	sw	$0,72($9)
	addiu	$5,$5,-1
	sw	$5,8($4)
	lui	$4,%hi(path_list_free)
	addiu	$4,$4,%lo(path_list_free)
	lw	$5,4($4)
	nop
	sw	$5,76($9)
	sw	$9,0($5)
	lw	$5,8($4)
	addiu	$9,$9,72
	sw	$9,4($4)
	addiu	$5,$5,1
	j	$L63
	sw	$5,8($4)

$L58:
	lw	$9,64($9)
	nop
$L57:
	bne	$9,$0,$L64
	nop

$L63:
	lw	$5,20($16)
	lw	$4,12($16)
	lw	$8,16($16)
	lw	$7,24($16)
	xor	$9,$4,$5
	srl	$10,$5,8
	xor	$9,$9,$8
	xor	$9,$9,$10
	srl	$10,$4,16
	xor	$9,$9,$10
	srl	$10,$7,4
	xor	$9,$9,$10
	xor	$17,$9,$17
	srl	$9,$17,12
	xor	$17,$9,$17
	and	$6,$17,$6
	li	$9,12			# 0xc
	mult	$6,$9
	lw	$9,%lo(table_path)($3)
	nop
	lw	$9,0($9)
	mflo	$6
	addu	$9,$9,$6
	lw	$9,0($9)
	j	$L65
	li	$10,2			# 0x2

$L72:
	lw	$12,4($9)
	lw	$11,0($16)
	nop
	bne	$12,$11,$L66
	nop

	lbu	$11,2($9)
	nop
	bne	$11,$10,$L66
	nop

	lw	$11,20($9)
	nop
	bne	$11,$4,$L66
	nop

	lw	$11,8($9)
	nop
	bne	$11,$8,$L66
	nop

	lw	$11,24($9)
	nop
	bne	$11,$5,$L66
	nop

	lw	$11,28($9)
	nop
	bne	$11,$7,$L66
	nop

	lw	$12,32($9)
	lw	$11,4($16)
	nop
	bne	$12,$11,$L66
	nop

	lw	$12,36($9)
	lw	$11,8($16)
	nop
	bne	$12,$11,$L66
	nop

	lw	$5,64($9)
	sb	$0,0($9)
	lw	$4,68($9)
	beq	$5,$0,$L67
	nop

	j	$L68
	sw	$4,68($5)

$L67:
	lw	$5,%lo(table_path)($3)
	nop
	lw	$5,0($5)
	nop
	addu	$5,$5,$6
	sw	$4,4($5)
$L68:
	lw	$4,68($9)
	lw	$5,64($9)
	nop
	sw	$5,0($4)
	lw	$3,%lo(table_path)($3)
	nop
	lw	$3,0($3)
	nop
	addu	$6,$3,$6
	lw	$3,8($6)
	nop
	addiu	$3,$3,-1
	sw	$3,8($6)
	lw	$5,72($9)
	lw	$4,76($9)
	beq	$5,$0,$L69
	lui	$3,%hi(path_list_inuse)

	j	$L70
	sw	$4,76($5)

$L69:
	addiu	$5,$3,%lo(path_list_inuse)
	sw	$4,4($5)
$L70:
	lw	$4,76($9)
	lw	$5,72($9)
	addiu	$3,$3,%lo(path_list_inuse)
	sw	$5,0($4)
	lw	$4,8($3)
	sw	$0,72($9)
	addiu	$4,$4,-1
	sw	$4,8($3)
	lui	$3,%hi(path_list_free)
	addiu	$3,$3,%lo(path_list_free)
	lw	$4,4($3)
	nop
	sw	$4,76($9)
	sw	$9,0($4)
	lw	$4,8($3)
	addiu	$9,$9,72
	sw	$9,4($3)
	addiu	$4,$4,1
	j	$L71
	sw	$4,8($3)

$L66:
	lw	$9,64($9)
	nop
$L65:
	bne	$9,$0,$L72
	nop

$L71:
	jal	arch_local_irq_restore
	move	$4,$2

	j	$L74
	move	$2,$0

$L52:
	lw	$8,36($8)
	nop
$L51:
	bne	$8,$0,$L75
	nop

	jal	arch_local_irq_restore
	move	$4,$2

	li	$2,-4			# 0xfffffffffffffffc
$L74:
	lw	$31,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	rtk_delNaptConnection
	.size	rtk_delNaptConnection, .-rtk_delNaptConnection
	.section	.text.rtk_idleNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_idleNaptConnection
	.set	nomips16
	.ent	rtk_idleNaptConnection
	.type	rtk_idleNaptConnection, @function
rtk_idleNaptConnection:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lw	$9,0($4)
	lui	$2,%hi(jiffies)
	lw	$11,%lo(jiffies)($2)
	xori	$6,$9,0x2
	li	$2,17			# 0x11
	li	$3,6			# 0x6
	movn	$3,$2,$6
	lw	$7,4($4)
	lw	$2,20($4)
	lw	$6,24($4)
	lw	$10,8($4)
	xor	$8,$2,$7
	srl	$12,$7,8
	xor	$8,$8,$6
	xor	$8,$8,$12
	srl	$12,$2,16
	xor	$8,$8,$12
	srl	$12,$10,4
	xor	$8,$8,$12
	lui	$12,%hi(path_table_list_max)
	lw	$13,%lo(path_table_list_max)($12)
	xor	$8,$8,$3
	srl	$14,$8,12
	xor	$8,$14,$8
	addiu	$13,$13,-1
	and	$8,$8,$13
	li	$14,12			# 0xc
	mult	$8,$14
	lui	$12,%hi(table_path)
	lw	$12,%lo(table_path)($12)
	nop
	lw	$12,0($12)
	mflo	$8
	addu	$8,$12,$8
	lw	$14,0($8)
	li	$8,-16777216			# 0xffffffffff000000
	ori	$15,$8,0xff00
	j	$L80
	addiu	$8,$8,256

$L84:
	lw	$24,4($14)
	nop
	bne	$24,$9,$L81
	nop

	lw	$24,0($14)
	nop
	and	$24,$24,$15
	bne	$24,$8,$L81
	nop

	lw	$24,16($14)
	nop
	bne	$24,$7,$L81
	nop

	lw	$24,12($14)
	nop
	bne	$24,$10,$L81
	nop

	lw	$25,24($14)
	lw	$24,12($4)
	nop
	bne	$25,$24,$L81
	nop

	lw	$25,28($14)
	lw	$24,16($4)
	nop
	bne	$25,$24,$L81
	nop

	lw	$24,20($14)
	nop
	bne	$24,$2,$L81
	nop

	lw	$24,8($14)
	nop
	bne	$24,$6,$L81
	nop

	lw	$8,40($14)
	subu	$15,$11,$5
	subu	$15,$15,$8
	bgez	$15,$L82
	nop

	lw	$4,56($14)
	j	$L90
	addu	$5,$5,$8

$L81:
	lw	$14,64($14)
	nop
$L80:
	bne	$14,$0,$L84
	nop

$L82:
	lw	$8,12($4)
	lw	$4,16($4)
	srl	$15,$2,8
	xor	$14,$8,$2
	xor	$14,$14,$4
	xor	$14,$14,$15
	srl	$15,$8,16
	xor	$14,$14,$15
	srl	$15,$6,4
	xor	$14,$14,$15
	xor	$3,$14,$3
	srl	$14,$3,12
	xor	$3,$14,$3
	and	$13,$3,$13
	li	$14,12			# 0xc
	mult	$13,$14
	li	$3,-16777216			# 0xffffffffff000000
	mflo	$13
	addu	$12,$12,$13
	lw	$12,0($12)
	ori	$13,$3,0xff00
	j	$L85
	addiu	$3,$3,512

$L87:
	lw	$14,4($12)
	nop
	bne	$14,$9,$L86
	nop

	lw	$14,0($12)
	nop
	and	$14,$14,$13
	bne	$14,$3,$L86
	nop

	lw	$14,20($12)
	nop
	bne	$14,$8,$L86
	nop

	lw	$14,8($12)
	nop
	bne	$14,$4,$L86
	nop

	lw	$14,24($12)
	nop
	bne	$14,$2,$L86
	nop

	lw	$14,28($12)
	nop
	bne	$14,$6,$L86
	nop

	lw	$14,32($12)
	nop
	bne	$14,$7,$L86
	nop

	lw	$14,36($12)
	nop
	bne	$14,$10,$L86
	nop

	lw	$3,40($12)
	subu	$11,$11,$5
	subu	$11,$11,$3
	bgez	$11,$L83
	move	$2,$0

	lw	$4,56($12)
	addu	$5,$5,$3
$L90:
	jal	rtl_set_ct_timeout_expires
	nop

	j	$L83
	li	$2,-1			# 0xffffffffffffffff

$L86:
	lw	$12,64($12)
	nop
$L85:
	bne	$12,$0,$L87
	nop

	move	$2,$0
$L83:
	lw	$31,20($sp)
	nop
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	rtk_idleNaptConnection
	.size	rtk_idleNaptConnection, .-rtk_idleNaptConnection
	.section	.text.rtl_fpTimer_update,"ax",@progbits
	.align	2
	.globl	rtl_fpTimer_update
	.set	nomips16
	.ent	rtl_fpTimer_update
	.type	rtl_fpTimer_update, @function
rtl_fpTimer_update:
	.frame	$sp,80,$31		# vars= 32, regs= 7/0, args= 16, gp= 0
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
	lui	$2,%hi(jiffies)
	move	$5,$0
	move	$16,$4
	lw	$20,%lo(jiffies)($2)
	jal	rtl_get_ct_protonum
	nop

	li	$3,17			# 0x11
	bne	$2,$3,$L92
	move	$4,$16

	jal	rtl_get_ct_udp_status
	nop

	andi	$2,$2,0x2
	bne	$2,$0,$L102
	li	$4,1			# 0x1

	move	$4,$0
$L102:
	move	$5,$16
	jal	udp_get_timeouts_by_state
	li	$6,1			# 0x1

	move	$17,$2
	j	$L94
	li	$19,1			# 0x1

$L92:
	jal	rtl_get_ct_protonum
	move	$5,$0

	li	$3,6			# 0x6
	bne	$2,$3,$L95
	li	$18,-1			# 0xffffffffffffffff

	jal	rtl_get_ct_tcp_state
	move	$4,$16

	sltu	$2,$2,6
	beq	$2,$0,$L104
	move	$2,$18

	jal	rtl_tcp_get_timeouts
	move	$4,$16

	move	$17,$2
	li	$19,2			# 0x2
$L94:
	move	$4,$16
	move	$5,$0
	jal	rtl_get_ct_ip_by_dir
	move	$6,$0

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$18,$2

	bne	$18,$2,$L96
	move	$4,$16

	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$19,16($sp)

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,20($sp)

	move	$4,$16
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	sw	$2,24($sp)

	move	$4,$16
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_get_ct_port_by_dir
	sw	$2,28($sp)

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$2,32($sp)

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,36($sp)

	addiu	$4,$sp,16
	move	$5,$17
	jal	rtk_idleNaptConnection
	sw	$2,40($sp)

	beq	$2,$0,$L95
	li	$18,-1			# 0xffffffffffffffff

	j	$L103
	move	$4,$16

$L96:
	move	$5,$0
	jal	rtl_get_ct_ip_by_dir
	li	$6,1			# 0x1

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$21,$2

	bne	$21,$2,$L95
	li	$18,-1			# 0xffffffffffffffff

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$19,16($sp)

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,20($sp)

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	sw	$2,24($sp)

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_port_by_dir
	sw	$2,28($sp)

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$2,32($sp)

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,36($sp)

	addiu	$4,$sp,16
	move	$5,$17
	jal	rtk_idleNaptConnection
	sw	$2,40($sp)

	beq	$2,$0,$L104
	move	$2,$18

	move	$4,$16
$L103:
	jal	rtl_check_for_acc
	addu	$5,$17,$20

	move	$18,$0
$L95:
	move	$2,$18
$L104:
	lw	$31,76($sp)
	lw	$21,72($sp)
	lw	$20,68($sp)
	lw	$19,64($sp)
	lw	$18,60($sp)
	lw	$17,56($sp)
	lw	$16,52($sp)
	j	$31
	addiu	$sp,$sp,80

	.set	macro
	.set	reorder
	.end	rtl_fpTimer_update
	.size	rtl_fpTimer_update, .-rtl_fpTimer_update
	.section	.text.rtl_br_fdb_time_update,"ax",@progbits
	.align	2
	.globl	rtl_br_fdb_time_update
	.set	nomips16
	.ent	rtl_br_fdb_time_update
	.type	rtl_br_fdb_time_update, @function
rtl_br_fdb_time_update:
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
	move	$17,$4
	move	$16,$5
	move	$4,$0
	move	$5,$6
	jal	rtl_get_hw_fdb_age
	li	$6,2			# 0x2

	li	$3,300			# 0x12c
	beq	$2,$3,$L108
	lui	$18,%hi(jiffies)

	li	$3,450			# 0x1c2
	beq	$2,$3,$L109
	li	$3,150			# 0x96

	bne	$2,$3,$L114
	move	$4,$16

	j	$L113
	nop

$L109:
	lw	$5,%lo(jiffies)($18)
	j	$L112
	move	$4,$16

$L108:
	lw	$5,%lo(jiffies)($18)
	move	$4,$16
	j	$L112
	addiu	$5,$5,-15000

$L113:
	lw	$5,%lo(jiffies)($18)
	move	$4,$16
	addiu	$5,$5,-30000
$L112:
	jal	rtl_set_fdb_aging
	nop

	move	$4,$16
$L114:
	lw	$18,%lo(jiffies)($18)
	jal	rtl_get_fdb_aging
	nop

	move	$4,$17
	jal	rtl_hold_time
	move	$16,$2

	addu	$16,$2,$16
	subu	$18,$18,$16
	slt	$18,$18,0
	li	$2,-1			# 0xffffffffffffffff
	movn	$2,$0,$18
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl_br_fdb_time_update
	.size	rtl_br_fdb_time_update, .-rtl_br_fdb_time_update
	.section	.text.FastPath_Route,"ax",@progbits
	.align	2
	.globl	FastPath_Route
	.set	nomips16
	.ent	FastPath_Route
	.type	FastPath_Route, @function
FastPath_Route:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	lui	$2,%hi(route_list_inuse)
	lw	$3,%lo(route_list_inuse)($2)
	move	$6,$0
	j	$L116
	move	$2,$0

$L118:
	lw	$5,8($3)
	nop
	sltu	$7,$5,$6
	bne	$7,$0,$L117
	and	$8,$4,$5

	lw	$7,4($3)
	nop
	bne	$8,$7,$L117
	nop

	addiu	$2,$3,16
	move	$6,$5
$L117:
	lw	$3,44($3)
	nop
$L116:
	bne	$3,$0,$L118
	nop

	j	$31
	nop

	.set	macro
	.set	reorder
	.end	FastPath_Route
	.size	FastPath_Route, .-FastPath_Route
	.section	.text.rtk_addNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_addNaptConnection
	.set	nomips16
	.ent	rtk_addNaptConnection
	.type	rtk_addNaptConnection, @function
rtk_addNaptConnection:
	.frame	$sp,160,$31		# vars= 104, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-160
	sw	$31,156($sp)
	sw	$fp,152($sp)
	sw	$23,148($sp)
	sw	$22,144($sp)
	sw	$21,140($sp)
	sw	$20,136($sp)
	sw	$19,132($sp)
	sw	$18,128($sp)
	sw	$17,124($sp)
	sw	$16,120($sp)
	li	$18,-1			# 0xffffffffffffffff
	move	$16,$4
	move	$17,$5
	sw	$6,168($sp)
	beq	$5,$0,$L120
	move	$19,$7

	jal	rtl_eth_hdr
	move	$4,$5

	beq	$2,$0,$L120
	li	$2,17			# 0x11

	lw	$3,0($16)
	lw	$4,20($16)
	xori	$3,$3,0x2
	li	$fp,6			# 0x6
	jal	filter_addconnect
	movn	$fp,$2,$3

	jal	arch_local_irq_save
	nop

	lw	$4,4($16)
	lw	$6,8($16)
	lw	$3,20($16)
	lw	$5,24($16)
	xor	$8,$6,$4
	move	$23,$2
	xor	$8,$8,$3
	lw	$2,0($16)
	srl	$7,$4,16
	xor	$8,$8,$5
	xor	$8,$8,$7
	srl	$7,$3,16
	xor	$8,$8,$7
	andi	$7,$2,0xffff
	xor	$8,$8,$7
	srl	$7,$8,12
	xor	$8,$7,$8
	lui	$7,%hi(napt_table_list_max)
	lw	$7,%lo(napt_table_list_max)($7)
	lui	$9,%hi(table_napt)
	addiu	$7,$7,-1
	and	$8,$8,$7
	li	$7,12			# 0xc
	mult	$8,$7
	lw	$7,%lo(table_napt)($9)
	nop
	lw	$7,0($7)
	mflo	$8
	addu	$7,$7,$8
	lw	$7,0($7)
	j	$L122
	nop

$L125:
	lw	$10,4($7)
	nop
	bne	$10,$2,$L123
	nop

	lw	$10,8($7)
	nop
	bne	$10,$4,$L123
	nop

	lw	$10,12($7)
	nop
	bne	$10,$6,$L123
	nop

	lw	$11,16($7)
	lw	$10,12($16)
	nop
	bne	$11,$10,$L123
	nop

	lw	$11,20($7)
	lw	$10,16($16)
	nop
	bne	$11,$10,$L123
	nop

	lw	$10,24($7)
	nop
	bne	$10,$3,$L123
	nop

	lw	$10,28($7)
	nop
	bne	$10,$5,$L123
	nop

	lw	$4,168($sp)
	move	$5,$fp
	jal	rtl_conntrack_drop_check_hook
	move	$6,$4

	jal	arch_local_irq_restore
	move	$4,$23

	j	$L120
	move	$18,$0

$L123:
	lw	$7,36($7)
	nop
$L122:
	bne	$7,$0,$L125
	nop

	lui	$4,%hi(napt_list_free)
	lw	$3,%lo(napt_list_free)($4)
	nop
	beq	$3,$0,$L126
	nop

	sw	$2,4($3)
	lw	$2,4($16)
	lw	$5,44($3)
	sw	$2,8($3)
	lw	$2,8($16)
	nop
	sw	$2,12($3)
	lw	$2,12($16)
	nop
	sw	$2,16($3)
	lw	$2,16($16)
	nop
	sw	$2,20($3)
	lw	$2,20($16)
	nop
	sw	$2,24($3)
	lw	$2,24($16)
	sw	$19,32($3)
	sw	$2,28($3)
	li	$2,-1			# 0xffffffffffffffff
	sb	$2,0($3)
	lw	$2,48($3)
	beq	$5,$0,$L127
	nop

	j	$L128
	sw	$2,48($5)

$L127:
	addiu	$5,$4,%lo(napt_list_free)
	sw	$2,4($5)
$L128:
	lw	$5,44($3)
	lw	$2,48($3)
	nop
	sw	$5,0($2)
	addiu	$2,$4,%lo(napt_list_free)
	lw	$4,8($2)
	sw	$0,44($3)
	addiu	$4,$4,-1
	sw	$4,8($2)
	lui	$2,%hi(napt_list_inuse)
	addiu	$2,$2,%lo(napt_list_inuse)
	lw	$4,4($2)
	nop
	sw	$4,48($3)
	sw	$3,0($4)
	addiu	$4,$3,44
	sw	$4,4($2)
	lw	$4,8($2)
	sw	$0,36($3)
	addiu	$4,$4,1
	sw	$4,8($2)
	lw	$2,%lo(table_napt)($9)
	nop
	lw	$4,0($2)
	nop
	addu	$4,$4,$8
	lw	$4,4($4)
	nop
	sw	$4,40($3)
	lw	$2,0($2)
	nop
	addu	$2,$2,$8
	lw	$2,4($2)
	nop
	sw	$3,0($2)
	lw	$2,%lo(table_napt)($9)
	addiu	$3,$3,36
	lw	$4,0($2)
	nop
	addu	$4,$4,$8
	sw	$3,4($4)
	lw	$2,0($2)
	sb	$0,16($sp)
	addu	$8,$2,$8
	lw	$2,8($8)
	nop
	addiu	$2,$2,1
	sw	$2,8($8)
	li	$2,33			# 0x21
	jal	rtl865x_getLanDev
	sb	$2,17($sp)

	jal	rtl865x_getWanDev
	move	$21,$2

	move	$4,$17
	jal	rtl_get_skb_protocol
	move	$20,$2

	move	$4,$17
	jal	rtl_ip_hdr
	sw	$2,64($sp)

	lbu	$22,0($2)
	move	$4,$17
	move	$19,$2
	andi	$22,$22,0xf
	sll	$22,$22,2
	jal	rtl_eth_hdr
	addu	$22,$2,$22

	addiu	$5,$2,6
	li	$6,6			# 0x6
	jal	memcpy
	addiu	$4,$sp,28

	jal	rtl_eth_hdr
	move	$4,$17

	move	$5,$2
	li	$6,6			# 0x6
	jal	memcpy
	addiu	$4,$sp,20

	jal	rtl_get_skb_mark
	move	$4,$17

	lw	$3,12($19)
	move	$4,$17
	sw	$2,80($sp)
	sw	$3,84($sp)
	lhu	$3,0($22)
	nop
	sw	$3,88($sp)
	lw	$3,16($19)
	nop
	sw	$3,92($sp)
	lhu	$3,2($22)
	jal	rtl_store_skb_dst
	sw	$3,96($sp)

	lw	$3,4($16)
	lui	$2,%hi(path_list_free)
	lw	$18,%lo(path_list_free)($2)
	sw	$3,56($sp)
	lw	$3,8($16)
	nop
	sw	$3,68($sp)
	lw	$3,20($16)
	nop
	sw	$3,60($sp)
	lw	$3,24($16)
	nop
	sw	$3,72($sp)
	lui	$3,%hi(path_table_list_max)
	lw	$4,%lo(path_table_list_max)($3)
	beq	$18,$0,$L129
	sw	$4,76($sp)

	lw	$2,0($16)
	nop
	sw	$2,4($18)
	lw	$7,4($16)
	nop
	sw	$7,16($18)
	lw	$5,8($16)
	nop
	sw	$5,12($18)
	lw	$2,20($16)
	nop
	sw	$2,20($18)
	lw	$2,24($16)
	nop
	sw	$2,8($18)
	lw	$6,12($16)
	nop
	sw	$6,24($18)
	lw	$3,16($16)
	nop
	sw	$3,28($18)
	lw	$4,20($16)
	nop
	sw	$4,32($18)
	lw	$2,24($16)
	nop
	sw	$2,36($18)
	sw	$3,112($sp)
	sw	$5,104($sp)
	sw	$6,108($sp)
	jal	FastPath_Route
	sw	$7,100($sp)

	sw	$2,44($18)
	li	$4,-1			# 0xffffffffffffffff
	li	$2,1			# 0x1
	sb	$2,2($18)
	sb	$4,0($18)
	sb	$0,1($18)
	lw	$6,108($sp)
	lw	$7,100($sp)
	lw	$3,112($sp)
	lw	$5,104($sp)
	beq	$7,$6,$L130
	nop

	sb	$2,1($18)
$L130:
	beq	$5,$3,$L131
	nop

	lbu	$2,1($18)
	nop
	ori	$2,$2,0x2
	sb	$2,1($18)
$L131:
	sw	$0,48($18)
	sw	$0,52($18)
	lw	$2,4($16)
	addiu	$4,$sp,36
	move	$5,$0
	sw	$2,12($19)
	lw	$2,8($16)
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,20($16)
	nop
	sw	$2,16($19)
	lw	$2,24($16)
	jal	memset
	sh	$2,2($22)

	move	$4,$17
	jal	rtl_set_skb_mark
	move	$5,$0

	beq	$21,$0,$L132
	nop

	lw	$4,4($16)
	move	$5,$21
	jal	rtl_arp_req_get_ha
	addiu	$6,$sp,36

	bne	$2,$0,$L132
	move	$4,$17

	jal	rtl_ppp_proto_check
	addiu	$5,$sp,16

	move	$3,$2
	li	$2,1			# 0x1
	bne	$3,$2,$L133
	move	$4,$17

	jal	rtl_skb_mac_header_was_set
	sw	$3,112($sp)

	lw	$3,112($sp)
	nop
	bne	$2,$3,$L133
	nop

	jal	rtl_eth_hdr
	move	$4,$17

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$3,$2,$3
	li	$2,2048			# 0x800
	beq	$3,$2,$L133
	move	$4,$17

	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffffffffffea

$L133:
	jal	rtl_eth_hdr
	move	$4,$17

	addiu	$4,$2,6
	addiu	$5,$sp,36
	jal	memcpy
	li	$6,6			# 0x6

	move	$4,$17
	jal	rtl_set_skb_dmac
	move	$5,$21

	lw	$3,64($sp)
	li	$2,2048			# 0x800
	bne	$3,$2,$L132
	lui	$2,%hi(nf_hooks)

	addiu	$3,$2,%lo(nf_hooks)
	lw	$3,128($3)
	lui	$2,%hi(nf_hooks+128)
	addiu	$2,$2,%lo(nf_hooks+128)
	beq	$3,$2,$L134
	move	$4,$17

	move	$5,$0
	move	$6,$21
	jal	rtl_ipt_do_table
	move	$7,$20

$L134:
	jal	rtl_get_skb_mark
	move	$4,$17

	beq	$2,$0,$L135
	nop

	jal	rtl_get_skb_mark
	move	$4,$17

	j	$L174
	lui	$4,%hi(nf_hooks)

$L135:
	lw	$2,48($18)
	lui	$4,%hi(nf_hooks)
$L174:
	sw	$2,48($18)
	addiu	$2,$4,%lo(nf_hooks)
	lw	$3,160($2)
	lui	$2,%hi(nf_hooks+160)
	addiu	$2,$2,%lo(nf_hooks+160)
	beq	$3,$2,$L137
	move	$4,$17

	li	$5,4			# 0x4
	move	$6,$21
	jal	rtl_ipt_do_table
	move	$7,$20

$L137:
	jal	rtl_get_skb_mark
	move	$4,$17

	beq	$2,$0,$L138
	nop

	jal	rtl_get_skb_mark
	move	$4,$17

	j	$L132
	sw	$2,52($18)

$L138:
	lw	$2,52($18)
	nop
	sw	$2,52($18)
$L132:
	lw	$2,12($16)
	addiu	$4,$sp,36
	move	$5,$0
	sw	$2,12($19)
	lw	$2,16($16)
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,20($16)
	nop
	sw	$2,16($19)
	lw	$2,24($16)
	jal	memset
	sh	$2,2($22)

	move	$4,$17
	jal	rtl_set_skb_mark
	move	$5,$0

	beq	$20,$0,$L140
	move	$5,$20

	lw	$4,20($16)
	jal	rtl_arp_req_get_ha
	addiu	$6,$sp,36

	bne	$2,$0,$L175
	li	$2,6			# 0x6

	move	$4,$17
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,16

	move	$3,$2
	li	$2,1			# 0x1
	bne	$3,$2,$L141
	move	$4,$17

	jal	rtl_skb_mac_header_was_set
	sw	$3,112($sp)

	lw	$3,112($sp)
	nop
	bne	$2,$3,$L141
	nop

	jal	rtl_eth_hdr
	move	$4,$17

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$3,$2,$3
	li	$2,2048			# 0x800
	beq	$3,$2,$L141
	move	$4,$17

	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffffffffffea

$L141:
	jal	rtl_eth_hdr
	move	$4,$17

	move	$4,$2
	addiu	$5,$sp,36
	jal	memcpy
	li	$6,6			# 0x6

	move	$4,$17
	jal	rtl_set_skb_smac
	move	$5,$20

	lw	$3,64($sp)
	li	$2,2048			# 0x800
	bne	$3,$2,$L175
	li	$2,6			# 0x6

	lui	$2,%hi(nf_hooks+160)
	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	beq	$3,$2,$L142
	move	$4,$17

	li	$5,4			# 0x4
	move	$6,$21
	jal	rtl_ipt_do_table
	move	$7,$20

$L142:
	jal	rtl_get_skb_mark
	move	$4,$17

	beq	$2,$0,$L143
	nop

	jal	rtl_get_skb_mark
	move	$4,$17

	j	$L140
	sw	$2,52($18)

$L143:
	lw	$2,52($18)
	nop
	sw	$2,52($18)
$L140:
	li	$2,6			# 0x6
$L175:
	bne	$fp,$2,$L145
	li	$4,1			# 0x1

	lw	$4,168($sp)
	jal	rtl_tcp_get_timeouts
	nop

	j	$L171
	nop

$L145:
	lw	$5,168($sp)
	jal	udp_get_timeouts_by_state
	li	$6,1			# 0x1

$L171:
	lw	$3,168($sp)
	sw	$2,60($18)
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	sw	$3,56($18)
	lw	$3,72($18)
	sw	$2,40($18)
	lw	$2,76($18)
	beq	$3,$0,$L147
	lui	$4,%hi(path_list_free)

	j	$L148
	sw	$2,76($3)

$L147:
	addiu	$3,$4,%lo(path_list_free)
	sw	$2,4($3)
$L148:
	lw	$6,56($sp)
	lw	$3,60($sp)
	srl	$2,$6,8
	xor	$4,$3,$6
	lw	$3,72($sp)
	lui	$6,%hi(path_list_free)
	xor	$4,$4,$3
	lw	$3,60($sp)
	xor	$4,$4,$2
	srl	$2,$3,16
	lw	$3,68($sp)
	xor	$4,$4,$2
	srl	$2,$3,4
	xor	$4,$4,$2
	lw	$3,72($18)
	lw	$2,76($18)
	xor	$4,$4,$fp
	srl	$5,$4,12
	sw	$3,0($2)
	addiu	$2,$6,%lo(path_list_free)
	lw	$6,76($sp)
	lw	$3,8($2)
	xor	$4,$5,$4
	addiu	$5,$6,-1
	and	$4,$4,$5
	li	$5,12			# 0xc
	addiu	$3,$3,-1
	mult	$4,$5
	sw	$3,8($2)
	lui	$2,%hi(path_list_inuse)
	addiu	$2,$2,%lo(path_list_inuse)
	lw	$3,4($2)
	sw	$0,72($18)
	sw	$3,76($18)
	sw	$18,0($3)
	addiu	$3,$18,72
	sw	$3,4($2)
	lw	$3,8($2)
	sw	$0,64($18)
	addiu	$3,$3,1
	sw	$3,8($2)
	lui	$2,%hi(table_path)
	lw	$3,%lo(table_path)($2)
	mflo	$4
	lw	$5,0($3)
	nop
	addu	$5,$5,$4
	lw	$5,4($5)
	nop
	sw	$5,68($18)
	lw	$3,0($3)
	nop
	addu	$3,$3,$4
	lw	$3,4($3)
	nop
	sw	$18,0($3)
	lw	$2,%lo(table_path)($2)
	addiu	$18,$18,64
	lw	$3,0($2)
	nop
	addu	$3,$3,$4
	sw	$18,4($3)
	lw	$2,0($2)
	nop
	addu	$4,$2,$4
	lw	$2,8($4)
	nop
	addiu	$2,$2,1
	sw	$2,8($4)
$L129:
	lw	$3,20($16)
	lui	$4,%hi(path_table_list_max)
	lui	$6,%hi(path_list_free)
	sw	$3,56($sp)
	lw	$3,24($16)
	lw	$4,%lo(path_table_list_max)($4)
	lw	$18,%lo(path_list_free)($6)
	sw	$3,68($sp)
	lw	$3,12($16)
	sw	$4,76($sp)
	sw	$3,60($sp)
	lw	$3,16($16)
	beq	$18,$0,$L149
	sw	$3,72($sp)

	lw	$2,0($16)
	nop
	sw	$2,4($18)
	lw	$2,20($16)
	nop
	sw	$2,16($18)
	lw	$2,24($16)
	nop
	sw	$2,12($18)
	lw	$7,12($16)
	nop
	sw	$7,20($18)
	lw	$6,16($16)
	nop
	sw	$6,8($18)
	lw	$2,20($16)
	nop
	sw	$2,24($18)
	lw	$2,24($16)
	nop
	sw	$2,28($18)
	lw	$3,4($16)
	nop
	sw	$3,32($18)
	lw	$5,8($16)
	move	$4,$3
	sw	$5,36($18)
	sw	$3,112($sp)
	sw	$5,104($sp)
	sw	$6,108($sp)
	jal	FastPath_Route
	sw	$7,100($sp)

	sw	$2,44($18)
	li	$2,2			# 0x2
	sb	$2,2($18)
	li	$2,-1			# 0xffffffffffffffff
	sb	$2,0($18)
	sb	$0,1($18)
	lw	$3,112($sp)
	lw	$7,100($sp)
	lw	$5,104($sp)
	lw	$6,108($sp)
	beq	$7,$3,$L150
	li	$2,4			# 0x4

	sb	$2,1($18)
$L150:
	beq	$6,$5,$L151
	nop

	lbu	$2,1($18)
	nop
	ori	$2,$2,0x8
	sb	$2,1($18)
$L151:
	sw	$0,48($18)
	sw	$0,52($18)
	lw	$2,20($16)
	addiu	$4,$sp,36
	move	$5,$0
	sw	$2,12($19)
	lw	$2,24($16)
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,12($16)
	nop
	sw	$2,16($19)
	lw	$2,16($16)
	jal	memset
	sh	$2,2($22)

	move	$4,$17
	jal	rtl_set_skb_mark
	move	$5,$0

	beq	$20,$0,$L152
	nop

	lw	$4,20($16)
	move	$5,$20
	jal	rtl_arp_req_get_ha
	addiu	$6,$sp,36

	bne	$2,$0,$L152
	move	$4,$17

	jal	rtl_ppp_proto_check
	addiu	$5,$sp,16

	move	$3,$2
	li	$2,1			# 0x1
	bne	$3,$2,$L153
	move	$4,$17

	jal	rtl_skb_mac_header_was_set
	sw	$3,112($sp)

	lw	$3,112($sp)
	nop
	bne	$2,$3,$L153
	nop

	jal	rtl_eth_hdr
	move	$4,$17

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$3,$2,$3
	li	$2,2048			# 0x800
	beq	$3,$2,$L153
	move	$4,$17

	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffffffffffea

$L153:
	jal	rtl_eth_hdr
	move	$4,$17

	addiu	$4,$2,6
	addiu	$5,$sp,36
	jal	memcpy
	li	$6,6			# 0x6

	move	$4,$17
	jal	rtl_set_skb_dmac
	move	$5,$20

	lw	$3,64($sp)
	li	$2,2048			# 0x800
	bne	$3,$2,$L154
	lui	$2,%hi(nf_hooks+128)

	lw	$3,%lo(nf_hooks+128)($2)
	addiu	$2,$2,%lo(nf_hooks+128)
	beq	$3,$2,$L154
	move	$4,$17

	move	$5,$0
	move	$6,$20
	jal	rtl_ipt_do_table
	move	$7,$21

$L154:
	jal	rtl_get_skb_mark
	move	$4,$17

	beq	$2,$0,$L155
	nop

	jal	rtl_get_skb_mark
	move	$4,$17

	j	$L152
	sw	$2,48($18)

$L155:
	lw	$2,48($18)
	nop
	sw	$2,48($18)
$L152:
	lw	$2,20($16)
	addiu	$4,$sp,36
	move	$5,$0
	sw	$2,12($19)
	lw	$2,24($16)
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,4($16)
	nop
	sw	$2,16($19)
	lw	$2,8($16)
	jal	memset
	sh	$2,2($22)

	move	$4,$17
	jal	rtl_set_skb_mark
	move	$5,$0

	beq	$21,$0,$L157
	move	$5,$21

	lw	$4,4($16)
	jal	rtl_arp_req_get_ha
	addiu	$6,$sp,36

	bne	$2,$0,$L157
	move	$4,$17

	jal	rtl_ppp_proto_check
	addiu	$5,$sp,16

	move	$16,$2
	li	$2,1			# 0x1
	bne	$16,$2,$L158
	nop

	jal	rtl_skb_mac_header_was_set
	move	$4,$17

	bne	$2,$16,$L158
	nop

	jal	rtl_eth_hdr
	move	$4,$17

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$3,$2,$3
	li	$2,2048			# 0x800
	beq	$3,$2,$L158
	move	$4,$17

	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffffffffffea

$L158:
	jal	rtl_eth_hdr
	move	$4,$17

	move	$4,$2
	addiu	$5,$sp,36
	jal	memcpy
	li	$6,6			# 0x6

	move	$4,$17
	jal	rtl_set_skb_smac
	move	$5,$21

	lw	$3,64($sp)
	li	$2,2048			# 0x800
	bne	$3,$2,$L157
	lui	$2,%hi(nf_hooks+160)

	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	beq	$3,$2,$L157
	move	$4,$17

	li	$5,4			# 0x4
	move	$6,$20
	jal	rtl_ipt_do_table
	move	$7,$21

$L157:
	jal	rtl_get_skb_mark
	move	$4,$17

	sw	$2,52($18)
	li	$2,6			# 0x6
	bne	$fp,$2,$L159
	li	$4,1			# 0x1

	lw	$4,168($sp)
	jal	rtl_tcp_get_timeouts
	nop

	j	$L172
	nop

$L159:
	lw	$5,168($sp)
	jal	udp_get_timeouts_by_state
	li	$6,1			# 0x1

$L172:
	lw	$3,168($sp)
	sw	$2,60($18)
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	sw	$3,56($18)
	lw	$3,72($18)
	sw	$2,40($18)
	lw	$2,76($18)
	beq	$3,$0,$L161
	nop

	j	$L162
	sw	$2,76($3)

$L161:
	lui	$4,%hi(path_list_free)
	addiu	$3,$4,%lo(path_list_free)
	sw	$2,4($3)
$L162:
	lw	$6,56($sp)
	lw	$3,60($sp)
	srl	$2,$6,8
	xor	$4,$3,$6
	lw	$3,72($sp)
	lui	$6,%hi(path_list_free)
	xor	$4,$4,$3
	lw	$3,60($sp)
	xor	$4,$4,$2
	srl	$2,$3,16
	lw	$3,68($sp)
	xor	$4,$4,$2
	srl	$2,$3,4
	xor	$4,$4,$2
	lw	$3,72($18)
	lw	$2,76($18)
	xor	$4,$4,$fp
	srl	$5,$4,12
	sw	$3,0($2)
	addiu	$2,$6,%lo(path_list_free)
	lw	$6,76($sp)
	lw	$3,8($2)
	xor	$4,$5,$4
	addiu	$5,$6,-1
	and	$4,$4,$5
	li	$5,12			# 0xc
	addiu	$3,$3,-1
	mult	$4,$5
	sw	$3,8($2)
	lui	$2,%hi(path_list_inuse)
	addiu	$2,$2,%lo(path_list_inuse)
	lw	$3,4($2)
	sw	$0,72($18)
	sw	$3,76($18)
	sw	$18,0($3)
	addiu	$3,$18,72
	sw	$3,4($2)
	lw	$3,8($2)
	sw	$0,64($18)
	addiu	$3,$3,1
	sw	$3,8($2)
	lui	$2,%hi(table_path)
	lw	$3,%lo(table_path)($2)
	mflo	$4
	lw	$5,0($3)
	nop
	addu	$5,$5,$4
	lw	$5,4($5)
	nop
	sw	$5,68($18)
	lw	$3,0($3)
	nop
	addu	$3,$3,$4
	lw	$3,4($3)
	nop
	sw	$18,0($3)
	lw	$2,%lo(table_path)($2)
	addiu	$18,$18,64
	lw	$3,0($2)
	nop
	addu	$3,$3,$4
	sw	$18,4($3)
	lw	$2,0($2)
	nop
	addu	$4,$2,$4
	lw	$2,8($4)
	nop
	addiu	$2,$2,1
	sw	$2,8($4)
$L149:
	jal	rtl_eth_hdr
	move	$4,$17

	addiu	$4,$2,6
	addiu	$5,$sp,28
	jal	memcpy
	li	$6,6			# 0x6

	jal	rtl_eth_hdr
	move	$4,$17

	move	$4,$2
	addiu	$5,$sp,20
	jal	memcpy
	li	$6,6			# 0x6

	lw	$5,80($sp)
	jal	rtl_set_skb_mark
	move	$4,$17

	lw	$3,84($sp)
	move	$4,$17
	sw	$3,12($19)
	lw	$3,88($sp)
	nop
	sh	$3,0($22)
	lw	$3,92($sp)
	nop
	sw	$3,16($19)
	lw	$3,96($sp)
	jal	rtl_set_skb_dst
	sh	$3,2($22)

	beq	$21,$0,$L163
	nop

	jal	arch_local_irq_save
	nop

	lw	$3,496($21)
	nop
	lw	$4,0($3)
	nop
	addiu	$4,$4,-1
	sw	$4,0($3)
	jal	arch_local_irq_restore
	move	$4,$2

$L163:
	beq	$20,$0,$L164
	nop

	jal	arch_local_irq_save
	nop

	lw	$3,496($20)
	nop
	lw	$4,0($3)
	nop
	addiu	$4,$4,-1
	sw	$4,0($3)
	jal	arch_local_irq_restore
	move	$4,$2

$L164:
	jal	arch_local_irq_restore
	move	$4,$23

	lw	$4,168($sp)
	move	$5,$fp
	jal	rtl_conntrack_drop_check_hook
	move	$6,$4

	j	$L173
	move	$18,$0

$L126:
	jal	arch_local_irq_restore
	move	$4,$23

	li	$18,-1			# 0xffffffffffffffff
$L173:
$L120:
	move	$2,$18
	lw	$31,156($sp)
	lw	$fp,152($sp)
	lw	$23,148($sp)
	lw	$22,144($sp)
	lw	$21,140($sp)
	lw	$20,136($sp)
	lw	$19,132($sp)
	lw	$18,128($sp)
	lw	$17,124($sp)
	lw	$16,120($sp)
	j	$31
	addiu	$sp,$sp,160

	.set	macro
	.set	reorder
	.end	rtk_addNaptConnection
	.size	rtk_addNaptConnection, .-rtk_addNaptConnection
	.section	.text.rtl_fp_mark_invalid,"ax",@progbits
	.align	2
	.globl	rtl_fp_mark_invalid
	.set	nomips16
	.ent	rtl_fp_mark_invalid
	.type	rtl_fp_mark_invalid, @function
rtl_fp_mark_invalid:
	.frame	$sp,72,$31		# vars= 8, regs= 10/0, args= 24, gp= 0
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
	li	$5,1			# 0x1
	move	$20,$4
	jal	rtl_get_ct_ip_by_dir
	move	$6,$0

	move	$4,$20
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$23,$2

	move	$4,$20
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$21,$2

	move	$4,$20
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$19,$2

	move	$4,$20
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	move	$17,$2

	move	$4,$20
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_port_by_dir
	move	$22,$2

	move	$4,$20
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,24($sp)

	li	$6,1			# 0x1
	move	$4,$20
	move	$5,$0
	jal	rtl_get_ct_port_by_dir
	move	$18,$2

	move	$4,$20
	li	$5,1			# 0x1
	jal	rtl_get_ct_protonum
	move	$16,$2

	move	$4,$20
	move	$5,$0
	jal	rtl_get_ct_protonum
	move	$fp,$2

	lw	$7,24($sp)
	move	$4,$23
	move	$5,$22
	move	$6,$21
	move	$20,$2
	jal	mark_path_invalid
	sw	$fp,16($sp)

	move	$4,$19
	move	$5,$18
	move	$6,$17
	move	$7,$16
	jal	mark_path_invalid
	sw	$20,16($sp)

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
	.end	rtl_fp_mark_invalid
	.size	rtl_fp_mark_invalid, .-rtl_fp_mark_invalid
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"lo\000"
	.section	.text.enter_fast_path,"ax",@progbits
	.align	2
	.globl	enter_fast_path
	.set	nomips16
	.ent	enter_fast_path
	.type	enter_fast_path, @function
enter_fast_path:
	.frame	$sp,96,$31		# vars= 32, regs= 10/0, args= 24, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-96
	sw	$31,92($sp)
	sw	$fp,88($sp)
	sw	$23,84($sp)
	sw	$22,80($sp)
	sw	$21,76($sp)
	sw	$20,72($sp)
	sw	$19,68($sp)
	sw	$18,64($sp)
	sw	$17,60($sp)
	sw	$16,56($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_dev_name
	move	$20,$4
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$17,$2
	.set	macro
	.set	reorder

	move	$16,$2
	lbu	$2,8($2)
	#nop
	sltu	$2,$2,2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L272
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	lbu	$21,9($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$21,$2,$L179
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$21,$2,$L178
	move	$6,$0
	.set	macro
	.set	reorder

$L179:
	lbu	$19,0($16)
	move	$6,$20
	move	$4,$16
	andi	$19,$19,0xf
	sll	$19,$19,2
	addu	$19,$16,$19
	.set	noreorder
	.set	nomacro
	jal	fast_path_pre_process_check
	move	$5,$19
	.set	macro
	.set	reorder

	move	$6,$2
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$6,$2,$L178
	lui	$2,%hi($LC0)
	.set	macro
	.set	reorder

	addiu	$2,$2,%lo($LC0)
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$3,($17)
1:	lbu	$1,($2)
	addiu	$17,1
	bne	$1,$3,2f
	addiu	$2,1
	bnez	$3,1b
	lbu	$3,($17)
	nop
	move	$3,$1
2:	subu	$3,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	beq	$3,$0,$L272
	lhu	$5,0($19)
	lhu	$7,2($19)
	lhu	$8,6($16)
	lw	$22,12($16)
	lw	$23,16($16)
	move	$10,$5
	.set	noreorder
	.set	nomacro
	bne	$21,$6,$L180
	move	$9,$7
	.set	macro
	.set	reorder

	andi	$2,$8,0x3fff
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L181
	li	$2,8192			# 0x2000
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$8,$2,$L314
	andi	$2,$8,0x3fff
	.set	macro
	.set	reorder

	j	$L181
$L312:
	move	$4,$22
	move	$6,$23
	.set	noreorder
	.set	nomacro
	jal	mark_path_invalid
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L178
	move	$6,$0
	.set	macro
	.set	reorder

$L280:
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L178
	move	$6,$0
	.set	macro
	.set	reorder

$L180:
	andi	$2,$8,0x3fff
$L314:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L268
	move	$18,$0
	.set	macro
	.set	reorder

	li	$2,8192			# 0x2000
	beq	$8,$2,$L269
	andi	$2,$8,0x2000
	bne	$2,$0,$L270
	andi	$8,$8,0x1fff
	.set	noreorder
	.set	nomacro
	j	$L281
	sltu	$8,$0,$8
	.set	macro
	.set	reorder

$L269:
	move	$8,$0
	.set	noreorder
	.set	nomacro
	j	$L183
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

$L270:
	move	$8,$0
$L281:
	move	$3,$0
$L183:
	lhu	$2,4($16)
	xor	$6,$23,$22
	lbu	$5,9($16)
	srl	$7,$6,16
	xor	$4,$2,$6
	xor	$4,$4,$7
	srl	$6,$4,8
	xor	$7,$4,$5
	lui	$4,%hi(max_udp_frag_entry)
	xor	$7,$7,$6
	lw	$6,%lo(max_udp_frag_entry)($4)
	li	$4,12			# 0xc
	lui	$11,%hi(negative_cache_table)
	addiu	$6,$6,-1
	and	$7,$7,$6
	mult	$7,$4
	lw	$6,%lo(negative_cache_table)($11)
	mflo	$4
	addu	$4,$6,$4
	lw	$4,0($4)
	.set	noreorder
	.set	nomacro
	j	$L184
	li	$6,3			# 0x3
	.set	macro
	.set	reorder

$L186:
	lhu	$12,2($4)
	#nop
	bne	$12,$2,$L185
	lw	$12,4($4)
	#nop
	bne	$12,$22,$L185
	lw	$12,8($4)
	#nop
	bne	$12,$23,$L185
	lbu	$12,1($4)
	#nop
	bne	$12,$5,$L185
	lbu	$12,0($4)
	#nop
	beq	$12,$6,$L272
$L185:
	lw	$4,40($4)
	#nop
$L184:
	bne	$4,$0,$L186
	.set	noreorder
	.set	nomacro
	j	$L310
	li	$6,12			# 0xc
	.set	macro
	.set	reorder

$L190:
	lhu	$6,2($18)
	#nop
	bne	$6,$2,$L188
	lw	$6,4($18)
	#nop
	bne	$6,$22,$L188
	lw	$6,8($18)
	#nop
	bne	$6,$23,$L188
	lbu	$6,1($18)
	#nop
	bne	$6,$5,$L188
	lbu	$6,0($18)
	#nop
	beq	$6,$4,$L189
$L188:
	lw	$18,44($18)
	#nop
$L262:
	bne	$18,$0,$L190
	j	$L311
$L189:
	lhu	$10,12($18)
	lhu	$9,14($18)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L182
	move	$fp,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L282
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

$L263:
	lui	$3,%hi(negative_cache_list_free)
	lw	$4,%lo(negative_cache_list_free)($3)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L178
	move	$6,$0
	.set	macro
	.set	reorder

	sb	$5,1($4)
	lw	$5,48($4)
	sh	$2,2($4)
	li	$2,3			# 0x3
	sb	$2,0($4)
	sw	$22,4($4)
	sw	$23,8($4)
	lw	$2,52($4)
	beq	$5,$0,$L192
	.set	noreorder
	.set	nomacro
	j	$L193
	sw	$2,52($5)
	.set	macro
	.set	reorder

$L192:
	addiu	$5,$3,%lo(negative_cache_list_free)
	sw	$2,4($5)
$L193:
	lw	$2,52($4)
	lw	$5,48($4)
	addiu	$3,$3,%lo(negative_cache_list_free)
	sw	$5,0($2)
	lw	$2,8($3)
	sw	$0,40($4)
	addiu	$2,$2,-1
	sw	$2,8($3)
	li	$2,12			# 0xc
	mult	$7,$2
	lw	$2,%lo(negative_cache_table)($11)
	mflo	$7
	addu	$2,$2,$7
	lw	$3,4($2)
	#nop
	sw	$3,44($4)
	lw	$2,4($2)
	#nop
	sw	$4,0($2)
	lw	$2,%lo(negative_cache_table)($11)
	#nop
	addu	$7,$2,$7
	addiu	$2,$4,40
	sw	$2,4($7)
	lw	$2,8($7)
	#nop
	addiu	$2,$2,1
	sw	$2,8($7)
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	#nop
	addiu	$2,$2,1000
	sw	$2,20($4)
	.set	noreorder
	.set	nomacro
	jal	add_timer
	addiu	$4,$4,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L178
	move	$6,$0
	.set	macro
	.set	reorder

$L268:
	move	$8,$0
	move	$3,$0
$L282:
	li	$fp,1			# 0x1
$L182:
	srl	$2,$23,16
	sw	$2,24($sp)
	lw	$4,24($sp)
	xor	$11,$23,$22
	srl	$2,$22,8
	xor	$2,$11,$2
	xor	$2,$2,$4
	xor	$2,$2,$21
	srl	$4,$10,4
	xor	$2,$2,$9
	xor	$2,$2,$4
	srl	$5,$2,12
	xor	$2,$5,$2
	lui	$5,%hi(path_table_list_max)
	lui	$4,%hi(table_path)
	lw	$5,%lo(path_table_list_max)($5)
	lw	$4,%lo(table_path)($4)
	li	$12,255			# 0xff
	addiu	$5,$5,-1
	and	$5,$2,$5
	lw	$2,0($4)
	li	$4,12			# 0xc
	mult	$5,$4
	li	$7,2			# 0x2
	li	$5,17			# 0x11
	li	$4,6			# 0x6
	mflo	$6
	addu	$2,$2,$6
	lw	$17,0($2)
	.set	noreorder
	.set	nomacro
	j	$L194
	li	$6,1			# 0x1
	.set	macro
	.set	reorder

$L259:
	lw	$2,12($17)
	#nop
	bne	$2,$10,$L195
	lw	$2,8($17)
	#nop
	bne	$2,$9,$L195
	lw	$2,16($17)
	#nop
	bne	$2,$22,$L195
	lw	$2,20($17)
	#nop
	bne	$2,$23,$L195
	lbu	$2,0($17)
	#nop
	bne	$2,$12,$L195
	lw	$2,4($17)
	#nop
	bne	$2,$7,$L196
	bne	$21,$4,$L195
	j	$L197
$L196:
	bne	$2,$6,$L195
	bne	$21,$5,$L195
$L197:
	lw	$5,32($17)
	lbu	$7,1($16)
	move	$6,$22
	move	$4,$20
	sw	$3,36($sp)
	sw	$8,40($sp)
	sw	$9,32($sp)
	sw	$10,28($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_route_input
	sw	$11,44($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L178
	move	$6,$0
	.set	macro
	.set	reorder

	lw	$5,32($17)
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_dst_check
	sw	$6,48($sp)
	.set	macro
	.set	reorder

	li	$4,-1			# 0xffffffffffffffff
	lw	$3,36($sp)
	lw	$8,40($sp)
	lw	$9,32($sp)
	lw	$10,28($sp)
	lw	$11,44($sp)
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L198
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_dst_release
	move	$4,$20
	.set	macro
	.set	reorder

	j	$L301
$L198:
	.set	noreorder
	.set	nomacro
	beq	$21,$2,$L200
	addiu	$7,$19,16
	.set	macro
	.set	reorder

	lhu	$7,6($19)
	addiu	$2,$19,6
	movz	$2,$0,$7
	move	$7,$2
$L200:
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L201
	lui	$2,%hi(max_udp_frag_entry)
	.set	macro
	.set	reorder

	lw	$12,%lo(max_udp_frag_entry)($2)
	lui	$2,%hi(udp_cache_list_free)
	lw	$4,%lo(udp_cache_list_free)($2)
	lhu	$3,4($16)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L202
	li	$5,1			# 0x1
	.set	macro
	.set	reorder

	lw	$8,52($4)
	sh	$3,2($4)
	sw	$22,4($4)
	sw	$23,8($4)
	sb	$21,1($4)
	sh	$10,12($4)
	sh	$9,14($4)
	sb	$5,0($4)
	lw	$6,56($4)
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L203
	addiu	$5,$2,%lo(udp_cache_list_free)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L204
	sw	$6,56($8)
	.set	macro
	.set	reorder

$L203:
	sw	$6,4($5)
$L204:
	xor	$3,$3,$11
	srl	$11,$11,16
	lw	$5,52($4)
	xor	$11,$3,$11
	lw	$3,56($4)
	addiu	$6,$12,-1
	addiu	$2,$2,%lo(udp_cache_list_free)
	sw	$5,0($3)
	xor	$5,$11,$21
	srl	$11,$11,8
	xor	$5,$5,$11
	and	$5,$5,$6
	li	$6,12			# 0xc
	mult	$5,$6
	lw	$3,8($2)
	sw	$0,44($4)
	addiu	$3,$3,-1
	sw	$3,8($2)
	lui	$2,%hi(udp_cache_table)
	lw	$3,%lo(udp_cache_table)($2)
	#nop
	lw	$6,0($3)
	mflo	$5
	addu	$6,$6,$5
	lw	$6,4($6)
	#nop
	sw	$6,48($4)
	lw	$3,0($3)
	addiu	$6,$4,44
	addu	$3,$3,$5
	lw	$3,4($3)
	#nop
	sw	$4,0($3)
	lw	$2,%lo(udp_cache_table)($2)
	#nop
	lw	$3,0($2)
	#nop
	addu	$3,$3,$5
	sw	$6,4($3)
	lw	$2,0($2)
	#nop
	addu	$5,$2,$5
	lw	$2,8($5)
	#nop
	addiu	$2,$2,1
	sw	$2,8($5)
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	#nop
	addiu	$2,$2,1000
	sw	$2,24($4)
	addiu	$4,$4,16
	.set	noreorder
	.set	nomacro
	jal	add_timer
	sw	$7,28($sp)
	.set	macro
	.set	reorder

	lw	$7,28($sp)
	.set	noreorder
	.set	nomacro
	j	$L313
	move	$6,$21
	.set	macro
	.set	reorder

$L202:
	.set	noreorder
	.set	nomacro
	jal	rtl_dst_release
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L178
	move	$6,$0
	.set	macro
	.set	reorder

$L201:
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L313
	move	$6,$21
	.set	macro
	.set	reorder

	addiu	$4,$18,16
	.set	noreorder
	.set	nomacro
	jal	del_timer
	sw	$7,28($sp)
	.set	macro
	.set	reorder

	lw	$2,4($18)
	lw	$4,8($18)
	lhu	$3,2($18)
	sb	$0,0($18)
	xor	$4,$4,$2
	lbu	$2,1($18)
	srl	$5,$4,16
	xor	$3,$3,$4
	xor	$3,$3,$5
	srl	$4,$3,8
	xor	$2,$3,$2
	lui	$3,%hi(max_udp_frag_entry)
	xor	$2,$2,$4
	lw	$4,%lo(max_udp_frag_entry)($3)
	lw	$5,44($18)
	addiu	$4,$4,-1
	and	$2,$2,$4
	lw	$7,28($sp)
	lw	$4,48($18)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L206
	lui	$3,%hi(udp_cache_table)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L207
	sw	$4,48($5)
	.set	macro
	.set	reorder

$L206:
	li	$6,12			# 0xc
	mult	$2,$6
	lw	$5,%lo(udp_cache_table)($3)
	#nop
	lw	$5,0($5)
	mflo	$6
	addu	$5,$5,$6
	sw	$4,4($5)
$L207:
	lw	$4,48($18)
	lw	$5,44($18)
	#nop
	sw	$5,0($4)
	li	$4,12			# 0xc
	mult	$2,$4
	lw	$3,%lo(udp_cache_table)($3)
	#nop
	lw	$3,0($3)
	mflo	$2
	addu	$2,$3,$2
	lw	$3,8($2)
	#nop
	addiu	$3,$3,-1
	sw	$3,8($2)
	lui	$2,%hi(udp_cache_list_free)
	addiu	$2,$2,%lo(udp_cache_list_free)
	lw	$3,4($2)
	sw	$0,52($18)
	sw	$3,56($18)
	sw	$18,0($3)
	lw	$3,8($2)
	addiu	$18,$18,52
	sw	$18,4($2)
	addiu	$3,$3,1
	sw	$3,8($2)
	move	$6,$21
$L313:
	move	$4,$20
	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	fast_path_before_nat_check
	sw	$7,28($sp)
	.set	macro
	.set	reorder

	move	$6,$2
	li	$2,1			# 0x1
	lw	$7,28($sp)
	.set	noreorder
	.set	nomacro
	beq	$6,$2,$L178
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$21,$2,$L208
	li	$3,1048576			# 0x100000
	.set	macro
	.set	reorder

	lw	$2,12($19)
	#nop
	and	$2,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L208
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$3,2($16)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L208
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

	lw	$7,28($sp)
$L208:
	lbu	$2,1($17)
	#nop
	sltu	$3,$2,13
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L209
	lui	$3,%hi($L215)
	.set	macro
	.set	reorder

	sll	$2,$2,2
	addiu	$3,$3,%lo($L215)
	addu	$2,$3,$2
	lw	$2,0($2)
	lhu	$4,10($16)
	j	$2
	.section	.rodata.enter_fast_path,"a",@progbits
	.align	2
	.align	2
$L215:
	.word	$L210
	.word	$L211
	.word	$L212
	.word	$L212
	.word	$L213
	.word	$L209
	.word	$L209
	.word	$L209
	.word	$L214
	.word	$L209
	.word	$L209
	.word	$L209
	.word	$L214
	.section	.text.enter_fast_path
$L211:
	lw	$2,24($17)
	srl	$3,$22,16
	andi	$22,$22,0xffff
	andi	$4,$2,0xffff
	srl	$2,$2,16
	addu	$2,$4,$2
	subu	$2,$3,$2
	lhu	$3,10($16)
	addu	$22,$2,$22
	addu	$22,$22,$3
	.set	noreorder
	.set	nomacro
	bgez	$22,$L216
	sra	$2,$22,16
	.set	macro
	.set	reorder

	subu	$22,$0,$22
	sra	$2,$22,16
	andi	$22,$22,0xffff
	addu	$22,$2,$22
	sra	$2,$22,16
	addu	$22,$22,$2
	.set	noreorder
	.set	nomacro
	j	$L283
	nor	$22,$0,$22
	.set	macro
	.set	reorder

$L216:
	andi	$22,$22,0xffff
	addu	$22,$2,$22
	sra	$2,$22,16
	addu	$22,$22,$2
$L283:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L225
	sh	$22,10($16)
	.set	macro
	.set	reorder

	li	$2,17			# 0x11
	lw	$3,24($17)
	.set	noreorder
	.set	nomacro
	bne	$21,$2,$L219
	andi	$5,$3,0xffff
	.set	macro
	.set	reorder

	beq	$7,$0,$L225
	lhu	$2,0($7)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L225
	andi	$6,$3,0xffff
	.set	macro
	.set	reorder

	lw	$4,12($16)
	srl	$3,$3,16
	srl	$5,$4,16
	addu	$3,$6,$3
	subu	$3,$5,$3
	andi	$4,$4,0xffff
	addu	$3,$3,$4
	addu	$3,$3,$2
	.set	noreorder
	.set	nomacro
	bgez	$3,$L220
	sra	$2,$3,16
	.set	macro
	.set	reorder

	subu	$3,$0,$3
	sra	$2,$3,16
	andi	$3,$3,0xffff
	addu	$3,$2,$3
	sra	$2,$3,16
	addu	$3,$3,$2
	.set	noreorder
	.set	nomacro
	j	$L285
	nor	$3,$0,$3
	.set	macro
	.set	reorder

$L220:
	andi	$3,$3,0xffff
	addu	$3,$2,$3
	sra	$2,$3,16
	addu	$3,$3,$2
$L285:
	.set	noreorder
	.set	nomacro
	j	$L225
	sh	$3,0($7)
	.set	macro
	.set	reorder

$L219:
	lw	$4,12($16)
	srl	$3,$3,16
	srl	$2,$4,16
	addu	$3,$5,$3
	lhu	$5,0($7)
	subu	$2,$2,$3
	andi	$4,$4,0xffff
	addu	$2,$2,$4
	addu	$2,$2,$5
	.set	noreorder
	.set	nomacro
	bgez	$2,$L221
	sra	$3,$2,16
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	sra	$3,$2,16
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L284
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L221:
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
$L284:
	sh	$2,0($7)
	lw	$2,12($19)
	li	$3,1048576			# 0x100000
	and	$2,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L225
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$3,2($16)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L225
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

	j	$L225
$L212:
	lw	$2,24($17)
	srl	$3,$22,16
	andi	$22,$22,0xffff
	srl	$4,$2,16
	andi	$2,$2,0xffff
	addu	$2,$4,$2
	subu	$2,$3,$2
	lhu	$3,10($16)
	addu	$22,$2,$22
	addu	$22,$22,$3
	.set	noreorder
	.set	nomacro
	bgez	$22,$L223
	sra	$2,$22,16
	.set	macro
	.set	reorder

	subu	$22,$0,$22
	sra	$2,$22,16
	andi	$22,$22,0xffff
	addu	$22,$2,$22
	sra	$2,$22,16
	addu	$22,$22,$2
	.set	noreorder
	.set	nomacro
	j	$L286
	nor	$22,$0,$22
	.set	macro
	.set	reorder

$L223:
	andi	$22,$22,0xffff
	addu	$22,$2,$22
	sra	$2,$22,16
	addu	$22,$22,$2
$L286:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L225
	sh	$22,10($16)
	.set	macro
	.set	reorder

	li	$2,17			# 0x11
	bne	$21,$2,$L226
	beq	$7,$0,$L227
	lhu	$3,0($7)
	#nop
	beq	$3,$0,$L227
	lw	$5,24($17)
	lw	$4,12($16)
	srl	$6,$5,16
	andi	$5,$5,0xffff
	srl	$2,$4,16
	addu	$5,$6,$5
	lhu	$6,0($19)
	subu	$2,$2,$5
	andi	$4,$4,0xffff
	addu	$2,$2,$4
	addu	$2,$2,$6
	lw	$6,28($17)
	#nop
	subu	$2,$2,$6
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	bgez	$2,$L228
	sra	$3,$2,16
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	sra	$3,$2,16
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L288
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L228:
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
$L288:
	.set	noreorder
	.set	nomacro
	j	$L227
	sh	$2,0($7)
	.set	macro
	.set	reorder

$L226:
	lw	$4,24($17)
	lw	$3,12($16)
	andi	$5,$4,0xffff
	srl	$4,$4,16
	addu	$4,$5,$4
	srl	$2,$3,16
	lhu	$5,0($19)
	subu	$2,$2,$4
	andi	$3,$3,0xffff
	addu	$2,$2,$3
	addu	$2,$2,$5
	lw	$5,28($17)
	lhu	$4,0($7)
	subu	$2,$2,$5
	addu	$2,$2,$4
	.set	noreorder
	.set	nomacro
	bgez	$2,$L229
	sra	$3,$2,16
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	sra	$3,$2,16
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L287
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L229:
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
$L287:
	sh	$2,0($7)
	lw	$2,12($19)
	li	$3,1048576			# 0x100000
	and	$2,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L227
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$3,2($16)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L227
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

$L227:
	lw	$2,28($17)
	#nop
	sh	$2,0($19)
$L225:
	lw	$2,24($17)
	.set	noreorder
	.set	nomacro
	j	$L210
	sw	$2,12($16)
	.set	macro
	.set	reorder

$L213:
	lw	$2,24($sp)
	andi	$23,$23,0xffff
	addu	$23,$23,$2
	lw	$2,32($17)
	#nop
	andi	$3,$2,0xffff
	subu	$3,$23,$3
	srl	$2,$2,16
	subu	$3,$3,$2
	addu	$3,$3,$4
	.set	noreorder
	.set	nomacro
	bgez	$3,$L231
	sra	$2,$3,16
	.set	macro
	.set	reorder

	subu	$3,$0,$3
	sra	$2,$3,16
	andi	$3,$3,0xffff
	addu	$3,$2,$3
	sra	$2,$3,16
	addu	$3,$3,$2
	.set	noreorder
	.set	nomacro
	j	$L289
	nor	$3,$0,$3
	.set	macro
	.set	reorder

$L231:
	andi	$3,$3,0xffff
	addu	$3,$2,$3
	sra	$2,$3,16
	addu	$3,$3,$2
$L289:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L299
	sh	$3,10($16)
	.set	macro
	.set	reorder

	li	$3,17			# 0x11
	lw	$2,32($17)
	.set	noreorder
	.set	nomacro
	bne	$21,$3,$L234
	andi	$3,$2,0xffff
	.set	macro
	.set	reorder

	beq	$7,$0,$L315
	lhu	$3,0($7)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L315
	andi	$4,$2,0xffff
	.set	macro
	.set	reorder

	subu	$23,$23,$4
	srl	$2,$2,16
	subu	$2,$23,$2
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	bgez	$2,$L235
	sra	$3,$2,16
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	sra	$3,$2,16
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L290
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L235:
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
$L290:
	.set	noreorder
	.set	nomacro
	j	$L299
	sh	$2,0($7)
	.set	macro
	.set	reorder

$L234:
	subu	$23,$23,$3
	lhu	$3,0($7)
	srl	$2,$2,16
	subu	$23,$23,$2
	addu	$23,$23,$3
	.set	noreorder
	.set	nomacro
	bgez	$23,$L236
	sra	$2,$23,16
	.set	macro
	.set	reorder

	subu	$23,$0,$23
	sra	$2,$23,16
	andi	$23,$23,0xffff
	addu	$23,$2,$23
	sra	$2,$23,16
	addu	$23,$23,$2
	.set	noreorder
	.set	nomacro
	j	$L291
	nor	$23,$0,$23
	.set	macro
	.set	reorder

$L236:
	andi	$23,$23,0xffff
	addu	$23,$2,$23
	sra	$2,$23,16
	addu	$23,$23,$2
$L291:
	.set	noreorder
	.set	nomacro
	j	$L299
	sh	$23,0($7)
	.set	macro
	.set	reorder

$L214:
	lw	$2,32($17)
	lw	$6,24($sp)
	andi	$23,$23,0xffff
	andi	$3,$2,0xffff
	addu	$23,$23,$6
	subu	$3,$23,$3
	srl	$2,$2,16
	subu	$3,$3,$2
	addu	$3,$3,$4
	.set	noreorder
	.set	nomacro
	bgez	$3,$L237
	sra	$2,$3,16
	.set	macro
	.set	reorder

	subu	$3,$0,$3
	sra	$2,$3,16
	andi	$3,$3,0xffff
	addu	$3,$2,$3
	sra	$2,$3,16
	addu	$3,$3,$2
	.set	noreorder
	.set	nomacro
	j	$L292
	nor	$3,$0,$3
	.set	macro
	.set	reorder

$L237:
	andi	$3,$3,0xffff
	addu	$3,$2,$3
	sra	$2,$3,16
	addu	$3,$3,$2
$L292:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L299
	sh	$3,10($16)
	.set	macro
	.set	reorder

	li	$2,17			# 0x11
	bne	$21,$2,$L240
	beq	$7,$0,$L241
	lhu	$3,0($7)
	#nop
	beq	$3,$0,$L241
	lw	$4,32($17)
	lhu	$5,2($19)
	andi	$2,$4,0xffff
	subu	$2,$23,$2
	srl	$4,$4,16
	subu	$2,$2,$4
	addu	$2,$2,$5
	lw	$5,36($17)
	#nop
	subu	$2,$2,$5
	.set	noreorder
	.set	nomacro
	j	$L305
	addu	$2,$2,$3
	.set	macro
	.set	reorder

$L240:
	lw	$3,32($17)
	lhu	$4,2($19)
	andi	$2,$3,0xffff
	subu	$2,$23,$2
	srl	$3,$3,16
	subu	$2,$2,$3
	addu	$2,$2,$4
	lw	$4,36($17)
	#nop
	subu	$2,$2,$4
	lhu	$4,0($7)
	#nop
	addu	$2,$2,$4
$L305:
	.set	noreorder
	.set	nomacro
	bgez	$2,$L243
	sra	$3,$2,16
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	sra	$3,$2,16
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L293
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L243:
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
$L293:
	sh	$2,0($7)
$L241:
	lw	$2,36($17)
	.set	noreorder
	.set	nomacro
	j	$L299
	sh	$2,2($19)
	.set	macro
	.set	reorder

$L209:
	lw	$3,24($17)
	andi	$2,$22,0xffff
	srl	$22,$22,16
	addu	$22,$2,$22
	lhu	$4,10($16)
	andi	$2,$3,0xffff
	subu	$2,$22,$2
	srl	$3,$3,16
	subu	$2,$2,$3
	addu	$2,$2,$4
	.set	noreorder
	.set	nomacro
	bgez	$2,$L244
	sra	$3,$2,16
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	sra	$3,$2,16
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L294
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L244:
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
$L294:
	sh	$2,10($16)
	lw	$2,24($sp)
	andi	$23,$23,0xffff
	lhu	$3,10($16)
	addu	$23,$23,$2
	lw	$2,32($17)
	#nop
	andi	$4,$2,0xffff
	subu	$4,$23,$4
	srl	$2,$2,16
	subu	$4,$4,$2
	addu	$4,$4,$3
	.set	noreorder
	.set	nomacro
	bgez	$4,$L246
	sra	$2,$4,16
	.set	macro
	.set	reorder

	subu	$4,$0,$4
	sra	$2,$4,16
	andi	$4,$4,0xffff
	addu	$4,$2,$4
	sra	$2,$4,16
	addu	$4,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L295
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L246:
	andi	$4,$4,0xffff
	addu	$4,$2,$4
	sra	$2,$4,16
	addu	$4,$4,$2
$L295:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L248
	sh	$4,10($16)
	.set	macro
	.set	reorder

	li	$2,17			# 0x11
	bne	$21,$2,$L249
	beq	$7,$0,$L250
	lhu	$2,0($7)
	#nop
	beq	$2,$0,$L250
	lw	$4,24($17)
	lhu	$5,0($19)
	andi	$3,$4,0xffff
	subu	$3,$22,$3
	srl	$4,$4,16
	subu	$3,$3,$4
	addu	$3,$3,$5
	lw	$5,28($17)
	#nop
	subu	$3,$3,$5
	.set	noreorder
	.set	nomacro
	j	$L309
	addu	$3,$3,$2
	.set	macro
	.set	reorder

$L249:
	lw	$2,24($17)
	lhu	$4,0($19)
	andi	$3,$2,0xffff
	subu	$3,$22,$3
	srl	$2,$2,16
	subu	$3,$3,$2
	addu	$3,$3,$4
	lw	$4,28($17)
	#nop
	subu	$3,$3,$4
	lhu	$4,0($7)
	#nop
	addu	$3,$3,$4
$L309:
	.set	noreorder
	.set	nomacro
	bgez	$3,$L254
	sra	$2,$3,16
	.set	macro
	.set	reorder

	subu	$3,$0,$3
	sra	$2,$3,16
	andi	$3,$3,0xffff
	addu	$3,$2,$3
	sra	$2,$3,16
	addu	$3,$3,$2
	.set	noreorder
	.set	nomacro
	j	$L297
	nor	$3,$0,$3
	.set	macro
	.set	reorder

$L254:
	andi	$3,$3,0xffff
	addu	$3,$2,$3
	sra	$2,$3,16
	addu	$3,$3,$2
$L297:
	sh	$3,0($7)
	lw	$3,32($17)
	lhu	$4,2($19)
	andi	$2,$3,0xffff
	subu	$2,$23,$2
	srl	$3,$3,16
	subu	$2,$2,$3
	addu	$2,$2,$4
	lw	$4,36($17)
	#nop
	subu	$2,$2,$4
	lhu	$4,0($7)
	#nop
	addu	$2,$2,$4
	.set	noreorder
	.set	nomacro
	bgez	$2,$L256
	sra	$3,$2,16
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	sra	$3,$2,16
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L298
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L256:
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
$L298:
	sh	$2,0($7)
$L250:
	lw	$2,28($17)
	#nop
	sh	$2,0($19)
	lw	$2,36($17)
	#nop
	sh	$2,2($19)
$L248:
	lw	$2,24($17)
	#nop
	sw	$2,12($16)
$L299:
	lw	$2,32($17)
	#nop
$L315:
	sw	$2,16($16)
$L210:
	lbu	$2,8($16)
	lbu	$3,9($16)
	lhu	$5,10($16)
	addiu	$4,$2,-1
	andi	$4,$4,0x00ff
	sb	$4,8($16)
	sll	$2,$2,8
	sll	$4,$4,8
	or	$2,$2,$3
	or	$3,$4,$3
	subu	$2,$2,$3
	addu	$2,$2,$5
	.set	noreorder
	.set	nomacro
	bgez	$2,$L257
	sra	$3,$2,16
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	sra	$3,$2,16
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L300
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L257:
	andi	$2,$2,0xffff
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$2,$3
$L300:
	sh	$2,10($16)
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	move	$5,$0
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dev
	move	$5,$0
	.set	macro
	.set	reorder

	lw	$5,48($17)
	lw	$6,52($17)
	.set	noreorder
	.set	nomacro
	jal	fastpath_set_qos_mark
	move	$4,$20
	.set	macro
	.set	reorder

	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	move	$6,$20
	move	$4,$16
	sw	$2,40($17)
	.set	noreorder
	.set	nomacro
	jal	fast_path_post_process_xmit_check
	move	$5,$19
	.set	macro
	.set	reorder

	li	$6,1			# 0x1
	.set	noreorder
	.set	nomacro
	beq	$2,$6,$L178
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	ip_finish_output3
	sw	$6,48($sp)
	.set	macro
	.set	reorder

$L301:
	lw	$6,48($sp)
	j	$L178
$L195:
	lw	$17,64($17)
	#nop
$L194:
	bne	$17,$0,$L259
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_inDev
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$16
	move	$5,$19
	move	$6,$20
	lw	$31,92($sp)
	lw	$fp,88($sp)
	lw	$23,84($sp)
	lw	$22,80($sp)
	lw	$21,76($sp)
	lw	$20,72($sp)
	lw	$19,68($sp)
	lw	$18,64($sp)
	lw	$17,60($sp)
	lw	$16,56($sp)
	.set	noreorder
	.set	nomacro
	j	fast_path_post_process_return_check
	addiu	$sp,$sp,96
	.set	macro
	.set	reorder

$L272:
	move	$6,$0
$L178:
	lw	$31,92($sp)
	lw	$fp,88($sp)
	lw	$23,84($sp)
	lw	$22,80($sp)
	lw	$21,76($sp)
	lw	$20,72($sp)
	lw	$19,68($sp)
	lw	$18,64($sp)
	lw	$17,60($sp)
	lw	$16,56($sp)
	move	$2,$6
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,96
	.set	macro
	.set	reorder

$L181:
	lw	$2,12($19)
	li	$3,65536			# 0x10000
	and	$3,$2,$3
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L280
	li	$3,393216			# 0x60000
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L312
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

$L310:
	mult	$7,$6
	lui	$4,%hi(udp_cache_table)
	lw	$4,%lo(udp_cache_table)($4)
	#nop
	lw	$4,0($4)
	mflo	$6
	addu	$4,$4,$6
	lw	$18,0($4)
	.set	noreorder
	.set	nomacro
	j	$L262
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

$L311:
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L263
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L182
	li	$fp,1			# 0x1
	.set	macro
	.set	reorder

	.end	enter_fast_path
	.size	enter_fast_path, .-enter_fast_path
	.section	.rodata.str1.4
	.align	2
$LC1:
	.ascii	"napt num: %d, path num: %d.\012\000"
	.section	.text.fastpath_dump_napt_entry_num,"ax",@progbits
	.align	2
	.globl	fastpath_dump_napt_entry_num
	.set	nomips16
	.ent	fastpath_dump_napt_entry_num
	.type	fastpath_dump_napt_entry_num, @function
fastpath_dump_napt_entry_num:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$2,%hi(path_list_inuse)
	move	$16,$5
	lw	$2,%lo(path_list_inuse)($2)
	j	$L317
	move	$7,$0

$L318:
	lw	$2,72($2)
	addiu	$7,$7,1
$L317:
	bne	$2,$0,$L318
	move	$6,$0

	lui	$2,%hi(napt_list_inuse)
	lw	$2,%lo(napt_list_inuse)($2)
	j	$L319
	nop

$L320:
	lw	$2,44($2)
	addiu	$6,$6,1
$L319:
	bne	$2,$0,$L320
	lui	$5,%hi($LC1)

	addu	$4,$4,$16
	jal	sprintf
	addiu	$5,$5,%lo($LC1)

	addu	$2,$16,$2
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fastpath_dump_napt_entry_num
	.size	fastpath_dump_napt_entry_num, .-fastpath_dump_napt_entry_num
	.section	.text.init_table_napt,"ax",@progbits
	.align	2
	.globl	init_table_napt
	.set	nomips16
	.ent	init_table_napt
	.type	init_table_napt, @function
init_table_napt:
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
	lui	$16,%hi(kmalloc_caches)
	addiu	$2,$16,%lo(kmalloc_caches)
	move	$17,$4
	lw	$4,20($2)
	move	$21,$5
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	lui	$18,%hi(table_napt)
	move	$22,$16
	move	$20,$2
	sw	$2,%lo(table_napt)($18)
	beq	$2,$0,$L322
	li	$16,-1			# 0xffffffffffffffff

	li	$4,12			# 0xc
	mult	$17,$4
	lui	$3,%hi(napt_list_inuse)
	addiu	$2,$3,%lo(napt_list_inuse)
	lui	$19,%hi(napt_list_free)
	sw	$0,8($2)
	sw	$2,4($2)
	addiu	$2,$19,%lo(napt_list_free)
	sw	$0,8($2)
	sw	$2,4($2)
	li	$5,32			# 0x20
	lui	$2,%hi(napt_table_list_max)
	sw	$0,%lo(napt_list_inuse)($3)
	sw	$0,%lo(napt_list_free)($19)
	sw	$17,%lo(napt_table_list_max)($2)
	jal	__kmalloc
	mflo	$4

	sw	$2,0($20)
	lw	$2,%lo(table_napt)($18)
	nop
	lw	$2,0($2)
	nop
	beq	$2,$0,$L322
	move	$2,$0

	j	$L323
	li	$4,12			# 0xc

$L324:
	lw	$3,%lo(table_napt)($18)
	addiu	$2,$2,1
	lw	$6,0($3)
	mflo	$5
	addu	$6,$6,$5
	sw	$0,8($6)
	lw	$3,0($3)
	nop
	addu	$3,$3,$5
	sw	$0,0($3)
	lw	$3,%lo(table_napt)($18)
	nop
	lw	$3,0($3)
	nop
	addu	$5,$3,$5
	sw	$5,4($5)
$L323:
	slt	$3,$2,$17
	bne	$3,$0,$L324
	mult	$2,$4

	j	$L325
	move	$16,$0

$L326:
	lw	$4,24($2)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	beq	$2,$0,$L329
	addiu	$3,$19,%lo(napt_list_free)

	lw	$4,4($3)
	sw	$0,44($2)
	addiu	$16,$16,1
	sw	$4,48($2)
	sw	$2,0($4)
	addiu	$2,$2,44
	sw	$2,4($3)
	lw	$2,8($3)
	nop
	addiu	$2,$2,1
	sw	$2,8($3)
$L325:
	slt	$2,$16,$21
	bne	$2,$0,$L326
	addiu	$2,$22,%lo(kmalloc_caches)

	j	$L322
	move	$16,$0

$L329:
	li	$16,-2			# 0xfffffffffffffffe
$L322:
	move	$2,$16
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
	.end	init_table_napt
	.size	init_table_napt, .-init_table_napt
	.section	.text.init_table_path,"ax",@progbits
	.align	2
	.globl	init_table_path
	.set	nomips16
	.ent	init_table_path
	.type	init_table_path, @function
init_table_path:
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
	lui	$16,%hi(kmalloc_caches)
	addiu	$2,$16,%lo(kmalloc_caches)
	move	$17,$4
	lw	$4,20($2)
	move	$21,$5
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	lui	$18,%hi(table_path)
	move	$22,$16
	move	$20,$2
	sw	$2,%lo(table_path)($18)
	beq	$2,$0,$L331
	li	$16,-1			# 0xffffffffffffffff

	li	$4,12			# 0xc
	mult	$17,$4
	lui	$3,%hi(path_list_inuse)
	addiu	$2,$3,%lo(path_list_inuse)
	lui	$19,%hi(path_list_free)
	sw	$0,8($2)
	sw	$2,4($2)
	addiu	$2,$19,%lo(path_list_free)
	sw	$0,8($2)
	sw	$2,4($2)
	li	$5,32			# 0x20
	lui	$2,%hi(path_table_list_max)
	sw	$0,%lo(path_list_inuse)($3)
	sw	$0,%lo(path_list_free)($19)
	sw	$17,%lo(path_table_list_max)($2)
	jal	__kmalloc
	mflo	$4

	sw	$2,0($20)
	lw	$2,%lo(table_path)($18)
	nop
	lw	$2,0($2)
	nop
	beq	$2,$0,$L331
	move	$2,$0

	j	$L332
	li	$4,12			# 0xc

$L333:
	lw	$3,%lo(table_path)($18)
	addiu	$2,$2,1
	lw	$6,0($3)
	mflo	$5
	addu	$6,$6,$5
	sw	$0,8($6)
	lw	$3,0($3)
	nop
	addu	$3,$3,$5
	sw	$0,0($3)
	lw	$3,%lo(table_path)($18)
	nop
	lw	$3,0($3)
	nop
	addu	$5,$3,$5
	sw	$5,4($5)
$L332:
	slt	$3,$2,$17
	bne	$3,$0,$L333
	mult	$2,$4

	j	$L334
	move	$16,$0

$L335:
	lw	$4,4($2)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	beq	$2,$0,$L338
	addiu	$3,$19,%lo(path_list_free)

	lw	$4,4($3)
	sw	$0,72($2)
	addiu	$16,$16,1
	sw	$4,76($2)
	sw	$2,0($4)
	addiu	$2,$2,72
	sw	$2,4($3)
	lw	$2,8($3)
	nop
	addiu	$2,$2,1
	sw	$2,8($3)
$L334:
	slt	$2,$16,$21
	bne	$2,$0,$L335
	addiu	$2,$22,%lo(kmalloc_caches)

	j	$L331
	move	$16,$0

$L338:
	li	$16,-2			# 0xfffffffffffffffe
$L331:
	move	$2,$16
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
	.end	init_table_path
	.size	init_table_path, .-init_table_path
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	"%s:%s\012\000"
	.align	2
$LC3:
	.ascii	"Realtek FastPath\000"
	.align	2
$LC4:
	.ascii	"v1.03\000"
	.section	.text.get_fastpath_module_info,"ax",@progbits
	.align	2
	.globl	get_fastpath_module_info
	.set	nomips16
	.ent	get_fastpath_module_info
	.type	get_fastpath_module_info, @function
get_fastpath_module_info:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L341
	nop

	lui	$5,%hi($LC2)
	lui	$6,%hi($LC3)
	lui	$7,%hi($LC4)
	addiu	$5,$5,%lo($LC2)
	addiu	$6,$6,%lo($LC3)
	addiu	$7,$7,%lo($LC4)
	j	sprintf
	nop

$L341:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	get_fastpath_module_info
	.size	get_fastpath_module_info, .-get_fastpath_module_info
	.globl	negative_cache_table
	.section	.bss.negative_cache_table,"aw",@nobits
	.align	2
	.type	negative_cache_table, @object
	.size	negative_cache_table, 4
negative_cache_table:
	.space	4
	.globl	negative_cache_list_free
	.section	.bss.negative_cache_list_free,"aw",@nobits
	.align	2
	.type	negative_cache_list_free, @object
	.size	negative_cache_list_free, 12
negative_cache_list_free:
	.space	12
	.globl	udp_cache_table
	.section	.bss.udp_cache_table,"aw",@nobits
	.align	2
	.type	udp_cache_table, @object
	.size	udp_cache_table, 4
udp_cache_table:
	.space	4
	.globl	udp_cache_list_free
	.section	.bss.udp_cache_list_free,"aw",@nobits
	.align	2
	.type	udp_cache_list_free, @object
	.size	udp_cache_list_free, 12
udp_cache_list_free:
	.space	12
	.globl	table_path
	.section	.bss.table_path,"aw",@nobits
	.align	2
	.type	table_path, @object
	.size	table_path, 4
table_path:
	.space	4
	.globl	path_list_free
	.section	.bss.path_list_free,"aw",@nobits
	.align	2
	.type	path_list_free, @object
	.size	path_list_free, 12
path_list_free:
	.space	12
	.globl	path_list_inuse
	.section	.bss.path_list_inuse,"aw",@nobits
	.align	2
	.type	path_list_inuse, @object
	.size	path_list_inuse, 12
path_list_inuse:
	.space	12
	.globl	table_napt
	.section	.bss.table_napt,"aw",@nobits
	.align	2
	.type	table_napt, @object
	.size	table_napt, 4
table_napt:
	.space	4
	.globl	napt_list_free
	.section	.bss.napt_list_free,"aw",@nobits
	.align	2
	.type	napt_list_free, @object
	.size	napt_list_free, 12
napt_list_free:
	.space	12
	.globl	napt_list_inuse
	.section	.bss.napt_list_inuse,"aw",@nobits
	.align	2
	.type	napt_list_inuse, @object
	.size	napt_list_inuse, 12
napt_list_inuse:
	.space	12
	.globl	route_list_free
	.section	.bss.route_list_free,"aw",@nobits
	.align	2
	.type	route_list_free, @object
	.size	route_list_free, 12
route_list_free:
	.space	12
	.globl	route_list_inuse
	.section	.bss.route_list_inuse,"aw",@nobits
	.align	2
	.type	route_list_inuse, @object
	.size	route_list_inuse, 12
route_list_inuse:
	.space	12
	.section	.bss.max_udp_frag_entry,"aw",@nobits
	.align	2
	.type	max_udp_frag_entry, @object
	.size	max_udp_frag_entry, 4
max_udp_frag_entry:
	.space	4
	.section	.bss.napt_table_list_max,"aw",@nobits
	.align	2
	.type	napt_table_list_max, @object
	.size	napt_table_list_max, 4
napt_table_list_max:
	.space	4
	.section	.bss.path_table_list_max,"aw",@nobits
	.align	2
	.type	path_table_list_max, @object
	.size	path_table_list_max, 4
path_table_list_max:
	.space	4
	.ident	"GCC: (Realtek RSDK-4.6.4 Build 2080) 4.6.4"
