#!/bin/sh

echo "test wapi cert ... "

echo "Gen ca cert ..."

#generate ca private key
openssl ecparam -genkey -name prime192v1 -out /var/myca/priv.key -noout

cp /var/myca/priv.key /var/myca/CA.key

openssl req -new -x509 -days 3650 -key /var/myca/CA.key -subj "/OU=realsil/CN=asu" -out /var/myca/CA.cert

#To generate ca public key from ca cert
openssl x509 -in /var/myca/CA.cert -pubkey -noout > /var/myca/CA_pub.key
 
#general CA crl
openssl ca -gencrl -config /usr/local/ssl/openssl.cnf -crldays 365 -crlexts crl_ext -out /var/myca/CA.crl

echo "Gen user cert ..."
#generate user private key
openssl ecparam -genkey -name prime192v1 -out /var/myca/priv.key -noout

cp /var/myca/priv.key /var/myca/user.key

openssl req -new -key /var/myca/user.key -subj "/OU=realsil/CN=test" -out /var/myca/user.csr

openssl ca -days 4 -batch -keyfile /var/myca/CA.key -cert /var/myca/CA.cert -in /var/myca/user.csr -out /var/myca/user.cert

openssl x509 -inform PEM -in /var/myca/user.cert -outform PEM -out /var/myca/user2.cert

