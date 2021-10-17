	.file	1 "rtkjnl.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.text
	.align	2
	.globl	bsp_id_check
	.set	nomips16
	.ent	bsp_id_check
	.type	bsp_id_check, @function
bsp_id_check:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	li	$2,-1207959552			# 0xffffffffb8000000
	lw	$3,0($2)
	li	$2,-65536			# 0xffffffffffff0000
	and	$2,$3,$2
	li	$3,-2120744960			# 0xffffffff81980000
	xor	$2,$2,$3
	li	$3,-1			# 0xffffffffffffffff
	movz	$3,$0,$2
	j	$31
	move	$2,$3

	.set	macro
	.set	reorder
	.end	bsp_id_check
	.size	bsp_id_check, .-bsp_id_check
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"version\000"
	.align	2
$LC1:
	.ascii	"\0015jnl: driver unloaded\012\000"
	.section	.exit.text,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	jnl_exit
	.type	jnl_exit, @function
jnl_exit:
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
	lui	$17,%hi(proc_info_root)
	lw	$5,%lo(proc_info_root)($17)
	beq	$5,$0,$L6
	lui	$4,%hi($LC0)

	lui	$16,%hi(remove_proc_entry)
	addiu	$16,$16,%lo(remove_proc_entry)
	jalr	$16
	addiu	$4,$4,%lo($LC0)

	lui	$4,%hi(proc_info_root_name)
	addiu	$4,$4,%lo(proc_info_root_name)
	move	$5,$0
	jalr	$16
	sw	$0,%lo(proc_info_root)($17)

$L6:
	lui	$2,%hi(jnl_free_data_cachep)
	lw	$4,%lo(jnl_free_data_cachep)($2)
	beq	$4,$0,$L7
	lui	$2,%hi(kmem_cache_destroy)

	addiu	$2,$2,%lo(kmem_cache_destroy)
	jalr	$2
	nop

$L7:
	lui	$2,%hi(jnl_handle_cache)
	lw	$4,%lo(jnl_handle_cache)($2)
	beq	$4,$0,$L8
	lui	$2,%hi(kmem_cache_destroy)

	addiu	$2,$2,%lo(kmem_cache_destroy)
	jalr	$2
	nop

$L8:
	lui	$2,%hi(jnl_head_cache)
	lw	$4,%lo(jnl_head_cache)($2)
	beq	$4,$0,$L9
	lui	$2,%hi(kmem_cache_destroy)

	addiu	$2,$2,%lo(kmem_cache_destroy)
	jalr	$2
	nop

$L9:
	lui	$2,%hi(jnl_revoke_record_cache)
	lw	$4,%lo(jnl_revoke_record_cache)($2)
	beq	$4,$0,$L10
	lui	$2,%hi(kmem_cache_destroy)

	addiu	$2,$2,%lo(kmem_cache_destroy)
	jalr	$2
	nop

$L10:
	lui	$16,%hi(jnl_slab)
	lui	$18,%hi(jnl_slab+36)
	lui	$17,%hi(kmem_cache_destroy)
	addiu	$16,$16,%lo(jnl_slab)
	addiu	$18,$18,%lo(jnl_slab+36)
	addiu	$17,$17,%lo(kmem_cache_destroy)
	lw	$2,0($16)
$L16:
	addiu	$16,$16,4
	beq	$2,$0,$L11
	move	$4,$2

	jalr	$17
	nop

$L11:
	bnel	$16,$18,$L16
	lw	$2,0($16)

	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	lui	$4,%hi($LC1)
	lui	$25,%hi(printk)
	addiu	$4,$4,%lo($LC1)
	addiu	$25,$25,%lo(printk)
	jr	$25
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	jnl_exit
	.size	jnl_exit, .-jnl_exit
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	"\0015jnl: vendor chip is not supported!\012\000"
	.align	2
$LC3:
	.ascii	"\0015jnl: cannot create /proc/%s\000"
	.align	2
$LC4:
	.ascii	"jnl_revoke_record\000"
	.align	2
$LC5:
	.ascii	"jnl_head\000"
	.align	2
$LC6:
	.ascii	"jnl_handle\000"
	.align	2
$LC7:
	.ascii	"jnl_free_data\000"
	.align	2
$LC8:
	.ascii	"\0015jnl: driver (%s) loaded at %p\012\000"
	.section	.init.text,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	jnl_init
	.type	jnl_init, @function
jnl_init:
	.frame	$sp,40,$31		# vars= 0, regs= 4/0, args= 24, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	li	$2,-1207959552			# 0xffffffffb8000000
	lw	$3,0($2)
	li	$2,-65536			# 0xffffffffffff0000
	lui	$16,%hi(printk)
	and	$3,$3,$2
	li	$2,-2120744960			# 0xffffffff81980000
	beq	$3,$2,$L18
	addiu	$16,$16,%lo(printk)

	lui	$4,%hi($LC2)
	jalr	$16
	addiu	$4,$4,%lo($LC2)

	j	$L19
	li	$2,-12			# 0xfffffffffffffff4

$L29:
	lui	$2,%hi(proc_mkdir)
	addiu	$4,$18,%lo(proc_info_root_name)
	addiu	$2,$2,%lo(proc_mkdir)
	jalr	$2
	move	$5,$0

	beq	$2,$0,$L20
	sw	$2,%lo(proc_info_root)($17)

	move	$6,$2
	lui	$4,%hi($LC0)
	lui	$7,%hi(jnl_proc_version_fops)
	lui	$2,%hi(proc_create_data)
	addiu	$4,$4,%lo($LC0)
	addiu	$7,$7,%lo(jnl_proc_version_fops)
	move	$5,$0
	addiu	$2,$2,%lo(proc_create_data)
	jalr	$2
	sw	$0,16($sp)

	j	$L30
	lui	$2,%hi(size_jnl_revoke_record)

