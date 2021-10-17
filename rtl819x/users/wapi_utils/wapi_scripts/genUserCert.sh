#!/bin/sh
USERNAME=$1
VALID_DAYS=$2
WAPIDIR=/var/myca
AUTO_CERT_BACKDIR=$WAPIDIR/newcerts

#patch for wapi
#because "openssl ecparam -genkey ... " may generate wrong private key sometimes
NORM_PRIV_KEY_SIZE=195
#Retry times is $MAX_TRY_NUM-1
MAX_TRY_NUM=2
#end patch

if [ "$USERNAME" = "" ] || [ "VALID_DAYS" = "" ]; then
	echo "usage: genUserCert.sh <username> <validDays>"
	exit 1
fi

#echo "Begin to generate user cert ... "
#echo "USERNAME=$USERNAME"

#To clear temporary files first
#To avoid this situation: 
# consume that: 1)input 1 -> output 2, 2)input 2 -> output 3
# if 1) run error, but temporary file 2 exist, then 2) still can run.
rm -f $WAPIDIR/priv.key 2>/dev/null
rm -f $WAPIDIR/user.key 2>/dev/null
rm -f $WAPIDIR/user.csr 2>/dev/null
rm -f $WAPIDIR/user.cert 2>/dev/null

#generate user private key
openssl ecparam -genkey -name prime192v1 -out $WAPIDIR/priv.key -noout

#patch for wapi
#because "openssl ecparam -genkey ... " may generate wrong private key sometimes
num=1
while [ $num -lt $MAX_TRY_NUM ];
do 
#    echo "num=$num"
    readFileSize -in $WAPIDIR/priv.key -out $WAPIDIR/privKeySize
    PRIV_KEY_SIZE=`cat $WAPIDIR/privKeySize`
#    echo "priv_key_size=$PRIV_KEY_SIZE"

    if [ "$PRIV_KEY_SIZE" != "$NORM_PRIV_KEY_SIZE" ]; then
	echo "Warning: generate wrong private key. Try again!"
	#Retry to generate user private key
	openssl ecparam -genkey -name prime192v1 -out $WAPIDIR/priv.key -noout
	num=`expr $num + 1`
    else
#	echo "Size of private key file generated is ok."
	num=$MAX_TRY_NUM
    fi
done
#end patch

cp $WAPIDIR/priv.key $WAPIDIR/user.key

#rm $WAPIDIR/priv.key

#generate user cert request
openssl req -new -key $WAPIDIR/user.key -subj "/OU=realsil/CN=$USERNAME" -out $WAPIDIR/user.csr

#bak current user cert serial
#cp $WAPIDIR/serial $WAPIDIR/serial.old

#generate user cert, during which serial automatically back into serial.old and update into the next one
#and openssl automatically back new certs at AUTO_CERT_BACKDIR/<serial>.pem
if [ -f "$WAPIDIR/user.csr" ]; then
	openssl ca -days $VALID_DAYS -batch -keyfile $WAPIDIR/CA.key -cert $WAPIDIR/CA.cert -in $WAPIDIR/user.csr -out $WAPIDIR/user.cert 2>/dev/null
else
	echo "Generate user cert error!"
	exit 1
fi

#To remove char format from user.cert, reserve PEM format only
openssl x509 -inform PEM -in $WAPIDIR/user.cert -outform PEM -out $WAPIDIR/tmp.cert
cp $WAPIDIR/tmp.cert $WAPIDIR/user.cert
rm -f $WAPIDIR/tmp.cert

#Add user private key into user.cert
cat $WAPIDIR/user.key >> $WAPIDIR/user.cert

#backup current user.cert into certs/<serial.old>.cert
SERIAL=`cat $WAPIDIR/serial.old`
cp $WAPIDIR/user.cert $WAPIDIR/certs/$SERIAL.cert

#To remove openssl auto backup usercert
rm -f $AUTO_CERT_BACKDIR/$SERIAL.pem

#echo "Generate user cert OK!"

#Store new generate user cert into flash
storeWapiFiles -oneUser

