SRCS = testcase.c
OBJS = $(SRCS:.c=.o)

TEST_CASE= testcase
TEST_CASE_SRC=testcase.c
TEST_CASE_OBJ=$(TEST_CASE_SRC:.c=.o)
INC=-I../../CUnit-2.1-2/CUnit/Headers/ 
LIB=-L../../CUnit-2.1-2/CUnit/Sources/.libs/

LIB += -L../../boa/apmib/
LIB += -L../rtk_api/
all: $(TEST_CASE)

$(TEST_CASE): $(TEST_CASE_OBJ)
	$(CC)  $(LIB) -o $@ $(TEST_CASE_OBJ) -lcunit -lapmib -lrtk_adapter

.c.o:
	$(CC) $(COMMON_CFLAGS) $(INC) -c -o $@ -fpic $<	

clean:
	@rm -f $(OBJS) 
	@rm -f $(TEST_CASE) $(TEST_CASE_OBJ)

romfs:
	$(ROMFSINST) /bin/$(TEST_CASE)


