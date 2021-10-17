	.file	1 "fastpath_core.c"
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
	.section	.text.rtl_init_bypass_packet,"ax",@progbits
	.align	2
	.globl	rtl_init_bypass_packet
	.set	nomips16
	.ent	rtl_init_bypass_packet
	.type	rtl_init_bypass_packet, @function
rtl_init_bypass_packet:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	sw	$0,64($4)
	j	$31
	sw	$0,68($4)

	.set	macro
	.set	reorder
	.end	rtl_init_bypass_packet
	.size	rtl_init_bypass_packet, .-rtl_init_bypass_packet
	.section	.text.rtl_check_bypass_packet,"ax",@progbits
	.align	2
	.globl	rtl_check_bypass_packet
	.set	nomips16
	.ent	rtl_check_bypass_packet
	.type	rtl_check_bypass_packet, @function
rtl_check_bypass_packet:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lw	$2,68($4)
	nop
	bne	$2,$0,$L8
	li	$2,1			# 0x1

	lw	$3,64($4)
	lui	$5,%hi(fastpath_packet_bypassed_num)
	addiu	$3,$3,1
	sw	$3,64($4)
	lw	$5,%lo(fastpath_packet_bypassed_num)($5)
	nop
	sltu	$3,$5,$3
	beq	$3,$0,$L9
	nop

$L8:
	sw	$2,68($4)
	move	$2,$0
$L9:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	rtl_check_bypass_packet
	.size	rtl_check_bypass_packet, .-rtl_check_bypass_packet
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
	beq	$2,$0,$L15
	nop

	lw	$6,8($4)
	lw	$5,4($4)
	lhu	$3,2($4)
	lbu	$2,1($4)
	xor	$5,$6,$5
	lui	$6,%hi(max_udp_frag_entry)
	xor	$3,$3,$5
	lw	$6,%lo(max_udp_frag_entry)($6)
	srl	$5,$5,16
	xor	$3,$3,$5
	lw	$5,40($4)
	xor	$2,$3,$2
	srl	$3,$3,8
	xor	$2,$2,$3
	addiu	$6,$6,-1
	sb	$0,0($4)
	lw	$3,44($4)
	beq	$5,$0,$L12
	and	$2,$2,$6

	j	$L13
	sw	$3,44($5)

$L12:
	li	$5,12			# 0xc
	mult	$2,$5
	lui	$6,%hi(udp_cache_table)
	lw	$6,%lo(udp_cache_table)($6)
	nop
	lw	$6,0($6)
	mflo	$5
	addu	$5,$6,$5
	sw	$3,4($5)
$L13:
	li	$3,12			# 0xc
	mult	$2,$3
	lw	$5,40($4)
	lw	$3,44($4)
	nop
	sw	$5,0($3)
	lui	$3,%hi(udp_cache_table)
	lw	$6,%lo(udp_cache_table)($3)
	lui	$3,%hi(udp_cache_list_free)
	addiu	$3,$3,%lo(udp_cache_list_free)
	lw	$6,0($6)
	addiu	$5,$4,48
	mflo	$2
	addu	$2,$6,$2
	lw	$6,8($2)
	nop
	addiu	$6,$6,-1
	sw	$6,8($2)
	sw	$0,48($4)
	lw	$2,4($3)
	nop
	sw	$2,52($4)
	lw	$2,4($3)
	nop
	sw	$4,0($2)
	lw	$2,8($3)
	sw	$5,4($3)
	addiu	$2,$2,1
	sw	$2,8($3)
$L15:
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
	
	lw	$6,8($4)
	lw	$5,4($4)
	lhu	$3,2($4)
	lbu	$2,1($4)
	xor	$5,$6,$5
	lui	$6,%hi(max_udp_frag_entry)
	xor	$3,$3,$5
	lw	$6,%lo(max_udp_frag_entry)($6)
	srl	$5,$5,16
	xor	$3,$3,$5
	lw	$5,36($4)
	xor	$2,$3,$2
	srl	$3,$3,8
	xor	$2,$2,$3
	addiu	$6,$6,-1
	sb	$0,0($4)
	lw	$3,40($4)
	beq	$5,$0,$L17
	and	$2,$2,$6

	j	$L18
	sw	$3,40($5)

$L17:
	li	$5,12			# 0xc
	mult	$2,$5
	lui	$6,%hi(negative_cache_table)
	lw	$6,%lo(negative_cache_table)($6)
	mflo	$5
	addu	$5,$6,$5
	sw	$3,4($5)
$L18:
	li	$3,12			# 0xc
	mult	$2,$3
	lw	$5,36($4)
	lw	$3,40($4)
	nop
	sw	$5,0($3)
	lui	$3,%hi(negative_cache_table)
	lw	$6,%lo(negative_cache_table)($3)
	lui	$3,%hi(negative_cache_list_free)
	addiu	$3,$3,%lo(negative_cache_list_free)
	addiu	$5,$4,44
	mflo	$2
	addu	$2,$6,$2
	lw	$6,8($2)
	nop
	addiu	$6,$6,-1
	sw	$6,8($2)
	sw	$0,44($4)
	lw	$2,4($3)
	nop
	sw	$2,48($4)
	lw	$2,4($3)
	nop
	sw	$4,0($2)
	lw	$2,8($3)
	sw	$5,4($3)
	addiu	$2,$2,1
	j	$31
	sw	$2,8($3)

	.set	macro
	.set	reorder
	.end	negative_cache_timeout
	.size	negative_cache_timeout, .-negative_cache_timeout
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
	li	$2,-4			# 0xfffffffc

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
	li	$2,-4			# 0xfffffffc

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
	j	$L37
	move	$2,$0

$L40:
	lw	$5,8($3)
	nop
	sltu	$7,$5,$6
	bne	$7,$0,$L38
	and	$8,$4,$5

	lw	$7,4($3)
	nop
	bne	$8,$7,$L38
	nop

	j	$L39
	addiu	$2,$3,16

$L38:
	move	$5,$6
$L39:
	lw	$3,44($3)
	move	$6,$5
$L37:
	bne	$3,$0,$L40
	nop

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
	lui	$8,%hi(path_table_list_max)
	xor	$2,$2,$7
	lw	$8,%lo(path_table_list_max)($8)
	xor	$2,$2,$3
	srl	$9,$2,12
	xor	$2,$9,$2
	addiu	$8,$8,-1
	and	$2,$2,$8
	li	$9,12			# 0xc
	mult	$2,$9
	lui	$9,%hi(table_path)
	lw	$9,%lo(table_path)($9)
	nop
	lw	$9,0($9)
	mflo	$2
	addu	$2,$9,$2
	lw	$9,0($2)
	j	$L43
	li	$10,255			# 0xff

$L47:
	lw	$2,12($9)
	nop
	bne	$2,$5,$L44
	nop

	lw	$2,8($9)
	nop
	bne	$2,$7,$L44
	nop

	lw	$2,16($9)
	nop
	bne	$2,$4,$L44
	nop

	lw	$2,20($9)
	nop
	bne	$2,$6,$L44
	nop

	lbu	$11,0($9)
	nop
	bne	$11,$10,$L44
	nop

	lbu	$10,2($9)
	li	$6,1			# 0x1
	bne	$10,$6,$L45
	sb	$0,0($9)

	lw	$6,24($9)
	lhu	$10,30($9)
	j	$L46
	nop

$L45:
	lw	$6,32($9)
	lhu	$10,38($9)
	j	$L46
	nop

$L44:
	lw	$9,72($9)
	nop
$L43:
	bne	$9,$0,$L47
	nop

	j	$31
	nop

$L46:
	beq	$6,$0,$L56
	nop

	lbu	$11,2($9)
	li	$9,1			# 0x1
	bne	$11,$9,$L49
	srl	$9,$4,16

	srl	$9,$2,8
	xor	$9,$9,$2
	xor	$3,$9,$3
	srl	$9,$7,4
	xor	$3,$3,$9
	xor	$3,$3,$6
	xor	$10,$3,$10
	j	$L55
	srl	$6,$6,16

$L49:
	xor	$9,$9,$4
	xor	$9,$9,$5
	xor	$3,$9,$3
	xor	$3,$3,$6
	srl	$6,$6,8
	xor	$6,$3,$6
	srl	$10,$10,4
$L55:
	xor	$10,$6,$10
	srl	$6,$10,12
	xor	$10,$6,$10
	and	$8,$10,$8
	li	$3,12			# 0xc
	mult	$8,$3
	lui	$3,%hi(table_path)
	lw	$3,%lo(table_path)($3)
	nop
	lw	$3,0($3)
	mflo	$8
	addu	$8,$3,$8
	lw	$3,0($8)
	j	$L51
	li	$6,255			# 0xff

$L53:
	lw	$8,24($3)
	nop
	bne	$8,$2,$L52
	nop

	lw	$8,28($3)
	nop
	bne	$8,$7,$L52
	nop

	lw	$8,32($3)
	nop
	bne	$8,$4,$L52
	nop

	lw	$8,36($3)
	nop
	bne	$8,$5,$L52
	nop

	lbu	$8,0($3)
	nop
	bne	$8,$6,$L52
	nop

	sb	$0,0($3)
$L52:
	lw	$3,72($3)
	nop
$L51:
	bne	$3,$0,$L53
	nop

$L56:
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
	
	beq	$4,$0,$L61
	lui	$5,%hi($LC0)

	lui	$6,%hi($LC1)
	lui	$7,%hi($LC2)
	addiu	$5,$5,%lo($LC0)
	addiu	$6,$6,%lo($LC1)
	addiu	$7,$7,%lo($LC2)
	j	sprintf
	nop

$L61:
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
	j	$L63
	move	$7,$0

$L64:
	lw	$2,80($2)
	addiu	$7,$7,1
$L63:
	bne	$2,$0,$L64
	move	$6,$0

	lui	$2,%hi(napt_list_inuse)
	lw	$2,%lo(napt_list_inuse)($2)
	j	$L65
	nop

$L66:
	lw	$2,44($2)
	addiu	$6,$6,1
$L65:
	bne	$2,$0,$L66
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
	
$L69 = .
$L70 = .
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$2,%hi(malloc_sizes+4)
	move	$17,$4
	lw	$4,%lo(malloc_sizes+4)($2)
	move	$19,$5
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	lui	$16,%hi(table_path)
	move	$18,$2
	beq	$2,$0,$L71
	sw	$2,%lo(table_path)($16)

$L72 = .
	li	$4,12			# 0xc
	mult	$17,$4
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
	sw	$17,%lo(path_table_list_max)($6)
	jal	__kmalloc
	mflo	$4

	sw	$2,0($18)
	lw	$2,%lo(table_path)($16)
	nop
	lw	$2,0($2)
	nop
	beq	$2,$0,$L71
	move	$2,$0

	j	$L73
	li	$3,12			# 0xc

$L74:
	lw	$5,%lo(table_path)($16)
	addiu	$2,$2,1
	lw	$5,0($5)
	mflo	$4
	addu	$5,$5,$4
	sw	$0,8($5)
	lw	$5,%lo(table_path)($16)
	nop
	lw	$5,0($5)
	nop
	addu	$5,$5,$4
	sw	$0,0($5)
	lw	$5,%lo(table_path)($16)
	nop
	lw	$5,0($5)
	nop
	addu	$4,$5,$4
	sw	$4,4($4)
$L73:
	slt	$4,$2,$17
	bne	$4,$0,$L74
	mult	$2,$3

	lui	$18,%hi(malloc_sizes)
	lui	$16,%hi(path_list_free)
	addiu	$18,$18,%lo(malloc_sizes)
	addiu	$16,$16,%lo(path_list_free)
	j	$L75
	move	$17,$0

$L76:
$L77 = .
	lw	$4,20($18)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	bne	$2,$0,$L78
	addiu	$3,$2,80

	j	$L79
	li	$2,-2			# 0xfffffffe

$L78:
	sw	$0,80($2)
	lw	$4,4($16)
	addiu	$17,$17,1
	sw	$4,84($2)
	lw	$4,4($16)
	nop
	sw	$2,0($4)
	lw	$2,8($16)
	sw	$3,4($16)
	addiu	$2,$2,1
	sw	$2,8($16)
