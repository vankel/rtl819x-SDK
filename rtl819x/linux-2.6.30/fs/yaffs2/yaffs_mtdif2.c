/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* mtd interface for YAFFS2 */

#include "yportenv.h"
#include "yaffs_trace.h"

#include "yaffs_mtdif2.h"

#include "linux/mtd/mtd.h"
#include "linux/types.h"
#include "linux/time.h"
#include <asm/cacheflush.h>
#include <linux/delay.h>
#include "yaffs_packedtags2.h"

#include "yaffs_linux.h"


void memDumpYaffs2 (void *start, u32 size, char * strHeader)
{
	int row, column, index, index2, max;
//	uint32 buffer[5];
	u8 *buf, *line, ascii[17];
	char empty = ' ';

	if(!start ||(size==0))
		return;
	line = (u8*)start;

	/*
	16 bytes per line
	*/
	if (strHeader)
		printk("%s", strHeader);
	column = size % 16;
	row = (size / 16) + 1;
	for (index = 0; index < row; index++, line += 16) 
	{
		buf = line;

		memset (ascii, 0, 17);

		max = (index == row - 1) ? column : 16;
		if ( max==0 ) break; /* If we need not dump this line, break it. */

		printk("\n%08x ", (u32) line);
		
		//Hex
		for (index2 = 0; index2 < max; index2++)
		{
			if (index2 == 8)
			printk("  ");
			printk("%02x ", (u8) buf[index2]);
			ascii[index2] = ((u8) buf[index2] < 32) ? empty : buf[index2];
		}

		if (max != 16)
		{
			if (max < 8)
				printk("  ");
			for (index2 = 16 - max; index2 > 0; index2--)
				printk("   ");
		}

		//ASCII
		printk("  %s", ascii);
	}
	printk("\n");
	return;
}

/* NB For use with inband tags....
 * We assume that the data buffer is of size total_bytes_per_chunk so
 * that we can also use it to load the tags.
 */
int nandmtd2_write_chunk_tags(struct yaffs_dev *dev, int nand_chunk,
			      const u8 *data,
			      const struct yaffs_ext_tags *tags)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
#if (MTD_VERSION_CODE > MTD_VERSION(2, 6, 17))
	struct mtd_oob_ops ops;
#else
	size_t dummy;
#endif
	int retval = 0;

	loff_t addr;

	struct yaffs_packed_tags2 pt;

	int packed_tags_size =
	    dev->param.no_tags_ecc ? sizeof(pt.t) : sizeof(pt);
	void *packed_tags_ptr =
	    dev->param.no_tags_ecc ? (void *)&pt.t : (void *)&pt;
