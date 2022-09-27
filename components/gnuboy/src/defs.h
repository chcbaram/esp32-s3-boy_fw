


#ifndef __DEFS_H__
#define __DEFS_H__



#ifdef IS_LITTLE_ENDIAN
#define LO 0
#define HI 1
#else
#define LO 1
#define HI 0
#endif


typedef unsigned char byte;

typedef unsigned char un8;
typedef unsigned short un16;
typedef unsigned int un32;

typedef signed char n8;
typedef signed short n16;
typedef signed int n32;

typedef un16 word;
typedef word addr;


// #define fopen     ob_fopen
// #define fclose    ob_fclose
// #define fread     ob_fread
// #define fwrite    ob_fwrite
// #define fgets     ob_fgets
// #define fseek     ob_fseek
// #define rewind    ob_frewind
// #define fgetc     ob_fgetc
// #define ftell     ob_ftell



#include <stdio.h>
#include "bsp.h"

extern FILE  *ob_fopen(const char *filename, const char *mode);
extern int    ob_fclose(FILE *stream);
extern size_t ob_fread(void *ptr, size_t size, size_t count, FILE *stream);
extern size_t ob_fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
extern int    ob_fflush(FILE *stream);
extern int    ob_feof(FILE *stream);
extern int    ob_fseek(FILE *stream, long offset, int whence);
extern long   ob_ftell(FILE *stream);
extern int    ob_fgetc(FILE *stream);
extern char*  ob_fgets(char* str, int num, FILE* stream);


#endif

