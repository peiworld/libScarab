/*
 *  test.c
 *  integer-fhe
 *
 *  Created by Henning Perl on 17.12.10.
 *
 */

#include "test.h"

#define ASSERT_HALFADD(__a,__b,__sum,__carry)		\
	fhe_halfadd(sum, carry, __a, __b, pk);			\
	assert(fhe_decrypt(sum, sk) == __sum);			\
	assert(fhe_decrypt(carry, sk) == __carry);

/* in FULLADD, __c is the carry in */
#define ASSERT_FULLADD(__a,__b,__c,__sum,__carry)	\
	fhe_fulladd(sum, carry, __a, __b, __c, pk);		\
	assert(fhe_decrypt(sum, sk) == __sum);			\
	assert(fhe_decrypt(carry, sk) == __carry);

#define ASSERT_HOMMUL(__a, __b, __check)			\
	fhe_mul(temp, __a, __b, pk);					\
	assert(fhe_decrypt(temp, sk) == __check);

#define ASSERT_HOMADD(__a, __b, __check)			\
	fhe_add(temp, __a, __b, pk);					\
	assert(fhe_decrypt(temp, sk) == __check);

/*
 * entire test case
 */
void
test_suite()
{
	test_fully_homomorphic();
	test_homomorphic();
	test_recrypt();
	test_encryt_decrypt();
	test_halfadd();
	test_fulladd();
}

void
test_encryt_decrypt()
{
	printf("ENCRYPT/DECRYPT\n");
	int m0, m1;
	mpz_t c0, c1;
	
	mpz_init(c0);
	mpz_init(c1);
	
	fhe_pk_t pk;
	fhe_sk_t sk;
	fhe_pk_init(pk);
	fhe_sk_init(sk);
	
	for (int i = 0; i < KEYRUNS; i++) {
		fhe_keygen(pk, sk);
		
		for (int j = 0; j < RUNS; j++) {
			// c0 is the cipher text
			fhe_encrypt(c0, pk, 0);
			m0 = fhe_decrypt(c0, sk);
			fhe_encrypt(c1, pk, 1);
			m1 = fhe_decrypt(c1, sk);
			
			assert(m0 == 0);
			assert(m1 == 1);
			printf(".");
			fflush(stdout);
		}
		printf("\n");
	}
	fhe_pk_clear(pk);
	fhe_sk_clear(sk);
	mpz_clear(c0);
	mpz_clear(c1);
	printf("PASSED.\n");
}

/*
 * Adding two bits together without carry-in
 */
void
test_halfadd()
{
	printf("HALFADD\n");
	mpz_t c0, c1;
	mpz_t sum, carry;
	
	mpz_init(c0);
	mpz_init(c1);
	mpz_init(sum);
	mpz_init(carry);
	
	fhe_pk_t pk;
	fhe_sk_t sk;
	fhe_pk_init(pk);
	fhe_sk_init(sk);
	
	for (int i = 0; i < KEYRUNS; i++) {
		fhe_keygen(pk, sk);
		
		fhe_encrypt(c0, pk, 0);
		fhe_encrypt(c1, pk, 1);
		
		// ASSERT_HALFADD(__a,__b,__sum,__carry)
		ASSERT_HALFADD(c0,c0,0,0);
		ASSERT_HALFADD(c1,c0,1,0);
		ASSERT_HALFADD(c0,c1,1,0);
		ASSERT_HALFADD(c1,c1,0,1);
		printf(".");
		fflush(stdout);
	}
	fhe_pk_clear(pk);
	fhe_sk_clear(sk);
	mpz_clear(sum);
	mpz_clear(carry);
	mpz_clear(c0);
	mpz_clear(c1);
	printf(" PASSED.\n");
}

/*
 * adding two bits with carry-in, which is just like the third bit
 */
void
test_fulladd()
{
	printf("FULLADD\n");
	/* define integer variables, c0, c1 are the cipher texts for the integers */
	mpz_t c0, c1;
	mpz_t sum, carry;
	
	/* initialize integer variable */
	mpz_init(c0);
	mpz_init(c1);
	mpz_init(sum);
	mpz_init(carry);
	
	/* define and initialize key pair variables */
	fhe_pk_t pk;
	fhe_sk_t sk;

	

	for (int i = 0; i < KEYRUNS; i++) {
		fhe_pk_init(pk);
		fhe_sk_init(sk);
		/* generate a key pair */
		fhe_keygen(pk, sk);
		printf("Key-%d pk: %Zd; sk:%Zd\n",i, pk, sk);
		/*
		 * Issue: the pk and sk are the same for every round, which means the encrypt
		 * is done using the same key pair for all key runs
		 */

		
		/* encrypt integer 0 or 1 using public key pk and store data in c0 or c1 */
		fhe_encrypt(c0, pk, 0);
		fhe_encrypt(c1, pk, 1);
		printf("c0: %Zd; c1: %Zd\n", c0, c1);
		
		// Ref about full adder:
		// http://computer.howstuffworks.com/boolean3.htm
		// ASSERT_FULLADD(__a,__b,__c,__sum,__carry)
		/* __c is carry in, which is the same as another input */
										/* A  B  CI  Sum  CO */
		ASSERT_FULLADD(c0,c0,c0,0,0);   /* 0  0  0    0   0  */
		ASSERT_FULLADD(c1,c0,c0,1,0);   /* 1  0  0    1   0  */
		ASSERT_FULLADD(c0,c1,c0,1,0);   /* 0  1  0    1   0  */
		ASSERT_FULLADD(c1,c1,c0,0,1);	/* 1  1  0    0   1  */
		ASSERT_FULLADD(c0,c0,c1,1,0);	/* 0  0  1    1   0  */
		ASSERT_FULLADD(c1,c0,c1,0,1);	/* 1  0  1    0   1  */
		ASSERT_FULLADD(c0,c1,c1,0,1);	/* 0  1  1    0   1  */
		ASSERT_FULLADD(c1,c1,c1,1,1);	/* 1  1  1    1   1  */
		printf(".");
		fflush(stdout);

		/* free used memory for key pairs */
		fhe_pk_clear(pk);
		fhe_sk_clear(sk);
	}



	/* free used memory */
	mpz_clear(sum);
	mpz_clear(carry);
	mpz_clear(c0);
	mpz_clear(c1);
	printf(" PASSED.\n");
}

