	.file	1 "filter.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.section	.text.dos_pkt_locate,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	dos_pkt_locate
	.type	dos_pkt_locate, @function
dos_pkt_locate:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	lw	$3,12($4)
	li	$2,36			# 0x24
	andi	$4,$3,0x3ff
	mult	$4,$2
	lui	$4,%hi(dos_pkt)
	addiu	$4,$4,%lo(dos_pkt)
	andi	$5,$3,0xff
	sll	$5,$5,2
	mflo	$2
	addu	$4,$4,$2
	lui	$2,%hi(ConnectedIp)
	addiu	$2,$2,%lo(ConnectedIp)
	addu	$2,$5,$2
	lw	$5,0($2)
	lw	$2,24($4)
	bne	$3,$5,$L2
	nop

	bne	$2,$3,$L2
	nop

	sb	$0,0($4)
	j	$31
	move	$2,$0

$L2:
	lb	$5,0($4)
	beq	$2,$3,$L4
	nop

	bne	$5,$0,$L7
	move	$2,$0

	li	$2,1			# 0x1
	sb	$2,0($4)
	lui	$2,%hi(cur_p_pkt)
	sw	$4,%lo(cur_p_pkt)($2)
	sw	$3,24($4)
	j	$31
	li	$2,1			# 0x1

$L4:
	li	$3,1			# 0x1
	bne	$5,$3,$L7
	move	$2,$0

	lui	$2,%hi(cur_p_pkt)
	sw	$4,%lo(cur_p_pkt)($2)
	li	$2,1			# 0x1
$L7:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	dos_pkt_locate
	.size	dos_pkt_locate, .-dos_pkt_locate
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
	li	$4,65536			# 0x10000
	addiu	$4,$4,20864
	div	$0,$3,$4
	li	$6,7			# 0x7
	lw	$5,24($sp)
	li	$4,1			# 0x1
	mflo	$2
	addiu	$2,$2,4
	div	$0,$2,$6
	lw	$6,8($5)
	mfhi	$2
	sll	$4,$4,$2
	ori	$4,$4,0x80
	and	$4,$4,$6
	beq	$4,$0,$L12
	move	$2,$0

	li	$6,3600			# 0xe10
	div	$0,$3,$6
	li	$4,24			# 0x18
	mflo	$6
	div	$0,$6,$4
	li	$4,60			# 0x3c
	mfhi	$6
	mult	$6,$4
	mflo	$6
	div	$0,$3,$4
	mflo	$3
	div	$0,$3,$4
	lw	$4,0($5)
	mfhi	$3
	bne	$4,$0,$L13
	addu	$3,$6,$3

	lw	$4,4($5)
	nop
	bne	$4,$0,$L14
	li	$2,1			# 0x1

	j	$L12
	nop

$L13:
	sltu	$4,$3,$4
	bne	$4,$0,$L12
	nop

$L14:
	lw	$2,4($5)
	nop
	sltu	$3,$2,$3
	xori	$2,$3,0x1
$L12:
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
	bne	$4,$0,$L19
	move	$2,$0

	lw	$2,4($5)
	nop
	sltu	$2,$2,$3
	xori	$2,$2,0x1
$L19:
	lw	$31,28($sp)
	nop
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	rtl_ip_range_filter_fastpath
	.size	rtl_ip_range_filter_fastpath, .-rtl_ip_range_filter_fastpath
	.section	.rodata.str1.4
	.align	2
$LC1:
	.ascii	"DoS: Whole System SYN Flood Attack\012\000"
	.align	2
$LC2:
	.ascii	"DoS: Whole System FIN Flood Attack\012\000"
	.align	2
$LC3:
	.ascii	"DoS: Whole System UDP Flood Attack\012\000"
	.align	2
$LC4:
	.ascii	"DoS: Whole System ICMP Flood Attack\012\000"
	.align	2
$LC5:
	.ascii	"DoS: Per-source SYN Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC6:
	.ascii	"DoS: Per-source FIN Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC7:
	.ascii	"DoS: Per-source UDP Flood Attack source=%u.%u.%u.%u dest"
	.ascii	"ination=%u.%u.%u.%u\012\000"
	.align	2
$LC8:
	.ascii	"DoS: Per-source ICMP Flood Attack source=%u.%u.%u.%u des"
	.ascii	"tination=%u.%u.%u.%u\012\000"
	.align	2
$LC9:
	.ascii	"DoS: Port Scan Attack source=%u.%u.%u.%u destination=%u."
	.ascii	"%u.%u.%u\012\000"
	.align	2
$LC10:
	.ascii	"DoS: Tcp Scan Attack source=%u.%u.%u.%u destination=%u.%"
	.ascii	"u.%u.%u\012\000"
	.align	2
$LC11:
	.ascii	"DoS: Tcp SYN With Data Attack source=%u.%u.%u.%u destina"
	.ascii	"tion=%u.%u.%u.%u\012\000"
	.align	2
$LC12:
	.ascii	"DoS: IP Land Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC13:
	.ascii	"DoS: UdpEchoChargen Attack source=%u.%u.%u.%u destinatio"
	.ascii	"n=%u.%u.%u.%u\012\000"
	.align	2
$LC14:
	.ascii	"DoS: UdpBomb Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC15:
	.ascii	"DoS: PingOfDeath Attack source=%u.%u.%u.%u destination=%"
	.ascii	"u.%u.%u.%u\012\000"
	.align	2
$LC16:
	.ascii	"DoS: IcmpSmurf Attack source=%u.%u.%u.%u destination=%u."
	.ascii	"%u.%u.%u\012\000"
	.align	2
$LC17:
	.ascii	"DoS: IpSpoof Attack source=%u.%u.%u.%u destination=%u.%u"
	.ascii	".%u.%u\012\000"
	.align	2
$LC18:
	.ascii	"DoS: TearDrop Attack source=%u.%u.%u.%u destination=%u.%"
	.ascii	"u.%u.%u\012\000"
	.section	.text.dos_timer_fn,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	dos_timer_fn
	.type	dos_timer_fn, @function
dos_timer_fn:
	.frame	$sp,64,$31		# vars= 0, regs= 5/0, args= 40, gp= 0
	.mask	0x800f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-64
	sw	$31,60($sp)
	sw	$19,56($sp)
	sw	$18,52($sp)
	sw	$17,48($sp)
	sw	$16,44($sp)
	lui	$17,%hi(item)
	lw	$3,%lo(item)($17)
	nop
	beq	$3,$0,$L22
	lui	$2,%hi(block_count)

	lui	$4,%hi(block_time)
	lw	$4,%lo(block_time)($4)
	lw	$5,%lo(block_count)($2)
	nop
	slt	$5,$5,$4
	bne	$5,$0,$L23
	lui	$4,%hi(block)

	sw	$0,%lo(block)($4)
	sw	$0,%lo(block_count)($2)
$L23:
	lw	$5,%lo(block)($4)
	li	$4,1			# 0x1
	bne	$5,$4,$L24
	nop

	lw	$4,%lo(block_count)($2)
	nop
	addiu	$4,$4,1
	sw	$4,%lo(block_count)($2)
$L24:
	andi	$2,$3,0x1e
	beq	$2,$0,$L25
	lui	$18,%hi(dos_pkt)

	addiu	$2,$18,%lo(dos_pkt)
	li	$14,36864			# 0x9000
	addu	$14,$2,$14
	move	$4,$0
	move	$5,$0
	move	$6,$0
	move	$7,$0
	li	$13,1			# 0x1
	andi	$12,$3,0x2
	andi	$11,$3,0x4
	andi	$10,$3,0x8
	andi	$9,$3,0x10
$L30:
	lb	$8,0($2)
	nop
	bne	$8,$13,$L26
	nop

	beq	$12,$0,$L27
	nop

	lw	$8,4($2)
	nop
	addu	$15,$7,$8
	slt	$8,$0,$8
	j	$L27
	movn	$7,$15,$8

$L57:
	lw	$8,8($2)
	nop
	addu	$15,$6,$8
	slt	$8,$0,$8
	j	$L28
	movn	$6,$15,$8

$L58:
	lw	$8,12($2)
	nop
	addu	$15,$5,$8
	slt	$8,$0,$8
	j	$L29
	movn	$5,$15,$8

$L59:
	lw	$8,16($2)
	nop
	addu	$15,$4,$8
	slt	$8,$0,$8
	movn	$4,$15,$8
$L26:
	addiu	$2,$2,36
$L63:
	bne	$2,$14,$L30
	nop

	lui	$2,%hi(whole_syn_threshold)
	lw	$8,%lo(whole_syn_threshold)($2)
	nop
	slt	$7,$8,$7
	beq	$7,$0,$L31
	lui	$2,%hi(LogFlag)

	andi	$3,$3,0x2
	beq	$3,$0,$L64
	lui	$3,%hi(whole_fin_threshold)

	blez	$8,$L64
	nop

	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x2
	sw	$3,%lo(LogFlag)($2)
$L31:
	lui	$3,%hi(whole_fin_threshold)
$L64:
	lw	$3,%lo(whole_fin_threshold)($3)
	nop
	slt	$6,$3,$6
	beq	$6,$0,$L32
	nop

	lw	$6,%lo(item)($17)
	nop
	andi	$6,$6,0x4
	beq	$6,$0,$L32
	nop

	blez	$3,$L65
	lui	$3,%hi(whole_udp_threshold)

	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x4
	sw	$3,%lo(LogFlag)($2)
$L32:
	lui	$3,%hi(whole_udp_threshold)
$L65:
	lw	$3,%lo(whole_udp_threshold)($3)
	nop
	slt	$5,$3,$5
	beq	$5,$0,$L33
	nop

	lw	$5,%lo(item)($17)
	nop
	andi	$5,$5,0x8
	beq	$5,$0,$L33
	nop

	blez	$3,$L66
	lui	$3,%hi(whole_icmp_threshold)

	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x8
	sw	$3,%lo(LogFlag)($2)
$L33:
	lui	$3,%hi(whole_icmp_threshold)
$L66:
	lw	$3,%lo(whole_icmp_threshold)($3)
	nop
	slt	$4,$3,$4
	beq	$4,$0,$L67
	lui	$19,%hi(LogFlag)

	lw	$4,%lo(item)($17)
	nop
	andi	$4,$4,0x10
	beq	$4,$0,$L67
	nop

	blez	$3,$L67
	nop

	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x10
	sw	$3,%lo(LogFlag)($2)
$L25:
	lui	$19,%hi(LogFlag)
$L67:
	lw	$16,%lo(LogFlag)($19)
	nop
	andi	$2,$16,0x2
	beq	$2,$0,$L68
	andi	$2,$16,0x4

	lui	$4,%hi($LC1)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC1)

	andi	$2,$16,0x4
$L68:
	beq	$2,$0,$L69
	andi	$2,$16,0x8

	lui	$4,%hi($LC2)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC2)

	andi	$2,$16,0x8
$L69:
	beq	$2,$0,$L70
	andi	$2,$16,0x10

	lui	$4,%hi($LC3)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC3)

	andi	$2,$16,0x10
$L70:
	beq	$2,$0,$L71
	andi	$2,$16,0x20

	lui	$4,%hi($LC4)
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC4)

	andi	$2,$16,0x20
