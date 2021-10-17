
CFLAGES = -D_FILE_OFFSET_BITS=64 -std=c99 -D_GNU_SOURCE

FUSE_INCLUDE = ../fuse-2.9.0/include/
FUSE_DIR = ../fuse-2.9.0/lib/.libs/
FUSE_LIB = fuse

LIBS = -ldl -lpthread

TARGET = fuse/mount.exfat-fuse
LIB_EXFAT = libexfat/libexfat.a

MAIN_SRC = fuse/main.c
LIB_SRC = libexfat/cluster.c libexfat/io.c libexfat/log.c libexfat/lookup.c libexfat/mount.c libexfat/node.c libexfat/time.c libexfat/utf.c libexfat/utils.c

MAIN_OBJ = $(MAIN_SRC:.c=.o)
LIB_OBJ = $(LIB_SRC:.c=.o)

all: $(TARGET)

$(LIB_EXFAT): $(LIB_OBJ)
	$(AR) rcs $(LIB_EXFAT) $(LIB_OBJ)

$(TARGET): $(MAIN_OBJ) $(LIB_EXFAT)
	$(CC) -o $@ $(MAIN_OBJ) -Llibexfat -lexfat -L$(FUSE_DIR) -l$(FUSE_LIB) $(LIBS) 

.c.o:
	$(CC) -o $@ -c -Wall -02 -ggdb $(CFLAGES) -Ilibexfat -I$(FUSE_INCLUDE) $<

clean:
	@rm -rf $(TARGET) $(LIB_EXFAT) $(MAIN_OBJ) $(LIB_OBJ)

romfs:
	$(ROMFSINST) fuse/mount.exfat-fuse /bin/mount.exfat-fuse
	$(ROMFSINST) -s /bin/mount.exfat-fuse /bin/mount.exfat
