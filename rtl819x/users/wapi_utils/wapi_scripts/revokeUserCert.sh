#!/bin/sh
SERIAL=$1
WAPIDIR=/var/myca
USER_CERT_DIR=$WAPIDIR/certs
OPENSSL_CONF=/usr/local/ssl/openssl.cnf

if [ "$SERIAL" = "" ]; then
	echo "usage: revokeUserCert.sh <serial> [option]"
	echo "description: serial must be a string of hex integer"
	echo "option: if use it, do not update ca crl and user certs into flash"
	exit 1
fi

USER_CERT=$SERIAL.cert

#echo "Begin to revoke $USER_CERT user cert ... "

#To revoke user cert of which user cert name is <serial>.cert
openssl ca -keyfile $WAPIDIR/CA.key -cert $WAPIDIR/CA.cert -revoke $USER_CERT_DIR/$USER_CERT

#To remove bak of user cert which is revoked
rm -f $USER_CERT_DIR/$USER_CERT 2>/dev/null

#echo "Revoke user cert: $USER_CERT OK!"

if [ "$2" != 'option' ]; then
#	echo "update ca crl and user certs into flash"
	#To update ca crl
	openssl ca -gencrl -config $OPENSSL_CONF -crldays 365 -crlexts crl_ext -out $WAPIDIR/CA.crl

	#Update ca crl into flash
	storeWapiFiles -caCrl

	#Update all user certs into flash
	storeWapiFiles -allUser
##else
#	echo "Not update ca crl and user certs into flash"
fi

