#include<stdio.h>
#include<string.h>
#include"mbedtls/aes.h"
#include"mbedtls/sha256.h"
#include"mbedtls/gcm.h"


int main(int argc, char *argv[])
{

	//unsigned char *key128="abcdef1234567890";
	unsigned char key128[16]={0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,
                                  0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68};

	//unsigned char *iv="3333333333333333";
	unsigned char iv[16]={0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
                             0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33};

	unsigned char input[16+16]={

                                 0x65,0x66,0x67,0x33,0x34,0x35,0x36,0x37,
                                 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,

				 0x51,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
				 0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F
	                        };

	unsigned char output[100];

	mbedtls_aes_context ctx;

	memset(output,0x00,100);

	mbedtls_aes_init(&ctx);

	if(mbedtls_aes_setkey_enc(&ctx,key128,128) == 0)
	{
		//mbedtls_aes_crypt_cbc(&ctx,MBEDTLS_AES_ENCRYPT,16+16,iv,input,output);
		mbedtls_aes_crypt_cbc(&ctx,MBEDTLS_AES_ENCRYPT,16,iv,input,output);

		for(int i=0;i<16;i++)
		{
			printf("output[i]= %x\n",output[i]);

		}
	}



	unsigned char* sha256_input = "hello world";
	unsigned char  sha256_output[32] = {0};

	mbedtls_sha256_context sha256_ctx;
	mbedtls_sha256_init(&sha256_ctx);

	mbedtls_sha256_starts(&sha256_ctx,0);
	mbedtls_sha256_update(&sha256_ctx,sha256_input,strlen(sha256_input));
	mbedtls_sha256_finish(&sha256_ctx,sha256_output);
	mbedtls_sha256_free(&sha256_ctx);

	for(int i=0;i<32;i++)
	{
		printf("hash[%d] = %02x\n",i,sha256_output[i]);

	}



//======================================
//aes128 gcm
//======================================
unsigned char gcm_output[128];
unsigned char tag_output[16];
mbedtls_gcm_context gcm_ctx;

mbedtls_gcm_init(&gcm_ctx);

memset(gcm_output,0x00,128);
memset(tag_output,0x00,16);

mbedtls_gcm_setkey(&gcm_ctx,MBEDTLS_CIPHER_ID_AES,key128,128);
mbedtls_gcm_crypt_and_tag(&gcm_ctx,MBEDTLS_GCM_ENCRYPT,16,iv,12,NULL,0,input,gcm_output,16,tag_output);
mbedtls_gcm_free(&gcm_ctx);

for(int i=0;i<16;i++)
{
	printf("gcm_out[%d]= %02x\n",i,gcm_output[i]);
}

for(int i=0;i<16;i++)
{
	printf("tag_out[%d]= %02x\n",i,tag_output[i]);
}


unsigned char xgcm_output[128];
unsigned char xtag_output[16];

memset(xgcm_output,0x00,128);
memset(xtag_output,0x00,16);

mbedtls_gcm_setkey(&gcm_ctx,MBEDTLS_CIPHER_ID_AES,key128,128);
mbedtls_gcm_crypt_and_tag(&gcm_ctx,MBEDTLS_GCM_DECRYPT,16,iv,12,NULL,0,gcm_output,xgcm_output,16,xtag_output);
mbedtls_gcm_free(&gcm_ctx);


for(int i=0;i<16;i++)
{
	printf("xgcm_out[%d]= %02x\n",i,xgcm_output[i]);
}

for(int i=0;i<16;i++)
{
	printf("xtag_out[%d]= %02x\n",i,xtag_output[i]);
}




	return 0;

}