$L20:
	lui	$4,%hi($LC3)
	addiu	$4,$4,%lo($LC3)
	jalr	$16
	addiu	$5,$18,%lo(proc_info_root_name)

	lui	$2,%hi(size_jnl_revoke_record)
$L30:
	lw	$5,%lo(size_jnl_revoke_record)($2)
	lui	$4,%hi($LC4)
	li	$7,131072			# 0x20000
	lui	$16,%hi(kmem_cache_create)
	addiu	$4,$4,%lo($LC4)
	ori	$7,$7,0x2000
	addiu	$16,$16,%lo(kmem_cache_create)
	move	$6,$0
	jalr	$16
	sw	$0,16($sp)

	lui	$3,%hi(jnl_revoke_record_cache)
	beq	$2,$0,$L22
	sw	$2,%lo(jnl_revoke_record_cache)($3)

	lui	$2,%hi(size_jnl_head)
	lw	$5,%lo(size_jnl_head)($2)
	lui	$4,%hi($LC5)
	addiu	$4,$4,%lo($LC5)
	move	$6,$0
	li	$7,131072			# 0x20000
	jalr	$16
	sw	$0,16($sp)

	lui	$3,%hi(jnl_head_cache)
	beq	$2,$0,$L23
	sw	$2,%lo(jnl_head_cache)($3)

	lui	$2,%hi(size_jnl_handle)
	lw	$5,%lo(size_jnl_handle)($2)
	lui	$4,%hi($LC6)
	addiu	$4,$4,%lo($LC6)
	move	$6,$0
	li	$7,131072			# 0x20000
	jalr	$16
	sw	$0,16($sp)

	lui	$17,%hi(jnl_handle_cache)
	beq	$2,$0,$L24
	sw	$2,%lo(jnl_handle_cache)($17)

	lui	$2,%hi(size_jnl_free_data)
	lw	$5,%lo(size_jnl_free_data)($2)
	lui	$4,%hi($LC7)
	addiu	$4,$4,%lo($LC7)
	move	$6,$0
	li	$7,131072			# 0x20000
	jalr	$16
	sw	$0,16($sp)

	move	$3,$2
	lui	$2,%hi(jnl_free_data_cachep)
	sw	$3,%lo(jnl_free_data_cachep)($2)
	bne	$3,$0,$L19
	move	$2,$0

$L25 = .
	lui	$2,%hi(kmem_cache_destroy)
	addiu	$2,$2,%lo(kmem_cache_destroy)
	jalr	$2
	lw	$4,%lo(jnl_handle_cache)($17)

	sw	$0,%lo(jnl_handle_cache)($17)
$L24:
	lui	$16,%hi(jnl_head_cache)
	lui	$2,%hi(kmem_cache_destroy)
	addiu	$2,$2,%lo(kmem_cache_destroy)
	jalr	$2
	lw	$4,%lo(jnl_head_cache)($16)

	sw	$0,%lo(jnl_head_cache)($16)
$L23:
	lui	$16,%hi(jnl_revoke_record_cache)
	lw	$4,%lo(jnl_revoke_record_cache)($16)
	lui	$2,%hi(kmem_cache_destroy)
	addiu	$2,$2,%lo(kmem_cache_destroy)
	jalr	$2
	lui	$17,%hi(proc_info_root)

	lw	$5,%lo(proc_info_root)($17)
	beq	$5,$0,$L22
	sw	$0,%lo(jnl_revoke_record_cache)($16)

	lui	$4,%hi($LC0)
	lui	$16,%hi(remove_proc_entry)
	addiu	$16,$16,%lo(remove_proc_entry)
	jalr	$16
	addiu	$4,$4,%lo($LC0)

	lui	$4,%hi(proc_info_root_name)
	addiu	$4,$4,%lo(proc_info_root_name)
	move	$5,$0
	jalr	$16
	sw	$0,%lo(proc_info_root)($17)

$L22:
	li	$2,-12			# 0xfffffffffffffff4
$L19:
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,40

$L18:
	lui	$2,%hi(__this_module+196)
	lw	$6,%lo(__this_module+196)($2)
	lui	$4,%hi($LC8)
	lui	$5,%hi(s_DriverVer)
	addiu	$4,$4,%lo($LC8)
	addiu	$5,$5,%lo(s_DriverVer)
	jalr	$16
	lui	$17,%hi(proc_info_root)

	lw	$2,%lo(proc_info_root)($17)
	bne	$2,$0,$L30
	lui	$2,%hi(size_jnl_revoke_record)

	j	$L29
	lui	$18,%hi(proc_info_root_name)

	.set	macro
	.set	reorder
	.end	jnl_init
	.size	jnl_init, .-jnl_init
	.section	.modinfo,"a",@progbits
	.type	__UNIQUE_ID_license1, @object
	.size	__UNIQUE_ID_license1, 12
__UNIQUE_ID_license1:
	.ascii	"license=GPL\000"
	.type	__UNIQUE_ID_description0, @object
	.size	__UNIQUE_ID_description0, 31
__UNIQUE_ID_description0:
	.ascii	"description=Paragon jnl driver\000"
	.globl	init_module
	init_module = jnl_init
	.globl	cleanup_module
	cleanup_module = jnl_exit
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 1459) 4.4.7"
