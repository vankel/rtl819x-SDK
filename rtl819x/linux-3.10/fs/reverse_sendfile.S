	.file	1 "reverse_sendfile.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.free_early_copied_skbs,"ax",@progbits
	.align	2
	.globl	free_early_copied_skbs
	.set	nomips16
	.ent	free_early_copied_skbs
	.type	free_early_copied_skbs, @function
free_early_copied_skbs:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	free_early_copied_skbs
	.size	free_early_copied_skbs, .-free_early_copied_skbs
	.section	.text.memcpy_tokerneliovec,"ax",@progbits
	.align	2
	.globl	memcpy_tokerneliovec
	.set	nomips16
	.ent	memcpy_tokerneliovec
	.type	memcpy_tokerneliovec, @function
memcpy_tokerneliovec:
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
	move	$16,$4
	move	$19,$5
	j	$L4
	move	$17,$6

$L6:
	beql	$2,$0,$L4
	addiu	$16,$16,8

	sltu	$18,$2,$17
	lw	$4,0($16)
	movz	$2,$17,$18
	move	$5,$19
	move	$6,$2
	jal	memcpy
	move	$18,$2

	lw	$3,4($16)
	lw	$2,0($16)
	addu	$19,$19,$18
	subu	$3,$3,$18
	addu	$2,$2,$18
	sw	$3,4($16)
	sw	$2,0($16)
	subu	$17,$17,$18
	addiu	$16,$16,8
$L4:
	bgtzl	$17,$L6
	lw	$2,4($16)

	lw	$31,36($sp)
	lw	$19,32($sp)
	lw	$18,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	memcpy_tokerneliovec
	.size	memcpy_tokerneliovec, .-memcpy_tokerneliovec
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"fs/reverse_sendfile.c\000"
	.section	.text.skb_copy_datagram_iovec1,"ax",@progbits
	.align	2
	.globl	skb_copy_datagram_iovec1
	.set	nomips16
	.ent	skb_copy_datagram_iovec1
	.type	skb_copy_datagram_iovec1, @function
skb_copy_datagram_iovec1:
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
	lw	$20,80($4)
	lw	$2,84($4)
	move	$17,$5
	move	$18,$4
	subu	$20,$20,$2
	subu	$21,$20,$5
	move	$5,$7
	move	$16,$7
	jal	do_trace_skb_copy_datagram_iovec
	move	$19,$6

	blez	$21,$L10
	slt	$2,$21,$16

	move	$4,$18
	jal	rtl_get_skb_data_common
	movz	$21,$16,$2

	addu	$5,$2,$17
	move	$4,$19
	jal	memcpy_tokerneliovec
	move	$6,$21

	bne	$2,$0,$L11
	subu	$16,$16,$21

	beq	$16,$0,$L12
	addu	$17,$17,$21

$L10:
	move	$21,$0
	j	$L13
	li	$fp,-2147483648			# 0xffffffff80000000

$L16:
	beql	$4,$0,$L23
	lw	$2,188($18)

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	jal	warn_slowpath_null
	li	$5,388			# 0x184

	lw	$2,188($18)
$L23:
	addiu	$4,$21,5
	sll	$4,$4,3
	addiu	$5,$2,44
	addu	$2,$2,$4
	lhu	$23,10($2)
	sll	$3,$21,3
	addu	$3,$5,$3
	addu	$23,$20,$23
	subu	$2,$23,$17
	slt	$22,$16,$2
	move	$5,$16
	movz	$5,$2,$22
	move	$4,$19
	addiu	$21,$21,1
	move	$22,$5
	blez	$2,$L15
	move	$6,$5

	lui	$2,%hi(mem_map)
	lw	$5,%lo(mem_map)($2)
	lw	$7,0($3)
	lhu	$2,4($3)
	subu	$16,$16,$22
	subu	$5,$7,$5
	sra	$5,$5,5
	addu	$2,$17,$2
	sll	$5,$5,14
	subu	$20,$2,$20
	addu	$5,$5,$fp
	jal	memcpy_tokerneliovec
	addu	$5,$5,$20

	bne	$2,$0,$L21
	li	$2,-14			# 0xfffffffffffffff2

	beq	$16,$0,$L12
	addu	$17,$17,$22

