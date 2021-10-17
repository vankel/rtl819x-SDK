	.file	1 "filter.c"
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
	li	$2,-1			# 0xffffffff

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
	j	$L11
	li	$5,10			# 0xa

$L13:
	beq	$7,$0,$L15
	mult	$2,$5

	mflo	$2
	addu	$2,$3,$2
$L11:
	lb	$6,0($4)
	addiu	$4,$4,1
	addiu	$3,$6,-48
	bne	$6,$0,$L13
	sltu	$7,$3,10

$L15:
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
	nop
	beq	$3,$0,$L24
	li	$2,1			# 0x1

	xor	$2,$3,$4
	sltu	$2,$0,$2
$L24:
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
	
	beq	$4,$0,$L26
	lui	$2,%hi(fast_path_cusRsp401_func)

	j	$L27
	sw	$4,%lo(fast_path_cusRsp401_func)($2)

$L26:
	lui	$3,%hi(GenerateHTTP401)
	addiu	$3,$3,%lo(GenerateHTTP401)
	sw	$3,%lo(fast_path_cusRsp401_func)($2)
$L27:
	beq	$5,$0,$L28
	lui	$2,%hi(fast_path_cusRspTCPFinAck_func)

	j	$L29
	sw	$5,%lo(fast_path_cusRspTCPFinAck_func)($2)

$L28:
	lui	$3,%hi(GenerateTCPFINACK)
	addiu	$3,$3,%lo(GenerateTCPFINACK)
	sw	$3,%lo(fast_path_cusRspTCPFinAck_func)($2)
$L29:
	beq	$6,$0,$L30
	lui	$2,%hi(fast_path_cusRspTCPEndAck_func)

	j	$31
	sw	$6,%lo(fast_path_cusRspTCPEndAck_func)($2)

$L30:
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
	nop
	lw	$2,8($5)
	mflo	$4
	addiu	$4,$4,4
	div	$0,$4,$6
	li	$6,1			# 0x1
	mfhi	$4
	sll	$4,$6,$4
	ori	$4,$4,0x80
	and	$2,$4,$2
	beq	$2,$0,$L39
	move	$2,$0

	li	$6,3600			# 0xe10
	div	$0,$3,$6
	li	$2,60			# 0x3c
	li	$6,24			# 0x18
	lw	$4,0($5)
	mflo	$7
	div	$0,$3,$2
	mflo	$3
	div	$0,$7,$6
	mfhi	$6
	div	$0,$3,$2
	mfhi	$8
	mult	$6,$2
	mflo	$6
	bne	$4,$0,$L37
	addu	$8,$6,$8

	lw	$3,4($5)
	nop
	bne	$3,$0,$L38
	li	$2,1			# 0x1

	j	$L39
	nop

$L37:
	sltu	$4,$8,$4
	bne	$4,$0,$L36
	nop

$L38:
	lw	$2,4($5)
	nop
	sltu	$8,$2,$8
	j	$L39
	xori	$2,$8,0x1

$L36:
	move	$2,$0
$L39:
	lw	$31,36($sp)
	nop
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
	nop
	sltu	$4,$3,$4
	bne	$4,$0,$L43
	move	$2,$0

	lw	$2,4($5)
	nop
	sltu	$2,$2,$3
	xori	$2,$2,0x1
$L43:
	lw	$31,28($sp)
	nop
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl_ip_range_filter_fastpath
	.size	rtl_ip_range_filter_fastpath, .-rtl_ip_range_filter_fastpath
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC0:
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
	lw	$5,%lo(res12)($16)
	nop
	beq	$5,$0,$L48
	nop

	lui	$4,%hi($LC0)
	jal	remove_proc_entry
	addiu	$4,$4,%lo($LC0)

	sw	$0,%lo(res12)($16)
$L48:
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
$LC1:
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
	nop
	beq	$5,$0,$L50
	nop

	lui	$4,%hi($LC1)
	jal	remove_proc_entry
	addiu	$4,$4,%lo($LC1)

	sw	$0,%lo(res3)($16)
$L50:
	lui	$4,%hi(dos_timer)
	jal	del_timer
	addiu	$4,$4,%lo(dos_timer)

	lw	$31,20($sp)
	lw	$16,16($sp)
	j	filter_exit_fastpath
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_exit
	.size	filter_exit, .-filter_exit
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
	.section	.text.filter_table_init,"ax",@progbits
	.align	2
	.globl	filter_table_init
	.set	nomips16
	.ent	filter_table_init
	.type	filter_table_init, @function
filter_table_init:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, gp= 0
	.mask	0x80000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	jal	filter_table_head_init
	nop

	lui	$4,%hi($LC0)
	addiu	$4,$4,%lo($LC0)
	move	$5,$0
	jal	create_proc_entry
	move	$6,$0

	lui	$3,%hi(res12)
	beq	$2,$0,$L55
	sw	$2,%lo(res12)($3)

	lui	$3,%hi(filter_table_write_proc)
	addiu	$3,$3,%lo(filter_table_write_proc)
	sw	$3,68($2)
	lui	$3,%hi(filter_table_read_proc)
	addiu	$3,$3,%lo(filter_table_read_proc)
	sw	$3,64($2)
$L55:
	lw	$31,20($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

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
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$2,%hi(item)
	sw	$0,%lo(item)($2)
	lui	$4,%hi($LC1)
	lui	$2,%hi(none_dos_drop_pkt_cnt)
	sw	$0,%lo(none_dos_drop_pkt_cnt)($2)
	addiu	$4,$4,%lo($LC1)
	lui	$2,%hi(url_count)
	move	$5,$0
	move	$6,$0
	jal	create_proc_entry
	sb	$0,%lo(url_count)($2)

	lui	$3,%hi(res3)
	beq	$2,$0,$L60
	sw	$2,%lo(res3)($3)

	lui	$3,%hi(dos_write_proc)
	addiu	$3,$3,%lo(dos_write_proc)
	sw	$3,68($2)
	lui	$3,%hi(dos_read_proc)
	addiu	$3,$3,%lo(dos_read_proc)
	sw	$3,64($2)
$L60:
	lui	$16,%hi(dos_timer)
	move	$6,$0
	addiu	$4,$16,%lo(dos_timer)
	jal	init_timer_key
	move	$5,$0

	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	lw	$5,%lo(jiffies)($2)
	addiu	$2,$16,%lo(dos_timer)
	addiu	$3,$3,100
	sw	$3,8($2)
	lui	$3,%hi(dos_timer_fn)
	addiu	$3,$3,%lo(dos_timer_fn)
	lui	$16,%hi(_tcpDosScanBitmap)
	sw	$3,12($2)
	move	$4,$2
	addiu	$5,$5,100
	jal	mod_timer
	sw	$0,16($2)

	addiu	$4,$16,%lo(_tcpDosScanBitmap)
	move	$5,$0
	jal	memset
	li	$6,64			# 0x40

	addiu	$3,$16,%lo(_tcpDosScanBitmap)
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
	sb	$2,%lo(_tcpDosScanBitmap)($16)

	jal	igmp_delete_init_netlink
	nop

	lw	$31,20($sp)
	lw	$16,16($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_init
	.size	filter_init, .-filter_init
	.section	.rodata.str1.4
	.align	2
$LC2:
	.ascii	"DoS: Whole System SYN Flood Attack\012\000"
	.align	2
$LC3:
	.ascii	"DoS: Whole System FIN Flood Attack\012\000"
	.align	2
$LC4:
	.ascii	"DoS: Whole System UDP Flood Attack\012\000"
	.align	2
$LC5:
	.ascii	"DoS: Whole System ICMP Flood Attack\012\000"
	.align	2
$LC6:
	.ascii	"DoS: Per-source SYN Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC7:
	.ascii	"DoS: Per-source FIN Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC8:
	.ascii	"DoS: Per-source UDP Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC9:
	.ascii	"DoS: Per-source ICMP Flood Attack source=%u.%u.%u.%u des"
	.ascii	"tination=%u.%u.%u.%u\012\000"
	.align	2
$LC10:
	.ascii	"DoS: Port Scan Attack source=%u.%u.%u.%u destination=%u."
	.ascii	"%u.%u.%u\012\000"
	.align	2
$LC11:
	.ascii	"DoS: Tcp Scan Attack source=%u.%u.%u.%u destination=%u.%"
	.ascii	"u.%u.%u\012\000"
	.align	2
$LC12:
	.ascii	"DoS: Tcp SYN With Data Attack source=%u.%u.%u.%u destina"
	.ascii	"tion=%u.%u.%u.%u\012\000"
	.align	2
$LC13:
	.ascii	"DoS: IP Land Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC14:
	.ascii	"DoS: UdpEchoChargen Attack source=%u.%u.%u.%u destinatio"
	.ascii	"n=%u.%u.%u.%u\012\000"
	.align	2
$LC15:
	.ascii	"DoS: UdpBomb Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC16:
	.ascii	"DoS: PingOfDeath Attack source=%u.%u.%u.%u destination=%"
	.ascii	"u.%u.%u.%u\012\000"
	.align	2
$LC17:
	.ascii	"DoS: IcmpSmurf Attack source=%u.%u.%u.%u destination=%u."
	.ascii	"%u.%u.%u\012\000"
	.align	2
$LC18:
	.ascii	"DoS: IpSpoof Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC19:
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
	nop
	beq	$2,$0,$L63
	lui	$3,%hi(block_count)

	lui	$4,%hi(block_time)
	lw	$4,%lo(block_time)($4)
	lw	$5,%lo(block_count)($3)
	nop
	slt	$4,$5,$4
	bne	$4,$0,$L64
	lui	$4,%hi(block)

	sw	$0,%lo(block)($4)
	sw	$0,%lo(block_count)($3)
$L64:
	lui	$3,%hi(block)
	lw	$4,%lo(block)($3)
	li	$3,1			# 0x1
	bne	$4,$3,$L104
	andi	$3,$2,0x1e

	lui	$3,%hi(block_count)
	lw	$4,%lo(block_count)($3)
	nop
	addiu	$4,$4,1
	sw	$4,%lo(block_count)($3)
	andi	$3,$2,0x1e
$L104:
	beq	$3,$0,$L66
	lui	$14,%hi(dos_pkt+36828)

	lui	$3,%hi(dos_pkt)
	addiu	$14,$14,%lo(dos_pkt+36828)
	addiu	$3,$3,%lo(dos_pkt)
	move	$4,$0
	move	$5,$0
	move	$6,$0
	move	$7,$0
	li	$13,1			# 0x1
	andi	$12,$2,0x2
	andi	$11,$2,0x4
	andi	$10,$2,0x8
	andi	$9,$2,0x10
$L72:
	lb	$8,0($3)
	nop
	bne	$8,$13,$L67
	nop

	beq	$12,$0,$L68
	nop

	lw	$8,4($3)
	nop
	slt	$15,$0,$8
	addu	$8,$7,$8
	j	$L68
	movn	$7,$8,$15

$L99:
	lw	$8,8($3)
	nop
	slt	$15,$0,$8
	addu	$8,$6,$8
	j	$L69
	movn	$6,$8,$15

$L100:
	lw	$8,12($3)
	nop
	slt	$15,$0,$8
	addu	$8,$5,$8
	j	$L70
	movn	$5,$8,$15

$L101:
	lw	$8,16($3)
	nop
	slt	$15,$0,$8
	addu	$8,$4,$8
	movn	$4,$8,$15
$L67:
	bne	$3,$14,$L72
	addiu	$3,$3,36

	lui	$3,%hi(whole_syn_threshold)
	lw	$3,%lo(whole_syn_threshold)($3)
	nop
	slt	$7,$3,$7
	beq	$7,$0,$L73
	andi	$7,$2,0x2

	beq	$7,$0,$L73
	nop

	blez	$3,$L73
	lui	$3,%hi(LogFlag)

	lw	$7,%lo(LogFlag)($3)
	nop
	ori	$7,$7,0x2
	sw	$7,%lo(LogFlag)($3)
$L73:
	lui	$3,%hi(whole_fin_threshold)
	lw	$3,%lo(whole_fin_threshold)($3)
	nop
	slt	$6,$3,$6
	beq	$6,$0,$L74
	andi	$6,$2,0x4

	beq	$6,$0,$L74
	nop

	blez	$3,$L74
	lui	$3,%hi(LogFlag)

	lw	$6,%lo(LogFlag)($3)
	nop
	ori	$6,$6,0x4
	sw	$6,%lo(LogFlag)($3)
$L74:
	lui	$3,%hi(whole_udp_threshold)
	lw	$3,%lo(whole_udp_threshold)($3)
	nop
	slt	$5,$3,$5
	beq	$5,$0,$L75
	andi	$5,$2,0x8

	beq	$5,$0,$L75
	nop

	blez	$3,$L75
	lui	$3,%hi(LogFlag)

	lw	$5,%lo(LogFlag)($3)
	nop
	ori	$5,$5,0x8
	sw	$5,%lo(LogFlag)($3)
$L75:
	lui	$3,%hi(whole_icmp_threshold)
	lw	$3,%lo(whole_icmp_threshold)($3)
	nop
	slt	$4,$3,$4
	beq	$4,$0,$L66
	andi	$2,$2,0x10

	beq	$2,$0,$L105
	lui	$2,%hi(LogFlag)

	blez	$3,$L105
	nop

	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x10
	sw	$3,%lo(LogFlag)($2)
$L66:
	lui	$2,%hi(LogFlag)
$L105:
	lw	$16,%lo(LogFlag)($2)
	nop
	andi	$2,$16,0x2
	beq	$2,$0,$L106
	andi	$2,$16,0x4

	lui	$4,%hi($LC2)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC2)

	andi	$2,$16,0x4
$L106:
	beq	$2,$0,$L107
	andi	$2,$16,0x8

	lui	$4,%hi($LC3)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC3)

	andi	$2,$16,0x8
$L107:
	beq	$2,$0,$L108
	andi	$2,$16,0x10

	lui	$4,%hi($LC4)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC4)

	andi	$2,$16,0x10
$L108:
	beq	$2,$0,$L109
	andi	$2,$16,0x20

	lui	$4,%hi($LC5)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC5)

	andi	$2,$16,0x20
$L109:
	beq	$2,$0,$L110
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
	lui	$4,%hi($LC6)
	addiu	$4,$4,%lo($LC6)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x40
$L110:
	beq	$2,$0,$L111
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
	lui	$4,%hi($LC7)
	addiu	$4,$4,%lo($LC7)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x80
$L111:
	beq	$2,$0,$L112
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
	lui	$4,%hi($LC8)
	addiu	$4,$4,%lo($LC8)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x100
$L112:
	beq	$2,$0,$L113
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
	lui	$4,%hi($LC9)
	addiu	$4,$4,%lo($LC9)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x200
