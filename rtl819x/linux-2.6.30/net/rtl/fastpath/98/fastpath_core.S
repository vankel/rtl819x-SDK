	.file	1 "fastpath_core.c"
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
	.globl	rtl_check_bypass_packet_hook
	.data
	.align	2
	.type	rtl_check_bypass_packet_hook, @object
	.size	rtl_check_bypass_packet_hook, 4
rtl_check_bypass_packet_hook:
	.word	0
	.globl	rtl_init_bypass_packet_hook
	.align	2
	.type	rtl_init_bypass_packet_hook, @object
	.size	rtl_init_bypass_packet_hook, 4
rtl_init_bypass_packet_hook:
	.word	0
	.globl	fastpath_packet_bypassed_num
	.align	2
	.type	fastpath_packet_bypassed_num, @object
	.size	fastpath_packet_bypassed_num, 4
fastpath_packet_bypassed_num:
	.word	10
	.section	.text.rtl_init_bypass_packet,"ax",@progbits
	.align	2
	.globl	rtl_init_bypass_packet
	.ent	rtl_init_bypass_packet
	.type	rtl_init_bypass_packet, @function
rtl_init_bypass_packet:
	.set	nomips16
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
	.section	.text.rtl_check_bypass_packet,"ax",@progbits
	.align	2
	.globl	rtl_check_bypass_packet
	.ent	rtl_check_bypass_packet
	.type	rtl_check_bypass_packet, @function
rtl_check_bypass_packet:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lw	$2,68($4)
	bne	$2,$0,$L4
	li	$5,1			# 0x1

	lw	$2,64($4)
	lui	$3,%hi(fastpath_packet_bypassed_num)
	addiu	$2,$2,1
	sw	$2,64($4)
	lw	$3,%lo(fastpath_packet_bypassed_num)($3)
	sltu	$3,$3,$2
	beq	$3,$0,$L2
	nop

$L4:
	li	$2,1			# 0x1
	move	$5,$0
	sw	$2,68($4)
$L2:
	j	$31
	move	$2,$5

	.set	macro
	.set	reorder
	.end	rtl_check_bypass_packet
	.section	.text.cache_timeout,"ax",@progbits
	.align	2
	.ent	cache_timeout
	.type	cache_timeout, @function
cache_timeout:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lbu	$2,0($4)
	beq	$2,$0,$L13
	move	$7,$4

	lw	$2,8($4)
	lw	$3,4($4)
	lhu	$4,2($4)
	xor	$3,$3,$2
	srl	$2,$3,16
	xor	$2,$2,$4
	xor	$3,$3,$2
	lui	$2,%hi(max_udp_frag_entry)
	lbu	$5,1($7)
	srl	$4,$3,8
	lw	$2,%lo(max_udp_frag_entry)($2)
	xor	$4,$4,$5
	xor	$3,$3,$4
	addiu	$2,$2,-1
	and	$4,$3,$2
	sll	$2,$4,1
	lw	$6,40($7)
	addu	$2,$2,$4
	sb	$0,0($7)
	lw	$3,44($7)
	beq	$6,$0,$L9
	sll	$5,$2,2

	j	$L10
	sw	$3,44($6)

$L9:
	lui	$2,%hi(udp_cache_table)
	lw	$2,%lo(udp_cache_table)($2)
	lw	$2,0($2)
	addu	$2,$5,$2
	sw	$3,4($2)
$L10:
	lw	$2,40($7)
	sll	$3,$4,1
	lw	$5,44($7)
	addu	$3,$3,$4
	sll	$3,$3,2
	lui	$4,%hi(udp_cache_list_free)
	sw	$2,0($5)
	lui	$2,%hi(udp_cache_table)
	lw	$2,%lo(udp_cache_table)($2)
	addiu	$4,$4,%lo(udp_cache_list_free)
	addiu	$6,$7,48
	lw	$2,0($2)
	addu	$3,$3,$2
	lw	$2,8($3)
	addiu	$2,$2,-1
	sw	$2,8($3)
	sw	$0,48($7)
	lw	$2,4($4)
	sw	$2,52($7)
	lw	$2,4($4)
	sw	$7,0($2)
	lw	$2,8($4)
	sw	$6,4($4)
	addiu	$2,$2,1
	sw	$2,8($4)
$L13:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	cache_timeout
	.section	.text.udp_fragCache_init,"ax",@progbits
	.align	2
	.globl	udp_fragCache_init
	.ent	udp_fragCache_init
	.type	udp_fragCache_init, @function
udp_fragCache_init:
	.set	nomips16
	.frame	$sp,56,$31		# vars= 16, regs= 6/0, args= 16, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
$L19:
$L60:
	addiu	$sp,$sp,-56
	lui	$2,%hi(malloc_sizes+4)
	sw	$19,44($sp)
	move	$19,$4
	sw	$18,40($sp)
	li	$5,32			# 0x20
	sw	$17,36($sp)
	sw	$16,32($sp)
	lui	$16,%hi(udp_cache_table)
	sw	$31,52($sp)
	sw	$20,48($sp)
	lw	$4,%lo(malloc_sizes+4)($2)
	jal	kmem_cache_alloc
	move	$18,$16

	move	$3,$0
	move	$17,$2
	beq	$2,$0,$L14
	sw	$2,%lo(udp_cache_table)($16)

$L69:
$L110:
	lui	$20,%hi(udp_cache_list_free)
	sll	$4,$19,1
	addiu	$3,$20,%lo(udp_cache_list_free)
	addu	$4,$4,$19
	sw	$3,4($3)
	lui	$2,%hi(max_udp_frag_entry)
	sw	$0,8($3)
	sll	$4,$4,2
	li	$5,32			# 0x20
	sw	$19,%lo(max_udp_frag_entry)($2)
	jal	__kmalloc
	sw	$0,%lo(udp_cache_list_free)($20)

	sw	$2,0($17)
	lw	$2,%lo(udp_cache_table)($16)
	lw	$2,0($2)
	beq	$2,$0,$L14
	li	$3,-1			# 0xffffffff

	j	$L113
	slt	$5,$0,$19

$L177:
	j	$L14
	move	$3,$0

$L113:
	beq	$5,$0,$L176
	move	$17,$0

	move	$4,$0
$L118:
	lw	$2,%lo(udp_cache_table)($18)
	addiu	$17,$17,1
	slt	$3,$17,$19
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$0,8($2)
	lw	$2,%lo(udp_cache_table)($18)
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$0,0($2)
	lw	$2,%lo(udp_cache_table)($18)
	lw	$2,0($2)
	addu	$2,$4,$2
	addiu	$4,$4,12
	bne	$3,$0,$L118
	sw	$2,4($2)

	move	$17,$0
$L178:
	beq	$5,$0,$L176
	lui	$2,%hi(malloc_sizes+12)

$L126:
$L167:
	li	$5,32			# 0x20
	lw	$4,%lo(malloc_sizes+12)($2)
	jal	kmem_cache_alloc
	addiu	$17,$17,1

	move	$5,$0
	move	$16,$2
	addiu	$4,$2,16
	beq	$2,$0,$L177
	move	$6,$0

	jal	init_timer_key
	nop

	lui	$2,%hi(cache_timeout)
	addiu	$2,$2,%lo(cache_timeout)
	addiu	$3,$20,%lo(udp_cache_list_free)
	sw	$2,28($16)
	addiu	$4,$16,48
	sw	$16,32($16)
	slt	$5,$17,$19
	sw	$0,48($16)
	lw	$2,4($3)
	sw	$2,52($16)
	lw	$2,4($3)
	sw	$16,0($2)
	lw	$2,8($3)
	sw	$4,4($3)
	addiu	$2,$2,1
	j	$L178
	sw	$2,8($3)

$L176:
	li	$3,1			# 0x1
$L14:
	lw	$31,52($sp)
	move	$2,$3
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	udp_fragCache_init
	.section	.text.negative_cache_timeout,"ax",@progbits
	.align	2
	.ent	negative_cache_timeout
	.type	negative_cache_timeout, @function
negative_cache_timeout:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lw	$2,8($4)
	move	$7,$4
	lw	$3,4($4)
	lhu	$4,2($4)
	xor	$3,$3,$2
	srl	$2,$3,16
	xor	$2,$2,$4
	xor	$3,$3,$2
	lui	$2,%hi(max_udp_frag_entry)
	lbu	$5,1($7)
	srl	$4,$3,8
	lw	$2,%lo(max_udp_frag_entry)($2)
	xor	$4,$4,$5
	xor	$3,$3,$4
	addiu	$2,$2,-1
	and	$4,$3,$2
	sll	$2,$4,1
	lw	$6,36($7)
	addu	$2,$2,$4
	sb	$0,0($7)
	lw	$3,40($7)
	beq	$6,$0,$L182
	sll	$5,$2,2

	j	$L183
	sw	$3,40($6)

$L182:
	lui	$2,%hi(negative_cache_table)
	lw	$2,%lo(negative_cache_table)($2)
	addu	$2,$5,$2
	sw	$3,4($2)
$L183:
	lw	$2,36($7)
	sll	$3,$4,1
	lw	$5,40($7)
	addu	$3,$3,$4
	sll	$3,$3,2
	lui	$4,%hi(negative_cache_list_free)
	sw	$2,0($5)
	lui	$2,%hi(negative_cache_table)
	lw	$2,%lo(negative_cache_table)($2)
	addiu	$4,$4,%lo(negative_cache_list_free)
	addiu	$6,$7,44
	addu	$3,$3,$2
	lw	$2,8($3)
	addiu	$2,$2,-1
	sw	$2,8($3)
	sw	$0,44($7)
	lw	$2,4($4)
	sw	$2,48($7)
	lw	$2,4($4)
	sw	$7,0($2)
	lw	$2,8($4)
	sw	$6,4($4)
	addiu	$2,$2,1
	j	$31
	sw	$2,8($4)

	.set	macro
	.set	reorder
	.end	negative_cache_timeout
	.section	.text.negative_fragCache_init,"ax",@progbits
	.align	2
	.globl	negative_fragCache_init
	.ent	negative_fragCache_init
	.type	negative_fragCache_init, @function
negative_fragCache_init:
	.set	nomips16
	.frame	$sp,56,$31		# vars= 16, regs= 5/0, args= 16, gp= 0
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
$L191:
$L232:
	addiu	$sp,$sp,-56
	li	$5,32			# 0x20
	sw	$19,44($sp)
	lui	$19,%hi(malloc_sizes)
	sw	$16,32($sp)
	addiu	$2,$19,%lo(malloc_sizes)
	sw	$31,48($sp)
	sw	$18,40($sp)
	sw	$17,36($sp)
	lw	$4,60($2)
	jal	kmem_cache_alloc
	lui	$16,%hi(negative_cache_table)

	move	$4,$16
	sw	$2,%lo(negative_cache_table)($16)
	beq	$2,$0,$L186
	move	$3,$0

	j	$L301
	lui	$18,%hi(negative_cache_list_free)

$L300:
	j	$L186
	move	$3,$0

$L301:
	addiu	$2,$18,%lo(negative_cache_list_free)
	li	$17,63			# 0x3f
	sw	$2,4($2)
	sw	$0,8($2)
	sw	$0,%lo(negative_cache_list_free)($18)
$L241:
	lw	$2,%lo(negative_cache_table)($4)
	addiu	$17,$17,-1
	addu	$2,$3,$2
	sw	$0,8($2)
	lw	$2,%lo(negative_cache_table)($4)
	addu	$2,$3,$2
	sw	$0,0($2)
	lw	$2,%lo(negative_cache_table)($4)
	addu	$2,$3,$2
	addiu	$3,$3,12
	bgez	$17,$L241
	sw	$2,4($2)

	move	$17,$0
$L295:
$L249:
$L290:
	addiu	$2,$19,%lo(malloc_sizes)
	li	$5,32			# 0x20
	lw	$4,12($2)
	jal	kmem_cache_alloc
	addiu	$17,$17,1

	move	$5,$0
	move	$16,$2
	addiu	$4,$2,12
	beq	$2,$0,$L300
	move	$6,$0

	jal	init_timer_key
	nop

	lui	$2,%hi(negative_cache_timeout)
	addiu	$2,$2,%lo(negative_cache_timeout)
	addiu	$3,$18,%lo(negative_cache_list_free)
	sw	$2,24($16)
	addiu	$4,$16,44
	sw	$16,28($16)
	slt	$5,$17,64
	sw	$0,44($16)
	lw	$2,4($3)
	sw	$2,48($16)
	lw	$2,4($3)
	sw	$16,0($2)
	lw	$2,8($3)
	sw	$4,4($3)
	addiu	$2,$2,1
	bne	$5,$0,$L295
	sw	$2,8($3)

	li	$3,1			# 0x1
$L186:
	lw	$31,48($sp)
	move	$2,$3
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	negative_fragCache_init
	.section	.text.FastPath_Hash_NAPT_Entry,"ax",@progbits
	.align	2
	.ent	FastPath_Hash_NAPT_Entry
	.type	FastPath_Hash_NAPT_Entry, @function
FastPath_Hash_NAPT_Entry:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	srl	$3,$4,16
	srl	$2,$6,16
	xor	$2,$2,$6
	xor	$3,$3,$4
	xor	$3,$3,$2
	lui	$2,%hi(napt_table_list_max)
	lhu	$4,18($sp)
	xor	$3,$3,$5
	xor	$3,$3,$7
	lw	$2,%lo(napt_table_list_max)($2)
	xor	$3,$3,$4
	srl	$4,$3,12
	xor	$3,$3,$4
	addiu	$2,$2,-1
	j	$31
	and	$2,$2,$3

	.set	macro
	.set	reorder
	.end	FastPath_Hash_NAPT_Entry
	.section	.text.rtk_addArp,"ax",@progbits
	.align	2
	.globl	rtk_addArp
	.ent	rtk_addArp
	.type	rtk_addArp, @function
rtk_addArp:
	.set	nomips16
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
	.section	.text.rtk_modifyArp,"ax",@progbits
	.align	2
	.globl	rtk_modifyArp
	.ent	rtk_modifyArp
	.type	rtk_modifyArp, @function
rtk_modifyArp:
	.set	nomips16
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
	.section	.text.rtk_delArp,"ax",@progbits
	.align	2
	.globl	rtk_delArp
	.ent	rtk_delArp
	.type	rtk_delArp, @function
rtk_delArp:
	.set	nomips16
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
	.section	.text.rtk_addRoute,"ax",@progbits
	.align	2
	.globl	rtk_addRoute
	.ent	rtk_addRoute
	.type	rtk_addRoute, @function