$L15:
	move	$20,$23
$L13:
	lw	$2,188($18)
	addu	$4,$16,$17
	lbu	$3,0($2)
	slt	$3,$21,$3
	bne	$3,$0,$L16
	slt	$4,$4,$20

	lui	$22,%hi($LC0)
	lw	$18,8($2)
	j	$L17
	addiu	$22,$22,%lo($LC0)

$L20:
	beq	$2,$0,$L18
	li	$5,423			# 0x1a7

	jal	warn_slowpath_null
	move	$4,$22

$L18:
	jal	rtl_get_skb_len_common
	move	$4,$18

	addu	$21,$2,$20
	subu	$2,$21,$17
	subu	$5,$17,$20
	move	$3,$16
	slt	$20,$16,$2
	movz	$3,$2,$20
	move	$4,$18
	move	$6,$19
	move	$20,$3
	blez	$2,$L19
	move	$7,$3

	jal	skb_copy_datagram_iovec1
	subu	$16,$16,$20

	bnel	$2,$0,$L21
	li	$2,-14			# 0xfffffffffffffff2

	beq	$16,$0,$L12
	addu	$17,$17,$20

$L19:
	lw	$18,0($18)
	move	$20,$21
$L17:
	addu	$2,$16,$17
	bne	$18,$0,$L20
	slt	$2,$2,$20

	bnel	$16,$0,$L21
	li	$2,-14			# 0xfffffffffffffff2

$L12:
	j	$L21
	move	$2,$0

$L11:
	li	$2,-14			# 0xfffffffffffffff2
$L21:
	lw	$31,52($sp)
	lw	$fp,48($sp)
	lw	$23,44($sp)
	lw	$22,40($sp)
	lw	$21,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	skb_copy_datagram_iovec1
	.size	skb_copy_datagram_iovec1, .-skb_copy_datagram_iovec1
	.section	.text.balance_dirty_pages_ratelimited_nr,"ax",@progbits
	.align	2
	.globl	balance_dirty_pages_ratelimited_nr
	.set	nomips16
	.ent	balance_dirty_pages_ratelimited_nr
	.type	balance_dirty_pages_ratelimited_nr, @function
balance_dirty_pages_ratelimited_nr:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lw	$6,76($4)
	li	$3,32			# 0x20
	li	$2,8			# 0x8
	lw	$6,192($6)
	movz	$2,$3,$6
	lw	$7,16($28)
	lui	$6,%hi(__per_cpu_offset)
	addiu	$6,$6,%lo(__per_cpu_offset)
	sll	$7,$7,2
	addu	$6,$7,$6
	lw	$7,0($6)
	lui	$6,%hi(bdp_ratelimits)
	addiu	$6,$6,%lo(bdp_ratelimits)
	addu	$6,$6,$7
	lw	$7,0($6)
	addu	$5,$5,$7
	sltu	$2,$5,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L27
	sw	$5,0($6)
	.set	macro
	.set	reorder

	sltu	$2,$5,32
	sw	$0,0($6)
	movn	$5,$3,$2
	srl	$2,$5,1
	addu	$5,$2,$5
	j	balance_dirty_pages
$L27:
	j	$31
	.end	balance_dirty_pages_ratelimited_nr
	.size	balance_dirty_pages_ratelimited_nr, .-balance_dirty_pages_ratelimited_nr
	.section	.rodata.str1.4
	.align	2
$LC1:
	.ascii	"%s: count(%u) exceeds maxinum\012\000"
	.align	2
$LC2:
	.ascii	"do_recvfile: write_begin() or write_end() is not impleme"
	.ascii	"nted\012\000"
	.align	2
$LC3:
	.ascii	"do_recvfile: failed to get page from VFS.\012\000"
	.align	2
