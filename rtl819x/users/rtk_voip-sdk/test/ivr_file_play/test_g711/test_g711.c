#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

/* included in rtk_voip/include/ */
#if 1
#include "voip_manager.h"
#else
#include "type.h"
#include "voip_params.h"
#include "voip_control.h"
#endif

#include "voip_ioctl.h"

int rtk_SetIvrPlayG711( int chid, int sid, IvrPlayDir_t dir, unsigned int nCount, const unsigned char *pData )
{
	TstVoipPlayIVR_G711 stVoipPlayIVR;
	
	if( nCount > MAX_FRAMES_OF_G711 )
		nCount = MAX_FRAMES_OF_G711;
	
	stVoipPlayIVR.ch_id = chid;
	stVoipPlayIVR.m_id = sid;
	stVoipPlayIVR.type = IVR_PLAY_TYPE_G711A;
	stVoipPlayIVR.dir = dir;
	stVoipPlayIVR.nFramesCount = nCount;
	memcpy( stVoipPlayIVR.data, pData, nCount * 80 );

    SETSOCKOPT(VOIP_MGR_PLAY_IVR, &stVoipPlayIVR, TstVoipPlayIVR_G711, 1);
    
    printf( "IVR playing time: %u0 ms\n", stVoipPlayIVR.playing_period_10ms );
    printf( "\tCopied data:%d\n", stVoipPlayIVR.nRetCopiedFrames );
    
    return stVoipPlayIVR.nRetCopiedFrames;
}

int main( int argc, char **argv )
{
	int dir = 0;
	FILE *fp;
	unsigned char buffer[ 80 * 10 ];
	unsigned int cRead, cWritten, shift;
	int chid=0;
	int sid=0;
	
	if( ( fp = fopen( "711_raw", "rb" ) ) == NULL ) {
		printf( "Open error\n" );
		return 1;
	}

	if (argc >= 4) {
		chid = atoi(argv[1]);
		sid  = atoi(argv[2]);
		dir  = atoi(argv[3]);
	}

	printf("play G.711A to chid%d, mid%d, dir%d\n",chid,sid,dir);
	
	while( 1 ) {
		
		cRead = fread( buffer, 80, 10, fp );
		shift = 0;
		
		if( cRead == 0 )	/* seen as eof */
			break;

lable_put_g711_data:
		cWritten = rtk_SetIvrPlayG711( chid, sid, dir, cRead, buffer + shift );
		//printf( "Write:%d, %d\n", cRead, shift );
		
		/* buffer is full */
		if( cWritten < cRead ) {
			
			
			printf( "Buffer is full.. Wait one second...\n" );
			//printf( "[%d:%d:%d]\n", cRead, cWritten, shift );
			rtk_SetIvrPlayG711( chid, sid, dir, 0, buffer );	/* show current playing time */
			
			sleep( 1 );	
			
			cRead -= cWritten;
			shift += cWritten * 80;
			
			goto lable_put_g711_data;
		}
	}
	
	fclose( fp );
	
	return 0;
}