$L113:
	beq	$2,$0,$L114
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
	lui	$4,%hi($LC10)
	addiu	$4,$4,%lo($LC10)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x8000
$L114:
	beq	$2,$0,$L115
	li	$2,65536			# 0x10000

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
	lui	$4,%hi($LC11)
	addiu	$4,$4,%lo($LC11)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	li	$2,65536			# 0x10000
$L115:
	and	$2,$16,$2
	beq	$2,$0,$L116
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
	lui	$4,%hi($LC12)
	addiu	$4,$4,%lo($LC12)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x800
$L116:
	beq	$2,$0,$L117
	li	$2,262144			# 0x40000

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
	lui	$4,%hi($LC13)
	addiu	$4,$4,%lo($LC13)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	li	$2,262144			# 0x40000
$L117:
	and	$2,$16,$2
	beq	$2,$0,$L118
	li	$2,131072			# 0x20000

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
	lui	$4,%hi($LC14)
	addiu	$4,$4,%lo($LC14)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	li	$2,131072			# 0x20000
$L118:
	and	$2,$16,$2
	beq	$2,$0,$L119
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
	lui	$4,%hi($LC15)
	addiu	$4,$4,%lo($LC15)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x4000
$L119:
	beq	$2,$0,$L120
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
	lui	$4,%hi($LC16)
	addiu	$4,$4,%lo($LC16)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x400
$L120:
	beq	$2,$0,$L121
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
	lui	$4,%hi($LC17)
	addiu	$4,$4,%lo($LC17)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x1000
$L121:
	beq	$2,$0,$L92
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
	lui	$4,%hi($LC18)
	addiu	$4,$4,%lo($LC18)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

$L92:
	andi	$16,$16,0x2000
	beq	$16,$0,$L122
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
	lui	$4,%hi($LC19)
	addiu	$4,$4,%lo($LC19)
	sw	$10,16($sp)
	sw	$9,20($sp)
	sw	$8,24($sp)
	sw	$3,28($sp)
	jal	scrlog_printk
	sw	$2,32($sp)

	lui	$2,%hi(LogFlag)
$L122:
	sw	$0,%lo(LogFlag)($2)
	lui	$3,%hi(dos_pkt+36864)
	lui	$2,%hi(dos_pkt)
	addiu	$3,$3,%lo(dos_pkt+36864)
	addiu	$2,$2,%lo(dos_pkt)
$L94:
	sb	$0,0($2)
	sw	$0,24($2)
	sw	$0,4($2)
	sw	$0,8($2)
	sw	$0,12($2)
	sw	$0,16($2)
	sw	$0,20($2)
	addiu	$2,$2,36
	bne	$2,$3,$L94
	nop

$L63:
	lui	$2,%hi(none_dos_drop_pkt_cnt)
	lw	$2,%lo(none_dos_drop_pkt_cnt)($2)
	nop
	sltu	$3,$2,3001
	bne	$3,$0,$L95
	lui	$3,%hi(item_backup)

	lui	$2,%hi(item)
	lw	$3,%lo(item)($2)
	nop
	beq	$3,$0,$L96
	lui	$4,%hi(item_backup)

	sw	$3,%lo(item_backup)($4)
	j	$L96
	sw	$0,%lo(item)($2)

$L95:
	lw	$4,%lo(item_backup)($3)
	nop
	beq	$4,$0,$L97
	lui	$5,%hi(item)

	sw	$4,%lo(item)($5)
	sw	$0,%lo(item_backup)($3)
$L97:
	beq	$2,$0,$L123
	lui	$2,%hi(jiffies)

$L96:
	lui	$2,%hi(none_dos_drop_pkt_cnt)
	sw	$0,%lo(none_dos_drop_pkt_cnt)($2)
	lui	$2,%hi(jiffies)
$L123:
	lw	$5,%lo(jiffies)($2)
	lw	$31,44($sp)
	lw	$16,40($sp)
	lui	$4,%hi(dos_timer)
	addiu	$4,$4,%lo(dos_timer)
	addiu	$5,$5,100
	j	mod_timer
	addiu	$sp,$sp,48

$L68:
	bne	$11,$0,$L99
	nop

$L69:
	bne	$10,$0,$L100
	nop

$L70:
	bne	$9,$0,$L101
	nop

	j	$L67
	nop

	.set	macro
	.set	reorder
	.end	dos_timer_fn
	.size	dos_timer_fn, .-dos_timer_fn
	.section	.rodata.str1.4
	.align	2
$LC20:
	.ascii	"%s\012\000"
	.section	.text.dos_read_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	dos_read_proc
	.type	dos_read_proc, @function
dos_read_proc:
	.frame	$sp,40,$31		# vars= 8, regs= 4/0, args= 16, gp= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	move	$16,$6
	move	$17,$5
	lui	$6,%hi(dos_flag)
	lui	$5,%hi($LC20)
	addiu	$6,$6,%lo(dos_flag)
	addiu	$5,$5,%lo($LC20)
	sw	$7,16($sp)
	jal	sprintf
	move	$18,$4

	lw	$7,16($sp)
	nop
	addu	$3,$7,$16
	slt	$3,$3,$2
	bne	$3,$0,$L125
	li	$4,1			# 0x1

	lw	$3,56($sp)
	nop
	sw	$4,0($3)