$LC4:
	.ascii	"do_recvfile: kernel_recvmsg_ret %d , count = %d, err = %"
	.ascii	"d\012\000"
	.align	2
$LC5:
	.ascii	"do_recvfile: write_end() failed! written: %d\012\000"
	.section	.text.do_splice_from_socket,"ax",@progbits
	.align	2
	.globl	do_splice_from_socket
	.set	nomips16
	.ent	do_splice_from_socket
	.type	do_splice_from_socket, @function
do_splice_from_socket:
	.frame	$sp,288,$31		# vars= 216, regs= 10/0, args= 32, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-288
	sw	$31,284($sp)
	sw	$fp,280($sp)
	sw	$23,276($sp)
	sw	$22,272($sp)
	sw	$21,268($sp)
	sw	$20,264($sp)
	sw	$19,260($sp)
	sw	$18,256($sp)
	sw	$17,252($sp)
	sw	$16,248($sp)
	sw	$6,296($sp)
	move	$fp,$5
	move	$21,$4
	jal	rtl_get_file_mapping
	sw	$7,300($sp)

	move	$17,$2
	lw	$2,296($sp)
	lw	$6,300($sp)
	lw	$23,0($17)
	lw	$5,4($2)
	lw	$4,0($2)
	li	$2,65536			# 0x10000
	ori	$2,$2,0x1
	sltu	$2,$6,$2
	sw	$5,44($sp)
	bne	$2,$0,$L31
	sw	$4,40($sp)

	lui	$4,%hi($LC1)
	lui	$5,%hi(__func__.45416)
	addiu	$4,$4,%lo($LC1)
	jal	printk
	addiu	$5,$5,%lo(__func__.45416)

	j	$L32
	li	$16,-22			# 0xffffffffffffffea

$L31:
	jal	rtl_inode_mutex_lock
	move	$4,$23

	jal	rtl_get_inode_sb
	move	$4,$23

	move	$4,$2
	jal	rtl_vfs_check_frozen
	li	$5,1			# 0x1

	lw	$3,76($17)
	lw	$2,0($28)
	move	$4,$23
	jal	rtl_get_inode_mode
	sw	$3,712($2)

	andi	$2,$2,0xf000
	xori	$7,$2,0x6000
	sltu	$7,$7,1
	move	$4,$21
	addiu	$5,$sp,40
	jal	generic_write_checks
	addiu	$6,$sp,300

	bne	$2,$0,$L33
	move	$16,$2

	lw	$2,300($sp)
	beq	$2,$0,$L36
	move	$18,$0

	lw	$2,68($17)
	lw	$3,20($2)
	beq	$3,$0,$L52
	lui	$4,%hi($LC2)

	lw	$2,24($2)
	bne	$2,$0,$L35
	nop

$L52:
	jal	printk
	addiu	$4,$4,%lo($LC2)

	j	$L36
	move	$18,$0

$L35:
	jal	file_remove_suid
	move	$4,$21

	jal	file_update_time
	move	$4,$21

	lw	$22,300($sp)
	addiu	$18,$sp,120
	addiu	$3,$sp,76
	move	$19,$0
$L40:
	li	$2,24			# 0x18
	mul	$5,$19,$2
	lw	$7,44($sp)
	addiu	$4,$sp,120
	lw	$2,68($17)
	li	$6,1			# 0x1
	sw	$6,20($sp)
	addiu	$6,$sp,32
	sw	$6,24($sp)
	lw	$6,40($sp)
	addu	$8,$5,$4
	andi	$4,$7,0x3fff
	li	$5,16384			# 0x4000
	sw	$4,240($sp)
	subu	$4,$5,$4
	sltu	$20,$4,$22
	movz	$4,$22,$20
	addiu	$8,$8,20
	sw	$4,16($sp)
	sw	$8,28($sp)
	lw	$2,20($2)
	move	$20,$4
	sw	$3,244($sp)
	move	$4,$21
	jalr	$2
	move	$5,$17

	subu	$22,$22,$20
	beq	$2,$0,$L37
	lw	$3,244($sp)

	lui	$4,%hi($LC3)
	addiu	$4,$4,%lo($LC3)
	jal	printk
	sw	$2,244($sp)

	lw	$5,244($sp)
	move	$22,$0
	move	$18,$0
	j	$L38
	move	$16,$5

