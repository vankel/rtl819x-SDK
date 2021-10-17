#
# Realtek Semiconductor Corp.
#
# Tony Wu (tonywu@realtek.com)
#
#

export TESTDIR=/tmp

echo "INFO: executing memory tests ..."
#
# mem tests
#
./mem01
./mem02

echo "running malloc stress tests ..."
./mallocstress -l 128 -t 8

./page01 8192 8
./page02 8192 8

./mtest01 -c 1024 -b 1024 -p 50 -w

#
# mmap tests
#
./mmap1 -x 0.1
./mmap3 -x 0.1 -n 4
./mmapstress01 -p 4 -t 1
./mmapstress02
./mmapstress03
./mmapstress04 test 4096
./mmapstress05
./mmapstress06 10
./mmapstress07 test 8192 1 8192
./mmapstress09 -p 4 -t 1
./mmapstress10 -p 4 -t 1

./mmstress -t 0.2 -p 1024

#
# vmtests
#
./data_space
./stack_space


#
# shm test
#
./shmt02
./shmt03
./shmt04
./shmt05
./shmt06
./shmt07
./shmt08
./shmt09
./shmt10

./shm_test -l 10 -t 8
