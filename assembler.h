#include "stdio.h"
#include "stdafx.h"
#include "stdlib.h"
#include "windows.h"
#include "string.h"
#include "math.h"

char * getimm(unsigned char * buff,int start,int size,int isunsigned);
int findprefix(unsigned char pf);
int mysizeof(unsigned char * arr);
int detect_prefix_conflict(unsigned char pf,unsigned char pf_buff);
int mod_reg_rm(unsigned char *buff,char** p_register);
int scale_index_base(unsigned char *buff,int length);

int assembler(unsigned char *buff,char *instrname);
int assembler0f(unsigned char *buff,char *instrname);
int assembler0f38(unsigned char *buff,char *instrname);
int assembler0f3a(unsigned char *buff,char *instrname);
//------------------------------------------------------------
extern char *logicinstrarray[8];
extern char *dainstrarray[8];
extern char *jccinstrarray[16];
extern char *setccinstrarray[16];
extern char *flaginstrarray[8];
extern char *stringinstrarray[4];
extern char *shiftinstrarray[8];
extern char *loopinstrarray[4];
extern char *jmpinstrarray[4];
extern char *unaryinstrarray[8];
extern char *flagbitinstrarray[8];
extern char *feffgrpinstrarray[8]; 
extern char *descriptorinstrarray[8];
extern char *descriptor0f01meminstrarray[8];
extern unsigned char prefixarray[11];
//------------------------------------------------------------
extern char *register32bitname[8];
extern char *register16bitname[8];
extern char *register8bitname[8];
extern char *segmentregister[8];
extern char *addressmethod16bit[8];
extern char *xmmregister[8]; 
extern char *mmregister[8];
extern char *crregister[8];
extern char *drregister[8];
//-------------------------------------------------------------
extern char base[10];
extern char displacement[10];
extern char registername[10];
extern char index[10];
extern char scale[10];
extern char immediate[10];
extern char segment[10];
extern char ptr[20];
extern char instruction[30];
extern char immbuffer[20];
extern int flag;//flag=1:two operand; flag=0:one operand
extern int mandatory_prefix_flag;
extern unsigned char prefix[0x80];
extern unsigned char prefix_buff[0x80];
extern unsigned char optype_addmethod;
extern char** p_register;