$L37:
	lw	$2,20($17)
	beq	$2,$0,$L39
	addiu	$19,$19,1

	lw	$4,32($sp)
	jal	__flush_dcache_page
	sw	$3,244($sp)

	lw	$3,244($sp)
$L39:
	lw	$2,32($sp)
	lui	$8,%hi(mem_map)
	lw	$7,%lo(mem_map)($8)
	sw	$2,0($18)
	lw	$4,44($sp)
	subu	$7,$2,$7
	lw	$5,40($sp)
	lw	$8,240($sp)
	sra	$7,$7,5
	addu	$2,$20,$4
	li	$6,-2147483648			# 0xffffffff80000000
	sll	$7,$7,14
	addu	$7,$7,$6
	sltu	$6,$2,$20
	addu	$7,$7,$8
	addu	$6,$6,$5
	sw	$5,8($18)
	sw	$4,12($18)
	sw	$20,16($18)
	sw	$7,0($3)
	sw	$20,4($3)
	addiu	$18,$18,24
	sw	$6,40($sp)
	sw	$2,44($sp)
	bne	$22,$0,$L40
	addiu	$3,$3,8

	move	$4,$fp
	li	$2,131072			# 0x20000
	sw	$2,72($sp)
	sw	$0,48($sp)
	sw	$0,52($sp)
	sw	$0,64($sp)
	jal	rtl_get_sk_rcvtimeo
	sw	$0,68($sp)

	move	$4,$fp
	li	$5,3200			# 0xc80
	jal	rtl_set_sk_rcvtimeo
	move	$18,$2

	lw	$2,300($sp)
	move	$4,$fp
	addiu	$5,$sp,48
	sw	$2,16($sp)
	li	$2,262144			# 0x40000
	ori	$2,$2,0x100
	addiu	$6,$sp,76
	move	$7,$19
	jal	kernel_recvmsg
	sw	$2,20($sp)

	move	$22,$2
	move	$4,$fp
	jal	rtl_set_sk_rcvtimeo
	move	$5,$18

	bgez	$22,$L41
	lw	$2,300($sp)

	move	$16,$22
	j	$L42
	move	$18,$0

$L41:
	beq	$22,$2,$L38
	move	$18,$22

	li	$16,-32			# 0xffffffffffffffe0
$L42:
	lw	$6,300($sp)
	lui	$4,%hi($LC4)
	addiu	$4,$4,%lo($LC4)
	move	$5,$22
	jal	printk
	move	$7,$16

$L38:
	lw	$3,44($sp)
	lw	$2,40($sp)
	lw	$4,296($sp)
	addiu	$20,$sp,136
	move	$fp,$0
	sw	$3,4($4)
	j	$L44
	sw	$2,0($4)

$L47:
	jal	mark_page_accessed
	addiu	$fp,$fp,1

	lw	$2,0($20)
	lw	$5,-16($20)
	lw	$4,4($20)
	lw	$3,68($17)
	sw	$5,24($sp)
	sw	$4,28($sp)
	sw	$2,20($sp)
	sw	$2,16($sp)
	lw	$7,-4($20)
	lw	$6,-8($20)
	lw	$2,24($3)
	move	$4,$21
	jalr	$2
	move	$5,$17

	bgezl	$2,$L44
	addiu	$20,$20,24

	blez	$22,$L45
	movz	$16,$2,$16

	lw	$2,0($20)
	lui	$4,%hi($LC5)
	addiu	$4,$4,%lo($LC5)
	subu	$18,$18,$2
	jal	printk
	move	$5,$18

$L45:
	addiu	$20,$20,24
