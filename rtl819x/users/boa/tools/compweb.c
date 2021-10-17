#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "apmib.h"

#define MAXFNAME	256

#undef WEB_PAGE_OFFSET

#ifdef CONFIG_RTL_FLASH_MAPPING_ENABLE
#ifdef CONFIG_RTL_WEB_PAGES_OFFSET
#define WEB_PAGE_OFFSET CONFIG_RTL_WEB_PAGES_OFFSET
#else
#define WEB_PAGE_OFFSET 0x10000
#endif
#else
#define WEB_PAGE_OFFSET 0x10000
#endif

#ifdef CONFIG_MTD_NAND
#undef CODE_IMAGE_OFFSET
#undef WEB_PAGE_OFFSET

#define RTK_NAND_REVERSE_SIZE	0x800000
#define WEB_PAGE_OFFSET 	(CONFIG_RTL_WEB_PAGES_OFFSET + RTK_NAND_REVERSE_SIZE)
#define CODE_IMAGE_OFFSET 	(CONFIG_RTL_CODE_IMAGE_OFFSET+ RTK_NAND_REVERSE_SIZE)
#endif

#define DWORD_SWAP(v) ( (((v&0xff)<<24)&0xff000000) | ((((v>>8)&0xff)<<16)&0xff0000) | \
				((((v>>16)&0xff)<<8)&0xff00) | (((v>>24)&0xff)&0xff) )
#define WORD_SWAP(v) ((unsigned short)(((v>>8)&0xff) | ((v<<8)&0xff00)))
#define __PACK__	__attribute__ ((packed))

/*
typedef struct _file_entry {
	char name[MAXFNAME];
	unsigned long size;
} file_entry_T;
typedef struct _header {
	unsigned char signature[4];
	unsigned long addr;
	unsigned long burn_addr;
	unsigned long len;
} HEADER_T, *HEADER_Tp;
*/

