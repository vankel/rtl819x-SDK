#!/bin/bash
# Program:
# 	Used for patch irq file.
# History:
# 2012/05/08
PATCH_DIR="irq"
PATCH_VERSION=$( patch -v 2>/dev/null | grep patch )
PATCH_RESULT=
IRQ_DIR="bsp_rtl8686"
PATCH_FILE="irq_patch irq_patch1"
PATCH_ORG="../../../../arch/rlx/"
CUR_DIR=$PWD
TEST_RESULT=

line(){
	echo "############################################";
}

changline(){
	echo ""
}

patch_version_check()
{
	
	if [ "$PATCH_VERSION" == "" ]; then
		line;
		echo "	Install patch first!";
		line;
		sudo apt-get install patch
	else
		line;
		echo "	Your version is $PATCH_VERSION!";
		line;
	fi	

}

restore(){
	echo "start to restore..."
        patch -RE -p0 < $file
        rm $file
	echo "end of restore!!!"
}


do_patch(){
	echo "start to patch..."
        patch -p0 < $file
        rm $file
	echo "end of patch!!!"
}


test_check() {
	cp -af $PATCH_DIR/$file  $PATCH_ORG
        cd  $PATCH_ORG
	PATCH_RESULT=$(patch -p0 --dry-run < $file | grep "rej" )
	if [ "$PATCH_RESULT" != "" ] ; then
		#echo "Error Msg: $PATCH_RESULT"
		TEST_RESULT=0;
	else
		TEST_RESULT=1;
	fi
	echo "Test patch file $file, result $TEST_RESULT"
}


restore_test_check() {
        cp -af $PATCH_DIR/$file  $PATCH_ORG
        cd  $PATCH_ORG
        PATCH_RESULT=$(patch -RE -p0 --dry-run < $file | grep "rej" )
        if [ "$PATCH_RESULT" != "" ] ; then
                #echo "Error Msg: $PATCH_RESULT"
                TEST_RESULT=0;
        else
                TEST_RESULT=1;
        fi
	echo "Test restore file $file, result $TEST_RESULT"
}


####################################################
#	Script boday
####################################################
patch_version_check;

case $1 in
	"restore")
		for file in $PATCH_FILE
		do
			restore_test_check;
			changline;
			if [ $TEST_RESULT == "1" ] ; then
				restore;
			fi
			cd $CUR_DIR
			changline;
		done
	;;
	*)
		for file in $PATCH_FILE
		do
                	test_check;
			changline;
			if [ $TEST_RESULT == "1" ] ; then
				do_patch;
			fi
			cd $CUR_DIR
			changline;
		done
	;;
esac
