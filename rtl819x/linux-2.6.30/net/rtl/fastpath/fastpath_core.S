	.file	1 "fastpath_core.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
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
	j	$L18
	move	$2,$0

$L21:
	sltu	$7,$5,$6
	bne	$7,$0,$L19
	and	$8,$4,$5

	lw	$7,4($3)
	bnel	$8,$7,$L20
	move	$5,$6

	j	$L20
	addiu	$2,$3,16

$L19:
	move	$5,$6
$L20:
	lw	$3,44($3)
	move	$6,$5
$L18:
	bnel	$3,$0,$L21
	lw	$5,8($3)

	j	$31
	nop

	.set	macro
	.set	reorder
	.end	FastPath_Route
	.size	FastPath_Route, .-FastPath_Route
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
	
	srl	$2,$4,8
	xor	$2,$2,$4
	srl	$3,$6,16
	andi	$5,$5,0xffff
	xor	$2,$2,$6
	xor	$2,$2,$3
	srl	$8,$5,4
	lhu	$3,18($sp)
	xor	$2,$2,$8
	andi	$7,$7,0xffff
	lui	$9,%hi(path_table_list_max)
	xor	$2,$2,$7
	lw	$9,%lo(path_table_list_max)($9)
	xor	$2,$2,$3
	srl	$10,$2,12
	xor	$2,$10,$2
	addiu	$9,$9,-1
	and	$2,$2,$9
	li	$10,12			# 0xc
	mul	$11,$2,$10
	lui	$8,%hi(table_path)
	lw	$8,%lo(table_path)($8)
	li	$10,255			# 0xff
	lw	$8,4($8)
	addu	$2,$11,$8
	j	$L25
	lw	$2,0($2)

$L29:
	bnel	$8,$5,$L25
	lw	$2,64($2)

	lw	$8,8($2)
	bnel	$8,$7,$L25
	lw	$2,64($2)

	lw	$8,16($2)
	bnel	$8,$4,$L25
	lw	$2,64($2)

	lw	$8,20($2)
	bnel	$8,$6,$L25
	lw	$2,64($2)

	lbu	$11,0($2)
	bnel	$11,$10,$L26
	lw	$2,64($2)

	lbu	$10,2($2)
	li	$6,1			# 0x1
	bne	$10,$6,$L27
	sb	$0,0($2)

	lw	$6,24($2)
	j	$L28
	lhu	$10,30($2)

$L27:
	lw	$6,32($2)
	j	$L28
	lhu	$10,38($2)

$L26:
$L25:
	bnel	$2,$0,$L29
	lw	$8,12($2)

	j	$31
	nop

$L28:
	beq	$6,$0,$L38
	nop

	lbu	$11,2($2)
	li	$2,1			# 0x1
	bnel	$11,$2,$L31
	srl	$2,$4,16

	srl	$2,$8,8
	xor	$2,$2,$8
	xor	$3,$2,$3
	srl	$2,$7,4
	xor	$3,$3,$2
	xor	$3,$3,$6
	xor	$10,$3,$10
	j	$L37
	srl	$6,$6,16

$L31:
	xor	$2,$2,$4
	xor	$2,$2,$5
	xor	$3,$2,$3
	xor	$3,$3,$6
	srl	$6,$6,8
	xor	$6,$3,$6
	srl	$10,$10,4
$L37:
	xor	$10,$6,$10
	srl	$6,$10,12
	xor	$10,$6,$10
	and	$9,$10,$9
	li	$3,12			# 0xc
	mul	$10,$9,$3
	lui	$2,%hi(table_path)
	lw	$2,%lo(table_path)($2)
	li	$6,255			# 0xff
	lw	$2,4($2)
	addu	$9,$10,$2
	j	$L33
	lw	$2,0($9)

$L35:
	bnel	$3,$8,$L33
	lw	$2,64($2)

	lw	$3,28($2)
	bnel	$3,$7,$L33
	lw	$2,64($2)

	lw	$3,32($2)
	bnel	$3,$4,$L33
	lw	$2,64($2)

	lw	$3,36($2)
	bnel	$3,$5,$L33
	lw	$2,64($2)

	lbu	$3,0($2)
	beql	$3,$6,$L34
	sb	$0,0($2)

$L34:
	lw	$2,64($2)
$L33:
	bnel	$2,$0,$L35
	lw	$3,24($2)

$L38:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	mark_path_invalid
	.size	mark_path_invalid, .-mark_path_invalid
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"%s:%s\012\000"
	.align	2
$LC1:
	.ascii	"Realtek FastPath\000"
	.align	2
$LC2:
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
	
	beq	$4,$0,$L43
	lui	$5,%hi($LC0)

	lui	$6,%hi($LC1)
	lui	$7,%hi($LC2)
	addiu	$5,$5,%lo($LC0)
	addiu	$6,$6,%lo($LC1)
	addiu	$7,$7,%lo($LC2)
	j	sprintf
	nop

$L43:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	get_fastpath_module_info
	.size	get_fastpath_module_info, .-get_fastpath_module_info
	.section	.rodata.str1.4
	.align	2
$LC3:
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
	j	$L45
	move	$7,$0

$L46:
	addiu	$7,$7,1
$L45:
	bnel	$2,$0,$L46
	lw	$2,72($2)

	lui	$2,%hi(napt_list_inuse)
	lw	$2,%lo(napt_list_inuse)($2)
	j	$L47
	move	$6,$0

$L48:
	addiu	$6,$6,1
$L47:
	bnel	$2,$0,$L48
	lw	$2,44($2)

	lui	$5,%hi($LC3)
	addu	$4,$4,$16
	jal	sprintf
	addiu	$5,$5,%lo($LC3)

	addu	$2,$2,$16
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	fastpath_dump_napt_entry_num
	.size	fastpath_dump_napt_entry_num, .-fastpath_dump_napt_entry_num
	.section	.text.init_table_path,"ax",@progbits
	.align	2
	.globl	init_table_path
	.set	nomips16
	.ent	init_table_path
	.type	init_table_path, @function
init_table_path:
	.frame	$sp,40,$31		# vars= 0, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$2,%hi(kmalloc_caches+20)
	move	$17,$4
	lw	$4,%lo(kmalloc_caches+20)($2)
	move	$19,$5
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	lui	$16,%hi(table_path)
	move	$18,$2
	beq	$2,$0,$L51
	sw	$2,%lo(table_path)($16)

	li	$4,12			# 0xc
	mul	$4,$17,$4
	lui	$7,%hi(path_list_inuse)
	lui	$6,%hi(path_list_free)
	addiu	$2,$6,%lo(path_list_free)
	addiu	$3,$7,%lo(path_list_inuse)
	sw	$0,%lo(path_list_free)($6)
	li	$5,32			# 0x20
	lui	$6,%hi(path_table_list_max)
	sw	$3,4($3)
	sw	$2,4($2)
	sw	$0,8($3)
	sw	$0,8($2)
	sw	$0,%lo(path_list_inuse)($7)
	jal	__kmalloc
	sw	$17,%lo(path_table_list_max)($6)

	sw	$2,4($18)
	lw	$2,%lo(table_path)($16)
	lw	$2,4($2)
	beq	$2,$0,$L51
	move	$2,$0

	j	$L52
	li	$4,12			# 0xc

$L53:
	lw	$5,%lo(table_path)($16)
	addiu	$2,$2,1
	lw	$5,4($5)
	addu	$5,$5,$3
	sw	$0,8($5)
	lw	$5,%lo(table_path)($16)
	lw	$5,4($5)
	addu	$5,$5,$3
	sw	$0,0($5)
	lw	$5,%lo(table_path)($16)
	lw	$5,4($5)
	addu	$3,$5,$3
	sw	$3,4($3)
$L52:
	slt	$3,$2,$17
	bne	$3,$0,$L53
	mul	$3,$2,$4

	lui	$18,%hi(kmalloc_caches)
	lui	$16,%hi(path_list_free)
	addiu	$18,$18,%lo(kmalloc_caches)
	addiu	$16,$16,%lo(path_list_free)
	j	$L54
	move	$17,$0

$L57:
	lw	$4,4($18)
	jal	kmem_cache_alloc
	addiu	$17,$17,1

	bne	$2,$0,$L55
	addiu	$3,$2,72

	j	$L56
	li	$2,-2			# 0xfffffffffffffffe

$L55:
	sw	$0,72($2)
	lw	$4,4($16)
	sw	$4,76($2)
	lw	$4,4($16)
	sw	$2,0($4)
	lw	$2,8($16)
	sw	$3,4($16)
	addiu	$2,$2,1
	sw	$2,8($16)
$L54:
	slt	$2,$17,$19
	bne	$2,$0,$L57
	li	$5,32			# 0x20

	lui	$2,%hi(table_path)
	lw	$3,%lo(table_path)($2)
	move	$2,$0
	j	$L56
	sw	$0,0($3)

$L51:
	li	$2,-1			# 0xffffffffffffffff
$L56:
	lw	$31,36($sp)
	lw	$19,32($sp)
	lw	$18,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	init_table_path
	.size	init_table_path, .-init_table_path
	.section	.text.init_table_napt,"ax",@progbits
	.align	2
	.globl	init_table_napt
	.set	nomips16
	.ent	init_table_napt
	.type	init_table_napt, @function
init_table_napt:
	.frame	$sp,40,$31		# vars= 0, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$2,%hi(kmalloc_caches+20)
	move	$17,$4
	lw	$4,%lo(kmalloc_caches+20)($2)
	move	$19,$5
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	lui	$16,%hi(table_napt)
	move	$18,$2
	beq	$2,$0,$L60
	sw	$2,%lo(table_napt)($16)

	li	$4,12			# 0xc
	mul	$4,$17,$4
	lui	$7,%hi(napt_list_inuse)
	lui	$6,%hi(napt_list_free)
	addiu	$2,$6,%lo(napt_list_free)
	addiu	$3,$7,%lo(napt_list_inuse)
	sw	$0,%lo(napt_list_free)($6)
	li	$5,32			# 0x20
	lui	$6,%hi(napt_table_list_max)
	sw	$3,4($3)
	sw	$2,4($2)
	sw	$0,8($3)
	sw	$0,8($2)
	sw	$0,%lo(napt_list_inuse)($7)
	jal	__kmalloc
	sw	$17,%lo(napt_table_list_max)($6)

	sw	$2,4($18)
	lw	$2,%lo(table_napt)($16)
	lw	$2,4($2)
	beq	$2,$0,$L60
	move	$2,$0

	j	$L61
	li	$4,12			# 0xc

$L62:
	lw	$5,%lo(table_napt)($16)
	addiu	$2,$2,1
	lw	$5,4($5)
	addu	$5,$5,$3
	sw	$0,8($5)
	lw	$5,%lo(table_napt)($16)
	lw	$5,4($5)
	addu	$5,$5,$3
	sw	$0,0($5)
	lw	$5,%lo(table_napt)($16)
	lw	$5,4($5)
	addu	$3,$5,$3
	sw	$3,4($3)
$L61:
	slt	$3,$2,$17
	bne	$3,$0,$L62
	mul	$3,$2,$4

	lui	$18,%hi(kmalloc_caches)
	lui	$16,%hi(napt_list_free)
	addiu	$18,$18,%lo(kmalloc_caches)
	addiu	$16,$16,%lo(napt_list_free)
	j	$L63
	move	$17,$0

$L66:
	lw	$4,24($18)
	jal	kmem_cache_alloc
	addiu	$17,$17,1

	bne	$2,$0,$L64
	addiu	$3,$2,44

	j	$L65
	li	$2,-2			# 0xfffffffffffffffe

$L64:
	sw	$0,44($2)
	lw	$4,4($16)
	sw	$4,48($2)
	lw	$4,4($16)
	sw	$2,0($4)
	lw	$2,8($16)
	sw	$3,4($16)
	addiu	$2,$2,1
	sw	$2,8($16)
$L63:
	slt	$2,$17,$19
	bne	$2,$0,$L66
	li	$5,32			# 0x20

	lui	$2,%hi(table_napt)
	lw	$3,%lo(table_napt)($2)
	move	$2,$0
	j	$L65
	sw	$0,0($3)

$L60:
	li	$2,-1			# 0xffffffffffffffff
$L65:
	lw	$31,36($sp)
	lw	$19,32($sp)
	lw	$18,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	init_table_napt
	.size	init_table_napt, .-init_table_napt
	.section	.text.cache_timeout,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	cache_timeout
	.type	cache_timeout, @function
cache_timeout:
	.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	lbu	$2,0($4)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L72
	move	$16,$4
	.set	macro
	.set	reorder

	lw	$3,4($16)
	lw	$4,8($4)
	lhu	$2,2($16)
	lbu	$17,1($16)
	xor	$4,$4,$3
	lui	$3,%hi(max_udp_frag_entry)
	xor	$2,$2,$4
	lw	$3,%lo(max_udp_frag_entry)($3)
	srl	$4,$4,16
	lui	$18,%hi(udp_cache_table)
	xor	$2,$2,$4
	lw	$4,%lo(udp_cache_table)($18)
	xor	$17,$2,$17
	srl	$2,$2,8
	xor	$17,$17,$2
	addiu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	and	$17,$17,$3
	.set	macro
	.set	reorder

	lw	$3,44($16)
	sb	$0,0($16)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L70
	lw	$2,48($16)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L71
	sw	$2,48($3)
	.set	macro
	.set	reorder

$L70:
	li	$4,12			# 0xc
	mul	$5,$17,$4
	lw	$3,%lo(udp_cache_table)($18)
	lw	$3,4($3)
	addu	$3,$5,$3
	sw	$2,4($3)
$L71:
	li	$6,12			# 0xc
	mul	$7,$17,$6
	lw	$2,48($16)
	lw	$4,44($16)
	lui	$3,%hi(udp_cache_table)
	addiu	$5,$16,52
	sw	$4,0($2)
	lw	$4,%lo(udp_cache_table)($3)
	lui	$2,%hi(udp_cache_list_free)
	addiu	$2,$2,%lo(udp_cache_list_free)
	lw	$4,4($4)
	addu	$17,$7,$4
	lw	$4,8($17)
	addiu	$4,$4,-1
	sw	$4,8($17)
	sw	$0,52($16)
	lw	$4,4($2)
	sw	$4,56($16)
	lw	$4,4($2)
	sw	$16,0($4)
	lw	$4,8($2)
	lw	$3,%lo(udp_cache_table)($3)
	sw	$5,4($2)
	addiu	$4,$4,1
	sw	$4,8($2)
	lhu	$2,2($3)
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$2,2($3)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
$L72:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
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
	.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	move	$16,$4
	lw	$3,4($16)
	lw	$4,8($4)
	lhu	$2,2($16)
	lbu	$17,1($16)
	xor	$4,$4,$3
	lui	$3,%hi(max_udp_frag_entry)
	xor	$2,$2,$4
	lw	$3,%lo(max_udp_frag_entry)($3)
	srl	$4,$4,16
	lui	$18,%hi(negative_cache_table)
	xor	$2,$2,$4
	lw	$4,%lo(negative_cache_table)($18)
	xor	$17,$2,$17
	srl	$2,$2,8
	xor	$17,$17,$2
	addiu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	and	$17,$17,$3
	.set	macro
	.set	reorder

	lw	$3,40($16)
	sb	$0,0($16)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L74
	lw	$2,44($16)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L75
	sw	$2,44($3)
	.set	macro
	.set	reorder

$L74:
	li	$4,12			# 0xc
	mul	$5,$17,$4
	lw	$3,%lo(negative_cache_table)($18)
	addu	$3,$5,$3
	sw	$2,8($3)
$L75:
	li	$5,12			# 0xc
	mul	$6,$17,$5
	lw	$2,44($16)
	lw	$4,40($16)
	lui	$3,%hi(negative_cache_table)
	addiu	$5,$16,48
	sw	$4,0($2)
	lw	$4,%lo(negative_cache_table)($3)
	lui	$2,%hi(negative_cache_list_free)
	addiu	$2,$2,%lo(negative_cache_list_free)
	addu	$17,$6,$4
	lw	$4,12($17)
	addiu	$4,$4,-1
	sw	$4,12($17)
	sw	$0,48($16)
	lw	$4,4($2)
	sw	$4,52($16)
	lw	$4,4($2)
	sw	$16,0($4)
	lw	$4,8($2)
	lw	$3,%lo(negative_cache_table)($3)
	sw	$5,4($2)
	addiu	$4,$4,1
	sw	$4,8($2)
	lhu	$2,2($3)
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$2,2($3)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	negative_cache_timeout
	.size	negative_cache_timeout, .-negative_cache_timeout
	.section	.rodata.str1.4
	.align	2
$LC4:
	.ascii	"lo\000"
	.section	.text.enter_fast_path,"ax",@progbits
	.align	2
	.globl	enter_fast_path
	.set	nomips16
	.ent	enter_fast_path
	.type	enter_fast_path, @function
enter_fast_path:
	.frame	$sp,112,$31		# vars= 48, regs= 10/0, args= 24, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-112
	sw	$31,108($sp)
	sw	$fp,104($sp)
	sw	$23,100($sp)
	sw	$22,96($sp)
	sw	$21,92($sp)
	sw	$20,88($sp)
	sw	$19,84($sp)
	sw	$18,80($sp)
	sw	$17,76($sp)
	sw	$16,72($sp)
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
	sltu	$2,$2,2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L78
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	lbu	$22,9($16)
	.set	noreorder
	.set	nomacro
	beq	$22,$2,$L79
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$22,$2,$L80
	move	$2,$0
	.set	macro
	.set	reorder

$L79:
	lw	$19,0($16)
	move	$4,$16
	move	$6,$20
	ext	$19,$19,24,4
	sll	$19,$19,2
	addu	$19,$16,$19
	.set	noreorder
	.set	nomacro
	jal	fast_path_pre_process_check
	move	$5,$19
	.set	macro
	.set	reorder

	li	$3,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L212
	lw	$31,108($sp)
	.set	macro
	.set	reorder

	lui	$3,%hi($LC4)
	addiu	$3,$3,%lo($LC4)
 #APP
 # 77 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$4,($17)
1:	lbu	$1,($3)
	addiu	$17,1
	bne	$1,$4,2f
	addiu	$3,1
	bnez	$4,1b
	lbu	$4,($17)
	move	$4,$1
2:	subu	$4,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beql	$4,$0,$L80
	move	$2,$0
	.set	macro
	.set	reorder

	lhu	$5,0($19)
	lhu	$7,2($19)
	lhu	$3,6($16)
	lw	$21,12($16)
	lw	$23,16($16)
	sw	$5,40($sp)
	.set	noreorder
	.set	nomacro
	bne	$22,$2,$L81
	sw	$7,36($sp)
	.set	macro
	.set	reorder

	andi	$2,$3,0x3fff
	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L82
	lw	$2,12($19)
	.set	macro
	.set	reorder

	li	$2,8192			# 0x2000
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L214
	andi	$2,$3,0x3fff
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L209
	lw	$2,12($19)
	.set	macro
	.set	reorder

$L208:
	move	$4,$21
	move	$6,$23
	.set	noreorder
	.set	nomacro
	jal	mark_path_invalid
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L80
	move	$2,$0
	.set	macro
	.set	reorder

$L171:
	and	$2,$2,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L80
	move	$2,$0
	.set	macro
	.set	reorder

$L81:
	andi	$2,$3,0x3fff
$L214:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L83
	move	$18,$0
	.set	macro
	.set	reorder

	li	$2,8192			# 0x2000
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L84
	andi	$2,$3,0x2000
	.set	macro
	.set	reorder

	li	$2,1			# 0x1
	sw	$0,32($sp)
	.set	noreorder
	.set	nomacro
	j	$L85
	sw	$2,24($sp)
	.set	macro
	.set	reorder

$L84:
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L172
	sw	$0,32($sp)
	.set	macro
	.set	reorder

	andi	$3,$3,0x1fff
	.set	noreorder
	.set	nomacro
	beql	$3,$0,$L86
	sw	$0,32($sp)
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	sw	$3,32($sp)
$L86:
$L172:
	sw	$0,24($sp)
$L85:
	lhu	$3,4($16)
	lbu	$2,9($16)
	xor	$17,$23,$21
	lui	$4,%hi(max_udp_frag_entry)
	lw	$7,%lo(max_udp_frag_entry)($4)
	xor	$6,$3,$17
	srl	$fp,$17,16
	lui	$18,%hi(negative_cache_table)
	xor	$6,$6,$fp
	xor	$5,$6,$2
	lw	$4,%lo(negative_cache_table)($18)
	srl	$6,$6,8
	xor	$5,$5,$6
	addiu	$7,$7,-1
	and	$5,$5,$7
	sw	$2,56($sp)
	sw	$3,64($sp)
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	sw	$5,60($sp)
	.set	macro
	.set	reorder

	lw	$5,60($sp)
	li	$4,12			# 0xc
	lw	$6,%lo(negative_cache_table)($18)
	mul	$8,$5,$4
	li	$7,3			# 0x3
	lw	$2,56($sp)
	lw	$3,64($sp)
	addu	$5,$8,$6
	.set	noreorder
	.set	nomacro
	j	$L87
	lw	$4,4($5)
	.set	macro
	.set	reorder

$L89:
	.set	noreorder
	.set	nomacro
	bnel	$5,$3,$L87
	lw	$4,40($4)
	.set	macro
	.set	reorder

	lw	$5,4($4)
	.set	noreorder
	.set	nomacro
	bnel	$5,$21,$L87
	lw	$4,40($4)
	.set	macro
	.set	reorder

	lw	$5,8($4)
	.set	noreorder
	.set	nomacro
	bnel	$5,$23,$L87
	lw	$4,40($4)
	.set	macro
	.set	reorder

	lbu	$5,1($4)
	.set	noreorder
	.set	nomacro
	bnel	$5,$2,$L87
	lw	$4,40($4)
	.set	macro
	.set	reorder

	lbu	$5,0($4)
	.set	noreorder
	.set	nomacro
	bnel	$5,$7,$L88
	lw	$4,40($4)
	.set	macro
	.set	reorder

	lhu	$2,2($6)
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L195
	sh	$2,2($6)
	.set	macro
	.set	reorder

$L88:
$L87:
	.set	noreorder
	.set	nomacro
	bnel	$4,$0,$L89
	lhu	$5,2($4)
	.set	macro
	.set	reorder

	lhu	$2,2($6)
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$2,2($6)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	lw	$5,12($16)
	lw	$6,16($16)
	lhu	$3,4($16)
	lbu	$2,9($16)
	xor	$4,$6,$5
	lui	$7,%hi(max_udp_frag_entry)
	xor	$8,$3,$4
	lw	$9,%lo(max_udp_frag_entry)($7)
	srl	$4,$4,16
	lui	$18,%hi(udp_cache_table)
	xor	$8,$8,$4
	xor	$7,$8,$2
	lw	$4,%lo(udp_cache_table)($18)
	srl	$8,$8,8
	xor	$7,$7,$8
	addiu	$9,$9,-1
	and	$7,$7,$9
	sw	$2,56($sp)
	sw	$3,64($sp)
	sw	$5,60($sp)
	sw	$6,52($sp)
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	sw	$7,48($sp)
	.set	macro
	.set	reorder

	lw	$7,48($sp)
	li	$9,12			# 0xc
	lw	$4,%lo(udp_cache_table)($18)
	mul	$11,$7,$9
	li	$10,1			# 0x1
	lw	$2,56($sp)
	lw	$8,4($4)
	lw	$3,64($sp)
	lw	$5,60($sp)
	lw	$6,52($sp)
	addu	$7,$11,$8
	.set	noreorder
	.set	nomacro
	j	$L90
	lw	$18,0($7)
	.set	macro
	.set	reorder

