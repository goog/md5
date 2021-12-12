#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define MOD_PARAM 56   // 448 bits
#define BYTES_IN_BLOCK 64

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


#define GET_UINT32(a,b,i)                            \
 {                                                 \
         (a) = ( (unsigned int) (b)[(i)  ]      )  \
             | ( (unsigned int) (b)[(i)+1] << 8 )        \
             | ( (unsigned int) (b)[(i)+2] << 16)        \
             | ( (unsigned int) (b)[(i)+3] << 24);       \
 }


//FOUR function

#define F(X,Y,Z) ((X & Y) | (~X & Z)) 
#define G(X,Y,Z) ((X & Z) | (~Z & Y)) 
#define H(X,Y,Z) (X ^ Y ^ Z)
#define I(X,Y,Z) (Y ^ ((X) | (~Z)))



//#define FF(a,b,c,d,k,s,i) (a = b +(a + F(b,c,d) + X[k] + T[i])<<s)
//#define GG(a,b,c,d,k,s,i) (a = b +(a + G(b,c,d) + X[k] + T[i])<<s)
//#define HH(a,b,c,d,k,s,i) (a = b +(a + H(b,c,d) + X[k] + T[i])<<s)
//#define II(a,b,c,d,k,s,i) (a = b +(a + I(b,c,d) + X[k] + T[i])<<s)


#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))


#define FF(a, b, c, d, k, s, i) { \
 (a) += F ((b), (c), (d)) + (X[k]) + T[i]; \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }


//#define FF(a,b,c,d,k,s,i) (a = b + ROTATE_LEFT((a + F(b,c,d) + X[k] + T[i]),s))
//#define GG(a,b,c,d,k,s,i) (a = b + ROTATE_LEFT((a + G(b,c,d) + X[k] + T[i]),s))

#define GG(a, b, c, d, k, s, i) { \
 (a) += G ((b), (c), (d)) + (X[k]) + (T[i]); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define HH(a, b, c, d, k, s, i) { \
 (a) += H ((b), (c), (d)) + (X[k]) + (T[i]); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }


#define II(a, b, c, d, k, s, i) { \
 (a) += I ((b), (c), (d)) + (X[k]) + (T[i]); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

//#define HH(a,b,c,d,k,s,i) (a = b + ROTATE_LEFT((a + H(b,c,d) + X[k] + T[i]),s))
//#define II(a,b,c,d,k,s,i) (a = b + ROTATE_LEFT((a + I(b,c,d) + X[k] + T[i]),s))


#define PRINT_ABCD() {  \
printf("A %08X ", A);   \
printf("B %08X ", B);   \
printf("C %08X ", C);   \
printf("D %08X \n", D); \
}


// 1 round left shift bit preset
#define S11 7
#define S12 12
#define S13 17
#define S14 22


// 2 round left shift bit preset
#define S21 5
#define S22 9
#define S23 14
#define S24 20



// 3 round left shift bit preset
#define S31 4
#define S32 11
#define S33 16
#define S34 23


// 4 round left shift bit preset
#define S41 6
#define S42 10
#define S43 15
#define S44 21




unsigned int T[65] = {
                  0,         // reserved
                  0xd76aa478,// 1
                  0xe8c7b756,// 2
                  0x242070db,// 3
                  0xc1bdceee,// 4
                  0xf57c0faf,// 5
                  0x4787c62a,//6
                  0xa8304613,//7
                  0xfd469501,//8
                  0x698098d8,//9
                  0x8b44f7af,//10
                  0xffff5bb1,//11
                  0x895cd7be,//12
                  0x6b901122,//13
                  0xfd987193,//14
                  0xa679438e,//15
                  0x49b40821,//16

                  0xf61e2562,//17
                  0xc040b340,//18
                  0x265e5a51,//19
                  0xe9b6c7aa,//20 conf
                  0xd62f105d,//21
                  0x02441453,//22
                  0xd8a1e681,//23
                  0xe7d3fbc8,//24
                  0x21e1cde6,//25
                  0xc33707d6,//26
                  0xf4d50d87,//27
                  0x455a14ed,//28
                  0xa9e3e905,//29
                  0xfcefa3f8,//30
                  0x676f02d9,//31
                  0x8d2a4c8a,//32 //
                  0xfffa3942,//33
                  0x8771f681,//34
                  0x6d9d6122,//35
                  0xfde5380c,//36
                  0xa4beea44,//37
                  0x4bdecfa9,//38
                  0xf6bb4b60,//39

                  
                  0xbebfbc70,  //40
                  0x289b7ec6,  //41
                  0xeaa127fa, // 42
                  0xd4ef3085,// 43
                  0x04881d05,// 44
                  0xd9d4d039,// 45
                  0xe6db99e5,
                  0x1fa27cf8,
                  0xc4ac5665,
                  0xf4292244, // 49
                  0x432aff97, //50
                  0xab9423a7, //51
                  0xfc93a039, //52
                  0x655b59c3, //53
                  0x8f0ccc92, //54
                  0xffeff47d, //55
                  0x85845dd1, //56
                  0x6fa87e4f, //57
                  0xfe2ce6e0, //58
                  0xa3014314, //59
                  0x4e0811a1, //60
                  0xf7537e82, //61
                  0xbd3af235, //62
                  0x2ad7d2bb, //63
                  0xeb86d391, //64
            };