$L44:
	slt	$2,$fp,$19
	bnel	$2,$0,$L47
	lw	$4,-16($20)

	bnel	$16,$0,$L51
	lw	$2,0($28)

	move	$4,$17
	jal	balance_dirty_pages_ratelimited_nr
	move	$5,$19

	j	$L51
	lw	$2,0($28)

$L33:
	move	$18,$0
$L36:
	lw	$2,0($28)
$L51:
	move	$4,$23
	movz	$16,$18,$16
	jal	rtl_inode_mutex_unlock
	sw	$0,712($2)

$L32:
	move	$2,$16
	lw	$31,284($sp)
	lw	$fp,280($sp)
	lw	$23,276($sp)
	lw	$22,272($sp)
	lw	$21,268($sp)
	lw	$20,264($sp)
	lw	$19,260($sp)
	lw	$18,256($sp)
	lw	$17,252($sp)
	lw	$16,248($sp)
	j	$31
	addiu	$sp,$sp,288

	.set	macro
	.set	reorder
	.end	do_splice_from_socket
	.size	do_splice_from_socket, .-do_splice_from_socket
	.section	.text.find_fd_from_socket,"ax",@progbits
	.align	2
	.globl	find_fd_from_socket
	.set	nomips16
	.ent	find_fd_from_socket
	.type	find_fd_from_socket, @function
find_fd_from_socket:
	.frame	$sp,56,$31		# vars= 16, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$19,48($sp)
	sw	$18,44($sp)
	sw	$17,40($sp)
	sw	$16,36($sp)
	li	$2,-9			# 0xfffffffffffffff7
	move	$18,$5
	addiu	$5,$sp,20
	sw	$6,24($sp)
	sw	$2,20($sp)
	jal	sockfd_lookup
	move	$19,$7

	move	$17,$2
	beq	$2,$0,$L54
	lw	$6,24($sp)

	lw	$2,20($2)
	beq	$2,$0,$L55
	move	$4,$6

	jal	fget_light
	addiu	$5,$sp,16

	beq	$2,$0,$L54
	move	$16,$2

	jal	rtl_get_file_mapping
	move	$4,$2

	lw	$2,68($2)
	lw	$3,20($2)
	beql	$3,$0,$L66
	lw	$2,20($sp)

	lw	$2,24($2)
	beq	$2,$0,$L66
	lw	$2,20($sp)

	jal	rtl_get_file_mode
	move	$4,$16

	andi	$2,$2,0x2
	beq	$2,$0,$L67
	lw	$2,16($sp)

	bne	$18,$0,$L68
	move	$6,$18

	jal	rtl_get_file_pos_addr
	move	$4,$16

	move	$18,$2
	move	$6,$18
$L68:
	move	$7,$19
	move	$4,$16
	jal	do_splice_from_socket
	move	$5,$17

	j	$L60
	sw	$2,20($sp)

$L65:
	jal	fput
	move	$4,$16

$L55:
	jal	fput
	lw	$4,16($17)

$L54:
	lw	$2,20($sp)
$L66:
	lw	$31,52($sp)
	lw	$19,48($sp)
	lw	$18,44($sp)
	lw	$17,40($sp)
	lw	$16,36($sp)
	j	$31
	addiu	$sp,$sp,56

$L60:
	lw	$2,16($sp)
$L67:
	beq	$2,$0,$L55
	nop

	j	$L65
	nop

	.set	macro
	.set	reorder
	.end	find_fd_from_socket
	.size	find_fd_from_socket, .-find_fd_from_socket
	.section	.rodata.__func__.45416,"a",@progbits
	.align	2
	.type	__func__.45416, @object
	.size	__func__.45416, 22
__func__.45416:
	.ascii	"do_splice_from_socket\000"
	.section	.data..percpu,"aw",@progbits
	.align	2
	.type	bdp_ratelimits, @object
	.size	bdp_ratelimits, 4
bdp_ratelimits:
	.space	4
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 1459) 4.4.7"
