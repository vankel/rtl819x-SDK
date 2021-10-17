	.file	1 "reverse_sendfile.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.rtl_tcp_ctl_init,"ax",@progbits
	.align	2
	.globl	rtl_tcp_ctl_init
	.set	nomips16
	.ent	rtl_tcp_ctl_init
	.type	rtl_tcp_ctl_init, @function
rtl_tcp_ctl_init:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$4,%hi(g2_tcp_ctl)
	addiu	$2,$4,%lo(g2_tcp_ctl)
	li	$5,16			# 0x10
	sw	$5,8($2)
	li	$5,8192			# 0x2000
	sw	$5,12($2)
	li	$5,4096			# 0x1000
	li	$3,1			# 0x1
	sw	$5,16($2)
	li	$5,2			# 0x2
	sw	$5,20($2)
	sw	$3,24($2)
	sw	$0,28($2)
	sw	$3,%lo(g2_tcp_ctl)($4)
	j	$31
	sw	$3,4($2)

	.set	macro
	.set	reorder
	.end	rtl_tcp_ctl_init
	.size	rtl_tcp_ctl_init, .-rtl_tcp_ctl_init
	.section	.text.rtl_splice_from_pipe_feed,"ax",@progbits
	.align	2
	.globl	rtl_splice_from_pipe_feed
	.set	nomips16
	.ent	rtl_splice_from_pipe_feed
	.type	rtl_splice_from_pipe_feed, @function
rtl_splice_from_pipe_feed:
	.frame	$sp,56,$31		# vars= 8, regs= 8/0, args= 16, gp= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$22,48($sp)
	sw	$21,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	lui	$2,%hi(pipe_to_sendpages)
	addiu	$2,$2,%lo(pipe_to_sendpages)
	move	$19,$6
	move	$17,$5
	bne	$6,$2,$L4
	move	$16,$4

	lw	$5,4($5)
	lw	$2,84($4)
	beq	$5,$0,$L5
	lw	$3,40($4)

	li	$18,24			# 0x18
	mul	$5,$3,$18
	move	$6,$17
	li	$19,24			# 0x18
	addu	$18,$5,$2
	jal	pipe_to_sendpages
	move	$5,$18

	bgtzl	$2,$L24
	lw	$3,8($18)

	j	$L22
	li	$3,-61			# 0xffffffffffffffc3

$L10:
	lw	$6,12($18)
	sw	$0,12($18)
	subu	$2,$2,$3
	lw	$3,16($6)
	jalr	$3
	sw	$2,16($sp)

	lw	$4,40($16)
	lw	$18,44($16)
	lw	$3,36($16)
	addiu	$4,$4,1
	addiu	$18,$18,-1
	and	$18,$18,$4
	addiu	$3,$3,-1
	sw	$3,36($16)
	sw	$18,40($16)
	lw	$2,16($sp)
	beq	$2,$0,$L9
	mul	$4,$18,$19

	lw	$3,84($16)
	addu	$18,$4,$3
	lw	$3,8($18)
$L24:
	move	$5,$18
	sltu	$6,$2,$3
	beq	$6,$0,$L10
	move	$4,$16

	beq	$2,$0,$L9
	subu	$3,$3,$2

	lw	$4,4($18)
	sw	$3,8($18)
	addu	$2,$4,$2
	sw	$2,4($18)
$L9:
	lw	$2,56($16)
	beql	$2,$0,$L25
	lw	$2,4($17)

	li	$2,1			# 0x1
	sb	$2,32($17)
	lw	$2,4($17)
$L25:
	j	$L8
	sltu	$2,$0,$2

$L15:
	lw	$18,40($16)
	lw	$3,84($16)
	lw	$2,4($17)
	mul	$5,$18,$21
	addu	$18,$5,$3
	lw	$3,8($18)
	lw	$22,12($18)
	move	$5,$18
	sltu	$7,$2,$3
	beq	$7,$0,$L12
	sw	$3,0($17)

	sw	$2,0($17)
$L12:
	jalr	$19
	nop

	bgtz	$2,$L13
	sra	$8,$2,31

	li	$3,-61			# 0xffffffffffffffc3
$L22:
	bne	$2,$3,$L23
	lw	$31,52($sp)

	j	$L23
	move	$2,$0

$L13:
	lw	$3,4($18)
	lw	$4,8($18)
	addu	$3,$3,$2
	subu	$4,$4,$2
	sw	$4,8($18)
	sw	$3,4($18)
	lw	$4,20($17)
	lw	$9,16($17)
	lw	$5,28($17)
	lw	$7,0($17)
	lw	$6,4($17)
	addu	$3,$4,$2
	addu	$8,$9,$8
	sltu	$4,$3,$4
	subu	$6,$6,$2
	addu	$5,$5,$2
	addu	$4,$4,$8
	subu	$2,$7,$2
	sw	$6,4($17)
	sw	$5,28($17)
	sw	$2,0($17)
	sw	$4,16($17)
	sw	$3,20($17)
	lw	$2,8($18)
	bnel	$2,$0,$L26
	lw	$2,4($17)

	sw	$0,12($18)
	lw	$2,16($22)
	move	$5,$18
	jalr	$2
	move	$4,$16

	lw	$4,40($16)
	lw	$5,44($16)
	lw	$3,36($16)
	lw	$2,56($16)
	addiu	$5,$5,-1
	addiu	$4,$4,1
	and	$4,$5,$4
	addiu	$3,$3,-1
	sw	$4,40($16)
	beq	$2,$0,$L14
	sw	$3,36($16)

	sb	$20,32($17)
$L14:
	lw	$2,4($17)
$L26:
	bnel	$2,$0,$L27
	lw	$2,36($16)

	j	$L7
	move	$2,$0

$L4:
	li	$21,24			# 0x18
	li	$20,1			# 0x1
	lw	$2,36($16)
$L27:
	move	$4,$16
	bne	$2,$0,$L15
	move	$6,$17

	j	$L8
	li	$2,1			# 0x1

$L7:
	j	$L23
	lw	$31,52($sp)

$L5:
	li	$2,1			# 0x1
$L8:
	lw	$31,52($sp)
$L23:
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	rtl_splice_from_pipe_feed
	.size	rtl_splice_from_pipe_feed, .-rtl_splice_from_pipe_feed
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
	j	$L29
	move	$17,$6

