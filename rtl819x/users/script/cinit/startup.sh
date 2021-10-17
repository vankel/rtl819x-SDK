#!/bin/sh
#
# script file to startup
TOOL=flash
GETMIB="$TOOL get"
LOADDEF="$TOOL default"
LOADDEFSW="$TOOL default-sw"
LOADDS="$TOOL reset"
# See if flash data is valid
$TOOL test-hwconf
if [ $? != 0 ]; then
	echo 'HW configuration invalid, reset default!'
	$LOADDEF
fi

$TOOL test-dsconf
if [ $? != 0 ]; then
	echo 'Default configuration invalid, reset default!'
	$LOADDEFSW
fi

$TOOL test-csconf
if [ $? != 0 ]; then
	echo 'Current configuration invalid, reset to default configuration!'
	$LOADDS
fi

# voip flash check
if [ "$VOIP_SUPPORT" != "" ]; then
$TOOL voip check
fi

if [ ! -e "$SET_TIME" ]; then
	flash settime
fi

# Generate WPS PIN number
eval `$GETMIB HW_WSC_PIN`
if [ "$HW_WSC_PIN" = "" ]; then
	$TOOL gen-pin
fi