$L75:
	slt	$2,$17,$19
	bne	$2,$0,$L76
	move	$2,$0

	j	$L79
	nop

$L71:
	li	$2,-1			# 0xffffffff
$L79:
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
	
$L82 = .
$L83 = .
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$2,%hi(malloc_sizes+4)
	move	$17,$4
	lw	$4,%lo(malloc_sizes+4)($2)
	move	$19,$5
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	lui	$16,%hi(table_napt)
	move	$18,$2
	beq	$2,$0,$L84
	sw	$2,%lo(table_napt)($16)

$L85 = .
	li	$4,12			# 0xc
	mult	$17,$4
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
	sw	$17,%lo(napt_table_list_max)($6)
	jal	__kmalloc
	mflo	$4

	sw	$2,0($18)
	lw	$2,%lo(table_napt)($16)
	nop
	lw	$2,0($2)
	nop
	beq	$2,$0,$L84
	move	$2,$0

	j	$L86
	li	$3,12			# 0xc

$L87:
	lw	$5,%lo(table_napt)($16)
	addiu	$2,$2,1
	lw	$5,0($5)
	mflo	$4
	addu	$5,$5,$4
	sw	$0,8($5)
	lw	$5,%lo(table_napt)($16)
	nop
	lw	$5,0($5)
	nop
	addu	$5,$5,$4
	sw	$0,0($5)
	lw	$5,%lo(table_napt)($16)
	nop
	lw	$5,0($5)
	nop
	addu	$4,$5,$4
	sw	$4,4($4)
$L86:
	slt	$4,$2,$17
	bne	$4,$0,$L87
	mult	$2,$3

	lui	$18,%hi(malloc_sizes)
	lui	$16,%hi(napt_list_free)
	addiu	$18,$18,%lo(malloc_sizes)
	addiu	$16,$16,%lo(napt_list_free)
	j	$L88
	move	$17,$0

$L89:
$L90 = .
	lw	$4,12($18)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	bne	$2,$0,$L91
	addiu	$3,$2,44

	j	$L92
	li	$2,-2			# 0xfffffffe

$L91:
	sw	$0,44($2)
	lw	$4,4($16)
	addiu	$17,$17,1
	sw	$4,48($2)
	lw	$4,4($16)
	nop
	sw	$2,0($4)
	lw	$2,8($16)
	sw	$3,4($16)
	addiu	$2,$2,1
	sw	$2,8($16)
$L88:
	slt	$2,$17,$19
	bne	$2,$0,$L89
	move	$2,$0

	j	$L92
	nop

$L84:
	li	$2,-1			# 0xffffffff
$L92:
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
	move	$16,$2
	.set	macro
	.set	reorder

	move	$17,$2
	lbu	$2,8($2)
	#nop
	sltu	$2,$2,2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L95
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	lbu	$21,9($17)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$21,$2,$L96
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$21,$2,$L97
	move	$4,$0
	.set	macro
	.set	reorder

$L96:
	lbu	$19,0($17)
	move	$4,$17
	move	$6,$20
	andi	$19,$19,0xf
	sll	$19,$19,2
	addu	$19,$17,$19
	.set	noreorder
	.set	nomacro
	jal	fast_path_pre_process_check
	move	$5,$19
	.set	macro
	.set	reorder

	move	$4,$2
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$4,$2,$L97
	lui	$3,%hi($LC4)
	.set	macro
	.set	reorder

	addiu	$2,$3,%lo($LC4)
#APP
 # 75 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$3,($16)
1:	lbu	$1,($2)
	addiu	$16,1
	bne	$1,$3,2f
	addiu	$2,1
	bnez	$3,1b
	lbu	$3,($16)
	nop
	move	$3,$1
2:	subu	$3,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
	beq	$3,$0,$L95
	lhu	$5,0($19)
	lhu	$7,2($19)
	lhu	$2,6($17)
	lw	$22,12($17)
	lw	$23,16($17)
	sw	$5,40($sp)
	.set	noreorder
	.set	nomacro
	bne	$21,$4,$L98
	sw	$7,36($sp)
	.set	macro
	.set	reorder

	andi	$3,$2,0x3fff
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L99
	li	$3,8192			# 0x2000
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L231
	andi	$3,$2,0x3fff
	.set	macro
	.set	reorder

	j	$L99
$L227:
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
	j	$L97
	move	$4,$0
	.set	macro
	.set	reorder

$L192:
	li	$4,393216			# 0x60000
	and	$3,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L97
	move	$4,$0
	.set	macro
	.set	reorder

$L98:
	andi	$3,$2,0x3fff
$L231:
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L100
	move	$18,$0
	.set	macro
	.set	reorder

	li	$3,8192			# 0x2000
	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L101
	li	$fp,1			# 0x1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L102
	sw	$0,32($sp)
	.set	macro
	.set	reorder

$L101:
	andi	$3,$2,0x2000
	bne	$3,$0,$L103
	andi	$2,$2,0x1fff
	beq	$2,$0,$L103
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	j	$L193
	sw	$2,32($sp)
	.set	macro
	.set	reorder

$L103:
	sw	$0,32($sp)
$L193:
	move	$fp,$0
$L102:
	lhu	$4,4($17)
	xor	$6,$23,$22
	lui	$5,%hi(max_udp_frag_entry)
	lbu	$3,9($17)
	lw	$7,%lo(max_udp_frag_entry)($5)
	xor	$2,$4,$6
	srl	$6,$6,16
	xor	$2,$2,$6
	xor	$5,$2,$3
	srl	$2,$2,8
	addiu	$6,$7,-1
	xor	$5,$5,$2
	and	$5,$5,$6
	li	$2,12			# 0xc
	mult	$5,$2
	lui	$6,%hi(negative_cache_table)
	lw	$6,%lo(negative_cache_table)($6)
	mflo	$2
	addu	$2,$6,$2
	lw	$2,0($2)
	.set	noreorder
	.set	nomacro
	j	$L104
	li	$7,3			# 0x3
	.set	macro
	.set	reorder

$L106:
	lhu	$6,2($2)
	#nop
	bne	$6,$4,$L105
	lw	$6,4($2)
	#nop
	bne	$6,$22,$L105
	lw	$6,8($2)
	#nop
	bne	$6,$23,$L105
	lbu	$6,1($2)
	#nop
	bne	$6,$3,$L105
	lbu	$6,0($2)
	#nop
	beq	$6,$7,$L95
$L105:
	lw	$2,36($2)
	#nop
$L104:
	bne	$2,$0,$L106
	.set	noreorder
	.set	nomacro
	j	$L225
	li	$2,12			# 0xc
	.set	macro
	.set	reorder

$L110:
	lhu	$2,2($18)
	#nop
	bne	$2,$4,$L108
	lw	$2,4($18)
	#nop
	bne	$2,$22,$L108
	lw	$2,8($18)
	#nop
	bne	$2,$23,$L108
	lbu	$2,1($18)
	#nop
	bne	$2,$3,$L108
	lbu	$2,0($18)
	#nop
	beq	$2,$7,$L109
$L108:
	lw	$18,40($18)
	#nop
$L187:
	bne	$18,$0,$L110
	j	$L226