$L31:
	beql	$2,$0,$L29
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
$L29:
	bgtzl	$17,$L31
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
	.section	.text.skb_copy_datagram_to_kernel_iovec,"ax",@progbits
	.align	2
	.globl	skb_copy_datagram_to_kernel_iovec
	.set	nomips16
	.ent	skb_copy_datagram_to_kernel_iovec
	.type	skb_copy_datagram_to_kernel_iovec, @function
skb_copy_datagram_to_kernel_iovec:
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
	move	$16,$5
	move	$17,$6
	move	$20,$7
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_shared_info
	move	$19,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L34
	lw	$22,8($2)
	.set	macro
	.set	reorder

	move	$18,$0
	li	$fp,-2147483648			# 0xffffffff80000000
$L42:
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_headlen
	move	$4,$19
	.set	macro
	.set	reorder

	li	$21,-1			# 0xffffffffffffffff
	move	$3,$18
$L39:
	addu	$18,$18,$2
	subu	$2,$18,$16
	slt	$5,$16,$18
	move	$4,$19
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L35
	slt	$23,$20,$2
	.set	macro
	.set	reorder

	movn	$2,$20,$23
	move	$23,$2
	subu	$3,$16,$3
	li	$2,-1			# 0xffffffffffffffff
	.set	noreorder
	.set	nomacro
	bne	$21,$2,$L36
	sw	$3,16($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data_common
	subu	$20,$20,$23
	.set	macro
	.set	reorder

	lw	$3,16($sp)
	move	$4,$17
	move	$6,$23
	.set	noreorder
	.set	nomacro
	j	$L45
	addu	$5,$2,$3
	.set	macro
	.set	reorder

$L36:
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_shared_info
	subu	$20,$20,$23
	.set	macro
	.set	reorder

	addiu	$2,$2,44
	sll	$3,$21,3
	addu	$4,$2,$3
	lui	$2,%hi(mem_map)
	lw	$5,0($4)
	lw	$3,%lo(mem_map)($2)
	subu	$5,$5,$3
	sra	$5,$5,5
	sll	$5,$5,14
	addu	$5,$5,$fp
	.set	noreorder
	.set	nomacro
	jal	rtl_get_frag_offset
	sw	$5,20($sp)
	.set	macro
	.set	reorder

	lw	$3,16($sp)
	lw	$5,20($sp)
	move	$6,$23
	addu	$2,$2,$3
	addu	$5,$5,$2
	move	$4,$17
$L45:
	.set	noreorder
	.set	nomacro
	jal	memcpy_tokerneliovec
	addu	$16,$16,$23
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L41
	move	$2,$0
	.set	macro
	.set	reorder

$L35:
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_shared_info
	move	$4,$19
	.set	macro
	.set	reorder

	lbu	$2,0($2)
	addiu	$21,$21,1
	slt	$2,$21,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L38
	move	$4,$19
	.set	macro
	.set	reorder

	jal	rtl_skb_shared_info
	addiu	$3,$21,5
	sll	$3,$3,3
	addu	$2,$2,$3
	lhu	$2,10($2)
	.set	noreorder
	.set	nomacro
	j	$L39
	move	$3,$18
	.set	macro
	.set	reorder

$L38:
	.set	noreorder
	.set	nomacro
	beq	$22,$0,$L41
	li	$2,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_shared_info
	move	$4,$22
	.set	macro
	.set	reorder

	lw	$2,8($2)
 #APP
 # 29 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/bug.h" 1
	tne $0, $2, 12
 # 0 "" 2
 #NO_APP
	move	$19,$22
	.set	noreorder
	.set	nomacro
	j	$L42
	lw	$22,0($22)
	.set	macro
	.set	reorder

$L34:
	move	$2,$0
$L41:
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

	.end	skb_copy_datagram_to_kernel_iovec
	.size	skb_copy_datagram_to_kernel_iovec, .-skb_copy_datagram_to_kernel_iovec
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
	lw	$2,76($4)
	lui	$3,%hi(ratelimit_pages)
	lw	$6,%lo(ratelimit_pages)($3)
	lw	$7,192($2)
	li	$2,8			# 0x8
	movz	$2,$6,$7
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
	bne	$2,$0,$L49
	sw	$5,0($6)
	.set	macro
	.set	reorder

	lw	$2,%lo(ratelimit_pages)($3)
	sw	$0,0($6)
	sltu	$3,$5,$2
	movn	$5,$2,$3
	srl	$2,$5,1
	addu	$5,$2,$5
	j	balance_dirty_pages
$L49:
	j	$31
	.end	balance_dirty_pages_ratelimited_nr
	.size	balance_dirty_pages_ratelimited_nr, .-balance_dirty_pages_ratelimited_nr
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
	.ascii	"%s: %d: %s:count(%d) exceed maxinum\012\000"
	.align	2
$LC1:
	.ascii	"fs/reverse_sendfile_fastpath/reverse_sendfile.c\000"
	.align	2
$LC2:
	.ascii	"%s: %d: %s: write_end fail,ret = %d\012\000"
	.section	.text.do_splice_from_socket,"ax",@progbits
	.align	2
	.globl	do_splice_from_socket
	.set	nomips16
	.ent	do_splice_from_socket
	.type	do_splice_from_socket, @function
do_splice_from_socket:
	.frame	$sp,704,$31		# vars= 632, regs= 10/0, args= 32, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-704
	sw	$31,700($sp)
	sw	$fp,696($sp)
	sw	$23,692($sp)
	sw	$22,688($sp)
	sw	$21,684($sp)
	sw	$20,680($sp)
	sw	$19,676($sp)
	sw	$18,672($sp)
	sw	$17,668($sp)
	sw	$16,664($sp)
	move	$20,$6
	sw	$7,716($sp)
	move	$19,$5
	.set	noreorder
	.set	nomacro
	jal	rtl_get_file_mapping
	move	$18,$4
	.set	macro
	.set	reorder

	move	$16,$2
	addiu	$2,$20,8
	or	$2,$2,$20
	sw	$2,628($sp)
	lw	$2,24($28)
	lw	$3,628($sp)
	and	$2,$3,$2
	lw	$3,0($16)
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L52
	sw	$3,624($sp)
	.set	macro
	.set	reorder

	addiu	$4,$sp,40
	move	$5,$20
	li	$6,8			# 0x8
 #APP
 # 574 "fs/reverse_sendfile_fastpath/reverse_sendfile.c" 1
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
	bne	$6,$0,$L52
	lw	$2,716($sp)
	.set	macro
	.set	reorder

	li	$3,262144			# 0x40000
	ori	$3,$3,0x1
	sltu	$3,$2,$3
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L53
	lui	$4,%hi($LC0)
	.set	macro
	.set	reorder

	lui	$5,%hi($LC1)
	lui	$7,%hi(__func__.54853)
	addiu	$4,$4,%lo($LC0)
	addiu	$5,$5,%lo($LC1)
	addiu	$7,$7,%lo(__func__.54853)
	li	$6,578			# 0x242
	sw	$2,16($sp)
	.set	noreorder
	.set	nomacro
	jal	printk
	li	$17,-22			# 0xffffffffffffffea
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L69
	move	$2,$17
	.set	macro
	.set	reorder

$L53:
	.set	noreorder
	.set	nomacro
	jal	rtl_inode_mutex_lock
	lw	$4,624($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_inode_sb
	lw	$4,624($sp)
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	rtl_vfs_check_frozen
	li	$5,1			# 0x1
	.set	macro
	.set	reorder

	lw	$3,76($16)
	lw	$2,0($28)
	move	$4,$18
	addiu	$5,$sp,40
	sw	$3,712($2)
	lw	$7,624($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_generic_write_checks
	addiu	$6,$sp,716
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L55
	move	$17,$2
	.set	macro
	.set	reorder

	lw	$2,716($sp)
	.set	noreorder
	.set	nomacro
	beql	$2,$0,$L71
	lw	$2,0($28)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	file_remove_suid
	move	$4,$18
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	file_update_time
	move	$4,$18
	.set	macro
	.set	reorder

	lw	$3,716($sp)
	addiu	$21,$sp,232
	addiu	$8,$sp,76
	move	$23,$21
	move	$22,$0
	li	$12,16384			# 0x4000
	li	$13,1			# 0x1
	lui	$11,%hi(mem_map)
	li	$14,-2147483648			# 0xffffffff80000000
$L59:
	lw	$7,44($sp)
	lw	$4,68($16)
	sw	$13,20($sp)
	andi	$10,$7,0x3fff
	subu	$2,$12,$10
	sltu	$fp,$2,$3
	movz	$2,$3,$fp
	sw	$2,16($sp)
	move	$fp,$2
	addiu	$2,$sp,32
	sw	$2,24($sp)
	addiu	$2,$sp,36
	sw	$2,28($sp)
	lw	$2,20($4)
	lw	$6,40($sp)
	sw	$3,640($sp)
	sw	$8,632($sp)
	sw	$10,636($sp)
	sw	$11,652($sp)
	sw	$12,644($sp)
	sw	$13,648($sp)
	sw	$14,656($sp)
	move	$4,$18
	.set	noreorder
	.set	nomacro
	jalr	$2
	move	$5,$16
	.set	macro
	.set	reorder

	lw	$3,640($sp)
	move	$9,$2
	lw	$8,632($sp)
	subu	$3,$3,$fp
	lw	$10,636($sp)
	lw	$11,652($sp)
	lw	$12,644($sp)
	lw	$13,648($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L56
	lw	$14,656($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L57
	move	$17,$0
	.set	macro
	.set	reorder

$L58:
	lw	$2,0($21)
	lw	$7,-16($21)
	lw	$6,4($21)
	lw	$3,68($16)
	sw	$2,20($sp)
	sw	$7,24($sp)
	sw	$6,28($sp)
	sw	$2,16($sp)
	lw	$7,-4($21)
	lw	$6,-8($21)
	lw	$2,24($3)
	sw	$9,632($sp)
	.set	noreorder
	.set	nomacro
	jalr	$2
	addiu	$17,$17,1
	.set	macro
	.set	reorder

	lw	$9,632($sp)
	addiu	$21,$21,24
$L57:
	slt	$2,$17,$22
	move	$4,$18
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L58
	move	$5,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L55
	move	$17,$9
	.set	macro
	.set	reorder

$L56:
	lw	$5,32($sp)
	lw	$6,%lo(mem_map)($11)
	addiu	$22,$22,1
	sw	$5,-16($23)
	lw	$4,40($sp)
	lw	$2,44($sp)
	subu	$5,$5,$6
	sra	$5,$5,5
	sll	$5,$5,14
	sw	$4,-8($23)
	sw	$2,-4($23)
	sw	$fp,0($23)
	addu	$5,$5,$14
	addu	$2,$fp,$2
	addu	$10,$5,$10
	lw	$5,36($sp)
	sltu	$6,$2,$fp
	addu	$4,$6,$4
	sw	$5,4($23)
	sw	$10,0($8)
	sw	$fp,4($8)
	addiu	$23,$23,24
	sw	$4,40($sp)
	sw	$2,44($sp)
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L59
	addiu	$8,$8,8
	.set	macro
	.set	reorder

	addiu	$6,$sp,76
	move	$4,$19
	li	$2,65536			# 0x10000
	sw	$6,56($sp)
	sw	$6,632($sp)
	sw	$2,72($sp)
	sw	$0,48($sp)
	sw	$0,52($sp)
	sw	$22,60($sp)
	sw	$0,64($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_rcvtimeo
	sw	$0,68($sp)
	.set	macro
	.set	reorder

	move	$4,$19
	li	$5,300			# 0x12c
	.set	noreorder
	.set	nomacro
	jal	rtl_set_sk_rcvtimeo
	move	$fp,$2
	.set	macro
	.set	reorder

	lw	$2,716($sp)
	lw	$6,632($sp)
	move	$4,$19
	sw	$2,16($sp)
	li	$2,131072			# 0x20000
	move	$7,$22
	addiu	$5,$sp,48
	ori	$2,$2,0x100
	.set	noreorder
	.set	nomacro
	jal	kernel_recvmsg
	sw	$2,20($sp)
	.set	macro
	.set	reorder

	move	$23,$2
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_sk_rcvtimeo
	move	$5,$fp
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bgez	$23,$L68
	move	$19,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L70
	slt	$2,$19,$22
	.set	macro
	.set	reorder

$L62:
	lw	$2,0($21)
	lw	$7,-16($21)
	lw	$6,4($21)
	lw	$3,68($16)
	sw	$2,20($sp)
	sw	$7,24($sp)
	sw	$6,28($sp)
	sw	$2,16($sp)
	lw	$7,-4($21)
	lw	$6,-8($21)
	lw	$2,24($3)
	addiu	$19,$19,1
	.set	noreorder
	.set	nomacro
	jalr	$2
	addiu	$21,$21,24
	.set	macro
	.set	reorder

	slt	$2,$19,$22
$L70:
	move	$4,$18
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L62
	move	$5,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L55
	move	$17,$23
	.set	macro
	.set	reorder

$L68:
	lw	$3,44($sp)
	lw	$2,716($sp)
	lw	$4,40($sp)
	sra	$5,$23,31
	subu	$2,$3,$2
	sltu	$3,$3,$2
	subu	$4,$4,$3
	addu	$3,$2,$23
	addu	$4,$4,$5
	sltu	$2,$3,$2
	addu	$2,$2,$4
	sw	$23,716($sp)
	lui	$fp,%hi($LC1)
	lui	$23,%hi(__func__.54853)
	sw	$2,40($sp)
	sw	$3,44($sp)
	addiu	$fp,$fp,%lo($LC1)
	.set	noreorder
	.set	nomacro
	j	$L63
	addiu	$23,$23,%lo(__func__.54853)
	.set	macro
	.set	reorder

$L65:
	lw	$2,0($21)
	lw	$7,-16($21)
	lw	$6,4($21)
	lw	$3,68($16)
	sw	$2,20($sp)
	sw	$7,24($sp)
	sw	$6,28($sp)
	sw	$2,16($sp)
	lw	$7,-4($21)
	lw	$6,-8($21)
	lw	$2,24($3)
	.set	noreorder
	.set	nomacro
	jalr	$2
	addiu	$19,$19,1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bgezl	$2,$L63
	addiu	$21,$21,24
	.set	macro
	.set	reorder

	lui	$4,%hi($LC2)
	addiu	$4,$4,%lo($LC2)
	move	$5,$fp
	li	$6,676			# 0x2a4
	move	$7,$23
	.set	noreorder
	.set	nomacro
	jal	printk
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	addiu	$21,$21,24
$L63:
	slt	$2,$19,$22
	move	$4,$18
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L65
	move	$5,$16
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	balance_dirty_pages_ratelimited_nr
	move	$5,$22
	.set	macro
	.set	reorder

	lw	$2,24($28)
	lw	$3,628($sp)
	and	$2,$3,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L55
	move	$4,$20
	.set	macro
	.set	reorder

	addiu	$5,$sp,40
	li	$6,8			# 0x8
 #APP
 # 680 "fs/reverse_sendfile_fastpath/reverse_sendfile.c" 1
	jal	__copy_user
	
 # 0 "" 2
 #NO_APP
$L55:
	lw	$2,0($28)
$L71:
	lw	$4,624($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_inode_mutex_unlock
	sw	$0,712($2)
	.set	macro
	.set	reorder

	lw	$2,716($sp)
	.set	noreorder
	.set	nomacro
	j	$L54
	movz	$17,$2,$17
	.set	macro
	.set	reorder

$L52:
	li	$17,-14			# 0xfffffffffffffff2
$L54:
	move	$2,$17
$L69:
	lw	$31,700($sp)
	lw	$fp,696($sp)
	lw	$23,692($sp)
	lw	$22,688($sp)
	lw	$21,684($sp)
	lw	$20,680($sp)
	lw	$19,676($sp)
	lw	$18,672($sp)
	lw	$17,668($sp)
	lw	$16,664($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,704
	.set	macro
	.set	reorder

	.end	do_splice_from_socket
	.size	do_splice_from_socket, .-do_splice_from_socket
	.section	.rodata.str1.4
	.align	2
$LC3:
	.ascii	"ufsd\000"
	.align	2
$LC4:
	.ascii	"fuseblk\000"
	.align	2
$LC5:
	.ascii	"[%s][%d]: Trap %s filesystem\012\000"
	.section	.text.rtl_splice,"ax",@progbits
	.align	2
	.globl	rtl_splice
	.set	nomips16
	.ent	rtl_splice
	.type	rtl_splice, @function
rtl_splice:
	.frame	$sp,56,$31		# vars= 16, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
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
	.set	noreorder
	.set	nomacro
	jal	sockfd_lookup
	move	$19,$7
	.set	macro
	.set	reorder

	move	$17,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L73
	lw	$6,24($sp)
	.set	macro
	.set	reorder

	lw	$2,20($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L74
	move	$4,$6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	fget_light
	addiu	$5,$sp,16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L74
	move	$16,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_file_mode
	move	$4,$2
	.set	macro
	.set	reorder

	andi	$2,$2,0x2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L82
	lw	$2,16($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_file_mapping
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_inode_sb
	lw	$4,0($2)
	.set	macro
	.set	reorder

	lw	$2,32($2)
	lui	$3,%hi($LC3)
	addiu	$3,$3,%lo($LC3)
	lw	$7,0($2)
	move	$2,$7
 #APP
 # 77 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$4,($2)
1:	lbu	$1,($3)
	addiu	$2,1
	bne	$1,$4,2f
	addiu	$3,1
	bnez	$4,1b
	lbu	$4,($2)
	move	$4,$1
2:	subu	$4,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L76
	lui	$3,%hi($LC4)
	.set	macro
	.set	reorder

	addiu	$3,$3,%lo($LC4)
	move	$2,$7
 #APP
 # 77 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$4,($2)
1:	lbu	$1,($3)
	addiu	$2,1
	bne	$1,$4,2f
	addiu	$3,1
	bnez	$4,1b
	lbu	$4,($2)
	move	$4,$1
2:	subu	$4,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L77
	move	$6,$18
	.set	macro
	.set	reorder

$L76:
	lui	$4,%hi($LC5)
	lui	$5,%hi(__func__.54991)
	addiu	$4,$4,%lo($LC5)
	addiu	$5,$5,%lo(__func__.54991)
	.set	noreorder
	.set	nomacro
	jal	printk
	li	$6,724			# 0x2d4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L82
	lw	$2,16($sp)
	.set	macro
	.set	reorder

$L77:
	move	$7,$19
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	do_splice_from_socket
	move	$5,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L75
	sw	$2,20($sp)
	.set	macro
	.set	reorder

$L81:
	.set	noreorder
	.set	nomacro
	jal	fput
	move	$4,$16
	.set	macro
	.set	reorder

$L74:
	.set	noreorder
	.set	nomacro
	jal	fput
	lw	$4,16($17)
	.set	macro
	.set	reorder

$L73:
	lw	$2,20($sp)
	lw	$31,52($sp)
	lw	$19,48($sp)
	lw	$18,44($sp)
	lw	$17,40($sp)
	lw	$16,36($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,56
	.set	macro
	.set	reorder

$L75:
	lw	$2,16($sp)
$L82:
	beq	$2,$0,$L74
	j	$L81
	.end	rtl_splice
	.size	rtl_splice, .-rtl_splice
	.section	.rodata.str1.4
	.align	2
$LC6:
	.ascii	"%s: err=-EPIPE!!!\015\012\000"
	.align	2
$LC7:
	.ascii	"*** psize=%d poffset=%d mss_now=%d size_goal=%d ***\012\000"
	.align	2
$LC8:
	.ascii	"%s: goto wait_for_memory\015\012\000"
	.align	2
$LC9:
	.ascii	"%s: skb->len=%d, page_count=%d\015\012\000"
	.align	2
$LC10:
	.ascii	"%s: sk_stream_wait_memory(), err=%d\015\012\000"
	.align	2
$LC11:
	.ascii	"%s: wait_for_memory(), mss_now=%d, size_goal=%d\015\012\000"
	.section	.text.tcp_sendpages,"ax",@progbits
	.align	2
	.globl	tcp_sendpages
	.set	nomips16
	.ent	tcp_sendpages
	.type	tcp_sendpages, @function
tcp_sendpages:
	.frame	$sp,176,$31		# vars= 112, regs= 10/0, args= 24, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-176
	sw	$31,172($sp)
	sw	$fp,168($sp)
	sw	$23,164($sp)
	sw	$22,160($sp)
	sw	$21,156($sp)
	sw	$20,152($sp)
	sw	$19,148($sp)
	sw	$18,144($sp)
	sw	$17,140($sp)
	sw	$16,136($sp)
	addiu	$22,$sp,32
	move	$20,$5
	move	$16,$4
	move	$5,$0
	move	$4,$22
	move	$19,$6
	li	$6,64			# 0x40
	move	$17,$7
	.set	noreorder
	.set	nomacro
	jal	memset
	lw	$18,192($sp)
	.set	macro
	.set	reorder

	lw	$21,40($20)
	li	$3,24			# 0x18
	lw	$2,84($20)
	mul	$5,$21,$3
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_route_caps
	addu	$21,$5,$2
	.set	macro
	.set	reorder

	andi	$3,$3,0x1
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L137
	move	$fp,$21
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_route_caps
	move	$4,$16
	.set	macro
	.set	reorder

	andi	$3,$3,0x1a
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L85
	move	$2,$0
	.set	macro
	.set	reorder

	move	$fp,$21
$L137:
	move	$23,$0
	li	$9,16384			# 0x4000
	.set	noreorder
	.set	nomacro
	j	$L86
	li	$8,24			# 0x18
	.set	macro
	.set	reorder

$L91:
	addu	$23,$23,$22
$L86:
	addu	$3,$23,$19
	andi	$3,$3,0x3fff
	subu	$2,$17,$23
	subu	$3,$9,$3
	sltu	$5,$2,$3
	movn	$3,$2,$5
	lw	$2,0($fp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L87
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bnel	$23,$17,$L88
	sw	$8,124($sp)
	.set	macro
	.set	reorder

$L87:
	.set	noreorder
	.set	nomacro
	j	$L89
	move	$22,$23
	.set	macro
	.set	reorder

$L88:
	sw	$9,120($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_socket
	sw	$3,128($sp)
	.set	macro
	.set	reorder

	lw	$3,128($sp)
	lw	$5,0($fp)
	move	$4,$2
	move	$7,$3
	move	$6,$19
	.set	noreorder
	.set	nomacro
	jal	sock_no_sendpage
	sw	$18,16($sp)
	.set	macro
	.set	reorder

	move	$22,$2
	lw	$3,128($sp)
	lw	$8,124($sp)
	.set	noreorder
	.set	nomacro
	bgez	$2,$L90
	lw	$9,120($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L89
	movn	$22,$23,$23
	.set	macro
	.set	reorder

$L90:
	.set	noreorder
	.set	nomacro
	bnel	$2,$3,$L86
	addu	$23,$23,$22
	.set	macro
	.set	reorder

	lw	$3,36($20)
	addiu	$2,$fp,24
	mul	$4,$3,$8
	addu	$3,$4,$21
	sltu	$3,$fp,$3
	.set	noreorder
	.set	nomacro
	j	$L91
	movn	$fp,$2,$3
	.set	macro
	.set	reorder

$L92:
	addiu	$2,$2,1
	addiu	$21,$21,24
	sw	$3,0($22)
	addiu	$22,$22,4
$L85:
	lw	$3,36($20)
	sltu	$3,$2,$3
	.set	noreorder
	.set	nomacro
	bnel	$3,$0,$L92
	lw	$3,0($21)
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	lock_sock_nested
	move	$5,$0
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_sk_sndtimeo
	move	$5,$18
	.set	macro
	.set	reorder

	lbu	$3,18($16)
	sw	$2,28($sp)
	li	$2,-259			# 0xfffffffffffffefd
	sra	$3,$2,$3
	andi	$3,$3,0x1
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L93
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	sk_stream_wait_connect
	addiu	$5,$sp,28
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L94
	move	$20,$2
	.set	macro
	.set	reorder

$L93:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_socket
	move	$4,$16
	.set	macro
	.set	reorder

	addiu	$2,$2,8
$L95:
 #APP
 # 145 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/bitops.h" 1
		ll	$3, 0($2)		# clear_bit	
	ins	$3, $0, 0, 1			
	sc	$3, 0($2)				

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L95
	addiu	$5,$sp,24
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	tcp_send_mss
	move	$6,$18
	.set	macro
	.set	reorder

	move	$23,$2
	lui	$2,%hi(g2_tcp_ctl+8)
	lw	$3,%lo(g2_tcp_ctl+8)($2)
	sltu	$4,$3,2
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L97
	sll	$2,$3,14
	.set	macro
	.set	reorder

	li	$2,196608			# 0x30000
	ori	$2,$2,0xffff
	addu	$2,$3,$2
	sll	$2,$2,14
$L97:
	lw	$3,24($sp)
	slt	$3,$2,$3
	.set	noreorder
	.set	nomacro
	bnel	$3,$0,$L98
	sw	$2,24($sp)
	.set	macro
	.set	reorder

$L98:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_err
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L138
	lui	$4,%hi($LC6)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_shutdown
	move	$4,$16
	.set	macro
	.set	reorder

	andi	$2,$2,0x2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L100
	lui	$2,%hi(g2_tcp_ctl+4)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC6)
$L138:
	lui	$5,%hi(__func__.54462)
	addiu	$4,$4,%lo($LC6)
	.set	noreorder
	.set	nomacro
	jal	printk
	addiu	$5,$5,%lo(__func__.54462)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L94
	li	$20,-32			# 0xffffffffffffffe0
	.set	macro
	.set	reorder

$L100:
	lw	$3,%lo(g2_tcp_ctl+4)($2)
	li	$2,3			# 0x3
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L139
	lui	$2,%hi(g2_tcp_ctl+4)
	.set	macro
	.set	reorder

	lw	$20,24($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_tp_snd_wnd
	move	$4,$16
	.set	macro
	.set	reorder

	sltu	$2,$20,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L139
	lui	$2,%hi(g2_tcp_ctl+4)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_tp_snd_wnd
	move	$4,$16
	.set	macro
	.set	reorder

	sw	$2,24($sp)
	lui	$2,%hi(g2_tcp_ctl+4)
$L139:
	lw	$3,%lo(g2_tcp_ctl+4)($2)
	li	$2,2			# 0x2
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L140
	li	$2,-65536			# 0xffffffffffff0000
	.set	macro
	.set	reorder

	lw	$2,24($sp)
	lui	$4,%hi($LC7)
	addiu	$4,$4,%lo($LC7)
	move	$5,$17
	move	$6,$19
	move	$7,$23
	.set	noreorder
	.set	nomacro
	jal	printk
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	li	$2,-65536			# 0xffffffffffff0000
$L140:
	ori	$2,$2,0x7fff
	and	$2,$18,$2
	andi	$5,$18,0x1
	sw	$2,112($sp)
	move	$22,$0
	move	$fp,$0
	sw	$0,96($sp)
	.set	noreorder
	.set	nomacro
	j	$L133
	sw	$5,116($sp)
	.set	macro
	.set	reorder

$L125:
	.set	noreorder
	.set	nomacro
	jal	rtl_tcp_write_queue_tail
	move	$4,$16
	.set	macro
	.set	reorder

	move	$20,$2
	srl	$2,$19,14
	sll	$2,$2,2
	addiu	$3,$sp,32
	addu	$2,$2,$3
	lw	$2,0($2)
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_get_tcp_send_head
	sw	$2,104($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L104
	lw	$21,24($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len_common
	move	$4,$20
	.set	macro
	.set	reorder

	subu	$2,$21,$2
	bgtz	$2,$L105
$L104:
	.set	noreorder
	.set	nomacro
	jal	rtl_sk_stream_memory_free
	move	$4,$16
	.set	macro
	.set	reorder

	beq	$2,$0,$L106
	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_allocation
	move	$4,$16
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	sk_stream_alloc_skb
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L107
	move	$20,$2
	.set	macro
	.set	reorder

	lui	$4,%hi($LC8)
	lui	$5,%hi(__func__.54462)
	addiu	$4,$4,%lo($LC8)
	.set	noreorder
	.set	nomacro
	jal	printk
	addiu	$5,$5,%lo(__func__.54462)
	.set	macro
	.set	reorder

	j	$L108
$L107:
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_entail
	move	$5,$2
	.set	macro
	.set	reorder

	li	$4,1			# 0x1
	sw	$4,96($sp)
$L105:
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_shared_info
	move	$4,$20
	.set	macro
	.set	reorder

	andi	$5,$19,0x3fff
	lbu	$2,0($2)
	sw	$5,108($sp)
	lw	$6,104($sp)
	lw	$7,108($sp)
	move	$4,$20
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_can_coalesce
	sw	$2,100($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L109
	move	$3,$2
	.set	macro
	.set	reorder

	lw	$4,100($sp)
	sltu	$2,$4,16
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L141
	lw	$2,108($sp)
	.set	macro
	.set	reorder

	move	$5,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_tcp_mark_push
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L104
$L109:
	lw	$2,108($sp)
$L141:
	li	$5,16384			# 0x4000
	move	$4,$16
	subu	$21,$5,$2
	sltu	$2,$17,$21
	movn	$21,$17,$2
	move	$5,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_sk_wmem_schedule
	sw	$3,128($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L108
	lw	$3,128($sp)
	.set	macro
	.set	reorder

	beq	$3,$0,$L110
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_shared_info
	move	$4,$20
	.set	macro
	.set	reorder

	lw	$4,100($sp)
	addiu	$3,$4,4
	sll	$3,$3,3
	addu	$2,$2,$3
	lhu	$3,10($2)
	addu	$3,$21,$3
	.set	noreorder
	.set	nomacro
	j	$L111
	sh	$3,10($2)
	.set	macro
	.set	reorder

$L110:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_page
	lw	$4,104($sp)
	.set	macro
	.set	reorder

	lw	$5,100($sp)
	lw	$6,104($sp)
	lw	$7,108($sp)
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_fill_page_desc
	sw	$21,16($sp)
	.set	macro
	.set	reorder

$L111:
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_add_skb_len_common
	move	$5,$21
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_add_skb_data_len_common
	move	$5,$21
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_add_skb_truesize_common
	move	$5,$21
	.set	macro
	.set	reorder

	move	$5,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_add_skb_sk_wmem_queued
	move	$4,$16
	.set	macro
	.set	reorder

	lw	$2,108($16)
	move	$4,$20
	subu	$2,$2,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_set_tcp_skb_ip_summed
	sw	$2,108($16)
	.set	macro
	.set	reorder

	move	$5,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_add_tp_write_seq
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_TCP_SKB_CB
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	rtl_add_skb_cb_end_seq
	move	$5,$21
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_shared_info
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$22,$0,$L112
	sh	$0,4($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_TCP_SKB_CB
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	rtl_and_skb_cb_tcp_flags
	li	$5,-9			# 0xfffffffffffffff7
	.set	macro
	.set	reorder

$L112:
	subu	$17,$17,$21
	addu	$22,$22,$21
	addu	$19,$19,$21
	.set	noreorder
	.set	nomacro
	bne	$17,$0,$L113
	addiu	$fp,$fp,1
	.set	macro
	.set	reorder

	lui	$2,%hi(g2_tcp_ctl+4)
	lw	$3,%lo(g2_tcp_ctl+4)($2)
	li	$2,2			# 0x2
	bne	$3,$2,$L114
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len_common
	move	$4,$20
	.set	macro
	.set	reorder

	lui	$4,%hi($LC9)
	lui	$5,%hi(__func__.54462)
	addiu	$4,$4,%lo($LC9)
	addiu	$5,$5,%lo(__func__.54462)
	move	$6,$2
	.set	noreorder
	.set	nomacro
	jal	printk
	move	$7,$fp
	.set	macro
	.set	reorder

	j	$L114
$L113:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len_common
	move	$4,$20
	.set	macro
	.set	reorder

	lw	$3,24($sp)
	sltu	$2,$2,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L133
	lw	$5,116($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$5,$0,$L133
	lw	$2,96($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L115
	lui	$3,%hi(g2_tcp_ctl)
	.set	macro
	.set	reorder

	addiu	$3,$3,%lo(g2_tcp_ctl)
	lw	$2,8($3)
	sltu	$2,$fp,$2
	bne	$2,$0,$L133
$L115:
	.set	noreorder
	.set	nomacro
	jal	forced_push
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L116
	move	$5,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_tcp_mark_push
	move	$4,$16
	.set	macro
	.set	reorder

	move	$4,$16
	move	$5,$23
	.set	noreorder
	.set	nomacro
	jal	__tcp_push_pending_frames
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L136
	move	$fp,$0
	.set	macro
	.set	reorder

$L116:
	.set	noreorder
	.set	nomacro
	jal	rtl_tcp_send_head
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bnel	$20,$2,$L142
	move	$fp,$0
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	tcp_push_one
	move	$5,$23
	.set	macro
	.set	reorder

	move	$fp,$0
$L136:
$L142:
	.set	noreorder
	.set	nomacro
	j	$L133
	sw	$0,96($sp)
	.set	macro
	.set	reorder

$L106:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_sk_socket
	move	$4,$16
	.set	macro
	.set	reorder

	addiu	$2,$2,8
	li	$4,-1			# 0xffffffffffffffff
$L143:
 #APP
 # 93 "/home/kai_wang/3493/branch-rtl819x-sdk-v3.4.9.3/linux-3.10/arch/mips/include/asm/bitops.h" 1
		ll	$3, 0($2)		# set_bit	
	ins	$3, $4, 2, 1			
	sc	$3, 0($2)				

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L143
	li	$4,-1			# 0xffffffffffffffff
	.set	macro
	.set	reorder

$L108:
	.set	noreorder
	.set	nomacro
	beq	$22,$0,$L144
	move	$4,$16
	.set	macro
	.set	reorder

	lw	$5,112($sp)
	move	$6,$23
	.set	noreorder
	.set	nomacro
	jal	rtl_tcp_push
	li	$7,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$16
$L144:
	.set	noreorder
	.set	nomacro
	jal	sk_stream_wait_memory
	addiu	$5,$sp,28
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L120
	move	$20,$2
	.set	macro
	.set	reorder

	lui	$5,%hi(g2_tcp_ctl)
	addiu	$5,$5,%lo(g2_tcp_ctl)
	lw	$3,4($5)
	li	$2,2			# 0x2
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L121
	lui	$4,%hi($LC10)
	.set	macro
	.set	reorder

	lui	$5,%hi(__func__.54462)
	addiu	$4,$4,%lo($LC10)
	addiu	$5,$5,%lo(__func__.54462)
	.set	noreorder
	.set	nomacro
	jal	printk
	move	$6,$20
	.set	macro
	.set	reorder

	j	$L121
$L120:
	move	$4,$16
	addiu	$5,$sp,24
	.set	noreorder
	.set	nomacro
	jal	tcp_send_mss
	move	$6,$18
	.set	macro
	.set	reorder

	move	$23,$2
	lui	$2,%hi(g2_tcp_ctl)
	addiu	$2,$2,%lo(g2_tcp_ctl)
	lw	$3,8($2)
	sltu	$4,$3,2
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L123
	sll	$2,$3,14
	.set	macro
	.set	reorder

	li	$4,196608			# 0x30000
	ori	$4,$4,0xffff
	addu	$2,$3,$4
	sll	$2,$2,14
$L123:
	lw	$3,24($sp)
	slt	$3,$2,$3
	.set	noreorder
	.set	nomacro
	bnel	$3,$0,$L124
	sw	$2,24($sp)
	.set	macro
	.set	reorder

$L124:
	jal	_cond_resched
	lui	$5,%hi(g2_tcp_ctl)
	addiu	$5,$5,%lo(g2_tcp_ctl)
	lw	$2,4($5)
	li	$3,2			# 0x2
	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L133
	lw	$7,24($sp)
	.set	macro
	.set	reorder

	lui	$4,%hi($LC11)
	lui	$5,%hi(__func__.54462)
	addiu	$4,$4,%lo($LC11)
	addiu	$5,$5,%lo(__func__.54462)
	.set	noreorder
	.set	nomacro
	jal	printk
	move	$6,$23
	.set	macro
	.set	reorder

$L133:
	bne	$17,$0,$L125
$L114:
	beq	$22,$0,$L126
	lw	$7,864($16)
$L145:
	move	$5,$18
	move	$6,$23
	ext	$7,$7,4,4
	.set	noreorder
	.set	nomacro
	jal	rtl_tcp_push
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L126
$L121:
	.set	noreorder
	.set	nomacro
	bnel	$22,$0,$L145
	lw	$7,864($16)
	.set	macro
	.set	reorder

$L94:
	move	$5,$18
	move	$6,$20
	.set	noreorder
	.set	nomacro
	jal	sk_stream_error
	move	$4,$16
	.set	macro
	.set	reorder

	move	$22,$2
$L126:
	.set	noreorder
	.set	nomacro
	jal	release_sock
	move	$4,$16
	.set	macro
	.set	reorder

$L89:
	move	$2,$22
	lw	$31,172($sp)
	lw	$fp,168($sp)
	lw	$23,164($sp)
	lw	$22,160($sp)
	lw	$21,156($sp)
	lw	$20,152($sp)
	lw	$19,148($sp)
	lw	$18,144($sp)
	lw	$17,140($sp)
	lw	$16,136($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,176
	.set	macro
	.set	reorder

	.end	tcp_sendpages
	.size	tcp_sendpages, .-tcp_sendpages
	.section	.text.inet_sendpages,"ax",@progbits
	.align	2
	.globl	inet_sendpages
	.set	nomips16
	.ent	inet_sendpages
	.type	inet_sendpages, @function
inet_sendpages:
	.frame	$sp,48,$31		# vars= 16, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$17,40($sp)
	sw	$16,36($sp)
	lw	$16,20($4)
	lw	$17,64($sp)
	sw	$5,16($sp)
	move	$4,$16
	sw	$6,20($sp)
	jal	rtl_sock_rps_record_flow
	sw	$7,24($sp)

	jal	rtl_get_sk_inet_num
	move	$4,$16

	lw	$5,16($sp)
	lw	$6,20($sp)
	bne	$2,$0,$L147
	lw	$7,24($sp)

	jal	rtl_get_sk_no_autobind
	move	$4,$16

	lw	$5,16($sp)
	lw	$6,20($sp)
	bne	$2,$0,$L147
	lw	$7,24($sp)

	jal	inet_autobind
	move	$4,$16

	lw	$5,16($sp)
	lw	$6,20($sp)
	beq	$2,$0,$L147
	lw	$7,24($sp)

	lw	$31,44($sp)
	lw	$17,40($sp)
	lw	$16,36($sp)
	li	$2,-11			# 0xfffffffffffffff5
	j	$31
	addiu	$sp,$sp,48

$L147:
	move	$4,$16
	sw	$17,64($sp)
	lw	$31,44($sp)
	lw	$17,40($sp)
	lw	$16,36($sp)
	j	tcp_sendpages
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	inet_sendpages
	.size	inet_sendpages, .-inet_sendpages
	.section	.text.sock_sendpages,"ax",@progbits
	.align	2
	.globl	sock_sendpages
	.set	nomips16
	.ent	sock_sendpages
	.type	sock_sendpages, @function
sock_sendpages:
	.frame	$sp,48,$31		# vars= 16, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$18,40($sp)
	sw	$17,36($sp)
	sw	$16,32($sp)
	move	$16,$4
	lw	$18,68($sp)
	sw	$5,16($sp)
	sw	$6,20($sp)
	jal	rtl_get_file_private_data
	sw	$7,24($sp)

	move	$4,$16
	jal	rtl_get_file_flags
	move	$17,$2

	andi	$2,$2,0x80
	li	$3,64			# 0x40
	movz	$3,$0,$2
	ori	$2,$3,0x8000
	movn	$3,$2,$18
	move	$4,$17
	lw	$5,16($sp)
	lw	$6,20($sp)
	lw	$7,24($sp)
	lw	$31,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	sw	$3,64($sp)
	j	inet_sendpages
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	sock_sendpages
	.size	sock_sendpages, .-sock_sendpages
	.section	.rodata.str1.4
	.align	2
$LC12:
	.ascii	"\0013%s confirm err=%d\012\000"
	.section	.text.pipe_to_sendpages,"ax",@progbits
	.align	2
	.globl	pipe_to_sendpages
	.set	nomips16
	.ent	pipe_to_sendpages
	.type	pipe_to_sendpages, @function
pipe_to_sendpages:
	.frame	$sp,64,$31		# vars= 8, regs= 8/0, args= 24, gp= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-64
	sw	$31,60($sp)
	sw	$22,56($sp)
	sw	$21,52($sp)
	sw	$20,48($sp)
	sw	$19,44($sp)
	sw	$18,40($sp)
	sw	$17,36($sp)
	sw	$16,32($sp)
	lw	$3,20($6)
	lw	$2,16($6)
	move	$16,$6
	sw	$3,28($sp)
	sw	$2,24($sp)
	move	$21,$5
	move	$19,$4
	lw	$22,12($6)
	move	$18,$5
	j	$L156
	move	$20,$0

$L159:
	lw	$2,12($18)
	addiu	$20,$20,1
	lw	$2,12($2)
	jalr	$2
	addiu	$18,$18,24

	beq	$2,$0,$L156
	move	$17,$2

	lui	$4,%hi($LC12)
	lui	$5,%hi(__func__.54747)
	addiu	$4,$4,%lo($LC12)
	addiu	$5,$5,%lo(__func__.54747)
	jal	printk
	move	$6,$2

	j	$L163
	move	$2,$17

$L156:
	lw	$2,36($19)
	move	$5,$18
	sltu	$2,$20,$2
	bne	$2,$0,$L159
	move	$4,$19

	lw	$7,4($16)
	lw	$2,8($16)
	addiu	$3,$sp,24
	sw	$7,0($16)
	lw	$6,4($21)
	ext	$2,$2,2,1
	move	$4,$22
	move	$5,$19
	sw	$3,16($sp)
	jal	sock_sendpages
	sw	$2,20($sp)

	bltz	$2,$L158
	move	$17,$2

	lw	$3,20($16)
	lw	$8,16($16)
	lw	$5,28($16)
	lw	$4,0($16)
	lw	$6,4($16)
	addu	$2,$3,$2
	sra	$7,$17,31
	addu	$7,$8,$7
	sltu	$3,$2,$3
	addu	$3,$3,$7
	addu	$5,$5,$17
	subu	$4,$4,$17
	subu	$6,$6,$17
	sw	$6,4($16)
	sw	$5,28($16)
	sw	$4,0($16)
	sw	$3,16($16)
	sw	$2,20($16)
$L158:
	move	$2,$17
$L163:
	lw	$31,60($sp)
	lw	$22,56($sp)
	lw	$21,52($sp)
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	j	$31
	addiu	$sp,$sp,64

	.set	macro
	.set	reorder
	.end	pipe_to_sendpages
	.size	pipe_to_sendpages, .-pipe_to_sendpages
	.globl	rtl_use_sendfile
	.section	.data.rtl_use_sendfile,"aw",@progbits
	.align	2
	.type	rtl_use_sendfile, @object
	.size	rtl_use_sendfile, 4
rtl_use_sendfile:
	.word	16
	.section	.rodata.__func__.54991,"a",@progbits
	.align	2
	.type	__func__.54991, @object
	.size	__func__.54991, 11
__func__.54991:
	.ascii	"rtl_splice\000"
	.section	.rodata.__func__.54853,"a",@progbits
	.align	2
	.type	__func__.54853, @object
	.size	__func__.54853, 22
__func__.54853:
	.ascii	"do_splice_from_socket\000"
	.section	.rodata.__func__.54747,"a",@progbits
	.align	2
	.type	__func__.54747, @object
	.size	__func__.54747, 18
__func__.54747:
	.ascii	"pipe_to_sendpages\000"
	.section	.rodata.__func__.54462,"a",@progbits
	.align	2
	.type	__func__.54462, @object
	.size	__func__.54462, 19
__func__.54462:
	.ascii	"do_tcp_send_mpages\000"
	.section	.data..percpu,"aw",@progbits
	.align	2
	.type	bdp_ratelimits, @object
	.size	bdp_ratelimits, 4
bdp_ratelimits:
	.space	4
	.globl	g2_tcp_ctl
	.section	.bss.g2_tcp_ctl,"aw",@nobits
	.align	2
	.type	g2_tcp_ctl, @object
	.size	g2_tcp_ctl, 32
g2_tcp_ctl:
	.space	32
	.ident	"GCC: (Realtek MSDK-4.4.7 Build 1459) 4.4.7"
