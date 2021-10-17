	.file	1 "rtl_nf_connGC.c"
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
	.globl	drop_priority
	.data
	.align	2
	.type	drop_priority, @object
	.size	drop_priority, 40
drop_priority:
	.byte	8
	.space	1
	.half	60
	.byte	6
	.space	1
	.half	30
	.byte	5
	.space	1
	.half	60
	.byte	7
	.space	1
	.half	120
	.byte	4
	.space	1
	.half	120
	.byte	11
	.space	1
	.half	85
	.byte	1
	.space	1
	.half	110
	.byte	2
	.space	1
	.half	30
	.byte	12
	.space	1
	.half	10
	.byte	3
	.space	1
	.half	120
	.globl	Udp_State_Hash_Head
	.align	2
	.type	Udp_State_Hash_Head, @object
	.size	Udp_State_Hash_Head, 16
Udp_State_Hash_Head:
	.byte	11
	.space	3
	.word	udp_unreply_list
	.byte	12
	.space	3
	.word	udp_assured_list
	.align	2
	.type	udp_assured_list, @object
	.size	udp_assured_list, 8
udp_assured_list:
	.word	udp_assured_list
	.word	udp_assured_list
	.align	2
	.type	udp_unreply_list, @object
	.size	udp_unreply_list, 8
udp_unreply_list:
	.word	udp_unreply_list
	.word	udp_unreply_list
	.globl	Tcp_State_Hash_Head
	.align	2
	.type	Tcp_State_Hash_Head, @object
	.size	Tcp_State_Hash_Head, 88
Tcp_State_Hash_Head:
	.word	0
	.word	0
	.word	1
	.word	syn_sent_list
	.word	2
	.word	syn_recv_list
	.word	3
	.word	established_list
	.word	4
	.word	fin_wait_list
	.word	5
	.word	close_wait_list
	.word	6
	.word	last_ack_list
	.word	7
	.word	time_wait_list
	.word	8
	.word	close_list
	.word	9
	.word	listen_list
	.word	10
	.word	0
	.align	2
	.type	listen_list, @object
	.size	listen_list, 8
listen_list:
	.word	listen_list
	.word	listen_list
	.align	2
	.type	close_list, @object
	.size	close_list, 8
close_list:
	.word	close_list
	.word	close_list
	.align	2
	.type	time_wait_list, @object
	.size	time_wait_list, 8
time_wait_list:
	.word	time_wait_list
	.word	time_wait_list
	.align	2
	.type	last_ack_list, @object
	.size	last_ack_list, 8
last_ack_list:
	.word	last_ack_list
	.word	last_ack_list
	.align	2
	.type	close_wait_list, @object
	.size	close_wait_list, 8
close_wait_list:
	.word	close_wait_list
	.word	close_wait_list
	.align	2
	.type	fin_wait_list, @object
	.size	fin_wait_list, 8
fin_wait_list:
	.word	fin_wait_list
	.word	fin_wait_list
	.align	2
	.type	established_list, @object
	.size	established_list, 8
established_list:
	.word	established_list
	.word	established_list
	.align	2
	.type	syn_recv_list, @object
	.size	syn_recv_list, 8
syn_recv_list:
	.word	syn_recv_list
	.word	syn_recv_list
	.align	2
	.type	syn_sent_list, @object
	.size	syn_sent_list, 8
syn_sent_list:
	.word	syn_sent_list
	.word	syn_sent_list
	.globl	conntrack_max
	.align	2
	.type	conntrack_max, @object
	.size	conntrack_max, 4
conntrack_max:
	.word	100
	.globl	conntrack_min
	.align	2
	.type	conntrack_min, @object
	.size	conntrack_min, 4
conntrack_min:
	.word	0
	.section	.text.conntrack_dointvec,"ax",@progbits
	.align	2
	.globl	conntrack_dointvec
	.ent	conntrack_dointvec
	.type	conntrack_dointvec, @function
conntrack_dointvec:
	.set	nomips16
	.frame	$sp,32,$31		# vars= 0, regs= 2/0, args= 24, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	lw	$2,48($sp)
	sw	$16,24($sp)
	move	$16,$5
	sw	$2,16($sp)
	lw	$2,52($sp)
	sw	$31,28($sp)
	jal	proc_dointvec
	sw	$2,20($sp)

	bne	$2,$0,$L1
	move	$3,$2

	beq	$16,$0,$L1
	move	$3,$0

	lui	$2,%hi(nf_conntrack_max)
	move	$5,$0
	lw	$6,%lo(nf_conntrack_max)($2)
	lui	$2,%hi(_prot_limit)
	addiu	$9,$2,%lo(_prot_limit)
	lui	$2,%hi(prot_limit)
	addiu	$8,$2,%lo(prot_limit)
	li	$2,1374355456			# 0x51eb0000
	ori	$7,$2,0x851f
	sll	$3,$5,2
$L15:
	addiu	$5,$5,1
	addu	$2,$3,$8
	slt	$4,$5,3
	lw	$2,0($2)
	addu	$3,$3,$9
	mult	$6,$2
	mflo	$2
	multu	$2,$7
	mfhi	$2
	srl	$2,$2,5
	sw	$2,0($3)
	bne	$4,$0,$L15
	sll	$3,$5,2

	li	$3,-859045888			# 0xcccc0000
	sll	$2,$6,2
	ori	$3,$3,0xcccd
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	multu	$2,$3
	mfhi	$2
	srl	$2,$2,2
	subu	$3,$6,$2
	sltu	$3,$3,65
	bne	$3,$0,$L8
	sw	$2,%lo(rtl_nf_conntrack_threshold)($4)

	addiu	$2,$6,-64
	sw	$2,%lo(rtl_nf_conntrack_threshold)($4)