/////////////////////////////////////////////////////////////////////////////
static int compress(char *inFile, char *outFile)
{
	char tmpBuf[100];
	sprintf(tmpBuf, "chmod 666 %s", inFile);
	system(tmpBuf);
	sprintf(tmpBuf, "bzip2 -9 -c %s > %s", inFile, outFile);
	system(tmpBuf);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
static unsigned char CHECKSUM(unsigned char *data, int len)
{
	int i;
	unsigned char sum=0;

	for (i=0; i<len; i++)
		sum += data[i];

	sum = ~sum + 1;
	return sum;
}
#endif

/////////////////////////////////////////////////////////////////////////////
static int lookfor_homepage_dir(FILE *fpList, char *dirpath, int is_for_web)
{
	char file[MAXFNAME];
	char *p;
	struct stat fst;

	fseek(fpList, 0L, SEEK_SET);
	dirpath[0] = '\0';

	while (fgets(file, sizeof(file), fpList) != NULL) {
		int is_break =0 ;
		p = file;

		while (*p) {
			if (*p == '\0') {
				is_break = 1;
				break;
			}

			if (*p == '\n' || *p == '\r') {
				*p = '\0';
				break;
			}
			else
				p++;
		}

		if (is_break) continue;
		if (stat(file, &fst) == 0 && fst.st_mode & S_IFDIR) {
			continue;
		}
		if (is_for_web) {
			p=strstr(file, "home.asp");
			if (p==NULL) 
				p=strstr(file, "home.htm");
		}
		else
			p=strrchr(file, '/');
		if (p) {

			*p = '\0';
			strcpy(dirpath, file);
// for debug
//printf("Found dir=%s\n", dirpath);
			return 0;
		}
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
static void strip_dirpath(char *file, char *dirpath)
{
	char *p, tmpBuf[MAXFNAME];

	if ((p=strstr(file, dirpath))) {
		strcpy(tmpBuf, &p[strlen(dirpath)]);
		strcpy(file, tmpBuf);
	}
// for debug
//printf("adding file %s\n", file);
}


/////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	FILE *fpList=NULL;
	char *outFile, *fileList;
	char *platform, *tag;
	struct stat fst;
	char buf[512], dirpath[100], file[MAXFNAME], tmpFile[100];
	FILE_ENTRY_T entry;
	unsigned char	*p;
	int i, len, fd=-1, fh=-1, filenum=0, is_web=1, pad=0;
	IMG_HEADER_T head;

	platform = argv[1];
	fileList = argv[2];
	outFile = argv[3];
	if ( argc > 4)
		is_web = 0;

	if(!strcmp(platform, "vpn"))
#if defined(CONFIG_RTL_8196B)
		tag = "w6bv";
#elif defined(CONFIG_RTL_8198C) || defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198B)
                tag = "w6cv";
#else		
		tag = "webv";
#endif
	else if(!strcmp(platform, "gw"))
#if defined(CONFIG_RTL_8196B)
		tag = "w6bg";
#elif defined(CONFIG_RTL_8198C) || defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198B)
                tag = "w6cg";
#else		
		tag = "webg";
#endif		
	else if(!strcmp(platform, "ap"))
#if defined(CONFIG_RTL_8196B)
		tag = "w6ba";
#elif defined(CONFIG_RTL_8198C) ||defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198B)
                tag = "w6ca";
#else
		tag = "weba";
#endif
	else if(!strcmp(platform, "cl"))
#if defined(CONFIG_RTL_8196B)
		tag = "w6bc";
#elif defined(CONFIG_RTL_8198C) ||defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198B)
                tag = "w6cc";
#else		
		tag = "webc";
#endif
	else{
		printf("unknow platform!\n");	
		return 0;
	}

	if(fh>0) { close(fh);fh=-1;}
	fh = open(outFile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("Create output file error %s!\n", outFile );
		goto EXIT_COMWEB;
	}
	lseek(fh, 0L, SEEK_SET);
	
	if(fpList) {fclose(fpList);fpList=NULL;}
	if ((fpList = fopen(fileList, "r")) == NULL) {
		printf("Can't open file list %s\n!", fileList);
		goto EXIT_COMWEB;
	}
	if (lookfor_homepage_dir(fpList, dirpath, is_web)<0) {
		printf("Can't find home.asp or home.htm page\n");
		goto EXIT_COMWEB;
	}

	fseek(fpList, 0L, SEEK_SET);
	while (fgets(file, sizeof(file), fpList) != NULL) {
		int is_break =0 ;
		p = file;

		while (*p) {
			if (*p == '\0') {
				is_break = 1;
				break;
			}

			if (*p == '\n' || *p == '\r') {
				*p = '\0';
				break;
			}
			else
				p++;
		}

		if (is_break) continue;
		if (stat(file, &fst) == 0 && fst.st_mode & S_IFDIR) {
			continue;
		}

		if(fd>0) {close(fd);fd=-1;}
		if ((fd = open(file, O_RDONLY)) < 0) {
			printf("Can't open file %s\n", file);
			goto EXIT_COMWEB;
		}
		lseek(fd, 0L, SEEK_SET);

		strip_dirpath(file, dirpath);

		strcpy(entry.name, file);
		entry.size = DWORD_SWAP(fst.st_size);

		if ( write(fh, (const void *)&entry, sizeof(entry))!=sizeof(entry) ) {
			printf("Write file failed!\n");
			goto EXIT_COMWEB;
		}

		i = 0;
		while ((len = read(fd, buf, sizeof(buf))) > 0) {
			if ( write(fh, (const void *)buf, len)!=len ) {
				printf("Write file failed!\n");
				goto EXIT_COMWEB;
			}
			i += len;
		}
		close(fd);
		fd=-1;
		if ( i != fst.st_size ) {
			printf("Size mismatch in file %s!\n", file );
		}

		filenum++;
	
	
	}

	fclose(fpList);
	fpList=NULL;
	close(fh);
	fh=-1;
	sync();

// for debug -------------
#if 0
sprintf(tmpFile, "cp %s web.lst -f", outFile);
system(tmpFile);
#endif
//-------------------------

	sprintf(tmpFile, "%sXXXXXX",  outFile);
	mkstemp(tmpFile);

	if ( compress(outFile, tmpFile) < 0) {
		printf("compress file error!\n");
		goto EXIT_COMWEB;
	}

	// append header
	if (stat(tmpFile, &fst) != 0) {
		printf("Create file error!\n");
		goto EXIT_COMWEB;
	}
	if((fst.st_size+1)%2)
		pad = 1;
	p = malloc(fst.st_size + 1 + pad);
	memset(p, 0 , fst.st_size + 1);
	if ( p == NULL ) {
		printf("allocate buffer failed!\n");
		goto EXIT_COMWEB;
	}

	memcpy(head.signature, tag, 4);
	head.len = fst.st_size + 1 + pad;
	printf("%s:%d len=%d leftLen=%d\n",__FILE__,__LINE__,head.len+sizeof(IMG_HEADER_T),CODE_IMAGE_OFFSET-WEB_PAGE_OFFSET);
	if(CODE_IMAGE_OFFSET-WEB_PAGE_OFFSET<head.len+sizeof(IMG_HEADER_T))
	{
		printf("webpage over size!!!!!Please remove some unused html files!\n");
		free(p);
		unlink(tmpFile);		
		return -1;
	}
	head.len = DWORD_SWAP(head.len);
	head.startAddr = DWORD_SWAP(WEB_PAGE_OFFSET);
	head.burnAddr = DWORD_SWAP(WEB_PAGE_OFFSET);

	if(fd>0) {close(fd);fd=-1;}
	if ((fd = open(tmpFile, O_RDONLY)) < 0) {
		printf("Can't open file %s\n", tmpFile);
		goto EXIT_COMWEB;
	}
	lseek(fd, 0L, SEEK_SET);
	if ( read(fd, p, fst.st_size) != fst.st_size ) {
		printf("read file error!\n");
		goto EXIT_COMWEB;
	}
	close(fd);
	fd=-1;

	p[fst.st_size + pad] = CHECKSUM(p, (fst.st_size+pad));

	if(fh>0) {close(fh);fh=-1;}
	fh = open(outFile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("Create output file error %s!\n", outFile );
		goto EXIT_COMWEB;
	}

	if ( write(fh, &head, sizeof(head)) != sizeof(head)) {
		printf("write header failed!\n");
		goto EXIT_COMWEB;
	}

	if ( write(fh, p, (fst.st_size+1+pad) ) != (fst.st_size+1+pad)) {
		printf("write data failed!\n");
		goto EXIT_COMWEB;
	}

	if(fh>0) close(fh);
	if(fd>0) close(fd);
	if(fpList!=NULL) fclose(fpList);
	chmod(outFile,  DEFFILEMODE);

	sync();

	free(p);
	unlink(tmpFile);

	return 0;
EXIT_COMWEB:
	if(fh>0) close(fh);
	if(fd>0) close(fd);
	if(fpList!=NULL) fclose(fpList);
	exit(1);
}
