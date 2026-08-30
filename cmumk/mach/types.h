/* SPDX License Indentifier : GPL-2.0 WITH Linux-Syscalls-Note */
/* FIle: mach/types.h */
/* 26/08/26 Pedro Emanuel
 * add x86_64 and ARM-support
 * add uint16, uint8, int64, int16, int8 , uint64, ULONGLONG, UCHAR, and more types
 */

/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef MACH_TYPES_H
#define MACH_TYPES_H

typedef unsigned int	natural_t;
typedef int		integer_t;
typedef char  int8;
typedef unsigned char  uint8;
typedef short  int16;
typedef unsigned short  uint16;
typedef int		int32;
typedef unsigned int	uint32;
typedef long  int64;
typedef unsigned long  uint64;
typedef long long  LONGLONG;
typedef unsigned long long  ULONGLONG;
typedef unsigned short WORD;  // NOLINT(runtime/int)
typedef WORD UWORD;
typedef WORD ATOM;

#ifdef __x86_64__
#define PAGE_SIZE  4096
#define PAGE_SHIFT  12
#elif __aarch64__
#define PAGE_SIZE  16384
#define PAGE_SHIFT  14
#endif

#define BYTE_SIZE	8	/* byte size in bits */
#define PAGE_MASK   (~(PAGE_SIZE - 1))
#define btop(x)		(((unsigned long long)(x)) >> PAGE_SHIFT)
#define ptob(x)		(((unsigned long long)(x)) << PAGE_SHIFT)

#define round_page(x)	((((uint64)(x)) + PAGE_SIZE - 1) & \
					~(PAGE_SIZE-1))
#define runc_page(x)	(((uint64)(x)) & ~(PAGE_SIZE-1))

typedef	natural_t	vm_offset_t;

typedef	natural_t	vm_size_t;

#define VM_MIN_ADDRESS          ((uint64) 0x0000000000000000)
#define VM_MAX_ADDRESS          ((uint64) 0x00007fffffffffff) // 128 Terabytes!

/* Aqui no meio existe um "buraco" de endereços inválidos que o hardware rejeita */

#define VM_MIN_KERNEL_ADDRESS   ((ULONGLONG) 0xffff800000000000) // Início do espaço do Kernel
#define VM_MAX_KERNEL_ADDRESS   ((ULONGLONG) 0xffffffffffffffff) // Topo absoluto da memória

//0x10 bytes (sizeof)
struct _LIST_ENTRY
{
    struct _LIST_ENTRY* Flink;                                              //0x0
    struct _LIST_ENTRY* Blink;                                              //0x8
}; 

typedef struct _LIST_ENTRY	*queue_head_t, list_head;

#endif