$L8:
	lui	$2,%hi(nf_conntrack_max)
	lw	$2,%lo(nf_conntrack_max)($2)
	beq	$2,$0,$L16
	lui	$2,%hi(rtl_newGC_session_status_flags)

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L9
	lui	$2,%hi(rtl_newGC_session_status_flags)

	lui	$4,%hi(rtl_newGC_session_status_flags)
	li	$3,3			# 0x3
	lw	$2,%lo(rtl_newGC_session_status_flags)($4)
	beq	$2,$3,$L3
	lui	$2,%hi(jiffies)

	lw	$2,%lo(jiffies)($2)
	sw	$3,%lo(rtl_newGC_session_status_flags)($4)
	lui	$3,%hi(rtl_newGC_session_status_time)
	addiu	$2,$2,1
	j	$L3
	sw	$2,%lo(rtl_newGC_session_status_time)($3)

$L9:
$L16:
	sw	$0,%lo(rtl_newGC_session_status_flags)($2)
$L3:
	move	$3,$0
$L1:
	lw	$31,28($sp)
	move	$2,$3
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	conntrack_dointvec
	.section	.text.conntrack_dointvec_minmax,"ax",@progbits
	.align	2
	.globl	conntrack_dointvec_minmax
	.ent	conntrack_dointvec_minmax
	.type	conntrack_dointvec_minmax, @function
conntrack_dointvec_minmax:
	.set	nomips16
	.frame	$sp,32,$31		# vars= 0, regs= 2/0, args= 24, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	lw	$2,48($sp)
	sw	$16,24($sp)
	move	$16,$5
	sw	$2,16($sp)
	lw	$2,52($sp)
	sw	$31,28($sp)
	jal	proc_dointvec_minmax
	sw	$2,20($sp)

	bne	$2,$0,$L17
	move	$3,$2

	beq	$16,$0,$L17
	move	$3,$0

	lui	$2,%hi(nf_conntrack_max)
	move	$5,$0
	lw	$6,%lo(nf_conntrack_max)($2)
	lui	$2,%hi(_prot_limit)
	addiu	$9,$2,%lo(_prot_limit)
	lui	$2,%hi(prot_limit)
	addiu	$8,$2,%lo(prot_limit)
	li	$2,1374355456			# 0x51eb0000
	ori	$7,$2,0x851f
	sll	$3,$5,2
$L31:
	addiu	$5,$5,1
	addu	$2,$3,$8
	slt	$4,$5,3
	lw	$2,0($2)
	addu	$3,$3,$9
	mult	$6,$2
	mflo	$2
	multu	$2,$7
	mfhi	$2
	srl	$2,$2,5
	sw	$2,0($3)
	bne	$4,$0,$L31
	sll	$3,$5,2

	li	$3,-859045888			# 0xcccc0000
	sll	$2,$6,2
	ori	$3,$3,0xcccd
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	multu	$2,$3
	mfhi	$2
	srl	$2,$2,2
	subu	$3,$6,$2
	sltu	$3,$3,65
	bne	$3,$0,$L24
	sw	$2,%lo(rtl_nf_conntrack_threshold)($4)

	addiu	$2,$6,-64
	sw	$2,%lo(rtl_nf_conntrack_threshold)($4)
$L24:
	lui	$2,%hi(nf_conntrack_max)
	lw	$2,%lo(nf_conntrack_max)($2)
	beq	$2,$0,$L32
	lui	$2,%hi(rtl_newGC_session_status_flags)

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L25
	lui	$2,%hi(rtl_newGC_session_status_flags)

	lui	$4,%hi(rtl_newGC_session_status_flags)
	li	$3,3			# 0x3
	lw	$2,%lo(rtl_newGC_session_status_flags)($4)
	beq	$2,$3,$L19
	lui	$2,%hi(jiffies)

	lw	$2,%lo(jiffies)($2)
	sw	$3,%lo(rtl_newGC_session_status_flags)($4)
	lui	$3,%hi(rtl_newGC_session_status_time)
	addiu	$2,$2,1
	j	$L19
	sw	$2,%lo(rtl_newGC_session_status_time)($3)

$L25:
$L32:
	sw	$0,%lo(rtl_newGC_session_status_flags)($2)
$L19:
	move	$3,$0
$L17:
	lw	$31,28($sp)
	move	$2,$3
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	conntrack_dointvec_minmax
	.section	.text.clean_from_lists,"ax",@progbits
	.align	2
	.globl	clean_from_lists
	.ent	clean_from_lists
	.type	clean_from_lists, @function
clean_from_lists:
	.set	nomips16
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$17,20($sp)
	move	$17,$5
	sw	$16,16($sp)
	move	$5,$0
	sw	$31,24($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_hlist_nulls_del_rcu
	move	$16,$4
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_hlist_nulls_del_rcu
	li	$5,1			# 0x1
	.set	macro
	.set	reorder

	jal	local_bh_disable
	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0
	.set	macro
	.set	reorder

	move	$3,$2
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L38
	slt	$2,$3,7
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L44
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L42
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L53
$L44:
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L40
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L53
$L38:
	lui	$2,%hi(prot_counters+4)
#APP
	   

	1:	ll	    $3, %lo(prot_counters+4)($2)		# atomic_sub		
       nop                                             
	subu	$3, 1					
	sc	    $3, %lo(prot_counters+4)($2)					
	beqz	$3, 2f					
	.subsection 2					
2:	b	1b					
	.previous					

#NO_APP
	j	$L52
$L40:
	lui	$2,%hi(prot_counters+8)
#APP
	   

	1:	ll	    $3, %lo(prot_counters+8)($2)		# atomic_sub		
       nop                                             
	subu	$3, 1					
	sc	    $3, %lo(prot_counters+8)($2)					
	beqz	$3, 2f					
	.subsection 2					
2:	b	1b					
	.previous					

#NO_APP
	j	$L52
$L42:
	lui	$2,%hi(prot_counters)
#APP
	   

	1:	ll	    $4, %lo(prot_counters)($2)		# atomic_sub		
       nop                                             
	subu	$4, $3					
	sc	    $4, %lo(prot_counters)($2)					
	beqz	$4, 2f					
	.subsection 2					
2:	b	1b					
	.previous					

#NO_APP
$L52:
#APP
	   

#NO_APP
	move	$4,$16
$L53:
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0
	.set	macro
	.set	reorder

	li	$3,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L54
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_protonum
	move	$5,$0
	.set	macro
	.set	reorder

	li	$3,17			# 0x11
	.set	noreorder
	.set	nomacro
	bne	$2,$3,$L51
	move	$4,$16
	.set	macro
	.set	reorder

$L54:
	.set	noreorder
	.set	nomacro
	jal	clean_from_lists_hooks
	move	$5,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_list_del
	move	$4,$16
	.set	macro
	.set	reorder

	lui	$2,%hi(nf_conntrack_max)
	lw	$2,%lo(nf_conntrack_max)($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L55
	lui	$2,%hi(rtl_newGC_session_status_flags)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_gc_threshold_check
	move	$4,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L47
	lui	$2,%hi(rtl_newGC_session_status_flags)
	.set	macro
	.set	reorder

	lui	$4,%hi(rtl_newGC_session_status_flags)
	li	$3,3			# 0x3
	lw	$2,%lo(rtl_newGC_session_status_flags)($4)
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L51
	lui	$2,%hi(jiffies)
	.set	macro
	.set	reorder

	lw	$2,%lo(jiffies)($2)
	sw	$3,%lo(rtl_newGC_session_status_flags)($4)
	lui	$3,%hi(rtl_newGC_session_status_time)
	addiu	$2,$2,1
	.set	noreorder
	.set	nomacro
	j	$L51
	sw	$2,%lo(rtl_newGC_session_status_time)($3)
	.set	macro
	.set	reorder

$L47:
$L55:
	sw	$0,%lo(rtl_newGC_session_status_flags)($2)
$L51:
	jal	local_bh_enable
	move	$4,$16
	lw	$31,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	nf_ct_remove_expectations
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	clean_from_lists
	.section	.text.rtl_death_action,"ax",@progbits
	.align	2
	.globl	rtl_death_action
	.ent	rtl_death_action
	.type	rtl_death_action, @function
rtl_death_action:
	.set	nomips16
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	lui	$2,%hi(init_net)
	sw	$17,20($sp)
	addiu	$17,$2,%lo(init_net)
	sw	$16,16($sp)
	sw	$31,24($sp)
	lw	$3,200($4)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L63
	move	$16,$4
	.set	macro
	.set	reorder

	lbu	$2,8($3)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L63
	addu	$3,$3,$2
	.set	macro
	.set	reorder

	beq	$3,$0,$L63
	lw	$2,0($3)
	beq	$2,$0,$L63
	lw	$2,64($2)
	beq	$2,$0,$L63
	jal	$2
$L63:
	jal	local_bh_disable
	.set	noreorder
	.set	nomacro
	jal	rtl_nf_ct_stat_inc
	move	$4,$17
	.set	macro
	.set	reorder

	move	$4,$16
	.set	noreorder
	.set	nomacro
	jal	clean_from_lists
	move	$5,$17
	.set	macro
	.set	reorder

	jal	local_bh_enable
	beq	$16,$0,$L56
#APP
	   

	1:	ll	$3, 0($16)		# atomic_sub_return	
       nop                                             
	subu	$2, $3, 1				
	sc	$2, 0($16)					
	beqz	$2, 2f					
	subu	$2, $3, 1				
	.subsection 2					
2:	b	1b					
	.previous					

	   

#NO_APP
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L56
	move	$4,$16
	.set	macro
	.set	reorder

	lw	$31,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	nf_conntrack_destroy
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

$L56:
	lw	$31,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	rtl_death_action
	.section	.text.rtl_connGC_addList,"ax",@progbits
	.align	2
	.globl	rtl_connGC_addList
	.ent	rtl_connGC_addList
	.type	rtl_connGC_addList, @function
rtl_connGC_addList:
	.set	nomips16
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$17,20($sp)
	move	$17,$5
	sw	$16,16($sp)
	sw	$31,24($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_ip_hdr
	move	$16,$4
	.set	macro
	.set	reorder

	beq	$2,$0,$L83
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_skb_protocol
	move	$4,$16
	.set	macro
	.set	reorder

	move	$3,$2
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L86
	slt	$2,$3,7
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L95
	li	$2,17			# 0x11
	.set	macro
	.set	reorder

	li	$2,1			# 0x1
	.set	noreorder
	.set	nomacro
	beq	$3,$2,$L93
	lui	$2,%hi(prot_counters)
	.set	macro
	.set	reorder

	j	$L83
$L95:
	beq	$3,$2,$L89
	j	$L83
$L86:
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_tcp_state
	move	$4,$17
	.set	macro
	.set	reorder

	lui	$3,%hi(Tcp_State_Hash_Head)
	sll	$2,$2,3
	addiu	$3,$3,%lo(Tcp_State_Hash_Head)
	addu	$2,$2,$3
	lw	$2,4($2)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L98
	lui	$2,%hi(prot_counters+4)
	.set	macro
	.set	reorder

	move	$4,$17
	li	$5,1			# 0x1
	.set	noreorder
	.set	nomacro
	jal	rtl_list_add_tail
	move	$6,$0
	.set	macro
	.set	reorder

	lui	$2,%hi(prot_counters+4)
$L98:
#APP
	   

	1:	ll	$3, %lo(prot_counters+4)($2)		# atomic_add    
       nop                                             
	addu	$3, 1					
	sc	$3, %lo(prot_counters+4)($2)                      
	beqz	$3, 2f					
	.subsection 2					
2:	b	1b                          
	.previous                       

#NO_APP
	j	$L97
$L89:
	.set	noreorder
	.set	nomacro
	jal	rtl_new_gc_get_ct_udp_status
	move	$4,$17
	.set	macro
	.set	reorder

	andi	$2,$2,0x2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L90
	move	$4,$17
	.set	macro
	.set	reorder

	li	$5,2			# 0x2
	.set	noreorder
	.set	nomacro
	j	$L96
	li	$6,1			# 0x1
	.set	macro
	.set	reorder

$L90:
	li	$5,2			# 0x2
	move	$6,$0
$L96:
	jal	rtl_list_add_tail
	lui	$2,%hi(prot_counters+8)
#APP
	   

	1:	ll	$3, %lo(prot_counters+8)($2)		# atomic_add    
       nop                                             
	addu	$3, 1					
	sc	$3, %lo(prot_counters+8)($2)                      
	beqz	$3, 2f					
	.subsection 2					
2:	b	1b                          
	.previous                       

#NO_APP
	j	$L97
$L93:
#APP
	   

	1:	ll	$4, %lo(prot_counters)($2)		# atomic_add    
       nop                                             
	addu	$4, $3					
	sc	$4, %lo(prot_counters)($2)                      
	beqz	$4, 2f					
	.subsection 2					
2:	b	1b                          
	.previous                       

#NO_APP
$L97:
#APP
	   

#NO_APP
$L83:
	lw	$31,24($sp)
	move	$2,$0
	lw	$17,20($sp)
	lw	$16,16($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	rtl_connGC_addList
	.section	.text.__nf_ct_refresh_acct_proto,"ax",@progbits
	.align	2
	.globl	__nf_ct_refresh_acct_proto
	.ent	__nf_ct_refresh_acct_proto
	.type	__nf_ct_refresh_acct_proto, @function
__nf_ct_refresh_acct_proto:
	.set	nomips16
	.frame	$sp,56,$31		# vars= 8, regs= 6/0, args= 24, gp= 0
	.mask	0x801f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-56
	sw	$16,32($sp)
	move	$16,$4
	sw	$20,48($sp)
	move	$20,$6
	sw	$19,44($sp)
	move	$19,$5
	sw	$18,40($sp)
	sw	$17,36($sp)
	sw	$31,52($sp)
	sw	$0,24($sp)
	lbu	$17,79($sp)
	jal	local_bh_disable
	move	$18,$7

	move	$4,$16
	jal	rtl_test_bit
	li	$5,10			# 0xa

	beq	$2,$0,$L119
	move	$4,$16

	jal	rtl_test_bit
	li	$5,3			# 0x3

	li	$3,-1			# 0xffffffff
	bne	$2,$3,$L106
	move	$5,$18

	jal	rtl_new_gc_set_ct_timeout_expires
	move	$4,$16

	li	$2,8			# 0x8
	j	$L107
	sw	$2,24($sp)

$L106:
	jal	rtl_del_ct_timer
	move	$4,$16

	beq	$2,$0,$L107
	lui	$2,%hi(jiffies)

	move	$4,$16
	lw	$5,%lo(jiffies)($2)
	jal	rtl_new_gc_set_ct_timeout_expires
	addu	$5,$5,$18

	jal	rtl_add_ct_timer
	move	$4,$16

	li	$2,8			# 0x8
	sw	$2,24($sp)
	li	$2,6			# 0x6
	beq	$17,$2,$L110
	li	$2,17			# 0x11

	beq	$17,$2,$L111
	nop

	j	$L107
	nop

$L110:
	lw	$6,84($sp)
	move	$4,$16
	j	$L118
	li	$5,1			# 0x1

$L111:
	jal	rtl_new_gc_get_ct_udp_status
	move	$4,$16

	andi	$2,$2,0x2
	beq	$2,$0,$L112
	move	$4,$16

	li	$5,2			# 0x2
	j	$L118
	li	$6,1			# 0x1

$L112:
	li	$5,2			# 0x2
	move	$6,$0
$L118:
	jal	rtl_list_move_tail
	nop

$L107:
	lw	$7,72($sp)
	addiu	$2,$sp,24
	move	$4,$16
	move	$5,$19
	move	$6,$20
	jal	__nf_ct_refresh_acct_proto_hooks
	sw	$2,16($sp)

$L119:
	jal	local_bh_enable
	nop

	lw	$31,52($sp)
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	j	$31
	addiu	$sp,$sp,56

	.set	macro
	.set	reorder
	.end	__nf_ct_refresh_acct_proto
	.section	.text.__conntrack_drop_check,"ax",@progbits
	.align	2
	.globl	__conntrack_drop_check
	.ent	__conntrack_drop_check
	.type	__conntrack_drop_check, @function
__conntrack_drop_check:
	.set	nomips16
	.frame	$sp,56,$31		# vars= 0, regs= 9/0, args= 16, gp= 0
	.mask	0x80ff0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-56
	move	$5,$0
	sw	$16,16($sp)
	move	$16,$4
	sw	$31,48($sp)
	sw	$23,44($sp)
	sw	$22,40($sp)
	sw	$21,36($sp)
	sw	$20,32($sp)
	sw	$19,28($sp)
	sw	$18,24($sp)
	jal	rtl_new_gc_get_ct_protonum
	sw	$17,20($sp)

	move	$4,$16
	move	$5,$0
	jal	rtl_new_gc_get_ct_port_by_dir
	move	$6,$0

	move	$4,$16
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_new_gc_get_ct_port_by_dir
	move	$23,$2

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_new_gc_get_ct_port_by_dir
	move	$22,$2

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_new_gc_get_ct_port_by_dir
	move	$21,$2

	move	$4,$16
	move	$5,$0
	move	$6,$0
	jal	rtl_new_gc_get_ct_ip_by_dir
	move	$20,$2

	move	$4,$16
	move	$5,$0
	li	$6,1			# 0x1
	jal	rtl_new_gc_get_ct_ip_by_dir
	move	$19,$2

	move	$4,$16
	li	$5,1			# 0x1
	move	$6,$0
	jal	rtl_new_gc_get_ct_ip_by_dir
	move	$17,$2

	move	$4,$16
	li	$5,1			# 0x1
	li	$6,1			# 0x1
	jal	rtl_new_gc_get_ct_ip_by_dir
	move	$18,$2

	li	$5,-268435456			# 0xf0000000
	move	$3,$2
	li	$4,-536870912			# 0xe0000000
	and	$2,$17,$5
	beq	$2,$4,$L122
	and	$2,$3,$5

	beq	$2,$4,$L120
	li	$2,-1			# 0xffffffff

	bne	$19,$3,$L125
	sltu	$2,$23,1024

	beq	$17,$18,$L122
	nop

$L125:
	bne	$2,$0,$L120
	li	$2,-1			# 0xffffffff

	sltu	$2,$22,1024
	bne	$2,$0,$L120
	li	$2,-1			# 0xffffffff

	sltu	$2,$21,1024
	bne	$2,$0,$L120
	li	$2,-1			# 0xffffffff

	sltu	$2,$20,1024
	bne	$2,$0,$L122
	li	$3,8080			# 0x1f90

	beq	$23,$3,$L122
	nop

	beq	$22,$3,$L122
	nop

	beq	$21,$3,$L120
	li	$2,-1			# 0xffffffff

	bne	$20,$3,$L120
	move	$2,$0

$L122:
	li	$2,-1			# 0xffffffff
$L120:
	lw	$31,48($sp)
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
	.end	__conntrack_drop_check
	.section	.text.isReservedConntrack,"ax",@progbits
	.align	2
	.globl	isReservedConntrack
	.ent	isReservedConntrack
	.type	isReservedConntrack, @function
isReservedConntrack:
	.set	nomips16
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lbu	$3,38($4)
	li	$2,1			# 0x1
	beq	$3,$2,$L126
	li	$7,1			# 0x1

	lw	$6,20($4)
	li	$8,-268435456			# 0xf0000000
	li	$9,-536870912			# 0xe0000000
	and	$2,$6,$8
	lw	$3,0($4)
	beq	$2,$9,$L137
	li	$7,-1			# 0xffffffff

	beq	$6,$7,$L137
	nop

	beq	$3,$0,$L137
	nop

	lw	$3,20($5)
	lw	$6,0($5)
	and	$2,$3,$8
	beq	$2,$9,$L137
	nop

	beq	$3,$7,$L137
	nop

	beq	$6,$0,$L137
	li	$3,80			# 0x50

	lhu	$2,16($4)
	beq	$2,$3,$L137
	li	$6,8080			# 0x1f90

	beq	$2,$6,$L137
	nop

	lhu	$4,36($4)
	beq	$4,$3,$L137
	nop

	beq	$4,$6,$L137
	nop

	lhu	$2,16($5)
	beq	$2,$3,$L137
	nop

	beq	$2,$6,$L137
	nop

	lhu	$5,36($5)
	beq	$5,$3,$L154
	li	$7,1			# 0x1

	bne	$5,$6,$L126
	move	$7,$0

$L137:
	li	$7,1			# 0x1
$L126:
$L154:
	j	$31
	move	$2,$7

	.set	macro
	.set	reorder
	.end	isReservedConntrack
	.section	.text.drop_one_conntrack,"ax",@progbits
	.align	2
	.globl	drop_one_conntrack
	.ent	drop_one_conntrack
	.type	drop_one_conntrack, @function
drop_one_conntrack:
	.set	nomips16
	.frame	$sp,64,$31		# vars= 0, regs= 10/0, args= 24, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-64
	lui	$2,%hi(_prot_limit+4)
	sw	$31,60($sp)
	lui	$3,%hi(prot_counters+4)
	sw	$fp,56($sp)
	sw	$23,52($sp)
	sw	$22,48($sp)
	sw	$21,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	lw	$2,%lo(_prot_limit+4)($2)
	lw	$3,%lo(prot_counters+4)($3)
	slt	$2,$2,$3
	beq	$2,$0,$L341
	lui	$2,%hi(_prot_limit+8)

	lui	$20,%hi(drop_priority_max_idx)
	jal	local_bh_disable
	move	$19,$0

	lw	$2,%lo(drop_priority_max_idx)($20)
	blez	$2,$L302
	lui	$3,%hi(drop_priority)

$L342:
	sll	$2,$19,2
	addiu	$3,$3,%lo(drop_priority)
	addu	$2,$2,$3
	lbu	$3,0($2)
	sltu	$2,$3,10
	beq	$2,$0,$L162
	lui	$2,%hi(Tcp_State_Hash_Head)

	sll	$3,$3,3
	addiu	$2,$2,%lo(Tcp_State_Hash_Head)
	addu	$3,$3,$2
	lw	$18,4($3)
	lw	$2,0($18)
	beq	$2,$18,$L162
	move	$17,$2

$L330:
	beq	$17,$18,$L162
	addiu	$16,$17,-204

	li	$2,-1			# 0xffffffff
	lb	$3,212($16)
	bne	$3,$2,$L169
	move	$4,$16

	jal	__conntrack_drop_check
	nop

	sb	$2,212($16)
$L169:
	lb	$2,212($16)
	bne	$2,$0,$L168
	move	$4,$16

	jal	rtl_del_ct_timer
	nop

	bne	$2,$0,$L316
	nop

$L168:
	lw	$17,0($17)
	j	$L330
	nop

$L162:
	lw	$2,%lo(drop_priority_max_idx)($20)
	addiu	$19,$19,1
	slt	$2,$19,$2
	bne	$2,$0,$L342
	lui	$3,%hi(drop_priority)

$L302:
	jal	local_bh_enable
	nop

	lui	$2,%hi(_prot_limit+8)
$L341:
	lui	$3,%hi(prot_counters+8)
	lw	$2,%lo(_prot_limit+8)($2)
	lw	$3,%lo(prot_counters+8)($3)
	slt	$2,$2,$3
	beq	$2,$0,$L180
	lui	$fp,%hi(drop_priority_max_idx)

	lui	$20,%hi(drop_priority_max_idx)
	jal	local_bh_disable
	move	$19,$0

	lw	$2,%lo(drop_priority_max_idx)($20)
	blez	$2,$L305
	lui	$3,%hi(drop_priority)

$L343:
	sll	$2,$19,2
	addiu	$3,$3,%lo(drop_priority)
	addu	$2,$2,$3
	lbu	$3,0($2)
	sltu	$2,$3,11
	bne	$2,$0,$L186
	lui	$2,%hi(Udp_State_Hash_Head)

	sll	$3,$3,3
	addiu	$2,$2,%lo(Udp_State_Hash_Head)
	addu	$3,$3,$2
	lw	$18,-84($3)
	lw	$2,0($18)
	beq	$2,$18,$L186
	move	$17,$2

$L331:
	beq	$17,$18,$L186
	addiu	$16,$17,-204

	li	$2,-1			# 0xffffffff
	lb	$3,212($16)
	bne	$3,$2,$L193
	move	$4,$16

	jal	__conntrack_drop_check
	nop

	sb	$2,212($16)
$L193:
	lb	$2,212($16)
	bne	$2,$0,$L192
	move	$4,$16

	jal	rtl_del_ct_timer
	nop

	bne	$2,$0,$L316
	nop

$L192:
	lw	$17,0($17)
	j	$L331
	nop

$L186:
	lw	$2,%lo(drop_priority_max_idx)($20)
	addiu	$19,$19,1
	slt	$2,$19,$2
	bne	$2,$0,$L343
	lui	$3,%hi(drop_priority)

$L305:
	jal	local_bh_enable
	lui	$fp,%hi(drop_priority_max_idx)

	j	$L339
	nop

$L316:
	jal	local_bh_enable
	nop

	jal	rtl_death_action
	move	$4,$16

	j	$L340
	lui	$4,%hi(rtl_newGC_session_status_flags)

$L180:
$L339:
	jal	local_bh_disable
	move	$19,$0

	lw	$2,%lo(drop_priority_max_idx)($fp)
	blez	$2,$L308
	lui	$22,%hi(drop_priority)

$L344:
	sll	$21,$19,2
	addiu	$2,$22,%lo(drop_priority)
	addu	$2,$21,$2
	lbu	$2,0($2)
	sltu	$3,$2,10
	beq	$3,$0,$L212
	sll	$4,$2,3

	lui	$2,%hi(Tcp_State_Hash_Head)
	addiu	$2,$2,%lo(Tcp_State_Hash_Head)
	addu	$2,$4,$2
	lw	$18,4($2)
	lw	$2,0($18)
	beq	$2,$18,$L233
	li	$23,3			# 0x3

	move	$17,$2
	move	$20,$0
$L332:
	beq	$17,$18,$L233
	addiu	$16,$17,-204

	li	$2,-1			# 0xffffffff
	lb	$3,212($16)
	move	$4,$16
	bne	$3,$2,$L219
	addiu	$20,$20,1

	jal	__conntrack_drop_check
	nop

	sb	$2,212($16)
$L219:
	lb	$2,212($16)
	bne	$2,$0,$L218
	move	$4,$16

	jal	rtl_get_ct_timer_expires
	nop

	lui	$3,%hi(jiffies)
	lw	$5,%lo(jiffies)($3)
	addiu	$3,$22,%lo(drop_priority)
	addu	$3,$21,$3
	subu	$2,$2,$5
	lhu	$3,2($3)
	srl	$2,$2,7
	sltu	$3,$3,$2
	bne	$3,$0,$L221
	move	$4,$16

	jal	rtl_del_ct_timer
	nop

	move	$4,$16
	move	$5,$19
	move	$6,$0
	beq	$2,$0,$L221
	li	$7,1			# 0x1

	jal	__drop_one_conntrack_process_hooks1
	sw	$23,16($sp)

	move	$4,$16
	move	$3,$2
	li	$2,1			# 0x1
	move	$5,$19
	move	$6,$0
	beq	$3,$2,$L317
	li	$7,1			# 0x1

	li	$2,2			# 0x2
	beq	$3,$2,$L244
	li	$3,1			# 0x1

	jal	__drop_one_conntrack_process_hooks2
	sw	$23,16($sp)

$L221:
	slt	$2,$20,129
	beq	$2,$0,$L244
	move	$3,$0

$L218:
	lw	$17,0($17)
	j	$L332
	nop

$L317:
	jal	local_bh_enable
	nop

	jal	rtl_death_action
	move	$4,$16

	j	$L244
	li	$3,1			# 0x1

$L212:
	lui	$2,%hi(Udp_State_Hash_Head)
	addiu	$2,$2,%lo(Udp_State_Hash_Head)
	addu	$2,$4,$2
	lw	$18,-84($2)
	lw	$2,0($18)
	beq	$2,$18,$L233
	li	$23,12			# 0xc

	move	$17,$2
	move	$20,$0
$L334:
	beq	$17,$18,$L233
	addiu	$16,$17,-204

	li	$2,-1			# 0xffffffff
	lb	$3,212($16)
	move	$4,$16
	bne	$3,$2,$L238
	addiu	$20,$20,1

	jal	__conntrack_drop_check
	nop

	sb	$2,212($16)
$L238:
	lb	$2,212($16)
	bne	$2,$0,$L237
	move	$4,$16

	jal	rtl_get_ct_timer_expires
	nop

	lui	$3,%hi(jiffies)
	lw	$5,%lo(jiffies)($3)
	addiu	$3,$22,%lo(drop_priority)
	addu	$3,$21,$3
	subu	$2,$2,$5
	lhu	$3,2($3)
	srl	$2,$2,7
	sltu	$3,$3,$2
	bne	$3,$0,$L240
	move	$4,$16

	jal	rtl_del_ct_timer
	nop

	move	$4,$16
	move	$5,$19
	move	$6,$0
	beq	$2,$0,$L240
	li	$7,1			# 0x1

	jal	__drop_one_conntrack_process_hooks1
	sw	$23,16($sp)

	move	$4,$16
	move	$3,$2
	li	$2,1			# 0x1
	move	$5,$19
	move	$6,$0
	beq	$3,$2,$L317
	li	$7,1			# 0x1

	li	$2,2			# 0x2
	beq	$3,$2,$L244
	li	$3,1			# 0x1

	jal	__drop_one_conntrack_process_hooks2
	sw	$23,16($sp)

$L240:
	slt	$2,$20,129
	beq	$2,$0,$L244
	move	$3,$0

$L237:
	lw	$17,0($17)
	j	$L334
	nop

$L233:
	move	$3,$0
$L244:
	li	$2,1			# 0x1
	beq	$3,$2,$L205
	addiu	$19,$19,1

	lw	$2,%lo(drop_priority_max_idx)($fp)
	slt	$2,$19,$2
	bne	$2,$0,$L344
	lui	$22,%hi(drop_priority)

$L308:
	jal	local_bh_enable
	lui	$fp,%hi(drop_priority_max_idx)

	jal	local_bh_disable
	move	$19,$0

	lw	$2,%lo(drop_priority_max_idx)($fp)
	blez	$2,$L312
	lui	$22,%hi(drop_priority)

$L345:
	sll	$21,$19,2
	addiu	$2,$22,%lo(drop_priority)
	addu	$2,$21,$2
	lbu	$2,0($2)
	sltu	$3,$2,10
	beq	$3,$0,$L258
	sll	$4,$2,3

	lui	$2,%hi(Tcp_State_Hash_Head)
	addiu	$2,$2,%lo(Tcp_State_Hash_Head)
	addu	$2,$4,$2
	lw	$18,4($2)
	lw	$2,0($18)
	beq	$2,$18,$L279
	li	$23,3			# 0x3

	move	$17,$2
	move	$20,$0
$L335:
	beq	$17,$18,$L279
	addiu	$16,$17,-204

	li	$2,-1			# 0xffffffff
	lb	$3,212($16)
	move	$4,$16
	bne	$3,$2,$L265
	addiu	$20,$20,1

	jal	__conntrack_drop_check
	nop

	sb	$2,212($16)
$L265:
	lb	$2,212($16)
	bne	$2,$0,$L264
	move	$4,$16

	jal	rtl_get_ct_timer_expires
	nop

	lui	$3,%hi(jiffies)
	lw	$5,%lo(jiffies)($3)
	addiu	$3,$22,%lo(drop_priority)
	addu	$3,$21,$3
	subu	$2,$2,$5
	lhu	$3,2($3)
	srl	$2,$2,9
	sltu	$3,$3,$2
	bne	$3,$0,$L267
	move	$4,$16

	jal	rtl_del_ct_timer
	nop

	move	$4,$16
	move	$5,$19
	li	$6,2			# 0x2
	beq	$2,$0,$L267
	move	$7,$0

	jal	__drop_one_conntrack_process_hooks1
	sw	$23,16($sp)

	move	$4,$16
	move	$3,$2
	li	$2,1			# 0x1
	move	$5,$19
	li	$6,2			# 0x2
	beq	$3,$2,$L322
	move	$7,$0

	li	$2,2			# 0x2
	beq	$3,$2,$L290
	li	$3,1			# 0x1

	jal	__drop_one_conntrack_process_hooks2
	sw	$23,16($sp)

$L267:
	slt	$2,$20,129
	beq	$2,$0,$L290
	move	$3,$0

$L264:
	lw	$17,0($17)
	j	$L335
	nop

$L322:
	jal	local_bh_enable
	nop

	jal	rtl_death_action
	move	$4,$16

	j	$L290
	li	$3,1			# 0x1

$L258:
	lui	$2,%hi(Udp_State_Hash_Head)
	addiu	$2,$2,%lo(Udp_State_Hash_Head)
	addu	$2,$4,$2
	lw	$18,-84($2)
	lw	$2,0($18)
	beq	$2,$18,$L279
	li	$23,12			# 0xc

	move	$17,$2
	move	$20,$0
$L337:
	beq	$17,$18,$L279
	addiu	$16,$17,-204

	li	$2,-1			# 0xffffffff
	lb	$3,212($16)
	move	$4,$16
	bne	$3,$2,$L284
	addiu	$20,$20,1

	jal	__conntrack_drop_check
	nop

	sb	$2,212($16)
$L284:
	lb	$2,212($16)
	bne	$2,$0,$L283
	move	$4,$16

	jal	rtl_get_ct_timer_expires
	nop

	lui	$3,%hi(jiffies)
	lw	$5,%lo(jiffies)($3)
	addiu	$3,$22,%lo(drop_priority)
	addu	$3,$21,$3
	subu	$2,$2,$5
	lhu	$3,2($3)
	srl	$2,$2,9
	sltu	$3,$3,$2
	bne	$3,$0,$L286
	move	$4,$16

	jal	rtl_del_ct_timer
	nop

	move	$4,$16
	move	$5,$19
	li	$6,2			# 0x2
	beq	$2,$0,$L286
	move	$7,$0

	jal	__drop_one_conntrack_process_hooks1
	sw	$23,16($sp)

	move	$4,$16
	move	$3,$2
	li	$2,1			# 0x1
	move	$5,$19
	li	$6,2			# 0x2
	beq	$3,$2,$L322
	move	$7,$0

	li	$2,2			# 0x2
	beq	$3,$2,$L290
	li	$3,1			# 0x1

	jal	__drop_one_conntrack_process_hooks2
	sw	$23,16($sp)

$L286:
	slt	$2,$20,129
	beq	$2,$0,$L290
	move	$3,$0

$L283:
	lw	$17,0($17)
	j	$L337
	nop

$L279:
	move	$3,$0
$L290:
	li	$2,1			# 0x1
	beq	$3,$2,$L205
	addiu	$19,$19,1

	lw	$2,%lo(drop_priority_max_idx)($fp)
	slt	$2,$19,$2
	bne	$2,$0,$L345
	lui	$22,%hi(drop_priority)

	j	$L312
	nop

$L205:
	lui	$4,%hi(rtl_newGC_session_status_flags)
$L340:
	lw	$2,%lo(rtl_newGC_session_status_flags)($4)
	bne	$2,$0,$L298
	nop

	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	li	$2,3			# 0x3
	sw	$2,%lo(rtl_newGC_session_status_flags)($4)
	lui	$2,%hi(rtl_newGC_session_status_time)
	addiu	$3,$3,1
	sw	$3,%lo(rtl_newGC_session_status_time)($2)
$L298:
	j	$L155
	li	$2,1			# 0x1

$L312:
	jal	local_bh_enable
	nop

	lui	$4,%hi(rtl_newGC_session_status_flags)
	lw	$2,%lo(rtl_newGC_session_status_flags)($4)
	li	$3,1			# 0x1
	beq	$2,$3,$L300
	lui	$2,%hi(jiffies)

	lw	$2,%lo(jiffies)($2)
	sw	$3,%lo(rtl_newGC_session_status_flags)($4)
	lui	$3,%hi(rtl_newGC_session_status_time)
	addiu	$2,$2,200
	sw	$2,%lo(rtl_newGC_session_status_time)($3)
$L300:
	move	$2,$0
$L155:
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
	.end	drop_one_conntrack
	.section	.text.rtl_nf_conn_GC_init,"ax",@progbits
	.align	2
	.globl	rtl_nf_conn_GC_init
	.ent	rtl_nf_conn_GC_init
	.type	rtl_nf_conn_GC_init, @function
rtl_nf_conn_GC_init:
	.set	nomips16
	.frame	$sp,32,$31		# vars= 0, regs= 3/0, args= 16, gp= 0
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	lui	$2,%hi(Tcp_State_Hash_Head)
	addiu	$sp,$sp,-32
	addiu	$2,$2,%lo(Tcp_State_Hash_Head)
	li	$5,10			# 0xa
	sw	$31,24($sp)
	addiu	$2,$2,4
	sw	$17,20($sp)
	sw	$16,16($sp)
$L352:
	lw	$3,0($2)
	addiu	$5,$5,-1
	beq	$3,$0,$L349
	addiu	$2,$2,8

	sw	$3,4($3)
	sw	$3,0($3)
$L349:
	bgez	$5,$L352
	nop

	lui	$2,%hi(Udp_State_Hash_Head)
	li	$5,1			# 0x1
	addiu	$2,$2,%lo(Udp_State_Hash_Head)
	addiu	$2,$2,4
$L358:
	lw	$3,0($2)
	addiu	$5,$5,-1
	beq	$3,$0,$L355
	addiu	$2,$2,8

	sw	$3,4($3)
	sw	$3,0($3)
$L355:
	bgez	$5,$L358
	lui	$16,%hi(prot_limit)

	move	$5,$0
	li	$6,12			# 0xc
	jal	memset
	addiu	$4,$16,%lo(prot_limit)

	lui	$2,%hi(prot_counters)
	addiu	$7,$2,%lo(prot_counters)
	lui	$2,%hi(_prot_limit)
	addiu	$6,$2,%lo(_prot_limit)
	move	$5,$0
	sll	$2,$5,2
$L381:
	addiu	$5,$5,1
	addu	$4,$2,$6
	slt	$3,$5,3
	addu	$2,$2,$7
	sw	$0,0($2)
	sw	$0,0($4)
	bne	$3,$0,$L381
	sll	$2,$5,2

	lui	$2,%hi(nf_conntrack_max)
	li	$3,10			# 0xa
	lw	$6,%lo(nf_conntrack_max)($2)
	lui	$2,%hi(_prot_limit)
	addiu	$9,$2,%lo(_prot_limit)
	lui	$2,%hi(drop_priority_max_idx)
	sw	$3,%lo(drop_priority_max_idx)($2)
	li	$2,2			# 0x2
	sw	$2,%lo(prot_limit)($16)
	li	$2,1374355456			# 0x51eb0000
	addiu	$4,$16,%lo(prot_limit)
	ori	$7,$2,0x851f
	li	$2,90			# 0x5a
	lui	$17,%hi(rtl_newGC_session_status_flags)
	sw	$2,4($4)
	li	$2,60			# 0x3c
	move	$8,$4
	move	$5,$0
	sw	$2,8($4)
	sw	$0,%lo(rtl_newGC_session_status_flags)($17)
	sll	$3,$5,2
$L382:
	addiu	$5,$5,1
	addu	$2,$3,$8
	slt	$4,$5,3
	lw	$2,0($2)
	addu	$3,$3,$9
	mult	$6,$2
	mflo	$2
	multu	$2,$7
	mfhi	$2
	srl	$2,$2,5
	sw	$2,0($3)
	bne	$4,$0,$L382
	sll	$3,$5,2

	li	$3,-859045888			# 0xcccc0000
	sll	$2,$6,2
	ori	$3,$3,0xcccd
	lui	$4,%hi(rtl_nf_conntrack_threshold)
	multu	$2,$3
	mfhi	$2
	srl	$2,$2,2
	subu	$3,$6,$2
	sltu	$3,$3,65
	bne	$3,$0,$L368
	sw	$2,%lo(rtl_nf_conntrack_threshold)($4)

	addiu	$2,$6,-64
	sw	$2,%lo(rtl_nf_conntrack_threshold)($4)
$L368:
	lui	$2,%hi(nf_conntrack_max)
	lw	$2,%lo(nf_conntrack_max)($2)
	beq	$2,$0,$L369
	nop

	jal	rtl_gc_threshold_check
	move	$4,$0

	bne	$2,$0,$L369
	li	$3,3			# 0x3

	lw	$2,%lo(rtl_newGC_session_status_flags)($17)
	beq	$2,$3,$L372
	lui	$2,%hi(jiffies)

	lw	$2,%lo(jiffies)($2)
	sw	$3,%lo(rtl_newGC_session_status_flags)($17)
	lui	$3,%hi(rtl_newGC_session_status_time)
	addiu	$2,$2,1
	j	$L372
	sw	$2,%lo(rtl_newGC_session_status_time)($3)

$L369:
	sw	$0,%lo(rtl_newGC_session_status_flags)($17)
$L372:
	jal	rtl_nf_conn_GC_init_hooks
	nop

	move	$2,$0
	lw	$31,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl_nf_conn_GC_init
	.globl	rtl_nf_conntrack_threshold
	.section	.bss
	.align	2
	.type	rtl_nf_conntrack_threshold, @object
	.size	rtl_nf_conntrack_threshold, 4
rtl_nf_conntrack_threshold:
	.space	4
	.globl	drop_priority_max_idx
	.align	2
	.type	drop_priority_max_idx, @object
	.size	drop_priority_max_idx, 4
drop_priority_max_idx:
	.space	4
	.globl	rtl_newGC_session_status_flags
	.section	.dram-fwd,"aw",@progbits
	.align	2
	.type	rtl_newGC_session_status_flags, @object
	.size	rtl_newGC_session_status_flags, 4
rtl_newGC_session_status_flags:
	.space	4
	.globl	rtl_newGC_session_status_time
	.align	2
	.type	rtl_newGC_session_status_time, @object
	.size	rtl_newGC_session_status_time, 4
rtl_newGC_session_status_time:
	.space	4
	.globl	prot_limit
	.section	.bss
	.align	2
	.type	prot_limit, @object
	.size	prot_limit, 12
prot_limit:
	.space	12
	.local	prot_counters
	.comm	prot_counters,12,4
	.local	_prot_limit
	.comm	_prot_limit,12,4
	.ident	"GCC: (GNU) 3.4.6-1.3.6"
