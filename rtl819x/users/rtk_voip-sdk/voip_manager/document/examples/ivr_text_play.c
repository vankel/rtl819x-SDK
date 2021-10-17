#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "voip_manager.h"

// Test steps:
// 1. Phone off-hook
// 2. Press any DTMF key to stop the dial tone
// 3. Kill solar_monitor and solar before busy tone played
// 4. Run test

int main(int argc, char *argv[])
{
	int chid = 0, mid = 0, path = 0;

	if (argc < 4)
	{
		printf("usage: %s <chid> <sid> <path>\n", argv[0]);
		return -1;
	}

	chid = atoi(argv[1]);
	mid = atoi(argv[2]);
	path = atoi(argv[3]);

	char text[] = 
	{
		IVR_TEXT_ID_DHCP, 
		IVR_TEXT_ID_FIX_IP,
		IVR_TEXT_ID_PLZ_ENTER_NUMBER, 
		IVR_TEXT_ID_PLEASE_ENTER_PASSWORD,
		IVR_TEXT_ID_NO_RESOURCE,
		'\0'
	};

	printf("play IVR text!\n");

	if (path == 0)
		rtk_IvrStartPlaying(chid, mid, IVR_DIR_LOCAL, text);
	else
		rtk_IvrStartPlaying(chid, mid, IVR_DIR_REMOTE, text);

	while (1)
	{
		if (0 == rtk_IvrPollPlaying(chid, mid))
		{
			printf("End! Play again!\n");
			if (path == 0)
				rtk_IvrStartPlaying(chid, mid, IVR_DIR_LOCAL, text);
			else
				rtk_IvrStartPlaying(chid, mid, IVR_DIR_REMOTE, text);
		}
		usleep(100000);//100ms
	}

	return 0;
}
