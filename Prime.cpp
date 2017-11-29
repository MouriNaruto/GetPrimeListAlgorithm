#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <stdlib.h>

#define GET_BIT_VALUE(Array, Index) \
	(bool)((Array)[(Index) >> 5] & (1 << ((Index) & 0x1F)))

#define SET_BIT_TRUE(Array, Index) \
	((Array)[(Index) >> 5] |= (1 << ((Index) & 0x1F)))

#define SET_BIT_FALSE(Array, Index) \
	((Array)[(Index) >> 5] &= ~(1 << ((Index) & 0x1F)))

/*
使用改进后的区间素数筛法获取[0,MaxNum]范围内的素数情况
返回的是用malloc分配的记录素数情况的位图
*/
unsigned long* GetPrimeList(unsigned long MaxNum)
{	
	// 计算内存块大小
	size_t size = ((MaxNum + 1) >> 3) + 1;

	// 分配一块内存
	unsigned long *BitArray = (unsigned long*)malloc(size);
	if (BitArray)
	{
		// 内存块初始化为令偶数的对应位为1且奇数的对应位为0的魔数，假定该范围奇
		// 数都是素数且偶数都是合数。
		memset(BitArray, 0xAA, size);

		// 设1不是质数，2是质数
		SET_BIT_FALSE(BitArray, 1);
		SET_BIT_TRUE(BitArray, 2);
 
		// 获取[3, sqrt(MaxNum))范围内的质数，因为[2, n]之间的任意所有合数都能
		// 由[2, sqrt(n)]内的任意质数组合得到。
		unsigned long MaxRange = sqrt(MaxNum);
		for (unsigned long CurNum = 3; CurNum < MaxRange + 1; CurNum += 2)
		{
			// 如果CurNum不为质数，则跳过
			if (!GET_BIT_VALUE(BitArray, CurNum)) continue;

			// 如果CurNum为质数，则用其筛掉[CurNum^2 ,MaxNum]范围内可以整除该质
			// 数的奇合数。因为在之前初始化内存块时已经筛去了全部的偶合数且n能
			// 把n^2以上的合数筛掉,[n,n^2]之间的会被比n小的质数筛掉。
			for (unsigned long i = CurNum * CurNum; i < MaxNum; i += CurNum << 1)
			{
				if (GET_BIT_VALUE(BitArray, i)) SET_BIT_FALSE(BitArray, i);
			}
		}
	}

	return BitArray;
}

#include <Windows.h>

ULONGLONG M2GetTickCount()
{
	LARGE_INTEGER Frequency = { 0 }, PerformanceCount = { 0 };

	if (QueryPerformanceFrequency(&Frequency))
	{
		if (QueryPerformanceCounter(&PerformanceCount))
		{
			return (PerformanceCount.QuadPart * 1000 / Frequency.QuadPart);
		}
	}

	return GetTickCount64();
}

template<class Function>
void Test(Function func)
{
	unsigned long MaxNum = 0x3FFFFFFF; //0x7FFFFFFF;

	unsigned long sum = 0;

	Sleep(1000);

	ULONGLONG StartTime = M2GetTickCount();

	unsigned long* BitMap = func(MaxNum);

	ULONGLONG EndTime = M2GetTickCount();
	wprintf(L"Time = %llu ms\n", EndTime - StartTime);

	if (BitMap)
	{
		for (unsigned long i = 0; i < MaxNum; ++i)
		{
			if (GET_BIT_VALUE(BitMap, i))
			{
				++sum;
			}
		}

		printf("%ld\n", sum);

		free(BitMap);
	}
}


int main0()
{		
	Test<>(&GetPrimeList);
	
	

	return 0;
}


//bool notPrime[1 << 24];
unsigned long notPrime[((0x3FFFFFFF + 1) >> 5) + 1];
size_t prime[54400028], pcnt;
int main()
{
	Test<>(&GetPrimeList);
	
	Sleep(1000);
	
	ULONGLONG StartTime = M2GetTickCount();

	//memset(notPrime, 0x55, sizeof(notPrime));

	const size_t x = (sizeof(notPrime) + sizeof(prime)) >> 20;

	//notPrime[1] = true;
	SET_BIT_TRUE(notPrime, 1);
	for (size_t i = 2; i < 0x3FFFFFFF; ++i)
	{
		//if (!notPrime[i]) prime[pcnt++] = i;
		if (!GET_BIT_VALUE(notPrime, i)) prime[pcnt++] = i;
		for (size_t j = 0; j < pcnt && i * prime[j] < 0x3FFFFFFF; ++j)
		{
			//notPrime[i * prime[j]] = true;
			SET_BIT_TRUE(notPrime, i * prime[j]);
			if (i % prime[j] == 0) break;
		}
	}

	ULONGLONG EndTime = M2GetTickCount();
	wprintf(L"Time = %llu ms\n", EndTime - StartTime);
	
	printf("%ld\n", pcnt);
	
	return 0;
}