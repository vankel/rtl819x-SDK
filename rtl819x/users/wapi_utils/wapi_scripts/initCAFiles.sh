#!/bin/sh
CANAME=asu
WAPIDIR=/var/myca
OPENSSL_CONF=/usr/local/ssl/openssl.cnf

#patch for wapi
#because "openssl ecparam -genkey ... " may generate wrong private key sometimes
NORM_PRIV_KEY_SIZE=195
#Retry times is $MAX_TRY_NUM-1
MAX_TRY_NUM=2
#end patch

#echo "Initial wapi area flash"
storeWapiFiles -reset

#echo "Begin to generate ca files ... "
#generate ca private key
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
#        echo "Size of private key file generated is ok."
        num=$MAX_TRY_NUM
    fi
done
#end patch

cp $WAPIDIR/priv.key $WAPIDIR/CA.key
rm $WAPIDIR/priv.key 

#generate CA self-signed cert
openssl req -new -x509 -days 3650 -key $WAPIDIR/CA.key -subj "/OU=realsil/CN=$CANAME" -out $WAPIDIR/CA.cert

#patch for wapi web UI
###################################
#cp -f $WAPIDIR/CA.cert /web/as.cer
#cp -f $WAPIDIR/CA.cert /web/ca.cer
###################################
#To generate ca public key from ca cert
openssl x509 -in $WAPIDIR/CA.cert -pubkey -noout > $WAPIDIR/CA_pub.key

#general CA crl
openssl ca -gencrl -config $OPENSSL_CONF -crldays 365 -crlexts crl_ext -out $WAPIDIR/CA.crl

#CA should not include private key
#cat $WAPIDIR/CA.key >> $WAPIDIR/CA.cert

cp -f $WAPIDIR/CA.cert /web/ca.cer
#echo "Generate ca private key, cert and crl OK!"

#echo "Begin to store ca cert, private key and crl ..."
#store ca cert into flash
storeWapiFiles -caCert

#store ca private key into flash
storeWapiFiles -caKey

#store ca crl into flash
storeWapiFiles -caCrl

#echo "Store ca cert, private key and crl OK!"