$L125:
	subu	$2,$2,$16
	slt	$3,$7,$2
	addu	$16,$18,$16
	sw	$16,0($17)
	movz	$7,$2,$3
	slt	$2,$7,0
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	movn	$7,$0,$2
	move	$2,$7
	j	$31
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	dos_read_proc
	.size	dos_read_proc, .-dos_read_proc
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
	.frame	$sp,32,$31		# vars= 8, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$16,24($sp)
	jal	rtl_ip_hdr
	lui	$4,%hi($LC21)
	addiu	$4,$4,%lo($LC21)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	move	$16,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L151
	lui	$4,%hi($LC22)
	.set	macro
	.set	reorder

	lw	$2,292($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L151
	sw	$2,16($sp)
	.set	macro
	.set	reorder

	lui	$3,%hi($LC21)
	lw	$2,12($2)
	.set	noreorder
	.set	nomacro
	j	$L130
	addiu	$6,$3,%lo($LC21)
	.set	macro
	.set	reorder

$L132:
	move	$4,$6
#APP
 # 75 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$7,($4)
1:	lbu	$1,($5)
	addiu	$4,1
	bne	$1,$7,2f
	addiu	$5,1
	bnez	$7,1b
	lbu	$7,($4)
	nop
	move	$7,$1
2:	subu	$7,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
	beq	$7,$0,$L131
	lw	$2,0($2)
	#nop
$L130:
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L132
	addiu	$5,$2,35
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L151
	lui	$4,%hi($LC22)
	.set	macro
	.set	reorder

$L131:
	lw	$3,20($2)
	lw	$4,16($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$3,$L133
	move	$2,$0
	.set	macro
	.set	reorder

	lui	$4,%hi($LC22)
$L151:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC22)
	.set	macro
	.set	reorder

	beq	$2,$0,$L134
	lw	$2,292($2)
	#nop
	sw	$2,16($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L136
	move	$3,$2
	.set	macro
	.set	reorder

	lui	$2,%hi($LC22)
	lw	$3,12($3)
	.set	noreorder
	.set	nomacro
	j	$L137
	addiu	$6,$2,%lo($LC22)
	.set	macro
	.set	reorder

$L139:
	move	$4,$6
#APP
 # 75 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$7,($4)
1:	lbu	$1,($5)
	addiu	$4,1
	bne	$1,$7,2f
	addiu	$5,1
	bnez	$7,1b
	lbu	$7,($4)
	nop
	move	$7,$1
2:	subu	$7,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
	beq	$7,$0,$L138
	lw	$3,0($3)
	#nop
$L137:
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L139
	addiu	$5,$3,35
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L133
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L138:
	lw	$4,16($16)
	lw	$2,16($3)
	.set	noreorder
	.set	nomacro
	j	$L152
	xor	$2,$4,$2
	.set	macro
	.set	reorder

$L134:
	lui	$4,%hi($LC23)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC23)
	.set	macro
	.set	reorder

	beq	$2,$0,$L140
	lw	$2,292($2)
	#nop
	sw	$2,16($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L136
	move	$3,$2
	.set	macro
	.set	reorder

	lui	$2,%hi($LC23)
	lw	$3,12($3)
	.set	noreorder
	.set	nomacro
	j	$L142
	addiu	$6,$2,%lo($LC23)
	.set	macro
	.set	reorder

$L144:
	move	$4,$6
#APP
 # 75 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$7,($4)
1:	lbu	$1,($5)
	addiu	$4,1
	bne	$1,$7,2f
	addiu	$5,1
	bnez	$7,1b
	lbu	$7,($4)
	nop
	move	$7,$1
2:	subu	$7,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
	beq	$7,$0,$L147
	lw	$3,0($3)
	#nop
$L142:
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L144
	addiu	$5,$3,35
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L133
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L140:
	lui	$4,%hi($LC24)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC24)
	.set	macro
	.set	reorder

	beq	$2,$0,$L136
	lw	$2,292($2)
	#nop
	sw	$2,16($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L136
	move	$3,$2
	.set	macro
	.set	reorder

	lui	$2,%hi($LC24)
	lw	$3,12($3)
	.set	noreorder
	.set	nomacro
	j	$L146
	addiu	$6,$2,%lo($LC24)
	.set	macro
	.set	reorder

$L148:
	move	$4,$6
#APP
 # 75 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$7,($4)
1:	lbu	$1,($5)
	addiu	$4,1
	bne	$1,$7,2f
	addiu	$5,1
	bnez	$7,1b
	lbu	$7,($4)
	nop
	move	$7,$1
2:	subu	$7,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
	beq	$7,$0,$L147
	lw	$3,0($3)
	#nop
$L146:
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L148
	addiu	$5,$3,35
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L133
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L147:
	lw	$4,16($16)
	lw	$2,20($3)
	#nop
	xor	$2,$4,$2
$L152:
	.set	noreorder
	.set	nomacro
	j	$L133
	sltu	$2,$0,$2
	.set	macro
	.set	reorder

$L136:
	li	$2,1			# 0x1
$L133:
	lw	$31,28($sp)
	lw	$16,24($sp)
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
	.frame	$sp,112,$31		# vars= 56, regs= 9/0, args= 16, gp= 0
	.mask	0x80ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-112
	sw	$31,108($sp)
	sw	$23,104($sp)
	sw	$22,100($sp)
	sw	$21,96($sp)
	sw	$20,92($sp)
	sw	$19,88($sp)
	sw	$18,84($sp)
	sw	$17,80($sp)
	sw	$16,76($sp)
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
	sw	$2,68($sp)
	.set	macro
	.set	reorder

	lbu	$18,0($2)
	move	$17,$2
	lui	$2,%hi(endStateClientPort)
	andi	$18,$18,0xf
	sll	$18,$18,2
	addu	$18,$17,$18
	lhu	$3,%lo(endStateClientPort)($2)
	lhu	$2,0($18)
	lw	$5,68($sp)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L154
	lui	$2,%hi(endStateClientSeq)
	.set	macro
	.set	reorder

	lw	$3,%lo(endStateClientSeq)($2)
	lw	$2,4($18)
	#nop
	bne	$3,$2,$L154
	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$17,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L154
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
	bne	$2,$0,$L169
	li	$5,32			# 0x20
	.set	macro
	.set	reorder

	j	$L173
$L174:
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L154
$L169:
	move	$4,$19
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	skb_copy_expand
	li	$7,32			# 0x20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L157
	move	$20,$2
	.set	macro
	.set	reorder

#APP
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	bne	$2,$0,$L158
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L158:
	lui	$4,%hi($LC25)
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC25)
	.set	macro
	.set	reorder

	j	$L154
$L157:
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$2
	.set	macro
	.set	reorder

	lbu	$5,0($2)
	move	$4,$20
	andi	$5,$5,0xf
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
	addiu	$4,$sp,64
	.set	noreorder
	.set	nomacro
	jal	rtl_get_lock_from_hh
	move	$21,$2
	.set	macro
	.set	reorder

	jal	local_bh_disable
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_get_data_from_hh
	move	$22,$2
	.set	macro
	.set	reorder

	move	$5,$2
	li	$6,16			# 0x10
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$22,-16
	.set	macro
	.set	reorder

	jal	local_bh_enable
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
	move	$4,$20
	.set	macro
	.set	reorder

#APP
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	bne	$2,$0,$L159
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L159:
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$5,$0
	.set	macro
	.set	reorder

	lhu	$2,2($18)
	lw	$4,56($sp)
	addiu	$21,$sp,44
	sh	$2,44($sp)
	lhu	$3,0($18)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	sh	$3,46($sp)
	and	$2,$4,$2
	li	$3,1342177280			# 0x50000000
	or	$2,$2,$3
	lw	$5,8($18)
	lw	$3,4($18)
	sw	$2,56($sp)
	sb	$0,13($21)
	lw	$2,56($sp)
	li	$6,-327680			# 0xfffb0000
	ori	$6,$6,0xffff
	and	$6,$2,$6
	li	$4,1048576			# 0x100000
	lui	$2,%hi(url_cache+12)
	lhu	$2,%lo(url_cache+12)($2)
	or	$6,$6,$4
	li	$4,-131072			# 0xfffe0000
	ori	$4,$4,0xffff
	and	$6,$6,$4
	addiu	$3,$3,1
	move	$4,$20
	sw	$6,56($sp)
	sw	$5,48($sp)
	sw	$3,52($sp)
	sh	$2,58($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sh	$0,62($sp)
	.set	macro
	.set	reorder

	lw	$3,12($18)
	li	$16,20			# 0x14
	addiu	$18,$2,-34
	srl	$3,$3,28
	sll	$3,$3,2
	.set	noreorder
	.set	nomacro
	j	$L170
	li	$5,8			# 0x8
	.set	macro
	.set	reorder

$L164:
	lbu	$2,0($4)
	addiu	$4,$16,1
	sltu	$7,$2,2
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L161
	addu	$6,$21,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L170
	move	$16,$4
	.set	macro
	.set	reorder

$L161:
	lbu	$4,0($6)
	.set	noreorder
	.set	nomacro
	bne	$2,$5,$L162
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
	j	$L175
	addiu	$16,$sp,44
	.set	macro
	.set	reorder

$L162:
	addu	$16,$16,$4
$L170:
	sltu	$2,$16,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L164
	addu	$4,$21,$16
	.set	macro
	.set	reorder

	addiu	$16,$sp,44
$L175:
	move	$5,$18
	sh	$0,60($sp)
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
 # 141 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
 # 83 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
	sh	$22,60($sp)
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

	lw	$3,24($sp)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1073741824			# 0x40000000
	or	$2,$2,$3
	li	$3,-251723776			# 0xf0ff0000
	ori	$3,$3,0xffff
	and	$2,$2,$3
	li	$3,83886080			# 0x5000000
	or	$2,$2,$3
	sw	$2,24($sp)
	li	$2,6			# 0x6
	sb	$2,33($sp)
	sh	$0,30($sp)
	sb	$0,25($sp)
	lw	$3,12($17)
	lui	$16,%hi(url_cache)
	addiu	$2,$16,%lo(url_cache)
	sw	$3,40($sp)
	lw	$3,16($17)
	lbu	$2,14($2)
	move	$4,$19
	move	$5,$0
	sw	$3,36($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$2,32($sp)
	.set	macro
	.set	reorder

	lhu	$2,%lo(url_cache)($16)
	li	$3,40			# 0x28
	sh	$3,26($sp)
	addiu	$2,$2,1
	addiu	$3,$sp,24
	sh	$2,28($sp)
	sh	$0,34($sp)
	lw	$7,4($3)
	lw	$6,24($sp)
	lw	$4,8($3)
	lw	$5,36($sp)
	addu	$8,$7,$6
	sltu	$7,$8,$7
	addu	$8,$8,$4
	addu	$7,$8,$7
	sltu	$4,$7,$4
	addu	$7,$7,$5
	addu	$4,$7,$4
	srl	$6,$6,22
	andi	$6,$6,0x3c
	sltu	$2,$4,$5
	addu	$6,$3,$6
	addu	$2,$2,$4
	addiu	$3,$sp,40
$L165:
	lw	$4,0($3)
	addiu	$3,$3,4
	addu	$2,$2,$4
	sltu	$4,$2,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L165
	addu	$2,$4,$2
	.set	macro
	.set	reorder

	move	$4,$20
#APP
 # 83 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
	sh	$2,34($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,14
	addiu	$5,$sp,24
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

$L154:
	lw	$31,108($sp)
	lw	$23,104($sp)
	lw	$22,100($sp)
	lw	$21,96($sp)
	lw	$20,92($sp)
	lw	$19,88($sp)
	lw	$18,84($sp)
	lw	$17,80($sp)
	lw	$16,76($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,112
	.set	macro
	.set	reorder

$L173:
#APP
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	bne	$2,$0,$L154
	j	$L174
	.end	GenerateTCPENDACK
	.size	GenerateTCPENDACK, .-GenerateTCPENDACK
	.section	.text.GenerateTCPFINACK,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	GenerateTCPFINACK
	.type	GenerateTCPFINACK, @function
GenerateTCPFINACK:
	.frame	$sp,112,$31		# vars= 56, regs= 9/0, args= 16, gp= 0
	.mask	0x80ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-112
	sw	$31,108($sp)
	sw	$23,104($sp)
	sw	$22,100($sp)
	sw	$21,96($sp)
	sw	$20,92($sp)
	sw	$19,88($sp)
	sw	$18,84($sp)
	sw	$17,80($sp)
	sw	$16,76($sp)
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
	sw	$2,68($sp)
	.set	macro
	.set	reorder

	lbu	$18,0($2)
	move	$17,$2
	lui	$2,%hi(finStateClientPort)
	andi	$18,$18,0xf
	sll	$18,$18,2
	addu	$18,$17,$18
	lhu	$3,%lo(finStateClientPort)($2)
	lhu	$2,0($18)
	lw	$5,68($sp)
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L177
	lui	$2,%hi(finStateClientSeq)
	.set	macro
	.set	reorder

	lw	$3,%lo(finStateClientSeq)($2)
	lw	$2,4($18)
	#nop
	bne	$3,$2,$L177
	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$17,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L177
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
	bne	$2,$0,$L192
	li	$5,32			# 0x20
	.set	macro
	.set	reorder

	j	$L196
$L197:
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L177
$L192:
	move	$4,$19
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	skb_copy_expand
	li	$7,32			# 0x20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L180
	move	$20,$2
	.set	macro
	.set	reorder

#APP
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	bne	$2,$0,$L181
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L181:
	lui	$4,%hi($LC25)
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC25)
	.set	macro
	.set	reorder

	j	$L177
$L180:
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$2
	.set	macro
	.set	reorder

	lbu	$5,0($2)
	move	$4,$20
	andi	$5,$5,0xf
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
	addiu	$4,$sp,64
	.set	noreorder
	.set	nomacro
	jal	rtl_get_lock_from_hh
	move	$21,$2
	.set	macro
	.set	reorder

	jal	local_bh_disable
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$21
	.set	noreorder
	.set	nomacro
	jal	rtl_get_data_from_hh
	move	$22,$2
	.set	macro
	.set	reorder

	move	$5,$2
	li	$6,16			# 0x10
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$22,-16
	.set	macro
	.set	reorder

	jal	local_bh_enable
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
	move	$4,$20
	.set	macro
	.set	reorder

#APP
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	bne	$2,$0,$L182
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L182:
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$5,$0
	.set	macro
	.set	reorder

	lhu	$2,2($18)
	lw	$4,56($sp)
	addiu	$21,$sp,44
	sh	$2,44($sp)
	lhu	$3,0($18)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	sh	$3,46($sp)
	and	$2,$4,$2
	li	$3,1342177280			# 0x50000000
	or	$2,$2,$3
	lw	$5,8($18)
	lw	$3,4($18)
	sw	$2,56($sp)
	sb	$0,13($21)
	lw	$2,56($sp)
	li	$6,-327680			# 0xfffb0000
	ori	$6,$6,0xffff
	and	$6,$2,$6
	lui	$2,%hi(url_cache+12)
	lhu	$2,%lo(url_cache+12)($2)
	li	$4,1048576			# 0x100000
	or	$6,$6,$4
	li	$4,65536			# 0x10000
	or	$6,$6,$4
	move	$4,$20
	sw	$6,56($sp)
	sw	$5,48($sp)
	sw	$3,52($sp)
	sh	$2,58($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sh	$0,62($sp)
	.set	macro
	.set	reorder

	lw	$3,12($18)
	li	$16,20			# 0x14
	addiu	$18,$2,-34
	srl	$3,$3,28
	sll	$3,$3,2
	.set	noreorder
	.set	nomacro
	j	$L193
	li	$5,8			# 0x8
	.set	macro
	.set	reorder

$L187:
	lbu	$2,0($4)
	addiu	$4,$16,1
	sltu	$7,$2,2
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L184
	addu	$6,$21,$4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L193
	move	$16,$4
	.set	macro
	.set	reorder

$L184:
	lbu	$4,0($6)
	.set	noreorder
	.set	nomacro
	bne	$2,$5,$L185
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

	j	$L186
$L185:
	addu	$16,$16,$4
$L193:
	sltu	$2,$16,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L187
	addu	$4,$21,$16
	.set	macro
	.set	reorder

$L186:
	lhu	$3,46($sp)
	lui	$2,%hi(endStateClientPort)
	addiu	$16,$sp,44
	sh	$3,%lo(endStateClientPort)($2)
	lw	$3,52($sp)
	lui	$2,%hi(endStateClientSeq)
	move	$5,$18
	sw	$3,%lo(endStateClientSeq)($2)
	sh	$0,60($sp)
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
 # 141 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
 # 83 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
	sh	$22,60($sp)
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

	lw	$3,24($sp)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1073741824			# 0x40000000
	or	$2,$2,$3
	li	$3,-251723776			# 0xf0ff0000
	ori	$3,$3,0xffff
	and	$2,$2,$3
	li	$3,83886080			# 0x5000000
	or	$2,$2,$3
	sw	$2,24($sp)
	li	$2,6			# 0x6
	sb	$2,33($sp)
	sh	$0,30($sp)
	sb	$0,25($sp)
	lw	$3,12($17)
	lui	$16,%hi(url_cache)
	addiu	$2,$16,%lo(url_cache)
	sw	$3,40($sp)
	lw	$3,16($17)
	lbu	$2,14($2)
	move	$4,$19
	move	$5,$0
	sw	$3,36($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$2,32($sp)
	.set	macro
	.set	reorder

	lhu	$2,%lo(url_cache)($16)
	li	$3,40			# 0x28
	sh	$3,26($sp)
	addiu	$2,$2,1
	addiu	$3,$sp,24
	sh	$2,28($sp)
	sh	$0,34($sp)
	lw	$7,4($3)
	lw	$6,24($sp)
	lw	$4,8($3)
	lw	$5,36($sp)
	addu	$8,$7,$6
	sltu	$7,$8,$7
	addu	$8,$8,$4
	addu	$7,$8,$7
	sltu	$4,$7,$4
	addu	$7,$7,$5
	addu	$4,$7,$4
	srl	$6,$6,22
	andi	$6,$6,0x3c
	sltu	$2,$4,$5
	addu	$6,$3,$6
	addu	$2,$2,$4
	addiu	$3,$sp,40
$L188:
	lw	$4,0($3)
	addiu	$3,$3,4
	addu	$2,$2,$4
	sltu	$4,$2,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$6,$L188
	addu	$2,$4,$2
	.set	macro
	.set	reorder

	move	$4,$20
#APP
 # 83 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
	sh	$2,34($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,14
	addiu	$5,$sp,24
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

$L177:
	lw	$31,108($sp)
	lw	$23,104($sp)
	lw	$22,100($sp)
	lw	$21,96($sp)
	lw	$20,92($sp)
	lw	$19,88($sp)
	lw	$18,84($sp)
	lw	$17,80($sp)
	lw	$16,76($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,112
	.set	macro
	.set	reorder

$L196:
#APP
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	bne	$2,$0,$L177
	j	$L197
	.end	GenerateTCPFINACK
	.size	GenerateTCPFINACK, .-GenerateTCPFINACK
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
	nop
	beq	$4,$0,$L200
	nop

	jal	kfree
	nop

	sw	$0,%lo(block_message)($16)
$L200:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	unregister_customRspStr
	.size	unregister_customRspStr, .-unregister_customRspStr
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
	.frame	$sp,176,$31		# vars= 120, regs= 9/0, args= 16, gp= 0
	.mask	0x80ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-176
	sw	$31,172($sp)
	sw	$23,168($sp)
	sw	$22,164($sp)
	sw	$21,160($sp)
	sw	$20,156($sp)
	sw	$19,152($sp)
	sw	$18,148($sp)
	sw	$17,144($sp)
	sw	$16,140($sp)
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
	sw	$2,132($sp)
	.set	macro
	.set	reorder

	lw	$5,132($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$2,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L202
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
	bne	$2,$0,$L223
	lui	$2,%hi(block_message)
	.set	macro
	.set	reorder

	j	$L230
$L231:
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L202
$L223:
	lw	$4,%lo(block_message)($2)
	#nop
	bne	$4,$0,$L225
	lui	$4,%hi($LC26)
	addiu	$4,$4,%lo($LC26)
$L225:
	jal	strlen
	move	$6,$2
	move	$4,$17
	li	$5,32			# 0x20
	.set	noreorder
	.set	nomacro
	jal	skb_copy_expand
	li	$7,32			# 0x20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L207
	move	$19,$2
	.set	macro
	.set	reorder

#APP
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	bne	$2,$0,$L208
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L208:
	lui	$4,%hi($LC25)
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC25)
	.set	macro
	.set	reorder

	j	$L202
$L207:
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$2
	.set	macro
	.set	reorder

	move	$18,$2
	lbu	$2,0($2)
	move	$4,$19
	andi	$2,$2,0xf
	sll	$20,$2,2
	addu	$20,$18,$20
	lw	$3,12($20)
	#nop
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
	#nop
	bne	$4,$0,$L226
	lui	$4,%hi($LC26)
	addiu	$4,$4,%lo($LC26)
$L226:
	jal	strlen
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
	addiu	$4,$sp,128
	.set	noreorder
	.set	nomacro
	jal	rtl_get_lock_from_hh
	move	$21,$2
	.set	macro
	.set	reorder

	jal	local_bh_disable
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
	move	$22,$2
	.set	macro
	.set	reorder

	move	$5,$2
	li	$6,16			# 0x10
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$22,-16
	.set	macro
	.set	reorder

	jal	local_bh_enable
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
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L233
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
$L233:
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$4,$19
	.set	macro
	.set	reorder

	lbu	$16,0($18)
	lw	$2,12($18)
	lw	$3,16($18)
	andi	$16,$16,0xf
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
	beq	$5,$0,$L212
	addu	$22,$16,$22
	.set	macro
	.set	reorder

	move	$4,$5
	.set	noreorder
	.set	nomacro
	jal	strlen
	sw	$5,132($sp)
	.set	macro
	.set	reorder

	addiu	$6,$2,1
	lw	$5,132($sp)
	.set	noreorder
	.set	nomacro
	j	$L227
	move	$4,$22
	.set	macro
	.set	reorder

$L212:
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
$L227:
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$21,-4			# 0xfffffffc
	.set	macro
	.set	reorder

	lw	$2,8($16)
	move	$4,$17
	lw	$22,4($16)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sw	$2,4($16)
	.set	macro
	.set	reorder

	lw	$4,12($20)
	lbu	$3,0($18)
	srl	$4,$4,28
	andi	$3,$3,0xf
	addu	$3,$4,$3
	mult	$3,$21
	move	$4,$19
	li	$17,20			# 0x14
	mflo	$3
	addu	$22,$3,$22
	addu	$22,$22,$2
	sw	$22,8($16)
	lui	$2,%hi(url_cache+12)
	lhu	$2,%lo(url_cache+12)($2)
	sb	$0,13($16)
	sh	$0,18($16)
	sh	$2,14($16)
	lw	$3,12($16)
	li	$2,-327680			# 0xfffb0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1048576			# 0x100000
	or	$2,$2,$3
	li	$3,524288			# 0x80000
	or	$2,$2,$3
	li	$3,-131072			# 0xfffe0000
	ori	$3,$3,0xffff
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sw	$2,12($16)
	.set	macro
	.set	reorder

	lbu	$5,0($18)
	lw	$3,12($20)
	li	$4,8			# 0x8
	andi	$20,$5,0xf
	mult	$20,$21
	srl	$3,$3,28
	sll	$3,$3,2
	mflo	$20
	addiu	$20,$20,-14
	.set	noreorder
	.set	nomacro
	j	$L224
	addu	$20,$20,$2
	.set	macro
	.set	reorder

$L218:
	lbu	$2,0($5)
	addiu	$5,$17,1
	sltu	$7,$2,2
	.set	noreorder
	.set	nomacro
	beq	$7,$0,$L215
	addu	$6,$16,$5
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L224
	move	$17,$5
	.set	macro
	.set	reorder

$L215:
	lbu	$5,0($6)
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L216
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
	j	$L232
	move	$5,$20
	.set	macro
	.set	reorder

$L216:
	addu	$17,$17,$5
$L224:
	sltu	$2,$17,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L218
	addu	$5,$16,$17
	.set	macro
	.set	reorder

	move	$5,$20
$L232:
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
 # 141 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
 # 83 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
	srl	$8,$7,22
	addu	$7,$6,$7
	sltu	$6,$7,$6
	addu	$7,$7,$4
	addu	$6,$7,$6
	sltu	$4,$6,$4
	addu	$6,$6,$5
	addu	$4,$6,$4
	sltu	$3,$4,$5
	andi	$6,$8,0x3c
	addu	$3,$3,$4
	addu	$5,$18,$6
	addiu	$2,$18,16
$L219:
	lw	$4,0($2)
	addiu	$2,$2,4
	addu	$3,$3,$4
	sltu	$4,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$5,$L219
	addu	$3,$4,$3
	.set	macro
	.set	reorder

	move	$2,$3
	move	$4,$19
	li	$3,1			# 0x1
#APP
 # 83 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/checksum.h" 1
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
	addiu	$4,$sp,24
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

$L202:
	lw	$31,172($sp)
	lw	$23,168($sp)
	lw	$22,164($sp)
	lw	$21,160($sp)
	lw	$20,156($sp)
	lw	$19,152($sp)
	lw	$18,148($sp)
	lw	$17,144($sp)
	lw	$16,140($sp)
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,176
	.set	macro
	.set	reorder

$L230:
#APP
 # 190 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/atomic.h" 1
	raw_local_irq_save	$3
 # 0 "" 2
#NO_APP
	lw	$2,72($16)
	#nop
	addiu	$2,$2,-1
	sw	$2,72($16)
#APP
 # 134 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/irqflags.h" 1
	raw_local_irq_restore	$3
 # 0 "" 2
#NO_APP
	bne	$2,$0,$L202
	j	$L231
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

	beq	$16,$0,$L235
	nop

$L236 = .
	jal	strlen
	move	$4,$16

	addiu	$4,$2,1
	jal	__kmalloc
	li	$5,208			# 0xd0

	beq	$2,$0,$L238
	sw	$2,%lo(block_message)($17)

	lui	$5,%hi($LC27)
	move	$4,$2
	addiu	$5,$5,%lo($LC27)
	j	$L239
	move	$6,$16

$L235:
	lui	$16,%hi($LC26)
	jal	strlen
	addiu	$4,$16,%lo($LC26)

	move	$4,$2
	jal	__kmalloc
	li	$5,208			# 0xd0

	beq	$2,$0,$L238
	sw	$2,%lo(block_message)($17)

	lui	$5,%hi($LC27)
	move	$4,$2
	addiu	$5,$5,%lo($LC27)
	addiu	$6,$16,%lo($LC26)
$L239:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	sprintf
	addiu	$sp,$sp,32

$L238:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	register_customRspStr
	.size	register_customRspStr, .-register_customRspStr
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
	bne	$2,$0,$L242
	move	$16,$5

	jal	filter_table_head_init
	nop

$L241 = .
$L242:
	lui	$2,%hi(malloc_sizes+4)
	lw	$4,%lo(malloc_sizes+4)($2)
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
	bne	$4,$0,$L246
	move	$16,$4

	lui	$2,%hi(___strtok)
	lw	$16,%lo(___strtok)($2)
	nop
	beq	$16,$0,$L251
	move	$2,$16

$L246:
	move	$4,$16
	jal	strspn
	move	$5,$17

	addu	$16,$16,$2
	lb	$2,0($16)
	nop
	bne	$2,$0,$L248
	lui	$2,%hi(___strtok)

	sw	$0,%lo(___strtok)($2)
	j	$L247
	move	$16,$0

$L248:
	move	$5,$17
	jal	strpbrk
	move	$4,$16

	beq	$2,$0,$L252
	lui	$3,%hi(___strtok)

	lb	$3,0($2)
	nop
	beq	$3,$0,$L252
	lui	$3,%hi(___strtok)

	sb	$0,0($2)
	addiu	$2,$2,1
$L252:
	sw	$2,%lo(___strtok)($3)
$L247:
	move	$2,$16
$L251:
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
	.section	.text.dos_write_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	dos_write_proc
	.type	dos_write_proc, @function
dos_write_proc:
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
	bne	$3,$0,$L254
	move	$16,$6
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L272
	li	$2,-14			# 0xfffffff2
	.set	macro
	.set	reorder

	lw	$3,24($28)
	addiu	$4,$5,80
	or	$2,$4,$5
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L254
	lui	$2,%hi(dos_flag)
	.set	macro
	.set	reorder

	addiu	$4,$2,%lo(dos_flag)
	li	$6,80			# 0x50
#APP
 # 2986 "net/rtl/fastpath/filter.c" 1
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
	bne	$6,$0,$L254
	addiu	$4,$sp,16
	.set	macro
	.set	reorder

	addiu	$2,$2,%lo(dos_flag)
	li	$5,80			# 0x50
	move	$3,$4
#APP
 # 50 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
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
	j	$L255
	lui	$19,%hi(whole_syn_threshold)
	.set	macro
	.set	reorder

$L271:
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
	beq	$4,$0,$L256
	lui	$4,%hi($L270)
	.set	macro
	.set	reorder

	sll	$3,$3,2
	addiu	$4,$4,%lo($L270)
	addu	$3,$4,$3
	lw	$3,0($3)
	#nop
	j	$3
	.rdata
	.align	2
	.align	2
$L270:
	.word	$L257
	.word	$L258
	.word	$L259
	.word	$L260
	.word	$L261
	.word	$L262
	.word	$L263
	.word	$L264
	.word	$L265
	.word	$L266
	.word	$L267
	.word	$L268
	.word	$L269
	.section	.text.dos_write_proc
$L257:
	lui	$3,%hi(op_mode)
	.set	noreorder
	.set	nomacro
	j	$L256
	sb	$2,%lo(op_mode)($3)
	.set	macro
	.set	reorder

$L258:
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
	j	$L256
	sw	$2,%lo(lan_addr)($4)
	.set	macro
	.set	reorder

$L259:
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
	j	$L256
	sw	$2,%lo(lan_mask)($3)
	.set	macro
	.set	reorder

$L260:
	lui	$4,%hi(item)
	sw	$2,%lo(item)($4)
	lui	$2,%hi(item_backup)
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$0,%lo(item_backup)($2)
	.set	macro
	.set	reorder

$L261:
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$2,%lo(whole_syn_threshold)($19)
	.set	macro
	.set	reorder

$L262:
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$2,%lo(whole_fin_threshold)($20)
	.set	macro
	.set	reorder

$L263:
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$2,%lo(whole_udp_threshold)($21)
	.set	macro
	.set	reorder

$L264:
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$2,%lo(whole_icmp_threshold)($22)
	.set	macro
	.set	reorder

$L265:
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$2,%lo(per_syn_threshold)($23)
	.set	macro
	.set	reorder

$L266:
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$2,%lo(per_fin_threshold)($fp)
	.set	macro
	.set	reorder

$L267:
	lui	$3,%hi(per_udp_threshold)
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$2,%lo(per_udp_threshold)($3)
	.set	macro
	.set	reorder

$L268:
	lui	$4,%hi(per_icmp_threshold)
	.set	noreorder
	.set	nomacro
	j	$L256
	sw	$2,%lo(per_icmp_threshold)($4)
	.set	macro
	.set	reorder

$L269:
	lui	$3,%hi(block_time)
	sw	$2,%lo(block_time)($3)
$L256:
	addiu	$18,$18,1
	andi	$18,$18,0x00ff
	move	$4,$0
$L255:
	lui	$5,%hi($LC28)
	.set	noreorder
	.set	nomacro
	jal	strtok
	addiu	$5,$5,%lo($LC28)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L271
	move	$17,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L272
	move	$2,$16
	.set	macro
	.set	reorder

$L254:
	li	$2,-14			# 0xfffffff2
$L272:
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

	.end	dos_write_proc
	.size	dos_write_proc, .-dos_write_proc
	.section	.rodata.str1.4
	.align	2
$LC29:
	.ascii	"type=%d num=%d\012\000"
	.align	2
$LC30:
	.ascii	"index=%d\011\000"
	.align	2
$LC31:
	.ascii	"mac=%02x:%02x:%02x:%02x:%02x:%02x\012\000"
	.align	2
$LC32:
	.ascii	"index=%d\011\011\000"
	.align	2
$LC33:
	.ascii	"url=%s\012\000"
	.align	2
$LC34:
	.ascii	"start=%x\011\000"
	.align	2
$LC35:
	.ascii	"end=%x\011\000"
	.align	2
$LC36:
	.ascii	"day=%x\012\000"
	.align	2
$LC37:
	.ascii	"unknow table type\012\000"
	.align	2
$LC38:
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
	
$L275 = .
$L276 = .
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
	lui	$16,%hi(malloc_sizes)
	addiu	$16,$16,%lo(malloc_sizes)
	move	$22,$4
	lw	$4,52($16)
	sw	$5,84($sp)
	li	$5,208			# 0xd0
	move	$20,$6
	jal	kmem_cache_alloc
	move	$21,$7

	move	$17,$2
	beq	$17,$0,$L278
	move	$2,$0

$L279 = .
$L280 = .
	move	$4,$17
	move	$5,$0
	jal	memset
	li	$6,512			# 0x200

	lw	$4,44($16)
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0

	bne	$2,$0,$L281
	move	$16,$2

	jal	kfree
	move	$4,$17

	j	$L278
	move	$2,$0

$L281:
	move	$4,$2
	move	$5,$0
	jal	memset
	li	$6,256			# 0x100

	lui	$fp,%hi($LC32)
	lui	$2,%hi(table_list_head)
	lui	$23,%hi($LC30)
	addiu	$fp,$fp,%lo($LC32)
	lw	$18,%lo(table_list_head)($2)
	j	$L282
	addiu	$23,$23,%lo($LC30)

$L294:
	lw	$7,24($18)
	lw	$6,16($18)
	move	$4,$17
	addiu	$7,$7,-1
	jal	sprintf
	addiu	$5,$5,%lo($LC29)

	lw	$2,16($18)
	li	$3,3			# 0x3
	beq	$2,$3,$L285
	nop

	li	$3,4			# 0x4
	beq	$2,$3,$L286
	nop

	li	$3,2			# 0x2
	bne	$2,$3,$L297
	move	$4,$16

	lw	$19,8($18)
	j	$L287
	addiu	$3,$18,8

$L288:
	lw	$6,20($19)
	move	$5,$23
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
	lui	$5,%hi($LC31)
	move	$4,$16
	sw	$2,20($sp)
	lbu	$2,32($19)
	addiu	$5,$5,%lo($LC31)
	sw	$2,24($sp)
	lbu	$2,33($19)
	jal	sprintf
	sw	$2,28($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,36($sp)
	nop
$L287:
	bne	$19,$3,$L288
	move	$4,$16

	j	$L289
	nop

$L285:
	lw	$19,8($18)
	j	$L290
	addiu	$3,$18,8

$L291:
	lw	$6,20($19)
	move	$5,$fp
	jal	sprintf
	sw	$3,36($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lui	$5,%hi($LC33)
	addiu	$6,$19,28
	move	$4,$16
	jal	sprintf
	addiu	$5,$5,%lo($LC33)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,36($sp)
	nop
$L290:
	bne	$19,$3,$L291
	move	$4,$16

	j	$L289
	nop

$L286:
	lw	$19,8($18)
	j	$L292
	addiu	$3,$18,8

$L293:
	lw	$6,20($19)
	move	$5,$23
	jal	sprintf
	sw	$3,36($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$6,28($19)
	lui	$5,%hi($LC34)
	move	$4,$16
	jal	sprintf
	addiu	$5,$5,%lo($LC34)

	addiu	$2,$19,28
	move	$4,$17
	move	$5,$16
	jal	strcat
	sw	$2,32($sp)

	lw	$2,32($sp)
	lui	$5,%hi($LC35)
	move	$4,$16
	lw	$6,4($2)
	jal	sprintf
	addiu	$5,$5,%lo($LC35)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$2,32($sp)
	lui	$5,%hi($LC36)
	move	$4,$16
	lw	$6,8($2)
	jal	sprintf
	addiu	$5,$5,%lo($LC36)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,36($sp)
	nop
$L292:
	bne	$19,$3,$L293
	move	$4,$16

	j	$L289
	nop

$L297:
	lui	$5,%hi($LC37)
	jal	sprintf
	addiu	$5,$5,%lo($LC37)

	move	$4,$17
	jal	strcat
	move	$5,$16

$L289:
	lw	$18,0($18)
$L282:
	lui	$3,%hi(table_list_head)
	addiu	$3,$3,%lo(table_list_head)
	bne	$18,$3,$L294
	lui	$5,%hi($LC29)

	lui	$2,%hi(url_filter_mode)
	lw	$6,%lo(url_filter_mode)($2)
	lui	$5,%hi($LC38)
	addiu	$5,$5,%lo($LC38)
	move	$4,$22
	jal	sprintf
	move	$7,$17

	addu	$3,$21,$20
	slt	$3,$3,$2
	bne	$3,$0,$L295
	nop

	lw	$3,96($sp)
	li	$4,1			# 0x1
	sw	$4,0($3)
$L295:
	subu	$2,$2,$20
	slt	$3,$21,$2
	movz	$21,$2,$3
	lw	$2,84($sp)
	addu	$20,$22,$20
	move	$4,$17
	sw	$20,0($2)
	slt	$2,$21,0
	movn	$21,$0,$2
	jal	kfree
	sw	$21,36($sp)

	jal	kfree
	move	$4,$16

	lw	$2,36($sp)
$L278:
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
	j	$31
	addiu	$sp,$sp,80

	.set	macro
	.set	reorder
	.end	filter_table_read_proc
	.size	filter_table_read_proc, .-filter_table_read_proc
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
	move	$19,$4
	move	$22,$5
	beq	$6,$0,$L299
	move	$20,$7

	lw	$2,28($6)
	nop
	beq	$2,$0,$L299
	addiu	$21,$6,8

	lw	$16,8($6)
	j	$L300
	move	$18,$0

$L310:
	lw	$2,16($16)
	nop
	andi	$3,$2,0x1
	bne	$3,$0,$L301
	move	$18,$16

	lw	$3,20($16)
	nop
	bne	$22,$3,$L302
	nop

$L301:
	andi	$3,$2,0x200
	beq	$3,$0,$L303
	nop

	andi	$2,$2,0x2
	lw	$6,0($20)
	beq	$2,$0,$L302
	nop

	beq	$6,$0,$L299
	move	$4,$19

	lw	$5,20($16)
	jal	scan_table
	move	$7,$6

	bne	$2,$0,$L307
	li	$2,1			# 0x1

	j	$L302
	nop

$L303:
	andi	$2,$2,0x100
	bne	$2,$0,$L305
	move	$4,$19

	lw	$2,28($17)
	nop
	jalr	$2
	addiu	$5,$16,28

	beq	$2,$0,$L302
	nop

$L305:
	lw	$2,16($16)
	nop
	andi	$3,$2,0x2
	bne	$3,$0,$L306
	andi	$3,$2,0x1

	beq	$3,$0,$L304
	srl	$2,$2,3

	j	$L314
	xori	$2,$2,0x1

$L306:
	lw	$6,0($20)
	nop
	beq	$6,$0,$L299
	move	$4,$19

	lw	$5,20($16)
	jal	scan_table
	move	$7,$6

	beq	$2,$0,$L302
	nop

	lw	$2,16($16)
	nop
	andi	$4,$2,0x1
	beq	$4,$0,$L308
	lui	$3,%hi(url_filter_mode)

	lw	$4,%lo(url_filter_mode)($3)
	srl	$3,$2,3
	bne	$4,$0,$L307
	andi	$2,$3,0x1

	j	$L314
	xori	$2,$3,0x1

$L308:
	lw	$2,%lo(url_filter_mode)($3)
	j	$L307
	sltu	$2,$2,1

$L302:
	lw	$16,0($16)
	nop
$L300:
	bne	$16,$21,$L310
	nop

	bne	$18,$0,$L311
	lui	$2,%hi(url_filter_flag)

	lui	$2,%hi(url_filter_mode)
	lw	$2,%lo(url_filter_mode)($2)
	nop
	bne	$2,$0,$L304
	li	$2,-1			# 0xffffffff

	j	$L307
	nop

$L311:
	lw	$2,%lo(url_filter_flag)($2)
	nop
	bne	$2,$0,$L304
	nop

	lw	$3,16($18)
	nop
	andi	$4,$3,0x1
	beq	$4,$0,$L307
	nop

	srl	$2,$3,3
$L314:
	j	$L307
	andi	$2,$2,0x1

$L299:
	j	$L307
	li	$2,-1			# 0xffffffff

$L304:
	li	$2,1			# 0x1
$L307:
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
	.section	.rodata.str1.4
	.align	2
$LC39:
	.ascii	"[ip_range_filter]%d.%d.%d.%d ===> %d.%d.%d.%d  [Blocked]"
	.ascii	"\012\000"
	.align	2
$LC40:
	.ascii	"[mac_filter]%d.%d.%d.%d(%02x:%02x:%02x:%02x:%02x:%02x) ="
	.ascii	"==> %d.%d.%d.%d [Blocked]\012\000"
	.align	2
$LC41:
	.ascii	"[url_key_filter]%d.%d.%d.%d ==> %d.%d.%d.%d(%s)  [Blocke"
	.ascii	"d]\012\000"
	.align	2
$LC42:
	.ascii	"[schedule_filter]%d.%d.%d.%d ===> %d.%d.%d.%d  [Blocked]"
	.ascii	"\012\000"
	.align	2
$LC43:
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
	beq	$18,$3,$L318
	nop

	slt	$2,$18,3
	beq	$2,$0,$L321
	li	$2,3			# 0x3

	li	$2,1			# 0x1
	bne	$18,$2,$L342
	lui	$4,%hi($LC43)

	j	$L340
	nop

$L321:
	beq	$18,$2,$L319
	li	$2,4			# 0x4

	bne	$18,$2,$L316
	lui	$4,%hi($LC43)

	j	$L341
	nop

$L340:
	beq	$16,$0,$L322
	nop

	lw	$2,16($16)
	lw	$3,12($16)
	lui	$4,%hi($LC39)
	srl	$10,$2,16
	srl	$9,$2,8
	srl	$6,$3,16
	srl	$7,$3,8
	andi	$10,$10,0xff
	andi	$9,$9,0xff
	andi	$8,$2,0xff
	j	$L337
	addiu	$4,$4,%lo($LC39)

$L318:
	beq	$2,$0,$L322
	nop

	beq	$16,$0,$L322
	nop

	lw	$18,12($16)
	jal	rtl_eth_hdr
	move	$4,$17

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
	srl	$6,$18,16
	srl	$7,$18,8
	lw	$3,16($16)
	andi	$6,$6,0xff
	sw	$4,16($sp)
	andi	$7,$7,0xff
	addiu	$23,$23,84
	srl	$5,$18,24
	move	$4,$0
	j	$L324
	addiu	$8,$sp,20

$L323:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L324:
	sltu	$9,$4,14
	addu	$11,$23,$4
	bne	$9,$0,$L323
	addu	$10,$8,$4

	addiu	$22,$22,98
	move	$4,$0
	j	$L326
	addiu	$8,$sp,36

$L325:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L326:
	sltu	$9,$4,14
	addu	$11,$22,$4
	bne	$9,$0,$L325
	addu	$10,$8,$4

	addiu	$21,$21,112
	move	$4,$0
	j	$L328
	addiu	$8,$sp,52

$L327:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L328:
	sltu	$9,$4,14
	addu	$11,$21,$4
	bne	$9,$0,$L327
	addu	$10,$8,$4

	addiu	$20,$20,126
	move	$4,$0
	j	$L330
	addiu	$8,$sp,68

$L329:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L330:
	sltu	$9,$4,14
	addu	$11,$20,$4
	bne	$9,$0,$L329
	addu	$10,$8,$4

	addiu	$19,$19,140
	move	$4,$0
	j	$L332
	addiu	$8,$sp,84

$L331:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L332:
	sltu	$9,$4,14
	addu	$11,$19,$4
	bne	$9,$0,$L331
	addu	$10,$8,$4

	addiu	$2,$2,154
	move	$4,$0
	j	$L334
	addiu	$8,$sp,100

$L333:
	lbu	$9,0($11)
	addiu	$4,$4,1
	sb	$9,0($10)
$L334:
	sltu	$9,$4,14
	addu	$11,$2,$4
	bne	$9,$0,$L333
	addu	$10,$8,$4

	srl	$9,$3,16
	srl	$8,$3,8
	lui	$4,%hi($LC40)
	andi	$9,$9,0xff
	andi	$8,$8,0xff
	andi	$2,$3,0xff
	addiu	$4,$4,%lo($LC40)
	srl	$3,$3,24
	sw	$3,116($sp)
	sw	$9,120($sp)
	sw	$8,124($sp)
	jal	scrlog_printk
	sw	$2,128($sp)

	j	$L322
	nop

$L319:
	beq	$16,$0,$L322
	nop

	lw	$2,16($16)
	lw	$3,12($16)
	lui	$4,%hi($LC41)
	srl	$10,$2,16
	srl	$9,$2,8
	andi	$8,$2,0xff
	srl	$2,$2,24
	sw	$2,20($sp)
	lui	$2,%hi(log_info)
	srl	$6,$3,16
	srl	$7,$3,8
	andi	$10,$10,0xff
	andi	$9,$9,0xff
	srl	$5,$3,24
	addiu	$2,$2,%lo(log_info)
	andi	$3,$3,0xff
	addiu	$4,$4,%lo($LC41)
	andi	$6,$6,0x00ff
	andi	$7,$7,0x00ff
	sw	$3,16($sp)
	sw	$10,24($sp)
	sw	$9,28($sp)
	sw	$8,32($sp)
	sw	$2,36($sp)
$L336:
	jal	scrlog_printk
	nop

	j	$L322
	nop

$L341:
	beq	$16,$0,$L322
	nop

	lw	$2,16($16)
	lw	$3,12($16)
	lui	$4,%hi($LC42)
	srl	$10,$2,16
	srl	$9,$2,8
	srl	$6,$3,16
	srl	$7,$3,8
	andi	$10,$10,0xff
	andi	$9,$9,0xff
	andi	$8,$2,0xff
	addiu	$4,$4,%lo($LC42)
$L337:
	srl	$5,$3,24
	srl	$2,$2,24
	andi	$3,$3,0xff
	andi	$6,$6,0x00ff
	andi	$7,$7,0x00ff
	sw	$3,16($sp)
	sw	$2,20($sp)
	sw	$10,24($sp)
	sw	$9,28($sp)
	j	$L336
	sw	$8,32($sp)

$L316:
$L342:
	jal	panic_printk
	addiu	$4,$4,%lo($LC43)

$L322:
	lw	$31,180($sp)
	lw	$23,176($sp)
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
	li	$19,-1			# 0xffffffff
	j	$L344
	li	$18,1			# 0x1

$L349:
	jal	scan_table
	nop

	beq	$2,$19,$L345
	nop

	bne	$2,$18,$L346
	lui	$2,%hi(log_enable)

	lw	$3,%lo(log_enable)($2)
	nop
	beq	$3,$0,$L348
	li	$2,1			# 0x1

	lw	$4,16($16)
	jal	log_fastFilter
	move	$5,$17

	j	$L348
	li	$2,1			# 0x1

$L346:
	lw	$16,0($16)
	nop
$L344:
	move	$6,$16
	move	$7,$16
	move	$4,$17
	bne	$16,$20,$L349
	move	$5,$0

$L345:
	move	$2,$0
$L348:
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

	bne	$2,$0,$L352
	nop

	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

$L352:
	move	$4,$16
	lw	$31,20($sp)
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
	nop
	bne	$2,$0,$L356
	move	$17,$4

	lui	$2,%hi(table_list_head+24)
	lw	$2,%lo(table_list_head+24)($2)
	nop
	sltu	$2,$2,2
	beq	$2,$0,$L356
	lui	$3,%hi(none_dos_drop_pkt_cnt)

	lw	$4,%lo(none_dos_drop_pkt_cnt)($3)
	j	$L431
	li	$2,1			# 0x1

$L356:
	jal	rtl_get_skb_type
	move	$4,$17

	bne	$2,$0,$L357
	li	$2,4			# 0x4

	jal	rtl_get_skb_protocol
	move	$4,$17

	li	$3,2048			# 0x800
	bne	$2,$3,$L357
	li	$2,4			# 0x4

	lui	$2,%hi(table_list_head+24)
	lw	$2,%lo(table_list_head+24)($2)
	nop
	sltu	$2,$2,2
	bne	$2,$0,$L439
	lui	$2,%hi(item)

	jal	FilterWeb_v2
	move	$4,$17

	beq	$2,$0,$L360
	lui	$3,%hi(fast_path_cusRsp401_func)

	lw	$2,%lo(fast_path_cusRsp401_func)($3)
	j	$L437
	nop

$L360:
	lw	$2,%lo(fast_path_cusRsp401_func)($3)
	nop
	beq	$2,$0,$L359
	lui	$3,%hi(url_cache)

	addiu	$3,$3,%lo(url_cache)
	lbu	$4,15($3)
	li	$2,1			# 0x1
	bne	$4,$2,$L362
	nop

	lui	$2,%hi(fast_path_cusRspTCPFinAck_func)
	lw	$2,%lo(fast_path_cusRspTCPFinAck_func)($2)
	j	$L437
	nop

$L362:
	lbu	$3,16($3)
	nop
	bne	$3,$2,$L439
	lui	$2,%hi(item)

	lui	$2,%hi(fast_path_cusRspTCPEndAck_func)
	lw	$2,%lo(fast_path_cusRspTCPEndAck_func)($2)
	nop
$L437:
	beq	$2,$0,$L361
	nop

	jalr	$2
	move	$4,$17

	j	$L357
	move	$2,$0

$L359:
	lui	$2,%hi(item)
$L439:
	lw	$2,%lo(item)($2)
	nop
	andi	$2,$2,0x1
	beq	$2,$0,$L363
	nop

	jal	rtl_ip_hdr
	move	$4,$17

	lui	$4,%hi($LC22)
	addiu	$4,$4,%lo($LC22)
	lbu	$20,0($2)
	jal	rtl_get_dev_by_name
	move	$16,$2

	lui	$4,%hi($LC23)
	addiu	$4,$4,%lo($LC23)
	lui	$18,%hi(wan_ppp_dev)
	sw	$2,%lo(wan_ppp_dev)($18)
	jal	rtl_get_dev_by_name
	lui	$19,%hi(wan_dev)

	move	$4,$17
	andi	$20,$20,0xf
	jal	rtl_get_skb_dev
	sw	$2,%lo(wan_dev)($19)

	beq	$2,$0,$L363
	nop

	jal	rtl_get_skb_dev
	move	$4,$17

	lw	$3,%lo(wan_dev)($19)
	nop
	beq	$3,$2,$L440
	lui	$2,%hi(item)

	jal	rtl_get_skb_dev
	move	$4,$17

	lw	$3,%lo(wan_ppp_dev)($18)
	nop
	bne	$3,$2,$L363
	lui	$2,%hi(item)

$L440:
	lw	$3,%lo(item)($2)
	sll	$20,$20,2
	andi	$2,$3,0x3fe
	beq	$2,$0,$L365
	addu	$20,$16,$20

	lw	$2,12($16)
	li	$4,36			# 0x24
	andi	$5,$2,0x3ff
	mult	$5,$4
	andi	$5,$2,0xff
	lui	$4,%hi(ConnectedIp)
	sll	$5,$5,2
	addiu	$4,$4,%lo(ConnectedIp)
	addu	$4,$5,$4
	lw	$5,0($4)
	lui	$4,%hi(dos_pkt)
	addiu	$4,$4,%lo(dos_pkt)
	mflo	$6
	bne	$2,$5,$L366
	addu	$4,$4,$6

	lw	$5,24($4)
	nop
	bne	$5,$2,$L366
	nop

	j	$L365
	sb	$0,0($4)

$L366:
	lw	$6,24($4)
	lb	$5,0($4)
	beq	$6,$2,$L367
	nop

	bne	$5,$0,$L365
	lui	$5,%hi(cur_p_pkt)

	sw	$4,%lo(cur_p_pkt)($5)
	sw	$2,24($4)
	li	$2,1			# 0x1
	j	$L368
	sb	$2,0($4)

$L367:
	li	$2,1			# 0x1
	bne	$5,$2,$L390
	move	$2,$0

	lui	$2,%hi(cur_p_pkt)
	j	$L368
	sw	$4,%lo(cur_p_pkt)($2)

$L425:
	lbu	$4,9($16)
	lui	$2,%hi(cur_p_pkt)
	lw	$5,%lo(cur_p_pkt)($2)
	li	$2,6			# 0x6
	bne	$4,$2,$L371
	move	$2,$0

	lbu	$4,13($20)
	li	$2,2			# 0x2
	andi	$4,$4,0x3f
	bne	$4,$2,$L371
	move	$2,$0

	lui	$2,%hi(block)
	lw	$4,%lo(block)($2)
	li	$2,1			# 0x1
	bne	$4,$2,$L370
	lui	$2,%hi(attack_saddr+4)

	lw	$6,%lo(attack_saddr+4)($2)
	lw	$4,12($16)
	nop
	beq	$6,$4,$L371
	li	$2,-1			# 0xffffffff

$L370:
	lw	$4,4($5)
	lui	$2,%hi(per_syn_threshold)
	addiu	$4,$4,1
	sw	$4,4($5)
	lw	$2,%lo(per_syn_threshold)($2)
	nop
	slt	$4,$2,$4
	beq	$4,$0,$L369
	andi	$4,$3,0x20

	beq	$4,$0,$L369
	nop

	blez	$2,$L369
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
$L369:
	move	$2,$0
$L371:
	andi	$4,$3,0x44
$L458:
	beq	$4,$0,$L441
	andi	$4,$3,0x88

	lbu	$5,9($16)
	lui	$4,%hi(cur_p_pkt)
	lw	$6,%lo(cur_p_pkt)($4)
	li	$4,6			# 0x6
	bne	$5,$4,$L375
	move	$4,$0

	lbu	$4,13($20)
	li	$5,1			# 0x1
	andi	$4,$4,0x3f
	bne	$4,$5,$L373
	lui	$5,%hi(block)

	lw	$5,%lo(block)($5)
	nop
	bne	$5,$4,$L374
	lui	$4,%hi(attack_saddr+8)

	lw	$7,%lo(attack_saddr+8)($4)
	lw	$5,12($16)
	nop
	beq	$7,$5,$L375
	li	$4,-1			# 0xffffffff

$L374:
	lw	$5,8($6)
	lui	$4,%hi(per_fin_threshold)
	addiu	$5,$5,1
	sw	$5,8($6)
	lw	$4,%lo(per_fin_threshold)($4)
	nop
	slt	$5,$4,$5
	beq	$5,$0,$L373
	andi	$5,$3,0x40

	beq	$5,$0,$L373
	nop

	blez	$4,$L373
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
$L373:
	move	$4,$0
$L375:
	or	$2,$2,$4
	andi	$4,$3,0x88
$L441:
	beq	$4,$0,$L442
	andi	$4,$3,0x110

	lbu	$5,9($16)
	lui	$4,%hi(cur_p_pkt)
	lw	$6,%lo(cur_p_pkt)($4)
	li	$4,17			# 0x11
	bne	$5,$4,$L379
	move	$4,$0

	lui	$4,%hi(block)
	lw	$5,%lo(block)($4)
	li	$4,1			# 0x1
	bne	$5,$4,$L378
	lui	$4,%hi(attack_saddr+12)

	lw	$7,%lo(attack_saddr+12)($4)
	lw	$5,12($16)
	nop
	beq	$7,$5,$L379
	li	$4,-1			# 0xffffffff

$L378:
	lw	$5,12($6)
	lui	$4,%hi(per_udp_threshold)
	addiu	$5,$5,1
	sw	$5,12($6)
	lw	$4,%lo(per_udp_threshold)($4)
	nop
	slt	$5,$4,$5
	beq	$5,$0,$L377
	andi	$5,$3,0x80

	beq	$5,$0,$L377
	nop

	blez	$4,$L377
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
$L377:
	move	$4,$0
$L379:
	or	$2,$2,$4
	andi	$4,$3,0x110
$L442:
	beq	$4,$0,$L443
	andi	$4,$3,0x200

	lbu	$4,9($16)
	lui	$5,%hi(cur_p_pkt)
	lw	$6,%lo(cur_p_pkt)($5)
	li	$5,1			# 0x1
	bne	$4,$5,$L381
	lui	$5,%hi(block)

	lw	$5,%lo(block)($5)
	nop
	bne	$5,$4,$L382
	lui	$4,%hi(attack_saddr+16)

	lw	$7,%lo(attack_saddr+16)($4)
	lw	$5,12($16)
	nop
	beq	$7,$5,$L383
	li	$4,-1			# 0xffffffff

$L382:
	lw	$5,16($6)
	lui	$4,%hi(per_icmp_threshold)
	addiu	$5,$5,1
	sw	$5,16($6)
	lw	$4,%lo(per_icmp_threshold)($4)
	nop
	slt	$5,$4,$5
	beq	$5,$0,$L381
	andi	$5,$3,0x100

	beq	$5,$0,$L381
	nop

	blez	$4,$L381
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
$L381:
	move	$4,$0
$L383:
	or	$2,$2,$4
	andi	$4,$3,0x200
$L443:
	beq	$4,$0,$L444
	li	$4,4194304			# 0x400000

	lbu	$6,9($16)
	lui	$4,%hi(cur_p_pkt)
	li	$5,6			# 0x6
	lw	$4,%lo(cur_p_pkt)($4)
	bne	$6,$5,$L445
	li	$5,17			# 0x11

	lhu	$5,32($4)
	nop
	beq	$5,$0,$L446
	nop

	lhu	$6,2($20)
	nop
	beq	$6,$5,$L446
	nop

	lw	$5,20($4)
	nop
	addiu	$5,$5,1
	sw	$5,20($4)
	lhu	$5,32($4)
	nop
$L446:
	bne	$5,$0,$L385
	nop

	lhu	$5,2($20)
	nop
	sh	$5,32($4)
$L385:
	lbu	$6,9($16)
	li	$5,17			# 0x11
$L445:
	bne	$6,$5,$L447
	li	$5,8388608			# 0x800000

	lhu	$5,32($4)
	nop
	beq	$5,$0,$L448
	nop

	lhu	$6,2($20)
	nop
	beq	$6,$5,$L448
	nop

	lw	$5,20($4)
	nop
	addiu	$5,$5,1
	sw	$5,20($4)
	lhu	$5,32($4)
	nop
$L448:
	bne	$5,$0,$L447
	li	$5,8388608			# 0x800000

	lhu	$5,2($20)
	nop
	sh	$5,32($4)
	li	$5,8388608			# 0x800000
$L447:
	and	$5,$3,$5
	lw	$4,20($4)
	beq	$5,$0,$L389
	nop

	j	$L433
	slt	$4,$4,11

$L389:
	slt	$4,$4,201
$L433:
	bne	$4,$0,$L444
	li	$4,4194304			# 0x400000

	lui	$4,%hi(LogFlag)
	lw	$5,%lo(LogFlag)($4)
	nop
	ori	$5,$5,0x200
	sw	$5,%lo(LogFlag)($4)
	lw	$5,12($16)
	lui	$4,%hi(attack_saddr+20)
	sw	$5,%lo(attack_saddr+20)($4)
	lw	$5,16($16)
	lui	$4,%hi(attack_daddr+20)
	sw	$5,%lo(attack_daddr+20)($4)
	li	$4,4194304			# 0x400000
$L444:
	and	$4,$3,$4
	bne	$4,$0,$L449
	andi	$4,$3,0x8000

$L365:
	move	$2,$0
$L390:
	andi	$4,$3,0x8000
$L449:
	beq	$4,$0,$L450
	li	$4,65536			# 0x10000

	lbu	$5,9($16)
	li	$4,6			# 0x6
	bne	$5,$4,$L393
	move	$4,$0

	lbu	$5,13($20)
	lui	$4,%hi(_tcpDosScanBitmap)
	addiu	$4,$4,%lo(_tcpDosScanBitmap)
	andi	$5,$5,0x3f
	addu	$4,$5,$4
	lb	$4,0($4)
	nop
	beq	$4,$0,$L392
	lui	$4,%hi(LogFlag)

	lw	$5,%lo(LogFlag)($4)
	nop
	ori	$5,$5,0x8000
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+44)
	li	$4,-1			# 0xffffffff
	sw	$6,%lo(attack_saddr+44)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+44)
	j	$L393
	sw	$6,%lo(attack_daddr+44)($5)

$L392:
	move	$4,$0
$L393:
	or	$2,$2,$4
	li	$4,65536			# 0x10000
$L450:
	and	$5,$3,$4
	beq	$5,$0,$L394
	li	$5,6			# 0x6

	lbu	$6,9($16)
	nop
	bne	$6,$5,$L395
	li	$5,2			# 0x2

	lbu	$6,13($20)
	nop
	andi	$6,$6,0x3f
	bne	$6,$5,$L395
	nop

	lbu	$6,0($16)
	lw	$5,12($20)
	lhu	$7,2($16)
	andi	$6,$6,0xf
	sll	$6,$6,2
	srl	$5,$5,28
	subu	$6,$7,$6
	sll	$5,$5,2
	bne	$6,$5,$L451
	lui	$5,%hi(LogFlag)

	lhu	$5,6($16)
	nop
	andi	$5,$5,0x2000
	beq	$5,$0,$L395
	lui	$5,%hi(LogFlag)

$L451:
	lw	$6,%lo(LogFlag)($5)
	nop
	or	$4,$6,$4
	sw	$4,%lo(LogFlag)($5)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+48)
	li	$4,-1			# 0xffffffff
	sw	$6,%lo(attack_saddr+48)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+48)
	j	$L397
	sw	$6,%lo(attack_daddr+48)($5)

$L395:
	move	$4,$0
$L397:
	or	$2,$2,$4
$L394:
	andi	$4,$3,0x800
	beq	$4,$0,$L452
	li	$4,262144			# 0x40000

	lw	$6,12($16)
	lw	$5,16($16)
	nop
	bne	$6,$5,$L400
	move	$4,$0

	lui	$4,%hi(LogFlag)
	lw	$5,%lo(LogFlag)($4)
	nop
	ori	$5,$5,0x800
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+28)
	li	$4,-1			# 0xffffffff
	sw	$6,%lo(attack_saddr+28)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+28)
	sw	$6,%lo(attack_daddr+28)($5)
$L400:
	or	$2,$2,$4
	li	$4,262144			# 0x40000
$L452:
	and	$4,$3,$4
	beq	$4,$0,$L453
	li	$5,131072			# 0x20000

	lbu	$5,9($16)
	li	$4,17			# 0x11
	bne	$5,$4,$L404
	move	$4,$0

	lhu	$4,2($20)
	li	$7,7			# 0x7
	beq	$4,$7,$L403
	nop

	beq	$4,$5,$L403
	li	$6,19			# 0x13

	beq	$4,$6,$L454
	lui	$4,%hi(LogFlag)

	lhu	$4,0($20)
	nop
	beq	$4,$7,$L403
	nop

	beq	$4,$5,$L403
	nop

	bne	$4,$6,$L402
	nop

$L403:
	lui	$4,%hi(LogFlag)
$L454:
	lw	$6,%lo(LogFlag)($4)
	li	$5,262144			# 0x40000
	or	$5,$6,$5
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+56)
	li	$4,-1			# 0xffffffff
	sw	$6,%lo(attack_saddr+56)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+56)
	j	$L404
	sw	$6,%lo(attack_daddr+56)($5)

$L402:
	move	$4,$0
$L404:
	or	$2,$2,$4
	li	$5,131072			# 0x20000
$L453:
	and	$4,$3,$5
	beq	$4,$0,$L455
	andi	$4,$3,0x4000

	lbu	$6,9($16)
	li	$4,17			# 0x11
	bne	$6,$4,$L407
	move	$4,$0

	lhu	$4,6($16)
	nop
	andi	$4,$4,0x3fff
	bne	$4,$0,$L407
	move	$4,$0

	lbu	$4,0($16)
	lhu	$7,2($16)
	lhu	$6,4($20)
	andi	$4,$4,0xf
	sll	$4,$4,2
	subu	$4,$7,$4
	slt	$4,$6,$4
	beq	$4,$0,$L406
	lui	$4,%hi(LogFlag)

	lw	$6,%lo(LogFlag)($4)
	nop
	or	$5,$6,$5
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+52)
	li	$4,-1			# 0xffffffff
	sw	$6,%lo(attack_saddr+52)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+52)
	j	$L407
	sw	$6,%lo(attack_daddr+52)($5)

$L406:
	move	$4,$0
$L407:
	or	$2,$2,$4
	andi	$4,$3,0x4000
$L455:
	beq	$4,$0,$L456
	andi	$4,$3,0x400

	lhu	$4,6($16)
	nop
	andi	$5,$4,0x2000
	bne	$5,$0,$L409
	andi	$4,$4,0x1fff

	beq	$4,$0,$L409
	sll	$4,$4,3

	lbu	$6,0($16)
	lhu	$5,2($16)
	andi	$6,$6,0xf
	sll	$6,$6,2
	subu	$4,$4,$6
	addu	$4,$4,$5
	li	$5,65536			# 0x10000
	sltu	$4,$4,$5
	bne	$4,$0,$L409
	lui	$4,%hi(LogFlag)

	lw	$5,%lo(LogFlag)($4)
	nop
	ori	$5,$5,0x4000
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+40)
	li	$4,-1			# 0xffffffff
	sw	$6,%lo(attack_saddr+40)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+40)
	j	$L410
	sw	$6,%lo(attack_daddr+40)($5)