void encode(unsigned int *input, unsigned char *output, unsigned int len)
{
    unsigned int i, j;

    for(i = 0, j = 0; j < len; i++, j += 4) 
    {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}


void padding(unsigned char *data, size_t data_len, unsigned char *output, size_t *output_size)
{

    int mod = data_len % MOD_PARAM;
    int padding_bytes = MOD_PARAM - mod;

    printf("padding_bytes %d\n", padding_bytes);

    memcpy(output, data, data_len);
    memset(output + data_len, 0x80, 1);

    if(padding_bytes - 1 > 0)
    {
        memset(output + data_len + 1, 0, padding_bytes - 1);
    }
    

    // add length
    *output_size = data_len + padding_bytes + 8; // 64bit

    //address to write bit length 
    unsigned int *length = (unsigned int *)(output + data_len + padding_bytes);
    size_t bits_cnt = data_len * 8;
    *length = (unsigned int)bits_cnt;
    *(length+1) = (unsigned int)(bits_cnt>>32);
    
    printf("all data has %ld bytes, %ld bits\n", *output_size, *output_size * 8);
}





// data_len = 64 *N
unsigned int compute_md5(unsigned char *data, size_t data_len)
{

    int N = data_len / BYTES_IN_BLOCK;
    // A 16 word block
    unsigned int X[16] = {0};    // 64 bytes
    int i = 0;

    unsigned int A ;
    unsigned int B ;
    unsigned int C ;
    unsigned int D ;
    unsigned int state[4] = 
    {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476
    };
    
    
    printf("========data hex format========================\n");
    for(i = 0; i < data_len; i++)
    {
        unsigned char tmp = *(data + i);
        //printf("%02x ", *(data + i));
        printf(" "BYTE_TO_BINARY_PATTERN"", BYTE_TO_BINARY(tmp));
    }
    printf("\n");
    
    
    for(i = 0; i < N; i++)
    {

        memcpy(X, data + BYTES_IN_BLOCK * i, BYTES_IN_BLOCK);
        #if 0
        unsigned char *buffer = (unsigned char *)(data + BYTES_IN_BLOCK * i);
        GET_UINT32 (X[ 0],buffer, 0);
        GET_UINT32 (X[ 1],buffer, 4);
        GET_UINT32 (X[ 2],buffer, 8);
        GET_UINT32 (X[ 3],buffer,12);
        GET_UINT32 (X[ 4],buffer,16);
        GET_UINT32 (X[ 5],buffer,20);
        GET_UINT32 (X[ 6],buffer,24);
        GET_UINT32 (X[ 7],buffer,28);
        GET_UINT32 (X[ 8],buffer,32);
        GET_UINT32 (X[ 9],buffer,36);
        GET_UINT32 (X[10],buffer,40);
        GET_UINT32 (X[11],buffer,44);
        GET_UINT32 (X[12],buffer,48);
        GET_UINT32 (X[13],buffer,52);
        GET_UINT32 (X[14],buffer,56);
        GET_UINT32(X[15],buffer,60);

        #endif

        A = state[0];
        B = state[1];
        C = state[2];
        D = state[3];


        // round 1
        FF(A, B, C, D, 0, S11, 1);    
        FF(D, A, B, C, 1, S12, 2);
        FF(C, D, A, B, 2, S13, 3);
        FF(B, C, D, A, 3, S14, 4);
        
        
        FF(A, B, C, D, 4, S11, 5);
        FF(D, A, B, C, 5, S12, 6);
        FF(C, D, A, B, 6, S13, 7);
        FF(B, C, D, A, 7, S14, 8);
        

        FF(A, B, C, D, 8, S11, 9);
        FF(D, A, B, C, 9, S12, 10);
        FF(C, D, A, B, 10, S13, 11);
        FF(B, C, D, A, 11, S14, 12);

        FF(A, B, C, D, 12, S11, 13);
        FF(D, A, B, C, 13, S12, 14);
        FF(C, D, A, B, 14, S13, 15);
        FF(B, C, D, A, 15, S14, 16);
        //printf("x s i %08x %d %08x\n", X[15], 22, T[16]);
        PRINT_ABCD();
        
        // round 2
        GG(A, B, C, D, 1, S21, 17);
        GG(D, A, B, C, 6, S22, 18);
        GG(C, D, A, B, 11, S23, 19);
        GG(B, C, D, A, 0, S24, 20);

        GG(A, B, C, D, 5, S21, 21);
        GG(D, A, B, C, 10, S22, 22);
        GG(C, D, A, B, 15, S23, 23);
        GG(B, C, D, A, 4, S24, 24);

        GG(A, B, C, D, 9, S21, 25);
        GG(D, A, B, C, 14, S22, 26);
        GG(C, D, A, B, 3, S23, 27);
        GG(B, C, D, A, 8, S24, 28);

        GG(A, B, C, D, 13, S21, 29);
        GG(D, A, B, C, 2, S22, 30);
        GG(C, D, A, B, 7, S23, 31);
        GG(B, C, D, A, 12, S24, 32);


        // ROUND 3
        HH(A, B, C, D, 5, S31, 33);
        HH(D, A, B, C, 8, S32, 34);
        HH(C, D, A, B, 11, S33, 35);
        HH(B, C, D, A, 14, S34, 36);

        HH(A, B, C, D, 1, S31, 37);
        HH(D, A, B, C, 4, S32, 38);
        HH(C, D, A, B, 7, S33, 39);
        HH(B, C, D, A, 10, S34, 40);

        HH(A, B, C, D, 13, S31, 41);
        HH(D, A, B, C, 0, S32, 42);
        HH(C, D, A, B, 3, S33, 43);
        HH(B, C, D, A, 6, S34, 44);

        HH(A, B, C, D, 9, S31, 45);
        HH(D, A, B, C, 12, S32, 46);
        HH(C, D, A, B, 15, S33, 47);
        HH(B, C, D, A, 2, S34, 48);

        
        // ROUND 4
        II(A, B, C, D, 0, S41, 49);
        II(D, A, B, C, 7, S42, 50);
        II(C, D, A, B, 14, S43, 51);
        II(B, C, D, A, 5, S44, 52);

        II(A, B, C, D, 12, S41, 53);
        II(D, A, B, C, 3, S42, 54);
        II(C, D, A, B, 10, S43, 55);
        II(B, C, D, A, 1, S44, 56);

        II(A, B, C, D, 8, S41, 57);
        II(D, A, B, C, 15, S42, 58);
        II(C, D, A, B, 6, S43, 59);
        II(B, C, D, A, 13, S44, 60);

        II(A, B, C, D, 4, S41, 61);
        II(D, A, B, C, 11, S42, 62);
        II(C, D, A, B, 2, S43, 63);
        II(B, C, D, A, 9, S44, 64);

        state[0] += A;
        state[1] += B;
        state[2] += C;
        state[3] += D;
    
        
    }


    
    unsigned char digest[16];
    encode(state, digest, sizeof(digest));

    for(i = 0; i < 16; i++)
        printf("%02x", digest[i]);
   
    
}



int main()
{
    char buf[] = "hello";
    unsigned char output[10*1024] = {0};
    size_t output_size = 0;
    padding(buf, strlen(buf), output, &output_size);
    
    compute_md5(output, output_size);

}









