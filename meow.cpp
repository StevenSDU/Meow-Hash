#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "meow.h"
using namespace std;

const char* Message = "Wei_Xiaoyang_202000460027";
const char* Hash = "sdu_cst_20220610";

#define movdqu(A, B)	    A = _mm_loadu_si128((__m128i *)(B))
#define pxor(A, B)	        A = _mm_xor_si128(A, B)
#define psubq(A, B)	        A = _mm_sub_epi64(A, B)
#define aesenc(A, B)	    A = _mm_aesenc_si128(A, B)
#define pxor_clear(A, B)	A = _mm_setzero_si128(); 
#define movq(A, B)          A = _mm_set_epi64x(0, B);
#define palignr(A, B, i)    A = _mm_alignr_epi8(A, B, i)
#define pshufb(A, B)        A = _mm_shuffle_epi8(A, B)
#define pand(A, B)          A = _mm_and_si128(A, B)


#define MEOW_INV_SHUFFLE(r0, r1, r2, r4, r5, r6) \
pxor(r1, r2);     \
aesenc(r4, r1);   \
psubq(r5, r6);    \
pxor(r4, r6);     \
psubq(r1, r5);    \
aesenc(r0, r4);

#define INSTRUCTION_REORDER_BARRIER _ReadWriteBarrier()
#define MEOW_MIX_REG(r1, r2, r3, r4, r5,  i1, i2, i3, i4) \
pxor(r4, i4);                \
psubq(r5, i3);               \
aesenc(r2, r4);              \
INSTRUCTION_REORDER_BARRIER; \
pxor(r2, i2);                \
psubq(r3, i1);               \
aesenc(r1, r2);              \
INSTRUCTION_REORDER_BARRIER;

static void PrintHash(meow_u128 Hash) 
{
	cout<<MeowU32From(Hash, 3)<<MeowU32From(Hash, 2)<<MeowU32From(Hash, 1)<<MeowU32From(Hash, 0)<<"\n";
}

static void PrintKey(meow_u128 Hash1, meow_u128 Hash2) 
{
	cout<<MeowU32From(Hash1, 3)<<MeowU32From(Hash1, 2)<<MeowU32From(Hash1, 1)<<MeowU32From(Hash1, 0)<<MeowU32From(Hash2, 3)<<MeowU32From(Hash2, 2)<<MeowU32From(Hash2, 1)<<MeowU32From(Hash2, 0)<<"\n";
}

static void Inverse(meow_umm Len, void* HashedMsg, void* msg)
{
	meow_u128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
	meow_u128 xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
	meow_u8* rcx = (meow_u8*)HashedMsg;
	movdqu(xmm0, rcx + 0x00);
	movdqu(xmm1, rcx + 0x10); 
	movdqu(xmm2, rcx + 0x20); 
	movdqu(xmm3, rcx + 0x30); 
	movdqu(xmm4, rcx + 0x40); 
	movdqu(xmm5, rcx + 0x50); 
	movdqu(xmm6, rcx + 0x60); 
	movdqu(xmm7, rcx + 0x70); 
    psubq(xmm0, xmm4);
	pxor(xmm0, xmm1);
	pxor(xmm4, xmm5);
	psubq(xmm0, xmm2);
	psubq(xmm1, xmm3);
	psubq(xmm4, xmm6);
	psubq(xmm5, xmm7);

	MEOW_INV_SHUFFLE(xmm3, xmm4, xmm5, xmm7, xmm0, xmm1);
	MEOW_INV_SHUFFLE(xmm2, xmm3, xmm4, xmm6, xmm7, xmm0);
	MEOW_INV_SHUFFLE(xmm1, xmm2, xmm3, xmm5, xmm6, xmm7);
	MEOW_INV_SHUFFLE(xmm0, xmm1, xmm2, xmm4, xmm5, xmm6);
	MEOW_INV_SHUFFLE(xmm7, xmm0, xmm1, xmm3, xmm4, xmm5);
	MEOW_INV_SHUFFLE(xmm6, xmm7, xmm0, xmm2, xmm3, xmm4);
	MEOW_INV_SHUFFLE(xmm5, xmm6, xmm7, xmm1, xmm2, xmm3);
	MEOW_INV_SHUFFLE(xmm4, xmm5, xmm6, xmm0, xmm1, xmm2);
	MEOW_INV_SHUFFLE(xmm3, xmm4, xmm5, xmm7, xmm0, xmm1);
	MEOW_INV_SHUFFLE(xmm2, xmm3, xmm4, xmm6, xmm7, xmm0);
	MEOW_INV_SHUFFLE(xmm1, xmm2, xmm3, xmm5, xmm6, xmm7);
	MEOW_INV_SHUFFLE(xmm0, xmm1, xmm2, xmm4, xmm5, xmm6);

	pxor_clear(xmm9, xmm9);
	pxor_clear(xmm11, xmm11);

	meow_u8* Last = (meow_u8*)msg + (Len & ~0xf);
	int unsigned Len8 = (Len & 0xf);
	if (Len8) {

		movdqu(xmm8, &MeowMaskLen[0x10 - Len8]);

		meow_u8* LastOk = (meow_u8*)((((meow_umm)(((meow_u8*)msg) + Len - 1)) | (MEOW_PAGESIZE - 1)) - 16);
		int Align = (Last > LastOk) ? ((int)(meow_umm)Last) & 0xf : 0;
		movdqu(xmm10, &MeowShiftAdjust[Align]);
		movdqu(xmm9, Last - Align);
		pshufb(xmm9, xmm10);

		pand(xmm9, xmm8);
	}

	if (Len & 0x10) {
		xmm11 = xmm9;
		movdqu(xmm9, Last - 0x10);
	}

	xmm8 = xmm9;
	xmm10 = xmm9;
	palignr(xmm8, xmm11, 15);
	palignr(xmm10, xmm11, 1);
	pxor_clear(xmm12, xmm12);
	pxor_clear(xmm13, xmm13);
	pxor_clear(xmm14, xmm14);
	movq(xmm15, Len);
	palignr(xmm12, xmm15, 15);
	palignr(xmm14, xmm15, 1);

	MEOW_MIX_REG(xmm1, xmm5, xmm7, xmm2, xmm3, xmm12, xmm13, xmm14, xmm15);

	cout << "对应的密钥为: " << endl;
	PrintKey(xmm0, xmm1);
	PrintKey(xmm2, xmm3);
	PrintKey(xmm4, xmm5);
	PrintKey(xmm6, xmm7);
	cout << endl;
	return;
}

int main() 
{
	int MsgLen = strlen(Message); 
	char* message = new char[MsgLen + 1];
	memset(message, 0, MsgLen + 1);
	memcpy(message, Message, MsgLen);
	int Hashlen = strlen(Hash);
	char* Hashed_message = new char[Hashlen + 1];
	memset(Hashed_message, 0, Hashlen + 1);
	memcpy(Hashed_message, Hash, Hashlen);
	cout << "明文为: " << message << endl;
	cout << "经过加密后的密文为: " << Hashed_message << endl;
	Inverse(MsgLen, Hashed_message, message);
	return 0;
}