$L409:
	move	$4,$0
$L410:
	or	$2,$2,$4
	andi	$4,$3,0x400
$L456:
	beq	$4,$0,$L411
	li	$5,1			# 0x1

	lbu	$4,0($16)
	lbu	$6,9($16)
	nop
	bne	$6,$5,$L412
	andi	$4,$4,0xf

	sll	$4,$4,2
	addu	$4,$16,$4
	lbu	$5,0($4)
	li	$4,8			# 0x8
	bne	$5,$4,$L413
	move	$4,$0

	lui	$4,%hi(lan_addr)
	lw	$6,%lo(lan_addr)($4)
	lui	$5,%hi(lan_mask)
	lw	$4,12($16)
	lw	$5,%lo(lan_mask)($5)
	xor	$4,$6,$4
	and	$4,$4,$5
	bne	$4,$0,$L412
	lui	$4,%hi(LogFlag)

	lw	$5,%lo(LogFlag)($4)
	nop
	ori	$5,$5,0x400
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+24)
	li	$4,-1			# 0xffffffff
	sw	$6,%lo(attack_saddr+24)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+24)
	j	$L413
	sw	$6,%lo(attack_daddr+24)($5)

$L412:
	move	$4,$0
$L413:
	or	$2,$2,$4
$L411:
	andi	$4,$3,0x1000
	beq	$4,$0,$L414
	lui	$4,%hi(lan_addr)

	lw	$6,%lo(lan_addr)($4)
	lui	$5,%hi(lan_mask)
	lw	$4,12($16)
	lw	$5,%lo(lan_mask)($5)
	xor	$4,$6,$4
	and	$4,$4,$5
	bne	$4,$0,$L416
	move	$4,$0

	lbu	$5,9($16)
	li	$4,1			# 0x1
	beq	$5,$4,$L415
	lui	$4,%hi(LogFlag)

	lw	$5,%lo(LogFlag)($4)
	nop
	ori	$5,$5,0x1000
	sw	$5,%lo(LogFlag)($4)
	lw	$6,12($16)
	lui	$5,%hi(attack_saddr+32)
	li	$4,-1			# 0xffffffff
	sw	$6,%lo(attack_saddr+32)($5)
	lw	$6,16($16)
	lui	$5,%hi(attack_daddr+32)
	j	$L416
	sw	$6,%lo(attack_daddr+32)($5)

