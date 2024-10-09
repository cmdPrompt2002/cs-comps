/* OpenSSH capabilities */

#define OPENSSH_VERSION_MAJOR 9
#define OPENSSH_VERSION_MINOR 7

#define OPENSSH_CIPHERS "3des-cbc,aes128-cbc,aes192-cbc,aes256-cbc,aes128-ctr,aes192-ctr,aes256-ctr,aes128-gcm@openssh.com,aes256-gcm@openssh.com,chacha20-poly1305@openssh.com"
#define OPENSSH_MACS "hmac-sha1,hmac-sha1-96,hmac-sha2-256,hmac-sha2-512,hmac-md5,hmac-md5-96,umac-64@openssh.com,umac-128@openssh.com,hmac-sha1-etm@openssh.com,hmac-sha1-96-etm@openssh.com,hmac-sha2-256-etm@openssh.com,hmac-sha2-512-etm@openssh.com,hmac-md5-etm@openssh.com,hmac-md5-96-etm@openssh.com,umac-64-etm@openssh.com,umac-128-etm@openssh.com"
#define OPENSSH_KEX "diffie-hellman-group1-sha1,diffie-hellman-group14-sha1,diffie-hellman-group14-sha256,diffie-hellman-group16-sha512,diffie-hellman-group18-sha512,diffie-hellman-group-exchange-sha1,diffie-hellman-group-exchange-sha256,ecdh-sha2-nistp256,ecdh-sha2-nistp384,ecdh-sha2-nistp521,curve25519-sha256,curve25519-sha256@libssh.org,sntrup761x25519-sha512@openssh.com"
#define OPENSSH_KEYS "ssh-ed25519,ssh-ed25519-cert-v01@openssh.com,sk-ssh-ed25519@openssh.com,sk-ssh-ed25519-cert-v01@openssh.com,ecdsa-sha2-nistp256,ecdsa-sha2-nistp256-cert-v01@openssh.com,ecdsa-sha2-nistp384,ecdsa-sha2-nistp384-cert-v01@openssh.com,ecdsa-sha2-nistp521,ecdsa-sha2-nistp521-cert-v01@openssh.com,sk-ecdsa-sha2-nistp256@openssh.com,sk-ecdsa-sha2-nistp256-cert-v01@openssh.com,ssh-dss,ssh-dss-cert-v01@openssh.com,ssh-rsa,ssh-rsa-cert-v01@openssh.com,webauthn-sk-ecdsa-sha2-nistp256@openssh.com,rsa-sha2-256,rsa-sha2-512"


#define OPENSSH_3DES_CBC 1
#define OPENSSH_AES128_CBC 1
#define OPENSSH_AES192_CBC 1
#define OPENSSH_AES256_CBC 1
/* #undef OPENSSH_RIJNDAEL_CBC_LYSATOR_LIU_SE */
#define OPENSSH_AES128_CTR 1
#define OPENSSH_AES192_CTR 1
#define OPENSSH_AES256_CTR 1
#define OPENSSH_AES128_GCM_OPENSSH_COM 1
#define OPENSSH_AES256_GCM_OPENSSH_COM 1
#define OPENSSH_CHACHA20_POLY1305_OPENSSH_COM 1
/* #undef OPENSSH_BLOWFISH_CBC */
#define OPENSSH_HMAC_SHA1 1
#define OPENSSH_HMAC_SHA1_96 1
#define OPENSSH_HMAC_SHA2_256 1
#define OPENSSH_HMAC_SHA2_512 1
#define OPENSSH_HMAC_MD5 1
#define OPENSSH_HMAC_MD5_96 1
#define OPENSSH_UMAC_64_OPENSSH_COM 1
#define OPENSSH_UMAC_128_OPENSSH_COM 1
#define OPENSSH_HMAC_SHA1_ETM_OPENSSH_COM 1
#define OPENSSH_HMAC_SHA1_96_ETM_OPENSSH_COM 1
#define OPENSSH_HMAC_SHA2_256_ETM_OPENSSH_COM 1
#define OPENSSH_HMAC_SHA2_512_ETM_OPENSSH_COM 1
#define OPENSSH_HMAC_MD5_ETM_OPENSSH_COM 1
#define OPENSSH_HMAC_MD5_96_ETM_OPENSSH_COM 1
#define OPENSSH_UMAC_64_ETM_OPENSSH_COM 1
#define OPENSSH_UMAC_128_ETM_OPENSSH_COM 1
#define OPENSSH_DIFFIE_HELLMAN_GROUP1_SHA1 1
#define OPENSSH_DIFFIE_HELLMAN_GROUP14_SHA1 1
#define OPENSSH_DIFFIE_HELLMAN_GROUP14_SHA256 1
#define OPENSSH_DIFFIE_HELLMAN_GROUP16_SHA512 1
#define OPENSSH_DIFFIE_HELLMAN_GROUP18_SHA512 1
#define OPENSSH_DIFFIE_HELLMAN_GROUP_EXCHANGE_SHA1 1
#define OPENSSH_DIFFIE_HELLMAN_GROUP_EXCHANGE_SHA256 1
#define OPENSSH_ECDH_SHA2_NISTP256 1
#define OPENSSH_ECDH_SHA2_NISTP384 1
#define OPENSSH_ECDH_SHA2_NISTP521 1
#define OPENSSH_CURVE25519_SHA256 1
#define OPENSSH_CURVE25519_SHA256_LIBSSH_ORG 1
#define OPENSSH_SSH_ED25519 1
#define OPENSSH_SSH_ED25519_CERT_V01_OPENSSH_COM 1
#define OPENSSH_SSH_RSA 1
#define OPENSSH_ECDSA_SHA2_NISTP256 1
#define OPENSSH_ECDSA_SHA2_NISTP384 1
#define OPENSSH_ECDSA_SHA2_NISTP521 1
#define OPENSSH_SSH_RSA_CERT_V01_OPENSSH_COM 1
#define OPENSSH_ECDSA_SHA2_NISTP256_CERT_V01_OPENSSH_COM 1
#define OPENSSH_ECDSA_SHA2_NISTP384_CERT_V01_OPENSSH_COM 1
#define OPENSSH_ECDSA_SHA2_NISTP521_CERT_V01_OPENSSH_COM 1

/* Available programs */

/* #undef NCAT_EXECUTABLE */
#define SSHD_EXECUTABLE "/usr/sbin/sshd"
#define SSH_EXECUTABLE "/usr/bin/ssh"
/* #undef WITH_TIMEOUT */
/* #undef TIMEOUT_EXECUTABLE */
/* #undef SOFTHSM2_LIBRARY */
/* #undef P11_KIT_CLIENT */
/* #undef PKCS11SPY */