$L71:
	beq	$2,$0,$L72
	andi	$2,$16,0x40

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,4($2)
	lbu	$6,5($2)
	lbu	$7,6($2)
	lbu	$2,7($2)
	lui	$4,%hi($LC5)
	addiu	$4,$4,%lo($LC5)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,4($2)
	nop
	sw	$3,20($sp)
	lbu	$3,5($2)
	nop
	sw	$3,24($sp)
	lbu	$3,6($2)
	nop
	sw	$3,28($sp)
	lbu	$2,7($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x40
$L72:
	beq	$2,$0,$L73
	andi	$2,$16,0x80

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,8($2)
	lbu	$6,9($2)
	lbu	$7,10($2)
	lbu	$2,11($2)
	lui	$4,%hi($LC6)
	addiu	$4,$4,%lo($LC6)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,8($2)
	nop
	sw	$3,20($sp)
	lbu	$3,9($2)
	nop
	sw	$3,24($sp)
	lbu	$3,10($2)
	nop
	sw	$3,28($sp)
	lbu	$2,11($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x80
$L73:
	beq	$2,$0,$L74
	andi	$2,$16,0x100

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,12($2)
	lbu	$6,13($2)
	lbu	$7,14($2)
	lbu	$2,15($2)
	lui	$4,%hi($LC7)
	addiu	$4,$4,%lo($LC7)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,12($2)
	nop
	sw	$3,20($sp)
	lbu	$3,13($2)
	nop
	sw	$3,24($sp)
	lbu	$3,14($2)
	nop
	sw	$3,28($sp)
	lbu	$2,15($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x100
$L74:
	beq	$2,$0,$L75
	andi	$2,$16,0x200

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,16($2)
	lbu	$6,17($2)
	lbu	$7,18($2)
	lbu	$2,19($2)
	lui	$4,%hi($LC8)
	addiu	$4,$4,%lo($LC8)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,16($2)
	nop
	sw	$3,20($sp)
	lbu	$3,17($2)
	nop
	sw	$3,24($sp)
	lbu	$3,18($2)
	nop
	sw	$3,28($sp)
	lbu	$2,19($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x200
$L75:
	beq	$2,$0,$L76
	andi	$2,$16,0x8000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,20($2)
	lbu	$6,21($2)
	lbu	$7,22($2)
	lbu	$2,23($2)
	lui	$4,%hi($LC9)
	addiu	$4,$4,%lo($LC9)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,20($2)
	nop
	sw	$3,20($sp)
	lbu	$3,21($2)
	nop
	sw	$3,24($sp)
	lbu	$3,22($2)
	nop
	sw	$3,28($sp)
	lbu	$2,23($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x8000
$L76:
	beq	$2,$0,$L77
	li	$2,65536			# 0x10000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,44($2)
	lbu	$6,45($2)
	lbu	$7,46($2)
	lbu	$2,47($2)
	lui	$4,%hi($LC10)
	addiu	$4,$4,%lo($LC10)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,44($2)
	nop
	sw	$3,20($sp)
	lbu	$3,45($2)
	nop
	sw	$3,24($sp)
	lbu	$3,46($2)
	nop
	sw	$3,28($sp)
	lbu	$2,47($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	li	$2,65536			# 0x10000
$L77:
	and	$2,$16,$2
	beq	$2,$0,$L78
	andi	$2,$16,0x800

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,48($2)
	lbu	$6,49($2)
	lbu	$7,50($2)
	lbu	$2,51($2)
	lui	$4,%hi($LC11)
	addiu	$4,$4,%lo($LC11)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,48($2)
	nop
	sw	$3,20($sp)
	lbu	$3,49($2)
	nop
	sw	$3,24($sp)
	lbu	$3,50($2)
	nop
	sw	$3,28($sp)
	lbu	$2,51($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x800
$L78:
	beq	$2,$0,$L79
	li	$2,262144			# 0x40000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,28($2)
	lbu	$6,29($2)
	lbu	$7,30($2)
	lbu	$2,31($2)
	lui	$4,%hi($LC12)
	addiu	$4,$4,%lo($LC12)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,28($2)
	nop
	sw	$3,20($sp)
	lbu	$3,29($2)
	nop
	sw	$3,24($sp)
	lbu	$3,30($2)
	nop
	sw	$3,28($sp)
	lbu	$2,31($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	li	$2,262144			# 0x40000
$L79:
	and	$2,$16,$2
	beq	$2,$0,$L80
	li	$2,131072			# 0x20000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,56($2)
	lbu	$6,57($2)
	lbu	$7,58($2)
	lbu	$2,59($2)
	lui	$4,%hi($LC13)
	addiu	$4,$4,%lo($LC13)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,56($2)
	nop
	sw	$3,20($sp)
	lbu	$3,57($2)
	nop
	sw	$3,24($sp)
	lbu	$3,58($2)
	nop
	sw	$3,28($sp)
	lbu	$2,59($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	li	$2,131072			# 0x20000
$L80:
	and	$2,$16,$2
	beq	$2,$0,$L81
	andi	$2,$16,0x4000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,52($2)
	lbu	$6,53($2)
	lbu	$7,54($2)
	lbu	$2,55($2)
	lui	$4,%hi($LC14)
	addiu	$4,$4,%lo($LC14)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,52($2)
	nop
	sw	$3,20($sp)
	lbu	$3,53($2)
	nop
	sw	$3,24($sp)
	lbu	$3,54($2)
	nop
	sw	$3,28($sp)
	lbu	$2,55($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x4000
$L81:
	beq	$2,$0,$L82
	andi	$2,$16,0x400

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,40($2)
	lbu	$6,41($2)
	lbu	$7,42($2)
	lbu	$2,43($2)
	lui	$4,%hi($LC15)
	addiu	$4,$4,%lo($LC15)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,40($2)
	nop
	sw	$3,20($sp)
	lbu	$3,41($2)
	nop
	sw	$3,24($sp)
	lbu	$3,42($2)
	nop
	sw	$3,28($sp)
	lbu	$2,43($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x400
$L82:
	beq	$2,$0,$L83
	andi	$2,$16,0x1000

	lui	$2,%hi(attack_saddr)
	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,24($2)
	lbu	$6,25($2)
	lbu	$7,26($2)
	lbu	$2,27($2)
	lui	$4,%hi($LC16)
	addiu	$4,$4,%lo($LC16)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,24($2)
	nop
	sw	$3,20($sp)
	lbu	$3,25($2)
	nop
	sw	$3,24($sp)
	lbu	$3,26($2)
	nop
	sw	$3,28($sp)
	lbu	$2,27($2)
	jal	scrlog_printk
	sw	$2,32($sp)

	andi	$2,$16,0x1000
$L83:
	beq	$2,$0,$L50
	lui	$2,%hi(attack_saddr)

	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,32($2)
	lbu	$6,33($2)
	lbu	$7,34($2)
	lbu	$2,35($2)
	lui	$4,%hi($LC17)
	addiu	$4,$4,%lo($LC17)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,32($2)
	nop
	sw	$3,20($sp)
	lbu	$3,33($2)
	nop
	sw	$3,24($sp)
	lbu	$3,34($2)
	nop
	sw	$3,28($sp)
	lbu	$2,35($2)
	jal	scrlog_printk
	sw	$2,32($sp)

$L50:
	andi	$16,$16,0x2000
	beq	$16,$0,$L51
	lui	$2,%hi(attack_saddr)

	addiu	$2,$2,%lo(attack_saddr)
	lbu	$5,36($2)
	lbu	$6,37($2)
	lbu	$7,38($2)
	lbu	$2,39($2)
	lui	$4,%hi($LC18)
	addiu	$4,$4,%lo($LC18)
	sw	$2,16($sp)
	lui	$2,%hi(attack_daddr)
	addiu	$2,$2,%lo(attack_daddr)
	lbu	$3,36($2)
	nop
	sw	$3,20($sp)
	lbu	$3,37($2)
	nop
	sw	$3,24($sp)
	lbu	$3,38($2)
	nop
	sw	$3,28($sp)
	lbu	$2,39($2)
	jal	scrlog_printk
	sw	$2,32($sp)

$L51:
	addiu	$18,$18,%lo(dos_pkt)
	li	$2,36864			# 0x9000
	sw	$0,%lo(LogFlag)($19)
	addu	$2,$18,$2
$L52:
	sb	$0,0($18)
	sw	$0,24($18)
	sw	$0,4($18)
	sw	$0,8($18)
	sw	$0,12($18)
	sw	$0,16($18)
	sw	$0,20($18)
	addiu	$18,$18,36
	bne	$18,$2,$L52
	nop

$L22:
	lui	$3,%hi(none_dos_drop_pkt_cnt)
	lw	$2,%lo(none_dos_drop_pkt_cnt)($3)
	nop
	sltu	$4,$2,3001
	bne	$4,$0,$L53
	move	$5,$3

	lw	$2,%lo(item)($17)
	nop
	beq	$2,$0,$L60
	lui	$3,%hi(item_backup)

	sw	$2,%lo(item_backup)($3)
	lui	$2,%hi(item)
	j	$L60
	sw	$0,%lo(item)($2)

$L53:
	lui	$3,%hi(item_backup)
	lw	$4,%lo(item_backup)($3)
	nop
	beq	$4,$0,$L55
	nop

	sw	$4,%lo(item)($17)
	sw	$0,%lo(item_backup)($3)
$L55:
	beq	$2,$0,$L84
	lui	$2,%hi(jiffies)

$L60:
	sw	$0,%lo(none_dos_drop_pkt_cnt)($5)
	lui	$2,%hi(jiffies)
$L84:
	lw	$5,%lo(jiffies)($2)
	lw	$31,60($sp)
	lw	$19,56($sp)
	lw	$18,52($sp)
	lw	$17,48($sp)
	lw	$16,44($sp)
	lui	$4,%hi(dos_timer)
	addiu	$4,$4,%lo(dos_timer)
	addiu	$5,$5,100
	j	mod_timer
	addiu	$sp,$sp,64

$L27:
	bne	$11,$0,$L57
	nop

$L28:
	bne	$10,$0,$L58
	nop

$L29:
	bne	$9,$0,$L59
	nop

	j	$L63
	addiu	$2,$2,36

	.set	macro
	.set	reorder
	.end	dos_timer_fn
	.size	dos_timer_fn, .-dos_timer_fn
	.section	.rodata.str1.4
	.align	2
$LC19:
	.ascii	"br0\000"
	.align	2
$LC20:
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
	lui	$4,%hi($LC19)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC19)
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,64($sp)
	.set	macro
	.set	reorder

	lbu	$18,0($2)
	move	$17,$2
	lw	$5,64($sp)
	andi	$18,$18,0xf
	sll	$18,$18,2
	addu	$18,$2,$18
	lui	$2,%hi(endStateClientPort)
	lhu	$3,%lo(endStateClientPort)($2)
	lhu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L86
	lui	$2,%hi(endStateClientSeq)
	.set	macro
	.set	reorder

	lw	$3,%lo(endStateClientSeq)($2)
	lw	$2,4($18)
	#nop
	bne	$3,$2,$L86
	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$17,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L86
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
	bne	$2,$0,$L107
	move	$4,$19
	.set	macro
	.set	reorder

	j	$L109
$L110:
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L86
$L107:
	li	$5,96			# 0x60
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	skb_copy_expand
	li	$7,32			# 0x20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L89
	move	$20,$2
	.set	macro
	.set	reorder

 #APP
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$17,20($16)
	move	$4,$2
	addiu	$17,$17,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$17,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	bne	$17,$0,$L90
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L90:
	lui	$4,%hi($LC20)
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC20)
	.set	macro
	.set	reorder

	j	$L86
$L89:
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

	move	$21,$2
$L106:
	lw	$22,4($21)
	#nop
	andi	$2,$22,0x1
	bne	$2,$0,$L106
$L92:
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
	move	$23,$2
	.set	macro
	.set	reorder

	move	$5,$2
	addiu	$4,$23,-16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,16			# 0x10
	.set	macro
	.set	reorder

	lw	$2,4($21)
	#nop
	bne	$2,$22,$L106
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
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$21,20($16)
	move	$4,$2
	addiu	$21,$21,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$21,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	bne	$21,$0,$L94
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L94:
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$4,$20
	.set	macro
	.set	reorder

	lhu	$2,2($18)
	lw	$3,8($18)
	lui	$22,%hi(url_cache)
	sh	$2,24($sp)
	lhu	$2,0($18)
	move	$4,$20
	li	$16,20			# 0x14
	sh	$2,26($sp)
	lw	$2,4($18)
	sw	$3,28($sp)
	lw	$3,36($sp)
	addiu	$2,$2,1
	sw	$2,32($sp)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1342177280			# 0x50000000
	or	$2,$2,$3
	sw	$2,36($sp)
	sb	$0,37($sp)
	lw	$3,36($sp)
	li	$2,-327680			# 0xfffffffffffb0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1048576			# 0x100000
	or	$2,$2,$3
	li	$3,-131072			# 0xfffffffffffe0000
	ori	$3,$3,0xffff
	and	$2,$2,$3
	sw	$2,36($sp)
	addiu	$2,$22,%lo(url_cache)
	lhu	$2,12($2)
	sh	$0,42($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sh	$2,38($sp)
	.set	macro
	.set	reorder

	lw	$5,12($18)
	addiu	$21,$2,-34
	move	$18,$22
	srl	$5,$5,28
	sll	$5,$5,2
	.set	noreorder
	.set	nomacro
	j	$L108
	li	$4,8			# 0x8
	.set	macro
	.set	reorder

$L100:
	addu	$3,$2,$16
	lbu	$2,0($3)
	#nop
	sltu	$6,$2,2
	beq	$6,$0,$L96
	.set	noreorder
	.set	nomacro
	j	$L108
	addiu	$16,$16,1
	.set	macro
	.set	reorder

$L96:
	lbu	$3,1($3)
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L98
	addiu	$22,$16,2
	.set	macro
	.set	reorder

	addiu	$2,$sp,24
	addu	$22,$2,$22
	addiu	$4,$sp,20
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	addiu	$16,$16,6
	addiu	$2,$sp,24
	addu	$16,$2,$16
	addiu	$4,$sp,16
	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$22
	addiu	$5,$sp,16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$16
	addiu	$5,$sp,20
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L111
	move	$5,$21
	.set	macro
	.set	reorder

$L98:
	addu	$16,$16,$3
$L108:
	sltu	$2,$16,$5
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L100
	addiu	$2,$sp,24
	.set	macro
	.set	reorder

	move	$5,$21
$L111:
	sh	$0,40($sp)
	move	$6,$0
	addiu	$4,$sp,24
	lw	$16,12($17)
	lw	$22,16($17)
	.set	noreorder
	.set	nomacro
	jal	csum_partial
	andi	$21,$21,0xffff
	.set	macro
	.set	reorder

	move	$4,$20
	addiu	$21,$21,6
 #APP
 # 136 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
		.set	push		# csum_tcpudp_nofold
	.set	noat		
	addu	$22, $16		
	sltu	$1, $22, $16	
	addu	$22, $1		
	addu	$22, $21		
	sltu	$1, $22, $21	
	addu	$22, $1		
	addu	$22, $2		
	sltu	$1, $22, $2	
	addu	$22, $1		
	.set	pop
 # 0 "" 2
 # 78 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
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
	sh	$22,40($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,34
	li	$6,20			# 0x14
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,24
	.set	macro
	.set	reorder

	lw	$3,44($sp)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1073741824			# 0x40000000
	or	$2,$2,$3
	li	$3,-251723776			# 0xfffffffff0ff0000
	ori	$3,$3,0xffff
	and	$2,$2,$3
	li	$3,83886080			# 0x5000000
	or	$2,$2,$3
	sw	$2,44($sp)
	li	$2,6			# 0x6
	sb	$2,53($sp)
	lw	$2,12($17)
	move	$4,$19
	move	$5,$0
	sw	$2,60($sp)
	lw	$2,16($17)
	sh	$0,50($sp)
	sb	$0,45($sp)
	sw	$2,56($sp)
	addiu	$2,$18,%lo(url_cache)
	lbu	$2,14($2)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$2,52($sp)
	.set	macro
	.set	reorder

	li	$2,40			# 0x28
	sh	$2,46($sp)
	lhu	$2,%lo(url_cache)($18)
	lw	$5,44($sp)
	sh	$0,54($sp)
	addiu	$2,$2,1
	sh	$2,48($sp)
	lw	$6,48($sp)
	lw	$4,52($sp)
	srl	$7,$5,22
	addu	$5,$6,$5
	sltu	$6,$5,$6
	addu	$5,$5,$4
	addu	$6,$5,$6
	lw	$5,56($sp)
	sltu	$4,$6,$4
	addiu	$2,$sp,44
	addu	$6,$6,$5
	addu	$4,$6,$4
	andi	$7,$7,0x3c
	sltu	$3,$4,$5
	addu	$7,$2,$7
	addu	$3,$3,$4
	addiu	$2,$sp,60
$L101:
	lw	$4,0($2)
	addiu	$2,$2,4
	addu	$3,$4,$3
	sltu	$4,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$7,$L101
	addu	$3,$4,$3
	.set	macro
	.set	reorder

	move	$4,$20
	move	$2,$3
 #APP
 # 78 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
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
	sh	$2,54($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,14
	addiu	$5,$sp,44
	li	$6,20			# 0x14
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$18,$18,%lo(url_cache)
	.set	macro
	.set	reorder

	sb	$0,16($18)
	.set	noreorder
	.set	nomacro
	jal	dev_queue_xmit
	move	$4,$20
	.set	macro
	.set	reorder

$L86:
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

$L109:
 #APP
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$17,20($16)
	move	$4,$2
	addiu	$17,$17,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$17,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	bne	$17,$0,$L86
	j	$L110
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
	lui	$4,%hi($LC19)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC19)
	.set	macro
	.set	reorder

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,64($sp)
	.set	macro
	.set	reorder

	lbu	$18,0($2)
	move	$17,$2
	lw	$5,64($sp)
	andi	$18,$18,0xf
	sll	$18,$18,2
	addu	$18,$2,$18
	lui	$2,%hi(finStateClientPort)
	lhu	$3,%lo(finStateClientPort)($2)
	lhu	$2,0($18)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L113
	lui	$2,%hi(finStateClientSeq)
	.set	macro
	.set	reorder

	lw	$3,%lo(finStateClientSeq)($2)
	lw	$2,4($18)
	#nop
	bne	$3,$2,$L113
	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$17,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L113
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
	bne	$2,$0,$L134
	move	$4,$19
	.set	macro
	.set	reorder

	j	$L136
$L137:
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L113
$L134:
	li	$5,96			# 0x60
	move	$6,$0
	.set	noreorder
	.set	nomacro
	jal	skb_copy_expand
	li	$7,32			# 0x20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L116
	move	$20,$2
	.set	macro
	.set	reorder

 #APP
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$17,20($16)
	move	$4,$2
	addiu	$17,$17,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$17,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	bne	$17,$0,$L117
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L117:
	lui	$4,%hi($LC20)
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC20)
	.set	macro
	.set	reorder

	j	$L113
$L116:
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

	move	$21,$2
$L133:
	lw	$22,4($21)
	#nop
	andi	$2,$22,0x1
	bne	$2,$0,$L133
$L119:
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
	move	$23,$2
	.set	macro
	.set	reorder

	move	$5,$2
	addiu	$4,$23,-16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,16			# 0x10
	.set	macro
	.set	reorder

	lw	$2,4($21)
	#nop
	bne	$2,$22,$L133
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
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$21,20($16)
	move	$4,$2
	addiu	$21,$21,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$21,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	bne	$21,$0,$L121
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L121:
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$4,$20
	.set	macro
	.set	reorder

	lhu	$2,2($18)
	lw	$3,8($18)
	lui	$22,%hi(url_cache)
	sh	$2,24($sp)
	lhu	$2,0($18)
	move	$4,$20
	li	$16,20			# 0x14
	sh	$2,26($sp)
	lw	$2,4($18)
	sw	$3,28($sp)
	lw	$3,36($sp)
	sw	$2,32($sp)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1342177280			# 0x50000000
	or	$2,$2,$3
	sw	$2,36($sp)
	sb	$0,37($sp)
	lw	$3,36($sp)
	li	$2,-327680			# 0xfffffffffffb0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1114112			# 0x110000
	or	$2,$2,$3
	sw	$2,36($sp)
	addiu	$2,$22,%lo(url_cache)
	lhu	$2,12($2)
	sh	$0,42($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sh	$2,38($sp)
	.set	macro
	.set	reorder

	lw	$5,12($18)
	addiu	$21,$2,-34
	move	$18,$22
	srl	$5,$5,28
	sll	$5,$5,2
	.set	noreorder
	.set	nomacro
	j	$L135
	li	$4,8			# 0x8
	.set	macro
	.set	reorder

$L127:
	addu	$3,$2,$16
	lbu	$2,0($3)
	#nop
	sltu	$6,$2,2
	beq	$6,$0,$L123
	.set	noreorder
	.set	nomacro
	j	$L135
	addiu	$16,$16,1
	.set	macro
	.set	reorder

$L123:
	lbu	$3,1($3)
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L125
	addiu	$22,$16,2
	.set	macro
	.set	reorder

	addiu	$2,$sp,24
	addu	$22,$2,$22
	addiu	$4,$sp,20
	move	$5,$22
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	addiu	$16,$16,6
	addiu	$2,$sp,24
	addu	$16,$2,$16
	addiu	$4,$sp,16
	move	$5,$16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$22
	addiu	$5,$sp,16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$16
	addiu	$5,$sp,20
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	j	$L126
$L125:
	addu	$16,$16,$3
$L135:
	sltu	$2,$16,$5
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L127
	addiu	$2,$sp,24
	.set	macro
	.set	reorder

$L126:
	lhu	$3,26($sp)
	lui	$2,%hi(endStateClientPort)
	move	$5,$21
	sh	$3,%lo(endStateClientPort)($2)
	lw	$3,32($sp)
	sh	$0,40($sp)
	move	$6,$0
	lui	$2,%hi(endStateClientSeq)
	addiu	$4,$sp,24
	sw	$3,%lo(endStateClientSeq)($2)
	lw	$16,12($17)
	lw	$22,16($17)
	.set	noreorder
	.set	nomacro
	jal	csum_partial
	andi	$21,$21,0xffff
	.set	macro
	.set	reorder

	move	$4,$20
	addiu	$21,$21,6
 #APP
 # 136 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
		.set	push		# csum_tcpudp_nofold
	.set	noat		
	addu	$22, $16		
	sltu	$1, $22, $16	
	addu	$22, $1		
	addu	$22, $21		
	sltu	$1, $22, $21	
	addu	$22, $1		
	addu	$22, $2		
	sltu	$1, $22, $2	
	addu	$22, $1		
	.set	pop
 # 0 "" 2
 # 78 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
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
	sh	$22,40($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,34
	li	$6,20			# 0x14
	.set	noreorder
	.set	nomacro
	jal	memcpy
	addiu	$5,$sp,24
	.set	macro
	.set	reorder

	lw	$3,44($sp)
	li	$2,268369920			# 0xfff0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1073741824			# 0x40000000
	or	$2,$2,$3
	li	$3,-251723776			# 0xfffffffff0ff0000
	ori	$3,$3,0xffff
	and	$2,$2,$3
	li	$3,83886080			# 0x5000000
	or	$2,$2,$3
	sw	$2,44($sp)
	li	$2,6			# 0x6
	sb	$2,53($sp)
	lw	$2,12($17)
	move	$4,$19
	move	$5,$0
	sw	$2,60($sp)
	lw	$2,16($17)
	sh	$0,50($sp)
	sb	$0,45($sp)
	sw	$2,56($sp)
	addiu	$2,$18,%lo(url_cache)
	lbu	$2,14($2)
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_ip_summed
	sb	$2,52($sp)
	.set	macro
	.set	reorder

	li	$2,40			# 0x28
	sh	$2,46($sp)
	lhu	$2,%lo(url_cache)($18)
	lw	$5,44($sp)
	sh	$0,54($sp)
	addiu	$2,$2,1
	sh	$2,48($sp)
	lw	$6,48($sp)
	lw	$4,52($sp)
	srl	$7,$5,22
	addu	$5,$6,$5
	sltu	$6,$5,$6
	addu	$5,$5,$4
	addu	$6,$5,$6
	lw	$5,56($sp)
	sltu	$4,$6,$4
	addiu	$2,$sp,44
	addu	$6,$6,$5
	addu	$4,$6,$4
	andi	$7,$7,0x3c
	sltu	$3,$4,$5
	addu	$7,$2,$7
	addu	$3,$3,$4
	addiu	$2,$sp,60
$L128:
	lw	$4,0($2)
	addiu	$2,$2,4
	addu	$3,$4,$3
	sltu	$4,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$7,$L128
	addu	$3,$4,$3
	.set	macro
	.set	reorder

	move	$4,$20
	move	$2,$3
 #APP
 # 78 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
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
	sh	$2,54($sp)
	.set	macro
	.set	reorder

	addiu	$4,$2,14
	addiu	$5,$sp,44
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,20			# 0x14
	.set	macro
	.set	reorder

	addiu	$18,$18,%lo(url_cache)
	li	$2,1			# 0x1
	sb	$0,15($18)
	sb	$2,16($18)
	.set	noreorder
	.set	nomacro
	jal	dev_queue_xmit
	move	$4,$20
	.set	macro
	.set	reorder

$L113:
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

$L136:
 #APP
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$17,20($16)
	move	$4,$2
	addiu	$17,$17,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$17,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	bne	$17,$0,$L113
	j	$L137
	.end	GenerateTCPFINACK
	.size	GenerateTCPFINACK, .-GenerateTCPFINACK
	.section	.rodata.str1.4
	.align	2
$LC21:
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
	.frame	$sp,176,$31		# vars= 120, regs= 10/0, args= 16, gp= 0
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
	move	$17,$4
	lui	$4,%hi($LC19)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$4,%lo($LC19)
	.set	macro
	.set	reorder

	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	sw	$2,128($sp)
	.set	macro
	.set	reorder

	lw	$5,128($sp)
	.set	noreorder
	.set	nomacro
	jal	rtl_neigh_lookup
	addiu	$4,$2,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L139
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
	bne	$2,$0,$L166
	lui	$18,%hi(block_message)
	.set	macro
	.set	reorder

	j	$L171
$L172:
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

	j	$L139
$L166:
	lw	$4,%lo(block_message)($18)
	#nop
	bne	$4,$0,$L168
$L142:
	lui	$4,%hi($LC21)
	addiu	$4,$4,%lo($LC21)
$L168:
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
	bne	$2,$0,$L144
	move	$20,$2
	.set	macro
	.set	reorder

 #APP
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$17,20($16)
	move	$4,$2
	addiu	$17,$17,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$17,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	bne	$17,$0,$L145
	.set	noreorder
	.set	nomacro
	jal	neigh_destroy
	move	$4,$16
	.set	macro
	.set	reorder

$L145:
	lui	$4,%hi($LC20)
	.set	noreorder
	.set	nomacro
	jal	scrlog_printk
	addiu	$4,$4,%lo($LC20)
	.set	macro
	.set	reorder

	j	$L139
$L144:
	.set	noreorder
	.set	nomacro
	jal	rtl_ip_hdr
	move	$4,$2
	.set	macro
	.set	reorder

	move	$19,$2
	lbu	$2,0($2)
	move	$4,$20
	andi	$2,$2,0xf
	sll	$22,$2,2
	addu	$22,$19,$22
	lw	$5,12($22)
	#nop
	srl	$5,$5,28
	addu	$5,$2,$5
	.set	noreorder
	.set	nomacro
	jal	skb_trim
	sll	$5,$5,2
	.set	macro
	.set	reorder

	lw	$4,%lo(block_message)($18)
	#nop
	bne	$4,$0,$L169
$L146:
	lui	$4,%hi($LC21)
	addiu	$4,$4,%lo($LC21)
$L169:
	jal	strlen
	move	$5,$2
	.set	noreorder
	.set	nomacro
	jal	skb_put
	move	$4,$20
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	rtl_get_hh_from_neigh
	move	$4,$16
	.set	macro
	.set	reorder

	move	$21,$2
$L165:
	lw	$23,4($21)
	#nop
	andi	$2,$23,0x1
	bne	$2,$0,$L165
$L149:
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
	move	$fp,$2
	.set	macro
	.set	reorder

	move	$5,$2
	addiu	$4,$fp,-16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,16			# 0x10
	.set	macro
	.set	reorder

	lw	$2,4($21)
	#nop
	bne	$2,$23,$L165
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
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$21,20($16)
	move	$4,$2
	addiu	$21,$21,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$21,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	.set	noreorder
	.set	nomacro
	bne	$21,$0,$L174
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
$L174:
	.set	noreorder
	.set	nomacro
	jal	rtl_set_skb_nfct
	move	$4,$20
	.set	macro
	.set	reorder

	lbu	$16,0($19)
	lw	$2,12($19)
	lw	$3,16($19)
	andi	$16,$16,0xf
	sll	$16,$16,2
	addu	$16,$19,$16
	sw	$3,12($19)
	sw	$2,16($19)
	lw	$21,12($16)
	lhu	$2,0($16)
	lhu	$3,2($16)
	lw	$5,%lo(block_message)($18)
	srl	$21,$21,28
	sll	$21,$21,2
	addu	$21,$16,$21
	sh	$3,0($16)
	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L152
	sh	$2,2($16)
	.set	macro
	.set	reorder

	move	$4,$5
	.set	noreorder
	.set	nomacro
	jal	strlen
	sw	$5,128($sp)
	.set	macro
	.set	reorder

	addiu	$6,$2,1
	lw	$5,128($sp)
	.set	noreorder
	.set	nomacro
	j	$L170
	move	$4,$21
	.set	macro
	.set	reorder

$L152:
	lui	$18,%hi($LC21)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$18,%lo($LC21)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$21
	addiu	$5,$18,%lo($LC21)
$L170:
	jal	memcpy
	lw	$2,8($16)
	lw	$18,4($16)
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sw	$2,4($16)
	.set	macro
	.set	reorder

	lw	$4,12($22)
	lbu	$3,0($19)
	li	$17,-4			# 0xfffffffffffffffc
	srl	$4,$4,28
	andi	$3,$3,0xf
	addu	$3,$4,$3
	mult	$3,$17
	sb	$0,13($16)
	move	$4,$20
	sh	$0,18($16)
	mflo	$3
	addu	$18,$3,$18
	addu	$18,$18,$2
	sw	$18,8($16)
	lui	$18,%hi(url_cache)
	addiu	$2,$18,%lo(url_cache)
	lhu	$2,12($2)
	#nop
	sh	$2,14($16)
	lw	$3,12($16)
	li	$2,-327680			# 0xfffffffffffb0000
	ori	$2,$2,0xffff
	and	$2,$3,$2
	li	$3,1572864			# 0x180000
	or	$2,$2,$3
	li	$3,-131072			# 0xfffffffffffe0000
	ori	$3,$3,0xffff
	and	$2,$2,$3
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_len
	sw	$2,12($16)
	.set	macro
	.set	reorder

	lbu	$21,0($19)
	lw	$5,12($22)
	li	$4,8			# 0x8
	andi	$21,$21,0xf
	mult	$21,$17
	srl	$5,$5,28
	sll	$5,$5,2
	li	$17,20			# 0x14
	mflo	$21
	addiu	$21,$21,-14
	.set	noreorder
	.set	nomacro
	j	$L167
	addu	$21,$21,$2
	.set	macro
	.set	reorder

$L159:
	addu	$3,$16,$17
	lbu	$2,0($3)
	#nop
	sltu	$6,$2,2
	beq	$6,$0,$L155
	.set	noreorder
	.set	nomacro
	j	$L167
	addiu	$17,$17,1
	.set	macro
	.set	reorder

$L155:
	lbu	$3,1($3)
	.set	noreorder
	.set	nomacro
	bne	$2,$4,$L157
	addiu	$22,$17,2
	.set	macro
	.set	reorder

	addu	$22,$16,$22
	addiu	$17,$17,6
	addiu	$4,$sp,20
	move	$5,$22
	addu	$17,$16,$17
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	addiu	$4,$sp,16
	move	$5,$17
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$22
	addiu	$5,$sp,16
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	move	$4,$17
	addiu	$5,$sp,20
	.set	noreorder
	.set	nomacro
	jal	memcpy
	li	$6,4			# 0x4
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L173
	move	$5,$21
	.set	macro
	.set	reorder

$L157:
	addu	$17,$17,$3
$L167:
	sltu	$2,$17,$5
	bne	$2,$0,$L159
	move	$5,$21
$L173:
	sh	$0,16($16)
	move	$6,$0
	move	$4,$16
	andi	$21,$21,0xffff
	lw	$17,12($19)
	lw	$22,16($19)
	.set	noreorder
	.set	nomacro
	jal	csum_partial
	addiu	$21,$21,6
	.set	macro
	.set	reorder

	move	$4,$20
 #APP
 # 136 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
		.set	push		# csum_tcpudp_nofold
	.set	noat		
	addu	$22, $17		
	sltu	$1, $22, $17	
	addu	$22, $1		
	addu	$22, $21		
	sltu	$1, $22, $21	
	addu	$22, $1		
	addu	$22, $2		
	sltu	$1, $22, $2	
	addu	$22, $1		
	.set	pop
 # 0 "" 2
 # 78 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
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
	jal	rtl_get_skb_len
	sh	$22,16($16)
	.set	macro
	.set	reorder

	addiu	$2,$2,-14
	sh	$2,2($19)
	addiu	$2,$18,%lo(url_cache)
	lbu	$2,14($2)
	sh	$0,6($19)
	lw	$5,0($19)
	sb	$2,8($19)
	lhu	$2,%lo(url_cache)($18)
	sh	$0,10($19)
	lw	$4,8($19)
	addiu	$2,$2,2
	sh	$2,4($19)
	lw	$6,4($19)
	srl	$7,$5,22
	andi	$7,$7,0x3c
	addu	$5,$6,$5
	sltu	$6,$5,$6
	addu	$5,$5,$4
	addu	$6,$5,$6
	lw	$5,12($19)
	sltu	$4,$6,$4
	addu	$7,$19,$7
	addu	$6,$6,$5
	addu	$4,$6,$4
	sltu	$3,$4,$5
	addu	$3,$3,$4
	addiu	$2,$19,16
$L160:
	lw	$4,0($2)
	addiu	$2,$2,4
	addu	$3,$4,$3
	sltu	$4,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$2,$7,$L160
	addu	$3,$4,$3
	.set	macro
	.set	reorder

	move	$2,$3
	addiu	$18,$18,%lo(url_cache)
 #APP
 # 78 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/checksum.h" 1
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
	sh	$2,10($19)
	li	$2,1			# 0x1
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	rtl_get_skb_data
	sb	$2,15($18)
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
	move	$4,$20
	sh	$3,%lo(finStateClientPort)($2)
	lw	$3,8($16)
	lui	$2,%hi(finStateClientSeq)
	.set	noreorder
	.set	nomacro
	jal	dev_queue_xmit
	sw	$3,%lo(finStateClientSeq)($2)
	.set	macro
	.set	reorder

$L139:
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
	move	$2,$0
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,176
	.set	macro
	.set	reorder

$L171:
 #APP
 # 176 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	jal	arch_local_irq_save
	lw	$17,20($16)
	move	$4,$2
	addiu	$17,$17,-1
	.set	noreorder
	.set	nomacro
	jal	arch_local_irq_restore
	sw	$17,20($16)
	.set	macro
	.set	reorder

 #APP
 # 215 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/atomic.h" 1
			

 # 0 "" 2
 #NO_APP
	bne	$17,$0,$L139
	j	$L172
	.end	GenerateHTTP401
	.size	GenerateHTTP401, .-GenerateHTTP401
	.section	.rodata.str1.4
	.align	2
$LC22:
	.ascii	"type=%d num=%d\012\000"
	.align	2
$LC23:
	.ascii	"index=%d\011\000"
	.align	2
$LC24:
	.ascii	"mac=%02x:%02x:%02x:%02x:%02x:%02x\012\000"
	.align	2
$LC25:
	.ascii	"index=%d\011\011\000"
	.align	2
$LC26:
	.ascii	"url=%s\012\000"
	.align	2
$LC27:
	.ascii	"start=%x\011\000"
	.align	2
$LC28:
	.ascii	"end=%x\011\000"
	.align	2
$LC29:
	.ascii	"day=%x\012\000"
	.align	2
$LC30:
	.ascii	"unknow table type\012\000"
	.align	2
$LC31:
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

	beq	$2,$0,$L176
	move	$17,$2

	move	$4,$2
	move	$5,$0
	jal	memset
	li	$6,512			# 0x200

	lw	$4,32($16)
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0

	move	$16,$2
	beq	$2,$0,$L192
	move	$4,$17

	move	$4,$2
	move	$5,$0
	jal	memset
	li	$6,256			# 0x100

	lui	$23,%hi($LC25)
	lui	$2,%hi(table_list_head)
	lui	$22,%hi($LC26)
	lui	$20,%hi($LC23)
	lw	$18,%lo(table_list_head)($2)
	lui	$fp,%hi($LC22)
	addiu	$23,$23,%lo($LC25)
	addiu	$22,$22,%lo($LC26)
	j	$L178
	addiu	$20,$20,%lo($LC23)

$L190:
	lw	$7,24($18)
	lw	$6,16($18)
	move	$4,$17
	jal	sprintf
	addiu	$7,$7,-1

	lw	$2,16($18)
	li	$3,3			# 0x3
	beq	$2,$3,$L181
	nop

	li	$3,4			# 0x4
	beq	$2,$3,$L182
	nop

	li	$3,2			# 0x2
	bne	$2,$3,$L191
	move	$4,$16

	lw	$19,8($18)
	j	$L183
	addiu	$3,$18,8

$L184:
	lw	$6,20($19)
	move	$5,$20
	jal	sprintf
	sw	$3,32($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lbu	$2,30($19)
	lbu	$6,28($19)
	lbu	$7,29($19)
	sw	$2,16($sp)
	lbu	$2,31($19)
	lui	$5,%hi($LC24)
	move	$4,$16
	sw	$2,20($sp)
	lbu	$2,32($19)
	addiu	$5,$5,%lo($LC24)
	sw	$2,24($sp)
	lbu	$2,33($19)
	jal	sprintf
	sw	$2,28($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,32($sp)
	nop
$L183:
	bne	$19,$3,$L184
	move	$4,$16

	j	$L185
	nop

$L181:
	lw	$19,8($18)
	j	$L186
	addiu	$3,$18,8

$L187:
	lw	$6,20($19)
	move	$5,$23
	jal	sprintf
	sw	$3,32($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	addiu	$6,$19,28
	move	$4,$16
	jal	sprintf
	move	$5,$22

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,32($sp)
	nop
$L186:
	bne	$19,$3,$L187
	move	$4,$16

	j	$L185
	nop

$L182:
	lw	$19,8($18)
	j	$L188
	addiu	$3,$18,8

$L189:
	lw	$6,20($19)
	move	$5,$20
	jal	sprintf
	sw	$3,32($sp)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$6,28($19)
	lui	$5,%hi($LC27)
	move	$4,$16
	jal	sprintf
	addiu	$5,$5,%lo($LC27)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$6,32($19)
	lui	$2,%hi($LC28)
	move	$4,$16
	jal	sprintf
	addiu	$5,$2,%lo($LC28)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$6,36($19)
	lui	$5,%hi($LC29)
	move	$4,$16
	jal	sprintf
	addiu	$5,$5,%lo($LC29)

	move	$4,$17
	jal	strcat
	move	$5,$16

	lw	$19,0($19)
	lw	$3,32($sp)
	nop
$L188:
	bne	$19,$3,$L189
	move	$4,$16

	j	$L185
	nop

$L191:
	lui	$5,%hi($LC30)
	jal	sprintf
	addiu	$5,$5,%lo($LC30)

	move	$4,$17
	jal	strcat
	move	$5,$16

$L185:
	lw	$18,0($18)
$L178:
	lui	$3,%hi(table_list_head)
	addiu	$2,$3,%lo(table_list_head)
	bne	$18,$2,$L190
	addiu	$5,$fp,%lo($LC22)

	lui	$2,%hi(url_filter_mode)
	lw	$4,80($sp)
	lw	$6,%lo(url_filter_mode)($2)
	lui	$5,%hi($LC31)
	addiu	$5,$5,%lo($LC31)
	jal	seq_printf
	move	$7,$17

	jal	kfree
	move	$4,$17

	move	$4,$16
$L192:
	jal	kfree
	nop

$L176:
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
	li	$17,6			# 0x6
	move	$16,$4
	li	$18,12			# 0xc
$L195:
	addu	$2,$5,$17
	move	$4,$16
	lbu	$19,-6($2)
	jal	rtl_skb_mac_header
	sw	$5,16($sp)

	addu	$2,$2,$17
	lbu	$2,0($2)
	lw	$5,16($sp)
	bne	$19,$2,$L196
	addiu	$17,$17,1

	bne	$17,$18,$L195
	li	$2,1			# 0x1

	j	$L194
	nop

$L196:
	move	$2,$0
$L194:
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
	.section	.text.find_pattern.constprop.4,"ax",@progbits
	.align	2
	.set	nomips16
	.ent	find_pattern.constprop.4
	.type	find_pattern.constprop.4, @function
find_pattern.constprop.4:
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
	lw	$23,88($sp)
	lw	$22,92($sp)
	move	$16,$4
	move	$17,$5
	sw	$0,0($22)
	move	$18,$6
	sw	$0,0($23)
	move	$21,$4
	move	$19,$0
	move	$20,$0
	subu	$8,$5,$7
	li	$3,13			# 0xd
	j	$L199
	li	$fp,10			# 0xa

$L207:
	lb	$2,0($21)
	nop
	bne	$2,$3,$L200
	nop

	andi	$2,$19,0x1
	bne	$2,$0,$L210
	addiu	$19,$19,1

	j	$L216
	slt	$2,$19,4

$L200:
	bne	$2,$fp,$L210
	andi	$2,$19,0x1

	beq	$2,$0,$L210
	addiu	$19,$19,1

	slt	$2,$19,4
$L216:
	bne	$2,$0,$L215
	move	$4,$21

	j	$L203
	move	$2,$0

$L210:
	move	$19,$0
	move	$4,$21
$L215:
	move	$6,$7
	move	$5,$18
	sw	$3,24($sp)
	sw	$7,16($sp)
	jal	memcmp
	sw	$8,20($sp)

	lw	$3,24($sp)
	lw	$7,16($sp)
	lw	$8,20($sp)
	beq	$2,$0,$L204
	addiu	$21,$21,1

	j	$L199
	addiu	$20,$20,1

$L204:
	addu	$20,$20,$7
	sw	$20,0($23)
	move	$2,$0
	j	$L205
	li	$3,13			# 0xd

$L206:
	bne	$4,$0,$L212
	addiu	$20,$20,1

	addiu	$2,$2,1
$L205:
	addu	$4,$16,$20
	lb	$4,0($4)
	nop
	bne	$4,$3,$L206
	sltu	$4,$17,$20

	sw	$2,0($22)
	j	$L203
	li	$2,1			# 0x1

$L199:
	sltu	$2,$8,$20
	beq	$2,$0,$L207
	nop

$L212:
	move	$2,$0
$L203:
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
	.end	find_pattern.constprop.4
	.size	find_pattern.constprop.4, .-find_pattern.constprop.4
	.section	.rodata.str1.4
	.align	2
$LC32:
	.ascii	"GET \000"
	.align	2
$LC33:
	.ascii	"Host: \000"
	.align	2
$LC34:
	.ascii	"%s\000"
	.section	.text.rtl_url_filter_fastpath,"ax",@progbits
	.align	2
	.globl	rtl_url_filter_fastpath
	.set	nomips16
	.ent	rtl_url_filter_fastpath
	.type	rtl_url_filter_fastpath, @function
rtl_url_filter_fastpath:
	.frame	$sp,72,$31		# vars= 16, regs= 8/0, args= 24, gp= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-72
	sw	$31,68($sp)
	sw	$22,64($sp)
	sw	$21,60($sp)
	sw	$20,56($sp)
	sw	$19,52($sp)
	sw	$18,48($sp)
	sw	$17,44($sp)
	sw	$16,40($sp)
	move	$16,$4
	move	$4,$5
	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$19,$5
	.set	macro
	.set	reorder

	sltu	$2,$2,2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L218
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
	move	$20,$2
	li	$2,6			# 0x6
	.set	noreorder
	.set	nomacro
	bne	$3,$2,$L236
	move	$2,$17
	.set	macro
	.set	reorder

	lbu	$21,0($20)
	lui	$2,%hi(kmalloc_caches+44)
	lw	$4,%lo(kmalloc_caches+44)($2)
	andi	$21,$21,0xf
	sll	$21,$21,2
	addu	$21,$20,$21
	lw	$18,12($21)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0
	.set	macro
	.set	reorder

	srl	$18,$18,28
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L218
	move	$16,$2
	.set	macro
	.set	reorder

	sll	$18,$18,2
	lhu	$22,2($20)
	lbu	$20,0($20)
	addu	$18,$21,$18
	lui	$21,%hi($LC32)
	move	$4,$18
	addiu	$5,$21,%lo($LC32)
	li	$6,4			# 0x4
	.set	noreorder
	.set	nomacro
	jal	memcmp
	andi	$20,$20,0xf
	.set	macro
	.set	reorder

	bne	$2,$0,$L235
$L220:
	sll	$20,$20,3
	addiu	$2,$sp,32
	subu	$20,$22,$20
	lui	$6,%hi($LC33)
	sw	$2,16($sp)
	move	$4,$18
	addiu	$2,$sp,28
	move	$5,$20
	addiu	$6,$6,%lo($LC33)
	li	$7,6			# 0x6
	.set	noreorder
	.set	nomacro
	jal	find_pattern.constprop.4
	sw	$2,20($sp)
	.set	macro
	.set	reorder

	bne	$2,$0,$L221
$L235:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L236
	move	$2,$17
	.set	macro
	.set	reorder

$L221:
	lw	$4,28($sp)
	lw	$3,32($sp)
	.set	noreorder
	.set	nomacro
	beq	$4,$0,$L222
	addu	$3,$18,$3
	.set	macro
	.set	reorder

	move	$2,$16
 #APP
 # 55 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
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
$L222:
	lw	$2,28($sp)
	move	$4,$18
	move	$5,$20
	addu	$2,$16,$2
	sb	$0,0($2)
	addiu	$2,$sp,32
	sw	$2,16($sp)
	addiu	$6,$21,%lo($LC32)
	addiu	$2,$sp,24
	li	$7,4			# 0x4
	.set	noreorder
	.set	nomacro
	jal	find_pattern.constprop.4
	sw	$2,20($sp)
	.set	macro
	.set	reorder

	beq	$2,$0,$L223
	lw	$3,24($sp)
	#nop
	addiu	$3,$3,-9
	.set	noreorder
	.set	nomacro
	bgtz	$3,$L224
	sw	$3,24($sp)
	.set	macro
	.set	reorder

$L223:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L218
	move	$17,$0
	.set	macro
	.set	reorder

$L224:
	lw	$2,28($sp)
	lw	$4,32($sp)
	addu	$2,$16,$2
	addu	$18,$18,$4
 #APP
 # 55 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($18)
	subu	$3,1
	sb	$1,($2)
	beqz	$1,2f
	addiu	$2,1
	bnez	$3,1b
	addiu	$18,1
2:
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	lw	$2,24($sp)
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

	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$17,$2
	.set	macro
	.set	reorder

	move	$18,$2
	sltu	$2,$17,$2
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L226
	move	$17,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L227
	move	$17,$16
	.set	macro
	.set	reorder

$L228:
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	memcmp
	move	$6,$18
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L233
	addiu	$17,$17,1
	.set	macro
	.set	reorder

$L227:
	addu	$2,$17,$18
	lb	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L228
	move	$5,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L226
	move	$17,$0
	.set	macro
	.set	reorder

$L233:
	li	$17,1			# 0x1
$L226:
	lui	$4,%hi(log_info)
	lui	$6,%hi($LC34)
	addiu	$4,$4,%lo(log_info)
	li	$5,32			# 0x20
	addiu	$6,$6,%lo($LC34)
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
	bne	$2,$3,$L218
	.set	noreorder
	.set	nomacro
	bne	$17,$0,$L234
	lui	$3,%hi(url_filter_flag)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L218
	sw	$2,%lo(url_filter_flag)($3)
	.set	macro
	.set	reorder

$L234:
	li	$17,1			# 0x1
$L218:
	move	$2,$17
$L236:
	lw	$31,68($sp)
	lw	$22,64($sp)
	lw	$21,60($sp)
	lw	$20,56($sp)
	lw	$19,52($sp)
	lw	$18,48($sp)
	lw	$17,44($sp)
	lw	$16,40($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,72
	.set	macro
	.set	reorder

	.end	rtl_url_filter_fastpath
	.size	rtl_url_filter_fastpath, .-rtl_url_filter_fastpath
	.section	.rodata.str1.4
	.align	2
$LC35:
	.ascii	"ppp0\000"
	.align	2
$LC36:
	.ascii	"eth1\000"
	.align	2
$LC37:
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
	lui	$17,%hi($LC19)
	.set	macro
	.set	reorder

	addiu	$4,$17,%lo($LC19)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	move	$16,$2
	.set	macro
	.set	reorder

	beq	$2,$0,$L238
	lw	$2,368($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L238
	addiu	$2,$2,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L239
	addiu	$17,$17,%lo($LC19)
	.set	macro
	.set	reorder

$L241:
	move	$3,$17
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($3)
1:	lbu	$1,($4)
	addiu	$3,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($3)
	nop
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	beq	$5,$0,$L240
	addiu	$2,$2,8
$L239:
	lw	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L241
	addiu	$4,$2,43
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L262
	lui	$17,%hi($LC35)
	.set	macro
	.set	reorder

$L240:
	lw	$3,28($2)
	lw	$4,16($16)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$4,$3,$L242
	move	$2,$0
	.set	macro
	.set	reorder

$L238:
	lui	$17,%hi($LC35)
$L262:
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$17,%lo($LC35)
	.set	macro
	.set	reorder

	beq	$2,$0,$L243
	lw	$3,368($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L242
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

	addiu	$3,$3,12
	.set	noreorder
	.set	nomacro
	j	$L244
	addiu	$17,$17,%lo($LC35)
	.set	macro
	.set	reorder

$L246:
	move	$2,$17
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($2)
1:	lbu	$1,($4)
	addiu	$2,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($2)
	nop
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	beq	$5,$0,$L245
	addiu	$3,$3,8
$L244:
	lw	$3,0($3)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L246
	addiu	$4,$3,43
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L242
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L245:
	lw	$4,16($16)
	lw	$2,24($3)
	.set	noreorder
	.set	nomacro
	j	$L263
	xor	$2,$4,$2
	.set	macro
	.set	reorder

$L243:
	lui	$17,%hi($LC36)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$17,%lo($LC36)
	.set	macro
	.set	reorder

	beq	$2,$0,$L247
	lw	$3,368($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L242
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

	addiu	$3,$3,12
	.set	noreorder
	.set	nomacro
	j	$L248
	addiu	$17,$17,%lo($LC36)
	.set	macro
	.set	reorder

$L250:
	move	$2,$17
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($2)
1:	lbu	$1,($4)
	addiu	$2,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($2)
	nop
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	beq	$5,$0,$L252
	addiu	$3,$3,8
$L248:
	lw	$3,0($3)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L250
	addiu	$4,$3,43
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L242
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L247:
	lui	$17,%hi($LC37)
	.set	noreorder
	.set	nomacro
	jal	rtl_get_dev_by_name
	addiu	$4,$17,%lo($LC37)
	.set	macro
	.set	reorder

	move	$3,$2
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L242
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

	lw	$3,368($3)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L242
	addiu	$3,$3,12
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L251
	addiu	$17,$17,%lo($LC37)
	.set	macro
	.set	reorder

$L253:
	move	$2,$17
 #APP
 # 80 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
	lbu	$5,($2)
1:	lbu	$1,($4)
	addiu	$2,1
	bne	$1,$5,2f
	addiu	$4,1
	bnez	$5,1b
	lbu	$5,($2)
	nop
	move	$5,$1
2:	subu	$5,$1
3:	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
	beq	$5,$0,$L252
	addiu	$3,$3,8
$L251:
	lw	$3,0($3)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L253
	addiu	$4,$3,43
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L242
	li	$2,1			# 0x1
	.set	macro
	.set	reorder

$L252:
	lw	$4,16($16)
	lw	$2,28($3)
	#nop
	xor	$2,$4,$2
$L263:
	sltu	$2,$0,$2
$L242:
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
	j	$L265
	move	$2,$0

$L269:
	addiu	$5,$3,-48
	sltu	$6,$5,10
	bne	$6,$0,$L266
	nop

	addiu	$5,$3,-97
	sltu	$5,$5,6
	bne	$5,$0,$L266
	addiu	$5,$3,-87

	addiu	$5,$3,-65
	sltu	$5,$5,6
	beq	$5,$0,$L270
	nop

	addiu	$5,$3,-55
$L266:
	sll	$2,$2,4
	addu	$2,$5,$2
	addiu	$4,$4,1
$L265:
	lbu	$3,0($4)
	nop
	bne	$3,$0,$L269
	nop

	j	$31
	nop

$L270:
	j	$31
	li	$2,-1			# 0xffffffffffffffff

	.set	macro
	.set	reorder
	.end	str2hexnum
	.size	str2hexnum, .-str2hexnum
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
	sw	$3,8($2)
	sw	$3,12($2)
	li	$3,1			# 0x1
	sw	$2,4($2)
	sw	$0,16($2)
	sw	$0,20($2)
	sw	$0,28($2)
	sw	$3,24($2)
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
	.section	.text.filter_table_regist,"ax",@progbits
	.align	2
	.globl	filter_table_regist
	.set	nomips16
	.ent	filter_table_regist
	.type	filter_table_regist, @function
filter_table_regist:
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
	lui	$16,%hi(table_list_head)
	addiu	$2,$16,%lo(table_list_head)
	lw	$2,24($2)
	move	$18,$4
	bne	$2,$0,$L273
	move	$17,$5

	jal	filter_table_head_init
	nop

$L273:
	lui	$2,%hi(kmalloc_caches+20)
	lw	$4,%lo(kmalloc_caches+20)($2)
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0

	li	$3,1			# 0x1
	sw	$2,0($2)
	sw	$3,24($2)
	addiu	$3,$2,8
	sw	$3,8($2)
	sw	$3,12($2)
	lw	$3,%lo(table_list_head)($16)
	sw	$18,16($2)
	sw	$0,20($2)
	sw	$17,28($2)
	sw	$2,4($3)
	sw	$3,0($2)
	addiu	$3,$16,%lo(table_list_head)
	sw	$3,4($2)
	sw	$2,%lo(table_list_head)($16)
	lw	$2,24($3)
	nop
	addiu	$2,$2,1
	sw	$2,24($3)
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	filter_table_regist
	.size	filter_table_regist, .-filter_table_regist
	.section	.text.filter_table_flush,"ax",@progbits
	.align	2
	.globl	filter_table_flush
	.set	nomips16
	.ent	filter_table_flush
	.type	filter_table_flush, @function
filter_table_flush:
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
	lui	$17,%hi(table_list_head)
	lw	$16,%lo(table_list_head)($17)
	li	$20,1048576			# 0x100000
	li	$19,2097152			# 0x200000
	lw	$18,0($16)
	addiu	$17,$17,%lo(table_list_head)
	addiu	$20,$20,256
	j	$L275
	addiu	$19,$19,512

$L279:
	lw	$4,8($16)
	nop
	lw	$21,0($4)
	j	$L276
	addiu	$22,$16,8

$L277:
	lw	$2,4($4)
	lw	$3,0($4)
	nop
	sw	$2,4($3)
	sw	$3,0($2)
	sw	$20,0($4)
	jal	kfree
	sw	$19,4($4)

	lw	$2,24($16)
	move	$4,$21
	addiu	$2,$2,-1
	sw	$2,24($16)
	lw	$21,0($21)
$L276:
	bne	$4,$22,$L277
	li	$2,3			# 0x3

	lw	$3,16($16)
	nop
	bne	$3,$2,$L278
	li	$2,1			# 0x1

	lw	$3,24($16)
	nop
	bne	$3,$2,$L278
	nop

	jal	Del_Pattern_ACL_For_ContentFilter
	nop

$L278:
	lw	$2,4($16)
	lw	$3,0($16)
	move	$4,$16
	sw	$2,4($3)
	sw	$3,0($2)
	sw	$20,0($16)
	jal	kfree
	sw	$19,4($16)

	lw	$2,24($17)
	move	$16,$18
	addiu	$2,$2,-1
	sw	$2,24($17)
	lw	$18,0($18)
$L275:
	bne	$16,$17,$L279
	li	$3,1			# 0x1

	lui	$2,%hi(filter_rule_num)
	sw	$3,%lo(filter_rule_num)($2)
	lw	$31,44($sp)
	lui	$2,%hi(log_enable)
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
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	filter_table_flush
	.size	filter_table_flush, .-filter_table_flush
	.section	.text.filter_item_regist,"ax",@progbits
	.align	2
	.globl	filter_item_regist
	.set	nomips16
	.ent	filter_item_regist
	.type	filter_item_regist, @function
filter_item_regist:
	.frame	$sp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$16,16($sp)
	lui	$3,%hi(table_list_head)
	lw	$2,%lo(table_list_head)($3)
	j	$L281
	addiu	$3,$3,%lo(table_list_head)

$L284:
	lw	$6,16($2)
	nop
	bne	$6,$4,$L282
	nop

	lw	$3,8($2)
	nop
	sw	$5,4($3)
	sw	$3,0($5)
	addiu	$3,$2,8
	sw	$3,4($5)
	lw	$3,24($2)
	sw	$5,8($2)
	addiu	$3,$3,1
	sw	$3,24($2)
	li	$2,3			# 0x3
	bne	$4,$2,$L283
	move	$16,$0

	li	$2,2			# 0x2
	bne	$3,$2,$L283
	nop

	jal	Add_Pattern_ACL_For_ContentFilter
	nop

	j	$L287
	move	$2,$16

$L282:
	lw	$2,0($2)
	nop
$L281:
	bne	$2,$3,$L284
	li	$16,-1			# 0xffffffffffffffff

$L283:
	move	$2,$16
$L287:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_item_regist
	.size	filter_item_regist, .-filter_item_regist
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
	sll	$17,$5,24
	jal	strlen
	move	$16,$4

	sra	$17,$17,24
	addiu	$2,$2,-1
	j	$L289
	move	$3,$0

$L291:
	lb	$4,0($4)
	nop
	bne	$4,$17,$L290
	addiu	$2,$2,-1

	addiu	$3,$3,1
$L289:
	bgez	$2,$L291
	addu	$4,$16,$2

$L290:
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
	.section	.rodata.str1.4
	.align	2
$LC38:
	.ascii	"init\000"
	.align	2
$LC39:
	.ascii	" \000"
	.align	2
$LC40:
	.ascii	"flush\000"
	.align	2
$LC41:
	.ascii	"enable_log\000"
	.align	2
$LC42:
	.ascii	"white\000"
	.align	2
$LC43:
	.ascii	"black\000"
	.align	2
$LC44:
	.ascii	"add:\000"
	.align	2
$LC45:
	.ascii	"#\000"
	.align	2
$LC46:
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
	lui	$19,%hi(kmalloc_caches)
	addiu	$2,$19,%lo(kmalloc_caches)
	lw	$4,40($2)
	move	$20,$5
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
	beq	$2,$0,$L293
	li	$18,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	move	$5,$0
	li	$6,1024			# 0x400
	lui	$18,%hi(get_info_from_usr_space)
	.set	noreorder
	.set	nomacro
	jal	memset
	move	$4,$2
	.set	macro
	.set	reorder

	addiu	$4,$18,%lo(get_info_from_usr_space)
	.set	noreorder
	.set	nomacro
	jal	strlen
	sw	$16,16($sp)
	.set	macro
	.set	reorder

	move	$6,$2
	addiu	$4,$18,%lo(get_info_from_usr_space)
	.set	noreorder
	.set	nomacro
	jal	memset
	move	$5,$0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$20,$0,$L294
	move	$2,$18
	.set	macro
	.set	reorder

	or	$4,$20,$17
	addu	$3,$20,$17
	or	$3,$4,$3
	lw	$4,24($28)
	move	$18,$17
	and	$3,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L295
	move	$6,$17
	.set	macro
	.set	reorder

	addiu	$4,$2,%lo(get_info_from_usr_space)
	move	$5,$20
 #APP
 # 707 "net/rtl/fastpath/filter_v2.c" 1
	.set	noreorder
	jal	__copy_user
	.set	noat
	addu	$1, $5, $6
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
$L295:
	bne	$6,$0,$L294
	.set	noreorder
	.set	nomacro
	beq	$17,$0,$L296
	addiu	$4,$2,%lo(get_info_from_usr_space)
	.set	macro
	.set	reorder

	move	$3,$16
 #APP
 # 55 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
	.set	noreorder
	.set	noat
1:	lbu	$1,($4)
	subu	$17,1
	sb	$1,($3)
	beqz	$1,2f
	addiu	$3,1
	bnez	$17,1b
	addiu	$4,1
2:
	.set	at
	.set	reorder
 # 0 "" 2
 #NO_APP
$L296:
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$2,%lo(get_info_from_usr_space)
	.set	macro
	.set	reorder

	addu	$2,$16,$2
	sb	$0,-1($2)
	lw	$20,16($sp)
	lui	$17,%hi($LC38)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC38)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC38)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L297
	lui	$17,%hi($LC40)
	.set	macro
	.set	reorder

	lui	$17,%hi($LC39)
	addiu	$19,$sp,16
	addiu	$5,$17,%lo($LC39)
	move	$4,$19
	.set	noreorder
	.set	nomacro
	jal	strsep
	lui	$20,%hi(filter_tlb)
	.set	macro
	.set	reorder

	addiu	$17,$17,%lo($LC39)
	addiu	$20,$20,%lo(filter_tlb)
	lw	$2,16($sp)
	#nop
$L331:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L294
	move	$5,$17
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$4,$19
	.set	macro
	.set	reorder

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

	lw	$2,16($sp)
	#nop
	bne	$2,$0,$L331
	j	$L299
$L297:
	lw	$20,16($sp)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC40)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC40)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L300
	lui	$17,%hi($LC41)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	filter_table_flush
	move	$4,$0
	.set	macro
	.set	reorder

	j	$L299
$L300:
	lw	$20,16($sp)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC41)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC41)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L301
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	lui	$2,%hi(log_enable)
	.set	noreorder
	.set	nomacro
	j	$L299
	sw	$3,%lo(log_enable)($2)
	.set	macro
	.set	reorder

$L301:
	lw	$20,16($sp)
	lui	$17,%hi($LC42)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC42)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC42)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L302
	li	$3,1			# 0x1
	.set	macro
	.set	reorder

	lui	$2,%hi(url_filter_mode)
	.set	noreorder
	.set	nomacro
	j	$L299
	sw	$3,%lo(url_filter_mode)($2)
	.set	macro
	.set	reorder

$L302:
	lw	$20,16($sp)
	lui	$17,%hi($LC43)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC43)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC43)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L303
	lui	$2,%hi(url_filter_mode)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L299
	sw	$0,%lo(url_filter_mode)($2)
	.set	macro
	.set	reorder

$L303:
	lw	$20,16($sp)
	lui	$17,%hi($LC44)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC44)
	.set	macro
	.set	reorder

	move	$6,$2
	move	$4,$20
	.set	noreorder
	.set	nomacro
	jal	memcmp
	addiu	$5,$17,%lo($LC44)
	.set	macro
	.set	reorder

	bne	$2,$0,$L299
	lw	$20,16($sp)
	.set	noreorder
	.set	nomacro
	jal	strlen
	addiu	$4,$17,%lo($LC44)
	.set	macro
	.set	reorder

	addu	$2,$20,$2
	sw	$2,16($sp)
	lb	$2,0($2)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L294
	lui	$5,%hi($LC45)
	.set	macro
	.set	reorder

	addiu	$4,$sp,16
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$5,%lo($LC45)
	.set	macro
	.set	reorder

	lw	$3,16($sp)
	#nop
	beq	$3,$0,$L294
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	li	$3,-1			# 0xffffffffffffffff
	.set	noreorder
	.set	nomacro
	beq	$2,$3,$L294
	lui	$20,%hi($LC39)
	.set	macro
	.set	reorder

	addiu	$4,$sp,16
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$20,%lo($LC39)
	.set	macro
	.set	reorder

	lw	$3,16($sp)
	#nop
	beq	$3,$0,$L294
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	li	$17,-1			# 0xffffffffffffffff
	.set	noreorder
	.set	nomacro
	beq	$2,$17,$L294
	addiu	$22,$sp,16
	.set	macro
	.set	reorder

	move	$4,$22
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$20,%lo($LC39)
	.set	macro
	.set	reorder

	lw	$3,16($sp)
	#nop
	beq	$3,$0,$L294
	.set	noreorder
	.set	nomacro
	jal	str2hexnum
	move	$4,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$17,$L294
	move	$23,$2
	.set	macro
	.set	reorder

	lui	$21,%hi($LC46)
$L323:
	move	$4,$22
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$21,%lo($LC46)
	.set	macro
	.set	reorder

	sw	$2,20($sp)
	.set	noreorder
	.set	nomacro
	j	$L304
	li	$17,59			# 0x3b
	.set	macro
	.set	reorder

$L306:
	lw	$fp,20($sp)
	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$4,$fp
	.set	macro
	.set	reorder

	addu	$2,$fp,$2
	lui	$5,%hi($LC46)
	sb	$17,0($2)
	move	$4,$22
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$5,%lo($LC46)
	.set	macro
	.set	reorder

	lw	$2,16($sp)
	#nop
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L332
	addiu	$4,$sp,20
	.set	macro
	.set	reorder

$L304:
	lw	$4,20($sp)
	.set	noreorder
	.set	nomacro
	jal	check_esc_end
	li	$5,92			# 0x5c
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L306
	addiu	$4,$sp,20
	.set	macro
	.set	reorder

$L332:
	.set	noreorder
	.set	nomacro
	jal	strsep
	addiu	$5,$20,%lo($LC39)
	.set	macro
	.set	reorder

	move	$17,$2
	lw	$2,20($sp)
	#nop
	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L326
	addiu	$fp,$20,%lo($LC39)
	.set	macro
	.set	reorder

	j	$L294
$L308:
	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$4,$17
	.set	macro
	.set	reorder

	addu	$2,$17,$2
	li	$3,32			# 0x20
	sb	$3,0($2)
	addiu	$4,$sp,20
	.set	noreorder
	.set	nomacro
	jal	strsep
	move	$5,$fp
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L329
	move	$4,$17
	.set	macro
	.set	reorder

$L326:
	move	$4,$17
$L329:
	.set	noreorder
	.set	nomacro
	jal	check_esc_end
	li	$5,92			# 0x5c
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L308
	move	$2,$17
	.set	macro
	.set	reorder

	move	$4,$0
	.set	noreorder
	.set	nomacro
	j	$L309
	li	$7,92			# 0x5c
	.set	macro
	.set	reorder

$L313:
	.set	noreorder
	.set	nomacro
	bne	$3,$7,$L311
	move	$3,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L328
	move	$5,$4
	.set	macro
	.set	reorder

$L312:
	sb	$6,0($3)
	addiu	$5,$5,1
	addiu	$3,$3,1
$L328:
	lb	$6,1($3)
	#nop
	bne	$6,$0,$L312
	addu	$5,$17,$5
	sb	$0,0($5)
$L311:
	addiu	$4,$4,1
	addiu	$2,$2,1
$L309:
	lb	$3,0($2)
	#nop
	bne	$3,$0,$L313
	addiu	$2,$19,%lo(kmalloc_caches)
	lw	$4,4($2)
	.set	noreorder
	.set	nomacro
	jal	kmem_cache_alloc
	li	$5,208			# 0xd0
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L299
	move	$5,$2
	.set	macro
	.set	reorder

$L314:
	sw	$2,0($5)
	sw	$2,4($5)
	lui	$3,%hi(filter_rule_num)
	addiu	$2,$2,8
	sw	$2,8($5)
	sw	$2,12($5)
	lw	$2,%lo(filter_rule_num)($3)
	sw	$0,24($5)
	sw	$23,16($5)
	sw	$2,20($5)
	move	$4,$17
	.set	noreorder
	.set	nomacro
	jal	strlen
	sw	$5,24($sp)
	.set	macro
	.set	reorder

	sltu	$3,$2,41
	lui	$fp,%hi(filter_rule_num)
	lw	$5,24($sp)
	.set	noreorder
	.set	nomacro
	beq	$3,$0,$L322
	li	$2,39			# 0x27
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	jal	strlen
	move	$4,$17
	.set	macro
	.set	reorder

	lw	$5,24($sp)
	beq	$2,$0,$L316
$L322:
	addiu	$3,$5,28
	move	$4,$2
 #APP
 # 55 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
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
$L316:
	addu	$2,$5,$2
	sb	$0,28($2)
	.set	noreorder
	.set	nomacro
	jal	filter_item_regist
	li	$4,3			# 0x3
	.set	macro
	.set	reorder

	lw	$2,%lo(filter_rule_num)($fp)
	#nop
	addiu	$2,$2,1
	sw	$2,%lo(filter_rule_num)($fp)
	lw	$2,16($sp)
	#nop
	beq	$2,$0,$L299
	lb	$2,0($2)
	#nop
	bne	$2,$0,$L323
$L299:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L330
	move	$2,$18
	.set	macro
	.set	reorder

$L294:
	.set	noreorder
	.set	nomacro
	jal	kfree
	move	$4,$16
	.set	macro
	.set	reorder

	li	$18,-14			# 0xfffffffffffffff2
$L293:
	move	$2,$18
$L330:
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
	lui	$21,%hi(url_filter_flag)
	move	$18,$4
	move	$22,$5
	move	$17,$6
	move	$20,$7
	sw	$0,%lo(url_filter_flag)($21)
	beq	$6,$0,$L334
	li	$2,-1			# 0xffffffffffffffff

	lw	$3,28($6)
	nop
	beq	$3,$0,$L334
	nop

	lw	$2,24($6)
	nop
	sltu	$2,$2,2
	bne	$2,$0,$L335
	addiu	$19,$6,8

	lw	$16,8($6)
	nop
	bne	$16,$19,$L356
	lui	$2,%hi(url_filter_mode)

	j	$L359
	nop

$L352:
	move	$16,$2
$L356:
	lw	$2,16($16)
	nop
	andi	$3,$2,0x1
	bne	$3,$0,$L360
	andi	$3,$2,0x200

	lw	$3,20($16)
	nop
	bne	$22,$3,$L337
	andi	$3,$2,0x200

$L360:
	beq	$3,$0,$L338
	nop

	andi	$2,$2,0x2
	lw	$6,0($20)
	beq	$2,$0,$L337
	nop

	beq	$6,$0,$L351
	move	$4,$18

	lw	$5,20($16)
	jal	scan_table
	move	$7,$6

	bne	$2,$0,$L334
	li	$2,1			# 0x1

	j	$L337
	nop

$L338:
	andi	$2,$2,0x100
	bne	$2,$0,$L339
	move	$4,$18

	lw	$2,28($17)
	nop
	jalr	$2
	addiu	$5,$16,28

	beq	$2,$0,$L337
	nop

$L339:
	lw	$3,16($16)
	nop
	andi	$2,$3,0x2
	bne	$2,$0,$L340
	andi	$4,$3,0x1

	beq	$4,$0,$L334
	li	$2,1			# 0x1

	j	$L358
	andi	$2,$3,0x8

$L340:
	lw	$6,0($20)
	nop
	beq	$6,$0,$L351
	move	$4,$18

	lw	$5,20($16)
	jal	scan_table
	move	$7,$6

	beq	$2,$0,$L337
	nop

	lw	$3,16($16)
	nop
	andi	$4,$3,0x1
	beq	$4,$0,$L341
	lui	$2,%hi(url_filter_mode)

	andi	$3,$3,0x8
	lw	$4,%lo(url_filter_mode)($2)
	sltu	$2,$0,$3
	j	$L357
	sltu	$3,$3,1

$L341:
	lw	$2,%lo(url_filter_mode)($2)
$L358:
	j	$L334
	sltu	$2,$2,1

$L337:
	lw	$2,0($16)
	nop
	bne	$2,$19,$L352
	nop

$L344 = .
	bne	$16,$0,$L345
	nop

$L335:
	lui	$2,%hi(url_filter_mode)
$L359:
	lw	$4,%lo(url_filter_mode)($2)
	li	$3,-1			# 0xffffffffffffffff
	li	$2,1			# 0x1
$L357:
	j	$L334
	movz	$2,$3,$4

$L345:
	lw	$3,%lo(url_filter_flag)($21)
	nop
	bne	$3,$0,$L334
	li	$2,1			# 0x1

	lw	$3,16($16)
	nop
	andi	$4,$3,0x1
	beq	$4,$0,$L334
	move	$2,$0

	andi	$2,$3,0x8
	j	$L334
	sltu	$2,$0,$2

$L351:
	li	$2,-1			# 0xffffffffffffffff
$L334:
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
$LC47:
	.ascii	"[ip_range_filter]%d.%d.%d.%d ===> %d.%d.%d.%d  [Blocked]"
	.ascii	"\012\000"
	.align	2
$LC48:
	.ascii	"[mac_filter]%d.%d.%d.%d(%02x:%02x:%02x:%02x:%02x:%02x) ="
	.ascii	"==> %d.%d.%d.%d [Blocked]\012\000"
	.align	2
$LC49:
	.ascii	"[url_key_filter]%d.%d.%d.%d ==> %d.%d.%d.%d(%s)  [Blocke"
	.ascii	"d]\012\000"
	.align	2
$LC50:
	.ascii	"[schedule_filter]%d.%d.%d.%d ===> %d.%d.%d.%d  [Blocked]"
	.ascii	"\012\000"
	.align	2
$LC51:
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
	move	$18,$5
	move	$16,$4
	jal	do_gettimeofday
	addiu	$4,$sp,136

	jal	rtl_ip_hdr
	move	$4,$18

	move	$4,$18
	jal	rtl_eth_hdr
	move	$17,$2

	li	$3,2			# 0x2
	beq	$16,$3,$L364
	nop

	slt	$2,$16,3
	beq	$2,$0,$L367
	li	$2,3			# 0x3

	li	$2,1			# 0x1
	bne	$16,$2,$L384
	lui	$4,%hi($LC51)

	j	$L382
	nop

$L367:
	beq	$16,$2,$L365
	li	$2,4			# 0x4

	bne	$16,$2,$L362
	lui	$4,%hi($LC51)

	j	$L383
	nop

$L382:
	beq	$17,$0,$L368
	nop

	lw	$5,12($17)
	lw	$2,16($17)
	lui	$4,%hi($LC47)
	andi	$3,$5,0xff
	sw	$3,16($sp)
	srl	$3,$2,24
	sw	$3,20($sp)
	srl	$3,$2,16
	andi	$3,$3,0xff
	sw	$3,24($sp)
	srl	$3,$2,8
	andi	$3,$3,0xff
	andi	$2,$2,0xff
	srl	$6,$5,16
	srl	$7,$5,8
	sw	$3,28($sp)
	sw	$2,32($sp)
	j	$L381
	addiu	$4,$4,%lo($LC47)

$L364:
	beq	$2,$0,$L368
	nop

	beq	$17,$0,$L368
	nop

	lw	$16,12($17)
	jal	rtl_eth_hdr
	move	$4,$18

	move	$4,$18
	jal	rtl_eth_hdr
	move	$23,$2

	move	$4,$18
	jal	rtl_eth_hdr
	move	$22,$2

	move	$4,$18
	jal	rtl_eth_hdr
	move	$21,$2

	move	$4,$18
	jal	rtl_eth_hdr
	move	$20,$2

	move	$4,$18
	jal	rtl_eth_hdr
	move	$19,$2

	srl	$6,$16,16
	srl	$7,$16,8
	srl	$5,$16,24
	andi	$16,$16,0xff
	lw	$4,16($17)
	andi	$6,$6,0xff
	andi	$7,$7,0xff
	sw	$16,16($sp)
	move	$3,$0
	addiu	$8,$sp,20
	j	$L370
	addiu	$23,$23,84

$L369:
	lbu	$10,0($10)
	addu	$9,$8,$3
	addiu	$3,$3,1
	sb	$10,0($9)
$L370:
	sltu	$9,$3,14
	bne	$9,$0,$L369
	addu	$10,$23,$3

	move	$3,$0
	addiu	$8,$sp,36
	j	$L372
	addiu	$22,$22,98

$L371:
	lbu	$10,0($10)
	addu	$9,$8,$3
	addiu	$3,$3,1
	sb	$10,0($9)
$L372:
	sltu	$9,$3,14
	bne	$9,$0,$L371
	addu	$10,$22,$3

	move	$3,$0
	addiu	$8,$sp,52
	j	$L374
	addiu	$21,$21,112

$L373:
	lbu	$10,0($10)
	addu	$9,$8,$3
	addiu	$3,$3,1
	sb	$10,0($9)
$L374:
	sltu	$9,$3,14
	bne	$9,$0,$L373
	addu	$10,$21,$3

	move	$3,$0
	addiu	$8,$sp,68
	j	$L376
	addiu	$20,$20,126

$L375:
	lbu	$10,0($10)
	addu	$9,$8,$3
	addiu	$3,$3,1
	sb	$10,0($9)
$L376:
	sltu	$9,$3,14
	bne	$9,$0,$L375
	addu	$10,$20,$3

	move	$3,$0
	addiu	$8,$sp,84
	j	$L378
	addiu	$19,$19,140

$L377:
	lbu	$10,0($10)
	addu	$9,$8,$3
	addiu	$3,$3,1
	sb	$10,0($9)
$L378:
	sltu	$9,$3,14
	bne	$9,$0,$L377
	addu	$10,$19,$3

	move	$3,$0
	addiu	$8,$sp,100
	j	$L380
	addiu	$2,$2,154

$L379:
	lbu	$10,0($10)
	addu	$9,$8,$3
	addiu	$3,$3,1
	sb	$10,0($9)
$L380:
	sltu	$9,$3,14
	bne	$9,$0,$L379
	addu	$10,$2,$3

	srl	$2,$4,24
	sw	$2,116($sp)
	srl	$2,$4,16
	andi	$2,$2,0xff
	sw	$2,120($sp)
	srl	$2,$4,8
	andi	$4,$4,0xff
	sw	$4,128($sp)
	lui	$4,%hi($LC48)
	andi	$2,$2,0xff
	addiu	$4,$4,%lo($LC48)
	jal	scrlog_printk
	sw	$2,124($sp)

	j	$L368
	nop

$L365:
	beq	$17,$0,$L368
	nop

	lw	$5,12($17)
	lw	$2,16($17)
	lui	$4,%hi($LC49)
	andi	$3,$5,0xff
	sw	$3,16($sp)
	srl	$3,$2,24
	sw	$3,20($sp)
	srl	$3,$2,16
	andi	$3,$3,0xff
	sw	$3,24($sp)
	srl	$3,$2,8
	andi	$2,$2,0xff
	sw	$2,32($sp)
	lui	$2,%hi(log_info)
	andi	$3,$3,0xff
	addiu	$2,$2,%lo(log_info)
	srl	$6,$5,16
	srl	$7,$5,8
	sw	$3,28($sp)
	sw	$2,36($sp)
	addiu	$4,$4,%lo($LC49)
$L381:
	srl	$5,$5,24
	andi	$6,$6,0x00ff
	jal	scrlog_printk
	andi	$7,$7,0x00ff

	j	$L368
	nop

$L383:
	beq	$17,$0,$L368
	nop

	lw	$5,12($17)
	lw	$2,16($17)
	lui	$4,%hi($LC50)
	andi	$3,$5,0xff
	sw	$3,16($sp)
	srl	$3,$2,24
	sw	$3,20($sp)
	srl	$3,$2,16
	andi	$3,$3,0xff
	sw	$3,24($sp)
	srl	$3,$2,8
	andi	$3,$3,0xff
	andi	$2,$2,0xff
	srl	$6,$5,16
	srl	$7,$5,8
	sw	$3,28($sp)
	sw	$2,32($sp)
	j	$L381
	addiu	$4,$4,%lo($LC50)

$L362:
$L384:
	jal	printk
	addiu	$4,$4,%lo($LC51)

$L368:
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
	lui	$20,%hi(table_list_head)
	lw	$16,%lo(table_list_head)($20)
	move	$17,$4
	addiu	$20,$20,%lo(table_list_head)
	li	$19,-1			# 0xffffffffffffffff
	j	$L386
	li	$18,1			# 0x1

$L389:
	move	$5,$0
	move	$6,$16
	jal	scan_table
	move	$7,$16

	beq	$2,$19,$L390
	nop

	bne	$2,$18,$L388
	lui	$2,%hi(log_enable)

	lw	$2,%lo(log_enable)($2)
	nop
	beq	$2,$0,$L387
	li	$18,1			# 0x1

	lw	$4,16($16)
	jal	log_fastFilter
	move	$5,$17

	j	$L392
	move	$2,$18

$L388:
	lw	$16,0($16)
	nop
$L386:
	bne	$16,$20,$L389
	move	$4,$17

$L390:
	move	$18,$0
$L387:
	move	$2,$18
$L392:
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

	beq	$2,$0,$L394
	move	$4,$16

	lw	$31,20($sp)
	lw	$16,16($sp)
	j	do_filter
	addiu	$sp,$sp,24

$L394:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	FilterWeb_v2
	.size	FilterWeb_v2, .-FilterWeb_v2
	.section	.rodata.str1.4
	.align	2
$LC52:
	.ascii	"filter_table\000"
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

	lui	$4,%hi($LC52)
	lui	$6,%hi(proc_root)
	lui	$7,%hi(filter_table_proc_fops)
	addiu	$4,$4,%lo($LC52)
	move	$5,$0
	addiu	$6,$6,%lo(proc_root)
	addiu	$7,$7,%lo(filter_table_proc_fops)
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
	nop
	beq	$2,$0,$L398
	lui	$4,%hi($LC52)

	lui	$5,%hi(proc_root)
	addiu	$4,$4,%lo($LC52)
	jal	remove_proc_entry
	addiu	$5,$5,%lo(proc_root)

	sw	$0,%lo(res12)($16)
$L398:
	lw	$31,20($sp)
	lw	$16,16($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	filter_exit_fastpath
	.size	filter_exit_fastpath, .-filter_exit_fastpath
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
	j	$L400
	li	$5,10			# 0xa

$L402:
	sltu	$6,$3,10
	beq	$6,$0,$L403
	nop

	mult	$2,$5
	addiu	$4,$4,1
	mflo	$2
	addu	$2,$2,$3
$L400:
	lb	$3,0($4)
	nop
	bne	$3,$0,$L402
	addiu	$3,$3,-48

	addiu	$3,$3,48
$L403:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	strtoint
	.size	strtoint, .-strtoint
	.section	.text.strtok,"ax",@progbits
	.align	2
	.globl	strtok
	.set	nomips16
	.ent	strtok
	.type	strtok, @function
strtok:
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
	lui	$17,%hi(___strtok)
	move	$18,$5
	bne	$4,$0,$L405
	move	$16,$4

	lw	$16,%lo(___strtok)($17)
	nop
	beq	$16,$0,$L410
	move	$2,$16

$L405:
	move	$4,$16
	jal	strspn
	move	$5,$18

	addu	$16,$16,$2
	lb	$2,0($16)
	nop
	bne	$2,$0,$L407
	move	$4,$16

	sw	$0,%lo(___strtok)($17)
	j	$L406
	move	$16,$0

$L407:
	jal	strpbrk
	move	$5,$18

	beq	$2,$0,$L408
	nop

	lb	$3,0($2)
	nop
	beq	$3,$0,$L408
	nop

	sb	$0,0($2)
	addiu	$2,$2,1
$L408:
	sw	$2,%lo(___strtok)($17)
$L406:
	move	$2,$16
$L410:
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	strtok
	.size	strtok, .-strtok
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
	sltu	$4,$6,2
	move	$16,$6
	.set	noreorder
	.set	nomacro
	bne	$4,$0,$L412
	li	$2,-14			# 0xfffffffffffffff2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	beq	$5,$0,$L412
	addiu	$4,$5,80
	.set	macro
	.set	reorder

	or	$3,$4,$5
	lw	$4,24($28)
	#nop
	and	$3,$3,$4
	.set	noreorder
	.set	nomacro
	bne	$3,$0,$L412
	lui	$3,%hi(dos_flag)
	.set	macro
	.set	reorder

	addiu	$4,$3,%lo(dos_flag)
	li	$6,80			# 0x50
 #APP
 # 3066 "net/rtl/fastpath/filter.c" 1
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
	bne	$6,$0,$L412
	addiu	$3,$3,%lo(dos_flag)
	.set	macro
	.set	reorder

	addiu	$2,$sp,16
	li	$4,80			# 0x50
 #APP
 # 55 "/home/kai_wang/3473/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/rlx/include/asm/string.h" 1
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
	lui	$fp,%hi($L428)
	li	$18,1			# 0x1
	addiu	$4,$sp,16
	addiu	$fp,$fp,%lo($L428)
	lui	$23,%hi(block_time)
	lui	$22,%hi(per_icmp_threshold)
	lui	$21,%hi(per_udp_threshold)
	lui	$20,%hi(per_fin_threshold)
	.set	noreorder
	.set	nomacro
	j	$L413
	lui	$19,%hi(per_syn_threshold)
	.set	macro
	.set	reorder

$L429:
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
	beq	$4,$0,$L414
	sll	$3,$3,2
	.set	macro
	.set	reorder

	addu	$3,$fp,$3
	lw	$3,0($3)
	#nop
	j	$3
	.section	.rodata.dos_single_write,"a",@progbits
	.align	2
	.align	2
$L428:
	.word	$L415
	.word	$L416
	.word	$L417
	.word	$L418
	.word	$L419
	.word	$L420
	.word	$L421
	.word	$L422
	.word	$L423
	.word	$L424
	.word	$L425
	.word	$L426
	.word	$L427
	.section	.text.dos_single_write
$L415:
	lui	$3,%hi(op_mode)
	.set	noreorder
	.set	nomacro
	j	$L414
	sb	$2,%lo(op_mode)($3)
	.set	macro
	.set	reorder

$L416:
	move	$4,$17
	move	$5,$0
	.set	noreorder
	.set	nomacro
	jal	simple_strtol
	li	$6,16			# 0x10
	.set	macro
	.set	reorder

	lui	$3,%hi(lan_addr)
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(lan_addr)($3)
	.set	macro
	.set	reorder

$L417:
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
	j	$L414
	sw	$2,%lo(lan_mask)($3)
	.set	macro
	.set	reorder

$L418:
	lui	$3,%hi(item)
	sw	$2,%lo(item)($3)
	lui	$2,%hi(item_backup)
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$0,%lo(item_backup)($2)
	.set	macro
	.set	reorder

$L419:
	lui	$3,%hi(whole_syn_threshold)
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(whole_syn_threshold)($3)
	.set	macro
	.set	reorder

$L420:
	lui	$3,%hi(whole_fin_threshold)
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(whole_fin_threshold)($3)
	.set	macro
	.set	reorder

$L421:
	lui	$3,%hi(whole_udp_threshold)
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(whole_udp_threshold)($3)
	.set	macro
	.set	reorder

$L422:
	lui	$3,%hi(whole_icmp_threshold)
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(whole_icmp_threshold)($3)
	.set	macro
	.set	reorder

$L423:
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(per_syn_threshold)($19)
	.set	macro
	.set	reorder

$L424:
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(per_fin_threshold)($20)
	.set	macro
	.set	reorder

$L425:
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(per_udp_threshold)($21)
	.set	macro
	.set	reorder

$L426:
	.set	noreorder
	.set	nomacro
	j	$L414
	sw	$2,%lo(per_icmp_threshold)($22)
	.set	macro
	.set	reorder

$L427:
	sw	$2,%lo(block_time)($23)
$L414:
	addiu	$18,$18,1
	andi	$18,$18,0x00ff
	move	$4,$0
$L413:
	lui	$5,%hi($LC39)
	.set	noreorder
	.set	nomacro
	jal	strtok
	addiu	$5,$5,%lo($LC39)
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$0,$L429
	move	$17,$2
	.set	macro
	.set	reorder

	move	$2,$16
$L412:
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
	beq	$3,$0,$L439
	li	$2,1			# 0x1

	xor	$2,$3,$4
	sltu	$2,$0,$2
$L439:
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	filter_checkConnect
	.size	filter_checkConnect, .-filter_checkConnect
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
	beq	$4,$0,$L440
	nop

	jal	kfree
	nop

	sw	$0,%lo(block_message)($16)
$L440:
	lw	$31,20($sp)
	lw	$16,16($sp)
	j	$31
	addiu	$sp,$sp,24

	.set	macro
	.set	reorder
	.end	unregister_customRspStr
	.size	unregister_customRspStr, .-unregister_customRspStr
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

	beq	$16,$0,$L443
	nop

	jal	strlen
	move	$4,$16

	addiu	$4,$2,1
	jal	__kmalloc
	li	$5,208			# 0xd0

	beq	$2,$0,$L442
	sw	$2,%lo(block_message)($17)

	lui	$5,%hi($LC34)
	move	$4,$2
	addiu	$5,$5,%lo($LC34)
	j	$L445
	move	$6,$16

$L443:
	lui	$16,%hi($LC21)
	jal	strlen
	addiu	$4,$16,%lo($LC21)

	move	$4,$2
	jal	__kmalloc
	li	$5,208			# 0xd0

	beq	$2,$0,$L442
	sw	$2,%lo(block_message)($17)

	lui	$5,%hi($LC34)
	move	$4,$2
	addiu	$5,$5,%lo($LC34)
	addiu	$6,$16,%lo($LC21)
$L445:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	sprintf
	addiu	$sp,$sp,32

$L442:
	lw	$31,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	register_customRspStr
	.size	register_customRspStr, .-register_customRspStr
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
	beq	$4,$0,$L447
	lui	$2,%hi(fast_path_cusRsp401_func)

	j	$L448
	sw	$4,%lo(fast_path_cusRsp401_func)($2)

$L447:
	lui	$3,%hi(GenerateHTTP401)
	addiu	$3,$3,%lo(GenerateHTTP401)
	sw	$3,%lo(fast_path_cusRsp401_func)($2)
$L448:
	beq	$5,$0,$L449
	lui	$2,%hi(fast_path_cusRspTCPFinAck_func)

	j	$L450
	sw	$5,%lo(fast_path_cusRspTCPFinAck_func)($2)

$L449:
	lui	$3,%hi(GenerateTCPFINACK)
	addiu	$3,$3,%lo(GenerateTCPFINACK)
	sw	$3,%lo(fast_path_cusRspTCPFinAck_func)($2)
$L450:
	beq	$6,$0,$L451
	lui	$2,%hi(fast_path_cusRspTCPEndAck_func)

	j	$31
	sw	$6,%lo(fast_path_cusRspTCPEndAck_func)($2)

$L451:
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
	.section	.text.filter_enter,"ax",@progbits
	.align	2
	.globl	filter_enter
	.set	nomips16
	.ent	filter_enter
	.type	filter_enter, @function
filter_enter:
	.frame	$sp,48,$31		# vars= 0, regs= 7/0, args= 16, gp= 0
	.mask	0x803f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-48
	sw	$31,44($sp)
	sw	$21,40($sp)
	sw	$20,36($sp)
	sw	$19,32($sp)
	sw	$18,28($sp)
	sw	$17,24($sp)
	sw	$16,20($sp)
	lui	$18,%hi(item)
	lw	$2,%lo(item)($18)
	nop
	bne	$2,$0,$L455
	move	$19,$4

	lui	$2,%hi(table_list_head+24)
	lw	$2,%lo(table_list_head+24)($2)
	nop
	sltu	$2,$2,2
	bne	$2,$0,$L574
	lui	$2,%hi(none_dos_drop_pkt_cnt)

$L455:
	jal	rtl_get_skb_type
	move	$4,$19

	bne	$2,$0,$L456
	li	$16,4			# 0x4

	jal	rtl_get_skb_protocol
	move	$4,$19

	li	$3,2048			# 0x800
	bne	$2,$3,$L573
	move	$2,$16

	lui	$2,%hi(table_list_head+24)
	lw	$2,%lo(table_list_head+24)($2)
	nop
	sltu	$2,$2,2
	bne	$2,$0,$L457
	nop

	jal	FilterWeb_v2
	move	$4,$19

	beq	$2,$0,$L458
	lui	$3,%hi(fast_path_cusRsp401_func)

	lw	$2,%lo(fast_path_cusRsp401_func)($3)
	j	$L572
	move	$16,$0

$L458:
	lw	$2,%lo(fast_path_cusRsp401_func)($3)
	nop
	beq	$2,$0,$L457
	lui	$3,%hi(url_cache)

	addiu	$3,$3,%lo(url_cache)
	lbu	$4,15($3)
	li	$2,1			# 0x1
	bne	$4,$2,$L459
	move	$16,$0

	lui	$2,%hi(fast_path_cusRspTCPFinAck_func)
	lw	$2,%lo(fast_path_cusRspTCPFinAck_func)($2)
	j	$L572
	nop

$L459:
	lbu	$3,16($3)
	nop
	bne	$3,$2,$L457
	lui	$2,%hi(fast_path_cusRspTCPEndAck_func)

	lw	$2,%lo(fast_path_cusRspTCPEndAck_func)($2)
	move	$16,$0
$L572:
	beq	$2,$0,$L456
	nop

	jalr	$2
	move	$4,$19

	j	$L573
	move	$2,$16

$L457:
	lw	$2,%lo(item)($18)
	nop
	andi	$2,$2,0x1
	beq	$2,$0,$L460
	nop

	jal	rtl_ip_hdr
	move	$4,$19

	lui	$4,%hi($LC35)
	addiu	$4,$4,%lo($LC35)
	lbu	$20,0($2)
	jal	rtl_get_dev_by_name
	move	$16,$2

	lui	$4,%hi($LC36)
	addiu	$4,$4,%lo($LC36)
	lui	$17,%hi(wan_ppp_dev)
	sw	$2,%lo(wan_ppp_dev)($17)
	jal	rtl_get_dev_by_name
	lui	$21,%hi(wan_dev)

	move	$4,$19
	andi	$20,$20,0xf
	jal	rtl_get_skb_dev
	sw	$2,%lo(wan_dev)($21)

	beq	$2,$0,$L460
	nop

	jal	rtl_get_skb_dev
	move	$4,$19

	lw	$3,%lo(wan_dev)($21)
	nop
	beq	$3,$2,$L462
	nop

	jal	rtl_get_skb_dev
	move	$4,$19

	lw	$3,%lo(wan_ppp_dev)($17)
	nop
	bne	$3,$2,$L460
	nop

$L462:
	lw	$21,%lo(item)($18)
	sll	$20,$20,2
	addu	$20,$16,$20
	andi	$2,$21,0x3fe
	beq	$2,$0,$L463
	move	$17,$0

	jal	dos_pkt_locate
	move	$4,$16

	beq	$2,$0,$L463
	andi	$2,$21,0x22

	beq	$2,$0,$L575
	andi	$2,$21,0x44

	lbu	$4,9($16)
	lui	$2,%hi(cur_p_pkt)
	li	$3,6			# 0x6
	lw	$2,%lo(cur_p_pkt)($2)
	bne	$4,$3,$L464
	li	$3,2			# 0x2

	lbu	$4,13($20)
	nop
	andi	$4,$4,0x3f
	bne	$4,$3,$L464
	lui	$3,%hi(block)

	lw	$5,%lo(block)($3)
	move	$4,$3
	li	$3,1			# 0x1
	bne	$5,$3,$L465
	lui	$3,%hi(attack_saddr+4)

	lw	$5,%lo(attack_saddr+4)($3)
	lw	$3,12($16)
	nop
	beq	$5,$3,$L464
	li	$17,-1			# 0xffffffffffffffff

$L465:
	lw	$3,4($2)
	nop
	addiu	$3,$3,1
	sw	$3,4($2)
	lui	$2,%hi(per_syn_threshold)
	lw	$2,%lo(per_syn_threshold)($2)
	nop
	slt	$3,$2,$3
	beq	$3,$0,$L464
	move	$17,$0

	andi	$3,$21,0x20
	beq	$3,$0,$L464
	nop

	blez	$2,$L464
	lui	$2,%hi(attack_saddr+4)

	lw	$3,12($16)
	nop
	sw	$3,%lo(attack_saddr+4)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+4)
	sw	$3,%lo(attack_daddr+4)($2)
	li	$2,1			# 0x1
	sw	$2,%lo(block)($4)
	lui	$2,%hi(LogFlag)
	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x20
	sw	$3,%lo(LogFlag)($2)
$L464:
	andi	$2,$21,0x44
$L575:
	beq	$2,$0,$L466
	lui	$2,%hi(cur_p_pkt)

	lbu	$5,9($16)
	li	$4,6			# 0x6
	lw	$3,%lo(cur_p_pkt)($2)
	bne	$5,$4,$L467
	move	$2,$0

	lbu	$4,13($20)
	li	$5,1			# 0x1
	andi	$4,$4,0x3f
	bne	$4,$5,$L467
	nop

	lui	$2,%hi(block)
	lw	$6,%lo(block)($2)
	nop
	bne	$6,$4,$L468
	move	$5,$2

	lui	$2,%hi(attack_saddr+8)
	lw	$6,%lo(attack_saddr+8)($2)
	lw	$4,12($16)
	nop
	beq	$6,$4,$L467
	li	$2,-1			# 0xffffffffffffffff

$L468:
	lw	$2,8($3)
	nop
	addiu	$2,$2,1
	sw	$2,8($3)
	lui	$3,%hi(per_fin_threshold)
	lw	$3,%lo(per_fin_threshold)($3)
	nop
	slt	$4,$3,$2
	beq	$4,$0,$L467
	move	$2,$0

	andi	$21,$21,0x40
	beq	$21,$0,$L467
	nop

	blez	$3,$L467
	lui	$3,%hi(attack_saddr+8)

	lw	$4,12($16)
	nop
	sw	$4,%lo(attack_saddr+8)($3)
	lw	$4,16($16)
	lui	$3,%hi(attack_daddr+8)
	sw	$4,%lo(attack_daddr+8)($3)
	li	$3,1			# 0x1
	sw	$3,%lo(block)($5)
	lui	$3,%hi(LogFlag)
	lw	$4,%lo(LogFlag)($3)
	nop
	ori	$4,$4,0x40
	sw	$4,%lo(LogFlag)($3)
$L467:
	or	$17,$17,$2
$L466:
	lw	$4,%lo(item)($18)
	nop
	andi	$2,$4,0x88
	beq	$2,$0,$L576
	lui	$2,%hi(cur_p_pkt)

	lbu	$6,9($16)
	li	$5,17			# 0x11
	lw	$3,%lo(cur_p_pkt)($2)
	bne	$6,$5,$L470
	move	$2,$0

	lui	$2,%hi(block)
	lw	$6,%lo(block)($2)
	move	$5,$2
	li	$2,1			# 0x1
	bne	$6,$2,$L471
	nop

	lui	$2,%hi(attack_saddr+12)
	lw	$7,%lo(attack_saddr+12)($2)
	lw	$6,12($16)
	nop
	beq	$7,$6,$L470
	li	$2,-1			# 0xffffffffffffffff

$L471:
	lw	$2,12($3)
	nop
	addiu	$2,$2,1
	sw	$2,12($3)
	lui	$3,%hi(per_udp_threshold)
	lw	$3,%lo(per_udp_threshold)($3)
	nop
	slt	$6,$3,$2
	beq	$6,$0,$L470
	move	$2,$0

	andi	$4,$4,0x80
	beq	$4,$0,$L470
	nop

	blez	$3,$L470
	lui	$3,%hi(attack_saddr+12)

	lw	$4,12($16)
	nop
	sw	$4,%lo(attack_saddr+12)($3)
	lw	$4,16($16)
	lui	$3,%hi(attack_daddr+12)
	sw	$4,%lo(attack_daddr+12)($3)
	li	$3,1			# 0x1
	sw	$3,%lo(block)($5)
	lui	$3,%hi(LogFlag)
	lw	$4,%lo(LogFlag)($3)
	nop
	ori	$4,$4,0x80
	sw	$4,%lo(LogFlag)($3)
$L470:
	or	$17,$17,$2
	lw	$4,%lo(item)($18)
	nop
$L576:
	andi	$2,$4,0x110
	beq	$2,$0,$L472
	lui	$2,%hi(cur_p_pkt)

	lbu	$5,9($16)
	li	$6,1			# 0x1
	lw	$3,%lo(cur_p_pkt)($2)
	bne	$5,$6,$L473
	move	$2,$0

	lui	$2,%hi(block)
	lw	$7,%lo(block)($2)
	nop
	bne	$7,$5,$L474
	move	$6,$2

	lui	$2,%hi(attack_saddr+16)
	lw	$7,%lo(attack_saddr+16)($2)
	lw	$5,12($16)
	nop
	beq	$7,$5,$L473
	li	$2,-1			# 0xffffffffffffffff

$L474:
	lw	$2,16($3)
	nop
	addiu	$2,$2,1
	sw	$2,16($3)
	lui	$3,%hi(per_icmp_threshold)
	lw	$3,%lo(per_icmp_threshold)($3)
	nop
	slt	$5,$3,$2
	beq	$5,$0,$L473
	move	$2,$0

	andi	$4,$4,0x100
	beq	$4,$0,$L473
	nop

	blez	$3,$L473
	lui	$3,%hi(attack_saddr+16)

	lw	$4,12($16)
	nop
	sw	$4,%lo(attack_saddr+16)($3)
	lw	$4,16($16)
	lui	$3,%hi(attack_daddr+16)
	sw	$4,%lo(attack_daddr+16)($3)
	li	$3,1			# 0x1
	sw	$3,%lo(block)($6)
	lui	$3,%hi(LogFlag)
	lw	$4,%lo(LogFlag)($3)
	nop
	ori	$4,$4,0x100
	sw	$4,%lo(LogFlag)($3)
$L473:
	or	$17,$17,$2
$L472:
	lw	$2,%lo(item)($18)
	nop
	andi	$2,$2,0x200
	beq	$2,$0,$L475
	lui	$2,%hi(cur_p_pkt)

	lbu	$4,9($16)
	li	$3,6			# 0x6
	lw	$2,%lo(cur_p_pkt)($2)
	bne	$4,$3,$L577
	li	$3,17			# 0x11

	lhu	$4,32($2)
	lhu	$3,2($20)
	beq	$4,$0,$L477
	nop

	beq	$3,$4,$L476
	nop

	lw	$3,20($2)
	nop
	addiu	$3,$3,1
	j	$L476
	sw	$3,20($2)

$L477:
	sh	$3,32($2)
$L476:
	lbu	$4,9($16)
	li	$3,17			# 0x11
$L577:
	bne	$4,$3,$L479
	nop

	lhu	$4,32($2)
	lhu	$3,2($20)
	beq	$4,$0,$L480
	nop

	beq	$3,$4,$L479
	nop

	lw	$3,20($2)
	nop
	addiu	$3,$3,1
	j	$L479
	sw	$3,20($2)

$L480:
	sh	$3,32($2)
$L479:
	lw	$3,%lo(item)($18)
	li	$4,8388608			# 0x800000
	lw	$2,20($2)
	and	$3,$4,$3
	beq	$3,$0,$L482
	nop

	j	$L571
	slt	$2,$2,11

$L482:
	slt	$2,$2,201
$L571:
	bne	$2,$0,$L475
	lui	$2,%hi(LogFlag)

	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x200
	sw	$3,%lo(LogFlag)($2)
	lw	$3,12($16)
	lui	$2,%hi(attack_saddr+20)
	sw	$3,%lo(attack_saddr+20)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+20)
	sw	$3,%lo(attack_daddr+20)($2)
$L475:
	lw	$2,%lo(item)($18)
	li	$3,4194304			# 0x400000
	and	$2,$3,$2
	movz	$17,$0,$2
$L463:
	lw	$2,%lo(item)($18)
	nop
	andi	$2,$2,0x8000
	beq	$2,$0,$L484
	li	$3,6			# 0x6

	lbu	$4,9($16)
	nop
	bne	$4,$3,$L485
	move	$2,$0

	lbu	$4,13($20)
	lui	$3,%hi(_tcpDosScanBitmap)
	addiu	$3,$3,%lo(_tcpDosScanBitmap)
	andi	$4,$4,0x3f
	addu	$3,$4,$3
	lb	$3,0($3)
	nop
	beq	$3,$0,$L485
	nop

	lui	$2,%hi(LogFlag)
	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x8000
	sw	$3,%lo(LogFlag)($2)
	lw	$3,12($16)
	lui	$2,%hi(attack_saddr+44)
	sw	$3,%lo(attack_saddr+44)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+44)
	sw	$3,%lo(attack_daddr+44)($2)
	li	$2,-1			# 0xffffffffffffffff
$L485:
	or	$17,$17,$2
$L484:
	lw	$2,%lo(item)($18)
	li	$3,65536			# 0x10000
	and	$2,$3,$2
	beq	$2,$0,$L486
	li	$4,6			# 0x6

	lbu	$5,9($16)
	nop
	bne	$5,$4,$L487
	move	$2,$0

	lbu	$5,13($20)
	li	$4,2			# 0x2
	andi	$5,$5,0x3f
	bne	$5,$4,$L487
	nop

	lbu	$5,0($16)
	lhu	$4,2($16)
	andi	$5,$5,0xf
	sll	$5,$5,2
	subu	$5,$4,$5
	lw	$4,12($20)
	nop
	srl	$4,$4,28
	sll	$4,$4,2
	bne	$5,$4,$L564
	nop

$L488:
	lhu	$4,6($16)
	nop
	andi	$4,$4,0x2000
	beq	$4,$0,$L487
	nop

$L564:
	lui	$2,%hi(LogFlag)
	lw	$4,%lo(LogFlag)($2)
	nop
	or	$3,$4,$3
	sw	$3,%lo(LogFlag)($2)
	lw	$3,12($16)
	lui	$2,%hi(attack_saddr+48)
	sw	$3,%lo(attack_saddr+48)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+48)
	sw	$3,%lo(attack_daddr+48)($2)
	li	$2,-1			# 0xffffffffffffffff
$L487:
	or	$17,$17,$2
$L486:
	lw	$3,%lo(item)($18)
	nop
	andi	$2,$3,0x800
	beq	$2,$0,$L578
	li	$2,262144			# 0x40000

	lw	$2,12($16)
	lw	$5,16($16)
	nop
	bne	$2,$5,$L490
	move	$4,$0

	lui	$4,%hi(LogFlag)
	lw	$5,%lo(LogFlag)($4)
	nop
	ori	$5,$5,0x800
	sw	$5,%lo(LogFlag)($4)
	lui	$4,%hi(attack_saddr+28)
	sw	$2,%lo(attack_saddr+28)($4)
	lui	$4,%hi(attack_daddr+28)
	sw	$2,%lo(attack_daddr+28)($4)
	li	$4,-1			# 0xffffffffffffffff
$L490:
	or	$17,$17,$4
	li	$2,262144			# 0x40000
$L578:
	and	$3,$3,$2
	beq	$3,$0,$L491
	li	$2,17			# 0x11

	lbu	$3,9($16)
	nop
	bne	$3,$2,$L492
	move	$4,$0

	lhu	$2,2($20)
	li	$6,7			# 0x7
	beq	$2,$6,$L493
	nop

	beq	$2,$3,$L493
	li	$5,19			# 0x13

	beq	$2,$5,$L579
	lui	$2,%hi(LogFlag)

	lhu	$2,0($20)
	nop
	beq	$2,$6,$L493
	nop

	beq	$2,$3,$L493
	nop

	bne	$2,$5,$L492
	nop

$L493:
	lui	$2,%hi(LogFlag)
$L579:
	lw	$4,%lo(LogFlag)($2)
	li	$3,262144			# 0x40000
	or	$3,$4,$3
	sw	$3,%lo(LogFlag)($2)
	lw	$3,12($16)
	lui	$2,%hi(attack_saddr+56)
	li	$4,-1			# 0xffffffffffffffff
	sw	$3,%lo(attack_saddr+56)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+56)
	sw	$3,%lo(attack_daddr+56)($2)
$L492:
	or	$17,$17,$4
$L491:
	lw	$2,%lo(item)($18)
	li	$3,131072			# 0x20000
	and	$2,$3,$2
	beq	$2,$0,$L494
	li	$4,17			# 0x11

	lbu	$5,9($16)
	nop
	bne	$5,$4,$L495
	move	$2,$0

	lhu	$4,6($16)
	nop
	andi	$4,$4,0x3fff
	bne	$4,$0,$L495
	nop

	lbu	$4,0($16)
	lhu	$5,2($16)
	andi	$4,$4,0xf
	sll	$4,$4,2
	subu	$4,$5,$4
	lhu	$5,4($20)
	nop
	slt	$4,$5,$4
	beq	$4,$0,$L495
	nop

	lui	$2,%hi(LogFlag)
	lw	$4,%lo(LogFlag)($2)
	nop
	or	$3,$4,$3
	sw	$3,%lo(LogFlag)($2)
	lw	$3,12($16)
	lui	$2,%hi(attack_saddr+52)
	sw	$3,%lo(attack_saddr+52)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+52)
	sw	$3,%lo(attack_daddr+52)($2)
	li	$2,-1			# 0xffffffffffffffff
$L495:
	or	$17,$17,$2
$L494:
	lw	$2,%lo(item)($18)
	nop
	andi	$2,$2,0x4000
	beq	$2,$0,$L496
	nop

	lhu	$3,6($16)
	nop
	andi	$4,$3,0x2000
	bne	$4,$0,$L497
	move	$2,$0

	andi	$3,$3,0x1fff
	beq	$3,$0,$L497
	sll	$3,$3,3

	lhu	$4,2($16)
	nop
	addu	$3,$3,$4
	lbu	$4,0($16)
	nop
	andi	$4,$4,0xf
	sll	$4,$4,2
	subu	$3,$3,$4
	li	$4,65536			# 0x10000
	sltu	$3,$3,$4
	bne	$3,$0,$L497
	nop

	lui	$2,%hi(LogFlag)
	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x4000
	sw	$3,%lo(LogFlag)($2)
	lw	$3,12($16)
	lui	$2,%hi(attack_saddr+40)
	sw	$3,%lo(attack_saddr+40)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+40)
	sw	$3,%lo(attack_daddr+40)($2)
	li	$2,-1			# 0xffffffffffffffff
$L497:
	or	$17,$17,$2
$L496:
	lw	$2,%lo(item)($18)
	nop
	andi	$2,$2,0x400
	beq	$2,$0,$L498
	li	$4,1			# 0x1

	lbu	$3,0($16)
	lbu	$5,9($16)
	andi	$3,$3,0xf
	bne	$5,$4,$L499
	move	$2,$0

	sll	$3,$3,2
	addu	$3,$16,$3
	lbu	$4,0($3)
	li	$3,8			# 0x8
	bne	$4,$3,$L499
	lui	$4,%hi(lan_addr)

	lw	$3,12($16)
	lw	$4,%lo(lan_addr)($4)
	lui	$5,%hi(lan_mask)
	lw	$5,%lo(lan_mask)($5)
	xor	$4,$3,$4
	and	$4,$4,$5
	bne	$4,$0,$L499
	nop

	lui	$2,%hi(LogFlag)
	lw	$4,%lo(LogFlag)($2)
	nop
	ori	$4,$4,0x400
	sw	$4,%lo(LogFlag)($2)
	lui	$2,%hi(attack_saddr+24)
	sw	$3,%lo(attack_saddr+24)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+24)
	sw	$3,%lo(attack_daddr+24)($2)
	li	$2,-1			# 0xffffffffffffffff
$L499:
	or	$17,$17,$2
$L498:
	lw	$2,%lo(item)($18)
	nop
	andi	$2,$2,0x1000
	beq	$2,$0,$L500
	lui	$2,%hi(lan_addr)

	lw	$4,%lo(lan_addr)($2)
	lw	$3,12($16)
	lui	$2,%hi(lan_mask)
	lw	$2,%lo(lan_mask)($2)
	xor	$4,$3,$4
	and	$4,$4,$2
	bne	$4,$0,$L501
	move	$2,$0

	lbu	$5,9($16)
	li	$4,1			# 0x1
	beq	$5,$4,$L501
	nop

	lui	$2,%hi(LogFlag)
	lw	$4,%lo(LogFlag)($2)
	nop
	ori	$4,$4,0x1000
	sw	$4,%lo(LogFlag)($2)
	lui	$2,%hi(attack_saddr+32)
	sw	$3,%lo(attack_saddr+32)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+32)
	sw	$3,%lo(attack_daddr+32)($2)
	li	$2,-1			# 0xffffffffffffffff
