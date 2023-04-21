crypt_and_hash.exe:crypt_and_hash.c
	gcc -o $@ $^ -I. -L. -lmbedcrypto -lmbedx509 -lmbedtls

.PHONY clean:
	rm -rf crypt_and_hash.exe