$L415:
	move	$4,$0
$L416:
	or	$2,$2,$4
$L414:
	andi	$3,$3,0x2000
	beq	$3,$0,$L417
	li	$4,36			# 0x24

	lw	$3,12($16)
	nop
	andi	$5,$3,0x3ff
	mult	$5,$4
	andi	$5,$3,0xff
	lui	$4,%hi(ConnectedIp)
	sll	$5,$5,2
	addiu	$4,$4,%lo(ConnectedIp)
	addu	$4,$5,$4
	lw	$5,0($4)
	lui	$4,%hi(dos_pkt)
	addiu	$4,$4,%lo(dos_pkt)
	mflo	$6
	bne	$3,$5,$L418
	addu	$4,$4,$6

	lw	$5,24($4)
	nop
	bne	$5,$3,$L418
	nop

	j	$L419
	sb	$0,0($4)

$L418:
	lw	$6,24($4)
	lb	$5,0($4)
	beq	$6,$3,$L420
	nop

	bne	$5,$0,$L419
	lui	$5,%hi(cur_p_pkt)

	sw	$4,%lo(cur_p_pkt)($5)
	sw	$3,24($4)
	li	$3,1			# 0x1
	j	$L421
	sb	$3,0($4)

$L420:
	li	$3,1			# 0x1
	bne	$5,$3,$L419
	lui	$3,%hi(cur_p_pkt)

	j	$L457
	sw	$4,%lo(cur_p_pkt)($3)

$L426:
	bne	$4,$0,$L424
	move	$4,$0

$L427:
	lhu	$4,6($16)
	nop
	andi	$6,$4,0x3fff
	beq	$6,$0,$L419
	sll	$7,$4,3

	lhu	$6,30($3)
	andi	$7,$7,0xffff
	slt	$7,$7,$6
	bne	$7,$0,$L422
	nop

	andi	$4,$4,0x2000
	bne	$4,$0,$L423
	nop

	sh	$0,30($3)
	j	$L424
	sh	$0,28($3)

$L423:
	sh	$5,28($3)
	lbu	$5,0($16)
	lhu	$7,2($16)
	move	$4,$0
	andi	$5,$5,0xf
	addu	$6,$7,$6
	sll	$5,$5,2
	subu	$6,$6,$5
	j	$L424
	sh	$6,30($3)

$L422:
	lui	$3,%hi(LogFlag)
	lw	$4,%lo(LogFlag)($3)
	nop
	ori	$4,$4,0x2000
	sw	$4,%lo(LogFlag)($3)
	lw	$5,12($16)
	lui	$3,%hi(attack_saddr+36)
	li	$4,-1			# 0xffffffff
	sw	$5,%lo(attack_saddr+36)($3)
	lw	$5,16($16)
	lui	$3,%hi(attack_daddr+36)
	j	$L424
	sw	$5,%lo(attack_daddr+36)($3)

