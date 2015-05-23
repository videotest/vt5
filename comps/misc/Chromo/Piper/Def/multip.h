/*
 * multip.h
 *
 * definition of some global locations used in slave
 * processor booting and control.
 */
/* insert number of processors here */
#define MASTER 0
#define NPROCESSORS		3

#ifdef GGG
#define C_Boot L_RAMBASE + 0xa6c
#define C_Flag C_Boot+4
#define C_Size C_Flag+4

/*
 * the following structure and definitions describe the processor
 * VME-bus and local address mappings.
 */
struct proc_desc {
	int p_type;			/* 128k or 512k words */
	int g_ram_base;		/* VME-bus (global) address base */
	int g_ram_top;		/* VME-bus address top */
	int l_ram_top;		/* local addressing address top */
};

#define L_RAMBASE 0x20000
#define P0_DESC	{512, 0xc80000, 0xd00000, 0xa0000}
#define P1_DESC	{512, 0x200000, 0x280000, 0xa0000}
#define P2_DESC	{512, 0x280000, 0x300000, 0xa0000}

#endif GGG

