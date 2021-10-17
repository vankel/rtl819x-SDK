
	#define UART_BASE         0xB8000000	
	#define UART_RBR	(0x2000+UART_BASE)
	#define	UART_LSR	(0x2014+UART_BASE)
#define REG32(reg)	(*(volatile unsigned int *)(reg))



unsigned int Check_UartRxDataRdy()
{
	if 	(REG32(UART_LSR) & (1<<24) )
		return 1;
	else
		return 0;
}

unsigned char Get_UartData()
{	return REG32(UART_RBR)>>24;
}

int Get_UartData_timeout(unsigned char *c, unsigned int  timeout)
{	
	unsigned int t=timeout;

	while(t--)
	{
		if(Check_UartRxDataRdy())
		{	*c=Get_UartData();
			return 1;
		}

	}
	return 0;
}

#define putc(x)	serial_outc(x)
#define getc_with_timeout  Get_UartData_timeout


#define true	1
#define false	0

//xmodem protocol header
#define SOH				0x01	/* start of header */
#define STX 			       0x02	/* start of header */
#define EOT				0x04	/* end of text */
#define ACK				0x06	/* acknowledge */
#define NAK				0x15	/* negative acknowledge */
#define CAN				0x18	/* cancel */
#define CRC				0x43	/* crc acknowledge */

#define RETRY			(20)

enum xmodem_state {
	XMODEM_RX_READY,
	XMODEM_WAIT_HEADER,
	XMODEM_RX_PACKET,
	XMODEM_CANCEL,
	XMODEM_EOT
};




int xmodem_receive(unsigned char  * buf)
{
	enum xmodem_state state = XMODEM_RX_READY;
	unsigned char  c;
	unsigned int retry = RETRY;
	unsigned int timeout=100000;
	unsigned int i;
	
	unsigned int packet_size = 128;
	unsigned char  block_index = 1;

	unsigned char head1,head2,head_csum;
	unsigned char csum;
	unsigned char *ptr=buf;
#if 0	
	if(!buf )
		return false;
#endif	

	
	/* received buffer size to zero */
	int size = 0;
	
	while(retry > 0)
	{
		switch(state)
		{
			case XMODEM_RX_READY:
				
				putc(NAK);			
				if(getc_with_timeout(&c, timeout))
				{
					if(c == SOH)
					{
						packet_size = 128;						
						state = XMODEM_RX_PACKET;
					}			
					else if(c == 3)  //Ctrl+C
					{
						putc(CAN);	putc(CAN);
						putc(CAN);	putc(CAN);
						size = 0;
						return false;
					}
				}	
				break;
				
			case XMODEM_WAIT_HEADER:
				if(getc_with_timeout(&c, timeout))
				{
					if(c == SOH)
					{
						packet_size = 128;						
						state = XMODEM_RX_PACKET;
					}
				
					else if(c == CAN)
					{	state = XMODEM_CANCEL;
					}
					else if(c == EOT)
					{	state = XMODEM_EOT;
					}
				}
				else
				{
					/* timed out, try again */
					retry--;
				}
				break;
				
			case XMODEM_RX_PACKET:
				if(getc_with_timeout(&c, timeout))
				{	head1=c;
				}
				else goto fail;
				
				if(getc_with_timeout(&c, timeout))
				{	head2=c;				
				}
				else goto fail;

				csum=0;
				for(i = 0; i < packet_size  ; i++)
				{
					if(getc_with_timeout(&c, timeout))
					{	ptr[i] = c;
						csum+=c;

					}
					else
						goto fail;
				}

				if(getc_with_timeout(&c, timeout))
				{	head_csum=c;	
				}
				else goto fail;
		

				
				state = XMODEM_WAIT_HEADER;
				
				/* packet was too small, retry */
				if(i < (packet_size))
				{
fail:				
					retry--;
					putc(NAK);
					continue;
				}
				
				/* check validity of packet */
				if( (head1 == 255-head2) && (head_csum==csum) )
				{
					/* is this the packet we were waiting for? */
					if(head1== block_index)
					{
						ptr+=packet_size;
						size += packet_size;						
						block_index++;
						retry = RETRY;
						putc(ACK);
						continue;
					}
					else if(head1 == (unsigned char )(block_index-1))
					{
						/* this is a retransmission of the last packet */
						putc(ACK);
						 continue;
					}
				}
				
				retry--;
				putc(NAK);
				break;
				
			case XMODEM_CANCEL:
				putc(ACK);
				
				return false;
				
			case XMODEM_EOT:
				putc(ACK);
				//mdelay(100);				
				return size;
							
			default:
				break;
		}
	}
	
	/* retry too much, fail */
	putc(CAN);	putc(CAN);
	putc(CAN);	putc(CAN);
	
	
	return false;
}