/*
	yaffs_trace(YAFFS_TRACE_MTD,
		"nandmtd2_write_chunk_tags chunk %d data %p tags %p",
		nand_chunk, data, tags);
*/

	//printk("nandmtd2_write_chunk_tags chunk %d data %p tags %p\n",
	//		nand_chunk, data, tags);

	//printk("%s-%d: total_bytes_per_chunk:%d\n",__FUNCTION__,__LINE__,dev->param.total_bytes_per_chunk);

	addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;

	/* For yaffs2 writing there must be both data and tags.
	 * If we're using inband tags, then the tags are stuffed into
	 * the end of the data buffer.
	 */
	if (!data || !tags)
		BUG();
	else if (dev->param.inband_tags) {
		struct yaffs_packed_tags2_tags_only *pt2tp;
		pt2tp =
		    (struct yaffs_packed_tags2_tags_only *)(data +
							dev->data_bytes_per_chunk);
//printk("%s-%d\n",__FUNCTION__,__LINE__);		
		yaffs_pack_tags2_tags_only(pt2tp, tags);
	} else {
		yaffs_pack_tags2(&pt, tags, !dev->param.no_tags_ecc);
	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
	//printk("%s-%d\n",__FUNCTION__,__LINE__);
	//memDumpYaffs2(data,dev->param.total_bytes_per_chunk,"YaffsTags");

	ops.mode = MTD_OOB_AUTO;
	ops.ooblen = (dev->param.inband_tags) ? 0 : packed_tags_size;
	ops.len = dev->param.total_bytes_per_chunk;
	ops.ooboffs = 0;
	ops.datbuf = (u8 *) data;
	ops.oobbuf = (dev->param.inband_tags) ? NULL : packed_tags_ptr;
	retval = mtd->write_oob(mtd, addr, &ops);

#else
	if (!dev->param.inband_tags) {
//printk("%s-%d\n",__FUNCTION__,__LINE__);				
		retval =
		    mtd->write_ecc(mtd, addr, dev->data_bytes_per_chunk,
				   &dummy, data, (u8 *) packed_tags_ptr, NULL);
	} else {
//printk("%s-%d\n",__FUNCTION__,__LINE__);
//		memDumpYaffs2(data,dev->param.total_bytes_per_chunk,"WTags");

		retval =
		    mtd->write(mtd, addr, dev->param.total_bytes_per_chunk,
			       &dummy, data);
	}
#endif

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_read_chunk_tags(struct yaffs_dev *dev, int nand_chunk,
			     u8 *data, struct yaffs_ext_tags *tags)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
#if (MTD_VERSION_CODE > MTD_VERSION(2, 6, 17))
	struct mtd_oob_ops ops;
#endif
	size_t dummy;
	int retval = 0;
	int local_data = 0;

	loff_t addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;

	struct yaffs_packed_tags2 pt;

	int packed_tags_size =
	    dev->param.no_tags_ecc ? sizeof(pt.t) : sizeof(pt);
	void *packed_tags_ptr =
	    dev->param.no_tags_ecc ? (void *)&pt.t : (void *)&pt;
/*
	yaffs_trace(YAFFS_TRACE_MTD,
		"nandmtd2_read_chunk_tags chunk %d data %p tags %p",
		nand_chunk, data, tags);
*/

		//printk("nandmtd2_read_chunk_tags chunk %d data %p tags %p\n",
		//	nand_chunk, data, tags);

	//printk("%s-%d: total_bytes_per_chunk:%d, data_bytes_per_chunk:%d\n",__FUNCTION__,__LINE__,dev->param.total_bytes_per_chunk,dev->data_bytes_per_chunk);
	//printk("%s-%d: packed_tags_size:%d packed_tags_ptr:%p\n",__FUNCTION__,__LINE__,packed_tags_size, packed_tags_ptr);

printk("");	
//__flush_cache_all();
//mdelay(100);

	if (dev->param.inband_tags) {

		if (!data) {
			local_data = 1;
			data = yaffs_get_temp_buffer(dev);
		}

	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
	if (dev->param.inband_tags || (data && !tags)){
		retval = mtd->read(mtd, addr, dev->param.total_bytes_per_chunk,
				   &dummy, data);
		//printk("%s-%d: addr:%lld dummy:%d\n",__FUNCTION__,__LINE__,addr, dummy);
		//mDumpYaffs2(data,dev->param.total_bytes_per_chunk,"RTags");
	}
	else if (tags) {
	//	printk("%s-%d\n",__FUNCTION__,__LINE__);
		ops.mode = MTD_OOB_AUTO;
		ops.ooblen = packed_tags_size;
		ops.len = data ? dev->data_bytes_per_chunk : packed_tags_size;
	//	printk("%s-%d: ops.len:%x size:%x\n",__FUNCTION__,__LINE__, ops.len,packed_tags_size);

	
		ops.ooboffs = 0;
		ops.datbuf = data;
		ops.oobbuf = yaffs_dev_to_lc(dev)->spare_buffer;
		retval = mtd->read_oob(mtd, addr, &ops);
	}
#else
	if (!dev->param.inband_tags && data && tags) {

		retval = mtd->read_ecc(mtd, addr, dev->data_bytes_per_chunk,
				       &dummy, data, dev->spare_buffer, NULL);
	} else {
		if (data)
			retval =
			    mtd->read(mtd, addr, dev->data_bytes_per_chunk,
				      &dummy, data);
		if (!dev->param.inband_tags && tags)
			retval =
			    mtd->read_oob(mtd, addr, mtd->oobsize, &dummy,
					  dev->spare_buffer);
	}
#endif

	if (dev->param.inband_tags) {
		//printk("%s-%d\n",__FUNCTION__,__LINE__);
		if (tags) {
			//printk("%s-%d\n",__FUNCTION__,__LINE__);
			struct yaffs_packed_tags2_tags_only *pt2tp;
			pt2tp =
				(struct yaffs_packed_tags2_tags_only *)
				&data[dev->data_bytes_per_chunk];
			yaffs_unpack_tags2_tags_only(tags, pt2tp);
		}
	} else {
		if (tags) {
			memcpy(packed_tags_ptr,
			       yaffs_dev_to_lc(dev)->spare_buffer,
			       packed_tags_size);
			yaffs_unpack_tags2(tags, &pt, !dev->param.no_tags_ecc);
		}
	}

	if (local_data)
		yaffs_release_temp_buffer(dev, data);

	if (tags && retval == -EBADMSG
	    && tags->ecc_result == YAFFS_ECC_RESULT_NO_ERROR) {
		tags->ecc_result = YAFFS_ECC_RESULT_UNFIXED;
		dev->n_ecc_unfixed++;
	}
	if (tags && retval == -EUCLEAN
	    && tags->ecc_result == YAFFS_ECC_RESULT_NO_ERROR) {
		tags->ecc_result = YAFFS_ECC_RESULT_FIXED;
		dev->n_ecc_fixed++;
	}
	if (retval == 0){
		//printk("%s-%d\n",__FUNCTION__,__LINE__);
		return YAFFS_OK;
	}
	else{
		//printk("%s-%d\n",__FUNCTION__,__LINE__);
		return YAFFS_FAIL;
	}
}

int nandmtd2_mark_block_bad(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	int retval;
	yaffs_trace(YAFFS_TRACE_MTD,
		"nandmtd2_mark_block_bad %d",
		block_no);

	retval =
	    mtd->block_markbad(mtd,
			       block_no * dev->param.chunks_per_block *
			       dev->param.total_bytes_per_chunk);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;

}

int nandmtd2_query_block(struct yaffs_dev *dev, int block_no,
			 enum yaffs_block_state *state, u32 *seq_number)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(dev);
	int retval;

	yaffs_trace(YAFFS_TRACE_MTD, "nandmtd2_query_block %d", block_no);
	retval =
	    mtd->block_isbad(mtd,
			     block_no * dev->param.chunks_per_block *
			     dev->param.total_bytes_per_chunk);

	if (retval) {
		yaffs_trace(YAFFS_TRACE_MTD, "block is bad");

		*state = YAFFS_BLOCK_STATE_DEAD;
		*seq_number = 0;
	} else {
		struct yaffs_ext_tags t;
		nandmtd2_read_chunk_tags(dev, block_no *
					 dev->param.chunks_per_block, NULL, &t);

		if (t.chunk_used) {
			*seq_number = t.seq_number;
			*state = YAFFS_BLOCK_STATE_NEEDS_SCAN;
		} else {
			*seq_number = 0;
			*state = YAFFS_BLOCK_STATE_EMPTY;
		}
	}
	yaffs_trace(YAFFS_TRACE_MTD,
		"block is bad seq %d state %d",
		*seq_number, *state);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

