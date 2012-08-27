/*
 * file    : f650-test.c
 * project : f640
 *
 * Created on: Aug 27, 2012
 * Author and copyright (C) 2012 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//
#include <stdint.h>
#define INT64_SUPPORTED

/***********************************************************************
Function prototypes, memory and string functions
***********************************************************************/
void * A_memcpy (void * dest, const void * src, size_t count); // Copy count bytes from src to dest
void * A_memmove(void * dest, const void * src, size_t count); // Same as memcpy, allows overlap between src and dest
void * A_memset (void * dest, int c, size_t count);            // Set count bytes in dest to (char)c
size_t GetMemcpyCacheLimit(void);                              // Data blocks bigger than this will be copied uncached by memcpy and memmove
void   SetMemcpyCacheLimit(size_t);                            // Change limit in GetMemcpyCacheLimit
size_t GetMemsetCacheLimit(void);                              // Data blocks bigger than this will be stored uncached by memset
void   SetMemsetCacheLimit(size_t);                            // Change limit in GetMemsetCacheLimit

/***********************************************************************
Function prototypes, miscellaneous functions
***********************************************************************/
int    RoundD (double x);                                      // Round to nearest or even
int    RoundF (float  x);                                      // Round to nearest or even
int    InstructionSet(void);                                   // Tell which instruction set is supported
char * ProcessorName(void);                                    // ASCIIZ text describing microprocessor
void   CpuType(int * vendor, int * family, int * model);       // Get CPU vendor, family and model
size_t DataCacheSize(int level);                               // Get size of data cache
uint64_t ReadTSC(void);                                     // Read microprocessor internal clock (64 bits)
void cpuid_ex (int abcd[4], int eax, int ecx);                 // call CPUID instruction
static inline void cpuid_abcd (int abcd[4], int eax) { cpuid_ex(abcd, eax, 0); }

int main(int argc, char *argv[]) {
    long l1, l2;
    //
    double d = 4.52524634624134;
    int r;
    //
    l1 = ReadTSC();
    r  = (int)(d + 0.5);
    l2 = ReadTSC();
    printf("%f = %d in %ld\n", d, r, (l2 - l1));
    //
    l1 = ReadTSC();
    r  = RoundD(d);
    l2 = ReadTSC();
    printf("%f = %d in %ld\n", d, r, (l2 - l1));
    //
    l1 = ReadTSC();
    l2 = ReadTSC();
    printf("TSC in %ld\n", (l2 - l1));

    return 0;
}