rtk_addRoute:
	.set	nomips16
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
	.section	.text.rtk_modifyRoute,"ax",@progbits
	.align	2
	.globl	rtk_modifyRoute
	.ent	rtk_modifyRoute
	.type	rtk_modifyRoute, @function
rtk_modifyRoute:
	.set	nomips16
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
	.section	.text.rtk_delRoute,"ax",@progbits
	.align	2
	.globl	rtk_delRoute
	.ent	rtk_delRoute
	.type	rtk_delRoute, @function
rtk_delRoute:
	.set	nomips16
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
	.section	.text.rtk_addSession,"ax",@progbits
	.align	2
	.globl	rtk_addSession
	.ent	rtk_addSession
	.type	rtk_addSession, @function
rtk_addSession:
	.set	nomips16
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
	.section	.text.rtk_delSession,"ax",@progbits
	.align	2
	.globl	rtk_delSession
	.ent	rtk_delSession
	.type	rtk_delSession, @function
rtk_delSession:
	.set	nomips16
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
	.section	.text.FastPath_Route,"ax",@progbits
	.align	2
	.globl	FastPath_Route
	.ent	FastPath_Route
	.type	FastPath_Route, @function
FastPath_Route:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(route_list_inuse)
	move	$8,$0
	lw	$3,%lo(route_list_inuse)($2)
	move	$7,$0
$L319:
	beq	$3,$0,$L318
	nop

	lw	$5,8($3)
	sltu	$2,$5,$7
	bne	$2,$0,$L314
	and	$6,$4,$5

	lw	$2,4($3)
	bne	$6,$2,$L314
	nop

	move	$7,$5
	addiu	$8,$3,16
$L314:
	lw	$3,44($3)
	j	$L319
	nop

$L318:
	j	$31
	move	$2,$8

	.set	macro
	.set	reorder
	.end	FastPath_Route
	.section	.text.rtk_addNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_addNaptConnection
	.ent	rtk_addNaptConnection
	.type	rtk_addNaptConnection, @function
rtk_addNaptConnection:
	.set	nomips16
	.frame	$sp,152,$31		# vars= 88, regs= 10/0, args= 24, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-152
	li	$3,-1			# 0xffffffff
	sw	$fp,144($sp)
	move	$fp,$7
	sw	$19,124($sp)
	move	$19,$5
	sw	$18,120($sp)
	move	$18,$4
	sw	$31,148($sp)
	sw	$23,140($sp)
	sw	$22,136($sp)
	sw	$21,132($sp)
	sw	$20,128($sp)
	sw	$17,116($sp)
	sw	$16,112($sp)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L320
	sw	$6,160($sp)
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
	beq	$2,$0,$L320
	li	$3,-1			# 0xffffffff
	.set	macro
	.set	reorder

	lw	$3,0($18)
	li	$2,17			# 0x11
	lw	$4,20($18)
	li	$5,6			# 0x6
	xori	$3,$3,0x2
	movn	$5,$2,$3		#conditional move  
	.set	noreorder
	.set	nomacro
	jal	filter_addconnect
	sw	$5,76($sp)
	.set	macro
	.set	reorder

#APP
	raw_local_irq_save	$23
#NO_APP
	lw	$22,0($18)
	lw	$21,4($18)
	lw	$20,8($18)
	andi	$2,$22,0xffff
	lw	$17,20($18)
	move	$4,$21
	lw	$16,24($18)
	move	$5,$20
	sw	$2,16($sp)
	move	$6,$17
	.set	noreorder
	.set	nomacro
	jal	FastPath_Hash_NAPT_Entry
	move	$7,$16
	.set	macro
	.set	reorder

	lui	$9,%hi(table_napt)
	lw	$3,%lo(table_napt)($9)
	sll	$7,$2,1
	sw	$2,72($sp)
	addu	$2,$7,$2
	lw	$3,0($3)
	sll	$2,$2,2
	addu	$2,$2,$3
	lw	$4,0($2)