/*
 * test recrypt a cipher text to make sure we can still decrypt it
 */
void
test_recrypt()
{
	printf("RECRYPT\n");
	
	mpz_t c0, c1;
	
	mpz_init(c0);
	mpz_init(c1);
	
	fhe_pk_t pk;
	fhe_sk_t sk;
	fhe_pk_init(pk);
	fhe_sk_init(sk);
	
	for (int i = 0; i < KEYRUNS; i++) {
		fhe_keygen(pk, sk);
		printf("Key-%d pk: %Zd; sk:%Zd\n",i, pk, sk);
		
		for (int j = 0; j < RUNS; j++) {
			fhe_encrypt(c0, pk, 0);
			fhe_encrypt(c1, pk, 1);
			printf("c0: %Zd; c1:%Zd\n", c0, c1);
			
			fhe_recrypt(c0, pk);
			assert(fhe_decrypt(c0, sk) == 0);
			
			fhe_recrypt(c1, pk);
			assert(fhe_decrypt(c1, sk) == 1);
			printf("recrypted c0: %Zd; c1:%Zd\n", c0, c1);
			
			printf(".");
			fflush(stdout);
		}
		printf("\n");
	}
	fhe_pk_clear(pk);
	fhe_sk_clear(sk);
	mpz_clear(c0);
	mpz_clear(c1);
	printf("PASSED.\n");
}


void
test_homomorphic()
{
	printf("HOMOMORPHIC (w/o recrypt)\n");
	
	int m;
	mpz_t c0, c1, temp;
	
	mpz_init(c0);
	mpz_init(c1);
	mpz_init(temp);
	
	fhe_pk_t pk;
	fhe_sk_t sk;
	fhe_pk_init(pk);
	fhe_sk_init(sk);
	
	for (int i = 0; i < KEYRUNS; i++) {
		mpz_t c0, c1;
		
		mpz_init(c0);
		mpz_init(c1);
		
		fhe_pk_t pk;
		fhe_sk_t sk;
		fhe_pk_init(pk);
		fhe_sk_init(sk);
		
		fhe_keygen(pk, sk);
		printf("Key-%d pk: %Zd; sk:%Zd\n",i, pk, sk);
		fhe_encrypt(c0, pk, 0);
		printf("\nadd-chain: ");
		for (int j = 0; j < RUNS*RUNS/RUNS/2; j++) {
			//printf("before c0: %Zd\n",c0);
			fhe_add(c0, c0, c0, pk);
			//printf("after c0: %Zd\n",c0);
			m = fhe_decrypt(c0, sk);
			printf("%i", m);
			fflush(stdout);
		}
		fhe_encrypt(c1, pk, 1);
		printf("\nmul-chain: ");
		for (int j = 0; j < RUNS*RUNS/RUNS/2; j++) {
			fhe_mul(c1, c1, c1, pk);
			m = fhe_decrypt(c1, sk);
			printf("%i", m);
			fflush(stdout);
		}
		printf("\n");
	}
	
	fhe_pk_clear(pk);
	fhe_sk_clear(sk);
	mpz_clear(c0);
	mpz_clear(c1);
	mpz_clear(temp);
	
	printf("PASSED.\n");
}

void
test_fully_homomorphic()
{
	printf("FULLY HOMOMORPHIC (with recrypt)\n");
	
	int m;
	mpz_t c0, c1, temp;
	
	mpz_init(c0);
	mpz_init(c1);
	mpz_init(temp);
	
	fhe_pk_t pk;
	fhe_sk_t sk;
	fhe_pk_init(pk);
	fhe_sk_init(sk);
	
	for (int i = 0; i < KEYRUNS; i++) {
		mpz_t c0, c1;
		
		mpz_init(c0);
		mpz_init(c1);
		
		fhe_pk_t pk;
		fhe_sk_t sk;
		fhe_pk_init(pk);
		fhe_sk_init(sk);
		
		fhe_keygen(pk, sk);
		fhe_encrypt(c0, pk, 0);
		printf("\nadd-chain: ");
		for (int j = 0; j < RUNS*RUNS; j++) {
			fhe_add(c0, c0, c0, pk);
			fhe_recrypt(c0, pk);
			m = fhe_decrypt(c0, sk);
			printf("%i", m);
			fflush(stdout);
		}
		fhe_encrypt(c1, pk, 1);
		printf("\nmul-chain: ");
		for (int j = 0; j < RUNS*RUNS; j++) {
			fhe_mul(c1, c1, c1, pk);
			fhe_recrypt(c1, pk);
			m = fhe_decrypt(c1, sk);
			printf("%i", m);
			fflush(stdout);
		}
		printf("\n");
	}
	
	fhe_pk_clear(pk);
	fhe_sk_clear(sk);
	mpz_clear(c0);
	mpz_clear(c1);
	mpz_clear(temp);
	
	printf("PASSED.\n");
}