$L501:
	or	$17,$17,$2
$L500:
	lw	$2,%lo(item)($18)
	nop
	andi	$2,$2,0x2000
	beq	$2,$0,$L502
	nop

	jal	dos_pkt_locate
	move	$4,$16

	beq	$2,$0,$L503
	move	$3,$0

	lui	$2,%hi(cur_p_pkt)
	lw	$2,%lo(cur_p_pkt)($2)
	lhu	$6,4($16)
	lhu	$4,28($2)
	nop
	beq	$6,$4,$L504
	nop

	bne	$4,$0,$L503
	nop

$L504:
	lhu	$4,6($16)
	nop
	andi	$5,$4,0x3fff
	beq	$5,$0,$L503
	move	$3,$0

	lhu	$5,30($2)
	sll	$7,$4,3
	andi	$7,$7,0xffff
	slt	$7,$7,$5
	bne	$7,$0,$L505
	andi	$4,$4,0x2000

	bne	$4,$0,$L506
	nop

	sh	$0,28($2)
	j	$L503
	sh	$0,30($2)

$L506:
	sh	$6,28($2)
	lhu	$4,2($16)
	nop
	addu	$5,$5,$4
	lbu	$4,0($16)
	nop
	andi	$4,$4,0xf
	sll	$4,$4,2
	subu	$5,$5,$4
	j	$L503
	sh	$5,30($2)

