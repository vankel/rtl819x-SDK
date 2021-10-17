//added by winfred_wang,param transmit used
#ifndef _PARAM_TAG_H_
#define _PARAM_TAG_H_

#define TAG_END				1
#define TAG_ADDR_START		0x80600000

struct tag_header {
	int size;
	int tag;
};

#ifdef CONFIG_NFJROM_RUN_IN_RAM_ENABLE	
#define TAG_FEATURE1_CMD			2
#define TAG_FEATURE1_INITRD			3
#define CL_SIZE						256

struct tag_feature1_cmd{
	char	cmd[CL_SIZE];	
};

struct tag_feature1_initrd{
	unsigned long	initrd_start;
	unsigned long	initrd_end;
};
#endif

struct param_tag{
	struct tag_header hdr;
	union parameter{
	#ifdef CONFIG_NFJROM_RUN_IN_RAM_ENABLE
	struct tag_feature1_cmd 	 	cmdline;
	struct tag_feature1_initrd  	initrd;
	#endif
		}param;
};
#define tag_next(t)	((struct param_tag *)((u32 *)(t) + (t)->hdr.size))

#endif

