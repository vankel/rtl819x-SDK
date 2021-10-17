TARGET = eccenc
OBJS = main.o bch6.o ecc.o

CXX = g++
CFLAGS = -Wall -O0 -g 
#CFLAGS = -Wall -O3 
LDFLAGS = -L/usr/lib -lm

ALL_CPP = $(patsubst %.o,%.cpp,$(OBJS))
ALL_DEP = $(patsubst %.o,%.d,$(OBJS))

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)

$(OBJS) : %.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

ifneq ($(MAKECMDGOALS),clean)
include $(ALL_DEP)
endif

$(ALL_DEP) : %.d : %.cpp
	$(CXX) $(CFLAGS) -MM $< > $@

.PHONY: clean clean_dep
clean :
	-rm -rf $(TARGET) $(OBJS)

clean_dep :
	-rm -f $(ALL_DEP)

