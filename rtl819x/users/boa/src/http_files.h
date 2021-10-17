
/*
 *	File information structure.
 */
typedef struct _path_entry_{
	char name[256]; 						/*dir/file name*/
#ifdef ENABLE_LFS
	off64_t	size;					/* File length */
#else
	unsigned long	size;					/* File length */
#endif
	unsigned int		isDir;					/* Set if directory */
	time_t			mtime;					/* Modified time */
	unsigned int type;/*sort type, name or time, or size*/
	unsigned int order; /*sort order*/
}__PACK__ PATH_ENTRY_T, *PATH_ENTRY_Tp; 

#define MAX_PATH_ENTRY 1000
extern PATH_ENTRY_T *FileEntryHead;
extern PATH_ENTRY_T *DirEntryHead;
extern int CurrDIRCount;
extern int CurrFILECount;
extern char upload_st[512];
void GetMagicKey(char *Key);
int getParentDirectory(char *dest_path, char *src_path);

void create_directory_list(char* Entrypath, int type, int order);
#if defined(ENABLE_LFS)
void get_LastModified_Size(char *dest_time,  char *Size, time_t *entryTime, off64_t Length, int isdir);
#else
void get_LastModified_Size(char *dest_time,  char *Size, time_t *entryTime, unsigned long Length, int isdir);
#endif

int remove_file(const char *path);
char * strcat_str(char * str, int * len, int * tmplen, const char * s2);
void create_directory_list(char* Entrypath, int type, int order);

void free_directory_list(void);
int rm_url_escape_char(char *src_str, char *dest_str);
int rm_url_escape_char2(char *src_str);
char* get_filenPath(char *path);

void get_ParentDirectory(char *dest_path, char *src_path, int usage);
char* find_Last2F_boundary(char *data, int dlen, char *pattern, int plen, int *result);
char* last_char_is(const char *s, int c);


