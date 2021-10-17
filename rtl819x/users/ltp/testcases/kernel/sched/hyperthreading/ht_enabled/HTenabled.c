/***************************************************************************
                          HTenabled.c  -  description
                             -------------------
    email                : sonic,zhang@intel.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "test.h"
#include "HTutils.h"

char *TCID = "smt_smp_enabled";
int TST_TOTAL = 1;

int main(int argc, char *argv[])
{
	int ret_val = 1;
#ifndef __i386__
	tst_brkm(TCONF, NULL, "This test suite can only execute on i386 architecture.");
#else
		ret_val = check_ht_capability();
#endif
	return ret_val;
}