$L419:
	move	$4,$0
$L424:
	or	$2,$2,$4
$L417:
	bne	$2,$0,$L357
	move	$2,$0

$L363:
	jal	rtl_ip_hdr
	move	$4,$17

	lbu	$3,9($2)
	li	$2,1			# 0x1
	beq	$3,$2,$L358
	lui	$3,%hi(none_dos_drop_pkt_cnt)

	lw	$4,%lo(none_dos_drop_pkt_cnt)($3)
	nop
$L431:
	addiu	$4,$4,1
	j	$L357
	sw	$4,%lo(none_dos_drop_pkt_cnt)($3)

$L358:
	j	$L357
	li	$2,4			# 0x4

$L361:
	move	$2,$0
$L357:
	lw	$31,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,40

$L368:
	andi	$2,$3,0x22
	bne	$2,$0,$L425
	move	$2,$0

	j	$L458
	andi	$4,$3,0x44

$L421:
	lui	$3,%hi(cur_p_pkt)
$L457:
	lw	$3,%lo(cur_p_pkt)($3)
	lhu	$5,4($16)
	lhu	$4,28($3)
	nop
	bne	$5,$4,$L426
	nop

	j	$L427
	nop

	.set	macro
	.set	reorder
	.end	filter_enter
	.size	filter_enter, .-filter_enter
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
	lw	$2,%lo(table_list_head)($2)
	j	$L460
	nop

$L464:
	lw	$7,16($2)
	nop
	bne	$7,$4,$L461
	nop

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
	bne	$4,$2,$L463
	move	$2,$0

	li	$2,2			# 0x2
	bne	$3,$2,$L462
	nop

	jal	Add_Pattern_ACL_For_ContentFilter
	nop

	j	$L463
	move	$2,$0

$L461:
	move	$2,$6
$L460:
	lw	$6,0($2)
	bne	$2,$3,$L464
	nop

	j	$L463
	li	$2,-1			# 0xffffffff

$L462:
	move	$2,$0
$L463:
	lw	$31,20($sp)
	nop
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
	j	$L467
	li	$21,1			# 0x1

$L471:
	lw	$2,8($16)
	nop
	lw	$23,0($2)
	j	$L468
	addiu	$fp,$16,8

$L469:
	lw	$3,4($2)
	lw	$5,0($2)
	nop
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
$L468:
	bne	$2,$fp,$L469
	move	$4,$2

	lw	$2,16($16)
	nop
	bne	$2,$22,$L470
	nop

	lw	$2,24($16)
	nop
	bne	$2,$21,$L470
	nop

	jal	Del_Pattern_ACL_For_ContentFilter
	nop

$L470:
	lw	$2,4($16)
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
$L467:
	bne	$16,$18,$L471
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
	.ascii	":\000"
	.align	2
$LC51:
	.ascii	";\000"
	.align	2
$LC52:
	.ascii	"Not enough memory for filter table...\012\000"
	.align	2
$LC53:
	.ascii	"-\000"
	.section	.text.filter_table_write_proc,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	filter_table_write_proc
	.type	filter_table_write_proc, @function
filter_table_write_proc:
	.frame	$sp,88,$31		# vars= 32, regs= 10/0, args= 16, gp= 0
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
$L474 = .
$L475 = .
	addiu	$sp,$sp,-88
	sw	$31,84($sp)
	sw	$fp,80($sp)
	sw	$23,76($sp)
	sw	$22,72($sp)
	sw	$21,68($sp)
	sw	$20,64($sp)
	sw	$19,60($sp)
	sw	$18,56($sp)
	sw	$17,52($sp)
	sw	$16,48($sp)
	lui	$2,%hi(malloc_sizes+60)
	lw	$4,%lo(malloc_sizes+60)($2)
	move	$19,$5
	li	$2,22			# 0x16
	li	$5,208			# 0xd0
	sb	$2,16($sp)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	move	$17,$6
	.set	macro
	.set	reorder

	move	$16,$2
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L477
	li	$18,-14			# 0xfffffff2
	.set	macro
	.set	reorder

	lui	$20,%hi(get_info_from_usr_space)
	addiu	$4,$20,%lo(get_info_from_usr_space)
	.set	noreorder
	.set	nomacro
	jal	strlen
	sw	$2,32($sp)
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
	beq	$19,$0,$L478
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
	bne	$2,$0,$L480
	move	$6,$17
	.set	macro
	.set	reorder

	addiu	$4,$20,%lo(get_info_from_usr_space)
	move	$5,$19
#APP
 # 671 "net/rtl/fastpath/filter_v2.c" 1
	.set	noreorder
	jal	__copy_user
	.set	noat
	addu	$1, $5, $6
	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
$L480:
	bne	$6,$0,$L478
	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L481
	lui	$3,%hi(get_info_from_usr_space)
	.set	macro
	.set	reorder

	addiu	$3,$3,%lo(get_info_from_usr_space)
	move	$2,$16
#APP
 # 50 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
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
$L481:
	lui	$4,%hi(get_info_from_usr_space)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$4,%lo(get_info_from_usr_space)
	.set	macro
	.set	reorder

	addu	$2,$16,$2
	sb	$0,-1($2)
	lw	$19,32($sp)
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
	bne	$2,$0,$L482
	lui	$17,%hi($LC45)
	.set	macro
	.set	reorder

	lui	$17,%hi($LC28)
	addiu	$19,$sp,32
	addiu	$5,$17,%lo($LC28)
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$4,$19
	.set	macro
	.set	reorder

	lui	$20,%hi(filter_tlb)
	sw	$2,20($sp)
	addiu	$17,$17,%lo($LC28)
	addiu	$20,$20,%lo(filter_tlb)
	move	$5,$17
$L527:
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$4,$19
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	sw	$2,20($sp)
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

	lw	$2,32($sp)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L527
	move	$5,$17
	.set	macro
	.set	reorder

	j	$L484
$L482:
	lw	$19,32($sp)
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
	bne	$2,$0,$L485
	lui	$17,%hi($LC46)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	filter_table_flush
	move	$4,$0
	.set	macro
	.set	reorder

	j	$L484
$L485:
	lw	$19,32($sp)
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
	bne	$2,$0,$L486
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	lui	$2,%hi(log_enable)
	.set	noreorder
	.set	nomacro
	j	$L484
	sw	$3,%lo(log_enable)($2)
	.set	macro
	.set	reorder

$L486:
	lw	$19,32($sp)
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
	bne	$2,$0,$L487
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	lui	$2,%hi(url_filter_mode)
	.set	noreorder
	.set	nomacro
	j	$L484
	sw	$3,%lo(url_filter_mode)($2)
	.set	macro
	.set	reorder

$L487:
	lw	$19,32($sp)
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
	bne	$2,$0,$L488
	lui	$2,%hi(url_filter_mode)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L484
	sw	$0,%lo(url_filter_mode)($2)
	.set	macro
	.set	reorder

$L488:
	lw	$19,32($sp)
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
	bne	$2,$0,$L484
	lui	$5,%hi($LC50)
	.set	macro
	.set	reorder

	addiu	$5,$5,%lo($LC50)
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$4,$sp,32
	.set	macro
	.set	reorder

	lui	$19,%hi($LC28)
	lui	$23,%hi(malloc_sizes)
	sw	$2,24($sp)
	addiu	$19,$19,%lo($LC28)
	addiu	$23,$23,%lo(malloc_sizes)
	addiu	$20,$sp,20
	lui	$21,%hi(filter_rule_num)
	addiu	$22,$sp,36
	lui	$5,%hi($LC51)
$L530:
	addiu	$4,$sp,32
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$5,%lo($LC51)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L484
	sw	$2,24($sp)
	.set	macro
	.set	reorder

	move	$4,$2
	move	$5,$19
	.set	noreorder
	.set	nomacro
	jal	memcmp
	li	$6,1			# 0x1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L528
	addiu	$fp,$sp,24
	.set	macro
	.set	reorder

	lw	$4,24($sp)
	jal	strlen
	li	$3,1			# 0x1
	beq	$2,$3,$L484
$L489:
	addiu	$fp,$sp,24
$L528:
	addiu	$17,$sp,16
	move	$4,$fp
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$17
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	sw	$2,20($sp)
	.set	macro
	.set	reorder

	li	$3,-1			# 0xffffffff
	beq	$2,$3,$L484
	addiu	$4,$sp,24
$L529:
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$sp,16
	.set	macro
	.set	reorder

	move	$5,$19
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	strsep
	sw	$2,20($sp)
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	move	$4,$20
	move	$5,$19
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$17,$2
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	move	$fp,$2
	li	$2,2			# 0x2
	.set	noreorder
	.set	nomacro
	beq	$17,$2,$L497
	slt	$2,$17,3
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L495
	li	$2,3			# 0x3
	.set	macro
	.set	reorder

	li	$2,1			# 0x1
	bne	$17,$2,$L490
	j	$L516
$L495:
	.set	noreorder
	.set	nomacro
	beq	$17,$2,$L514
	li	$2,4			# 0x4
	.set	macro
	.set	reorder

	bne	$17,$2,$L490
	j	$L515
$L516:
	lw	$4,20($23)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L525
	move	$17,$2
	.set	macro
	.set	reorder

	addiu	$2,$2,8
	sw	$2,12($17)
	sw	$17,0($17)
	sw	$17,4($17)
	sw	$2,8($17)
	sw	$0,24($17)
	lw	$2,%lo(filter_rule_num)($21)
	move	$5,$19
	move	$4,$20
	sw	$fp,16($17)
	.set	noreorder
	.set	nomacro
	jal	strsep
	sw	$2,20($17)
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	li	$6,4			# 0x4
	addiu	$4,$17,28
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	sw	$2,36($sp)
	.set	macro
	.set	reorder

	move	$5,$19
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	addiu	$4,$17,32
	move	$5,$22
	li	$6,4			# 0x4
	.set	noreorder
	.set	nomacro
	jal	memcpy
	sw	$2,36($sp)
	.set	macro
	.set	reorder

	move	$5,$17
	.set	noreorder
	.set	nomacro
	jal	filter_item_regist
	li	$4,1			# 0x1
	.set	macro
	.set	reorder

	lw	$2,20($sp)
	#nop
	bne	$2,$0,$L516
	j	$L490
$L492 = .
$L497:
	lw	$4,20($23)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L525
	move	$17,$2
	.set	macro
	.set	reorder

	addiu	$2,$2,8
	sw	$2,12($17)
	sw	$17,0($17)
	sw	$17,4($17)
	sw	$2,8($17)
	sw	$0,24($17)
	lw	$2,%lo(filter_rule_num)($21)
	move	$4,$20
	sw	$fp,16($17)
	sw	$2,20($17)
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$19
	.set	macro
	.set	reorder

	sw	$2,28($sp)
	move	$fp,$17
$L499:
	lui	$5,%hi($LC53)
	addiu	$5,$5,%lo($LC53)
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$4,$sp,28
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	sb	$2,28($fp)
	lw	$2,28($sp)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L499
	addiu	$fp,$fp,1
	.set	macro
	.set	reorder

$L500 = .
	move	$5,$17
	.set	noreorder
	.set	nomacro
	jal	filter_item_regist
	li	$4,2			# 0x2
	.set	macro
	.set	reorder

	j	$L490