$L94:
	.set	noreorder
	.set	nomacro
	bnel	$7,$3,$L90
	lw	$18,44($18)
	.set	macro
	.set	reorder

	lw	$7,4($18)
	.set	noreorder
	.set	nomacro
	bnel	$7,$5,$L90
	lw	$18,44($18)
	.set	macro
	.set	reorder

	lw	$7,8($18)
	.set	noreorder
	.set	nomacro
	bnel	$7,$6,$L90
	lw	$18,44($18)
	.set	macro
	.set	reorder

	lbu	$7,1($18)
	.set	noreorder
	.set	nomacro
	bnel	$7,$2,$L90
	lw	$18,44($18)
	.set	macro
	.set	reorder

	lbu	$7,0($18)
	.set	noreorder
	.set	nomacro
	bnel	$7,$10,$L91
	lw	$18,44($18)
	.set	macro
	.set	reorder

	lhu	$2,2($4)
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$2,2($4)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	lhu	$2,12($18)
	lw	$4,24($sp)
	move	$fp,$0
	sw	$2,40($sp)
	lhu	$3,14($18)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L93
	sw	$3,36($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L210
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L91:
$L90:
	.set	noreorder
	.set	nomacro
	bnel	$18,$0,$L94
	lhu	$7,2($18)
	.set	macro
	.set	reorder

	lhu	$2,2($4)
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$2,2($4)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	lw	$5,24($sp)
	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L92
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

	lui	$19,%hi(negative_cache_table)
	lw	$4,%lo(negative_cache_table)($19)
	lui	$2,%hi(max_udp_frag_entry)
	lbu	$18,9($16)
	lw	$20,%lo(max_udp_frag_entry)($2)
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	lhu	$16,4($16)
	.set	macro
	.set	reorder

	lui	$3,%hi(negative_cache_list_free)
	lw	$2,%lo(negative_cache_list_free)($3)
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L95
	lw	$5,48($2)
	.set	macro
	.set	reorder

	lw	$2,%lo(negative_cache_table)($19)
$L196:
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
$L195:
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L80
	move	$2,$0
	.set	macro
	.set	reorder

$L95:
	li	$4,3			# 0x3
	sb	$4,0($2)
	sw	$21,4($2)
	sw	$23,8($2)
	sh	$16,2($2)
	sb	$18,1($2)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L96
	lw	$4,52($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L97
	sw	$4,52($5)
	.set	macro
	.set	reorder

$L96:
	addiu	$3,$3,%lo(negative_cache_list_free)
	sw	$4,4($3)
$L97:
	xor	$17,$16,$17
	xor	$fp,$17,$fp
	xor	$18,$fp,$18
	srl	$fp,$fp,8
	xor	$fp,$18,$fp
	addiu	$20,$20,-1
	and	$20,$fp,$20
	li	$3,12			# 0xc
	mul	$20,$20,$3
	lw	$5,48($2)
	lw	$3,52($2)
	lui	$4,%hi(negative_cache_list_free)
	addiu	$4,$4,%lo(negative_cache_list_free)
	sw	$5,0($3)
	lw	$6,8($4)
	lui	$3,%hi(negative_cache_table)
	addiu	$5,$2,40
	addiu	$6,$6,-1
	sw	$6,8($4)
	sw	$0,40($2)
	lw	$4,%lo(negative_cache_table)($3)
	addu	$4,$4,$20
	lw	$4,8($4)
	sw	$4,44($2)
	lw	$4,%lo(negative_cache_table)($3)
	addu	$4,$4,$20
	lw	$4,8($4)
	sw	$2,0($4)
	lw	$4,%lo(negative_cache_table)($3)
	addu	$4,$4,$20
	sw	$5,8($4)
	lw	$4,%lo(negative_cache_table)($3)
	addu	$20,$4,$20
	lw	$4,12($20)
	addiu	$4,$4,1
	sw	$4,12($20)
	lw	$3,%lo(negative_cache_table)($3)
	lhu	$4,2($3)
	addiu	$4,$4,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$4,2($3)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	lui	$3,%hi(jiffies)
	lw	$3,%lo(jiffies)($3)
	addiu	$4,$2,12
	addiu	$3,$3,1000
	.set	noreorder
	.set	nomacro
	jal	add_timer
	sw	$3,20($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L80
	move	$2,$0
	.set	macro
	.set	reorder

$L83:
	sw	$0,32($sp)
	.set	noreorder
	.set	nomacro
	j	$L173
	sw	$0,24($sp)
	.set	macro
	.set	reorder

$L92:
$L210:
	sw	$2,24($sp)
$L173:
	li	$fp,1			# 0x1
$L93:
	lui	$17,%hi(table_path)
	lw	$4,%lo(table_path)($17)
	srl	$3,$23,16
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	sw	$3,28($sp)
	.set	macro
	.set	reorder

	lw	$4,28($sp)
	srl	$2,$21,8
	xor	$3,$23,$21
	xor	$3,$3,$2
	lw	$5,36($sp)
	lw	$6,40($sp)
	xor	$3,$3,$4
	xor	$3,$3,$22
	lui	$4,%hi(path_table_list_max)
	xor	$3,$3,$5
	lw	$5,%lo(path_table_list_max)($4)
	srl	$4,$6,4
	xor	$3,$3,$4
	srl	$6,$3,12
	xor	$3,$6,$3
	addiu	$5,$5,-1
	and	$3,$3,$5
	li	$5,12			# 0xc
	mul	$6,$3,$5
	lw	$2,%lo(table_path)($17)
	li	$8,255			# 0xff
	li	$7,2			# 0x2
	lw	$4,4($2)
	li	$5,17			# 0x11
	addu	$3,$6,$4
	lw	$17,0($3)
	li	$6,1			# 0x1
	.set	noreorder
	.set	nomacro
	j	$L98
	li	$4,6			# 0x6
	.set	macro
	.set	reorder

$L167:
	lw	$9,40($sp)
	.set	noreorder
	.set	nomacro
	bnel	$3,$9,$L98
	lw	$17,64($17)
	.set	macro
	.set	reorder

	lw	$3,8($17)
	lw	$11,36($sp)
	.set	noreorder
	.set	nomacro
	bnel	$3,$11,$L98
	lw	$17,64($17)
	.set	macro
	.set	reorder

	lw	$3,16($17)
	.set	noreorder
	.set	nomacro
	bnel	$3,$21,$L98
	lw	$17,64($17)
	.set	macro
	.set	reorder

	lw	$3,20($17)
	.set	noreorder
	.set	nomacro
	bne	$3,$23,$L99
	sw	$3,44($sp)
	.set	macro
	.set	reorder

	lbu	$3,0($17)
	.set	noreorder
	.set	nomacro
	bnel	$3,$8,$L98
	lw	$17,64($17)
	.set	macro
	.set	reorder

	lw	$3,4($17)
	bne	$3,$7,$L100
	.set	noreorder
	.set	nomacro
	bnel	$22,$4,$L98
	lw	$17,64($17)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L211
	lw	$5,32($17)
	.set	macro
	.set	reorder

$L100:
	.set	noreorder
	.set	nomacro
	bnel	$3,$6,$L98
	lw	$17,64($17)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bnel	$22,$5,$L98
	lw	$17,64($17)
	.set	macro
	.set	reorder

	lw	$5,32($17)
$L211:
	lbu	$7,1($16)
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_route_input
	move	$6,$21
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L215
	lui	$2,%hi(table_path)
	.set	macro
	.set	reorder

	lw	$5,32($17)
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_dst_check
	move	$4,$20
	.set	macro
	.set	reorder

	li	$3,-1			# 0xffffffffffffffff
	beq	$2,$3,$L203
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$22,$2,$L105
	addiu	$3,$19,16
	.set	macro
	.set	reorder

	lhu	$3,6($19)
	addiu	$2,$19,6
	movz	$2,$0,$3
	move	$3,$2
$L105:
	lw	$4,24($sp)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L107
	lw	$2,32($sp)
	.set	macro
	.set	reorder

	lui	$2,%hi(udp_cache_table)
	lui	$5,%hi(max_udp_frag_entry)
	lw	$4,%lo(udp_cache_table)($2)
	lw	$5,%lo(max_udp_frag_entry)($5)
	lhu	$18,4($16)
	sw	$2,56($sp)
	sw	$3,64($sp)
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	sw	$5,60($sp)
	.set	macro
	.set	reorder

	lui	$4,%hi(udp_cache_list_free)
	lw	$6,%lo(udp_cache_list_free)($4)
	lw	$2,56($sp)
	lw	$3,64($sp)
	.set	noreorder
	.set	nomacro
	bne	$6,$0,$L108
	lw	$5,60($sp)
	.set	macro
	.set	reorder

	lw	$2,%lo(udp_cache_table)($2)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
$L203:
	.set	noreorder
	.set	nomacro
	jal	rtl_dst_release
	move	$4,$20
	.set	macro
	.set	reorder

	lui	$2,%hi(table_path)
$L215:
	.set	noreorder
	.set	nomacro
	j	$L196
	lw	$2,%lo(table_path)($2)
	.set	macro
	.set	reorder

$L108:
	lw	$8,40($sp)
	li	$2,1			# 0x1
	lw	$7,52($6)
	sh	$8,12($6)
	lw	$9,36($sp)
	sb	$2,0($6)
	sh	$18,2($6)
	sh	$9,14($6)
	sw	$21,4($6)
	lw	$11,44($sp)
	lw	$2,56($6)
	sb	$22,1($6)
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L109
	sw	$11,8($6)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L110
	sw	$2,56($7)
	.set	macro
	.set	reorder

$L109:
	addiu	$4,$4,%lo(udp_cache_list_free)
	sw	$2,4($4)
$L110:
	lw	$4,44($sp)
	addiu	$5,$5,-1
	lw	$7,52($6)
	xor	$2,$4,$21
	xor	$18,$18,$2
	srl	$2,$2,16
	xor	$18,$18,$2
	srl	$2,$18,8
	xor	$18,$18,$22
	xor	$18,$18,$2
	and	$5,$18,$5
	li	$2,12			# 0xc
	mul	$5,$5,$2
	lw	$2,56($6)
	lui	$4,%hi(udp_cache_list_free)
	addiu	$4,$4,%lo(udp_cache_list_free)
	sw	$7,0($2)
	lw	$8,8($4)
	lui	$2,%hi(udp_cache_table)
	addiu	$7,$6,44
	addiu	$8,$8,-1
	sw	$8,8($4)
	sw	$0,44($6)
	lw	$4,%lo(udp_cache_table)($2)
	lw	$4,4($4)
	addu	$4,$4,$5
	lw	$4,4($4)
	sw	$4,48($6)
	lw	$4,%lo(udp_cache_table)($2)
	lw	$4,4($4)
	addu	$4,$4,$5
	lw	$4,4($4)
	sw	$6,0($4)
	lw	$4,%lo(udp_cache_table)($2)
	lw	$4,4($4)
	addu	$4,$4,$5
	sw	$7,4($4)
	lw	$4,%lo(udp_cache_table)($2)
	lw	$4,4($4)
	addu	$5,$4,$5
	lw	$4,8($5)
	addiu	$4,$4,1
	sw	$4,8($5)
	lw	$2,%lo(udp_cache_table)($2)
	lhu	$4,2($2)
	addiu	$4,$4,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$4,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	addiu	$4,$6,16
	addiu	$2,$2,1000
	sw	$2,24($6)
	.set	noreorder
	.set	nomacro
	jal	add_timer
	sw	$3,64($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L111
	lw	$3,64($sp)
	.set	macro
	.set	reorder

$L107:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L216
	move	$4,$20
	.set	macro
	.set	reorder

	addiu	$4,$18,16
	.set	noreorder
	.set	nomacro
	jal	del_timer
	sw	$3,64($sp)
	.set	macro
	.set	reorder

	lw	$5,8($18)
	lw	$4,4($18)
	lhu	$6,2($18)
	lbu	$2,1($18)
	xor	$4,$5,$4
	lui	$5,%hi(max_udp_frag_entry)
	lw	$7,%lo(max_udp_frag_entry)($5)
	xor	$6,$6,$4
	srl	$4,$4,16
	lui	$5,%hi(udp_cache_table)
	xor	$6,$6,$4
	xor	$2,$6,$2
	lw	$4,%lo(udp_cache_table)($5)
	srl	$6,$6,8
	xor	$2,$2,$6
	addiu	$7,$7,-1
	and	$2,$2,$7
	sw	$2,56($sp)
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	sw	$5,60($sp)
	.set	macro
	.set	reorder

	lw	$6,44($18)
	sb	$0,0($18)
	lw	$4,48($18)
	lw	$2,56($sp)
	lw	$3,64($sp)
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L112
	lw	$5,60($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L113
	sw	$4,48($6)
	.set	macro
	.set	reorder

$L112:
	li	$6,12			# 0xc
	mul	$7,$2,$6
	lw	$5,%lo(udp_cache_table)($5)
	lw	$5,4($5)
	addu	$5,$7,$5
	sw	$4,4($5)
$L113:
	li	$8,12			# 0xc
	mul	$9,$2,$8
	lw	$4,48($18)
	lw	$6,44($18)
	lui	$5,%hi(udp_cache_table)
	addiu	$7,$18,52
	sw	$6,0($4)
	lw	$6,%lo(udp_cache_table)($5)
	lui	$4,%hi(udp_cache_list_free)
	addiu	$4,$4,%lo(udp_cache_list_free)
	lw	$6,4($6)
	addu	$2,$9,$6
	lw	$6,8($2)
	addiu	$6,$6,-1
	sw	$6,8($2)
	sw	$0,52($18)
	lw	$2,4($4)
	sw	$2,56($18)
	lw	$2,4($4)
	sw	$18,0($2)
	lw	$6,8($4)
	lw	$2,%lo(udp_cache_table)($5)
	sw	$7,4($4)
	addiu	$5,$6,1
	sw	$5,8($4)
	lhu	$4,2($2)
	addiu	$4,$4,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$4,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
$L111:
	move	$4,$20
$L216:
	move	$5,$16
	move	$6,$22
	.set	noreorder
	.set	nomacro
	jal	fast_path_before_nat_check
	sw	$3,64($sp)
	.set	macro
	.set	reorder

	li	$4,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L114
	lw	$3,64($sp)
	.set	macro
	.set	reorder

	lui	$3,%hi(table_path)
	lw	$3,%lo(table_path)($3)
	lhu	$4,2($3)
	addiu	$4,$4,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$4,2($3)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L212
	lw	$31,108($sp)
	.set	macro
	.set	reorder

$L114:
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bnel	$22,$2,$L217
	lbu	$2,1($17)
	.set	macro
	.set	reorder

	lw	$2,12($19)
	ext	$2,$2,20,1
	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L217
	lbu	$2,1($17)
	.set	macro
	.set	reorder

	lhu	$4,2($16)
	li	$2,40			# 0x28
	.set	noreorder
	.set	nomacro
	bnel	$4,$2,$L217
	lbu	$2,1($17)
	.set	macro
	.set	reorder

	move	$4,$20
	li	$5,40			# 0x28
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	sw	$3,64($sp)
	.set	macro
	.set	reorder

	lw	$3,64($sp)
	lbu	$2,1($17)
$L217:
	sltu	$4,$2,13
	.set	noreorder
	.set	nomacro
	beql	$4,$0,$L218
	lw	$4,24($17)
	.set	macro
	.set	reorder

	lui	$4,%hi($L122)
	addiu	$4,$4,%lo($L122)
	sll	$2,$2,2
	addu	$2,$4,$2
	lw	$2,0($2)
	.set	noreorder
	.set	nomacro
	j	$2
	lhu	$4,10($16)
	.set	macro
	.set	reorder

	.section	.rodata.enter_fast_path,"a",@progbits
	.align	2
	.align	2
$L122:
	.word	$L117
	.word	$L118
	.word	$L119
	.word	$L119
	.word	$L120
	.word	$L116
	.word	$L116
	.word	$L116
	.word	$L121
	.word	$L116
	.word	$L116
	.word	$L116
	.word	$L121
	.section	.text.enter_fast_path
$L118:
	lw	$2,24($17)
	srl	$5,$21,16
	andi	$21,$21,0xffff
	srl	$6,$2,16
	subu	$6,$0,$6
	andi	$2,$2,0xffff
	subu	$2,$6,$2
	addu	$2,$2,$5
	addu	$21,$2,$21
	addu	$4,$21,$4
	.set	noreorder
	.set	nomacro
	bgez	$4,$L123
	andi	$21,$4,0xffff
	.set	macro
	.set	reorder

	subu	$4,$0,$4
	andi	$2,$4,0xffff
	sra	$4,$4,16
	addu	$4,$2,$4
	sra	$2,$4,16
	addu	$4,$2,$4
	.set	noreorder
	.set	nomacro
	j	$L174
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L123:
	sra	$4,$4,16
	addu	$4,$21,$4
	sra	$2,$4,16
	addu	$4,$2,$4
$L174:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L132
	sh	$4,10($16)
	.set	macro
	.set	reorder

	li	$2,17			# 0x11
	.set	noreorder
	.set	nomacro
	bnel	$22,$2,$L126
	lw	$5,24($17)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$3,$0,$L219
	lw	$2,24($17)
	.set	macro
	.set	reorder

	lhu	$4,0($3)
	.set	noreorder
	.set	nomacro
	beql	$4,$0,$L219
	lw	$2,24($17)
	.set	macro
	.set	reorder

	lw	$6,24($17)
	lw	$5,12($16)
	srl	$2,$6,16
	subu	$2,$0,$2
	andi	$6,$6,0xffff
	srl	$7,$5,16
	subu	$2,$2,$6
	addu	$2,$2,$7
	andi	$5,$5,0xffff
	addu	$2,$2,$5
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	bgez	$2,$L127
	andi	$4,$2,0xffff
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	andi	$4,$2,0xffff
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L176
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L127:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L176:
	.set	noreorder
	.set	nomacro
	j	$L132
	sh	$2,0($3)
	.set	macro
	.set	reorder

$L126:
	lw	$4,12($16)
	lhu	$2,0($3)
	srl	$6,$5,16
	subu	$6,$0,$6
	andi	$5,$5,0xffff
	subu	$5,$6,$5
	srl	$6,$4,16
	addu	$5,$5,$6
	andi	$4,$4,0xffff
	addu	$4,$5,$4
	addu	$2,$2,$4
	.set	noreorder
	.set	nomacro
	bgez	$2,$L128
	andi	$4,$2,0xffff
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	andi	$4,$2,0xffff
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L175
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L128:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L175:
	sh	$2,0($3)
	lw	$2,12($19)
	ext	$2,$2,20,1
	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L219
	lw	$2,24($17)
	.set	macro
	.set	reorder

	lhu	$3,2($16)
	li	$2,40			# 0x28
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L219
	lw	$2,24($17)
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L213
	lw	$2,24($17)
	.set	macro
	.set	reorder

$L119:
	lw	$2,24($17)
	srl	$5,$21,16
	andi	$21,$21,0xffff
	srl	$6,$2,16
	subu	$6,$0,$6
	andi	$2,$2,0xffff
	subu	$2,$6,$2
	addu	$2,$2,$5
	addu	$21,$2,$21
	addu	$4,$21,$4
	.set	noreorder
	.set	nomacro
	bgez	$4,$L130
	andi	$21,$4,0xffff
	.set	macro
	.set	reorder

	subu	$4,$0,$4
	andi	$2,$4,0xffff
	sra	$4,$4,16
	addu	$4,$2,$4
	sra	$2,$4,16
	addu	$4,$2,$4
	.set	noreorder
	.set	nomacro
	j	$L177
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L130:
	sra	$4,$4,16
	addu	$4,$21,$4
	sra	$2,$4,16
	addu	$4,$2,$4
$L177:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L132
	sh	$4,10($16)
	.set	macro
	.set	reorder

	li	$2,17			# 0x11
	.set	noreorder
	.set	nomacro
	bnel	$22,$2,$L133
	lw	$5,24($17)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$3,$0,$L220
	lw	$2,28($17)
	.set	macro
	.set	reorder

	lhu	$4,0($3)
	.set	noreorder
	.set	nomacro
	beql	$4,$0,$L220
	lw	$2,28($17)
	.set	macro
	.set	reorder

	lw	$6,24($17)
	lw	$5,12($16)
	lhu	$8,0($19)
	srl	$2,$6,16
	subu	$2,$0,$2
	andi	$6,$6,0xffff
	srl	$7,$5,16
	subu	$2,$2,$6
	addu	$2,$2,$7
	andi	$5,$5,0xffff
	lw	$7,28($17)
	addu	$2,$2,$5
	addu	$2,$2,$8
	subu	$2,$2,$7
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	bgez	$2,$L135
	andi	$4,$2,0xffff
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	andi	$4,$2,0xffff
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L179
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L135:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L179:
	.set	noreorder
	.set	nomacro
	j	$L134
	sh	$2,0($3)
	.set	macro
	.set	reorder

$L133:
	lw	$4,12($16)
	lhu	$8,0($19)
	srl	$2,$5,16
	subu	$2,$0,$2
	andi	$5,$5,0xffff
	srl	$6,$4,16
	subu	$2,$2,$5
	addu	$2,$2,$6
	andi	$4,$4,0xffff
	lw	$7,28($17)
	addu	$2,$2,$4
	lhu	$6,0($3)
	addu	$2,$2,$8
	subu	$2,$2,$7
	addu	$2,$6,$2
	.set	noreorder
	.set	nomacro
	bgez	$2,$L136
	andi	$4,$2,0xffff
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	andi	$4,$2,0xffff
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L178
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L136:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L178:
	sh	$2,0($3)
	lw	$2,12($19)
	ext	$2,$2,20,1
	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L220
	lw	$2,28($17)
	.set	macro
	.set	reorder

	lhu	$3,2($16)
	li	$2,40			# 0x28
	.set	noreorder
	.set	nomacro
	bnel	$3,$2,$L220
	lw	$2,28($17)
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

$L134:
	lw	$2,28($17)
$L220:
	sh	$2,0($19)
$L132:
	lw	$2,24($17)
$L213:
$L219:
	.set	noreorder
	.set	nomacro
	j	$L117
	sw	$2,12($16)
	.set	macro
	.set	reorder

$L120:
	lw	$5,32($17)
	lw	$11,28($sp)
	andi	$23,$23,0xffff
	andi	$21,$5,0xffff
	addu	$2,$23,$11
	subu	$21,$2,$21
	srl	$5,$5,16
	subu	$5,$21,$5
	addu	$4,$5,$4
	.set	noreorder
	.set	nomacro
	bgez	$4,$L138
	andi	$21,$4,0xffff
	.set	macro
	.set	reorder

	subu	$4,$0,$4
	andi	$5,$4,0xffff
	sra	$4,$4,16
	addu	$4,$5,$4
	sra	$5,$4,16
	addu	$4,$5,$4
	.set	noreorder
	.set	nomacro
	j	$L180
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L138:
	sra	$4,$4,16
	addu	$4,$21,$4
	sra	$5,$4,16
	addu	$4,$5,$4
$L180:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L189
	sh	$4,10($16)
	.set	macro
	.set	reorder

	li	$4,17			# 0x11
	.set	noreorder
	.set	nomacro
	bnel	$22,$4,$L141
	lw	$4,32($17)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$3,$0,$L221
	lw	$2,32($17)
	.set	macro
	.set	reorder

	lhu	$4,0($3)
	.set	noreorder
	.set	nomacro
	beql	$4,$0,$L221
	lw	$2,32($17)
	.set	macro
	.set	reorder

	lw	$5,32($17)
	andi	$23,$5,0xffff
	subu	$23,$2,$23
	srl	$5,$5,16
	subu	$23,$23,$5
	.set	noreorder
	.set	nomacro
	j	$L198
	addu	$23,$4,$23
	.set	macro
	.set	reorder

$L141:
	lhu	$23,0($3)
	andi	$5,$4,0xffff
	subu	$2,$2,$5
	srl	$4,$4,16
	subu	$2,$2,$4
	addu	$23,$23,$2
$L198:
	.set	noreorder
	.set	nomacro
	bgez	$23,$L143
	andi	$2,$23,0xffff
	.set	macro
	.set	reorder

	subu	$23,$0,$23
	andi	$2,$23,0xffff
	sra	$23,$23,16
	addu	$23,$2,$23
	sra	$2,$23,16
	addu	$23,$2,$23
	.set	noreorder
	.set	nomacro
	j	$L181
	nor	$23,$0,$23
	.set	macro
	.set	reorder

$L143:
	sra	$23,$23,16
	addu	$23,$2,$23
	sra	$2,$23,16
	addu	$23,$2,$23
$L181:
	.set	noreorder
	.set	nomacro
	j	$L189
	sh	$23,0($3)
	.set	macro
	.set	reorder

$L121:
	lw	$2,32($17)
	lw	$5,28($sp)
	andi	$23,$23,0xffff
	andi	$21,$2,0xffff
	addu	$23,$23,$5
	subu	$21,$23,$21
	srl	$2,$2,16
	subu	$21,$21,$2
	addu	$21,$21,$4
	.set	noreorder
	.set	nomacro
	bgez	$21,$L144
	andi	$2,$21,0xffff
	.set	macro
	.set	reorder

	subu	$21,$0,$21
	andi	$2,$21,0xffff
	sra	$21,$21,16
	addu	$21,$2,$21
	sra	$2,$21,16
	addu	$21,$2,$21
	.set	noreorder
	.set	nomacro
	j	$L182
	nor	$21,$0,$21
	.set	macro
	.set	reorder

$L144:
	sra	$21,$21,16
	addu	$21,$2,$21
	sra	$2,$21,16
	addu	$21,$2,$21
$L182:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L189
	sh	$21,10($16)
	.set	macro
	.set	reorder

	li	$2,17			# 0x11
	.set	noreorder
	.set	nomacro
	bnel	$22,$2,$L147
	lw	$4,32($17)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$3,$0,$L222
	lw	$2,36($17)
	.set	macro
	.set	reorder

	lhu	$4,0($3)
	.set	noreorder
	.set	nomacro
	beql	$4,$0,$L222
	lw	$2,36($17)
	.set	macro
	.set	reorder

	lw	$5,32($17)
	lhu	$7,2($19)
	lw	$6,36($17)
	andi	$2,$5,0xffff
	subu	$2,$23,$2
	srl	$5,$5,16
	subu	$2,$2,$5
	addu	$2,$2,$7
	subu	$2,$2,$6
	.set	noreorder
	.set	nomacro
	j	$L200
	addu	$2,$4,$2
	.set	macro
	.set	reorder

$L147:
	lhu	$7,2($19)
	lw	$6,36($17)
	andi	$2,$4,0xffff
	subu	$2,$23,$2
	srl	$4,$4,16
	subu	$2,$2,$4
	lhu	$5,0($3)
	addu	$2,$2,$7
	subu	$2,$2,$6
	addu	$2,$5,$2
$L200:
	.set	noreorder
	.set	nomacro
	bgez	$2,$L150
	andi	$4,$2,0xffff
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	andi	$4,$2,0xffff
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L183
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L150:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L183:
	sh	$2,0($3)
	lw	$2,36($17)
$L222:
	.set	noreorder
	.set	nomacro
	j	$L189
	sh	$2,2($19)
	.set	macro
	.set	reorder

$L116:
	lw	$4,24($17)
$L218:
	srl	$2,$21,16
	andi	$21,$21,0xffff
	addu	$21,$21,$2
	lhu	$5,10($16)
	andi	$2,$4,0xffff
	subu	$2,$21,$2
	srl	$4,$4,16
	subu	$2,$2,$4
	addu	$2,$2,$5
	.set	noreorder
	.set	nomacro
	bgez	$2,$L151
	andi	$4,$2,0xffff
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	andi	$4,$2,0xffff
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L184
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L151:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L184:
	sh	$2,10($16)
	lw	$2,32($17)
	lw	$6,28($sp)
	andi	$23,$23,0xffff
	andi	$5,$2,0xffff
	addu	$23,$23,$6
	lhu	$4,10($16)
	subu	$5,$23,$5
	srl	$2,$2,16
	subu	$5,$5,$2
	addu	$5,$5,$4
	.set	noreorder
	.set	nomacro
	bgez	$5,$L153
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	andi	$2,$5,0xffff
	sra	$5,$5,16
	addu	$5,$2,$5
	sra	$2,$5,16
	addu	$5,$2,$5
	.set	noreorder
	.set	nomacro
	j	$L185
	nor	$5,$0,$5
	.set	macro
	.set	reorder

$L153:
	sra	$5,$5,16
	addu	$5,$2,$5
	sra	$2,$5,16
	addu	$5,$2,$5
$L185:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L155
	sh	$5,10($16)
	.set	macro
	.set	reorder

	li	$2,17			# 0x11
	.set	noreorder
	.set	nomacro
	bnel	$22,$2,$L156
	lw	$2,24($17)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$3,$0,$L223
	lw	$2,28($17)
	.set	macro
	.set	reorder

	lhu	$2,0($3)
	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L223
	lw	$2,28($17)
	.set	macro
	.set	reorder

	lw	$5,24($17)
	lhu	$7,0($19)
	lw	$6,28($17)
	andi	$4,$5,0xffff
	subu	$4,$21,$4
	srl	$5,$5,16
	subu	$4,$4,$5
	addu	$4,$4,$7
	subu	$4,$4,$6
	.set	noreorder
	.set	nomacro
	j	$L206
	addu	$4,$2,$4
	.set	macro
	.set	reorder

$L156:
	lhu	$7,0($19)
	lw	$6,28($17)
	andi	$4,$2,0xffff
	subu	$4,$21,$4
	srl	$2,$2,16
	subu	$4,$4,$2
	lhu	$5,0($3)
	addu	$4,$4,$7
	subu	$4,$4,$6
	addu	$4,$5,$4
$L206:
	.set	noreorder
	.set	nomacro
	bgez	$4,$L161
	andi	$2,$4,0xffff
	.set	macro
	.set	reorder

	subu	$4,$0,$4
	andi	$2,$4,0xffff
	sra	$4,$4,16
	addu	$4,$2,$4
	sra	$2,$4,16
	addu	$4,$2,$4
	.set	noreorder
	.set	nomacro
	j	$L187
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L161:
	sra	$4,$4,16
	addu	$4,$2,$4
	sra	$2,$4,16
	addu	$4,$2,$4
$L187:
	sh	$4,0($3)
	lw	$4,32($17)
	lhu	$7,2($19)
	lw	$6,36($17)
	andi	$2,$4,0xffff
	subu	$2,$23,$2
	srl	$4,$4,16
	subu	$2,$2,$4
	lhu	$5,0($3)
	addu	$2,$2,$7
	subu	$2,$2,$6
	addu	$2,$5,$2
	.set	noreorder
	.set	nomacro
	bgez	$2,$L163
	andi	$4,$2,0xffff
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	andi	$4,$2,0xffff
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	j	$L188
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L163:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L188:
	sh	$2,0($3)
	lw	$2,28($17)
$L223:
	sh	$2,0($19)
	lw	$2,36($17)
	sh	$2,2($19)
$L155:
	lw	$2,24($17)
	sw	$2,12($16)
$L189:
	lw	$2,32($17)
$L221:
	sw	$2,16($16)
$L117:
	lbu	$5,8($16)
	lbu	$4,9($16)
	lhu	$7,10($16)
	addiu	$3,$5,-1
	andi	$3,$3,0x00ff
	sll	$2,$5,8
	or	$2,$2,$4
	sll	$6,$3,8
	addu	$2,$2,$7
	or	$4,$6,$4
	subu	$2,$2,$4
	.set	noreorder
	.set	nomacro
	bgez	$2,$L164
	sb	$3,8($16)
	.set	macro
	.set	reorder

	subu	$2,$0,$2
	andi	$3,$2,0xffff
	sra	$2,$2,16
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$3,$2
	.set	noreorder
	.set	nomacro
	j	$L190
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L164:
	andi	$3,$2,0xffff
	sra	$2,$2,16
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$3,$2
$L190:
	sh	$2,10($16)
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	move	$4,$20
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
	move	$4,$16
	move	$5,$19
	sw	$2,40($17)
	.set	noreorder
	.set	nomacro
	jal	fast_path_post_process_xmit_check
	move	$6,$20
	.set	macro
	.set	reorder

	li	$4,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L166
	lui	$3,%hi(table_path)
	.set	macro
	.set	reorder

	lw	$2,%lo(table_path)($3)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L80
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L166:
	lw	$2,%lo(table_path)($3)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	jal	ip_finish_output3
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L80
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L99:
	lw	$17,64($17)
$L98:
	.set	noreorder
	.set	nomacro
	bnel	$17,$0,$L167
	lw	$3,12($17)
	.set	macro
	.set	reorder

	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_inDev
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$16
	move	$5,$19
	move	$6,$20
	lw	$31,108($sp)
	lw	$fp,104($sp)
	lw	$23,100($sp)
	lw	$22,96($sp)
	lw	$21,92($sp)
	lw	$20,88($sp)
	lw	$19,84($sp)
	lw	$18,80($sp)
	lw	$17,76($sp)
	lw	$16,72($sp)
	.set	noreorder
	.set	nomacro
	j	fast_path_post_process_return_check
	addiu	$sp,$sp,112
	.set	macro
	.set	reorder

$L78:
	move	$2,$0
$L80:
	lw	$31,108($sp)
$L212:
	lw	$fp,104($sp)
	lw	$23,100($sp)
	lw	$22,96($sp)
	lw	$21,92($sp)
	lw	$20,88($sp)
	lw	$19,84($sp)
	lw	$18,80($sp)
	lw	$17,76($sp)
	lw	$16,72($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,112
	.set	macro
	.set	reorder

$L82:
$L209:
	ext	$4,$2,16,1
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L171
	li	$4,393216			# 0x60000
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L208
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	.end	enter_fast_path
	.size	enter_fast_path, .-enter_fast_path
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
	move	$21,$4
	jal	rtl_get_ct_ip_by_dir
	move	$6,$0

	move	$4,$21
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$fp,$2

	move	$4,$21
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$22,$2

	move	$4,$21
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$19,$2

	move	$4,$21
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	move	$17,$2

	move	$4,$21
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_port_by_dir
	move	$23,$2

	move	$4,$21
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,24($sp)

	li	$6,1			# 0x1
	move	$4,$21
	move	$5,$0
	jal	rtl_get_ct_port_by_dir
	move	$18,$2

	move	$4,$21
	li	$5,1			# 0x1
	jal	rtl_get_ct_protonum
	move	$16,$2

	move	$4,$21
	move	$5,$0
	jal	rtl_get_ct_protonum
	move	$20,$2

	lw	$7,24($sp)
	move	$4,$fp
	move	$5,$23
	move	$6,$22
	move	$21,$2
	jal	mark_path_invalid
	sw	$20,16($sp)

	move	$4,$19
	move	$5,$18
	move	$6,$17
	move	$7,$16
	jal	mark_path_invalid
	sw	$21,16($sp)

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
	beq	$2,$3,$L229
	li	$3,450			# 0x1c2

	beq	$2,$3,$L230
	li	$3,150			# 0x96

	bne	$2,$3,$L237
	move	$4,$16

	j	$L236
	lui	$2,%hi(jiffies)

$L230:
	lui	$2,%hi(jiffies)
	lw	$5,%lo(jiffies)($2)
	j	$L234
	move	$4,$16

$L229:
	lui	$2,%hi(jiffies)
	lw	$5,%lo(jiffies)($2)
	move	$4,$16
	j	$L234
	addiu	$5,$5,-15000

$L236:
	lw	$5,%lo(jiffies)($2)
	move	$4,$16
	addiu	$5,$5,-30000
$L234:
	jal	rtl_set_fdb_aging
	nop

	move	$4,$16
$L237:
	lui	$2,%hi(jiffies)
	lw	$16,%lo(jiffies)($2)
	jal	rtl_get_fdb_aging
	nop

	move	$4,$17
	jal	rtl_hold_time
	move	$18,$2

	addu	$2,$2,$18
	subu	$2,$16,$2
	slt	$2,$2,0
	li	$3,-1			# 0xffffffffffffffff
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	movn	$3,$0,$2
	move	$2,$3
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl_br_fdb_time_update
	.size	rtl_br_fdb_time_update, .-rtl_br_fdb_time_update
	.section	.text.rtk_idleNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_idleNaptConnection
	.set	nomips16
	.ent	rtk_idleNaptConnection
	.type	rtk_idleNaptConnection, @function
rtk_idleNaptConnection:
	.frame	$sp,40,$31		# vars= 8, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$17,32($sp)
	sw	$16,28($sp)
	lui	$17,%hi(table_path)
	move	$16,$4
	lw	$4,%lo(table_path)($17)
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	sw	$5,16($sp)
	.set	macro
	.set	reorder

	lw	$8,4($16)
	lw	$4,20($16)
	lw	$7,24($16)
	lw	$10,8($16)
	xor	$2,$4,$8
	srl	$3,$8,8
	lw	$11,0($16)
	xor	$2,$2,$7
	xor	$2,$2,$3
	srl	$3,$4,16
	xor	$2,$2,$3
	srl	$3,$10,4
	xori	$13,$11,0x2
	li	$12,6			# 0x6
	xor	$2,$2,$3
	li	$6,17			# 0x11
	lui	$3,%hi(path_table_list_max)
	movz	$6,$12,$13
	lw	$14,%lo(path_table_list_max)($3)
	xor	$2,$2,$6
	srl	$3,$2,12
	xor	$3,$3,$2
	addiu	$14,$14,-1
	and	$3,$3,$14
	li	$12,12			# 0xc
	mul	$2,$3,$12
	lw	$9,%lo(table_path)($17)
	lui	$12,%hi(jiffies)
	lw	$12,%lo(jiffies)($12)
	lw	$13,4($9)
	lw	$5,16($sp)
	addu	$3,$2,$13
	li	$2,-16777216			# 0xffffffffff000000
	ori	$15,$2,0x100
	lw	$3,0($3)
	.set	noreorder
	.set	nomacro
	j	$L241
	ori	$2,$2,0xff00
	.set	macro
	.set	reorder

$L245:
	.set	noreorder
	.set	nomacro
	bnel	$24,$11,$L241
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$24,0($3)
	and	$24,$24,$2
	.set	noreorder
	.set	nomacro
	bnel	$24,$15,$L241
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$24,16($3)
	.set	noreorder
	.set	nomacro
	bnel	$24,$8,$L241
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$24,12($3)
	.set	noreorder
	.set	nomacro
	bnel	$24,$10,$L241
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$25,24($3)
	lw	$24,12($16)
	.set	noreorder
	.set	nomacro
	bnel	$25,$24,$L241
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$25,28($3)
	lw	$24,16($16)
	.set	noreorder
	.set	nomacro
	bnel	$25,$24,$L241
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$24,20($3)
	.set	noreorder
	.set	nomacro
	bnel	$24,$4,$L241
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$24,8($3)
	.set	noreorder
	.set	nomacro
	bnel	$24,$7,$L242
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$2,40($3)
	subu	$15,$12,$5
	subu	$15,$15,$2
	.set	noreorder
	.set	nomacro
	bgezl	$15,$L253
	lw	$2,12($16)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L252
	lw	$4,56($3)
	.set	macro
	.set	reorder

$L242:
$L241:
	.set	noreorder
	.set	nomacro
	bnel	$3,$0,$L245
	lw	$24,4($3)
	.set	macro
	.set	reorder

	lw	$2,12($16)
$L253:
	lw	$15,16($16)
	srl	$24,$4,8
	xor	$3,$2,$4
	xor	$3,$3,$15
	xor	$3,$3,$24
	srl	$24,$2,16
	xor	$3,$3,$24
	srl	$24,$7,4
	xor	$3,$3,$24
	xor	$6,$3,$6
	srl	$3,$6,12
	xor	$6,$3,$6
	and	$14,$6,$14
	li	$3,12			# 0xc
	mul	$6,$14,$3
	li	$3,-16777216			# 0xffffffffff000000
	ori	$14,$3,0x200
	addu	$13,$6,$13
	ori	$6,$3,0xff00
	.set	noreorder
	.set	nomacro
	j	$L246
	lw	$3,0($13)
	.set	macro
	.set	reorder

$L249:
	.set	noreorder
	.set	nomacro
	bnel	$13,$11,$L246
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$13,0($3)
	and	$13,$13,$6
	.set	noreorder
	.set	nomacro
	bnel	$13,$14,$L246
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$13,20($3)
	.set	noreorder
	.set	nomacro
	bnel	$13,$2,$L246
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$13,8($3)
	.set	noreorder
	.set	nomacro
	bnel	$13,$15,$L246
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$13,24($3)
	.set	noreorder
	.set	nomacro
	bnel	$13,$4,$L246
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$13,28($3)
	.set	noreorder
	.set	nomacro
	bnel	$13,$7,$L246
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$13,32($3)
	.set	noreorder
	.set	nomacro
	bnel	$13,$8,$L246
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$13,36($3)
	.set	noreorder
	.set	nomacro
	bnel	$13,$10,$L247
	lw	$3,64($3)
	.set	macro
	.set	reorder

	lw	$2,40($3)
	subu	$12,$12,$5
	subu	$12,$12,$2
	.set	noreorder
	.set	nomacro
	bgezl	$12,$L254
	lhu	$2,2($9)
	.set	macro
	.set	reorder

	lw	$4,56($3)
$L252:
	.set	noreorder
	.set	nomacro
	jal	rtl_set_ct_timeout_expires
	addu	$5,$5,$2
	.set	macro
	.set	reorder

	lui	$2,%hi(table_path)
	lw	$2,%lo(table_path)($2)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L244
	li	$2,-1			# 0xffffffffffffffff
	.set	macro
	.set	reorder

$L247:
$L246:
	.set	noreorder
	.set	nomacro
	bnel	$3,$0,$L249
	lw	$13,4($3)
	.set	macro
	.set	reorder

	lhu	$2,2($9)
$L254:
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$2,2($9)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	move	$2,$0
$L244:
	lw	$31,36($sp)
	lw	$17,32($sp)
	lw	$16,28($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,40
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
	.frame	$sp,72,$31		# vars= 32, regs= 6/0, args= 16, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-72
	sw	$31,68($sp)
	sw	$20,64($sp)
	sw	$19,60($sp)
	sw	$18,56($sp)
	sw	$17,52($sp)
	sw	$16,48($sp)
	lui	$2,%hi(jiffies)
	move	$5,$0
	lw	$19,%lo(jiffies)($2)
	jal	rtl_get_ct_protonum
	move	$16,$4

	li	$3,17			# 0x11
	bnel	$2,$3,$L256
	move	$4,$16

	jal	rtl_get_ct_udp_status
	move	$4,$16

	andi	$2,$2,0x2
	bne	$2,$0,$L264
	li	$4,1			# 0x1

	move	$4,$0
$L264:
	move	$5,$16
	jal	udp_get_timeouts_by_state
	li	$6,1			# 0x1

	move	$17,$2
	j	$L258
	li	$18,1			# 0x1

$L256:
	jal	rtl_get_ct_protonum
	move	$5,$0

	li	$3,6			# 0x6
	bne	$2,$3,$L262
	li	$2,-1			# 0xffffffffffffffff

	jal	rtl_get_ct_tcp_state
	move	$4,$16

	sltu	$2,$2,6
	beq	$2,$0,$L262
	li	$2,-1			# 0xffffffffffffffff

	jal	rtl_tcp_get_timeouts
	move	$4,$16

	move	$17,$2
	li	$18,2			# 0x2
$L258:
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$4,$16

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$20,$2

	bnel	$20,$2,$L260
	move	$5,$0

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$18,16($sp)

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
	j	$L265
	move	$5,$0

$L260:
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$4,$16

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$20,$2

	bne	$20,$2,$L262
	li	$2,-1			# 0xffffffffffffffff

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$18,16($sp)

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
$L265:
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,36($sp)

	addiu	$4,$sp,16
	move	$5,$17
	jal	rtk_idleNaptConnection
	sw	$2,40($sp)

	beql	$2,$0,$L259
	li	$2,-1			# 0xffffffffffffffff

	addu	$5,$17,$19
	jal	rtl_check_for_acc
	move	$4,$16

	move	$2,$0
$L259:
$L262:
	lw	$31,68($sp)
	lw	$20,64($sp)
	lw	$19,60($sp)
	lw	$18,56($sp)
	lw	$17,52($sp)
	lw	$16,48($sp)
	j	$31
	addiu	$sp,$sp,72

	.set	macro
	.set	reorder
	.end	rtl_fpTimer_update
	.size	rtl_fpTimer_update, .-rtl_fpTimer_update
	.section	.text.rtk_delNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_delNaptConnection
	.set	nomips16
	.ent	rtk_delNaptConnection
	.type	rtk_delNaptConnection, @function
rtk_delNaptConnection:
	.frame	$sp,40,$31		# vars= 0, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lw	$3,0($4)
	li	$2,6			# 0x6
	li	$18,17			# 0x11
	xori	$3,$3,0x2
	move	$16,$4
	movz	$18,$2,$3
 #APP
 # 39 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	reorder						
	.set	noat						
	di	$17					
	andi	$17, 1					
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	lw	$4,20($4)
	.set	noreorder
	.set	nomacro
	jal	filter_delconnect
	lui	$19,%hi(table_napt)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	lw	$4,%lo(table_napt)($19)
	.set	macro
	.set	reorder

	lw	$6,4($16)
	lw	$9,8($16)
	lw	$5,20($16)
	lw	$8,24($16)
	xor	$2,$9,$6
	xor	$2,$2,$5
	srl	$3,$6,16
	lw	$7,0($16)
	xor	$2,$2,$8
	xor	$2,$2,$3
	srl	$3,$5,16
	xor	$2,$2,$3
	lui	$3,%hi(napt_table_list_max)
	andi	$4,$7,0xffff
	lw	$3,%lo(napt_table_list_max)($3)
	xor	$2,$2,$4
	srl	$4,$2,12
	xor	$2,$4,$2
	addiu	$3,$3,-1
	and	$4,$2,$3
	li	$3,12			# 0xc
	mul	$4,$4,$3
	lw	$3,%lo(table_napt)($19)
	lw	$2,4($3)
	addu	$2,$2,$4
	.set	noreorder
	.set	nomacro
	j	$L269
	lw	$2,0($2)
	.set	macro
	.set	reorder

$L293:
	.set	noreorder
	.set	nomacro
	bnel	$10,$7,$L269
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$10,8($2)
	.set	noreorder
	.set	nomacro
	bnel	$10,$6,$L269
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$10,12($2)
	.set	noreorder
	.set	nomacro
	bnel	$10,$9,$L269
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$11,16($2)
	lw	$10,12($16)
	.set	noreorder
	.set	nomacro
	bnel	$11,$10,$L269
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$11,20($2)
	lw	$10,16($16)
	.set	noreorder
	.set	nomacro
	bnel	$11,$10,$L269
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$10,24($2)
	.set	noreorder
	.set	nomacro
	bnel	$10,$5,$L269
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$10,28($2)
	.set	noreorder
	.set	nomacro
	bnel	$10,$8,$L270
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$5,36($2)
	sb	$0,0($2)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L271
	lw	$3,40($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L272
	sw	$3,40($5)
	.set	macro
	.set	reorder

$L271:
	lui	$5,%hi(table_napt)
	lw	$5,%lo(table_napt)($5)
	lw	$5,4($5)
	addu	$5,$5,$4
	sw	$3,4($5)
$L272:
	lw	$3,40($2)
	lw	$5,36($2)
	sw	$5,0($3)
	lui	$3,%hi(table_napt)
	lw	$3,%lo(table_napt)($3)
	lw	$3,4($3)
	addu	$4,$3,$4
	lw	$3,8($4)
	addiu	$3,$3,-1
	sw	$3,8($4)
	lw	$4,44($2)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L273
	lw	$3,48($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L274
	sw	$3,48($4)
	.set	macro
	.set	reorder

$L273:
	lui	$4,%hi(napt_list_inuse+4)
	sw	$3,%lo(napt_list_inuse+4)($4)
$L274:
	lw	$3,48($2)
	lw	$5,44($2)
	lui	$4,%hi(napt_list_inuse)
	addiu	$4,$4,%lo(napt_list_inuse)
	sw	$5,0($3)
	lw	$5,8($4)
	lui	$3,%hi(napt_list_free)
	addiu	$3,$3,%lo(napt_list_free)
	addiu	$5,$5,-1
	sw	$5,8($4)
	sw	$0,44($2)
	lw	$4,4($3)
	addiu	$5,$2,44
	lui	$19,%hi(table_path)
	sw	$4,48($2)
	lw	$4,4($3)
	sw	$2,0($4)
	lw	$2,8($3)
	lw	$4,%lo(table_path)($19)
	sw	$5,4($3)
	addiu	$2,$2,1
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	sw	$2,8($3)
	.set	macro
	.set	reorder

	lw	$5,4($16)
	lw	$4,20($16)
	lw	$7,24($16)
	lw	$6,8($16)
	xor	$2,$4,$5
	srl	$3,$5,8
	xor	$2,$2,$7
	xor	$2,$2,$3
	srl	$3,$4,16
	xor	$2,$2,$3
	srl	$3,$6,4
	xor	$2,$2,$3
	lui	$3,%hi(path_table_list_max)
	lw	$8,%lo(path_table_list_max)($3)
	xor	$2,$2,$18
	srl	$3,$2,12
	xor	$2,$3,$2
	addiu	$8,$8,-1
	and	$3,$2,$8
	li	$8,12			# 0xc
	mul	$3,$3,$8
	lw	$2,%lo(table_path)($19)
	li	$8,1			# 0x1
	lw	$2,4($2)
	addu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	j	$L275
	lw	$2,0($2)
	.set	macro
	.set	reorder

$L282:
	lw	$9,0($16)
	.set	noreorder
	.set	nomacro
	bnel	$10,$9,$L275
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lbu	$9,2($2)
	.set	noreorder
	.set	nomacro
	bnel	$9,$8,$L275
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$9,16($2)
	.set	noreorder
	.set	nomacro
	bnel	$9,$5,$L275
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$9,12($2)
	.set	noreorder
	.set	nomacro
	bnel	$9,$6,$L275
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$10,24($2)
	lw	$9,12($16)
	.set	noreorder
	.set	nomacro
	bnel	$10,$9,$L275
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$10,28($2)
	lw	$9,16($16)
	.set	noreorder
	.set	nomacro
	bnel	$10,$9,$L275
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$9,20($2)
	.set	noreorder
	.set	nomacro
	bnel	$9,$4,$L275
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$9,8($2)
	.set	noreorder
	.set	nomacro
	bnel	$9,$7,$L276
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$5,64($2)
	sb	$0,0($2)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L277
	lw	$4,68($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L278
	sw	$4,68($5)
	.set	macro
	.set	reorder

$L277:
	lui	$5,%hi(table_path)
	lw	$5,%lo(table_path)($5)
	lw	$5,4($5)
	addu	$5,$5,$3
	sw	$4,4($5)
$L278:
	lw	$4,68($2)
	lw	$5,64($2)
	sw	$5,0($4)
	lui	$4,%hi(table_path)
	lw	$4,%lo(table_path)($4)
	lw	$4,4($4)
	addu	$3,$4,$3
	lw	$4,8($3)
	addiu	$4,$4,-1
	sw	$4,8($3)
	lw	$4,72($2)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L279
	lw	$3,76($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L280
	sw	$3,76($4)
	.set	macro
	.set	reorder

$L279:
	lui	$4,%hi(path_list_inuse+4)
	sw	$3,%lo(path_list_inuse+4)($4)
$L280:
	lw	$3,76($2)
	lw	$5,72($2)
	lui	$4,%hi(path_list_inuse)
	addiu	$4,$4,%lo(path_list_inuse)
	sw	$5,0($3)
	lw	$5,8($4)
	lui	$3,%hi(path_list_free)
	addiu	$3,$3,%lo(path_list_free)
	addiu	$5,$5,-1
	sw	$5,8($4)
	sw	$0,72($2)
	lw	$5,4($3)
	addiu	$4,$2,72
	sw	$5,76($2)
	lw	$5,4($3)
	sw	$2,0($5)
	lw	$2,8($3)
	sw	$4,4($3)
	addiu	$2,$2,1
	.set	noreorder
	.set	nomacro
	j	$L281
	sw	$2,8($3)
	.set	macro
	.set	reorder

$L276:
$L275:
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L282
	lw	$10,4($2)
	.set	macro
	.set	reorder

$L281:
	lw	$4,20($16)
	lw	$3,12($16)
	lw	$6,16($16)
	lw	$5,24($16)
	xor	$7,$3,$4
	srl	$2,$4,8
	xor	$7,$7,$6
	xor	$7,$7,$2
	srl	$2,$3,16
	xor	$7,$7,$2
	srl	$2,$5,4
	xor	$7,$7,$2
	lui	$2,%hi(path_table_list_max)
	lw	$2,%lo(path_table_list_max)($2)
	xor	$18,$7,$18
	srl	$7,$18,12
	xor	$18,$7,$18
	addiu	$2,$2,-1
	and	$18,$18,$2
	li	$2,12			# 0xc
	mul	$18,$18,$2
	lui	$2,%hi(table_path)
	lw	$2,%lo(table_path)($2)
	li	$7,2			# 0x2
	lw	$2,4($2)
	addu	$2,$2,$18
	.set	noreorder
	.set	nomacro
	j	$L283
	lw	$2,0($2)
	.set	macro
	.set	reorder

$L290:
	lw	$8,0($16)
	.set	noreorder
	.set	nomacro
	bnel	$9,$8,$L283
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lbu	$8,2($2)
	.set	noreorder
	.set	nomacro
	bnel	$8,$7,$L283
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$8,20($2)
	.set	noreorder
	.set	nomacro
	bnel	$8,$3,$L283
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$8,8($2)
	.set	noreorder
	.set	nomacro
	bnel	$8,$6,$L283
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$8,24($2)
	.set	noreorder
	.set	nomacro
	bnel	$8,$4,$L283
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$8,28($2)
	.set	noreorder
	.set	nomacro
	bnel	$8,$5,$L283
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$9,32($2)
	lw	$8,4($16)
	.set	noreorder
	.set	nomacro
	bnel	$9,$8,$L283
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$9,36($2)
	lw	$8,8($16)
	.set	noreorder
	.set	nomacro
	bnel	$9,$8,$L284
	lw	$2,64($2)
	.set	macro
	.set	reorder

	lw	$4,64($2)
	sb	$0,0($2)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L285
	lw	$3,68($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L286
	sw	$3,68($4)
	.set	macro
	.set	reorder

$L285:
	lui	$4,%hi(table_path)
	lw	$4,%lo(table_path)($4)
	lw	$4,4($4)
	addu	$4,$4,$18
	sw	$3,4($4)
$L286:
	lw	$3,68($2)
	lw	$4,64($2)
	sw	$4,0($3)
	lui	$3,%hi(table_path)
	lw	$3,%lo(table_path)($3)
	lw	$3,4($3)
	addu	$18,$3,$18
	lw	$3,8($18)
	addiu	$3,$3,-1
	sw	$3,8($18)
	lw	$4,72($2)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L287
	lw	$3,76($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L288
	sw	$3,76($4)
	.set	macro
	.set	reorder

$L287:
	lui	$4,%hi(path_list_inuse+4)
	sw	$3,%lo(path_list_inuse+4)($4)
$L288:
	lw	$3,76($2)
	lw	$5,72($2)
	lui	$4,%hi(path_list_inuse)
	addiu	$4,$4,%lo(path_list_inuse)
	sw	$5,0($3)
	lw	$5,8($4)
	lui	$3,%hi(path_list_free)
	addiu	$3,$3,%lo(path_list_free)
	addiu	$5,$5,-1
	sw	$5,8($4)
	sw	$0,72($2)
	lw	$5,4($3)
	addiu	$4,$2,72
	sw	$5,76($2)
	lw	$5,4($3)
	sw	$2,0($5)
	lw	$2,8($3)
	sw	$4,4($3)
	addiu	$2,$2,1
	.set	noreorder
	.set	nomacro
	j	$L289
	sw	$2,8($3)
	.set	macro
	.set	reorder

$L284:
$L283:
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L290
	lw	$9,4($2)
	.set	macro
	.set	reorder

$L289:
	lui	$2,%hi(table_path)
	lw	$2,%lo(table_path)($2)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	lui	$2,%hi(table_napt)
	lw	$2,%lo(table_napt)($2)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	andi	$2,$17,0x1
	bne	$2,$0,$L291
 #APP
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$17, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L296
	lw	$31,36($sp)
	.set	macro
	.set	reorder

$L291:
 #APP
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$17, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L292
	move	$2,$0
	.set	macro
	.set	reorder

$L270:
$L269:
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L293
	lw	$10,4($2)
	.set	macro
	.set	reorder

	lhu	$2,2($3)
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$2,2($3)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$17, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	li	$2,-4			# 0xfffffffffffffffc
$L292:
	lw	$31,36($sp)
$L296:
	lw	$19,32($sp)
	lw	$18,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,40
	.set	macro
	.set	reorder

	.end	rtk_delNaptConnection
	.size	rtk_delNaptConnection, .-rtk_delNaptConnection
	.section	.text.rtk_addNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_addNaptConnection
	.set	nomips16
	.ent	rtk_addNaptConnection
	.type	rtk_addNaptConnection, @function
rtk_addNaptConnection:
	.frame	$sp,144,$31		# vars= 88, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-144
	sw	$31,140($sp)
	sw	$fp,136($sp)
	sw	$23,132($sp)
	sw	$22,128($sp)
	sw	$21,124($sp)
	sw	$20,120($sp)
	sw	$19,116($sp)
	sw	$18,112($sp)
	sw	$17,108($sp)
	sw	$16,104($sp)
	move	$17,$5
	move	$16,$4
	sw	$6,152($sp)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L298
	move	$18,$7
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$5
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L298
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	lw	$3,0($16)
	lw	$4,20($16)
	xori	$3,$3,0x2
	li	$fp,17			# 0x11
	.set	noreorder
	.set	nomacro
	jal	filter_addconnect
	movz	$fp,$2,$3
	.set	macro
	.set	reorder

 #APP
 # 39 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	reorder						
	.set	noat						
	di	$23					
	andi	$23, 1					
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	lw	$4,4($16)
	lw	$2,8($16)
	lw	$3,20($16)
	lw	$5,24($16)
	xor	$2,$4,$2
	xor	$2,$2,$3
	xor	$2,$2,$5
	srl	$4,$4,16
	lhu	$5,2($16)
	xor	$2,$2,$4
	srl	$3,$3,16
	xor	$2,$2,$3
	lui	$3,%hi(napt_table_list_max)
	lw	$3,%lo(napt_table_list_max)($3)
	lui	$19,%hi(table_napt)
	xor	$2,$2,$5
	lw	$4,%lo(table_napt)($19)
	srl	$20,$2,12
	xor	$2,$20,$2
	addiu	$3,$3,-1
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	and	$20,$2,$3
	.set	macro
	.set	reorder

	li	$2,12			# 0xc
	mul	$20,$20,$2
	lw	$3,%lo(table_napt)($19)
	lw	$2,4($3)
	addu	$2,$2,$20
	.set	noreorder
	.set	nomacro
	j	$L301
	lw	$2,0($2)
	.set	macro
	.set	reorder

$L305:
	lw	$4,0($16)
	.set	noreorder
	.set	nomacro
	bnel	$5,$4,$L301
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$5,8($2)
	lw	$4,4($16)
	.set	noreorder
	.set	nomacro
	bnel	$5,$4,$L301
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$5,12($2)
	lw	$4,8($16)
	.set	noreorder
	.set	nomacro
	bnel	$5,$4,$L301
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$5,16($2)
	lw	$4,12($16)
	.set	noreorder
	.set	nomacro
	bnel	$5,$4,$L301
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$5,20($2)
	lw	$4,16($16)
	.set	noreorder
	.set	nomacro
	bnel	$5,$4,$L301
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$5,24($2)
	lw	$4,20($16)
	.set	noreorder
	.set	nomacro
	bnel	$5,$4,$L301
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$5,28($2)
	lw	$4,24($16)
	.set	noreorder
	.set	nomacro
	bnel	$5,$4,$L302
	lw	$2,36($2)
	.set	macro
	.set	reorder

	lw	$4,152($sp)
	move	$5,$fp
	.set	noreorder
	.set	nomacro
	jal	rtl_conntrack_drop_check_hook
	move	$6,$4
	.set	macro
	.set	reorder

	lui	$2,%hi(table_napt)
	lw	$2,%lo(table_napt)($2)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	andi	$2,$23,0x1
	bne	$2,$0,$L303
 #APP
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$23, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L360
	lw	$31,140($sp)
	.set	macro
	.set	reorder

$L303:
 #APP
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$23, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L304
	move	$2,$0
	.set	macro
	.set	reorder

$L302:
$L301:
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L305
	lw	$5,4($2)
	.set	macro
	.set	reorder

	lui	$4,%hi(napt_list_free)
	lw	$2,%lo(napt_list_free)($4)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L306
	li	$7,-1			# 0xffffffffffffffff
	.set	macro
	.set	reorder

	lw	$3,0($16)
	lw	$5,44($2)
	sw	$3,4($2)
	lw	$6,4($16)
	lw	$3,48($2)
	sw	$6,8($2)
	lw	$6,8($16)
	sw	$6,12($2)
	lw	$6,12($16)
	sw	$6,16($2)
	lw	$6,16($16)
	sw	$6,20($2)
	lw	$6,20($16)
	sw	$6,24($2)
	lw	$6,24($16)
	sw	$18,32($2)
	sb	$7,0($2)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L307
	sw	$6,28($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L308
	sw	$3,48($5)
	.set	macro
	.set	reorder

$L307:
	addiu	$4,$4,%lo(napt_list_free)
	sw	$3,4($4)
$L308:
	lw	$3,48($2)
	lw	$5,44($2)
	lui	$4,%hi(napt_list_free)
	addiu	$4,$4,%lo(napt_list_free)
	sw	$5,0($3)
	lw	$5,8($4)
	lui	$3,%hi(napt_list_inuse)
	addiu	$3,$3,%lo(napt_list_inuse)
	addiu	$5,$5,-1
	sw	$5,8($4)
	sw	$0,44($2)
	lw	$5,4($3)
	addiu	$7,$2,44
	lui	$4,%hi(table_napt)
	sw	$5,48($2)
	lw	$6,4($3)
	addiu	$5,$2,36
	sw	$2,0($6)
	lw	$6,8($3)
	sw	$7,4($3)
	addiu	$6,$6,1
	sw	$6,8($3)
	sw	$0,36($2)
	lw	$3,%lo(table_napt)($4)
	lw	$3,4($3)
	addu	$3,$3,$20
	lw	$3,4($3)
	sw	$3,40($2)
	lw	$3,%lo(table_napt)($4)
	lw	$3,4($3)
	addu	$3,$3,$20
	lw	$3,4($3)
	sw	$2,0($3)
	lw	$2,%lo(table_napt)($4)
	lw	$2,4($2)
	addu	$2,$2,$20
	sw	$5,4($2)
	lw	$2,%lo(table_napt)($4)
	lw	$2,4($2)
	addu	$20,$2,$20
	lw	$2,8($20)
	addiu	$2,$2,1
	sw	$2,8($20)
	li	$2,33			# 0x21
	sb	$2,17($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl865x_getLanDev
	sb	$0,16($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl865x_getWanDev
	move	$21,$2
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_protocol
	move	$20,$2
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,64($sp)
	.set	macro
	.set	reorder

	lw	$22,0($2)
	move	$4,$17
	move	$19,$2
	ext	$22,$22,24,4
	sll	$22,$22,2
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	addu	$22,$2,$22
	.set	macro
	.set	reorder

	addiu	$5,$2,6
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$sp,20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	move	$5,$2
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$sp,28
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	sw	$2,80($sp)
	lw	$2,12($19)
	move	$4,$17
	sw	$2,96($sp)
	lhu	$3,0($22)
	sw	$3,88($sp)
	lw	$5,16($19)
	sw	$5,92($sp)
	lhu	$6,2($22)
	.set	noreorder
	.set	nomacro
	jal	rtl_store_skb_dst
	sw	$6,84($sp)
	.set	macro
	.set	reorder

	lui	$2,%hi(table_path)
	.set	noreorder
	.set	nomacro
	jal	_raw_spin_lock
	lw	$4,%lo(table_path)($2)
	.set	macro
	.set	reorder

	lui	$2,%hi(path_list_free)
	lw	$18,%lo(path_list_free)($2)
	lui	$2,%hi(path_table_list_max)
	lw	$2,%lo(path_table_list_max)($2)
	sw	$2,76($sp)
	lw	$2,4($16)
	sw	$2,60($sp)
	lw	$3,8($16)
	sw	$3,72($sp)
	lw	$4,20($16)
	sw	$4,56($sp)
	lw	$5,24($16)
	.set	noreorder
	.set	nomacro
	beq	$18,$0,$L309
	sw	$5,68($sp)
	.set	macro
	.set	reorder

	lw	$2,0($16)
	move	$4,$0
	move	$7,$0
	sw	$2,4($18)
	lw	$6,4($16)
	sw	$6,16($18)
	lw	$2,8($16)
	sw	$2,12($18)
	lw	$2,20($16)
	sw	$2,20($18)
	lw	$2,24($16)
	sw	$2,8($18)
	lw	$5,12($16)
	sw	$5,24($18)
	lw	$2,16($16)
	sw	$2,28($18)
	lw	$8,20($16)
	sw	$8,32($18)
	lw	$2,24($16)
	sw	$2,36($18)
	lui	$2,%hi(route_list_inuse)
	.set	noreorder
	.set	nomacro
	j	$L310
	lw	$2,%lo(route_list_inuse)($2)
	.set	macro
	.set	reorder

$L313:
	sltu	$9,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$9,$0,$L311
	and	$10,$3,$8
	.set	macro
	.set	reorder

	lw	$9,4($2)
	.set	noreorder
	.set	nomacro
	bnel	$10,$9,$L312
	move	$3,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L312
	addiu	$7,$2,16
	.set	macro
	.set	reorder

$L311:
	move	$3,$4
$L312:
	lw	$2,44($2)
	move	$4,$3
$L310:
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L313
	lw	$3,8($2)
	.set	macro
	.set	reorder

	li	$2,1			# 0x1
	li	$3,-1			# 0xffffffffffffffff
	sw	$7,44($18)
	sb	$3,0($18)
	sb	$2,2($18)
	.set	noreorder
	.set	nomacro
	beq	$6,$5,$L314
	sb	$0,1($18)
	.set	macro
	.set	reorder

	sb	$2,1($18)
$L314:
	lw	$3,12($18)
	lw	$2,28($18)
	.set	noreorder
	.set	nomacro
	beql	$3,$2,$L364
	sw	$0,48($18)
	.set	macro
	.set	reorder

	lbu	$2,1($18)
	ori	$2,$2,0x2
	sb	$2,1($18)
	sw	$0,48($18)
$L364:
	sw	$0,52($18)
	lw	$2,4($16)
	addiu	$3,$sp,36
	move	$4,$3
	sw	$2,12($19)
	lw	$2,8($16)
	move	$5,$0
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,20($16)
	sw	$2,16($19)
	lw	$2,24($16)
	sh	$2,2($22)
	.set	noreorder
	.set	nomacro
	jal	memset
	sw	$3,100($sp)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$21,$0,$L316
	lw	$3,100($sp)
	.set	macro
	.set	reorder

	lw	$4,4($16)
	move	$6,$3
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$5,$21
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L365
	lw	$2,12($16)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,16
	.set	macro
	.set	reorder

	move	$3,$2
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L317
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	sw	$3,100($sp)
	.set	macro
	.set	reorder

	lw	$3,100($sp)
	bne	$2,$3,$L317
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$3,$2,$3
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L317
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffffffffffea
	.set	macro
	.set	reorder

$L317:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	addiu	$4,$2,6
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,36
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dmac
	move	$5,$21
	.set	macro
	.set	reorder

	lw	$6,64($sp)
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	bnel	$6,$2,$L365
	lw	$2,12($16)
	.set	macro
	.set	reorder

	lui	$2,%hi(nf_hooks+128)
	lw	$3,%lo(nf_hooks+128)($2)
	addiu	$2,$2,%lo(nf_hooks+128)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L318
	move	$4,$17
	.set	macro
	.set	reorder

	move	$5,$0
	move	$6,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$20
	.set	macro
	.set	reorder

$L318:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L319
	lw	$2,48($18)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L361
	sw	$2,48($18)
	.set	macro
	.set	reorder

$L319:
	sw	$2,48($18)
$L361:
	lui	$2,%hi(nf_hooks+160)
	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L321
	move	$4,$17
	.set	macro
	.set	reorder

	li	$5,4			# 0x4
	move	$6,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$20
	.set	macro
	.set	reorder

$L321:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L322
	lw	$2,52($18)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L316
	sw	$2,52($18)
	.set	macro
	.set	reorder

$L322:
	sw	$2,52($18)
$L316:
	lw	$2,12($16)
$L365:
	addiu	$3,$sp,36
	move	$4,$3
	sw	$2,12($19)
	lw	$2,16($16)
	move	$5,$0
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,20($16)
	sw	$2,16($19)
	lw	$2,24($16)
	sh	$2,2($22)
	.set	noreorder
	.set	nomacro
	jal	memset
	sw	$3,100($sp)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L324
	lw	$3,100($sp)
	.set	macro
	.set	reorder

	lw	$4,20($16)
	move	$6,$3
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$5,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L366
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,16
	.set	macro
	.set	reorder

	move	$3,$2
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L325
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	sw	$3,100($sp)
	.set	macro
	.set	reorder

	lw	$3,100($sp)
	bne	$2,$3,$L325
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$3,$2,$3
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L325
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffffffffffea
	.set	macro
	.set	reorder

$L325:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	move	$4,$2
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,36
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_smac
	move	$5,$20
	.set	macro
	.set	reorder

	lw	$3,64($sp)
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L366
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	lui	$2,%hi(nf_hooks+160)
	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L326
	move	$4,$17
	.set	macro
	.set	reorder

	li	$5,4			# 0x4
	move	$6,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$20
	.set	macro
	.set	reorder

$L326:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L327
	lw	$2,52($18)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L324
	sw	$2,52($18)
	.set	macro
	.set	reorder

$L327:
	sw	$2,52($18)
$L324:
	li	$2,6			# 0x6
$L366:
	.set	noreorder
	.set	nomacro
	bne	$fp,$2,$L329
	lw	$5,152($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_tcp_get_timeouts
	lw	$4,152($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L362
	sw	$2,60($18)
	.set	macro
	.set	reorder

$L329:
	li	$4,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	udp_get_timeouts_by_state
	li	$6,1			# 0x1
	.set	macro
	.set	reorder

	sw	$2,60($18)
$L362:
	lw	$4,152($sp)
	lui	$2,%hi(jiffies)
	lw	$3,72($18)
	sw	$4,56($18)
	lw	$4,%lo(jiffies)($2)
	lw	$2,76($18)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L331
	sw	$4,40($18)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L332
	sw	$2,76($3)
	.set	macro
	.set	reorder

$L331:
	lui	$3,%hi(path_list_free+4)
	sw	$2,%lo(path_list_free+4)($3)
$L332:
	lw	$2,76($18)
	lw	$4,72($18)
	lw	$6,60($sp)
	lw	$5,56($sp)
	sw	$4,0($2)
	lw	$2,68($sp)
	xor	$3,$5,$6
	srl	$4,$6,8
	xor	$3,$3,$2
	xor	$3,$3,$4
	lw	$4,56($sp)
	lw	$6,72($sp)
	lui	$2,%hi(path_list_free)
	addiu	$2,$2,%lo(path_list_free)
	srl	$5,$4,16
	lw	$4,8($2)
	xor	$3,$3,$5
	srl	$5,$6,4
	xor	$3,$3,$5
	xor	$3,$3,$fp
	addiu	$4,$4,-1
	sw	$4,8($2)
	sw	$0,72($18)
	srl	$4,$3,12
	xor	$3,$4,$3
	lw	$4,76($sp)
	lui	$2,%hi(path_list_inuse)
	addiu	$2,$2,%lo(path_list_inuse)
	addiu	$5,$4,-1
	lw	$4,4($2)
	and	$3,$3,$5
	li	$5,12			# 0xc
	mul	$3,$3,$5
	sw	$4,76($18)
	lw	$6,4($2)
	addiu	$5,$18,72
	lui	$4,%hi(table_path)
	sw	$18,0($6)
	lw	$6,8($2)
	sw	$5,4($2)
	addiu	$5,$18,64
	addiu	$6,$6,1
	sw	$6,8($2)
	sw	$0,64($18)
	lw	$2,%lo(table_path)($4)
	lw	$2,4($2)
	addu	$2,$2,$3
	lw	$2,4($2)
	sw	$2,68($18)
	lw	$2,%lo(table_path)($4)
	lw	$2,4($2)
	addu	$2,$2,$3
	lw	$2,4($2)
	sw	$18,0($2)
	lw	$2,%lo(table_path)($4)
	lw	$2,4($2)
	addu	$2,$2,$3
	sw	$5,4($2)
	lw	$2,%lo(table_path)($4)
	lw	$2,4($2)
	addu	$3,$2,$3
	lw	$2,8($3)
	addiu	$2,$2,1
	sw	$2,8($3)
$L309:
	lui	$2,%hi(path_list_free)
	lw	$18,%lo(path_list_free)($2)
	lui	$2,%hi(path_table_list_max)
	lw	$2,%lo(path_table_list_max)($2)
	sw	$2,76($sp)
	lw	$2,20($16)
	sw	$2,60($sp)
	lw	$3,24($16)
	sw	$3,72($sp)
	lw	$4,12($16)
	sw	$4,56($sp)
	lw	$5,16($16)
	.set	noreorder
	.set	nomacro
	beq	$18,$0,$L333
	sw	$5,68($sp)
	.set	macro
	.set	reorder

	lw	$2,0($16)
	move	$5,$0
	move	$7,$0
	sw	$2,4($18)
	lw	$2,20($16)
	sw	$2,16($18)
	lw	$2,24($16)
	sw	$2,12($18)
	lw	$6,12($16)
	sw	$6,20($18)
	lw	$2,16($16)
	sw	$2,8($18)
	lw	$2,20($16)
	sw	$2,24($18)
	lw	$2,24($16)
	sw	$2,28($18)
	lw	$4,4($16)
	sw	$4,32($18)
	lw	$2,8($16)
	sw	$2,36($18)
	lui	$2,%hi(route_list_inuse)
	.set	noreorder
	.set	nomacro
	j	$L334
	lw	$2,%lo(route_list_inuse)($2)
	.set	macro
	.set	reorder

$L337:
	sltu	$8,$3,$5
	.set	noreorder
	.set	nomacro
	bne	$8,$0,$L335
	and	$9,$3,$4
	.set	macro
	.set	reorder

	lw	$8,4($2)
	.set	noreorder
	.set	nomacro
	bnel	$9,$8,$L336
	move	$3,$5
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L336
	addiu	$7,$2,16
	.set	macro
	.set	reorder

$L335:
	move	$3,$5
$L336:
	lw	$2,44($2)
	move	$5,$3
$L334:
	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L337
	lw	$3,8($2)
	.set	macro
	.set	reorder

	li	$2,2			# 0x2
	sb	$2,2($18)
	li	$2,-1			# 0xffffffffffffffff
	sw	$7,44($18)
	sb	$2,0($18)
	.set	noreorder
	.set	nomacro
	beq	$6,$4,$L338
	sb	$0,1($18)
	.set	macro
	.set	reorder

	li	$2,4			# 0x4
	sb	$2,1($18)
$L338:
	lw	$3,8($18)
	lw	$2,36($18)
	.set	noreorder
	.set	nomacro
	beql	$3,$2,$L367
	sw	$0,48($18)
	.set	macro
	.set	reorder

	lbu	$2,1($18)
	ori	$2,$2,0x8
	sb	$2,1($18)
	sw	$0,48($18)
$L367:
	sw	$0,52($18)
	lw	$2,20($16)
	addiu	$3,$sp,36
	move	$4,$3
	sw	$2,12($19)
	lw	$2,24($16)
	move	$5,$0
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,12($16)
	sw	$2,16($19)
	lw	$2,16($16)
	sh	$2,2($22)
	.set	noreorder
	.set	nomacro
	jal	memset
	sw	$3,100($sp)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L340
	lw	$3,100($sp)
	.set	macro
	.set	reorder

	lw	$4,20($16)
	move	$6,$3
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$5,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bnel	$2,$0,$L368
	lw	$2,20($16)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,16
	.set	macro
	.set	reorder

	move	$3,$2
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L341
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	sw	$3,100($sp)
	.set	macro
	.set	reorder

	lw	$3,100($sp)
	bne	$2,$3,$L341
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$3,$2,$3
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L341
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffffffffffea
	.set	macro
	.set	reorder

$L341:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	addiu	$4,$2,6
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,36
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dmac
	move	$5,$20
	.set	macro
	.set	reorder

	lw	$6,64($sp)
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	bne	$6,$2,$L342
	lui	$2,%hi(nf_hooks+128)
	.set	macro
	.set	reorder

	lw	$3,%lo(nf_hooks+128)($2)
	addiu	$2,$2,%lo(nf_hooks+128)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L342
	move	$4,$17
	.set	macro
	.set	reorder

	move	$5,$0
	move	$6,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$21
	.set	macro
	.set	reorder

$L342:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L343
	lw	$2,48($18)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L340
	sw	$2,48($18)
	.set	macro
	.set	reorder

$L343:
	sw	$2,48($18)
$L340:
	lw	$2,20($16)
$L368:
	addiu	$3,$sp,36
	move	$4,$3
	sw	$2,12($19)
	lw	$2,24($16)
	move	$5,$0
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,4($16)
	sw	$2,16($19)
	lw	$2,8($16)
	sh	$2,2($22)
	.set	noreorder
	.set	nomacro
	jal	memset
	sw	$3,100($sp)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$21,$0,$L345
	lw	$3,100($sp)
	.set	macro
	.set	reorder

	lw	$4,4($16)
	move	$6,$3
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$5,$21
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L345
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,16
	.set	macro
	.set	reorder

	move	$16,$2
	li	$2,1			# 0x1
	bne	$16,$2,$L346
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	move	$4,$17
	.set	macro
	.set	reorder

	bne	$2,$16,$L346
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$3,$2,$3
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L346
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffffffffffea
	.set	macro
	.set	reorder

$L346:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	move	$4,$2
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,36
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_smac
	move	$5,$21
	.set	macro
	.set	reorder

	lw	$3,64($sp)
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L345
	lui	$2,%hi(nf_hooks+160)
	.set	macro
	.set	reorder

	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L345
	move	$4,$17
	.set	macro
	.set	reorder

	li	$5,4			# 0x4
	move	$6,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$21
	.set	macro
	.set	reorder

$L345:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	sw	$2,52($18)
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$fp,$2,$L347
	lw	$5,152($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_tcp_get_timeouts
	lw	$4,152($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L363
	sw	$2,60($18)
	.set	macro
	.set	reorder

$L347:
	li	$4,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	udp_get_timeouts_by_state
	li	$6,1			# 0x1
	.set	macro
	.set	reorder

	sw	$2,60($18)
$L363:
	lw	$4,152($sp)
	lui	$2,%hi(jiffies)
	lw	$3,72($18)
	sw	$4,56($18)
	lw	$4,%lo(jiffies)($2)
	lw	$2,76($18)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L349
	sw	$4,40($18)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L350
	sw	$2,76($3)
	.set	macro
	.set	reorder

$L349:
	lui	$3,%hi(path_list_free+4)
	sw	$2,%lo(path_list_free+4)($3)
$L350:
	lw	$2,76($18)
	lw	$4,72($18)
	lw	$6,60($sp)
	lw	$5,56($sp)
	sw	$4,0($2)
	lw	$2,68($sp)
	xor	$3,$5,$6
	srl	$4,$6,8
	xor	$3,$3,$2
	xor	$3,$3,$4
	lw	$4,56($sp)
	lw	$6,72($sp)
	lui	$2,%hi(path_list_free)
	addiu	$2,$2,%lo(path_list_free)
	srl	$5,$4,16
	lw	$4,8($2)
	xor	$3,$3,$5
	srl	$5,$6,4
	xor	$3,$3,$5
	xor	$3,$3,$fp
	addiu	$4,$4,-1
	sw	$4,8($2)
	sw	$0,72($18)
	srl	$4,$3,12
	xor	$3,$4,$3
	lw	$4,76($sp)
	lui	$2,%hi(path_list_inuse)
	addiu	$2,$2,%lo(path_list_inuse)
	addiu	$5,$4,-1
	lw	$4,4($2)
	and	$3,$3,$5
	li	$5,12			# 0xc
	mul	$3,$3,$5
	sw	$4,76($18)
	lw	$6,4($2)
	addiu	$5,$18,72
	lui	$4,%hi(table_path)
	sw	$18,0($6)
	lw	$6,8($2)
	sw	$5,4($2)
	addiu	$5,$18,64
	addiu	$6,$6,1
	sw	$6,8($2)
	sw	$0,64($18)
	lw	$2,%lo(table_path)($4)
	lw	$2,4($2)
	addu	$2,$2,$3
	lw	$2,4($2)
	sw	$2,68($18)
	lw	$2,%lo(table_path)($4)
	lw	$2,4($2)
	addu	$2,$2,$3
	lw	$2,4($2)
	sw	$18,0($2)
	lw	$2,%lo(table_path)($4)
	lw	$2,4($2)
	addu	$2,$2,$3
	sw	$5,4($2)
	lw	$2,%lo(table_path)($4)
	lw	$2,4($2)
	addu	$3,$2,$3
	lw	$2,8($3)
	addiu	$2,$2,1
	sw	$2,8($3)
$L333:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	addiu	$4,$2,6
	addiu	$5,$sp,20
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$17
	.set	macro
	.set	reorder

	move	$4,$2
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,28
	.set	macro
	.set	reorder

	lw	$5,80($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	lw	$2,96($sp)
	sw	$2,12($19)
	lw	$3,88($sp)
	sh	$3,0($22)
	lw	$4,92($sp)
	sw	$4,16($19)
	lw	$5,84($sp)
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dst
	sh	$5,2($22)
	.set	macro
	.set	reorder

	beq	$21,$0,$L351
 #APP
 # 39 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	reorder						
	.set	noat						
	di	$3					
	andi	$3, 1					
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	lw	$4,16($28)
	lui	$2,%hi(__per_cpu_offset)
	addiu	$2,$2,%lo(__per_cpu_offset)
	sll	$4,$4,2
	addu	$2,$4,$2
	lw	$4,0($2)
	lw	$2,580($21)
	addu	$2,$2,$4
	lw	$4,0($2)
	addiu	$4,$4,-1
	sw	$4,0($2)
 #APP
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$3, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
$L351:
	.set	noreorder
	.set	nomacro
	beql	$20,$0,$L369
	lui	$2,%hi(table_path)
	.set	macro
	.set	reorder

 #APP
 # 39 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	reorder						
	.set	noat						
	di	$3					
	andi	$3, 1					
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	lw	$4,16($28)
	lui	$2,%hi(__per_cpu_offset)
	addiu	$2,$2,%lo(__per_cpu_offset)
	sll	$4,$4,2
	addu	$2,$4,$2
	lw	$4,0($2)
	lw	$2,580($20)
	addu	$2,$2,$4
	lw	$4,0($2)
	addiu	$4,$4,-1
	sw	$4,0($2)
 #APP
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$3, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	lui	$2,%hi(table_path)
$L369:
	lw	$2,%lo(table_path)($2)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	lui	$2,%hi(table_napt)
	lw	$2,%lo(table_napt)($2)
	lhu	$3,2($2)
	addiu	$3,$3,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$3,2($2)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$23, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	lw	$4,152($sp)
	move	$5,$fp
	.set	noreorder
	.set	nomacro
	jal	rtl_conntrack_drop_check_hook
	move	$6,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L359
	move	$2,$0
	.set	macro
	.set	reorder

$L306:
	lhu	$2,2($3)
	addiu	$2,$2,1
 #APP
 # 137 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 #NO_APP
	sh	$2,2($3)
 #APP
 # 139 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/spinlock.h" 1
	.set	push
	.set	noreorder
	.set	mips2
	sync
	.set	pop
 # 0 "" 2
 # 58 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/irqflags.h" 1
		.set	push						
	.set	noreorder					
	.set	noat						
	beqz	$23, 1f					
	di							
	ei							
1:								
	sll $0, $0, 3			
	.set	pop						

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	j	$L304
	li	$2,-1			# 0xffffffffffffffff
	.set	macro
	.set	reorder

$L359:
	.set	noreorder
	.set	nomacro
	j	$L360
	lw	$31,140($sp)
	.set	macro
	.set	reorder

$L298:
	li	$2,-1			# 0xffffffffffffffff
$L304:
	lw	$31,140($sp)
$L360:
	lw	$fp,136($sp)
	lw	$23,132($sp)
	lw	$22,128($sp)
	lw	$21,124($sp)
	lw	$20,120($sp)
	lw	$19,116($sp)
	lw	$18,112($sp)
	lw	$17,108($sp)
	lw	$16,104($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,144
	.set	macro
	.set	reorder

	.end	rtk_addNaptConnection
	.size	rtk_addNaptConnection, .-rtk_addNaptConnection
	.section	.text.negative_fragCache_init,"ax",@progbits
	.align	2
	.globl	negative_fragCache_init
	.set	nomips16
	.ent	negative_fragCache_init
	.type	negative_fragCache_init, @function
negative_fragCache_init:
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
	lui	$2,%hi(kmalloc_caches+40)
	lw	$4,%lo(kmalloc_caches+40)($2)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	lui	$3,%hi(negative_cache_table)
	beq	$2,$0,$L371
	sw	$2,%lo(negative_cache_table)($3)

	lui	$4,%hi(negative_cache_list_free)
	addiu	$2,$4,%lo(negative_cache_list_free)
	sw	$2,4($2)
	sw	$0,8($2)
	sw	$0,%lo(negative_cache_list_free)($4)
	move	$2,$0
	li	$6,12			# 0xc
	li	$5,64			# 0x40
$L372:
	mul	$4,$2,$6
	lw	$7,%lo(negative_cache_table)($3)
	addiu	$2,$2,1
	addu	$7,$7,$4
	sw	$0,12($7)
	lw	$7,%lo(negative_cache_table)($3)
	addu	$7,$7,$4
	sw	$0,4($7)
	lw	$7,%lo(negative_cache_table)($3)
	addu	$4,$7,$4
	addiu	$7,$4,4
	bne	$2,$5,$L372
	sw	$7,8($4)

	lui	$20,%hi(kmalloc_caches)
	lui	$19,%hi(negative_cache_timeout)
	lui	$16,%hi(negative_cache_list_free)
	addiu	$20,$20,%lo(kmalloc_caches)
	addiu	$19,$19,%lo(negative_cache_timeout)
	addiu	$16,$16,%lo(negative_cache_list_free)
	move	$17,$0
	li	$18,64			# 0x40
$L373:
	lw	$4,24($20)
	li	$5,32			# 0x20
	jal	kmem_cache_alloc
	addiu	$17,$17,1

	addiu	$4,$2,12
	move	$5,$0
	move	$6,$0
	beq	$2,$0,$L371
	move	$7,$0

	jal	init_timer_key
	sw	$2,16($sp)

	lw	$2,16($sp)
	sw	$2,32($2)
	sw	$19,28($2)
	sw	$0,48($2)
	lw	$4,4($16)
	addiu	$3,$2,48
	sw	$4,52($2)
	lw	$4,4($16)
	sw	$2,0($4)
	lw	$2,8($16)
	sw	$3,4($16)
	addiu	$2,$2,1
	bne	$17,$18,$L373
	sw	$2,8($16)

	lui	$2,%hi(negative_cache_table)
	lw	$3,%lo(negative_cache_table)($2)
	li	$2,1			# 0x1
	j	$L374
	sw	$0,0($3)

$L371:
	move	$2,$0
$L374:
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
	.end	negative_fragCache_init
	.size	negative_fragCache_init, .-negative_fragCache_init
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
	lui	$2,%hi(kmalloc_caches+20)
	move	$17,$4
	lw	$4,%lo(kmalloc_caches+20)($2)
	li	$5,32			# 0x20
	jal	kmem_cache_alloc
	lui	$16,%hi(udp_cache_table)

	move	$18,$2
	beq	$2,$0,$L379
	sw	$2,%lo(udp_cache_table)($16)

	li	$4,12			# 0xc
	mul	$4,$17,$4
	lui	$3,%hi(udp_cache_list_free)
	addiu	$2,$3,%lo(udp_cache_list_free)
	sw	$0,%lo(udp_cache_list_free)($3)
	li	$5,32			# 0x20
	lui	$3,%hi(max_udp_frag_entry)
	sw	$2,4($2)
	sw	$0,8($2)
	jal	__kmalloc
	sw	$17,%lo(max_udp_frag_entry)($3)

	sw	$2,4($18)
	lw	$4,%lo(udp_cache_table)($16)
	move	$3,$0
	lw	$4,4($4)
	bne	$4,$0,$L388
	li	$2,-1			# 0xffffffffffffffff

	j	$L389
	lw	$31,44($sp)

$L383:
	lw	$5,%lo(udp_cache_table)($16)
	addiu	$3,$3,1
	lw	$5,4($5)
	addu	$5,$5,$2
	sw	$0,8($5)
	lw	$5,%lo(udp_cache_table)($16)
	lw	$5,4($5)
	addu	$5,$5,$2
	sw	$0,0($5)
	lw	$5,%lo(udp_cache_table)($16)
	lw	$5,4($5)
	addu	$2,$5,$2
	j	$L387
	sw	$2,4($2)

$L388:
	li	$4,12			# 0xc
$L387:
	slt	$2,$3,$17
	bne	$2,$0,$L383
	mul	$2,$3,$4

	lui	$20,%hi(kmalloc_caches)
	lui	$19,%hi(cache_timeout)
	lui	$16,%hi(udp_cache_list_free)
	addiu	$20,$20,%lo(kmalloc_caches)
	addiu	$19,$19,%lo(cache_timeout)
	addiu	$16,$16,%lo(udp_cache_list_free)
	j	$L384
	move	$18,$0

$L385:
	lw	$4,24($20)
	jal	kmem_cache_alloc
	addiu	$18,$18,1

	addiu	$4,$2,16
	move	$5,$0
	move	$6,$0
	beq	$2,$0,$L379
	move	$7,$0

	jal	init_timer_key
	sw	$2,16($sp)

	lw	$2,16($sp)
	sw	$2,36($2)
	sw	$19,32($2)
	sw	$0,52($2)
	lw	$4,4($16)
	addiu	$3,$2,52
	sw	$4,56($2)
	lw	$4,4($16)
	sw	$2,0($4)
	lw	$2,8($16)
	sw	$3,4($16)
	addiu	$2,$2,1
	sw	$2,8($16)
$L384:
	slt	$2,$18,$17
	bne	$2,$0,$L385
	li	$5,32			# 0x20

	lui	$2,%hi(udp_cache_table)
	lw	$3,%lo(udp_cache_table)($2)
	li	$2,1			# 0x1
	j	$L381
	sw	$0,0($3)

$L379:
	move	$2,$0
$L381:
	lw	$31,44($sp)
$L389:
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
	.section	.bss.max_udp_frag_entry,"aw",@nobits
	.align	2
	.type	max_udp_frag_entry, @object
	.size	max_udp_frag_entry, 4
max_udp_frag_entry:
	.space	4
	.globl	route_list_inuse
	.section	.bss.route_list_inuse,"aw",@nobits
	.align	2
	.type	route_list_inuse, @object
	.size	route_list_inuse, 12
route_list_inuse:
	.space	12
	.globl	route_list_free
	.section	.bss.route_list_free,"aw",@nobits
	.align	2
	.type	route_list_free, @object
	.size	route_list_free, 12
route_list_free:
	.space	12
	.globl	napt_list_inuse
	.section	.bss.napt_list_inuse,"aw",@nobits
	.align	2
	.type	napt_list_inuse, @object
	.size	napt_list_inuse, 12
napt_list_inuse:
	.space	12
	.globl	napt_list_free
	.section	.bss.napt_list_free,"aw",@nobits
	.align	2
	.type	napt_list_free, @object
	.size	napt_list_free, 12
napt_list_free:
	.space	12
	.globl	table_napt
	.section	.bss.table_napt,"aw",@nobits
	.align	2
	.type	table_napt, @object
	.size	table_napt, 4
table_napt:
	.space	4
	.globl	path_list_inuse
	.section	.bss.path_list_inuse,"aw",@nobits
	.align	2
	.type	path_list_inuse, @object
	.size	path_list_inuse, 12
path_list_inuse:
	.space	12
	.globl	path_list_free
	.section	.bss.path_list_free,"aw",@nobits
	.align	2
	.type	path_list_free, @object
	.size	path_list_free, 12
path_list_free:
	.space	12
	.globl	table_path
	.section	.bss.table_path,"aw",@nobits
	.align	2
	.type	table_path, @object
	.size	table_path, 4
table_path:
	.space	4
	.globl	udp_cache_list_free
	.section	.bss.udp_cache_list_free,"aw",@nobits
	.align	2
	.type	udp_cache_list_free, @object
	.size	udp_cache_list_free, 12
udp_cache_list_free:
	.space	12
	.globl	udp_cache_table
	.section	.bss.udp_cache_table,"aw",@nobits
	.align	2
	.type	udp_cache_table, @object
	.size	udp_cache_table, 4
udp_cache_table:
	.space	4
	.globl	negative_cache_list_free
	.section	.bss.negative_cache_list_free,"aw",@nobits
	.align	2
	.type	negative_cache_list_free, @object
	.size	negative_cache_list_free, 12
negative_cache_list_free:
	.space	12
	.globl	negative_cache_table
	.section	.bss.negative_cache_table,"aw",@nobits
	.align	2
	.type	negative_cache_table, @object
	.size	negative_cache_table, 4
negative_cache_table:
	.space	4
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 1459) 4.4.7"
