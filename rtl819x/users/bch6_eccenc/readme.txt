usage: 
page size 2k, bbi_dma_offset  2000 ,  bbi_swap_offset   23

prepare 3 file:   boot linux.bin squashfs.o
./eccenc --chunk-size 2048 --chunk-per-block 128 -bso 23 -bdo 2000 boot 0 linux.bin 3145728 squashfs.o 9437184

 will create burn.bin.ecc file