$L423:
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L422
	lui	$5,%hi(napt_list_free)
	.set	macro
	.set	reorder

	lw	$2,4($4)
	bne	$2,$22,$L328
	lw	$2,8($4)
	bne	$2,$21,$L328
	lw	$2,12($4)
	bne	$2,$20,$L328
	lw	$3,16($4)
	lw	$2,12($18)
	bne	$3,$2,$L328
	lw	$3,20($4)
	lw	$2,16($18)
	bne	$3,$2,$L328
	lw	$2,24($4)
	bne	$2,$17,$L328
	lw	$2,28($4)
	bne	$2,$16,$L328
	lw	$4,160($sp)
	lw	$5,76($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_conntrack_drop_check_hook
	move	$6,$4
	.set	macro
	.set	reorder

#APP
	raw_local_irq_restore	$23
#NO_APP
	.set	noreorder
	.set	nomacro
	j	$L320
	move	$3,$0
	.set	macro
	.set	reorder

$L328:
	lw	$4,36($4)
	j	$L423
$L422:
	lw	$8,%lo(napt_list_free)($5)
	beq	$8,$0,$L337
	lw	$2,0($18)
	lw	$4,44($8)
	sw	$2,4($8)
	lw	$2,4($18)
	lw	$3,48($8)
	sw	$2,8($8)
	lw	$2,8($18)
	sw	$2,12($8)
	lw	$2,12($18)
	sw	$2,16($8)
	lw	$2,16($18)
	sw	$2,20($8)
	lw	$2,20($18)
	sw	$2,24($8)
	lw	$2,24($18)
	sw	$fp,32($8)
	sw	$2,28($8)
	li	$2,-1
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L339
	sb	$2,0($8)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L340
	sw	$3,48($4)
	.set	macro
	.set	reorder

$L339:
	addiu	$2,$5,%lo(napt_list_free)
	sw	$3,4($2)
$L340:
	lw	$4,48($8)
	addiu	$5,$5,%lo(napt_list_free)
	lw	$2,44($8)
	lui	$3,%hi(napt_list_inuse)
	addiu	$3,$3,%lo(napt_list_inuse)
	addiu	$6,$8,44
	sw	$2,0($4)
	lw	$2,72($sp)
	addu	$4,$7,$2
	addiu	$7,$8,36
	lw	$2,8($5)
	sll	$4,$4,2
	addiu	$2,$2,-1
	sw	$2,8($5)
	sw	$0,44($8)
	lw	$2,4($3)
	sw	$2,48($8)
	lw	$2,4($3)
	sw	$8,0($2)
	lw	$2,8($3)
	sw	$6,4($3)
	addiu	$2,$2,1
	sw	$2,8($3)
	sw	$0,36($8)
	lw	$2,%lo(table_napt)($9)
	lw	$2,0($2)
	addu	$2,$4,$2
	lw	$2,4($2)
	sw	$2,40($8)
	lw	$2,%lo(table_napt)($9)
	lw	$2,0($2)
	addu	$2,$4,$2
	lw	$2,4($2)
	sw	$8,0($2)
	lw	$2,%lo(table_napt)($9)
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$7,4($2)
	lw	$2,%lo(table_napt)($9)
	lw	$2,0($2)
	addu	$4,$4,$2
	lw	$2,8($4)
	addiu	$2,$2,1
	sw	$2,8($4)
	li	$2,33
	sb	$2,57($sp)
	li	$2,8
	sb	$2,64($sp)
	sb	$0,56($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl865x_getLanDev
	sb	$0,65($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl865x_getWanDev
	move	$20,$2
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_protocol
	move	$21,$2
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,100($sp)
	.set	macro
	.set	reorder

	move	$4,$19
	move	$fp,$2
	lbu	$2,0($2)
	andi	$2,$2,0xf
	sll	$2,$2,2
	addu	$2,$fp,$2
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	sw	$2,80($sp)
	.set	macro
	.set	reorder

	li	$6,6			# 0x6
	addiu	$5,$2,6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$sp,24
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	li	$6,6			# 0x6
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$sp,32
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	move	$4,$19
	sw	$2,104($sp)
	lw	$3,12($fp)
	lw	$5,80($sp)
	sw	$3,84($sp)
	lhu	$5,0($5)
	lw	$9,80($sp)
	sw	$5,92($sp)
	lw	$8,16($fp)
	sw	$8,88($sp)
	lhu	$9,2($9)
	.set	noreorder
	.set	nomacro
	jal	rtl_store_skb_dst
	sw	$9,96($sp)
	.set	macro
	.set	reorder

	lui	$8,%hi(path_list_free)
	lw	$5,4($18)
	lw	$6,20($18)
	lw	$4,8($18)
	srl	$2,$5,8
	srl	$3,$6,16
	xor	$2,$2,$5
	xor	$3,$3,$6
	srl	$4,$4,4
	lw	$5,24($18)
	xor	$2,$2,$3
	xor	$2,$2,$4
	lui	$4,%hi(path_table_list_max)
	xor	$2,$2,$5
	lw	$5,76($sp)
	lw	$3,%lo(path_table_list_max)($4)
	xor	$2,$2,$5
	srl	$4,$2,12
	addiu	$3,$3,-1
	xor	$2,$2,$4
	lw	$16,%lo(path_list_free)($8)
	and	$3,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$16,$0,$L344
	sw	$3,72($sp)
	.set	macro
	.set	reorder

	lw	$2,0($18)
	sw	$2,4($16)
	lw	$2,4($18)
	sw	$2,16($16)
	lw	$2,8($18)
	sw	$2,12($16)
	lw	$2,20($18)
	sw	$2,20($16)
	lw	$2,24($18)
	sw	$2,8($16)
	lw	$2,12($18)
	sw	$2,24($16)
	lw	$2,16($18)
	sw	$2,28($16)
	lw	$4,20($18)
	sw	$4,32($16)
	lw	$2,24($18)
	.set	noreorder
	.set	nomacro
	jal	FastPath_Route
	sw	$2,36($16)
	.set	macro
	.set	reorder

	sw	$2,44($16)
	li	$2,1
	sb	$2,2($16)
	li	$2,-1
	sb	$2,0($16)
	lui	$2,%hi(rtl_init_bypass_packet_hook)
	sb	$0,1($16)
	lw	$2,%lo(rtl_init_bypass_packet_hook)($2)
	beq	$2,$0,$L345
	.set	noreorder
	.set	nomacro
	jal	$2
	move	$4,$16
	.set	macro
	.set	reorder

$L345:
	lw	$3,16($16)
	lw	$2,24($16)
	beq	$3,$2,$L346
	lbu	$2,1($16)
	ori	$2,$2,0x1
	sb	$2,1($16)
$L346:
	lw	$3,12($16)
	lw	$2,28($16)
	beq	$3,$2,$L347
	lbu	$2,1($16)
	ori	$2,$2,0x2
	sb	$2,1($16)
$L347:
	sw	$0,48($16)
	addiu	$22,$sp,40
	sw	$0,52($16)
	move	$4,$22
	lw	$2,4($18)
	move	$5,$0
	li	$6,14			# 0xe
	sw	$2,12($fp)
	lw	$2,8($18)
	lw	$9,80($sp)
	sh	$2,0($9)
	lw	$2,20($18)
	sw	$2,16($fp)
	lw	$2,24($18)
	.set	noreorder
	.set	nomacro
	jal	memset
	sh	$2,2($9)
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$5,$0
	.set	macro
	.set	reorder

	beq	$20,$0,$L348
	lw	$4,4($18)
	move	$5,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$6,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L348
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,56
	.set	macro
	.set	reorder

	move	$17,$2
	li	$2,1			# 0x1
	bne	$17,$2,$L349
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	move	$4,$19
	.set	macro
	.set	reorder

	bne	$2,$17,$L349
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$2,$2,$3
	li	$3,2048			# 0x800
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L349
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffea
	.set	macro
	.set	reorder

$L349:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	move	$5,$22
	addiu	$4,$2,6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dmac
	move	$5,$20
	.set	macro
	.set	reorder

	li	$2,2048			# 0x800
	lw	$3,100($sp)
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
	beq	$3,$2,$L352
	move	$4,$19
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

$L352:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	beq	$2,$0,$L354
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L427
	sw	$2,48($16)
	.set	macro
	.set	reorder

$L354:
	lw	$2,48($16)
	sw	$2,48($16)
$L427:
	lui	$2,%hi(nf_hooks+160)
	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L357
	move	$4,$19
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

$L357:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	beq	$2,$0,$L359
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L348
	sw	$2,52($16)
	.set	macro
	.set	reorder

$L359:
	lw	$2,52($16)
	sw	$2,52($16)
$L348:
	lw	$2,12($18)
	addiu	$22,$sp,40
	move	$4,$22
	move	$5,$0
	sw	$2,12($fp)
	li	$6,14			# 0xe
	lw	$2,16($18)
	lw	$8,80($sp)
	sh	$2,0($8)
	lw	$2,20($18)
	sw	$2,16($fp)
	lw	$2,24($18)
	.set	noreorder
	.set	nomacro
	jal	memset
	sh	$2,2($8)
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$21,$0,$L361
	move	$5,$21
	.set	macro
	.set	reorder

	lw	$4,20($18)
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$6,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L361
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,56
	.set	macro
	.set	reorder

	move	$17,$2
	li	$2,1			# 0x1
	bne	$17,$2,$L362
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	move	$4,$19
	.set	macro
	.set	reorder

	bne	$2,$17,$L362
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$2,$2,$3
	li	$3,2048			# 0x800
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L362
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffea
	.set	macro
	.set	reorder

$L362:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	move	$5,$22
	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_smac
	move	$5,$21
	.set	macro
	.set	reorder

	li	$2,2048			# 0x800
	lw	$9,100($sp)
	.set	noreorder
	.set	nomacro
	bne	$9,$2,$L361
	lui	$2,%hi(nf_hooks+160)
	.set	macro
	.set	reorder

	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L365
	move	$4,$19
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

$L365:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	beq	$2,$0,$L367
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L361
	sw	$2,52($16)
	.set	macro
	.set	reorder

$L367:
	lw	$2,52($16)
	sw	$2,52($16)
$L361:
	lw	$3,76($sp)
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L369
	lui	$2,%hi(nf_ct_udp_timeout_stream)
	.set	macro
	.set	reorder

	lw	$4,160($sp)
	jal	rtl_tcp_get_timeouts
	.set	noreorder
	.set	nomacro
	j	$L428
	sw	$2,60($16)
	.set	macro
	.set	reorder

$L369:
	lw	$2,%lo(nf_ct_udp_timeout_stream)($2)
	sw	$2,60($16)
$L428:
	lui	$2,%hi(jiffies)
	lw	$4,160($sp)
	lw	$3,80($16)
	sw	$4,56($16)
	lw	$2,%lo(jiffies)($2)
	lw	$4,84($16)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L372
	sw	$2,40($16)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L373
	sw	$4,84($3)
	.set	macro
	.set	reorder

$L372:
	lui	$5,%hi(path_list_free)
	addiu	$2,$5,%lo(path_list_free)
	sw	$4,4($2)
$L373:
	lw	$3,84($16)
	lui	$5,%hi(path_list_free)
	lw	$2,80($16)
	addiu	$5,$5,%lo(path_list_free)
	addiu	$7,$16,80
	lui	$6,%hi(table_path)
	sw	$2,0($3)
	lui	$3,%hi(path_list_inuse)
	lw	$2,8($5)
	addiu	$3,$3,%lo(path_list_inuse)
	lw	$8,72($sp)
	addiu	$2,$2,-1
	sw	$2,8($5)
	sll	$4,$8,1
	lw	$9,72($sp)
	addiu	$8,$16,72
	sw	$0,80($16)
	lw	$2,4($3)
	addu	$4,$4,$9
	sll	$4,$4,2
	sw	$2,84($16)
	lw	$2,4($3)
	sw	$16,0($2)
	lw	$2,8($3)
	sw	$7,4($3)
	addiu	$2,$2,1
	sw	$2,8($3)
	sw	$0,72($16)
	lw	$2,%lo(table_path)($6)
	lw	$2,0($2)
	addu	$2,$4,$2
	lw	$2,4($2)
	sw	$2,76($16)
	lw	$2,%lo(table_path)($6)
	lw	$2,0($2)
	addu	$2,$4,$2
	lw	$2,4($2)
	sw	$16,0($2)
	lw	$2,%lo(table_path)($6)
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$8,4($2)
	lw	$2,%lo(table_path)($6)
	lw	$2,0($2)
	addu	$4,$4,$2
	lw	$2,8($4)
	addiu	$2,$2,1
	sw	$2,8($4)
$L344:
	lw	$5,12($18)
	lui	$8,%hi(path_list_free)
	lw	$6,20($18)
	lw	$4,24($18)
	srl	$3,$5,16
	srl	$2,$6,8
	xor	$3,$3,$5
	xor	$2,$2,$6
	srl	$4,$4,4
	lw	$5,16($18)
	xor	$2,$2,$3
	xor	$2,$2,$4
	lui	$4,%hi(path_table_list_max)
	xor	$2,$2,$5
	lw	$5,76($sp)
	lw	$3,%lo(path_table_list_max)($4)
	xor	$2,$2,$5
	srl	$4,$2,12
	addiu	$3,$3,-1
	xor	$2,$2,$4
	lw	$16,%lo(path_list_free)($8)
	and	$3,$3,$2
	.set	noreorder
	.set	nomacro
	beq	$16,$0,$L377
	sw	$3,72($sp)
	.set	macro
	.set	reorder

	lw	$2,0($18)
	sw	$2,4($16)
	lw	$2,20($18)
	sw	$2,16($16)
	lw	$2,24($18)
	sw	$2,12($16)
	lw	$2,12($18)
	sw	$2,20($16)
	lw	$2,16($18)
	sw	$2,8($16)
	lw	$2,20($18)
	sw	$2,24($16)
	lw	$2,24($18)
	sw	$2,28($16)
	lw	$4,4($18)
	sw	$4,32($16)
	lw	$2,8($18)
	.set	noreorder
	.set	nomacro
	jal	FastPath_Route
	sw	$2,36($16)
	.set	macro
	.set	reorder

	sw	$2,44($16)
	li	$2,2
	sb	$2,2($16)
	li	$2,-1
	sb	$2,0($16)
	lui	$2,%hi(rtl_init_bypass_packet_hook)
	sb	$0,1($16)
	lw	$2,%lo(rtl_init_bypass_packet_hook)($2)
	beq	$2,$0,$L378
	.set	noreorder
	.set	nomacro
	jal	$2
	move	$4,$16
	.set	macro
	.set	reorder

$L378:
	lw	$3,20($16)
	lw	$2,32($16)
	beq	$3,$2,$L379
	lbu	$2,1($16)
	ori	$2,$2,0x4
	sb	$2,1($16)
$L379:
	lw	$3,8($16)
	lw	$2,36($16)
	beq	$3,$2,$L380
	lbu	$2,1($16)
	ori	$2,$2,0x8
	sb	$2,1($16)
$L380:
	sw	$0,48($16)
	addiu	$22,$sp,40
	sw	$0,52($16)
	move	$4,$22
	lw	$2,20($18)
	move	$5,$0
	li	$6,14			# 0xe
	sw	$2,12($fp)
	lw	$2,24($18)
	lw	$9,80($sp)
	sh	$2,0($9)
	lw	$2,12($18)
	sw	$2,16($fp)
	lw	$2,16($18)
	.set	noreorder
	.set	nomacro
	jal	memset
	sh	$2,2($9)
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$5,$0
	.set	macro
	.set	reorder

	beq	$21,$0,$L381
	lw	$4,20($18)
	move	$5,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$6,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L381
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,56
	.set	macro
	.set	reorder

	move	$17,$2
	li	$2,1			# 0x1
	bne	$17,$2,$L382
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	move	$4,$19
	.set	macro
	.set	reorder

	bne	$2,$17,$L382
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$2,$2,$3
	li	$3,2048			# 0x800
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L382
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffea
	.set	macro
	.set	reorder

$L382:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	move	$5,$22
	addiu	$4,$2,6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dmac
	move	$5,$21
	.set	macro
	.set	reorder

	li	$2,2048			# 0x800
	lw	$3,100($sp)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L383
	lui	$2,%hi(nf_hooks+128)
	.set	macro
	.set	reorder

	lw	$3,%lo(nf_hooks+128)($2)
	addiu	$2,$2,%lo(nf_hooks+128)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L383
	move	$4,$19
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

$L383:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	beq	$2,$0,$L387
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L381
	sw	$2,48($16)
	.set	macro
	.set	reorder

$L387:
	lw	$2,48($16)
	sw	$2,48($16)
$L381:
	lw	$2,20($18)
	addiu	$22,$sp,40
	move	$4,$22
	move	$5,$0
	sw	$2,12($fp)
	li	$6,14			# 0xe
	lw	$2,24($18)
	lw	$8,80($sp)
	sh	$2,0($8)
	lw	$2,4($18)
	sw	$2,16($fp)
	lw	$2,8($18)
	.set	noreorder
	.set	nomacro
	jal	memset
	sh	$2,2($8)
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L389
	move	$5,$20
	.set	macro
	.set	reorder

	lw	$4,4($18)
	.set	noreorder
	.set	nomacro
	jal	rtl_arp_req_get_ha
	move	$6,$22
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L389
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_ppp_proto_check
	addiu	$5,$sp,56
	.set	macro
	.set	reorder

	move	$17,$2
	li	$2,1			# 0x1
	bne	$17,$2,$L390
	.set	noreorder
	.set	nomacro
	jal	rtl_skb_mac_header_was_set
	move	$4,$19
	.set	macro
	.set	reorder

	bne	$2,$17,$L390
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	lbu	$3,12($2)
	lbu	$2,13($2)
	sll	$3,$3,8
	or	$2,$2,$3
	li	$3,2048			# 0x800
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L390
	move	$4,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_skb_set_mac_header
	li	$5,-22			# 0xffffffea
	.set	macro
	.set	reorder

$L390:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	move	$5,$22
	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_smac
	move	$5,$20
	.set	macro
	.set	reorder

	li	$2,2048			# 0x800
	lw	$9,100($sp)
	.set	noreorder
	.set	nomacro
	bne	$9,$2,$L389
	lui	$2,%hi(nf_hooks+160)
	.set	macro
	.set	reorder

	lw	$3,%lo(nf_hooks+160)($2)
	addiu	$2,$2,%lo(nf_hooks+160)
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L389
	move	$4,$19
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

$L389:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	sw	$2,52($16)
	li	$2,6			# 0x6
	lw	$3,76($sp)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L395
	lui	$2,%hi(nf_ct_udp_timeout_stream)
	.set	macro
	.set	reorder

	lw	$4,160($sp)
	jal	rtl_tcp_get_timeouts
	.set	noreorder
	.set	nomacro
	j	$L429
	sw	$2,60($16)
	.set	macro
	.set	reorder

$L395:
	lw	$2,%lo(nf_ct_udp_timeout_stream)($2)
	sw	$2,60($16)
$L429:
	lui	$2,%hi(jiffies)
	lw	$4,160($sp)
	lw	$3,80($16)
	sw	$4,56($16)
	lw	$2,%lo(jiffies)($2)
	lw	$4,84($16)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L398
	sw	$2,40($16)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L399
	sw	$4,84($3)
	.set	macro
	.set	reorder

$L398:
	lui	$5,%hi(path_list_free)
	addiu	$2,$5,%lo(path_list_free)
	sw	$4,4($2)
$L399:
	lw	$3,84($16)
	lui	$5,%hi(path_list_free)
	lw	$2,80($16)
	addiu	$5,$5,%lo(path_list_free)
	addiu	$7,$16,80
	lui	$6,%hi(table_path)
	sw	$2,0($3)
	lui	$3,%hi(path_list_inuse)
	lw	$2,8($5)
	addiu	$3,$3,%lo(path_list_inuse)
	lw	$8,72($sp)
	addiu	$2,$2,-1
	sw	$2,8($5)
	sll	$4,$8,1
	lw	$9,72($sp)
	addiu	$8,$16,72
	sw	$0,80($16)
	lw	$2,4($3)
	addu	$4,$4,$9
	sll	$4,$4,2
	sw	$2,84($16)
	lw	$2,4($3)
	sw	$16,0($2)
	lw	$2,8($3)
	sw	$7,4($3)
	addiu	$2,$2,1
	sw	$2,8($3)
	sw	$0,72($16)
	lw	$2,%lo(table_path)($6)
	lw	$2,0($2)
	addu	$2,$4,$2
	lw	$2,4($2)
	sw	$2,76($16)
	lw	$2,%lo(table_path)($6)
	lw	$2,0($2)
	addu	$2,$4,$2
	lw	$2,4($2)
	sw	$16,0($2)
	lw	$2,%lo(table_path)($6)
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$8,4($2)
	lw	$2,%lo(table_path)($6)
	lw	$2,0($2)
	addu	$4,$4,$2
	lw	$2,8($4)
	addiu	$2,$2,1
	sw	$2,8($4)
$L377:
	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	addiu	$5,$sp,24
	li	$6,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$2,6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_eth_hdr
	move	$4,$19
	.set	macro
	.set	reorder

	addiu	$5,$sp,32
	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,6			# 0x6
	.set	macro
	.set	reorder

	lw	$5,104($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_mark
	move	$4,$19
	.set	macro
	.set	reorder

	move	$4,$19
	lw	$2,84($sp)
	sw	$2,12($fp)
	lw	$5,92($sp)
	lw	$3,80($sp)
	sh	$5,0($3)
	lw	$8,88($sp)
	sw	$8,16($fp)
	lw	$9,96($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dst
	sh	$9,2($3)
	.set	macro
	.set	reorder

	beq	$20,$0,$L402
#APP
	   

	1:	ll	    $2, 452($20)		# atomic_sub		
       nop                                             
	subu	$2, 1					
	sc	    $2, 452($20)					
	beqz	$2, 2f					
	.subsection 2					
2:	b	1b					
	.previous					

	   

#NO_APP
$L402:
	beq	$21,$0,$L405
#APP
	   

	1:	ll	    $2, 452($21)		# atomic_sub		
       nop                                             
	subu	$2, 1					
	sc	    $2, 452($21)					
	beqz	$2, 2f					
	.subsection 2					
2:	b	1b					
	.previous					

	   

#NO_APP
	j	$L405
$L337:
#APP
	raw_local_irq_restore	$23
#NO_APP
	.set	noreorder
	.set	nomacro
	j	$L320
	li	$3,-1			# 0xffffffff
	.set	macro
	.set	reorder

$L405:
#APP
	raw_local_irq_restore	$23
#NO_APP
	lw	$4,160($sp)
	lw	$5,76($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_conntrack_drop_check_hook
	move	$6,$4
	.set	macro
	.set	reorder

	move	$3,$0
$L320:
	lw	$31,148($sp)
	move	$2,$3
	lw	$fp,144($sp)
	lw	$23,140($sp)
	lw	$22,136($sp)
	lw	$21,132($sp)
	lw	$20,128($sp)
	lw	$19,124($sp)
	lw	$18,120($sp)
	lw	$17,116($sp)
	lw	$16,112($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,152
	.set	macro
	.set	reorder

	.end	rtk_addNaptConnection
	.section	.text.rtk_delNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_delNaptConnection
	.ent	rtk_delNaptConnection
	.type	rtk_delNaptConnection, @function
rtk_delNaptConnection:
	.set	nomips16
	.frame	$sp,64,$31		# vars= 0, regs= 9/0, args= 24, gp= 0
	.mask	0x80ff0000,-8
	.fmask	0x00000000,0
	addiu	$sp,$sp,-64
	li	$3,17			# 0x11
	sw	$23,52($sp)
	li	$23,6			# 0x6
	sw	$16,24($sp)
	move	$16,$4
	sw	$31,56($sp)
	sw	$22,48($sp)
	sw	$21,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	lw	$2,0($4)
	xori	$2,$2,0x2
	movn	$23,$3,$2		#conditional move  
#APP
	raw_local_irq_save	$17
#NO_APP
	lw	$4,20($4)
	jal	filter_delconnect
	lw	$22,0($16)
	lw	$21,4($16)
	lw	$20,8($16)
	andi	$2,$22,0xffff
	lw	$19,20($16)
	move	$4,$21
	lw	$18,24($16)
	move	$5,$20
	move	$6,$19
	sw	$2,16($sp)
	.set	noreorder
	.set	nomacro
	jal	FastPath_Hash_NAPT_Entry
	move	$7,$18
	.set	macro
	.set	reorder

	lui	$10,%hi(table_napt)
	lw	$3,%lo(table_napt)($10)
	sll	$8,$2,1
	move	$6,$2
	addu	$2,$8,$2
	lw	$3,0($3)
	sll	$2,$2,2
	addu	$2,$2,$3
	lw	$5,0($2)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L485
	lui	$2,%hi(napt_list_inuse)
	.set	macro
	.set	reorder

	lui	$24,%hi(path_list_inuse)
	addiu	$9,$2,%lo(napt_list_inuse)
	lui	$2,%hi(napt_list_free)
	addiu	$7,$2,%lo(napt_list_free)
$L477:
	lw	$2,4($5)
	bne	$2,$22,$L436
	lw	$2,8($5)
	bne	$2,$21,$L436
	lw	$2,12($5)
	bne	$2,$20,$L436
	lw	$3,16($5)
	lw	$2,12($16)
	bne	$3,$2,$L436
	lw	$3,20($5)
	lw	$2,16($16)
	bne	$3,$2,$L436
	lw	$2,24($5)
	bne	$2,$19,$L436
	lw	$2,28($5)
	bne	$2,$18,$L436
	lw	$2,36($5)
	sb	$0,0($5)
	lw	$4,40($5)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L439
	addu	$3,$8,$6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L440
	sw	$4,40($2)
	.set	macro
	.set	reorder

$L439:
	lw	$2,%lo(table_napt)($10)
	sll	$3,$3,2
	lw	$2,0($2)
	addu	$3,$3,$2
	sw	$4,4($3)
$L440:
	lw	$2,36($5)
	sll	$3,$6,1
	lw	$4,40($5)
	addu	$3,$3,$6
	sll	$3,$3,2
	sw	$2,0($4)
	lui	$2,%hi(table_napt)
	lw	$2,%lo(table_napt)($2)
	lw	$2,0($2)
	addu	$3,$3,$2
	lw	$2,8($3)
	addiu	$2,$2,-1
	sw	$2,8($3)
	lw	$2,44($5)
	lw	$3,48($5)
	beq	$2,$0,$L442
	.set	noreorder
	.set	nomacro
	j	$L443
	sw	$3,48($2)
	.set	macro
	.set	reorder

$L442:
	sw	$3,4($9)
$L443:
	lw	$3,48($5)
	addiu	$4,$5,44
	lw	$2,44($5)
	lui	$18,%hi(path_table_list_max)
	lui	$12,%hi(table_path)
	sw	$2,0($3)
	lw	$2,8($9)
	addiu	$2,$2,-1
	sw	$2,8($9)
	sw	$0,44($5)
	lw	$2,4($7)
	sw	$2,48($5)
	lw	$2,4($7)
	sw	$5,0($2)
	lw	$2,8($7)
	sw	$4,4($7)
	addiu	$2,$2,1
	sw	$2,8($7)
	lw	$8,4($16)
	lw	$7,20($16)
	lw	$11,8($16)
	srl	$2,$8,8
	srl	$3,$7,16
	xor	$2,$2,$8
	xor	$3,$3,$7
	srl	$4,$11,4
	xor	$2,$2,$3
	lw	$10,24($16)
	xor	$2,$2,$4
	lw	$4,%lo(path_table_list_max)($18)
	xor	$2,$2,$10
	xor	$2,$2,$23
	addiu	$4,$4,-1
	srl	$3,$2,12
	xor	$2,$2,$3
	and	$6,$4,$2
	lw	$3,%lo(table_path)($12)
	sll	$2,$6,1
	addu	$2,$2,$6
	lw	$3,0($3)
	sll	$2,$2,2
	addu	$3,$2,$3
	lw	$5,0($3)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L447
	move	$4,$2
	.set	macro
	.set	reorder

	lui	$2,%hi(path_list_free)
	addiu	$6,$2,%lo(path_list_free)
	move	$13,$4
	addiu	$9,$24,%lo(path_list_inuse)
	li	$14,1			# 0x1
	move	$15,$12
$L457:
	lw	$3,4($5)
	lw	$2,0($16)
	bne	$3,$2,$L448
	lbu	$2,2($5)
	bne	$2,$14,$L448
	lw	$2,16($5)
	bne	$2,$8,$L448
	lw	$2,12($5)
	bne	$2,$11,$L448
	lw	$3,24($5)
	lw	$2,12($16)
	bne	$3,$2,$L448
	lw	$3,28($5)
	lw	$2,16($16)
	bne	$3,$2,$L448
	lw	$2,20($5)
	bne	$2,$7,$L448
	lw	$2,8($5)
	bne	$2,$10,$L448
	lw	$2,72($5)
	sb	$0,0($5)
	lw	$3,76($5)
	beq	$2,$0,$L451
	.set	noreorder
	.set	nomacro
	j	$L452
	sw	$3,76($2)
	.set	macro
	.set	reorder

$L451:
	lw	$2,%lo(table_path)($12)
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$3,4($2)
$L452:
	lw	$3,76($5)
	lw	$2,72($5)
	sw	$2,0($3)
	lw	$2,%lo(table_path)($15)
	lw	$3,0($2)
	addu	$3,$13,$3
	lw	$2,8($3)
	addiu	$2,$2,-1
	sw	$2,8($3)
	lw	$3,80($5)
	lw	$2,84($5)
	beq	$3,$0,$L454
	.set	noreorder
	.set	nomacro
	j	$L455
	sw	$2,84($3)
	.set	macro
	.set	reorder

$L454:
	sw	$2,4($9)
$L455:
	lw	$2,80($5)
	addiu	$4,$5,80
	lw	$3,84($5)
	sw	$2,0($3)
	lw	$2,8($9)
	addiu	$2,$2,-1
	sw	$2,8($9)
	sw	$0,80($5)
	lw	$2,4($6)
	sw	$2,84($5)
	lw	$2,4($6)
	sw	$5,0($2)
	lw	$2,8($6)
	sw	$4,4($6)
	addiu	$2,$2,1
	.set	noreorder
	.set	nomacro
	j	$L447
	sw	$2,8($6)
	.set	macro
	.set	reorder

$L448:
	lw	$5,72($5)
	bne	$5,$0,$L457
$L447:
	lw	$8,20($16)
	lui	$10,%hi(table_path)
	lw	$7,12($16)
	lw	$12,24($16)
	srl	$2,$8,8
	srl	$3,$7,16
	xor	$2,$2,$8
	xor	$3,$3,$7
	srl	$4,$12,4
	xor	$2,$2,$3
	lw	$11,16($16)
	xor	$2,$2,$4
	lw	$4,%lo(path_table_list_max)($18)
	xor	$2,$2,$11
	xor	$2,$2,$23
	addiu	$4,$4,-1
	srl	$3,$2,12
	xor	$2,$2,$3
	and	$6,$4,$2
	lw	$3,%lo(table_path)($10)
	sll	$2,$6,1
	addu	$2,$2,$6
	lw	$3,0($3)
	sll	$2,$2,2
	addu	$3,$2,$3
	lw	$5,0($3)
	beq	$5,$0,$L460
	move	$4,$2
	lui	$2,%hi(path_list_free)
	addiu	$9,$24,%lo(path_list_inuse)
	addiu	$6,$2,%lo(path_list_free)
	move	$13,$4
	li	$14,2			# 0x2
	move	$15,$10
$L470:
	lw	$3,4($5)
	lw	$2,0($16)
	bne	$3,$2,$L461
	lbu	$2,2($5)
	bne	$2,$14,$L461
	lw	$2,20($5)
	bne	$2,$7,$L461
	lw	$2,8($5)
	bne	$2,$11,$L461
	lw	$2,24($5)
	bne	$2,$8,$L461
	lw	$2,28($5)
	bne	$2,$12,$L461
	lw	$3,32($5)
	lw	$2,4($16)
	bne	$3,$2,$L461
	lw	$3,36($5)
	lw	$2,8($16)
	bne	$3,$2,$L461
	lw	$2,72($5)
	sb	$0,0($5)
	lw	$3,76($5)
	beq	$2,$0,$L464
	.set	noreorder
	.set	nomacro
	j	$L465
	sw	$3,76($2)
	.set	macro
	.set	reorder

$L464:
	lw	$2,%lo(table_path)($10)
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$3,4($2)
$L465:
	lw	$3,76($5)
	lw	$2,72($5)
	sw	$2,0($3)
	lw	$2,%lo(table_path)($15)
	lw	$3,0($2)
	addu	$3,$13,$3
	lw	$2,8($3)
	addiu	$2,$2,-1
	sw	$2,8($3)
	lw	$3,80($5)
	lw	$2,84($5)
	beq	$3,$0,$L467
	.set	noreorder
	.set	nomacro
	j	$L468
	sw	$2,84($3)
	.set	macro
	.set	reorder

$L467:
	sw	$2,4($9)
$L468:
	lw	$2,80($5)
	addiu	$4,$5,80
	lw	$3,84($5)
	sw	$2,0($3)
	lw	$2,8($9)
	addiu	$2,$2,-1
	sw	$2,8($9)
	sw	$0,80($5)
	lw	$2,4($6)
	sw	$2,84($5)
	lw	$2,4($6)
	sw	$5,0($2)
	lw	$2,8($6)
	sw	$4,4($6)
	addiu	$2,$2,1
	.set	noreorder
	.set	nomacro
	j	$L460
	sw	$2,8($6)
	.set	macro
	.set	reorder

$L461:
	lw	$5,72($5)
	bne	$5,$0,$L470
$L460:
#APP
	raw_local_irq_restore	$17
#NO_APP
	.set	noreorder
	.set	nomacro
	j	$L430
	move	$2,$0
	.set	macro
	.set	reorder

$L436:
	lw	$5,36($5)
	bne	$5,$0,$L477
$L485:
#APP
	raw_local_irq_restore	$17
#NO_APP
	li	$2,-4			# 0xfffffffc
$L430:
	lw	$31,56($sp)
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

	.end	rtk_delNaptConnection
	.section	.text.rtk_idleNaptConnection,"ax",@progbits
	.align	2
	.globl	rtk_idleNaptConnection
	.ent	rtk_idleNaptConnection
	.type	rtk_idleNaptConnection, @function
rtk_idleNaptConnection:
	.set	nomips16
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	move	$8,$4
	sw	$16,16($sp)
	li	$13,6			# 0x6
	sw	$31,20($sp)
	lui	$16,%hi(path_table_list_max)
	lw	$10,4($4)
	lui	$25,%hi(table_path)
	lw	$9,20($4)
	move	$15,$5
	lw	$14,8($4)
	srl	$2,$10,8
	srl	$3,$9,16
	xor	$2,$2,$10
	xor	$3,$3,$9
	srl	$4,$14,4
	lw	$12,0($8)
	xor	$2,$2,$3
	lw	$11,24($8)
	xor	$2,$2,$4
	li	$3,17			# 0x11
	xori	$4,$12,0x2
	movn	$13,$3,$4		#conditional move  
	xor	$2,$2,$11
	xor	$2,$2,$13
	lw	$4,%lo(path_table_list_max)($16)
	srl	$3,$2,12
	xor	$2,$2,$3
	addiu	$4,$4,-1
	and	$4,$4,$2
	lw	$3,%lo(table_path)($25)
	sll	$2,$4,1
	addu	$2,$2,$4
	lw	$4,0($3)
	sll	$2,$2,2
	lui	$3,%hi(jiffies)
	addu	$2,$2,$4
	lw	$24,%lo(jiffies)($3)
	lw	$6,0($2)
$L507:
	beq	$6,$0,$L493
	li	$2,-16777216			# 0xff000000

	lw	$3,4($6)
	ori	$4,$2,0xff00
	bne	$3,$12,$L494
	ori	$7,$2,0x100

	lw	$2,0($6)
	and	$2,$2,$4
	bne	$2,$7,$L494
	nop

	lw	$2,16($6)
	bne	$2,$10,$L494
	nop

	lw	$2,12($6)
	bne	$2,$14,$L494
	nop

	lw	$3,24($6)
	lw	$2,12($8)
	bne	$3,$2,$L494
	nop

	lw	$3,28($6)
	lw	$2,16($8)
	bne	$3,$2,$L494
	nop

	lw	$2,20($6)
	bne	$2,$9,$L494
	nop

	lw	$2,8($6)
	bne	$2,$11,$L494
	subu	$2,$24,$15

	lw	$5,40($6)
	subu	$2,$2,$5
	bgez	$2,$L493
	nop

	j	$L509
	nop

$L494:
	lw	$6,72($6)
	j	$L507
	nop

$L493:
	lw	$11,20($8)
	lw	$10,12($8)
	lw	$14,24($8)
	srl	$2,$11,8
	srl	$3,$10,16
	xor	$2,$2,$11
	xor	$3,$3,$10
	srl	$4,$14,4
	xor	$2,$2,$3
	lw	$12,16($8)
	xor	$2,$2,$4
	lw	$4,%lo(path_table_list_max)($16)
	xor	$2,$2,$12
	xor	$2,$2,$13
	addiu	$4,$4,-1
	srl	$3,$2,12
	xor	$2,$2,$3
	and	$4,$4,$2
	lw	$3,%lo(table_path)($25)
	sll	$2,$4,1
	addu	$2,$2,$4
	lw	$3,0($3)
	sll	$2,$2,2
	addu	$2,$2,$3
	lw	$6,0($2)
$L508:
	beq	$6,$0,$L500
	li	$2,-16777216			# 0xff000000

	lw	$4,4($6)
	lw	$3,0($8)
	ori	$9,$2,0xff00
	bne	$4,$3,$L501
	ori	$7,$2,0x200

	lw	$2,0($6)
	and	$2,$2,$9
	bne	$2,$7,$L501
	nop

	lw	$2,20($6)
	bne	$2,$10,$L501
	nop

	lw	$2,8($6)
	bne	$2,$12,$L501
	nop

	lw	$2,24($6)
	bne	$2,$11,$L501
	nop

	lw	$2,28($6)
	bne	$2,$14,$L501
	nop

	lw	$3,32($6)
	lw	$2,4($8)
	bne	$3,$2,$L501
	nop

	lw	$3,36($6)
	lw	$2,8($8)
	bne	$3,$2,$L501
	subu	$2,$24,$15

	lw	$5,40($6)
	subu	$2,$2,$5
	bgez	$2,$L500
	nop

$L509:
	lw	$4,56($6)
	jal	rtl_set_ct_timeout_expires
	addu	$5,$5,$15

	j	$L488
	li	$2,-1			# 0xffffffff

$L501:
	lw	$6,72($6)
	j	$L508
	nop

$L500:
	move	$2,$0
$L488:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	rtk_idleNaptConnection
	.section	.text.rtl_fpTimer_update,"ax",@progbits
	.align	2
	.globl	rtl_fpTimer_update
	.ent	rtl_fpTimer_update
	.type	rtl_fpTimer_update, @function
rtl_fpTimer_update:
	.set	nomips16
	.frame	$sp,72,$31		# vars= 32, regs= 6/0, args= 16, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-72
	lui	$2,%hi(jiffies)
	sw	$17,52($sp)
	move	$5,$0
	sw	$31,68($sp)
	move	$17,$4
	sw	$20,64($sp)
	sw	$19,60($sp)
	sw	$18,56($sp)
	sw	$16,48($sp)
	lw	$20,%lo(jiffies)($2)
	jal	rtl_get_ct_protonum
	nop

	li	$3,17			# 0x11
	bne	$2,$3,$L511
	move	$4,$17

	jal	rtl_get_ct_udp_status
	li	$19,1			# 0x1

	andi	$2,$2,0x2
	beq	$2,$0,$L512
	lui	$2,%hi(nf_ct_udp_timeout)

	lui	$2,%hi(nf_ct_udp_timeout_stream)
	lw	$18,%lo(nf_ct_udp_timeout_stream)($2)
	j	$L526
	move	$4,$17

$L512:
	lw	$18,%lo(nf_ct_udp_timeout)($2)
	j	$L526
	move	$4,$17

$L511:
	jal	rtl_get_ct_protonum
	move	$5,$0

	li	$3,6			# 0x6
	bne	$2,$3,$L510
	li	$2,-1			# 0xffffffff

	jal	rtl_get_ct_tcp_state
	move	$4,$17

	sltu	$2,$2,6
	beq	$2,$0,$L510
	li	$2,-1			# 0xffffffff

	move	$4,$17
	jal	rtl_tcp_get_timeouts
	li	$19,2			# 0x2

	move	$18,$2
	move	$4,$17
$L526:
	move	$5,$0
	jal	rtl_get_ct_ip_by_dir
	move	$6,$0

	move	$4,$17
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$16,$2

	bne	$16,$2,$L517
	move	$4,$17

	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$19,16($sp)

	move	$4,$17
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,20($sp)

	move	$4,$17
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	sw	$2,24($sp)

	move	$4,$17
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_get_ct_port_by_dir
	sw	$2,28($sp)

	move	$4,$17
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$2,32($sp)

	move	$4,$17
	j	$L525
	move	$5,$0

$L517:
	move	$5,$0
	jal	rtl_get_ct_ip_by_dir
	li	$6,1			# 0x1

	move	$4,$17
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$16,$2

	bne	$16,$2,$L510
	li	$2,-1			# 0xffffffff

	move	$4,$17
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$19,16($sp)

	move	$4,$17
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,20($sp)

	move	$4,$17
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	sw	$2,24($sp)

	move	$4,$17
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_port_by_dir
	sw	$2,28($sp)

	move	$4,$17
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	sw	$2,32($sp)

	move	$4,$17
	li	$5,1			# 0x1
$L525:
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	sw	$2,36($sp)

	addiu	$4,$sp,16
	move	$5,$18
	jal	rtk_idleNaptConnection
	sw	$2,40($sp)

	beq	$2,$0,$L522
	addu	$5,$20,$18

	jal	rtl_check_for_acc
	move	$4,$17

	j	$L510
	move	$2,$0

$L522:
	li	$2,-1			# 0xffffffff
$L510:
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
	.section	.text.rtl_br_fdb_time_update,"ax",@progbits
	.align	2
	.globl	rtl_br_fdb_time_update
	.ent	rtl_br_fdb_time_update
	.type	rtl_br_fdb_time_update, @function
rtl_br_fdb_time_update:
	.set	nomips16
	.frame	$sp,40,$31		# vars= 8, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$17,28($sp)
	move	$17,$4
	sw	$16,24($sp)
	move	$4,$0
	move	$16,$5
	move	$5,$6
	sw	$31,32($sp)
	jal	rtl_get_hw_fdb_age
	li	$6,2			# 0x2

	move	$3,$2
	sltu	$5,$2,301
	li	$2,300			# 0x12c
	beq	$3,$2,$L530
	move	$4,$16

	beq	$5,$0,$L533
	li	$2,450			# 0x1c2

	li	$2,150			# 0x96
	beq	$3,$2,$L531
	lui	$2,%hi(jiffies)

	j	$L528
	nop

$L533:
	bne	$3,$2,$L528
	move	$4,$16

	lui	$2,%hi(jiffies)
	lw	$5,%lo(jiffies)($2)
	j	$L535
	nop

$L530:
	lui	$2,%hi(jiffies)
	lw	$5,%lo(jiffies)($2)
	j	$L535
	addiu	$5,$5,-15000

$L531:
	lw	$5,%lo(jiffies)($2)
	addiu	$5,$5,-30000
$L535:
	jal	rtl_set_fdb_aging
	nop

$L528:
	jal	rtl_get_fdb_aging
	move	$4,$16

	move	$4,$17
	jal	rtl_hold_time
	move	$16,$2

	lui	$3,%hi(jiffies)
	lw	$3,%lo(jiffies)($3)
	addu	$16,$16,$2
	lw	$31,32($sp)
	li	$2,-1			# 0xffffffff
	subu	$3,$3,$16
	slt	$3,$3,0
	lw	$17,28($sp)
	movn	$2,$0,$3		#conditional move
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	rtl_br_fdb_time_update
	.section	.text.mark_path_invalid,"ax",@progbits
	.align	2
	.ent	mark_path_invalid
	.type	mark_path_invalid, @function
mark_path_invalid:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	srl	$2,$4,8
	srl	$3,$6,16
	xor	$2,$2,$4
	xor	$3,$3,$6
	andi	$5,$5,0xffff
	xor	$2,$2,$3
	move	$11,$4
	srl	$4,$5,4
	xor	$2,$2,$4
	andi	$7,$7,0xffff
	lhu	$10,18($sp)
	lui	$12,%hi(path_table_list_max)
	xor	$2,$2,$7
	lui	$13,%hi(table_path)
	xor	$2,$2,$10
	lw	$3,%lo(path_table_list_max)($12)
	srl	$4,$2,12
	xor	$2,$2,$4
	addiu	$3,$3,-1
	and	$4,$3,$2
	lw	$3,%lo(table_path)($13)
	sll	$2,$4,1
	addu	$2,$2,$4
	lw	$3,0($3)
	sll	$2,$2,2
	addu	$2,$2,$3
	lw	$4,0($2)
	beq	$4,$0,$L559
	nop

	li	$3,255			# 0xff
$L544:
	lw	$2,12($4)
	bne	$2,$5,$L540
	nop

	lw	$2,8($4)
	bne	$2,$7,$L540
	nop

	lw	$2,16($4)
	bne	$2,$11,$L540
	nop

	lw	$2,20($4)
	bne	$2,$6,$L540
	nop

	lbu	$2,0($4)
	bne	$2,$3,$L540
	li	$2,1			# 0x1

	lbu	$3,2($4)
	beq	$3,$2,$L557
	sb	$0,0($4)

	lw	$8,32($4)
	lhu	$9,38($4)
	j	$L539
	nop

$L540:
	lw	$4,72($4)
	bne	$4,$0,$L544
	nop

$L560:
	j	$31
	nop

$L539:
	beq	$8,$0,$L560
	li	$2,1			# 0x1

	lbu	$3,2($4)
	bne	$3,$2,$L546
	srl	$3,$11,16

	srl	$2,$6,8
	srl	$3,$8,16
	xor	$3,$3,$8
	xor	$2,$2,$6
	xor	$2,$2,$3
	srl	$3,$7,4
	xor	$2,$2,$3
	j	$L558
	xor	$2,$2,$9

$L557:
	lw	$8,24($4)
	lhu	$9,30($4)
	j	$L539
	nop

$L546:
	srl	$2,$8,8
	xor	$3,$3,$11
	xor	$2,$2,$8
	xor	$2,$2,$3
	srl	$3,$9,4
	xor	$2,$2,$3
	xor	$2,$2,$5
$L558:
	xor	$2,$2,$10
	lw	$3,%lo(path_table_list_max)($12)
	srl	$4,$2,12
	xor	$2,$2,$4
	addiu	$3,$3,-1
	and	$4,$3,$2
	lw	$2,%lo(table_path)($13)
	sll	$3,$4,1
	addu	$3,$3,$4
	lw	$2,0($2)
	sll	$3,$3,2
	addu	$3,$3,$2
	lw	$4,0($3)
	beq	$4,$0,$L559
	nop

	li	$3,255			# 0xff
$L554:
	lw	$2,24($4)
	bne	$2,$6,$L552
	nop

	lw	$2,28($4)
	bne	$2,$7,$L552
	nop

	lw	$2,32($4)
	bne	$2,$11,$L552
	nop

	lw	$2,36($4)
	bne	$2,$5,$L552
	nop

	lbu	$2,0($4)
	bne	$2,$3,$L552
	nop

	sb	$0,0($4)
$L552:
	lw	$4,72($4)
	bne	$4,$0,$L554
	nop

$L559:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	mark_path_invalid
	.section	.text.rtl_fp_mark_invalid,"ax",@progbits
	.align	2
	.globl	rtl_fp_mark_invalid
	.ent	rtl_fp_mark_invalid
	.type	rtl_fp_mark_invalid, @function
rtl_fp_mark_invalid:
	.set	nomips16
	.frame	$sp,72,$31		# vars= 8, regs= 10/0, args= 24, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-72
	li	$5,1			# 0x1
	sw	$16,32($sp)
	move	$6,$0
	move	$16,$4
	sw	$31,68($sp)
	sw	$fp,64($sp)
	sw	$23,60($sp)
	sw	$22,56($sp)
	sw	$21,52($sp)
	sw	$20,48($sp)
	sw	$19,44($sp)
	sw	$18,40($sp)
	jal	rtl_get_ct_ip_by_dir
	sw	$17,36($sp)

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	sw	$2,24($sp)

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_ip_by_dir
	move	$fp,$2

	move	$4,$16
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_get_ct_ip_by_dir
	move	$23,$2

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	move	$22,$2

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_get_ct_port_by_dir
	move	$21,$2

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_get_ct_port_by_dir
	move	$20,$2

	li	$6,1			# 0x1
	move	$4,$16
	move	$5,$0
	jal	rtl_get_ct_port_by_dir
	move	$19,$2

	move	$4,$16
	li	$5,1			# 0x1
	jal	rtl_get_ct_protonum
	move	$18,$2

	move	$4,$16
	move	$5,$0
	jal	rtl_get_ct_protonum
	move	$17,$2

	move	$5,$21
	lw	$4,24($sp)
	move	$6,$fp
	move	$7,$20
	move	$16,$2
	jal	mark_path_invalid
	sw	$17,16($sp)

	move	$4,$23
	move	$5,$19
	move	$6,$22
	move	$7,$18
	jal	mark_path_invalid
	sw	$16,16($sp)

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
	.rdata
	.align	2
$LC0:
	.ascii	"lo\000"
	.section	.text.enter_fast_path,"ax",@progbits
	.align	2
	.globl	enter_fast_path
	.ent	enter_fast_path
	.type	enter_fast_path, @function
enter_fast_path:
	.set	nomips16
	.frame	$sp,88,$31		# vars= 24, regs= 10/0, args= 24, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-88
	li	$2,1			# 0x1
	sw	$31,84($sp)
	sw	$22,72($sp)
	move	$22,$0
	sw	$18,56($sp)
	sw	$16,48($sp)
	sw	$4,24($sp)
	sw	$fp,80($sp)
	sw	$23,76($sp)
	sw	$21,68($sp)
	sw	$20,64($sp)
	sw	$19,60($sp)
	sw	$17,52($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_dev_name
	sw	$2,36($sp)
	.set	macro
	.set	reorder

	lw	$4,24($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$16,$2
	.set	macro
	.set	reorder

	move	$4,$0
	sw	$0,40($sp)
	move	$18,$2
	lbu	$2,8($2)
	sltu	$2,$2,2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L562
	sw	$0,44($sp)
	.set	macro
	.set	reorder

	lbu	$21,9($18)
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$21,$2,$L564
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L562
$L564:
	lbu	$2,0($18)
	move	$4,$18
	lw	$6,24($sp)
	andi	$2,$2,0xf
	sll	$2,$2,2
	addu	$23,$18,$2
	.set	noreorder
	.set	nomacro
	jal	fast_path_pre_process_check
	move	$5,$23
	.set	macro
	.set	reorder

	move	$5,$2
	move	$4,$2
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$5,$2,$L562
	lui	$3,%hi($LC0)
	.set	macro
	.set	reorder

	addiu	$2,$3,%lo($LC0)
#APP
	.set	noreorder
	.set	noat
	lbu	$3,($16)
1:	lbu	$1,($2)
	addiu	$16,1
	bne	$1,$3,2f
	addiu	$2,1
	bnez	$3,1b
	lbu	$3,($16)
	move	$3,$1
2:	subu	$3,$1
3:	.set	at
	.set	reorder
#NO_APP
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L562
	move	$4,$0
	.set	macro
	.set	reorder

	lhu	$4,0($23)
	lhu	$6,6($18)
	lw	$19,12($18)
	lw	$20,16($18)
	andi	$3,$6,0xffff
	sw	$4,28($sp)
	lhu	$2,2($23)
	.set	noreorder
	.set	nomacro
	bne	$21,$5,$L576
	sw	$2,32($sp)
	.set	macro
	.set	reorder

	andi	$2,$3,0x3fff
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L572
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

	xori	$2,$3,0x2000
	sltu	$4,$2,1
$L572:
	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	bne	$4,$2,$L760
	andi	$2,$6,0x3fff
	.set	macro
	.set	reorder

	lw	$5,12($23)
	li	$2,65536			# 0x10000
	and	$2,$5,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L574
	li	$2,393216			# 0x60000
	.set	macro
	.set	reorder

	lw	$5,28($sp)
	move	$4,$19
	lw	$7,32($sp)
	move	$6,$20
	.set	noreorder
	.set	nomacro
	jal	mark_path_invalid
	sw	$21,16($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L562
	move	$4,$0
	.set	macro
	.set	reorder

$L574:
	and	$2,$5,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L562
	move	$4,$0
	.set	macro
	.set	reorder

$L576:
	andi	$2,$6,0x3fff
$L760:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L577
	li	$2,8192			# 0x2000
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L578
	andi	$2,$3,0x2000
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	.set	noreorder
	.set	nomacro
	j	$L579
	sw	$3,40($sp)
	.set	macro
	.set	reorder

$L578:
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L579
	andi	$3,$3,0x1fff
	.set	macro
	.set	reorder

	lw	$4,44($sp)
	li	$2,1			# 0x1
	movn	$4,$2,$3		#conditional move
	sw	$4,44($sp)
$L579:
	lhu	$6,4($18)
	xor	$3,$19,$20
	srl	$2,$3,16
	lui	$9,%hi(max_udp_frag_entry)
	andi	$8,$6,0xffff
	lui	$10,%hi(negative_cache_table)
	xor	$2,$2,$8
	lbu	$7,9($18)
	xor	$3,$3,$2
	srl	$2,$3,8
	andi	$5,$7,0x00ff
	lw	$4,%lo(max_udp_frag_entry)($9)
	xor	$2,$2,$5
	xor	$3,$3,$2
	addiu	$4,$4,-1
	and	$3,$3,$4
	sll	$2,$3,1
	addu	$2,$2,$3
	lw	$3,%lo(negative_cache_table)($10)
	sll	$2,$2,2
	addu	$2,$2,$3
	lw	$3,0($2)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L588
	li	$4,3			# 0x3
	.set	macro
	.set	reorder

$L587:
	lhu	$2,2($3)
	bne	$2,$8,$L584
	lw	$2,4($3)
	bne	$2,$19,$L584
	lw	$2,8($3)
	bne	$2,$20,$L584
	lbu	$2,1($3)
	bne	$2,$5,$L584
	lbu	$2,0($3)
	beq	$2,$4,$L741
$L584:
	lw	$3,36($3)
	bne	$3,$0,$L587
	.set	noreorder
	.set	nomacro
	j	$L759
	xor	$3,$19,$20
	.set	macro
	.set	reorder

$L741:
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L562
	move	$4,$0
	.set	macro
	.set	reorder

$L588:
	xor	$3,$19,$20
$L759:
	andi	$8,$6,0xffff
	srl	$2,$3,16
	andi	$5,$7,0x00ff
	xor	$2,$2,$8
	xor	$3,$3,$2
	srl	$2,$3,8
	lw	$4,%lo(max_udp_frag_entry)($9)
	xor	$2,$2,$5
	xor	$3,$3,$2
	lui	$2,%hi(udp_cache_table)
	addiu	$4,$4,-1
	and	$3,$3,$4
	lw	$4,%lo(udp_cache_table)($2)
	sll	$2,$3,1
	addu	$2,$2,$3
	lw	$3,0($4)
	sll	$2,$2,2
	addu	$2,$2,$3
	lw	$4,0($2)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L738
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

$L596:
	lhu	$2,2($4)
	bne	$2,$8,$L593
	lw	$2,4($4)
	bne	$2,$19,$L593
	lw	$2,8($4)
	bne	$2,$20,$L593
	lbu	$2,1($4)
	bne	$2,$5,$L593
	lbu	$2,0($4)
	beq	$2,$3,$L742
$L593:
	lw	$4,40($4)
	bne	$4,$0,$L596
$L738:
	.set	noreorder
	.set	nomacro
	j	$L597
	move	$22,$0
	.set	macro
	.set	reorder

$L742:
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L597
	move	$22,$4
	.set	macro
	.set	reorder

	lhu	$2,12($4)
	sw	$2,28($sp)
	lhu	$3,14($4)
	lw	$4,40($sp)
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L577
	sw	$3,32($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L577
	sw	$0,36($sp)
	.set	macro
	.set	reorder

$L597:
	lw	$2,40($sp)
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L577
	xor	$3,$19,$20
	.set	macro
	.set	reorder

	andi	$6,$6,0xffff
	srl	$2,$3,16
	andi	$8,$7,0x00ff
	xor	$2,$2,$6
	lui	$5,%hi(negative_cache_list_free)
	xor	$3,$3,$2
	lw	$4,%lo(max_udp_frag_entry)($9)
	srl	$2,$3,8
	xor	$2,$2,$8
	lw	$7,%lo(negative_cache_list_free)($5)
	xor	$3,$3,$2
	addiu	$4,$4,-1
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L754
	and	$9,$3,$4
	.set	macro
	.set	reorder

	lw	$4,44($7)
	li	$2,3
	sh	$6,2($7)
	sw	$19,4($7)
	sw	$20,8($7)
	sb	$8,1($7)
	sb	$2,0($7)
	lw	$3,48($7)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L605
	addiu	$2,$5,%lo(negative_cache_list_free)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L606
	sw	$3,48($4)
	.set	macro
	.set	reorder

$L605:
	sw	$3,4($2)
$L606:
	lw	$4,48($7)
	addiu	$5,$5,%lo(negative_cache_list_free)
	lw	$2,44($7)
	sll	$3,$9,1
	addu	$3,$3,$9
	addiu	$6,$7,36
	sw	$2,0($4)
	sll	$3,$3,2
	lw	$2,8($5)
	addiu	$4,$7,12
	addiu	$2,$2,-1
	sw	$2,8($5)
	sw	$0,36($7)
	lw	$2,%lo(negative_cache_table)($10)
	addu	$2,$3,$2
	lw	$2,4($2)
	sw	$2,40($7)
	lw	$2,%lo(negative_cache_table)($10)
	addu	$2,$3,$2
	lw	$2,4($2)
	sw	$7,0($2)
	lw	$2,%lo(negative_cache_table)($10)
	addu	$2,$3,$2
	sw	$6,4($2)
	lw	$2,%lo(negative_cache_table)($10)
	addu	$3,$3,$2
	lw	$2,8($3)
	addiu	$2,$2,1
	sw	$2,8($3)
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	addiu	$2,$2,1000
	.set	noreorder
	.set	nomacro
	jal	add_timer
	sw	$2,20($7)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L562
	move	$4,$0
	.set	macro
	.set	reorder

$L577:
	lw	$4,28($sp)
	srl	$2,$19,8
	srl	$fp,$20,16
	xor	$2,$2,$19
	xor	$3,$fp,$20
	xor	$2,$2,$3
	srl	$3,$4,4
	xor	$2,$2,$3
	lw	$3,32($sp)
	xor	$2,$2,$3
	lui	$3,%hi(path_table_list_max)
	xor	$2,$2,$21
	lw	$4,%lo(path_table_list_max)($3)
	srl	$3,$2,12
	xor	$2,$2,$3
	addiu	$4,$4,-1
	and	$4,$4,$2
	lui	$2,%hi(table_path)
	lw	$3,%lo(table_path)($2)
	sll	$2,$4,1
	addu	$2,$2,$4
	lw	$3,0($3)
	sll	$2,$2,2
	addu	$2,$2,$3
	lw	$16,0($2)
$L749:
	beq	$16,$0,$L740
	lw	$2,12($16)
	lw	$4,28($sp)
	bne	$2,$4,$L611
	lw	$2,8($16)
	lw	$3,32($sp)
	bne	$2,$3,$L611
	lw	$2,16($16)
	bne	$2,$19,$L611
	lw	$2,20($16)
	.set	noreorder
	.set	nomacro
	bne	$2,$20,$L611
	li	$2,255			# 0xff
	.set	macro
	.set	reorder

	lbu	$3,0($16)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L611
	li	$2,2			# 0x2
	.set	macro
	.set	reorder

	lw	$3,4($16)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L761
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$21,$2,$L613
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L761:
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L611
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L611
$L613:
	lui	$2,%hi(rtl_check_bypass_packet_hook)
	lw	$2,%lo(rtl_check_bypass_packet_hook)($2)
	beq	$2,$0,$L615
	.set	noreorder
	.set	nomacro
	jal	$2
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L562
	move	$4,$0
	.set	macro
	.set	reorder

$L615:
	lw	$4,24($sp)
	lw	$5,32($16)
	lbu	$7,1($18)
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_route_input
	move	$6,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L562
	move	$4,$0
	.set	macro
	.set	reorder

	lw	$4,24($sp)
	jal	rtl_skb_dst_check
	li	$3,-1			# 0xffffffff
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L745
	li	$2,6			# 0x6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$21,$2,$L620
	addiu	$17,$23,16
	.set	macro
	.set	reorder

	lhu	$3,6($23)
	addiu	$2,$23,6
	move	$17,$0
	movn	$17,$2,$3		#conditional move  
$L620:
	lw	$4,40($sp)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L623
	xor	$3,$19,$20
	.set	macro
	.set	reorder

	lui	$2,%hi(max_udp_frag_entry)
	lhu	$11,4($18)
	srl	$4,$3,16
	lw	$5,%lo(max_udp_frag_entry)($2)
	andi	$9,$21,0x00ff
	xor	$4,$4,$11
	lui	$6,%hi(udp_cache_list_free)
	xor	$3,$3,$4
	addiu	$5,$5,-1
	lw	$4,28($sp)
	srl	$2,$3,8
	xor	$2,$2,$9
	andi	$10,$4,0xffff
	xor	$3,$3,$2
	lw	$4,32($sp)
	and	$12,$3,$5
	lw	$8,%lo(udp_cache_list_free)($6)
	.set	noreorder
	.set	nomacro
	beq	$8,$0,$L745
	andi	$7,$4,0xffff
	.set	macro
	.set	reorder

	lw	$4,48($8)
	li	$2,1
	sh	$11,2($8)
	sb	$9,1($8)
	sh	$10,12($8)
	sh	$7,14($8)
	sb	$2,0($8)
	sw	$19,4($8)
	sw	$20,8($8)
	lw	$3,52($8)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L629
	addiu	$2,$6,%lo(udp_cache_list_free)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L630
	sw	$3,52($4)
	.set	macro
	.set	reorder

$L629:
	sw	$3,4($2)
$L630:
	lw	$4,52($8)
	addiu	$6,$6,%lo(udp_cache_list_free)
	lw	$2,48($8)
	lui	$5,%hi(udp_cache_table)
	sll	$3,$12,1
	addiu	$7,$8,40
	sw	$2,0($4)
	addu	$3,$3,$12
	lw	$2,8($6)
	sll	$3,$3,2
	addiu	$4,$8,16
	addiu	$2,$2,-1
	sw	$2,8($6)
	sw	$0,40($8)
	lw	$2,%lo(udp_cache_table)($5)
	lw	$2,0($2)
	addu	$2,$3,$2
	lw	$2,4($2)
	sw	$2,44($8)
	lw	$2,%lo(udp_cache_table)($5)
	lw	$2,0($2)
	addu	$2,$3,$2
	lw	$2,4($2)
	sw	$8,0($2)
	lw	$2,%lo(udp_cache_table)($5)
	lw	$2,0($2)
	addu	$2,$3,$2
	sw	$7,4($2)
	lw	$2,%lo(udp_cache_table)($5)
	lw	$2,0($2)
	addu	$3,$3,$2
	lw	$2,8($3)
	addiu	$2,$2,1
	sw	$2,8($3)
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	addiu	$2,$2,1000
	.set	noreorder
	.set	nomacro
	jal	add_timer
	sw	$2,24($8)
	.set	macro
	.set	reorder

	j	$L632
$L623:
	lw	$2,44($sp)
	beq	$2,$0,$L632
	.set	noreorder
	.set	nomacro
	jal	del_timer
	addiu	$4,$22,16
	.set	macro
	.set	reorder

	lw	$2,8($22)
	lw	$3,4($22)
	lhu	$4,2($22)
	xor	$3,$3,$2
	srl	$2,$3,16
	xor	$2,$2,$4
	xor	$3,$3,$2
	lui	$2,%hi(max_udp_frag_entry)
	lbu	$5,1($22)
	srl	$4,$3,8
	lw	$2,%lo(max_udp_frag_entry)($2)
	xor	$4,$4,$5
	lw	$6,40($22)
	xor	$3,$3,$4
	addiu	$2,$2,-1
	sb	$0,0($22)
	lw	$4,44($22)
	.set	noreorder
	.set	nomacro
	beq	$6,$0,$L636
	and	$5,$3,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L637
	sw	$4,44($6)
	.set	macro
	.set	reorder

$L636:
	lui	$3,%hi(udp_cache_table)
	sll	$2,$5,1
	lw	$3,%lo(udp_cache_table)($3)
	addu	$2,$2,$5
	sll	$2,$2,2
	lw	$3,0($3)
	addu	$2,$2,$3
	sw	$4,4($2)
$L637:
	lw	$4,44($22)
	sll	$3,$5,1
	lw	$2,40($22)
	addu	$3,$3,$5
	sll	$3,$3,2
	addiu	$5,$22,48
	sw	$2,0($4)
	lui	$2,%hi(udp_cache_table)
	lw	$2,%lo(udp_cache_table)($2)
	lui	$4,%hi(udp_cache_list_free)
	addiu	$4,$4,%lo(udp_cache_list_free)
	lw	$2,0($2)
	addu	$3,$3,$2
	lw	$2,8($3)
	addiu	$2,$2,-1
	sw	$2,8($3)
	sw	$0,48($22)
	lw	$2,4($4)
	sw	$2,52($22)
	lw	$2,4($4)
	sw	$22,0($2)
	lw	$2,8($4)
	sw	$5,4($4)
	addiu	$2,$2,1
	sw	$2,8($4)
$L632:
	lw	$4,24($sp)
	move	$5,$18
	.set	noreorder
	.set	nomacro
	jal	fast_path_before_nat_check
	move	$6,$21
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L562
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$21,$2,$L642
	li	$3,1048576			# 0x100000
	.set	macro
	.set	reorder

	lw	$2,12($23)
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L642
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$3,2($18)
	bne	$3,$2,$L642
	lw	$4,24($sp)
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

$L642:
	lbu	$3,1($16)
	sltu	$2,$3,13
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L706
	sll	$2,$3,2
	.set	macro
	.set	reorder

	lui	$3,%hi($L729)
	addiu	$3,$3,%lo($L729)
	addu	$2,$2,$3
	lw	$2,0($2)
	lhu	$4,10($18)
	j	$2
	.rdata
	.align	2
$L729:
	.word	$L644
	.word	$L646
	.word	$L662
	.word	$L662
	.word	$L677
	.word	$L706
	.word	$L706
	.word	$L706
	.word	$L692
	.word	$L706
	.word	$L706
	.word	$L706
	.word	$L692
	.section	.text.enter_fast_path
$L646:
	lw	$3,24($16)
	srl	$2,$19,16
	andi	$6,$19,0xffff
	andi	$4,$4,0xffff
	addu	$6,$6,$2
	andi	$2,$3,0xffff
	subu	$6,$6,$2
	srl	$3,$3,16
	subu	$6,$6,$3
	addu	$6,$6,$4
	.set	noreorder
	.set	nomacro
	bgez	$6,$L648
	andi	$2,$6,0xffff
	.set	macro
	.set	reorder

	subu	$6,$0,$6
	sra	$2,$6,16
	andi	$3,$6,0xffff
	addu	$6,$2,$3
	sra	$2,$6,16
	addu	$6,$6,$2
	nor	$2,$0,$6
	.set	noreorder
	.set	nomacro
	j	$L647
	sh	$2,10($18)
	.set	macro
	.set	reorder

$L648:
	sra	$3,$6,16
	addu	$6,$3,$2
	sra	$2,$6,16
	addu	$6,$6,$2
	sh	$6,10($18)
$L647:
	lw	$3,36($sp)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L666
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L651
	beq	$17,$0,$L666
	lhu	$6,0($17)
	beq	$6,$0,$L666
	lw	$2,12($18)
	lw	$3,24($16)
	srl	$4,$2,16
	andi	$5,$2,0xffff
	addu	$5,$5,$4
	andi	$2,$3,0xffff
	subu	$5,$5,$2
	srl	$3,$3,16
	subu	$5,$5,$3
	addu	$5,$5,$6
	.set	noreorder
	.set	nomacro
	bgez	$5,$L654
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	sra	$2,$5,16
	andi	$3,$5,0xffff
	addu	$5,$2,$3
	sra	$2,$5,16
	addu	$5,$5,$2
	nor	$2,$0,$5
	.set	noreorder
	.set	nomacro
	j	$L666
	sh	$2,0($17)
	.set	macro
	.set	reorder

$L654:
	sra	$3,$5,16
	addu	$5,$3,$2
	sra	$2,$5,16
	addu	$5,$5,$2
	.set	noreorder
	.set	nomacro
	j	$L666
	sh	$5,0($17)
	.set	macro
	.set	reorder

$L651:
	lw	$2,12($18)
	lw	$3,24($16)
	srl	$4,$2,16
	andi	$5,$2,0xffff
	addu	$5,$5,$4
	andi	$2,$3,0xffff
	subu	$5,$5,$2
	srl	$3,$3,16
	lhu	$2,0($17)
	subu	$5,$5,$3
	addu	$5,$5,$2
	.set	noreorder
	.set	nomacro
	bgez	$5,$L658
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	sra	$2,$5,16
	andi	$3,$5,0xffff
	addu	$5,$2,$3
	sra	$2,$5,16
	addu	$5,$5,$2
	nor	$2,$0,$5
	.set	noreorder
	.set	nomacro
	j	$L657
	sh	$2,0($17)
	.set	macro
	.set	reorder

$L658:
	sra	$3,$5,16
	addu	$5,$3,$2
	sra	$2,$5,16
	addu	$5,$5,$2
	sh	$5,0($17)
$L657:
	lw	$2,12($23)
	li	$3,1048576			# 0x100000
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L666
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$3,2($18)
	bne	$3,$2,$L666
	lw	$4,24($sp)
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

	j	$L666
$L662:
	lw	$3,24($16)
	srl	$2,$19,16
	andi	$6,$19,0xffff
	andi	$4,$4,0xffff
	addu	$6,$6,$2
	andi	$2,$3,0xffff
	subu	$6,$6,$2
	srl	$3,$3,16
	subu	$6,$6,$3
	addu	$6,$6,$4
	.set	noreorder
	.set	nomacro
	bgez	$6,$L664
	andi	$2,$6,0xffff
	.set	macro
	.set	reorder

	subu	$6,$0,$6
	sra	$2,$6,16
	andi	$3,$6,0xffff
	addu	$6,$2,$3
	sra	$2,$6,16
	addu	$6,$6,$2
	nor	$2,$0,$6
	.set	noreorder
	.set	nomacro
	j	$L663
	sh	$2,10($18)
	.set	macro
	.set	reorder

$L664:
	sra	$3,$6,16
	addu	$6,$3,$2
	sra	$2,$6,16
	addu	$6,$6,$2
	sh	$6,10($18)
$L663:
	lw	$4,36($sp)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L666
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L667
	beq	$17,$0,$L672
	lhu	$6,0($17)
	beq	$6,$0,$L672
	lw	$2,12($18)
	lw	$3,24($16)
	srl	$4,$2,16
	andi	$5,$2,0xffff
	addu	$5,$5,$4
	andi	$2,$3,0xffff
	subu	$5,$5,$2
	srl	$3,$3,16
	lhu	$4,0($23)
	subu	$5,$5,$3
	lw	$2,28($16)
	addu	$5,$5,$4
	subu	$5,$5,$2
	addu	$5,$5,$6
	.set	noreorder
	.set	nomacro
	bgez	$5,$L670
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	sra	$2,$5,16
	andi	$3,$5,0xffff
	addu	$5,$2,$3
	sra	$2,$5,16
	addu	$5,$5,$2
	nor	$2,$0,$5
	.set	noreorder
	.set	nomacro
	j	$L672
	sh	$2,0($17)
	.set	macro
	.set	reorder

$L670:
	sra	$3,$5,16
	addu	$5,$3,$2
	sra	$2,$5,16
	addu	$5,$5,$2
	.set	noreorder
	.set	nomacro
	j	$L672
	sh	$5,0($17)
	.set	macro
	.set	reorder

$L667:
	lw	$2,12($18)
	lw	$3,24($16)
	srl	$4,$2,16
	andi	$5,$2,0xffff
	addu	$5,$5,$4
	andi	$2,$3,0xffff
	subu	$5,$5,$2
	srl	$3,$3,16
	lhu	$2,0($23)
	subu	$5,$5,$3
	lw	$3,28($16)
	addu	$5,$5,$2
	lhu	$2,0($17)
	subu	$5,$5,$3
	addu	$5,$5,$2
	.set	noreorder
	.set	nomacro
	bgez	$5,$L674
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	sra	$2,$5,16
	andi	$3,$5,0xffff
	addu	$5,$2,$3
	sra	$2,$5,16
	addu	$5,$5,$2
	nor	$2,$0,$5
	.set	noreorder
	.set	nomacro
	j	$L673
	sh	$2,0($17)
	.set	macro
	.set	reorder

$L674:
	sra	$3,$5,16
	addu	$5,$3,$2
	sra	$2,$5,16
	addu	$5,$5,$2
	sh	$5,0($17)
$L673:
	lw	$2,12($23)
	li	$3,1048576			# 0x100000
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L672
	li	$2,40			# 0x28
	.set	macro
	.set	reorder

	lhu	$3,2($18)
	bne	$3,$2,$L672
	lw	$4,24($sp)
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	li	$5,40			# 0x28
	.set	macro
	.set	reorder

$L672:
	lw	$2,28($16)
	sh	$2,0($23)
$L666:
	lw	$2,24($16)
	.set	noreorder
	.set	nomacro
	j	$L644
	sw	$2,12($18)
	.set	macro
	.set	reorder

$L677:
	lw	$2,32($16)
	andi	$6,$20,0xffff
	addu	$5,$6,$fp
	andi	$4,$4,0xffff
	andi	$3,$2,0xffff
	srl	$2,$2,16
	subu	$5,$5,$3
	subu	$5,$5,$2
	addu	$5,$5,$4
	.set	noreorder
	.set	nomacro
	bgez	$5,$L679
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	sra	$2,$5,16
	andi	$3,$5,0xffff
	addu	$5,$2,$3
	sra	$2,$5,16
	addu	$5,$5,$2
	nor	$2,$0,$5
	.set	noreorder
	.set	nomacro
	j	$L678
	sh	$2,10($18)
	.set	macro
	.set	reorder

$L679:
	sra	$3,$5,16
	addu	$5,$3,$2
	sra	$2,$5,16
	addu	$5,$5,$2
	sh	$5,10($18)
$L678:
	lw	$2,36($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L753
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L682
	beq	$17,$0,$L753
	lhu	$4,0($17)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L753
	addu	$6,$6,$fp
	.set	macro
	.set	reorder

	lw	$2,32($16)
	.set	noreorder
	.set	nomacro
	j	$L762
	andi	$3,$2,0xffff
	.set	macro
	.set	reorder

$L682:
	lw	$2,32($16)
	addu	$6,$6,$fp
	lhu	$4,0($17)
	andi	$3,$2,0xffff
$L762:
	srl	$2,$2,16
	subu	$6,$6,$3
	subu	$6,$6,$2
	addu	$6,$6,$4
	.set	noreorder
	.set	nomacro
	bgez	$6,$L689
	andi	$2,$6,0xffff
	.set	macro
	.set	reorder

	subu	$6,$0,$6
	sra	$2,$6,16
	andi	$3,$6,0xffff
	addu	$6,$2,$3
	sra	$2,$6,16
	addu	$6,$6,$2
	nor	$2,$0,$6
	.set	noreorder
	.set	nomacro
	j	$L753
	sh	$2,0($17)
	.set	macro
	.set	reorder

$L689:
	sra	$3,$6,16
	addu	$6,$3,$2
	sra	$2,$6,16
	addu	$6,$6,$2
	.set	noreorder
	.set	nomacro
	j	$L753
	sh	$6,0($17)
	.set	macro
	.set	reorder

$L692:
	lw	$2,32($16)
	andi	$6,$20,0xffff
	addu	$5,$6,$fp
	andi	$4,$4,0xffff
	andi	$3,$2,0xffff
	srl	$2,$2,16
	subu	$5,$5,$3
	subu	$5,$5,$2
	addu	$5,$5,$4
	.set	noreorder
	.set	nomacro
	bgez	$5,$L694
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	sra	$2,$5,16
	andi	$3,$5,0xffff
	addu	$5,$2,$3
	sra	$2,$5,16
	addu	$5,$5,$2
	nor	$2,$0,$5
	.set	noreorder
	.set	nomacro
	j	$L693
	sh	$2,10($18)
	.set	macro
	.set	reorder

$L694:
	sra	$3,$5,16
	addu	$5,$3,$2
	sra	$2,$5,16
	addu	$5,$5,$2
	sh	$5,10($18)
$L693:
	lw	$3,36($sp)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L753
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L697
	beq	$17,$0,$L702
	lhu	$5,0($17)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L702
	addu	$6,$6,$fp
	.set	macro
	.set	reorder

	lw	$2,32($16)
	lhu	$4,2($23)
	andi	$3,$2,0xffff
	srl	$2,$2,16
	subu	$6,$6,$3
	subu	$6,$6,$2
	lw	$2,36($16)
	addu	$6,$6,$4
	subu	$6,$6,$2
	.set	noreorder
	.set	nomacro
	j	$L756
	addu	$6,$6,$5
	.set	macro
	.set	reorder

$L697:
	lw	$2,32($16)
	addu	$6,$6,$fp
	lhu	$4,2($23)
	andi	$3,$2,0xffff
	srl	$2,$2,16
	subu	$6,$6,$3
	subu	$6,$6,$2
	lw	$3,36($16)
	addu	$6,$6,$4
	lhu	$2,0($17)
	subu	$6,$6,$3
	addu	$6,$6,$2
$L756:
	.set	noreorder
	.set	nomacro
	bgez	$6,$L704
	andi	$2,$6,0xffff
	.set	macro
	.set	reorder

	subu	$6,$0,$6
	sra	$2,$6,16
	andi	$3,$6,0xffff
	addu	$6,$2,$3
	sra	$2,$6,16
	addu	$6,$6,$2
	nor	$2,$0,$6
	.set	noreorder
	.set	nomacro
	j	$L702
	sh	$2,0($17)
	.set	macro
	.set	reorder

$L704:
	sra	$3,$6,16
	addu	$6,$3,$2
	sra	$2,$6,16
	addu	$6,$6,$2
	sh	$6,0($17)
$L702:
	lw	$2,36($16)
	.set	noreorder
	.set	nomacro
	j	$L753
	sh	$2,2($23)
	.set	macro
	.set	reorder

$L706:
	lw	$2,24($16)
	srl	$6,$19,16
	andi	$4,$19,0xffff
	andi	$3,$2,0xffff
	addu	$5,$4,$6
	subu	$5,$5,$3
	srl	$2,$2,16
	lhu	$3,10($18)
	subu	$5,$5,$2
	addu	$5,$5,$3
	.set	noreorder
	.set	nomacro
	bgez	$5,$L708
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	sra	$2,$5,16
	andi	$3,$5,0xffff
	addu	$5,$2,$3
	sra	$2,$5,16
	addu	$5,$5,$2
	nor	$2,$0,$5
	.set	noreorder
	.set	nomacro
	j	$L707
	sh	$2,10($18)
	.set	macro
	.set	reorder

$L708:
	sra	$3,$5,16
	addu	$5,$3,$2
	sra	$2,$5,16
	addu	$5,$5,$2
	sh	$5,10($18)
$L707:
	lw	$2,32($16)
	andi	$7,$20,0xffff
	addu	$5,$7,$fp
	andi	$3,$2,0xffff
	srl	$2,$2,16
	subu	$5,$5,$3
	lhu	$3,10($18)
	subu	$5,$5,$2
	addu	$5,$5,$3
	.set	noreorder
	.set	nomacro
	bgez	$5,$L711
	andi	$2,$5,0xffff
	.set	macro
	.set	reorder

	subu	$5,$0,$5
	sra	$2,$5,16
	andi	$3,$5,0xffff
	addu	$5,$2,$3
	sra	$2,$5,16
	addu	$5,$5,$2
	nor	$2,$0,$5
	.set	noreorder
	.set	nomacro
	j	$L710
	sh	$2,10($18)
	.set	macro
	.set	reorder

$L711:
	sra	$3,$5,16
	addu	$5,$3,$2
	sra	$2,$5,16
	addu	$5,$5,$2
	sh	$5,10($18)
$L710:
	lw	$2,36($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L713
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	bne	$21,$2,$L714
	beq	$17,$0,$L722
	lhu	$5,0($17)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L722
	addu	$6,$4,$6
	.set	macro
	.set	reorder

	lw	$2,24($16)
	lhu	$4,0($23)
	andi	$3,$2,0xffff
	srl	$2,$2,16
	subu	$6,$6,$3
	subu	$6,$6,$2
	lw	$2,28($16)
	addu	$6,$6,$4
	subu	$6,$6,$2
	.set	noreorder
	.set	nomacro
	j	$L758
	addu	$6,$6,$5
	.set	macro
	.set	reorder

$L714:
	lw	$2,24($16)
	addu	$6,$4,$6
	lhu	$4,0($23)
	andi	$3,$2,0xffff
	srl	$2,$2,16
	subu	$6,$6,$3
	subu	$6,$6,$2
	lw	$3,28($16)
	addu	$6,$6,$4
	lhu	$2,0($17)
	subu	$6,$6,$3
	addu	$6,$6,$2
$L758:
	.set	noreorder
	.set	nomacro
	bgez	$6,$L724
	andi	$2,$6,0xffff
	.set	macro
	.set	reorder

	subu	$6,$0,$6
	sra	$2,$6,16
	andi	$3,$6,0xffff
	addu	$6,$2,$3
	sra	$2,$6,16
	addu	$6,$6,$2
	nor	$2,$0,$6
	.set	noreorder
	.set	nomacro
	j	$L723
	sh	$2,0($17)
	.set	macro
	.set	reorder

$L724:
	sra	$3,$6,16
	addu	$6,$3,$2
	sra	$2,$6,16
	addu	$6,$6,$2
	sh	$6,0($17)
$L723:
	lw	$2,32($16)
	addu	$6,$7,$fp
	lhu	$4,2($23)
	andi	$3,$2,0xffff
	srl	$2,$2,16
	subu	$6,$6,$3
	subu	$6,$6,$2
	lw	$3,36($16)
	addu	$6,$6,$4
	lhu	$2,0($17)
	subu	$6,$6,$3
	addu	$6,$6,$2
	.set	noreorder
	.set	nomacro
	bgez	$6,$L727
	andi	$2,$6,0xffff
	.set	macro
	.set	reorder

	subu	$6,$0,$6
	sra	$2,$6,16
	andi	$3,$6,0xffff
	addu	$6,$2,$3
	sra	$2,$6,16
	addu	$6,$6,$2
	nor	$2,$0,$6
	.set	noreorder
	.set	nomacro
	j	$L722
	sh	$2,0($17)
	.set	macro
	.set	reorder

$L727:
	sra	$3,$6,16
	addu	$6,$3,$2
	sra	$2,$6,16
	addu	$6,$6,$2
	sh	$6,0($17)
$L722:
	lw	$2,28($16)
	sh	$2,0($23)
	lw	$2,36($16)
	sh	$2,2($23)
$L713:
	lw	$2,24($16)
	sw	$2,12($18)
$L753:
	lw	$2,32($16)
	sw	$2,16($18)
$L644:
	lbu	$2,8($18)
	lbu	$4,9($18)
	addiu	$5,$2,-1
	andi	$2,$2,0x00ff
	andi	$3,$5,0x00ff
	sll	$2,$2,8
	sll	$3,$3,8
	or	$6,$2,$4
	or	$3,$3,$4
	lhu	$2,10($18)
	subu	$6,$6,$3
	addu	$6,$6,$2
	.set	noreorder
	.set	nomacro
	bgez	$6,$L731
	sb	$5,8($18)
	.set	macro
	.set	reorder

	subu	$6,$0,$6
	sra	$2,$6,16
	andi	$3,$6,0xffff
	addu	$6,$2,$3
	sra	$2,$6,16
	addu	$6,$6,$2
	nor	$2,$0,$6
	.set	noreorder
	.set	nomacro
	j	$L730
	sh	$2,10($18)
	.set	macro
	.set	reorder

$L731:
	andi	$2,$6,0xffff
	sra	$3,$6,16
	addu	$6,$3,$2
	sra	$2,$6,16
	addu	$6,$6,$2
	sh	$6,10($18)
$L730:
	lw	$4,24($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	move	$5,$0
	.set	macro
	.set	reorder

	lw	$4,24($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_dev
	move	$5,$0
	.set	macro
	.set	reorder

	lw	$4,24($sp)
	lw	$5,48($16)
	lw	$6,52($16)
	jal	fastpath_set_qos_mark
	lui	$2,%hi(jiffies)
	lw	$2,%lo(jiffies)($2)
	move	$4,$18
	lw	$6,24($sp)
	move	$5,$23
	.set	noreorder
	.set	nomacro
	jal	fast_path_post_process_xmit_check
	sw	$2,40($16)
	.set	macro
	.set	reorder

	li	$3,1			# 0x1
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L562
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

	lw	$4,24($sp)
	jal	ip_finish_output3
	.set	noreorder
	.set	nomacro
	j	$L562
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

$L611:
	lw	$16,72($16)
	j	$L749
$L740:
	lw	$4,24($sp)
	jal	rtl_set_skb_inDev
	move	$4,$18
	lw	$6,24($sp)
	move	$5,$23
	lw	$31,84($sp)
	lw	$fp,80($sp)
	lw	$23,76($sp)
	lw	$22,72($sp)
	lw	$21,68($sp)
	lw	$20,64($sp)
	lw	$19,60($sp)
	lw	$18,56($sp)
	lw	$17,52($sp)
	lw	$16,48($sp)
	.set	noreorder
	.set	nomacro
	j	fast_path_post_process_return_check
	addiu	$sp,$sp,88
	.set	macro
	.set	reorder

$L745:
	lw	$4,24($sp)
	jal	rtl_dst_release
$L754:
	move	$4,$0
$L562:
	lw	$31,84($sp)
	move	$2,$4
	lw	$fp,80($sp)
	lw	$23,76($sp)
	lw	$22,72($sp)
	lw	$21,68($sp)
	lw	$20,64($sp)
	lw	$19,60($sp)
	lw	$18,56($sp)
	lw	$17,52($sp)
	lw	$16,48($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,88
	.set	macro
	.set	reorder

	.end	enter_fast_path
	.rdata
	.align	2
$LC1:
	.ascii	"napt num: %d, path num: %d.\n\000"
	.section	.text.fastpath_dump_napt_entry_num,"ax",@progbits
	.align	2
	.globl	fastpath_dump_napt_entry_num
	.ent	fastpath_dump_napt_entry_num
	.type	fastpath_dump_napt_entry_num, @function
fastpath_dump_napt_entry_num:
	.set	nomips16
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	lui	$2,%hi(path_list_inuse)
	sw	$16,16($sp)
	move	$7,$0
	sw	$31,20($sp)
	move	$16,$5
	lw	$2,%lo(path_list_inuse)($2)
$L776:
	beq	$2,$0,$L773
	move	$6,$0

	lw	$2,80($2)
	j	$L776
	addiu	$7,$7,1

$L773:
	lui	$2,%hi(napt_list_inuse)
	lw	$2,%lo(napt_list_inuse)($2)
$L777:
	beq	$2,$0,$L775
	lui	$5,%hi($LC1)

	lw	$2,44($2)
	j	$L777
	addiu	$6,$6,1

$L775:
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
	.section	.text.init_table_napt,"ax",@progbits
	.align	2
	.globl	init_table_napt
	.ent	init_table_napt
	.type	init_table_napt, @function
init_table_napt:
	.set	nomips16
	.frame	$sp,64,$31		# vars= 16, regs= 7/0, args= 16, gp= 0
	.mask	0x803f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
$L783:
$L824:
	addiu	$sp,$sp,-64
	lui	$2,%hi(malloc_sizes+4)
	sw	$21,52($sp)
	move	$21,$5
	sw	$19,44($sp)
	li	$5,32			# 0x20
	sw	$18,40($sp)
	move	$18,$4
	sw	$17,36($sp)
	sw	$16,32($sp)
	lui	$16,%hi(table_napt)
	sw	$31,56($sp)
	sw	$20,48($sp)
	lw	$4,%lo(malloc_sizes+4)($2)
	jal	kmem_cache_alloc
	move	$19,$16

	li	$3,-1			# 0xffffffff
	move	$17,$2
	beq	$2,$0,$L778
	sw	$2,%lo(table_napt)($16)

$L834:
$L875:
	lui	$2,%hi(napt_list_inuse)
	lui	$20,%hi(napt_list_free)
	sll	$4,$18,1
	addiu	$3,$2,%lo(napt_list_inuse)
	addiu	$6,$20,%lo(napt_list_free)
	addu	$4,$4,$18
	sw	$0,%lo(napt_list_inuse)($2)
	sll	$4,$4,2
	sw	$3,4($3)
	lui	$2,%hi(napt_table_list_max)
	sw	$0,8($3)
	li	$5,32			# 0x20
	sw	$6,4($6)
	sw	$0,8($6)
	sw	$18,%lo(napt_table_list_max)($2)
	jal	__kmalloc
	sw	$0,%lo(napt_list_free)($20)

	sw	$2,0($17)
	lw	$2,%lo(table_napt)($16)
	lw	$2,0($2)
	beq	$2,$0,$L778
	li	$3,-1			# 0xffffffff

	j	$L878
	move	$16,$0

$L942:
	j	$L778
	li	$3,-2			# 0xfffffffe

$L878:
	blez	$18,$L939
	nop

	move	$4,$0
$L883:
	lw	$2,%lo(table_napt)($19)
	addiu	$16,$16,1
	slt	$3,$16,$18
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$0,8($2)
	lw	$2,%lo(table_napt)($19)
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$0,0($2)
	lw	$2,%lo(table_napt)($19)
	lw	$2,0($2)
	addu	$2,$4,$2
	addiu	$4,$4,12
	bne	$3,$0,$L883
	sw	$2,4($2)

$L939:
	blez	$21,$L941
	move	$16,$0

$L937:
$L891:
$L932:
	lui	$2,%hi(malloc_sizes+12)
	li	$5,32			# 0x20
	lw	$4,%lo(malloc_sizes+12)($2)
	jal	kmem_cache_alloc
	addiu	$16,$16,1

	addiu	$4,$20,%lo(napt_list_free)
	move	$3,$2
	addiu	$5,$2,44
	beq	$2,$0,$L942
	slt	$6,$16,$21

	sw	$0,44($2)
	lw	$2,4($4)
	sw	$2,48($3)
	lw	$2,4($4)
	sw	$3,0($2)
	lw	$2,8($4)
	sw	$5,4($4)
	addiu	$2,$2,1
	bne	$6,$0,$L937
	sw	$2,8($4)

$L941:
	move	$3,$0
$L778:
	lw	$31,56($sp)
	move	$2,$3
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
	.end	init_table_napt
	.section	.text.init_table_path,"ax",@progbits
	.align	2
	.globl	init_table_path
	.ent	init_table_path
	.type	init_table_path, @function
init_table_path:
	.set	nomips16
	.frame	$sp,64,$31		# vars= 16, regs= 7/0, args= 16, gp= 0
	.mask	0x803f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
$L948:
$L989:
	addiu	$sp,$sp,-64
	lui	$2,%hi(malloc_sizes+4)
	sw	$21,52($sp)
	move	$21,$5
	sw	$19,44($sp)
	li	$5,32			# 0x20
	sw	$18,40($sp)
	move	$18,$4
	sw	$17,36($sp)
	sw	$16,32($sp)
	lui	$16,%hi(table_path)
	sw	$31,56($sp)
	sw	$20,48($sp)
	lw	$4,%lo(malloc_sizes+4)($2)
	jal	kmem_cache_alloc
	move	$19,$16

	li	$3,-1			# 0xffffffff
	move	$17,$2
	beq	$2,$0,$L943
	sw	$2,%lo(table_path)($16)

$L999:
$L1040:
	lui	$2,%hi(path_list_inuse)
	lui	$20,%hi(path_list_free)
	sll	$4,$18,1
	addiu	$3,$2,%lo(path_list_inuse)
	addiu	$6,$20,%lo(path_list_free)
	addu	$4,$4,$18
	sw	$0,%lo(path_list_inuse)($2)
	sll	$4,$4,2
	sw	$3,4($3)
	lui	$2,%hi(path_table_list_max)
	sw	$0,8($3)
	li	$5,32			# 0x20
	sw	$6,4($6)
	sw	$0,8($6)
	sw	$18,%lo(path_table_list_max)($2)
	jal	__kmalloc
	sw	$0,%lo(path_list_free)($20)

	sw	$2,0($17)
	lw	$2,%lo(table_path)($16)
	lw	$2,0($2)
	beq	$2,$0,$L943
	li	$3,-1			# 0xffffffff

	j	$L1043
	move	$16,$0

$L1107:
	j	$L943
	li	$3,-2			# 0xfffffffe

$L1043:
	blez	$18,$L1104
	nop

	move	$4,$0
$L1048:
	lw	$2,%lo(table_path)($19)
	addiu	$16,$16,1
	slt	$3,$16,$18
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$0,8($2)
	lw	$2,%lo(table_path)($19)
	lw	$2,0($2)
	addu	$2,$4,$2
	sw	$0,0($2)
	lw	$2,%lo(table_path)($19)
	lw	$2,0($2)
	addu	$2,$4,$2
	addiu	$4,$4,12
	bne	$3,$0,$L1048
	sw	$2,4($2)

$L1104:
	blez	$21,$L1106
	move	$16,$0

$L1102:
$L1056:
$L1097:
	lui	$2,%hi(malloc_sizes+20)
	li	$5,32			# 0x20
	lw	$4,%lo(malloc_sizes+20)($2)
	jal	kmem_cache_alloc
	addiu	$16,$16,1

	addiu	$4,$20,%lo(path_list_free)
	move	$3,$2
	addiu	$5,$2,80
	beq	$2,$0,$L1107
	slt	$6,$16,$21

	sw	$0,80($2)
	lw	$2,4($4)
	sw	$2,84($3)
	lw	$2,4($4)
	sw	$3,0($2)
	lw	$2,8($4)
	sw	$5,4($4)
	addiu	$2,$2,1
	bne	$6,$0,$L1102
	sw	$2,8($4)

$L1106:
	move	$3,$0
$L943:
	lw	$31,56($sp)
	move	$2,$3
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
	.end	init_table_path
	.rdata
	.align	2
$LC2:
	.ascii	"%s:%s\n\000"
	.align	2
$LC3:
	.ascii	"Realtek FastPath\000"
	.align	2
$LC4:
	.ascii	"v1.03\000"
	.section	.text.get_fastpath_module_info,"ax",@progbits
	.align	2
	.globl	get_fastpath_module_info
	.ent	get_fastpath_module_info
	.type	get_fastpath_module_info, @function
get_fastpath_module_info:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$5,%hi($LC2)
	lui	$6,%hi($LC3)
	lui	$7,%hi($LC4)
	addiu	$5,$5,%lo($LC2)
	addiu	$6,$6,%lo($LC3)
	beq	$4,$0,$L1110
	addiu	$7,$7,%lo($LC4)

	j	sprintf
	nop

$L1110:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	get_fastpath_module_info
	.globl	route_list_inuse
	.section	.bss
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
	.local	napt_table_list_max
	.comm	napt_table_list_max,4,4
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
	.local	path_table_list_max
	.comm	path_table_list_max,4,4
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
	.local	max_udp_frag_entry
	.comm	max_udp_frag_entry,4,4
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
	.ident	"GCC: (GNU) 3.4.6-1.3.6"