$L514:
	lw	$4,20($23)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L525
	move	$17,$2
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$19
	.set	macro
	.set	reorder

	sw	$2,28($sp)
	lb	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L502
	addiu	$2,$17,8
	.set	macro
	.set	reorder

	sw	$2,12($17)
	sw	$17,0($17)
	sw	$17,4($17)
	sw	$2,8($17)
	sw	$0,24($17)
	lw	$2,%lo(filter_rule_num)($21)
	sw	$fp,16($17)
	sw	$2,20($17)
	lw	$4,28($sp)
	jal	strlen
	sltu	$3,$2,41
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L504
	li	$2,39			# 0x27
	.set	macro
	.set	reorder

	lw	$4,28($sp)
	jal	strlen
$L504:
	lw	$4,28($sp)
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L505
	addiu	$3,$17,28
	.set	macro
	.set	reorder

	move	$5,$2
#APP
 # 50 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($4)
	subu	$5,1
	sb	$1,($3)
	beqz	$1,2f
	addiu	$3,1
	bnez	$5,1b
	addiu	$4,1
2:
	.set	at
	.set	reorder
 # 0 "" 2
#NO_APP
$L505:
	addu	$2,$17,$2
	sb	$0,28($2)
	move	$5,$17
	.set	noreorder
	.set	nomacro
	jal	filter_item_regist
	li	$4,3			# 0x3
	.set	macro
	.set	reorder

$L502:
	lw	$2,20($sp)
	#nop
	bne	$2,$0,$L514
	j	$L490
$L515:
	lw	$4,20($23)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L506
	move	$17,$2
	.set	macro
	.set	reorder

$L525:
	lui	$4,%hi($LC52)
	.set	noreorder
	.set	nomacro
	jal	panic_printk
	addiu	$4,$4,%lo($LC52)
	.set	macro
	.set	reorder

	j	$L484
$L506:
	addiu	$2,$2,8
	sw	$2,12($17)
	sw	$17,0($17)
	sw	$17,4($17)
	sw	$2,8($17)
	sw	$0,24($17)
	lw	$2,%lo(filter_rule_num)($21)
	move	$5,$19
	move	$4,$20
	sw	$fp,16($17)
	.set	noreorder
	.set	nomacro
	jal	strsep
	sw	$2,20($17)
	.set	macro
	.set	reorder

	move	$4,$2
	addiu	$3,$17,28
	sw	$3,40($sp)
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	lw	$4,40($sp)
	li	$6,4			# 0x4
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	sw	$2,36($sp)
	.set	macro
	.set	reorder

	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$19
	.set	macro
	.set	reorder

	lw	$3,36($sp)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L507
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L507
	li	$5,1440			# 0x5a0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L508
	sw	$5,36($sp)
	.set	macro
	.set	reorder

$L507:
	lw	$4,28($sp)
	jal	str2hexnum
	sw	$2,36($sp)
$L508:
	lw	$2,40($sp)
	li	$6,4			# 0x4
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$4,$2,4
	.set	macro
	.set	reorder

	move	$5,$19
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$4,$20
	.set	macro
	.set	reorder

	move	$4,$2
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	sw	$2,28($sp)
	.set	macro
	.set	reorder

	lw	$5,40($sp)
	li	$3,255			# 0xff
	movn	$3,$2,$2
	addiu	$4,$5,8
	li	$6,4			# 0x4
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	sw	$3,36($sp)
	.set	macro
	.set	reorder

	move	$5,$17
	.set	noreorder
	.set	nomacro
	jal	filter_item_regist
	li	$4,4			# 0x4
	.set	macro
	.set	reorder

	lw	$2,20($sp)
	#nop
	bne	$2,$0,$L515
$L490:
	lw	$2,24($sp)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L529
	addiu	$4,$sp,24
	.set	macro
	.set	reorder

	lw	$2,%lo(filter_rule_num)($21)
	#nop
	addiu	$2,$2,1
	sw	$2,%lo(filter_rule_num)($21)
	lw	$2,32($sp)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L530
	lui	$5,%hi($LC51)
	.set	macro
	.set	reorder

$L484:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L526
	move	$2,$18
	.set	macro
	.set	reorder

$L478:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	li	$18,-14			# 0xfffffff2
$L477:
	move	$2,$18
$L526:
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
	j	$31
	addiu	$sp,$sp,88
	.set	macro
	.set	reorder

	.end	filter_table_write_proc
	.size	filter_table_write_proc, .-filter_table_write_proc
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
$L537:
	move	$4,$16
	lbu	$19,0($2)
	jal	rtl_skb_mac_header
	sw	$5,16($sp)

	addu	$2,$2,$17
	lbu	$2,6($2)
	lw	$5,16($sp)
	beq	$19,$2,$L532
	addiu	$17,$17,1

	j	$L533
	move	$2,$0

$L532:
	bne	$17,$18,$L537
	addu	$2,$5,$17

	li	$2,1			# 0x1
$L533:
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
	.section	.text.T.860,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	T.860
	.type	T.860, @function
T.860:
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
	j	$L539
	li	$23,10			# 0xa

$L549:
	lb	$2,0($22)
	nop
	bne	$2,$fp,$L540
	nop

	andi	$2,$20,0x1
	beq	$2,$0,$L542
	addiu	$20,$20,1

	j	$L544
	move	$20,$0

$L540:
	bne	$2,$23,$L541
	andi	$2,$20,0x1

	beq	$2,$0,$L541
	addiu	$20,$20,1

$L542:
	slt	$2,$20,4
	beq	$2,$0,$L543
	move	$4,$22

	j	$L552
	move	$5,$8

$L541:
	move	$20,$0
$L544:
	move	$4,$22
	move	$5,$8
$L552:
	move	$6,$7
	sw	$3,24($sp)
	sw	$7,20($sp)
	jal	memcmp
	sw	$8,16($sp)

	lw	$3,24($sp)
	lw	$7,20($sp)
	lw	$8,16($sp)
	beq	$2,$0,$L545
	addiu	$22,$22,1

	j	$L539
	addiu	$21,$21,1

$L545:
	addu	$21,$21,$7
	sw	$21,0($18)
	addu	$17,$17,$21
	move	$2,$0
	j	$L546
	li	$3,13			# 0xd

$L547:
	bne	$5,$0,$L543
	addiu	$17,$17,1

	addiu	$2,$2,1
$L546:
	lb	$4,0($17)
	addu	$5,$2,$21
	bne	$4,$3,$L547
	sltu	$5,$19,$5

	sw	$2,0($16)
	j	$L548
	li	$2,1			# 0x1

$L539:
	sltu	$2,$3,$21
	beq	$2,$0,$L549
	nop

$L543:
	move	$2,$0
$L548:
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
	.end	T.860
	.size	T.860, .-T.860
	.section	.rodata.str1.4
	.align	2
$LC54:
	.ascii	"GET \000"
	.align	2
$LC55:
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
	bne	$2,$0,$L558
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
	bne	$3,$2,$L558
	move	$17,$0
	.set	macro
	.set	reorder

$L555 = .
$L556 = .
	lbu	$21,0($19)
	lui	$2,%hi(malloc_sizes+68)
	lw	$4,%lo(malloc_sizes+68)($2)
	andi	$21,$21,0xf
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
	beq	$2,$0,$L554
	move	$16,$2
	.set	macro
	.set	reorder

	sll	$17,$17,2
	lbu	$20,0($19)
	addu	$17,$21,$17
	lui	$5,%hi($LC54)
	addiu	$5,$5,%lo($LC54)
	move	$4,$17
	li	$6,4			# 0x4
	lhu	$21,2($19)
	.set	noreorder
	.set	nomacro
	jal	memcmp
	andi	$20,$20,0xf
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L561
	li	$2,-8			# 0xfffffff8
	.set	macro
	.set	reorder

	mult	$20,$2
	lui	$6,%hi($LC55)
	addiu	$19,$sp,24
	addiu	$2,$sp,28
	addiu	$6,$6,%lo($LC55)
	move	$4,$17
	li	$7,6			# 0x6
	sw	$2,20($sp)
	sw	$19,16($sp)
	mflo	$20
	addu	$20,$20,$21
	.set	noreorder
	.set	nomacro
	jal	T.860
	move	$5,$20
	.set	macro
	.set	reorder

	beq	$2,$0,$L561
	lw	$4,28($sp)
	lw	$3,24($sp)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L560
	addu	$3,$17,$3
	.set	macro
	.set	reorder

	move	$2,$16
#APP
 # 50 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
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
$L560:
	lw	$2,28($sp)
	lui	$6,%hi($LC54)
	move	$5,$20
	addu	$2,$16,$2
	sb	$0,0($2)
	addiu	$6,$6,%lo($LC54)
	addiu	$2,$sp,32
	move	$4,$17
	li	$7,4			# 0x4
	sw	$19,16($sp)
	.set	noreorder
	.set	nomacro
	jal	T.860
	sw	$2,20($sp)
	.set	macro
	.set	reorder

	beq	$2,$0,$L561
	lw	$3,32($sp)
	#nop
	addiu	$3,$3,-9
	.set	noreorder
	.set	nomacro
	bgtz	$3,$L562
	sw	$3,32($sp)
	.set	macro
	.set	reorder

$L561:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L558
	move	$17,$0
	.set	macro
	.set	reorder

$L562:
	lw	$2,28($sp)
	lw	$4,24($sp)
	addu	$2,$16,$2
	addu	$17,$17,$4
#APP
 # 50 "/backup/qian_yu/svn/branch-rtl819x-sdk-v3.4.6/linux-2.6.30/arch/rlx/include/asm/string.h" 1
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
	bne	$17,$0,$L564
	move	$20,$2
	.set	macro
	.set	reorder

	addu	$19,$16,$2
	.set	noreorder
	.set	nomacro
	j	$L565
	move	$17,$16
	.set	macro
	.set	reorder

$L567:
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	memcmp
	move	$6,$20
	.set	macro
	.set	reorder

	addiu	$19,$19,1
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L565
	addiu	$17,$17,1
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L566
	li	$17,1			# 0x1
	.set	macro
	.set	reorder

$L565:
	lb	$2,0($19)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L567
	move	$5,$18
	.set	macro
	.set	reorder

$L564:
	move	$17,$0
$L566:
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
	bne	$2,$3,$L558
	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L568
	lui	$3,%hi(url_filter_flag)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L558
	li	$17,1			# 0x1
	.set	macro
	.set	reorder

$L568:
	.set	noreorder
	.set	nomacro
	j	$L558
	sw	$2,%lo(url_filter_flag)($3)
	.set	macro
	.set	reorder

$L554:
	move	$17,$0
$L558:
	move	$2,$17
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
	.data
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
	.globl	fast_path_cusRsp401_func
	.section	.bss,"aw",@nobits
	.align	2
	.type	fast_path_cusRsp401_func, @object
	.size	fast_path_cusRsp401_func, 4
fast_path_cusRsp401_func:
	.space	4
	.globl	fast_path_cusRspTCPFinAck_func
	.align	2
	.type	fast_path_cusRspTCPFinAck_func, @object
	.size	fast_path_cusRspTCPFinAck_func, 4
fast_path_cusRspTCPFinAck_func:
	.space	4
	.globl	fast_path_cusRspTCPEndAck_func
	.align	2
	.type	fast_path_cusRspTCPEndAck_func, @object
	.size	fast_path_cusRspTCPEndAck_func, 4
fast_path_cusRspTCPEndAck_func:
	.space	4
	.local	res3
	.comm	res3,4,4
	.section	.dram-l34-fwd,"aw",@progbits
	.align	2
	.type	item, @object
	.size	item, 4
item:
	.space	4
	.align	2
	.type	none_dos_drop_pkt_cnt, @object
	.size	none_dos_drop_pkt_cnt, 4
none_dos_drop_pkt_cnt:
	.space	4
	.type	url_count, @object
	.size	url_count, 1
url_count:
	.space	1
	.local	block_count
	.comm	block_count,4,4
	.local	block_time
	.comm	block_time,4,4
	.local	block
	.comm	block,4,4
	.local	item_backup
	.comm	item_backup,4,4
	.local	url_cache
	.comm	url_cache,20,4
	.local	endStateClientPort
	.comm	endStateClientPort,2,2
	.local	endStateClientSeq
	.comm	endStateClientSeq,4,4
	.local	finStateClientPort
	.comm	finStateClientPort,2,2
	.local	finStateClientSeq
	.comm	finStateClientSeq,4,4
	.local	block_message
	.comm	block_message,4,4
	.local	res12
	.comm	res12,4,4
	.local	log_enable
	.comm	log_enable,4,4
	.local	url_filter_mode
	.comm	url_filter_mode,4,4
	.local	url_filter_flag
	.comm	url_filter_flag,4,4
	.local	get_info_from_usr_space
	.comm	get_info_from_usr_space,1024,4
	.local	log_info
	.comm	log_info,32,4
	.local	filter_rule_num
	.comm	filter_rule_num,4,4
	.local	LogFlag
	.comm	LogFlag,4,4
	.local	ConnectedIp
	.comm	ConnectedIp,1024,4
	.local	dos_pkt
	.comm	dos_pkt,36864,4
	.local	cur_p_pkt
	.comm	cur_p_pkt,4,4
	.local	dos_timer
	.comm	dos_timer,24,4
	.local	_tcpDosScanBitmap
	.comm	_tcpDosScanBitmap,64,4
	.local	op_mode
	.comm	op_mode,1,1
	.local	whole_syn_threshold
	.comm	whole_syn_threshold,4,4
	.local	whole_fin_threshold
	.comm	whole_fin_threshold,4,4
	.local	whole_udp_threshold
	.comm	whole_udp_threshold,4,4
	.local	whole_icmp_threshold
	.comm	whole_icmp_threshold,4,4
	.local	per_syn_threshold
	.comm	per_syn_threshold,4,4
	.local	per_fin_threshold
	.comm	per_fin_threshold,4,4
	.local	per_udp_threshold
	.comm	per_udp_threshold,4,4
	.local	per_icmp_threshold
	.comm	per_icmp_threshold,4,4
	.local	lan_addr
	.comm	lan_addr,4,4
	.local	lan_mask
	.comm	lan_mask,4,4
	.local	attack_saddr
	.comm	attack_saddr,60,4
	.local	attack_daddr
	.comm	attack_daddr,60,4
	.local	dos_flag
	.comm	dos_flag,80,4
	.globl	table_list_head
	.section	.bss
	.align	2
	.type	table_list_head, @object
	.size	table_list_head, 32
table_list_head:
	.space	32
	.globl	wan_dev
	.align	2
	.type	wan_dev, @object
	.size	wan_dev, 4
wan_dev:
	.space	4
	.globl	wan_ppp_dev
	.align	2
	.type	wan_ppp_dev, @object
	.size	wan_ppp_dev, 4
wan_ppp_dev:
	.space	4
	.globl	___strtok
	.align	2
	.type	___strtok, @object
	.size	___strtok, 4
___strtok:
	.space	4
	.ident	"GCC: (GNU) 4.4.5-1.5.5p2"
