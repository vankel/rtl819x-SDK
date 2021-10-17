

/*
 *  memory read or write speed benchmark program.
 *  support cache/uncache read/write and
 *          cache write with cache write back
 *  date:2010.04.21
 */


#include <asm/regdef.h>
//#include "mips.h"
#include "radiax.h"

	.text
	.set	noat
	.set	noreorder
	.globl	byte_r
	.ent	byte_r
byte_r:
1:
	addiu	a1, a1, -1
	bne	zero, a1, 1b
	lbp	t0,(a0)+1

	jr		ra
	nop

	.end	byte_r




	.text
	.set	noat
	.set	noreorder
	.globl	short_r
	.ent	short_r
short_r:
1:
	addiu	a1, a1, -2
	bne	zero, a1, 1b
	lhp	t0,(a0)+2

	jr		ra
	nop

	.end	short_r


	.text
	.set	noat
	.set	noreorder
	.globl	word_r
	.ent	word_r
word_r:
1:
	addiu	a1, a1, -4
	bne	zero, a1, 1b
	lwp	t0,(a0)+4

	jr		ra
	nop

	.end	word_r


	.text
	.set	noat
	.set	noreorder
	.globl	tword_r
	.ent	tword_r
tword_r:
1:
	addiu	a1, a1, -8
	bne	zero, a1, 1b
	ltp	t0,(a0)+8

	jr		ra
	nop
	.end	tword_r


	.text
	.set	noat
	.set	noreorder
	.globl	uncache_byte_w
	.ent	uncache_byte_w
byte_w:
1:
	addiu	a1, a1, -1
	bne	zero, a1, 1b
	sbp	zero,(a0)+1

	jr		ra
	nop

	.end	byte_w




	.text
	.set	noat
	.set	noreorder
	.globl	short_w
	.ent	short_w
short_w:
1:
	addiu	a1, a1, -2
	bne	zero, a1, 1b
	shp	zero,(a0)+2

	jr		ra
	nop

	.end	short_w


	.text
	.set	noat
	.set	noreorder
	.globl	word_w
	.ent	word_w
word_w:
1:
	addiu	a1, a1, -4
	bne	zero, a1, 1b
	swp	zero,(a0)+4

	jr		ra
	nop

	.end	word_w


	.text
	.set	noat
	.set	noreorder
	.globl	tword_w
	.ent	tword_w
tword_w:
1:
	addiu	a1, a1, -8
	bne	zero, a1, 1b
	stp	t0,(a0)+8

	jr		ra
	nop

	.end	tword_w



	.text
	.set	noat
	.set	noreorder
	.globl	byte_w_cache_wb
	.ent	byte_w_cache_wb
byte_w_cache_wb:
	//lui	t1, 0x8000
	//or	a0, a0, t1 /* physic addr to cache */
	or	t2, zero, 0xf

1:
	andi	t3, a0, 0xf
	bgtz		t3, 3f
	addiu	a1, a1, -1
	cache	0x19, -1(a0)	/* write back cache */

3:
	bne	zero, a1, 1b
	sbp	t0,(a0)+1



	jr		ra
	cache	0x19, -1(a0)	/* write back cache */

	.end	byte_w_cache_wb




	.text
	.set	noat
	.set	noreorder
	.globl	short_w_cache_wb
	.ent	short_w_cache_wb
short_w_cache_wb:
	lui	t1, 0x8000
	or	a0, a0, t1 /* physic addr to cache */


1:
	andi	t3, a0, 0xf
	bgtz		t3, 3f
	addiu	a1, a1, -2
	cache	0x19, -1(a0)	/* write back cache */

3:
	bne	zero, a1, 1b
	shp	t0,(a0)+2


	jr		ra
	cache	0x19, -1(a0)	/* write back cache */
	.end	short_w_cache_wb



	.text
	.set	noat
	.set	noreorder
	.globl	word_w_cache_wb
	.ent	word_w_cache_wb
word_w_cache_wb:
	lui	t1, 0x8000
	or	a0, a0, t1 /* physic addr to cache */


1:
	andi	t3, a0, 0xf
	bgtz		t3, 3f
	addiu	a1, a1, -4
	cache	0x19, -1(a0)	/* write back cache */

3:
	bne	zero, a1, 1b
	swp	t0,(a0)+4

	jr		ra
	cache	0x19, -1(a0)	/* write back cache */

	.end	word_w_cache_wb


	.text
	.set	noat
	.set	noreorder
	.globl	tword_w_cache_wb
	.ent	tword_w_cache_wb
tword_w_cache_wb:
	lui	t1, 0x8000
	or	a0, a0, t1 /* physic addr to cache */


1:
	andi	t3, a0, 0xf
	bgtz		t3, 3f
	addiu	a1, a1, -8
	cache	0x19, -1(a0)	/* write back cache */

3:
	bne	zero, a1, 1b
	stp	t0,(a0)+8
	jr		ra
	cache	0x19, -1(a0)	/* write back cache */

	.end	tword_w_cache_wb