$L505:
	lui	$2,%hi(LogFlag)
	lw	$3,%lo(LogFlag)($2)
	nop
	ori	$3,$3,0x2000
	sw	$3,%lo(LogFlag)($2)
	lw	$3,12($16)
	lui	$2,%hi(attack_saddr+36)
	sw	$3,%lo(attack_saddr+36)($2)
	lw	$3,16($16)
	lui	$2,%hi(attack_daddr+36)
	sw	$3,%lo(attack_daddr+36)($2)
	li	$3,-1			# 0xffffffffffffffff
$L503:
	or	$17,$17,$3
$L502:
	bne	$17,$0,$L456
	move	$16,$0

$L460:
	jal	rtl_ip_hdr
	move	$4,$19

	lbu	$3,9($2)
	li	$2,1			# 0x1
	beq	$3,$2,$L456
	li	$16,4			# 0x4

	lui	$2,%hi(none_dos_drop_pkt_cnt)
$L574:
	lw	$3,%lo(none_dos_drop_pkt_cnt)($2)
	li	$16,1			# 0x1
	addiu	$3,$3,1
	sw	$3,%lo(none_dos_drop_pkt_cnt)($2)
$L456:
	move	$2,$16
$L573:
	lw	$31,44($sp)
	lw	$21,40($sp)
	lw	$20,36($sp)
	lw	$19,32($sp)
	lw	$18,28($sp)
	lw	$17,24($sp)
	lw	$16,20($sp)
	j	$31
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	filter_enter
	.size	filter_enter, .-filter_enter
	.section	.rodata.str1.4
	.align	2
