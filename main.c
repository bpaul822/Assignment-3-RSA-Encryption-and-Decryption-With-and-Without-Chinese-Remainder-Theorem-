#include <stdio.h>
#include <stdint.h>
#include<math.h>
#include <time.h>

//custom mod function
uint64_t modcal(uint64_t n, uint64_t base){
  return (n -(n/base)*base);
}

//modulo inverse extended Euclid Algo
// Assumption: gcd(a, m) = 1
uint64_t modInverse(uint64_t a, uint64_t m)
{
	uint64_t m0 = m;
	int64_t y = 0, x = 1;
	uint64_t q,t;
	if (m == 1){
		return 0;
		printf("Number is prime");
	}
	while (a > 1) {
		q = a / m;
		t = m;
		m = modcal(a,m);
		a = t;
		t = y;
		y = x - q * y;
		x = t;
	}
	// Make x positive
	if (x < 0)
		x += m0;
	return x;
}

//Fast Modulo exponentiation: a^b modn
uint64_t FastExpMod (uint64_t a, uint64_t b, uint64_t n) {
	uint64_t x = 1,y;
	a = modcal(a,n);
    y = a;
	while (b > 0) {
		if (modcal(b,2) == 1)
              x = modcal((x*y),n);
			  y = modcal((y*y),n);
		b= b >> 1;
	}
	return (modcal(x,n));
}

// Returns gcd of a and b
uint64_t  gcd(uint64_t  a, uint64_t  h)
{
	uint64_t  temp;
	while (1)
	{
		temp = modcal(a,h);
		if (temp == 0)
		return h;
		a = h;
		h = temp;
	}
}

// Encryption c = (msg ^ e) % n
uint64_t encrypt(uint64_t msg, uint64_t  e, uint64_t n,int mode){
    uint64_t c;
	c=FastExpMod(msg,e,n);
	if(mode == 1){
	  printf("\nRSA Encrypted Ciper Text = %llu ", c);
	}
	return c;
}

// Decryption msg = (c ^ d) % n
int impcnt=0,max_imp =0;
void decrypt(uint64_t c, uint64_t d, uint64_t n, uint64_t p, uint64_t q, uint64_t msg, int mode){
    clock_t start_t, end_t, total_t1,total_t2;
    uint64_t m;
    uint64_t vp,vq,xp,xq,dp1,dq1,vpq;//,cp1,cq1,cn1;

    start_t = clock();
    if(mode == 1){
      printf("\n\nStarting of Decryption(Without CRT), start_t1 = %ld\n", start_t);}
	m=FastExpMod(c,d,n);
	if(mode == 1){
	  printf("\nRSA Decrypted Message(Without CRT) = %llu \n", m);}

    end_t = clock();
    total_t1 = (double)(end_t - start_t);
    if(mode == 1){
      printf("\nEnd of Decryption(Without CRT), start_t1 = %ld  end_t1 = %ld  total_t1 =%ld\n ", start_t, end_t,total_t1);}

    dp1 = modcal(d,p-1);
    dq1 = modcal(d,q-1);
    xp  = q*modInverse(q,p);
    xq  = p*modInverse(p,q);

    start_t = clock();
    if(mode == 1){
      printf("\n\nStarting of Decryption(Using CRT), start_t2 = %ld\n", start_t);}

    vp = FastExpMod(c,dp1,p);
    vq = FastExpMod(c,dq1,q);
    vpq = vp*xp + vq*xq;
    m   = vpq -((vpq/n)*n);//custom mod function

	if(mode == 1){
	  printf("\nRSA Decrypted Message(Using CRT) = %llu \n", m);
    }
    end_t = clock();
    total_t2 = (double)(end_t - start_t);
    if(mode == 1){
      printf("\nEnd of Decryption(Using CRT), start_t2 = %ld  end_t2 = %ld  total_t2 = %ld\n ", start_t, end_t,total_t2);
    }

      if (total_t1 > total_t2 ){
        impcnt++;
      if(total_t1 > max_imp){
        max_imp = total_t1 - total_t2;
      }
    }
}

int main()
{
    int    check,mode;
	uint64_t p;
	p = 71789;
	uint64_t q = 51419;// Two random prime numbers
    float percent_imp;
    uint64_t d_int, d,c,m;
    uint64_t msg = 0;
	uint64_t n = p*q;
	uint64_t e ;
	uint64_t phi = (p-1)*(q-1);
	printf("n = %llu \np = %llu \nq = %llu \npi(n) = %llu\n",n,p,q,phi);

	//Key generation
	while (e < phi){
		// e must be co-prime to phi and smaller than phi.
		if (gcd(e, phi)==1 && e != p && e != q)
			break;
		else
			e++;
	}
	d = modInverse(e,phi);
    printf("\nPublic Key = %llu \nPrivate Key = %llu \n", e,d);
    //Key check for confirming they are modulo inverses
    check = modcal(d*e,phi);
    if(check !=1){
        printf("Error: Inverse e*d modphi is not equal to 1 !\n");
        return 0;
    }
    printf("\nEnter the Mode\n 1: RSA Encrypt followed by Decrypt \
           \n 2:Demonstration of CRT improvement iterating over large samples \n ");
    scanf("%ld",&mode);

    if(mode == 1){
        printf("\nEnter the Number to be encrypted: Positive number < %llu\n ",n);
        scanf("%llu",&msg);
        if(msg >= n){
            printf("Error: Encryption cannot be deciphered as msg > n: You need to enter a number < n! \n");
            return 0;
        }
	    printf("Number Entered is = %llu ", msg);
	    c= encrypt(msg,e,n,mode);
        decrypt(c,d,n,p,q,msg,mode);
    }
    else {
      int steps = 1000000,i;
        for (i=0 ; i <n ; i= i+ n/steps){
       msg=i;
       c= encrypt(msg,e,n,mode);
       decrypt(c,d,n,p,q,msg,mode);
      }
       percent_imp = ((impcnt*1.0)/(steps*1.0))*100.0;
       printf("Out of %llu Iterations, No of cases CRT gave Improvement is %d \
        \nAverage number of Improvements %f Max Improvement is %d units\n",steps,impcnt,percent_imp,max_imp);
    }
   getch();
   return 0;
}