$L109:
	lhu	$2,12($18)
	#nop
	sw	$2,40($sp)
	lhu	$3,14($18)
	sw	$0,24($sp)
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L113
	sw	$3,36($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L228
	li	$fp,1			# 0x1
	.set	macro
	.set	reorder

$L188:
	lui	$6,%hi(negative_cache_list_free)
	lw	$2,%lo(negative_cache_list_free)($6)
	#nop
	beq	$2,$0,$L95
	lw	$7,44($2)
	sb	$3,1($2)
	li	$3,3			# 0x3
	sb	$3,0($2)
	sh	$4,2($2)
	sw	$22,4($2)
	sw	$23,8($2)
	lw	$3,48($2)
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L114
	addiu	$6,$6,%lo(negative_cache_list_free)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L115
	sw	$3,48($7)
	.set	macro
	.set	reorder

$L114:
	sw	$3,4($6)
$L115:
	li	$3,12			# 0xc
	mult	$5,$3
	lw	$6,44($2)
	lw	$3,48($2)
	lui	$4,%hi(negative_cache_list_free)
	addiu	$4,$4,%lo(negative_cache_list_free)
	sw	$6,0($3)
	lw	$6,8($4)
	lui	$3,%hi(negative_cache_table)
	addiu	$7,$2,36
	addiu	$6,$6,-1
	sw	$6,8($4)
	sw	$0,36($2)
	lw	$6,%lo(negative_cache_table)($3)
	addiu	$4,$2,12
	mflo	$5
	addu	$6,$6,$5
	lw	$6,4($6)
	#nop
	sw	$6,40($2)
	lw	$6,%lo(negative_cache_table)($3)
	#nop
	addu	$6,$6,$5
	lw	$6,4($6)
	#nop
	sw	$2,0($6)
	lw	$6,%lo(negative_cache_table)($3)
	#nop
	addu	$6,$6,$5
	sw	$7,4($6)
	lw	$3,%lo(negative_cache_table)($3)
	#nop
	addu	$5,$3,$5
	lw	$3,8($5)
	#nop
	addiu	$3,$3,1
	sw	$3,8($5)
	lui	$3,%hi(jiffies)
	lw	$3,%lo(jiffies)($3)
	#nop
	addiu	$3,$3,1000
	.set	noreorder
	.set	nomacro
	jal	add_timer
	sw	$3,20($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L97
	move	$4,$0
	.set	macro
	.set	reorder

$L100:
	li	$2,1			# 0x1
	sw	$0,32($sp)
	move	$fp,$0
	.set	noreorder
	.set	nomacro
	j	$L113
	sw	$2,24($sp)
	.set	macro
	.set	reorder

$L228:
	sw	$fp,24($sp)
$L113:
	xor	$3,$23,$22
	srl	$2,$22,8
	xor	$2,$3,$2
	lw	$4,36($sp)
	srl	$3,$23,16
	lw	$5,40($sp)
	xor	$2,$2,$3
	sw	$3,28($sp)
	xor	$2,$2,$21
	lui	$3,%hi(path_table_list_max)
	xor	$2,$2,$4
	lw	$3,%lo(path_table_list_max)($3)
	srl	$4,$5,4
	xor	$2,$2,$4
	srl	$4,$2,12
	xor	$2,$4,$2
	addiu	$3,$3,-1
	and	$2,$2,$3
	li	$3,12			# 0xc
	mult	$2,$3
	lui	$3,%hi(table_path)
	lw	$3,%lo(table_path)($3)
	li	$8,255			# 0xff
	li	$7,2			# 0x2
	lw	$3,0($3)
	li	$6,1			# 0x1
	li	$5,17			# 0x11
	mflo	$2
	addu	$2,$3,$2
	lw	$16,0($2)
	.set	noreorder
	.set	nomacro
	j	$L116
	li	$4,6			# 0x6
	.set	macro
	.set	reorder

$L184:
	lw	$2,12($16)
	lw	$3,40($sp)
	#nop
	bne	$2,$3,$L117
	lw	$2,8($16)
	lw	$3,36($sp)
	#nop
	bne	$2,$3,$L117
	lw	$2,16($16)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$22,$L117
	sw	$2,48($sp)
	.set	macro
	.set	reorder

	lw	$3,20($16)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$23,$L117
	sw	$3,44($sp)
	.set	macro
	.set	reorder

	lbu	$2,0($16)
	#nop
	bne	$2,$8,$L117
	lw	$2,4($16)
	#nop
	bne	$2,$7,$L118
	.set	noreorder
	.set	nomacro
	bne	$21,$4,$L117
	lui	$2,%hi(rtl_check_bypass_packet_hook)
	.set	macro
	.set	reorder

	j	$L229
$L118:
	bne	$2,$6,$L117
	.set	noreorder
	.set	nomacro
	bne	$21,$5,$L117
	lui	$2,%hi(rtl_check_bypass_packet_hook)
	.set	macro
	.set	reorder

$L229:
	lw	$2,%lo(rtl_check_bypass_packet_hook)($2)
	#nop
	beq	$2,$0,$L120
	.set	noreorder
	.set	nomacro
	jalr	$2
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L97
	move	$4,$0
	.set	macro
	.set	reorder

$L120:
	lw	$5,32($16)
	lbu	$7,1($17)
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_route_input
	move	$6,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L97
	move	$4,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_dst_check
	move	$4,$20
	.set	macro
	.set	reorder

	li	$3,-1			# 0xffffffff
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L126
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$21,$2,$L123
	addiu	$3,$19,16
	.set	macro
	.set	reorder

	lhu	$3,6($19)
	addiu	$2,$19,6
	movz	$2,$0,$3
	move	$3,$2
$L123:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L125
	lui	$7,%hi(udp_cache_list_free)
	.set	macro
	.set	reorder

	lw	$2,%lo(udp_cache_list_free)($7)
	lui	$4,%hi(max_udp_frag_entry)
	lw	$9,%lo(max_udp_frag_entry)($4)
	lhu	$4,4($17)
	beq	$2,$0,$L126
	lw	$5,40($sp)
	lw	$8,48($2)
	sh	$5,12($2)
	lw	$6,36($sp)
	li	$5,1			# 0x1
	sb	$5,0($2)
	sh	$6,14($2)
	sh	$4,2($2)
	sw	$22,4($2)
	lw	$5,44($sp)
	lw	$6,52($2)
	sb	$21,1($2)
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L127
	sw	$5,8($2)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L128
	sw	$6,52($8)
	.set	macro
	.set	reorder

$L127:
	addiu	$7,$7,%lo(udp_cache_list_free)
	sw	$6,4($7)
$L128:
	lw	$7,44($sp)
	addiu	$8,$2,40
	xor	$6,$7,$22
	xor	$4,$4,$6
	srl	$6,$6,16
	xor	$4,$4,$6
	srl	$6,$4,8
	xor	$5,$4,$21
	xor	$5,$5,$6
	addiu	$6,$9,-1
	and	$5,$5,$6
	li	$6,12			# 0xc
	mult	$5,$6
	lw	$7,48($2)
	lw	$6,52($2)
	lui	$4,%hi(udp_cache_list_free)
	addiu	$4,$4,%lo(udp_cache_list_free)
	sw	$7,0($6)
	lw	$7,8($4)
	lui	$6,%hi(udp_cache_table)
	addiu	$7,$7,-1
	sw	$7,8($4)
	sw	$0,40($2)
	lw	$7,%lo(udp_cache_table)($6)
	addiu	$4,$2,16
	lw	$7,0($7)
	mflo	$5
	addu	$7,$7,$5
	lw	$7,4($7)
	#nop
	sw	$7,44($2)
	lw	$7,%lo(udp_cache_table)($6)
	#nop
	lw	$7,0($7)
	#nop
	addu	$7,$7,$5
	lw	$7,4($7)
	#nop
	sw	$2,0($7)
	lw	$7,%lo(udp_cache_table)($6)
	#nop
	lw	$7,0($7)
	#nop
	addu	$7,$7,$5
	sw	$8,4($7)
	lw	$6,%lo(udp_cache_table)($6)
	#nop
	lw	$6,0($6)
	#nop
	addu	$5,$6,$5
	lw	$6,8($5)
	#nop
	addiu	$6,$6,1
	sw	$6,8($5)
	lui	$5,%hi(jiffies)
	lw	$5,%lo(jiffies)($5)
	#nop
	addiu	$5,$5,1000
	sw	$5,24($2)
	.set	noreorder
	.set	nomacro
	jal	add_timer
	sw	$3,52($sp)
	.set	macro
	.set	reorder

	lw	$3,52($sp)
	.set	noreorder
	.set	nomacro
	j	$L230
	move	$4,$20
	.set	macro
	.set	reorder

$L126:
	.set	noreorder
	.set	nomacro
	jal	rtl_dst_release
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L97
	move	$4,$0
	.set	macro
	.set	reorder

$L125:
	lw	$2,32($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L230
	move	$4,$20
	.set	macro
	.set	reorder

	addiu	$4,$18,16
	.set	noreorder
	.set	nomacro
	jal	del_timer
	sw	$3,52($sp)
	.set	macro
	.set	reorder

	lw	$6,8($18)
	lw	$5,4($18)
	lhu	$4,2($18)
	lbu	$2,1($18)
	xor	$5,$6,$5
	lui	$6,%hi(max_udp_frag_entry)
	xor	$4,$4,$5
	lw	$6,%lo(max_udp_frag_entry)($6)
	srl	$5,$5,16
	xor	$4,$4,$5
	lw	$5,40($18)
	xor	$2,$4,$2
	srl	$4,$4,8
	xor	$2,$2,$4
	addiu	$6,$6,-1
	sb	$0,0($18)
	lw	$4,44($18)
	lw	$3,52($sp)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L130
	and	$2,$2,$6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L131
	sw	$4,44($5)
	.set	macro
	.set	reorder

$L130:
	li	$5,12			# 0xc
	mult	$2,$5
	lui	$6,%hi(udp_cache_table)
	lw	$6,%lo(udp_cache_table)($6)
	#nop
	lw	$6,0($6)
	mflo	$5
	addu	$5,$6,$5
	sw	$4,4($5)
$L131:
	li	$4,12			# 0xc
	mult	$2,$4
	lw	$5,40($18)
	lw	$4,44($18)
	#nop
	sw	$5,0($4)
	lui	$4,%hi(udp_cache_table)
	lw	$6,%lo(udp_cache_table)($4)
	lui	$4,%hi(udp_cache_list_free)
	addiu	$4,$4,%lo(udp_cache_list_free)
	lw	$6,0($6)
	addiu	$5,$18,48
	mflo	$2
	addu	$2,$6,$2
	lw	$6,8($2)
	#nop
	addiu	$6,$6,-1
	sw	$6,8($2)
	sw	$0,48($18)
	lw	$2,4($4)
	#nop
	sw	$2,52($18)
	lw	$2,4($4)
	#nop
	sw	$18,0($2)
	lw	$2,8($4)
	sw	$5,4($4)
	addiu	$2,$2,1
	sw	$2,8($4)
	move	$4,$20
$L230:
	move	$5,$17
	move	$6,$21
	.set	noreorder
	.set	nomacro
	jal	fast_path_before_nat_check
	sw	$3,52($sp)
	.set	macro
	.set	reorder

	move	$4,$2
	li	$2,1			# 0x1
	lw	$3,52($sp)
	.set	noreorder
	.set	nomacro
	beq	$4,$2,$L97
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$21,$2,$L132
	li	$2,1048576			# 0x100000
	.set	macro
	.set	reorder

	lw	$4,12($19)
	#nop
	and	$2,$4,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L132
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$4,2($17)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$4,$2,$L132
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

	lw	$3,52($sp)
$L132:
	lbu	$2,1($16)
	#nop
	sltu	$4,$2,13
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L133
	lui	$4,%hi($L139)
	.set	macro
	.set	reorder

	addiu	$4,$4,%lo($L139)
	sll	$2,$2,2
	addu	$2,$4,$2
	lw	$2,0($2)
	lhu	$4,10($17)
	j	$2
	.rdata
	.align	2
	.align	2
$L139:
	.word	$L134
	.word	$L135
	.word	$L136
	.word	$L136
	.word	$L137
	.word	$L133
	.word	$L133
	.word	$L133
	.word	$L138
	.word	$L133
	.word	$L133
	.word	$L133
	.word	$L138
	.section	.text.enter_fast_path
$L135:
	lw	$2,24($16)
	srl	$5,$22,16
	andi	$22,$22,0xffff
	srl	$6,$2,16
	subu	$6,$0,$6
	andi	$2,$2,0xffff
	subu	$2,$6,$2
	addu	$2,$2,$5
	addu	$22,$2,$22
	addu	$4,$22,$4
	.set	noreorder
	.set	nomacro
	bgez	$4,$L140
	andi	$22,$4,0xffff
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
	j	$L194
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L140:
	sra	$4,$4,16
	addu	$4,$22,$4
	sra	$2,$4,16
	addu	$4,$2,$4
$L194:
	sh	$4,10($17)
	lw	$2,24($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L149
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L143
	beq	$3,$0,$L149
	lhu	$4,0($3)
	#nop
	beq	$4,$0,$L149
	lw	$6,24($16)
	lw	$5,12($17)
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
	bgez	$2,$L144
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
	j	$L196
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L144:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L196:
	.set	noreorder
	.set	nomacro
	j	$L149
	sh	$2,0($3)
	.set	macro
	.set	reorder

$L143:
	lw	$5,24($16)
	lw	$4,12($17)
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
	bgez	$2,$L145
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
	j	$L195
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L145:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L195:
	sh	$2,0($3)
	lw	$3,12($19)
	li	$2,1048576			# 0x100000
	and	$2,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L149
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$3,2($17)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L149
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

	j	$L149
$L136:
	lw	$2,24($16)
	lw	$5,48($sp)
	srl	$22,$2,16
	subu	$22,$0,$22
	andi	$2,$2,0xffff
	srl	$6,$5,16
	subu	$22,$22,$2
	andi	$5,$5,0xffff
	addu	$22,$22,$6
	addu	$22,$22,$5
	addu	$4,$22,$4
	.set	noreorder
	.set	nomacro
	bgez	$4,$L147
	andi	$22,$4,0xffff
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
	j	$L197
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L147:
	sra	$4,$4,16
	addu	$4,$22,$4
	sra	$2,$4,16
	addu	$4,$2,$4
$L197:
	sh	$4,10($17)
	lw	$6,24($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L149
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L150
	beq	$3,$0,$L151
	lhu	$4,0($3)
	#nop
	beq	$4,$0,$L151
	lw	$6,24($16)
	lw	$5,12($17)
	lhu	$8,0($19)
	srl	$2,$6,16
	subu	$2,$0,$2
	andi	$6,$6,0xffff
	srl	$7,$5,16
	subu	$2,$2,$6
	addu	$2,$2,$7
	andi	$5,$5,0xffff
	lw	$7,28($16)
	addu	$2,$2,$5
	addu	$2,$2,$8
	subu	$2,$2,$7
	addu	$2,$4,$2
	.set	noreorder
	.set	nomacro
	bgez	$2,$L152
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
	j	$L199
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L152:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L199:
	.set	noreorder
	.set	nomacro
	j	$L151
	sh	$2,0($3)
	.set	macro
	.set	reorder

$L150:
	lw	$5,24($16)
	lw	$4,12($17)
	lhu	$8,0($19)
	srl	$2,$5,16
	subu	$2,$0,$2
	andi	$5,$5,0xffff
	srl	$6,$4,16
	subu	$2,$2,$5
	addu	$2,$2,$6
	andi	$4,$4,0xffff
	lw	$7,28($16)
	addu	$2,$2,$4
	lhu	$6,0($3)
	addu	$2,$2,$8
	subu	$2,$2,$7
	addu	$2,$6,$2
	.set	noreorder
	.set	nomacro
	bgez	$2,$L153
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
	j	$L198
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L153:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L198:
	sh	$2,0($3)
	lw	$3,12($19)
	li	$2,1048576			# 0x100000
	and	$2,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L151
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$3,2($17)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L151
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

$L151:
	lw	$2,28($16)
	#nop
	sh	$2,0($19)
$L149:
	lw	$2,24($16)
	.set	noreorder
	.set	nomacro
	j	$L134
	sw	$2,12($17)
	.set	macro
	.set	reorder

$L137:
	lw	$5,32($16)
	lw	$7,28($sp)
	andi	$23,$23,0xffff
	andi	$22,$5,0xffff
	addu	$2,$23,$7
	subu	$22,$2,$22
	srl	$5,$5,16
	subu	$5,$22,$5
	addu	$4,$5,$4
	.set	noreorder
	.set	nomacro
	bgez	$4,$L155
	andi	$22,$4,0xffff
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
	j	$L200
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L155:
	sra	$4,$4,16
	addu	$4,$22,$4
	sra	$5,$4,16
	addu	$4,$5,$4
$L200:
	sh	$4,10($17)
	lw	$4,24($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L209
	li	$4,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$4,$L158
	beq	$3,$0,$L209
	lhu	$4,0($3)
	#nop
	beq	$4,$0,$L209
	lw	$5,32($16)
	#nop
	andi	$23,$5,0xffff
	subu	$23,$2,$23
	srl	$5,$5,16
	subu	$23,$23,$5
	.set	noreorder
	.set	nomacro
	j	$L215
	addu	$23,$4,$23
	.set	macro
	.set	reorder

$L158:
	lw	$4,32($16)
	lhu	$23,0($3)
	andi	$5,$4,0xffff
	subu	$2,$2,$5
	srl	$4,$4,16
	subu	$2,$2,$4
	addu	$23,$23,$2
$L215:
	.set	noreorder
	.set	nomacro
	bgez	$23,$L160
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
	j	$L201
	nor	$23,$0,$23
	.set	macro
	.set	reorder

$L160:
	sra	$23,$23,16
	addu	$23,$2,$23
	sra	$2,$23,16
	addu	$23,$2,$23
$L201:
	.set	noreorder
	.set	nomacro
	j	$L209
	sh	$23,0($3)
	.set	macro
	.set	reorder

$L138:
	lw	$2,32($16)
	lw	$5,28($sp)
	andi	$23,$23,0xffff
	andi	$22,$2,0xffff
	addu	$23,$23,$5
	subu	$22,$23,$22
	srl	$2,$2,16
	subu	$22,$22,$2
	addu	$22,$22,$4
	.set	noreorder
	.set	nomacro
	bgez	$22,$L161
	andi	$2,$22,0xffff
	.set	macro
	.set	reorder

	subu	$22,$0,$22
	andi	$2,$22,0xffff
	sra	$22,$22,16
	addu	$22,$2,$22
	sra	$2,$22,16
	addu	$22,$2,$22
	.set	noreorder
	.set	nomacro
	j	$L202
	nor	$22,$0,$22
	.set	macro
	.set	reorder

$L161:
	sra	$22,$22,16
	addu	$22,$2,$22
	sra	$2,$22,16
	addu	$22,$2,$22
$L202:
	sh	$22,10($17)
	lw	$6,24($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L209
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L164
	beq	$3,$0,$L165
	lhu	$4,0($3)
	#nop
	beq	$4,$0,$L165
	lw	$5,32($16)
	lhu	$7,2($19)
	lw	$6,36($16)
	andi	$2,$5,0xffff
	subu	$2,$23,$2
	srl	$5,$5,16
	subu	$2,$2,$5
	addu	$2,$2,$7
	subu	$2,$2,$6
	.set	noreorder
	.set	nomacro
	j	$L217
	addu	$2,$4,$2
	.set	macro
	.set	reorder

$L164:
	lw	$4,32($16)
	lhu	$7,2($19)
	lw	$6,36($16)
	andi	$2,$4,0xffff
	subu	$2,$23,$2
	srl	$4,$4,16
	subu	$2,$2,$4
	lhu	$5,0($3)
	addu	$2,$2,$7
	subu	$2,$2,$6
	addu	$2,$5,$2
$L217:
	.set	noreorder
	.set	nomacro
	bgez	$2,$L167
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
	j	$L203
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L167:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L203:
	sh	$2,0($3)
$L165:
	lw	$2,36($16)
	.set	noreorder
	.set	nomacro
	j	$L209
	sh	$2,2($19)
	.set	macro
	.set	reorder

$L133:
	lw	$4,24($16)
	srl	$2,$22,16
	andi	$22,$22,0xffff
	addu	$22,$22,$2
	lhu	$5,10($17)
	andi	$2,$4,0xffff
	subu	$2,$22,$2
	srl	$4,$4,16
	subu	$2,$2,$4
	addu	$2,$2,$5
	.set	noreorder
	.set	nomacro
	bgez	$2,$L168
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
	j	$L204
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L168:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L204:
	sh	$2,10($17)
	lw	$2,32($16)
	lw	$7,28($sp)
	andi	$23,$23,0xffff
	andi	$5,$2,0xffff
	addu	$23,$23,$7
	lhu	$4,10($17)
	subu	$5,$23,$5
	srl	$2,$2,16
	subu	$5,$5,$2
	addu	$5,$5,$4
	.set	noreorder
	.set	nomacro
	bgez	$5,$L170
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
	j	$L205
	nor	$5,$0,$5
	.set	macro
	.set	reorder

$L170:
	sra	$5,$5,16
	addu	$5,$2,$5
	sra	$2,$5,16
	addu	$5,$2,$5
$L205:
	sh	$5,10($17)
	lw	$2,24($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L172
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L173
	beq	$3,$0,$L174
	lhu	$2,0($3)
	#nop
	beq	$2,$0,$L174
	lw	$5,24($16)
	lhu	$7,0($19)
	lw	$6,28($16)
	andi	$4,$5,0xffff
	subu	$4,$22,$4
	srl	$5,$5,16
	subu	$4,$4,$5
	addu	$4,$4,$7
	subu	$4,$4,$6
	.set	noreorder
	.set	nomacro
	j	$L221
	addu	$4,$2,$4
	.set	macro
	.set	reorder

$L173:
	lw	$2,24($16)
	lhu	$7,0($19)
	lw	$6,28($16)
	andi	$4,$2,0xffff
	subu	$4,$22,$4
	srl	$2,$2,16
	subu	$4,$4,$2
	lhu	$5,0($3)
	addu	$4,$4,$7
	subu	$4,$4,$6
	addu	$4,$5,$4
$L221:
	.set	noreorder
	.set	nomacro
	bgez	$4,$L178
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
	j	$L207
	nor	$4,$0,$4
	.set	macro
	.set	reorder

$L178:
	sra	$4,$4,16
	addu	$4,$2,$4
	sra	$2,$4,16
	addu	$4,$2,$4
$L207:
	sh	$4,0($3)
	lw	$4,32($16)
	lhu	$7,2($19)
	lw	$6,36($16)
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
	bgez	$2,$L180
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
	j	$L208
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L180:
	sra	$2,$2,16
	addu	$2,$4,$2
	sra	$4,$2,16
	addu	$2,$4,$2
$L208:
	sh	$2,0($3)
$L174:
	lw	$2,28($16)
	#nop
	sh	$2,0($19)
	lw	$2,36($16)
	#nop
	sh	$2,2($19)
$L172:
	lw	$2,24($16)
	#nop
	sw	$2,12($17)
$L209:
	lw	$2,32($16)
	#nop
	sw	$2,16($17)
$L134:
	lbu	$5,8($17)
	lbu	$4,9($17)
	lhu	$7,10($17)
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
	bgez	$2,$L181
	sb	$3,8($17)
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
	j	$L210
	nor	$2,$0,$2
	.set	macro
	.set	reorder

$L181:
	andi	$3,$2,0xffff
	sra	$2,$2,16
	addu	$2,$3,$2
	sra	$3,$2,16
	addu	$2,$3,$2
$L210:
	sh	$2,10($17)
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

	lw	$5,48($16)
	lw	$6,52($16)
	.set	noreorder
	.set	nomacro
	jal	fastpath_set_qos_mark
	move	$4,$20
	.set	macro
	.set	reorder

	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	move	$4,$17
	move	$5,$19
	sw	$2,40($16)
	.set	noreorder
	.set	nomacro
	jal	fast_path_post_process_xmit_check
	move	$6,$20
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L97
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	ip_finish_output3
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L97
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

$L117:
	lw	$16,72($16)
	#nop
$L116:
	bne	$16,$0,$L184
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_inDev
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$17
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

$L95:
	move	$4,$0
$L97:
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
	move	$2,$4
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,96
	.set	macro
	.set	reorder

$L99:
	lw	$3,12($19)
	li	$4,65536			# 0x10000
	and	$4,$3,$4
	beq	$4,$0,$L192
	.set	noreorder
	.set	nomacro
	j	$L227
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

$L225:
	mult	$5,$2
	lui	$6,%hi(udp_cache_table)
	lw	$6,%lo(udp_cache_table)($6)
	#nop
	lw	$6,0($6)
	mflo	$2
	addu	$2,$6,$2
	lw	$18,0($2)
	.set	noreorder
	.set	nomacro
	j	$L187
	li	$7,1			# 0x1
	.set	macro
	.set	reorder

$L226:
	.set	noreorder
	.set	nomacro
	beq	$fp,$0,$L188
	li	$fp,1			# 0x1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L113
	sw	$fp,24($sp)
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
	beq	$2,$3,$L237
	li	$3,450			# 0x1c2

	beq	$2,$3,$L238
	li	$3,150			# 0x96

	bne	$2,$3,$L245
	move	$4,$16

	j	$L244
	lui	$2,%hi(jiffies)

$L238:
	lui	$2,%hi(jiffies)
	lw	$5,%lo(jiffies)($2)
	j	$L242
	move	$4,$16

$L237:
	lui	$2,%hi(jiffies)
	lw	$5,%lo(jiffies)($2)
	move	$4,$16
	j	$L242
	addiu	$5,$5,-15000

$L244:
	lw	$5,%lo(jiffies)($2)
	move	$4,$16
	addiu	$5,$5,-30000
$L242:
	jal	rtl_set_fdb_aging
	nop

	move	$4,$16
$L245:
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
	li	$3,-1			# 0xffffffff
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
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	lw	$7,4($4)
	lw	$2,20($4)
	lw	$6,24($4)
	lw	$8,8($4)
	xor	$12,$2,$7
	srl	$3,$7,8
	lw	$9,0($4)
	xor	$12,$12,$6
	xor	$12,$12,$3
	srl	$3,$2,16
	xor	$12,$12,$3
	srl	$10,$8,4
	xori	$13,$9,0x2
	li	$11,6			# 0x6
	xor	$12,$12,$10
	li	$3,17			# 0x11
	lui	$10,%hi(path_table_list_max)
	movz	$3,$11,$13
	lw	$11,%lo(path_table_list_max)($10)
	xor	$12,$12,$3
	srl	$10,$12,12
	xor	$12,$10,$12
	addiu	$11,$11,-1
	and	$12,$12,$11
	li	$10,12			# 0xc
	mult	$12,$10
	lui	$10,%hi(table_path)
	lw	$10,%lo(table_path)($10)
	li	$12,-16777216			# 0xff000000
	lui	$13,%hi(jiffies)
	lw	$10,0($10)
	ori	$15,$12,0x100
	lw	$13,%lo(jiffies)($13)
	mflo	$24
	addu	$14,$10,$24
	lw	$14,0($14)
	j	$L249
	ori	$12,$12,0xff00

$L253:
	lw	$24,4($14)
	nop
	bne	$24,$9,$L250
	nop

	lw	$24,0($14)
	nop
	and	$24,$24,$12
	bne	$24,$15,$L250
	nop

	lw	$24,16($14)
	nop
	bne	$24,$7,$L250
	nop

	lw	$24,12($14)
	nop
	bne	$24,$8,$L250
	nop

	lw	$25,24($14)
	lw	$24,12($4)
	nop
	bne	$25,$24,$L250
	nop

	lw	$25,28($14)
	lw	$24,16($4)
	nop
	bne	$25,$24,$L250
	nop

	lw	$24,20($14)
	nop
	bne	$24,$2,$L250
	nop

	lw	$24,8($14)
	nop
	bne	$24,$6,$L250
	nop

	lw	$12,40($14)
	subu	$15,$13,$5
	subu	$15,$15,$12
	bgez	$15,$L251
	nop

	lw	$4,56($14)
	j	$L259
	addu	$5,$5,$12

$L250:
	lw	$14,72($14)
	nop
$L249:
	bne	$14,$0,$L253
	nop

$L251:
	lw	$12,12($4)
	lw	$4,16($4)
	srl	$15,$2,8
	xor	$14,$12,$2
	xor	$14,$14,$4
	xor	$14,$14,$15
	srl	$15,$12,16
	xor	$14,$14,$15
	srl	$15,$6,4
	xor	$14,$14,$15
	xor	$3,$14,$3
	srl	$14,$3,12
	xor	$3,$14,$3
	and	$11,$3,$11
	li	$14,12			# 0xc
	mult	$11,$14
	li	$14,-16777216			# 0xff000000
	ori	$15,$14,0x200
	mflo	$11
	addu	$10,$10,$11
	lw	$3,0($10)
	j	$L254
	ori	$14,$14,0xff00

$L257:
	lw	$10,4($3)
	nop
	bne	$10,$9,$L255
	nop

	lw	$10,0($3)
	nop
	and	$10,$10,$14
	bne	$10,$15,$L255
	nop

	lw	$10,20($3)
	nop
	bne	$10,$12,$L255
	nop

	lw	$10,8($3)
	nop
	bne	$10,$4,$L255
	nop

	lw	$10,24($3)
	nop
	bne	$10,$2,$L255
	nop

	lw	$10,28($3)
	nop
	bne	$10,$6,$L255
	nop

	lw	$10,32($3)
	nop
	bne	$10,$7,$L255
	nop

	lw	$10,36($3)
	nop
	bne	$10,$8,$L255
	nop

	lw	$2,40($3)
	subu	$13,$13,$5
	subu	$13,$13,$2
	bgez	$13,$L256
	nop

	lw	$4,56($3)
	addu	$5,$5,$2
$L259:
	jal	rtl_set_ct_timeout_expires
	nop

	j	$L252
	li	$2,-1			# 0xffffffff

$L255:
	lw	$3,72($3)
	nop
$L254:
	bne	$3,$0,$L257
	nop

$L256:
	move	$2,$0
$L252:
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
	bne	$2,$3,$L261
	move	$4,$16

	jal	rtl_get_ct_udp_status
	move	$4,$16

	andi	$2,$2,0x2
	beq	$2,$0,$L262
	lui	$2,%hi(nf_ct_udp_timeout)

	lui	$2,%hi(nf_ct_udp_timeout_stream)
	lw	$17,%lo(nf_ct_udp_timeout_stream)($2)
	j	$L263
	li	$18,1			# 0x1

$L262:
	lw	$17,%lo(nf_ct_udp_timeout)($2)
	j	$L263
	li	$18,1			# 0x1

$L261:
	jal	rtl_get_ct_protonum
	move	$5,$0

	li	$3,6			# 0x6
	bne	$2,$3,$L267
	li	$2,-1			# 0xffffffff

	jal	rtl_get_ct_tcp_state
	move	$4,$16

	sltu	$2,$2,6
	beq	$2,$0,$L267
	li	$2,-1			# 0xffffffff

	jal	rtl_tcp_get_timeouts
	move	$4,$16

	move	$17,$2
	li	$18,2			# 0x2
$L263:
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$4,$16

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$20,$2

	bne	$20,$2,$L265
	li	$6,1			# 0x1

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
	j	$L270
	move	$5,$0

$L265:
	move	$5,$0
	jal	rtl_get_ct_ip_by_dir
	move	$4,$16

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$20,$2

	bne	$20,$2,$L267
	li	$2,-1			# 0xffffffff

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
$L270:
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,36($sp)

	addiu	$4,$sp,16
	move	$5,$17
	jal	rtk_idleNaptConnection
	sw	$2,40($sp)

	beq	$2,$0,$L264
	addu	$5,$17,$19

	jal	rtl_check_for_acc
	move	$4,$16

	j	$L267
	move	$2,$0

$L264:
	li	$2,-1			# 0xffffffff
$L267:
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
	.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$18,24($sp)
	sw	$17,20($sp)
	sw	$16,16($sp)
	lw	$3,0($4)
	li	$2,6			# 0x6
	li	$18,17			# 0x11
	xori	$3,$3,0x2
	move	$16,$4
	movz	$18,$2,$3
#APP
 # 1358 "net/rtl/fastpath/fastpath_core.c" 1
	raw_local_irq_save	$17
 # 0 "" 2
#NO_APP
	lw	$4,20($4)
	jal	filter_delconnect
	lw	$3,4($16)
	lw	$6,8($16)
	lw	$2,20($16)
	lw	$5,24($16)
	xor	$7,$6,$3
	xor	$7,$7,$2
	srl	$8,$3,16
	lw	$4,0($16)
	xor	$7,$7,$5
	xor	$7,$7,$8
	srl	$8,$2,16
	xor	$7,$7,$8
	lui	$8,%hi(napt_table_list_max)
	lw	$9,%lo(napt_table_list_max)($8)
	andi	$8,$4,0xffff
	xor	$7,$7,$8
	srl	$8,$7,12
	xor	$7,$8,$7
	addiu	$9,$9,-1
	and	$8,$7,$9
	li	$9,12			# 0xc
	mult	$8,$9
	lui	$7,%hi(table_napt)
	lw	$7,%lo(table_napt)($7)
	#nop
	lw	$7,0($7)
	mflo	$8
	addu	$7,$7,$8
	lw	$7,0($7)
	j	$L274
$L298:
	lw	$9,4($7)
	#nop
	bne	$9,$4,$L275
	lw	$9,8($7)
	#nop
	bne	$9,$3,$L275
	lw	$9,12($7)
	#nop
	bne	$9,$6,$L275
	lw	$10,16($7)
	lw	$9,12($16)
	#nop
	bne	$10,$9,$L275
	lw	$10,20($7)
	lw	$9,16($16)
	#nop
	bne	$10,$9,$L275
	lw	$9,24($7)
	#nop
	bne	$9,$2,$L275
	lw	$9,28($7)
	#nop
	bne	$9,$5,$L275
	lw	$3,36($7)
	sb	$0,0($7)
	lw	$2,40($7)
	beq	$3,$0,$L276
	.set	noreorder
	.set	nomacro
	j	$L277
	sw	$2,40($3)
	.set	macro
	.set	reorder

$L276:
	lui	$3,%hi(table_napt)
	lw	$3,%lo(table_napt)($3)
	#nop
	lw	$3,0($3)
	#nop
	addu	$3,$3,$8
	sw	$2,4($3)
$L277:
	lw	$2,40($7)
	lw	$3,36($7)
	#nop
	sw	$3,0($2)
	lui	$2,%hi(table_napt)
	lw	$2,%lo(table_napt)($2)
	#nop
	lw	$2,0($2)
	#nop
	addu	$8,$2,$8
	lw	$2,8($8)
	#nop
	addiu	$2,$2,-1
	sw	$2,8($8)
	lw	$3,44($7)
	lw	$2,48($7)
	beq	$3,$0,$L278
	.set	noreorder
	.set	nomacro
	j	$L279
	sw	$2,48($3)
	.set	macro
	.set	reorder

$L278:
	lui	$3,%hi(napt_list_inuse+4)
	sw	$2,%lo(napt_list_inuse+4)($3)
$L279:
	lw	$2,48($7)
	lw	$4,44($7)
	lui	$3,%hi(napt_list_inuse)
	addiu	$3,$3,%lo(napt_list_inuse)
	sw	$4,0($2)
	lw	$4,8($3)
	lui	$2,%hi(napt_list_free)
	addiu	$2,$2,%lo(napt_list_free)
	addiu	$4,$4,-1
	sw	$4,8($3)
	sw	$0,44($7)
	lw	$3,4($2)
	addiu	$4,$7,44
	sw	$3,48($7)
	lw	$3,4($2)
	#nop
	sw	$7,0($3)
	lw	$3,8($2)
	sw	$4,4($2)
	lui	$4,%hi(path_table_list_max)
	addiu	$3,$3,1
	sw	$3,8($2)
	lw	$3,4($16)
	lw	$2,20($16)
	lw	$5,24($16)
	lw	$8,%lo(path_table_list_max)($4)
	xor	$6,$2,$3
	lw	$4,8($16)
	srl	$7,$3,8
	xor	$6,$6,$5
	xor	$6,$6,$7
	srl	$7,$2,16
	xor	$6,$6,$7
	srl	$7,$4,4
	xor	$6,$6,$7
	xor	$6,$6,$18
	srl	$7,$6,12
	xor	$6,$7,$6
	addiu	$8,$8,-1
	and	$7,$6,$8
	li	$8,12			# 0xc
	mult	$7,$8
	lui	$6,%hi(table_path)
	lw	$6,%lo(table_path)($6)
	#nop
	lw	$6,0($6)
	mflo	$7
	addu	$6,$6,$7
	lw	$6,0($6)
	.set	noreorder
	.set	nomacro
	j	$L280
	li	$8,1			# 0x1
	.set	macro
	.set	reorder

$L287:
	lw	$10,4($6)
	lw	$9,0($16)
	#nop
	bne	$10,$9,$L281
	lbu	$9,2($6)
	#nop
	bne	$9,$8,$L281
	lw	$9,16($6)
	#nop
	bne	$9,$3,$L281
	lw	$9,12($6)
	#nop
	bne	$9,$4,$L281
	lw	$10,24($6)
	lw	$9,12($16)
	#nop
	bne	$10,$9,$L281
	lw	$10,28($6)
	lw	$9,16($16)
	#nop
	bne	$10,$9,$L281
	lw	$9,20($6)
	#nop
	bne	$9,$2,$L281
	lw	$9,8($6)
	#nop
	bne	$9,$5,$L281
	lw	$3,72($6)
	sb	$0,0($6)
	lw	$2,76($6)
	beq	$3,$0,$L282
	.set	noreorder
	.set	nomacro
	j	$L283
	sw	$2,76($3)
	.set	macro
	.set	reorder

$L282:
	lui	$3,%hi(table_path)
	lw	$3,%lo(table_path)($3)
	#nop
	lw	$3,0($3)
	#nop
	addu	$3,$3,$7
	sw	$2,4($3)
$L283:
	lw	$2,76($6)
	lw	$3,72($6)
	#nop
	sw	$3,0($2)
	lui	$2,%hi(table_path)
	lw	$2,%lo(table_path)($2)
	#nop
	lw	$2,0($2)
	#nop
	addu	$7,$2,$7
	lw	$2,8($7)
	#nop
	addiu	$2,$2,-1
	sw	$2,8($7)
	lw	$3,80($6)
	lw	$2,84($6)
	beq	$3,$0,$L284
	.set	noreorder
	.set	nomacro
	j	$L285
	sw	$2,84($3)
	.set	macro
	.set	reorder

$L284:
	lui	$3,%hi(path_list_inuse+4)
	sw	$2,%lo(path_list_inuse+4)($3)
$L285:
	lw	$2,84($6)
	lw	$4,80($6)
	lui	$3,%hi(path_list_inuse)
	addiu	$3,$3,%lo(path_list_inuse)
	sw	$4,0($2)
	lw	$4,8($3)
	lui	$2,%hi(path_list_free)
	addiu	$2,$2,%lo(path_list_free)
	addiu	$4,$4,-1
	sw	$4,8($3)
	sw	$0,80($6)
	lw	$3,4($2)
	addiu	$4,$6,80
	sw	$3,84($6)
	lw	$3,4($2)
	#nop
	sw	$6,0($3)
	lw	$3,8($2)
	sw	$4,4($2)
	addiu	$3,$3,1
	.set	noreorder
	.set	nomacro
	j	$L286
	sw	$3,8($2)
	.set	macro
	.set	reorder

$L281:
	lw	$6,72($6)
	#nop
$L280:
	bne	$6,$0,$L287
$L286:
	lw	$3,20($16)
	lw	$2,12($16)
	lw	$5,16($16)
	lw	$4,24($16)
	xor	$7,$2,$3
	srl	$6,$3,8
	xor	$7,$7,$5
	xor	$7,$7,$6
	srl	$6,$2,16
	xor	$7,$7,$6
	srl	$6,$4,4
	xor	$7,$7,$6
	lui	$6,%hi(path_table_list_max)
	lw	$6,%lo(path_table_list_max)($6)
	xor	$18,$7,$18
	srl	$7,$18,12
	xor	$18,$7,$18
	addiu	$6,$6,-1
	and	$18,$18,$6
	li	$6,12			# 0xc
	mult	$18,$6
	lui	$6,%hi(table_path)
	lw	$6,%lo(table_path)($6)
	#nop
	lw	$6,0($6)
	mflo	$18
	addu	$6,$6,$18
	lw	$6,0($6)
	.set	noreorder
	.set	nomacro
	j	$L288
	li	$7,2			# 0x2
	.set	macro
	.set	reorder

$L295:
	lw	$9,4($6)
	lw	$8,0($16)
	#nop
	bne	$9,$8,$L289
	lbu	$8,2($6)
	#nop
	bne	$8,$7,$L289
	lw	$8,20($6)
	#nop
	bne	$8,$2,$L289
	lw	$8,8($6)
	#nop
	bne	$8,$5,$L289
	lw	$8,24($6)
	#nop
	bne	$8,$3,$L289
	lw	$8,28($6)
	#nop
	bne	$8,$4,$L289
	lw	$9,32($6)
	lw	$8,4($16)
	#nop
	bne	$9,$8,$L289
	lw	$9,36($6)
	lw	$8,8($16)
	#nop
	bne	$9,$8,$L289
	lw	$3,72($6)
	sb	$0,0($6)
	lw	$2,76($6)
	beq	$3,$0,$L290
	.set	noreorder
	.set	nomacro
	j	$L291
	sw	$2,76($3)
	.set	macro
	.set	reorder

$L290:
	lui	$3,%hi(table_path)
	lw	$3,%lo(table_path)($3)
	#nop
	lw	$3,0($3)
	#nop
	addu	$3,$3,$18
	sw	$2,4($3)
$L291:
	lw	$2,76($6)
	lw	$3,72($6)
	#nop
	sw	$3,0($2)
	lui	$2,%hi(table_path)
	lw	$2,%lo(table_path)($2)
	#nop
	lw	$2,0($2)
	#nop
	addu	$18,$2,$18
	lw	$2,8($18)
	#nop
	addiu	$2,$2,-1
	sw	$2,8($18)
	lw	$3,80($6)
	lw	$2,84($6)
	beq	$3,$0,$L292
	.set	noreorder
	.set	nomacro
	j	$L293
	sw	$2,84($3)
	.set	macro
	.set	reorder

$L292:
	lui	$3,%hi(path_list_inuse+4)
	sw	$2,%lo(path_list_inuse+4)($3)
$L293:
	lw	$2,84($6)
	lw	$4,80($6)
	lui	$3,%hi(path_list_inuse)
	addiu	$3,$3,%lo(path_list_inuse)
	sw	$4,0($2)
	lw	$4,8($3)
	lui	$2,%hi(path_list_free)
	addiu	$2,$2,%lo(path_list_free)
	addiu	$4,$4,-1
	sw	$4,8($3)
	sw	$0,80($6)
	lw	$3,4($2)
	addiu	$4,$6,80
	sw	$3,84($6)
	lw	$3,4($2)
	#nop
	sw	$6,0($3)
	lw	$3,8($2)
	sw	$4,4($2)
	addiu	$3,$3,1
	.set	noreorder
	.set	nomacro
	j	$L294
	sw	$3,8($2)
	.set	macro
	.set	reorder

$L289:
	lw	$6,72($6)
	#nop
$L288:
	bne	$6,$0,$L295
$L294:
	andi	$2,$17,0x1
	bne	$2,$0,$L296
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$17
 # 0 "" 2
#NO_APP
	j	$L297
$L296:
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$17
 # 0 "" 2
#NO_APP
	.set	noreorder
	.set	nomacro
	j	$L297
	move	$2,$0
	.set	macro
	.set	reorder

$L275:
	lw	$7,36($7)
	#nop
$L274:
	bne	$7,$0,$L298
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$17
 # 0 "" 2
#NO_APP
	li	$2,-4			# 0xfffffffc
$L297:
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
	beq	$5,$0,$L302
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
	beq	$2,$0,$L302
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
 # 928 "net/rtl/fastpath/fastpath_core.c" 1
	raw_local_irq_save	$23
 # 0 "" 2
#NO_APP
	lw	$4,4($16)
	lw	$6,8($16)
	lw	$3,20($16)
	lw	$5,24($16)
	xor	$7,$6,$4
	xor	$7,$7,$3
	srl	$8,$4,16
	lw	$2,0($16)
	xor	$7,$7,$5
	xor	$7,$7,$8
	srl	$8,$3,16
	xor	$7,$7,$8
	lui	$8,%hi(napt_table_list_max)
	lw	$9,%lo(napt_table_list_max)($8)
	andi	$8,$2,0xffff
	xor	$7,$7,$8
	srl	$8,$7,12
	xor	$7,$8,$7
	addiu	$9,$9,-1
	and	$8,$7,$9
	li	$9,12			# 0xc
	mult	$8,$9
	lui	$7,%hi(table_napt)
	lw	$7,%lo(table_napt)($7)
	#nop
	lw	$7,0($7)
	mflo	$8
	addu	$7,$7,$8
	lw	$7,0($7)
	j	$L305
$L309:
	lw	$9,4($7)
	#nop
	bne	$9,$2,$L306
	lw	$9,8($7)
	#nop
	bne	$9,$4,$L306
	lw	$9,12($7)
	#nop
	bne	$9,$6,$L306
	lw	$10,16($7)
	lw	$9,12($16)
	#nop
	bne	$10,$9,$L306
	lw	$10,20($7)
	lw	$9,16($16)
	#nop
	bne	$10,$9,$L306
	lw	$9,24($7)
	#nop
	bne	$9,$3,$L306
	lw	$9,28($7)
	#nop
	bne	$9,$5,$L306
	lw	$4,152($sp)
	move	$5,$fp
	.set	noreorder
	.set	nomacro
	jal	rtl_conntrack_drop_check_hook
	move	$6,$4
	.set	macro
	.set	reorder

	andi	$2,$23,0x1
	bne	$2,$0,$L307
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$23
 # 0 "" 2
#NO_APP
	j	$L308
$L307:
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$23
 # 0 "" 2
#NO_APP
	.set	noreorder
	.set	nomacro
	j	$L308
	move	$2,$0
	.set	macro
	.set	reorder

$L306:
	lw	$7,36($7)
	#nop
$L305:
	bne	$7,$0,$L309
	lui	$4,%hi(napt_list_free)
	lw	$3,%lo(napt_list_free)($4)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L310
	li	$7,-1			# 0xffffffff
	.set	macro
	.set	reorder

	sw	$2,4($3)
	lw	$2,4($16)
	lw	$5,44($3)
	sw	$2,8($3)
	lw	$6,8($16)
	lw	$2,48($3)
	sw	$6,12($3)
	lw	$6,12($16)
	#nop
	sw	$6,16($3)
	lw	$6,16($16)
	#nop
	sw	$6,20($3)
	lw	$6,20($16)
	#nop
	sw	$6,24($3)
	lw	$6,24($16)
	sw	$18,32($3)
	sb	$7,0($3)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L311
	sw	$6,28($3)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L312
	sw	$2,48($5)
	.set	macro
	.set	reorder

$L311:
	addiu	$4,$4,%lo(napt_list_free)
	sw	$2,4($4)
$L312:
	lw	$2,48($3)
	lw	$5,44($3)
	lui	$4,%hi(napt_list_free)
	addiu	$4,$4,%lo(napt_list_free)
	sw	$5,0($2)
	lw	$5,8($4)
	lui	$2,%hi(napt_list_inuse)
	addiu	$2,$2,%lo(napt_list_inuse)
	addiu	$5,$5,-1
	sw	$5,8($4)
	sw	$0,44($3)
	lw	$5,4($2)
	addiu	$7,$3,44
	lui	$4,%hi(table_napt)
	sw	$5,48($3)
	lw	$6,4($2)
	addiu	$5,$3,36
	sw	$3,0($6)
	lw	$6,8($2)
	sw	$7,4($2)
	addiu	$6,$6,1
	sw	$6,8($2)
	sw	$0,36($3)
	lw	$2,%lo(table_napt)($4)
	#nop
	lw	$2,0($2)
	#nop
	addu	$2,$2,$8
	lw	$2,4($2)
	#nop
	sw	$2,40($3)
	lw	$2,%lo(table_napt)($4)
	#nop
	lw	$2,0($2)
	#nop
	addu	$2,$2,$8
	lw	$2,4($2)
	#nop
	sw	$3,0($2)
	lw	$2,%lo(table_napt)($4)
	#nop
	lw	$2,0($2)
	#nop
	addu	$2,$2,$8
	sw	$5,4($2)
	lw	$2,%lo(table_napt)($4)
	#nop
	lw	$2,0($2)
	#nop
	addu	$8,$2,$8
	lw	$2,8($8)
	#nop
	addiu	$2,$2,1
	sw	$2,8($8)
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
	move	$20,$2
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_protocol
	move	$19,$2
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,64($sp)
	.set	macro
	.set	reorder

	lbu	$22,0($2)
	move	$4,$17
	move	$21,$2
	andi	$22,$22,0xf
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
	lw	$2,12($21)
	move	$4,$17
	sw	$2,96($sp)
	lhu	$3,0($22)
	#nop
	sw	$3,88($sp)
	lw	$5,16($21)
	#nop
	sw	$5,92($sp)
	lhu	$6,2($22)
	.set	noreorder
	.set	nomacro
	jal	rtl_store_skb_dst
	sw	$6,84($sp)
	.set	macro
	.set	reorder

	lui	$2,%hi(path_list_free)
	lw	$18,%lo(path_list_free)($2)
	lui	$2,%hi(path_table_list_max)
	lw	$2,%lo(path_table_list_max)($2)
	#nop
	sw	$2,76($sp)
	lw	$2,4($16)
	#nop
	sw	$2,60($sp)
	lw	$3,8($16)
	#nop
	sw	$3,72($sp)
	lw	$5,20($16)
	#nop
	sw	$5,56($sp)
	lw	$6,24($16)
	.set	noreorder
	.set	nomacro
	beq	$18,$0,$L313
	sw	$6,68($sp)
	.set	macro
	.set	reorder

	lw	$2,0($16)
	move	$4,$0
	sw	$2,4($18)
	lw	$2,4($16)
	#nop
	sw	$2,16($18)
	lw	$2,8($16)
	#nop
	sw	$2,12($18)
	lw	$2,20($16)
	#nop
	sw	$2,20($18)
	lw	$2,24($16)
	#nop
	sw	$2,8($18)
	lw	$2,12($16)
	#nop
	sw	$2,24($18)
	lw	$2,16($16)
	#nop
	sw	$2,28($18)
	lw	$6,20($16)
	#nop
	sw	$6,32($18)
	lw	$2,24($16)
	#nop
	sw	$2,36($18)
	lui	$2,%hi(route_list_inuse)
	lw	$2,%lo(route_list_inuse)($2)
	.set	noreorder
	.set	nomacro
	j	$L314
	move	$5,$0
	.set	macro
	.set	reorder

$L317:
	lw	$3,8($2)
	#nop
	sltu	$7,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$7,$0,$L315
	and	$8,$3,$6
	.set	macro
	.set	reorder

	lw	$7,4($2)
	#nop
	bne	$8,$7,$L315
	.set	noreorder
	.set	nomacro
	j	$L316
	addiu	$5,$2,16
	.set	macro
	.set	reorder

$L315:
	move	$3,$4
$L316:
	lw	$2,44($2)
	move	$4,$3
$L314:
	bne	$2,$0,$L317
	li	$2,1			# 0x1
	sb	$2,2($18)
	li	$2,-1			# 0xffffffff
	sb	$2,0($18)
	sw	$5,44($18)
	sb	$0,1($18)
	lui	$2,%hi(rtl_init_bypass_packet_hook)
	lw	$2,%lo(rtl_init_bypass_packet_hook)($2)
	#nop
	beq	$2,$0,$L318
	.set	noreorder
	.set	nomacro
	jalr	$2
	move	$4,$18
	.set	macro
	.set	reorder

$L318:
	lw	$3,16($18)
	lw	$2,24($18)
	#nop
	beq	$3,$2,$L319
	lbu	$2,1($18)
	#nop
	ori	$2,$2,0x1
	sb	$2,1($18)
$L319:
	lw	$3,12($18)
	lw	$2,28($18)
	#nop
	beq	$3,$2,$L320
	lbu	$2,1($18)
	#nop
	ori	$2,$2,0x2
	sb	$2,1($18)
$L320:
	sw	$0,48($18)
	sw	$0,52($18)
	lw	$2,4($16)
	addiu	$3,$sp,36
	move	$4,$3
	sw	$2,12($21)
	lw	$2,8($16)
	move	$5,$0
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,20($16)
	#nop
	sw	$2,16($21)
	lw	$2,24($16)
	#nop
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

	lw	$3,100($sp)
	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L321
	move	$6,$3
	.set	macro
	.set	reorder

	lw	$4,4($16)
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$5,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L321
	move	$4,$17
	.set	macro
	.set	reorder

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
	bne	$3,$2,$L322
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
	#nop
	bne	$2,$3,$L322
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
	beq	$3,$2,$L322
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffea
	.set	macro
	.set	reorder

$L322:
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

	lw	$3,64($sp)
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L321
	lui	$2,%hi(nf_hooks+128)
	.set	macro
	.set	reorder

	lw	$3,%lo(nf_hooks+128)($2)
	addiu	$2,$2,%lo(nf_hooks+128)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L323
	move	$4,$17
	.set	macro
	.set	reorder

	move	$5,$0
	move	$6,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$19
	.set	macro
	.set	reorder

$L323:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	beq	$2,$0,$L324
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L366
	sw	$2,48($18)
	.set	macro
	.set	reorder

$L324:
	lw	$2,48($18)
	#nop
	sw	$2,48($18)
$L366:
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
	move	$6,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$19
	.set	macro
	.set	reorder

$L326:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	beq	$2,$0,$L327
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L321
	sw	$2,52($18)
	.set	macro
	.set	reorder

$L327:
	lw	$2,52($18)
	#nop
	sw	$2,52($18)
$L321:
	lw	$2,12($16)
	addiu	$3,$sp,36
	move	$4,$3
	sw	$2,12($21)
	lw	$2,16($16)
	move	$5,$0
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,20($16)
	#nop
	sw	$2,16($21)
	lw	$2,24($16)
	#nop
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

	lw	$3,100($sp)
	.set	noreorder
	.set	nomacro
	beq	$19,$0,$L329
	move	$6,$3
	.set	macro
	.set	reorder

	lw	$4,20($16)
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$5,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L369
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
	bne	$3,$2,$L330
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	sw	$3,100($sp)
	.set	macro
	.set	reorder

	lw	$3,100($sp)
	#nop
	bne	$2,$3,$L330
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
	beq	$3,$2,$L330
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffea
	.set	macro
	.set	reorder

$L330:
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

	move	$5,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_smac
	move	$4,$17
	.set	macro
	.set	reorder

	lw	$5,64($sp)
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	bne	$5,$2,$L369
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	lui	$2,%hi(nf_hooks+160)
	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L331
	move	$4,$17
	.set	macro
	.set	reorder

	li	$5,4			# 0x4
	move	$6,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$19
	.set	macro
	.set	reorder

$L331:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	beq	$2,$0,$L332
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L329
	sw	$2,52($18)
	.set	macro
	.set	reorder

$L332:
	lw	$2,52($18)
	#nop
	sw	$2,52($18)
$L329:
	li	$2,6			# 0x6
$L369:
	.set	noreorder
	.set	nomacro
	bne	$fp,$2,$L334
	lui	$2,%hi(nf_ct_udp_timeout_stream)
	.set	macro
	.set	reorder

	lw	$4,152($sp)
	jal	rtl_tcp_get_timeouts
	.set	noreorder
	.set	nomacro
	j	$L367
	sw	$2,60($18)
	.set	macro
	.set	reorder

$L334:
	lw	$2,%lo(nf_ct_udp_timeout_stream)($2)
	#nop
	sw	$2,60($18)
$L367:
	lw	$6,152($sp)
	lui	$2,%hi(jiffies)
	lw	$3,80($18)
	sw	$6,56($18)
	lw	$4,%lo(jiffies)($2)
	lw	$2,84($18)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L336
	sw	$4,40($18)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L337
	sw	$2,84($3)
	.set	macro
	.set	reorder

$L336:
	lui	$3,%hi(path_list_free+4)
	sw	$2,%lo(path_list_free+4)($3)
$L337:
	lw	$2,84($18)
	lw	$3,80($18)
	lw	$6,60($sp)
	lw	$5,56($sp)
	sw	$3,0($2)
	lw	$2,68($sp)
	xor	$4,$5,$6
	srl	$3,$6,8
	xor	$4,$4,$2
	xor	$4,$4,$3
	lw	$3,56($sp)
	lw	$6,72($sp)
	lui	$2,%hi(path_list_free)
	addiu	$2,$2,%lo(path_list_free)
	srl	$5,$3,16
	lw	$3,8($2)
	xor	$4,$4,$5
	srl	$5,$6,4
	xor	$4,$4,$5
	xor	$4,$4,$fp
	addiu	$3,$3,-1
	sw	$3,8($2)
	sw	$0,80($18)
	srl	$3,$4,12
	xor	$4,$3,$4
	lw	$3,76($sp)
	lui	$2,%hi(path_list_inuse)
	addiu	$2,$2,%lo(path_list_inuse)
	addiu	$5,$3,-1
	and	$4,$4,$5
	lw	$3,4($2)
	li	$5,12			# 0xc
	mult	$4,$5
	addiu	$5,$18,80
	sw	$3,84($18)
	lw	$6,4($2)
	lui	$3,%hi(table_path)
	sw	$18,0($6)
	lw	$6,8($2)
	sw	$5,4($2)
	addiu	$5,$18,72
	addiu	$6,$6,1
	sw	$6,8($2)
	sw	$0,72($18)
	lw	$2,%lo(table_path)($3)
	#nop
	lw	$2,0($2)
	mflo	$4
	addu	$2,$2,$4
	lw	$2,4($2)
	#nop
	sw	$2,76($18)
	lw	$2,%lo(table_path)($3)
	#nop
	lw	$2,0($2)
	#nop
	addu	$2,$2,$4
	lw	$2,4($2)
	#nop
	sw	$18,0($2)
	lw	$2,%lo(table_path)($3)
	#nop
	lw	$2,0($2)
	#nop
	addu	$2,$2,$4
	sw	$5,4($2)
	lw	$2,%lo(table_path)($3)
	#nop
	lw	$2,0($2)
	#nop
	addu	$4,$2,$4
	lw	$2,8($4)
	#nop
	addiu	$2,$2,1
	sw	$2,8($4)
$L313:
	lui	$2,%hi(path_list_free)
	lw	$18,%lo(path_list_free)($2)
	lui	$2,%hi(path_table_list_max)
	lw	$2,%lo(path_table_list_max)($2)
	#nop
	sw	$2,76($sp)
	lw	$2,20($16)
	#nop
	sw	$2,60($sp)
	lw	$3,24($16)
	#nop
	sw	$3,72($sp)
	lw	$5,12($16)
	#nop
	sw	$5,56($sp)
	lw	$6,16($16)
	.set	noreorder
	.set	nomacro
	beq	$18,$0,$L338
	sw	$6,68($sp)
	.set	macro
	.set	reorder

	lw	$2,0($16)
	move	$4,$0
	sw	$2,4($18)
	lw	$2,20($16)
	#nop
	sw	$2,16($18)
	lw	$2,24($16)
	#nop
	sw	$2,12($18)
	lw	$2,12($16)
	#nop
	sw	$2,20($18)
	lw	$2,16($16)
	#nop
	sw	$2,8($18)
	lw	$2,20($16)
	#nop
	sw	$2,24($18)
	lw	$2,24($16)
	#nop
	sw	$2,28($18)
	lw	$6,4($16)
	#nop
	sw	$6,32($18)
	lw	$2,8($16)
	#nop
	sw	$2,36($18)
	lui	$2,%hi(route_list_inuse)
	lw	$2,%lo(route_list_inuse)($2)
	.set	noreorder
	.set	nomacro
	j	$L339
	move	$5,$0
	.set	macro
	.set	reorder

$L342:
	lw	$3,8($2)
	#nop
	sltu	$7,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$7,$0,$L340
	and	$8,$3,$6
	.set	macro
	.set	reorder

	lw	$7,4($2)
	#nop
	bne	$8,$7,$L340
	.set	noreorder
	.set	nomacro
	j	$L341
	addiu	$5,$2,16
	.set	macro
	.set	reorder

$L340:
	move	$3,$4
$L341:
	lw	$2,44($2)
	move	$4,$3
$L339:
	bne	$2,$0,$L342
	li	$2,2			# 0x2
	sb	$2,2($18)
	li	$2,-1			# 0xffffffff
	sb	$2,0($18)
	sw	$5,44($18)
	sb	$0,1($18)
	lui	$2,%hi(rtl_init_bypass_packet_hook)
	lw	$2,%lo(rtl_init_bypass_packet_hook)($2)
	#nop
	beq	$2,$0,$L343
	.set	noreorder
	.set	nomacro
	jalr	$2
	move	$4,$18
	.set	macro
	.set	reorder

$L343:
	lw	$3,20($18)
	lw	$2,32($18)
	#nop
	beq	$3,$2,$L344
	lbu	$2,1($18)
	#nop
	ori	$2,$2,0x4
	sb	$2,1($18)
$L344:
	lw	$3,8($18)
	lw	$2,36($18)
	#nop
	beq	$3,$2,$L345
	lbu	$2,1($18)
	#nop
	ori	$2,$2,0x8
	sb	$2,1($18)
$L345:
	sw	$0,48($18)
	sw	$0,52($18)
	lw	$2,20($16)
	addiu	$3,$sp,36
	move	$4,$3
	sw	$2,12($21)
	lw	$2,24($16)
	move	$5,$0
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,12($16)
	#nop
	sw	$2,16($21)
	lw	$2,16($16)
	#nop
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

	lw	$3,100($sp)
	.set	noreorder
	.set	nomacro
	beq	$19,$0,$L346
	move	$6,$3
	.set	macro
	.set	reorder

	lw	$4,20($16)
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$5,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L346
	move	$4,$17
	.set	macro
	.set	reorder

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
	bne	$3,$2,$L347
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
	#nop
	bne	$2,$3,$L347
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
	beq	$3,$2,$L347
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffea
	.set	macro
	.set	reorder

$L347:
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
	move	$5,$19
	.set	macro
	.set	reorder

	lw	$3,64($sp)
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L348
	lui	$2,%hi(nf_hooks+128)
	.set	macro
	.set	reorder

	lw	$3,%lo(nf_hooks+128)($2)
	addiu	$2,$2,%lo(nf_hooks+128)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L348
	move	$4,$17
	.set	macro
	.set	reorder

	move	$5,$0
	move	$6,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$20
	.set	macro
	.set	reorder

$L348:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	beq	$2,$0,$L349
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L346
	sw	$2,48($18)
	.set	macro
	.set	reorder

$L349:
	lw	$2,48($18)
	#nop
	sw	$2,48($18)
$L346:
	lw	$2,20($16)
	addiu	$3,$sp,36
	move	$4,$3
	sw	$2,12($21)
	lw	$2,24($16)
	move	$5,$0
	li	$6,14			# 0xe
	sh	$2,0($22)
	lw	$2,4($16)
	#nop
	sw	$2,16($21)
	lw	$2,8($16)
	#nop
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

	lw	$3,100($sp)
	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L351
	move	$6,$3
	.set	macro
	.set	reorder

	lw	$4,4($16)
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$5,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L351
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
	bne	$16,$2,$L352
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	move	$4,$17
	.set	macro
	.set	reorder

	bne	$2,$16,$L352
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
	beq	$3,$2,$L352
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffea
	.set	macro
	.set	reorder

$L352:
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

	move	$5,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_smac
	move	$4,$17
	.set	macro
	.set	reorder

	lw	$5,64($sp)
	li	$2,2048			# 0x800
	.set	noreorder
	.set	nomacro
	bne	$5,$2,$L351
	lui	$2,%hi(nf_hooks+160)
	.set	macro
	.set	reorder

	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L351
	move	$4,$17
	.set	macro
	.set	reorder

	li	$5,4			# 0x4
	move	$6,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_ipt_do_table
	move	$7,$20
	.set	macro
	.set	reorder

$L351:
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
	bne	$fp,$2,$L353
	lui	$2,%hi(nf_ct_udp_timeout_stream)
	.set	macro
	.set	reorder

	lw	$4,152($sp)
	jal	rtl_tcp_get_timeouts
	.set	noreorder
	.set	nomacro
	j	$L368
	sw	$2,60($18)
	.set	macro
	.set	reorder

$L353:
	lw	$2,%lo(nf_ct_udp_timeout_stream)($2)
	#nop
	sw	$2,60($18)
$L368:
	lw	$6,152($sp)
	lui	$2,%hi(jiffies)
	lw	$3,80($18)
	sw	$6,56($18)
	lw	$4,%lo(jiffies)($2)
	lw	$2,84($18)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L355
	sw	$4,40($18)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L356
	sw	$2,84($3)
	.set	macro
	.set	reorder

$L355:
	lui	$3,%hi(path_list_free+4)
	sw	$2,%lo(path_list_free+4)($3)
$L356:
	lw	$2,84($18)
	lw	$3,80($18)
	lw	$6,60($sp)
	lw	$5,56($sp)
	sw	$3,0($2)
	lw	$2,68($sp)
	xor	$4,$5,$6
	srl	$3,$6,8
	xor	$4,$4,$2
	xor	$4,$4,$3
	lw	$3,56($sp)
	lw	$6,72($sp)
	lui	$2,%hi(path_list_free)
	addiu	$2,$2,%lo(path_list_free)
	srl	$5,$3,16
	lw	$3,8($2)
	xor	$4,$4,$5
	srl	$5,$6,4
	xor	$4,$4,$5
	xor	$4,$4,$fp
	addiu	$3,$3,-1
	sw	$3,8($2)
	sw	$0,80($18)
	srl	$3,$4,12
	xor	$4,$3,$4
	lw	$3,76($sp)
	lui	$2,%hi(path_list_inuse)
	addiu	$2,$2,%lo(path_list_inuse)
	addiu	$5,$3,-1
	and	$4,$4,$5
	lw	$3,4($2)
	li	$5,12			# 0xc
	mult	$4,$5
	addiu	$5,$18,80
	sw	$3,84($18)
	lw	$6,4($2)
	lui	$3,%hi(table_path)
	sw	$18,0($6)
	lw	$6,8($2)
	sw	$5,4($2)
	addiu	$5,$18,72
	addiu	$6,$6,1
	sw	$6,8($2)
	sw	$0,72($18)
	lw	$2,%lo(table_path)($3)
	#nop
	lw	$2,0($2)
	mflo	$4
	addu	$2,$2,$4
	lw	$2,4($2)
	#nop
	sw	$2,76($18)
	lw	$2,%lo(table_path)($3)
	#nop
	lw	$2,0($2)
	#nop
	addu	$2,$2,$4
	lw	$2,4($2)
	#nop
	sw	$18,0($2)
	lw	$2,%lo(table_path)($3)
	#nop
	lw	$2,0($2)
	#nop
	addu	$2,$2,$4
	sw	$5,4($2)
	lw	$2,%lo(table_path)($3)
	#nop
	lw	$2,0($2)
	#nop
	addu	$4,$2,$4
	lw	$2,8($4)
	#nop
	addiu	$2,$2,1
	sw	$2,8($4)
$L338:
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
	move	$4,$17
	sw	$2,12($21)
	lw	$3,88($sp)
	#nop
	sh	$3,0($22)
	lw	$5,92($sp)
	#nop
	sw	$5,16($21)
	lw	$6,84($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dst
	sh	$6,2($22)
	.set	macro
	.set	reorder

	beq	$20,$0,$L357
#APP
 # 114 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$2
 # 0 "" 2
#NO_APP
	lw	$3,452($20)
	#nop
	addiu	$3,$3,-1
	sw	$3,452($20)
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$2
 # 0 "" 2
#NO_APP
$L357:
	beq	$19,$0,$L358
#APP
 # 114 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$2
 # 0 "" 2
#NO_APP
	lw	$3,452($19)
	#nop
	addiu	$3,$3,-1
	sw	$3,452($19)
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$2
 # 0 "" 2
#NO_APP
$L358:
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$23
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
	j	$L308
	move	$2,$0
	.set	macro
	.set	reorder

$L310:
#APP
 # 134 "/home/kai_wang/3466/branch-rtl819x-sdk-v3.4.6.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$23
 # 0 "" 2
#NO_APP
	.set	noreorder
	.set	nomacro
	j	$L308
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

$L302:
	li	$2,-1			# 0xffffffff
$L308:
	lw	$31,140($sp)
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
	
$L371 = .
$L372 = .
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	lui	$2,%hi(malloc_sizes+60)
	lw	$4,%lo(malloc_sizes+60)($2)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	lui	$3,%hi(negative_cache_table)
	beq	$2,$0,$L373
	sw	$2,%lo(negative_cache_table)($3)

	lui	$4,%hi(negative_cache_list_free)
	addiu	$2,$4,%lo(negative_cache_list_free)
	sw	$0,%lo(negative_cache_list_free)($4)
	sw	$2,4($2)
	sw	$0,8($2)
	li	$5,12			# 0xc
	move	$2,$0
	li	$4,64			# 0x40
$L374:
	mult	$2,$5
	lw	$7,%lo(negative_cache_table)($3)
	addiu	$2,$2,1
	mflo	$6
	addu	$7,$7,$6
	sw	$0,8($7)
	lw	$7,%lo(negative_cache_table)($3)
	nop
	addu	$7,$7,$6
	sw	$0,0($7)
	lw	$7,%lo(negative_cache_table)($3)
	nop
	addu	$6,$7,$6
	bne	$2,$4,$L374
	sw	$6,4($6)

	lui	$20,%hi(malloc_sizes)
	lui	$19,%hi(negative_cache_timeout)
	lui	$16,%hi(negative_cache_list_free)
	addiu	$20,$20,%lo(malloc_sizes)
	addiu	$19,$19,%lo(negative_cache_timeout)
	addiu	$16,$16,%lo(negative_cache_list_free)
	move	$17,$0
	li	$18,64			# 0x40
$L375:
	lw	$4,12($20)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	beq	$2,$0,$L373
	addiu	$4,$2,12

	move	$5,$0
	move	$6,$0
	jal	init_timer_key
	sw	$2,16($sp)

	lw	$2,16($sp)
	addiu	$17,$17,1
	sw	$2,28($2)
	sw	$19,24($2)
	sw	$0,44($2)
	lw	$4,4($16)
	addiu	$3,$2,44
	sw	$4,48($2)
	lw	$4,4($16)
	nop
	sw	$2,0($4)
	lw	$2,8($16)
	sw	$3,4($16)
	addiu	$2,$2,1
	bne	$17,$18,$L375
	sw	$2,8($16)

	j	$L376
	li	$2,1			# 0x1

$L373:
	move	$2,$0
$L376:
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
	
$L381 = .
$L382 = .
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	lui	$2,%hi(malloc_sizes+4)
	move	$17,$4
	lw	$4,%lo(malloc_sizes+4)($2)
	li	$5,32			# 0x20
	jal	kmem_cache_alloc
	lui	$16,%hi(udp_cache_table)

	move	$18,$2
	beq	$2,$0,$L383
	sw	$2,%lo(udp_cache_table)($16)

$L384 = .
	li	$4,12			# 0xc
	mult	$17,$4
	lui	$3,%hi(udp_cache_list_free)
	addiu	$2,$3,%lo(udp_cache_list_free)
	sw	$0,%lo(udp_cache_list_free)($3)
	li	$5,32			# 0x20
	lui	$3,%hi(max_udp_frag_entry)
	sw	$2,4($2)
	sw	$0,8($2)
	sw	$17,%lo(max_udp_frag_entry)($3)
	jal	__kmalloc
	mflo	$4

	sw	$2,0($18)
	lw	$4,%lo(udp_cache_table)($16)
	move	$3,$0
	lw	$4,0($4)
	nop
	bne	$4,$0,$L394
	li	$2,-1			# 0xffffffff

	j	$L386
	nop

$L388:
	lw	$5,%lo(udp_cache_table)($16)
	addiu	$3,$3,1
	lw	$5,0($5)
	mflo	$4
	addu	$5,$5,$4
	sw	$0,8($5)
	lw	$5,%lo(udp_cache_table)($16)
	nop
	lw	$5,0($5)
	nop
	addu	$5,$5,$4
	sw	$0,0($5)
	lw	$5,%lo(udp_cache_table)($16)
	nop
	lw	$5,0($5)
	nop
	addu	$4,$5,$4
	j	$L393
	sw	$4,4($4)

$L394:
	li	$2,12			# 0xc
$L393:
	slt	$4,$3,$17
	bne	$4,$0,$L388
	mult	$3,$2

	lui	$20,%hi(malloc_sizes)
	lui	$19,%hi(cache_timeout)
	lui	$16,%hi(udp_cache_list_free)
	addiu	$20,$20,%lo(malloc_sizes)
	addiu	$19,$19,%lo(cache_timeout)
	addiu	$16,$16,%lo(udp_cache_list_free)
	j	$L389
	move	$18,$0

$L390:
$L391 = .
	lw	$4,12($20)
	jal	kmem_cache_alloc
	li	$5,32			# 0x20

	beq	$2,$0,$L383
	addiu	$4,$2,16

	move	$5,$0
	move	$6,$0
	jal	init_timer_key
	sw	$2,16($sp)

	lw	$2,16($sp)
	addiu	$18,$18,1
	sw	$2,32($2)
	sw	$19,28($2)
	sw	$0,48($2)
	lw	$4,4($16)
	addiu	$3,$2,48
	sw	$4,52($2)
	lw	$4,4($16)
	nop
	sw	$2,0($4)
	lw	$2,8($16)
	sw	$3,4($16)
	addiu	$2,$2,1
	sw	$2,8($16)
$L389:
	slt	$2,$18,$17
	bne	$2,$0,$L390
	li	$2,1			# 0x1

	j	$L386
	nop

$L383:
	move	$2,$0
$L386:
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
	.globl	fastpath_packet_bypassed_num
	.data
	.align	2
	.type	fastpath_packet_bypassed_num, @object
	.size	fastpath_packet_bypassed_num, 4
fastpath_packet_bypassed_num:
	.word	10
	.globl	rtl_init_bypass_packet_hook
	.section	.bss,"aw",@nobits
	.align	2
	.type	rtl_init_bypass_packet_hook, @object
	.size	rtl_init_bypass_packet_hook, 4
rtl_init_bypass_packet_hook:
	.space	4
	.globl	rtl_check_bypass_packet_hook
	.align	2
	.type	rtl_check_bypass_packet_hook, @object
	.size	rtl_check_bypass_packet_hook, 4
rtl_check_bypass_packet_hook:
	.space	4
	.local	napt_table_list_max
	.comm	napt_table_list_max,4,4
	.local	path_table_list_max
	.comm	path_table_list_max,4,4
	.local	max_udp_frag_entry
	.comm	max_udp_frag_entry,4,4
	.globl	route_list_inuse
	.align	2
	.type	route_list_inuse, @object
	.size	route_list_inuse, 12
route_list_inuse:
	.space	12
	.globl	route_list_free
	.align	2
	.type	route_list_free, @object
	.size	route_list_free, 12
route_list_free:
	.space	12
	.globl	napt_list_inuse
	.align	2
	.type	napt_list_inuse, @object
	.size	napt_list_inuse, 12
napt_list_inuse:
	.space	12
	.globl	napt_list_free
	.align	2
	.type	napt_list_free, @object
	.size	napt_list_free, 12
napt_list_free:
	.space	12
	.globl	table_napt
	.align	2
	.type	table_napt, @object
	.size	table_napt, 4
table_napt:
	.space	4
	.globl	path_list_inuse
	.align	2
	.type	path_list_inuse, @object
	.size	path_list_inuse, 12
path_list_inuse:
	.space	12
	.globl	path_list_free
	.align	2
	.type	path_list_free, @object
	.size	path_list_free, 12
path_list_free:
	.space	12
	.globl	table_path
	.align	2
	.type	table_path, @object
	.size	table_path, 4
table_path:
	.space	4
	.globl	udp_cache_list_free
	.align	2
	.type	udp_cache_list_free, @object
	.size	udp_cache_list_free, 12
udp_cache_list_free:
	.space	12
	.globl	udp_cache_table
	.align	2
	.type	udp_cache_table, @object
	.size	udp_cache_table, 4
udp_cache_table:
	.space	4
	.globl	negative_cache_list_free
	.align	2
	.type	negative_cache_list_free, @object
	.size	negative_cache_list_free, 12
negative_cache_list_free:
	.space	12
	.globl	negative_cache_table
	.align	2
	.type	negative_cache_table, @object
	.size	negative_cache_table, 4
negative_cache_table:
	.space	4
	.ident	"GCC: (GNU) 4.4.5-1.5.5p2"