$LC53:
	.ascii	"enable_dos\000"
	.section	.init.text,"ax",@progbits
	.align	2
	.globl	filter_init
	.set	nomips16
	.ent	filter_init
	.type	filter_init, @function
filter_init:
	.frame	$sp,32,$31		# vars= 0, regs= 2/0, args= 24, gp= 0
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$16,24($sp)
	lui	$2,%hi(item)
	sw	$0,%lo(item)($2)
	lui	$4,%hi($LC53)
	lui	$2,%hi(none_dos_drop_pkt_cnt)
	lui	$6,%hi(proc_root)
	lui	$7,%hi(dos_proc_fops)
	lui	$16,%hi(dos_timer)
	sw	$0,%lo(none_dos_drop_pkt_cnt)($2)
	addiu	$4,$4,%lo($LC53)
	lui	$2,%hi(url_count)
	move	$5,$0
	addiu	$6,$6,%lo(proc_root)
	addiu	$7,$7,%lo(dos_proc_fops)
	sw	$0,16($sp)
	jal	proc_create_data
	sb	$0,%lo(url_count)($2)

	move	$7,$0
	lui	$3,%hi(res3)
	addiu	$4,$16,%lo(dos_timer)
	move	$6,$0
	move	$5,$0
	jal	init_timer_key
	sw	$2,%lo(res3)($3)

	lui	$2,%hi(jiffies)
	lw	$3,%lo(jiffies)($2)
	addiu	$4,$16,%lo(dos_timer)
	lw	$5,%lo(jiffies)($2)
	addiu	$3,$3,100
	sw	$3,8($4)
	lui	$3,%hi(dos_timer_fn)
	addiu	$3,$3,%lo(dos_timer_fn)
	sw	$3,16($4)
	lui	$16,%hi(_tcpDosScanBitmap)
	sw	$0,20($4)
	jal	mod_timer
	addiu	$5,$5,100

	addiu	$4,$16,%lo(_tcpDosScanBitmap)
	move	$5,$0
	jal	memset
	li	$6,64			# 0x40

	addiu	$3,$16,%lo(_tcpDosScanBitmap)
	li	$2,1			# 0x1
	sb	$2,63($3)
	sb	$2,58($3)
	sb	$2,41($3)
	sb	$2,40($3)
	sb	$2,33($3)
	sb	$2,32($3)
	sb	$2,9($3)
	sb	$2,8($3)
	sb	$2,3($3)
	jal	filter_init_fastpath
	sb	$2,%lo(_tcpDosScanBitmap)($16)

	jal	igmp_delete_init_netlink
	nop

	lw	$31,28($sp)
	lw	$16,24($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	filter_init
	.size	filter_init, .-filter_init
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
	beq	$5,$0,$L582
	nop

	lui	$4,%hi($LC53)
	jal	remove_proc_entry
	addiu	$4,$4,%lo($LC53)

	sw	$0,%lo(res3)($16)
$L582:
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
	.globl	fast_path_cusRspTCPEndAck_func
	.section	.bss.fast_path_cusRspTCPEndAck_func,"aw",@nobits
	.align	2
	.type	fast_path_cusRspTCPEndAck_func, @object
	.size	fast_path_cusRspTCPEndAck_func, 4
fast_path_cusRspTCPEndAck_func:
	.space	4
	.globl	fast_path_cusRspTCPFinAck_func
	.section	.bss.fast_path_cusRspTCPFinAck_func,"aw",@nobits
	.align	2
	.type	fast_path_cusRspTCPFinAck_func, @object
	.size	fast_path_cusRspTCPFinAck_func, 4
fast_path_cusRspTCPFinAck_func:
	.space	4
	.globl	fast_path_cusRsp401_func
	.section	.bss.fast_path_cusRsp401_func,"aw",@nobits
	.align	2
	.type	fast_path_cusRsp401_func, @object
	.size	fast_path_cusRsp401_func, 4
fast_path_cusRsp401_func:
	.space	4
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
	.globl	___strtok
	.section	.bss.___strtok,"aw",@nobits
	.align	2
	.type	___strtok, @object
	.size	___strtok, 4
___strtok:
	.space	4
	.globl	wan_ppp_dev
	.section	.bss.wan_ppp_dev,"aw",@nobits
	.align	2
	.type	wan_ppp_dev, @object
	.size	wan_ppp_dev, 4
wan_ppp_dev:
	.space	4
	.globl	wan_dev
	.section	.bss.wan_dev,"aw",@nobits
	.align	2
	.type	wan_dev, @object
	.size	wan_dev, 4
wan_dev:
	.space	4
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
	.globl	table_list_head
	.section	.bss.table_list_head,"aw",@nobits
	.align	2
	.type	table_list_head, @object
	.size	table_list_head, 32
table_list_head:
	.space	32
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
	.section	.bss.get_info_from_usr_space,"aw",@nobits
	.align	2
	.type	get_info_from_usr_space, @object
	.size	get_info_from_usr_space, 1024
get_info_from_usr_space:
	.space	1024
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
	.section	.bss.filter_rule_num,"aw",@nobits
	.align	2
	.type	filter_rule_num, @object
	.size	filter_rule_num, 4
filter_rule_num:
	.space	4
	.section	.bss.dos_flag,"aw",@nobits
	.align	2
	.type	dos_flag, @object
	.size	dos_flag, 80
dos_flag:
	.space	80
	.section	.bss.op_mode,"aw",@nobits
	.type	op_mode, @object
	.size	op_mode, 1
op_mode:
	.space	1
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
	.section	.dram-l34-fwd,"aw",@progbits
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
	.section	.bss.block_time,"aw",@nobits
	.align	2
	.type	block_time, @object
	.size	block_time, 4
block_time:
	.space	4
	.section	.bss.log_info,"aw",@nobits
	.align	2
	.type	log_info, @object
	.size	log_info, 32
log_info:
	.space	32
	.section	.bss.url_filter_flag,"aw",@nobits
	.align	2
	.type	url_filter_flag, @object
	.size	url_filter_flag, 4
url_filter_flag:
	.space	4
	.section	.bss.res12,"aw",@nobits
	.align	2
	.type	res12, @object
	.size	res12, 4
res12:
	.space	4
	.section	.bss.block_message,"aw",@nobits
	.align	2
	.type	block_message, @object
	.size	block_message, 4
block_message:
	.space	4
	.section	.bss.url_cache,"aw",@nobits
	.align	2
	.type	url_cache, @object
	.size	url_cache, 20
url_cache:
	.space	20
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
	.section	.bss.ConnectedIp,"aw",@nobits
	.align	2
	.type	ConnectedIp, @object
	.size	ConnectedIp, 1024
ConnectedIp:
	.space	1024
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
	.section	.dram-l34-fwd
	.align	2
	.type	none_dos_drop_pkt_cnt, @object
	.size	none_dos_drop_pkt_cnt, 4
none_dos_drop_pkt_cnt:
	.space	4
	.section	.bss.cur_p_pkt,"aw",@nobits
	.align	2
	.type	cur_p_pkt, @object
	.size	cur_p_pkt, 4
cur_p_pkt:
	.space	4
	.section	.bss.block,"aw",@nobits
	.align	2
	.type	block, @object
	.size	block, 4
block:
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
	.section	.bss.LogFlag,"aw",@nobits
	.align	2
	.type	LogFlag, @object
	.size	LogFlag, 4
LogFlag:
	.space	4
	.section	.bss._tcpDosScanBitmap,"aw",@nobits
	.align	2
	.type	_tcpDosScanBitmap, @object
	.size	_tcpDosScanBitmap, 64
_tcpDosScanBitmap:
	.space	64
	.section	.bss.dos_pkt,"aw",@nobits
	.align	2
	.type	dos_pkt, @object
	.size	dos_pkt, 36864
dos_pkt:
	.space	36864
	.section	.dram-l34-fwd
	.type	url_count, @object
	.size	url_count, 1
url_count:
	.space	1
	.section	.bss.res3,"aw",@nobits
	.align	2
	.type	res3, @object
	.size	res3, 4
res3:
	.space	4
	.section	.bss.dos_timer,"aw",@nobits
	.align	2
	.type	dos_timer, @object
	.size	dos_timer, 28
dos_timer:
	.space	28
	.section	.bss.block_count,"aw",@nobits
	.align	2
	.type	block_count, @object
	.size	block_count, 4
block_count:
	.space	4
	.ident	"GCC: (Realtek RSDK-4.6.4 Build 2080) 4.6.4"
