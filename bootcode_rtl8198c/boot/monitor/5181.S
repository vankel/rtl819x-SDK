	.section .lx5181_binary,"a"
	.globl lx5181_binary_start
lx5181_binary_start:

	//.incbin "5181-uart0.bin"				//len=76 byte
	//.incbin "5181-uart1.bin"
	//.incbin "5181-uart0-show_5181_BOOT_OK.bin"	//len=204 byte
	//.incbin "5181_uart1_eCos_nfjrom" //111616 byte
	.incbin	"5181_uart0_eCos_nfjrom_mp"	//111616 byte

	.globl lx5181_binary_end
lx5181_binary_end:

