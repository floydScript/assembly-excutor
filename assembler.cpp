#include "stdafx.h"
#include "assembler.h"


char *logicinstrarray[8] ={
	"add",
	"or" ,
	"adc",
	"sbb",
	"and",
	"sub",
	"xor",
	"cmp",
};
char *dainstrarray[8] ={
	"daa",
	"das" ,
	"aaa",
	"aas",
};
char *jccinstrarray[16] ={
	"jo",
	"jno" ,
	"jb",
	"jnb",
	"je",
	"jnz",
	"jbe",
	"ja",
	"js",
	"jns",
	"jpe",
	"jpo",
	"jl",
	"jnl",
	"jle",
	"jg",
};
char *setccinstrarray[16] ={
	"seto",
	"setno",
	"setb",
	"setae",
	"sete",
	"setne",
	"setbe",
	"seta",
	"sets",
	"setns",
	"setpe",
	"setpo",
	"setl",
	"setge",
	"setle",
	"setg",
};
char *flaginstrarray[8] ={
	"cwde",
	"cdq" ,
	"",
	"wait",
	"pushfd",
	"popfd",
	"sahf",
	"lahf",
};
char *stringinstrarray[4] ={
	"",
	"stos",
	"lods",
	"scas",
};

char *shiftinstrarray[8] ={
	"rol",
	"ror",
	"rcl",
	"rcr",
	"shl",
	"shr",
	"sal",
	"sar",
};

char *loopinstrarray[4] ={
	"loopdne",
	"loopde",
	"loopd",
	"jecxz",
};

char *jmpinstrarray[4] ={
	"call",
	"jmp",
	"jmp far",
	"jmp short",
};

char *unaryinstrarray[8] ={
	"test",
	"test",
	"not",
	"neg",
	"mul",
	"imul",
	"div",
	"idiv",
};

char *flagbitinstrarray[8] ={
	"clc",
	"stc",
	"cli",
	"sti",
	"cld",
	"std",
};

char *feffgrpinstrarray[8] ={
	"inc",
	"dec",
	"call",
	"call far",
	"jmp",
	"jmp far",
	"push",
	"",
};
char *descriptorinstrarray[8] ={
	"sldt",
	"str",
	"lldt",
	"ltr",
	"verr",
	"verw",
	"",
	"",
};
char *descriptor0f01meminstrarray[8]={
	"sgdt",
	"sidt",
	"lgdt",
	"lidt",
	"smsw",
	"",
	"lmsw",
	"invlpg",
};

unsigned char prefixarray[11] = {
	0x26,
	0x2e,
	0x36,
	0x3e,
	0x64,
	0x65,
	0x66,
	0x67,
	0xf0,
	0xf2,
	0xf3,
};
//------------------------------------------------

char *register32bitname[8] = {
	"eax",
	"ecx",
	"edx",
	"ebx",
	"esp",
	"ebp",
	"esi",
	"edi",
};
char *register16bitname[8] = {
	"ax",
	"cx",
	"dx",
	"bx",
	"sp",
	"bp",
	"si",
	"di",
};
char *register8bitname[8] = {
	"al",
	"cl",
	"dl",
	"bl",
	"ah",
	"ch",
	"dh",
	"bh",
};
char *segmentregister[8] = {
	"es",
	"cs",
	"ss",
	"ds",
	"fs",
	"gs",
	"seg6:",
	"seg7:",
};
char *addressmethod16bit[8] = {
	"bx+si",
	"bx+di",
	"bp+si",
	"bp+di",
	"si",
	"di",
	"bp",
	"bx",
};

char *xmmregister[8] = {
	"xmm0",
	"xmm1",
	"xmm2",
	"xmm3",
	"xmm4",
	"xmm5",
	"xmm6",
	"xmm7",
};

char *mmregister[8] = {
	"mm0",
	"mm1",
	"mm2",
	"mm3",
	"mm4",
	"mm5",
	"mm6",
	"mm7",
};

char *crregister[8] = {
	"cr0",
	"cr1",
	"cr2",
	"cr3",
	"cr4",
	"cr5",
	"cr6",
	"cr7",
};

char *drregister[8] = {
	"dr0",
	"dr1",
	"dr2",
	"dr3",
	"dr4",
	"dr5",
	"dr6",
	"dr7",
};
//------------------------------------------------


char base[10] = "";
char displacement[10] = "";
char registername[10] = "";
char index[10] = "";
char scale[10] = "";
char immediate[10] = "";
char segment[10] = "";
char ptr[20] = "";
char instruction[30] = "";

char immbuffer[20] = "";
int flag = 1;//flag=1:two operand; flag=0:one operand
int mandatory_prefix_flag = 0;
unsigned char prefix[0x80] = {0};
unsigned char prefix_buff[0x80] = {0};
unsigned char optype_addmethod;

char** p_register;

//-------------------------------------------------------------------------------------------------//
char * getimm(unsigned char * buff,int start,int size,int isunsigned)
{
	int imm = 0 ;
	for(int i = start+size-1;i>=start;i--)
	{
		imm = imm<<8;
		imm +=buff[i];
	}
	int sign;
	if(size == 1)
	{
		sign = imm&0x80;

		if(isunsigned)
			sprintf(immbuffer,"0x%02x",imm);
		else
			sprintf(immbuffer,sign?"-0x%02x":"+0x%02x",imm&0x7f);
	}
	else if (size == 4)
	{
		sign = imm&0x80000000;

		if(isunsigned)
			sprintf(immbuffer,"0x%08x",imm);
		else
			sprintf(immbuffer,sign?"-0x%08x":"+0x%08x",sign?(0-imm):imm);
	}else if(size == 2)
	{
		sign = imm&0x8000;

		if(isunsigned)
			sprintf(immbuffer,"0x%04x",imm);
		else
			sprintf(immbuffer,sign?"-0x%04x":"+0x%04x",sign?(0-imm):imm);
	}
	return immbuffer;
}

int findprefix(unsigned char pf)
{
	int len = sizeof(prefix)/sizeof(prefix[0]);
	
	for(int i = 0;i<len;i++)
	{
		if(prefix[i] == pf)
			return 1;
	}
	return 0;
}

int mysizeof(unsigned char * arr)
{
	int i;
	for(i = 0;i<=0x80;i++)
	{
		if(arr[i] == 0)
			break;
	}
	return i;
}

int detect_prefix_conflict(unsigned char pf,unsigned char pf_buff)
{
	int flag1 = 0 ,i,flag2 = 0;
	unsigned char segment_prefix[6] = {0x26,0x2e,0x36,0x3e,0x64,0x65};
	unsigned char rep_prefix[2] = {0xf2,0xf3};
	int len;
	len = sizeof(segment_prefix)/sizeof(segment_prefix[0]);
	for(i = 0; i< len ;i++)
	{
		if(segment_prefix[i] == pf)
			flag1 =1;
		if (segment_prefix[i] == pf_buff)
			flag2 = 1;
	}

	if((flag1 == 1) && (flag2 == 1))
		return 1;

	flag1 = 0;
	flag2 = 0;
	len = sizeof(rep_prefix)/sizeof(rep_prefix[0]);
	for(i = 0; i< len ;i++)
	{
		if(rep_prefix[i] == pf)
			flag1 = 1;
		if(rep_prefix[i] == pf_buff)
			flag2 = 1;
	}

	if((flag1 == 1) && (flag2 == 1))
		return 1;
	else
		return 0;

}
//-------------------------------------------------------------------------------------------------//
int mod_reg_rm(unsigned char *buff,char** p_register)
{
	int length = 2;
	unsigned char mod;
	unsigned char r_m;
	unsigned char reg;
	
	mod = (buff[1]>>6)&3;
	r_m = buff[1]&7;
	reg = (buff[1]>>3)&7;

	if((buff[0]==0x80) || (buff[0]==0x82) || (buff[0]==0x83) || (buff[0]==0xc0) || (buff[0]==0xc6) || (buff[0]==0xf6))
	{
		strcpy(immediate,getimm(buff,2,1,1));
		length += 1;
	}
	else if((buff[0]==0x81) || (buff[0]==0xc1) || (buff[0]==0xc7) || (buff[0]==0xf7))
	{
		if(findprefix(0x66))
		{
			strcpy(immediate,getimm(buff,2,2,1));
			length +=2;
		}
		else
		{
			strcpy(immediate,getimm(buff,2,4,1));
			length += 4;
		}
	}
	else
		strcpy(registername,p_register[reg]);


	switch(mod)
	{		

	case 0:
		if(findprefix(0x67))
		{
			if((buff[1]&7)==6)
			{
				strcpy(base,"");
				strcpy(displacement,(buff[1]&7)==6?getimm(buff,2,2,1):"");
				length += 2;
			}	
			else
				strcpy(base,addressmethod16bit[buff[1]&7]);
		}
		else
		{
			if(r_m == 4);
			else if(r_m == 5)
			{
				strcpy(displacement,getimm(buff,2,4,1));
				length += 4;
			}
			else
			{
				strcpy(base,register32bitname[r_m]);
			}
		}
		break;
	case 1:
		if(findprefix(0x67))
		{
			strcpy(base,addressmethod16bit[buff[1]&7]);
			strcpy(displacement,getimm(buff,2,1,0));
			length += 1;
		}
		else
		{
			if(r_m == 4)
			{
				strcpy(displacement,getimm(buff,3,1,0));
				length += 1;
			}
			else
			{
				strcpy(displacement,getimm(buff,2,1,0));
				strcpy(base,register32bitname[r_m]);
				length += 1;
			}
		}
		break;
	case 2:
		if(findprefix(0x67))
		{
			strcpy(base,addressmethod16bit[buff[1]&7]);
			strcpy(displacement,getimm(buff,2,4,0));
			length += 4;
		}
		else
		{
			if(r_m == 4)
			{
				strcpy(displacement,getimm(buff,3,4,0));
				length += 4;
			}
			else
			{
				strcpy(displacement,getimm(buff,2,4,0));
				strcpy(base,register32bitname[r_m]);
				length += 4;
			}
		}
		break;
	case 3:
		strcpy(base,p_register[r_m]);
		break;
	default:
		break;

	}
	
	return length;
}

int scale_index_base(unsigned char *buff,int length)
{
	if(findprefix(0x67))
		return length;


	unsigned char s;
	unsigned char i;
	unsigned char b;
	
	s = (buff[2]>>6)&3;
	b = buff[2]&7;
	i = (buff[2]>>3)&7;

	if(b==5)
	{
		unsigned char mod = (buff[1]>>6)&3;
		if(mod == 0)
		{
			strcpy(displacement,getimm(buff,3,4,0));
			length += 4;
		}
		else
			strcpy(base,"ebp+");
	}
	else
		strcpy(base,register32bitname[b]);

	if(i != 4)
	{
		sprintf(index,b==5?"%s":"+%s",register32bitname[i]);
		//sprintf(index,"+%s",register32bitname[i]);
		unsigned char product = 1;
		for(int j = 1;j<=s;j++)
			product = 2*product;
			
		sprintf(scale,s==0?"":"*%d",product);
	}
	
	return length+1;
}


int assembler(unsigned char *buff,char *instrname)
{
	memset(base,0,10);
	memset(displacement,0,10);
	memset(registername,0,10);
	memset(index,0,10);
	memset(scale,0,10);
	memset(immediate,0,10);
	memset(segment,0,10);
	memset(ptr,0,20);
	memset(immbuffer,0,20);
	memset(prefix,0,10);
	memset(instruction,0,30);
	p_register = NULL;

	flag = 1;
	mandatory_prefix_flag = 0;
	optype_addmethod = 0;
	
	int length = 0;
	int differ_num;


	//if prefix_buff is not null
	int prefix_buff_len = mysizeof(prefix_buff);
	if(prefix_buff_len>0)
	{
		buff = buff+prefix_buff_len;
		goto push_prefix_buff;
	}

	//set prefix
	int i,j,k;
	for(i = 0,k = 0;i<sizeof(buff);)
	{
		for(j=0;j<sizeof(prefixarray);j++)
		{
			if(prefixarray[j] == *buff)
			{
				prefix[k] = prefixarray[j];
				buff++;
				k++;
				break;
			}
		}
		i++;
		if(i!=k)
			break;
	}
	
	//set prefix_buff 
	for(i = 0,k = 0;i<mysizeof(prefix);i++)
	{
		int f = 0;
		for(j = 0;j<mysizeof(prefix_buff);j++)
		{
			if(prefix[i] == prefix_buff[j])//remove duplication
			{
				f = 1;
				break;
			}
			if(detect_prefix_conflict(prefix[i],prefix_buff[j]))//cover the conflict
			{
				f = 1;
				prefix_buff[j] = prefix[i];
				break;
			}
		}
		if(f)
			continue;
		prefix_buff[k] = prefix[i];
		k++;
	}
	differ_num = mysizeof(prefix) - mysizeof(prefix_buff);
	if(differ_num)
	{
		strcpy(instrname,"???");
		return differ_num;
	}

push_prefix_buff:
	memcpy(prefix,prefix_buff,mysizeof(prefix_buff));
	memset(prefix_buff,0,0x80);//initial prefix_buff must be here.

	char dest[40] = "";
	char src[40] = "";
	

	char temp[] = "";
	
	
	if((buff[0]&&1))
	{
		p_register = register32bitname;
		strcpy(ptr,"dword ptr");
		/*
		if(findprefix(0x66)&(buff[0]!=0xf0))
		{
			p_register = register16bitname;
			strcpy(ptr,"word ptr");
		}
		*/
	}
	else
	{
		p_register = register8bitname;
		strcpy(ptr,"byte ptr");
	}



	unsigned char b1,b2,b3;
	b1 = buff[0];
	b2 = buff[1];
	b3 = buff[2];

	switch(b1)
	{
	case 0x0f:
		{
			length = 0;
		}	
		break;

	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x18:
	case 0x19:
	case 0x1a:
	case 0x1b:
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x28:
	case 0x29:
	case 0x2a:
	case 0x2b:
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x38:
	case 0x39:
	case 0x3a:
	case 0x3b:
		strcpy(instruction,logicinstrarray[(buff[0]>>3)&7]);

Public:
		flag = 0;
		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&&(b2<0xc0))
			length = scale_index_base(buff,length);


		if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
			strcpy(segment,"ss");
		else
			strcpy(segment,"ds");
	
		if(b2>=0xc0)
			optype_addmethod = 0x31;
		else
		{
			if((b1&0x3)<2)
				optype_addmethod = 0x01;
			else
				optype_addmethod = 0x11;
		}
		break;
	
	case 0x04:
	case 0x05:
	case 0x0c:
	case 0x0d:
	case 0x14:
	case 0x15:
	case 0x1c:
	case 0x1d:
	case 0x24:
	case 0x25:
	case 0x2c:
	case 0x2d:
	case 0x34:
	case 0x35:
	case 0x3c:
	case 0x3d:

		strcpy(instruction,logicinstrarray[(buff[0]>>3)&7]);

		flag = 0;
		
		strcpy(registername,b1&1?"eax":"al");
		strcpy(immediate,b1&1?getimm(buff,1,4,1):getimm(buff,1,1,1));
		length = b1&1?5:2;

		optype_addmethod = 0x13;

		break;

	case 0x06:
	case 0x07:
	case 0x0e:
	case 0x16:
	case 0x17:
	case 0x1e:
	case 0x1f:
		strcpy(instruction,b1&1?"pop":"push");
		strcpy(registername,segmentregister[(b1>>3)&3]);
		optype_addmethod = 0x12;
		length = 1;
		break;

	case 0x27:
	case 0x2f:
	case 0x37:
	case 0x3f:
		strcpy(instruction,dainstrarray[(b1>>3)&3]);
		length = 1;
		break;



	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
		strcpy(registername,register32bitname[b1&7]);
		optype_addmethod = 0x12;
		strcpy(instruction,"inc");
		length = 1;
		break;

	case 0x48:
	case 0x49:
	case 0x4a:
	case 0x4b:
	case 0x4c:
	case 0x4d:
	case 0x4e:
	case 0x4f:
		strcpy(registername,register32bitname[b1&7]);
		optype_addmethod = 0x12;
		strcpy(instruction,"dec");
		length = 1;
		break;


	case 0x50:
	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
		strcpy(registername,register32bitname[b1&7]);
		optype_addmethod = 0x12;
		strcpy(instruction,"push");
		length = 1;
		break;


	case 0x58:
	case 0x59:
	case 0x5a:
	case 0x5b:
	case 0x5c:
	case 0x5d:
	case 0x5e:
	case 0x5f:
		strcpy(registername,register32bitname[b1&7]);
		optype_addmethod = 0x12;
		strcpy(instruction,"pop");
		length = 1;
		break;

	case 0x60:
		strcpy(instruction,findprefix(0x66)?"pushaw":"pushad");
		length = 1;
		break;
	case 0x61:
		strcpy(instruction,findprefix(0x66)?"popaw":"popad");
		length = 1;
		break;

	case 0x62:
		p_register = register32bitname;
		strcpy(ptr,"qword ptr");

		strcpy(instruction,"bound");	
		goto Public;

	case 0x63:
		p_register = register16bitname;
		strcpy(ptr,"word ptr");

		strcpy(instruction,"arpl");	

		flag = 0;
		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&&(b2<0xc0))
			length = scale_index_base(buff,length);

		if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
			strcpy(segment,"ss");
		else
			strcpy(segment,"ds");
		
		if(b2>=0xc0)
			optype_addmethod = 0x31;
		else
			optype_addmethod = 0x01;
		
		break;

	case 0x68:
	case 0x6a:
		strcpy(immediate,b1==0x68?getimm(buff,1,4,1):getimm(buff,1,1,1));
		optype_addmethod = 0x22;
		strcpy(instruction,"push");
		length = (b1==0x68?5:2);
		break;

	case 0x69:
	case 0x6b:
		p_register = register32bitname;
		strcpy(ptr,"dword ptr");

		strcpy(instruction,"imul");	

		flag = 0;
		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&&(b2<0xc0))
			length = scale_index_base(buff,length);


		if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
			strcpy(segment,"ss");
		else
			strcpy(segment,"ds");
		
		if(b2>=0xc0)
		{
			strcpy(immediate,b1==0x6b?getimm(buff,2,1,1):getimm(buff,2,4,1));
			optype_addmethod = 0x14;
		}
		else
		{
			if((b2&7)==4)
			{
				if((b3&7)==5)
				{
					strcpy(immediate,b1==0x6b?getimm(buff,7,1,1):getimm(buff,7,4,1));
				}
				else
				{
					strcpy(immediate,b1==0x6b?getimm(buff,3,1,1):getimm(buff,3,4,1));
				}
			}
			else if((b2&7)==5)
			{
				strcpy(immediate,b1==0x6b?getimm(buff,6,1,1):getimm(buff,6,4,1));
			}
			else 
			{
				strcpy(immediate,b1==0x6b?getimm(buff,2,1,1):getimm(buff,2,4,1));
			}
			optype_addmethod = 0x11;
		}

		length += b1==0x6b?1:4;
		break;//FIXME:if prefix is 0x66,then jump back to case 0x66 -----------------------------------------------------------------------------------------------------------------------------------


	case 0x6d:
		flag = 0;
		strcpy(segment,"es");
		strcpy(base,"edi");
		strcpy(registername,"ds");

		optype_addmethod = 0x01;
		strcpy(instruction,"ins");
		length = 1;
		break;
	case 0x6e:
		flag = 0;
		strcpy(registername,"bx");
		strcpy(segment,"ds");
		strcpy(base,"esi");

		optype_addmethod = 0x11;
		strcpy(instruction,"outs");
		length = 1;
		break;
	case 0x6f:
		flag = 0;
		strcpy(registername,"dx");
		strcpy(segment,"ds");
		strcpy(base,"esi");

		optype_addmethod = 0x11;
		strcpy(instruction,"outs");
		length = 1;
		break;

	case 0x70:
	case 0x71:
	case 0x72:
	case 0x73:
	case 0x74:
	case 0x75:
	case 0x76:
	case 0x77:
	case 0x78:
	case 0x79:
	case 0x7a:
	case 0x7b:
	case 0x7c:
	case 0x7d:
	case 0x7e:
	case 0x7f:
		optype_addmethod = 0x41;
		strcpy(instruction,jccinstrarray[b1&0x0f]);
		length = 2;
		break;

	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
		strcpy(instruction,logicinstrarray[(buff[1]>>3)&7]);

Immediate:
		flag = 0;
		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&&(b2<0xc0))
			length = scale_index_base(buff,length);


		if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
			strcpy(segment,"ss");
		else
			strcpy(segment,"ds");
	
		if(b2>=0xc0)
		{
			strcpy(registername,base);
			optype_addmethod = 0x13;
		}
		else
			optype_addmethod = 0x02;

		break;


	case 0x84:
	case 0x85:
		strcpy(instruction,"test");	
		goto Public;

	case 0x86:
	case 0x87:
		strcpy(instruction,"xchg");	
		goto Public;

	case 0x88:
	case 0x89:
	case 0x8a:
	case 0x8b:
		strcpy(instruction,"mov");	
		goto Public;

	case 0x8c://FIXME:bug occurred when b2>=0xc0
		p_register = segmentregister;
		strcpy(instruction,"mov");	
		goto Public;

	case 0x8d:
		strcpy(instruction,"lea");

DestIsRegister:
		flag = 0;
		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&&(b2<0xc0))
			length = scale_index_base(buff,length);

		if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
			strcpy(segment,"ss");
		else
			strcpy(segment,"ds");

		if(b2>=0xc0)
			optype_addmethod = 0x14;
		else
			optype_addmethod = 0x11;
		break;

	case 0x8e:
		p_register = segmentregister;
		strcpy(instruction,"mov");	
		goto Public;

	case 0x8f://TODO
		length = 0;
		break;

	case 0x90:
		strcpy(instruction,"nop");	
		length = 1;
		break;
	
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
		flag = 0;
		strcpy(registername,register32bitname[b1&7]);
		strcpy(base,"eax");

		optype_addmethod = 0x31;
		strcpy(instruction,"xchg");	
		length = 1;
		break;

	case 0x98:
	case 0x99:
	case 0x9b:
	case 0x9c:
	case 0x9d:
	case 0x9e:
	case 0x9f:
		strcpy(instruction,flaginstrarray[b1&7]);
		length = 1;
		break;

	case 0x9a:
		optype_addmethod = 0x42;
		strcpy(instruction,"call");	
		length = 7;
		break;

	case 0xa0:
	case 0xa1:
	case 0xa2:
	case 0xa3:
		flag = 0;
		strcpy(registername,b1&1?"eax":"al");
		strcpy(displacement,getimm(buff,1,4,1));
		strcpy(segment,"ds");
		if((b1&3)>>1)
		{
			optype_addmethod = 0x01;
		}
		else
		{
			optype_addmethod = 0x11;
		}
		strcpy(instruction,"mov");	
		length = 5;
		break;

	case 0xa4:
	case 0xa5:
	case 0xa6:
	case 0xa7:
		strcpy(instruction,((b1&3)>>1)?"cmps":"movs");
		flag = 0;
		optype_addmethod = 0x43;
		length = 1;
		break;


	case 0xa8:
	case 0xa9:
		flag = 0;
		strcpy(instruction,"test");
		strcpy(base,b1&1?"eax":"al");
		strcpy(immediate,b1&1?getimm(buff,1,4,1):getimm(buff,1,1,1));

		optype_addmethod = 0x32;
		length = b1&1?5:2;
		break;

	case 0xaa:
	case 0xab:
	case 0xac:
	case 0xad:
	case 0xae:
	case 0xaf:
		strcpy(instruction,stringinstrarray[(b1&7)>>1]);
		optype_addmethod = 0x44;
		length = 1;
		break;


	case 0xb0:
	case 0xb1:
	case 0xb2:
	case 0xb3:
	case 0xb4:
	case 0xb5:
	case 0xb6:
	case 0xb7:
	case 0xb8:
	case 0xb9:
	case 0xba:
	case 0xbb:
	case 0xbc:
	case 0xbd:
	case 0xbe:
	case 0xbf:
		flag = 0;
		strcpy(instruction,"mov");
		if(b1&0x8)
		{
			if(findprefix(0x66))
			{
				p_register = register16bitname;
				strcpy(immediate,getimm(buff,1,2,1));
				length = 3;
			}
			else
			{
				p_register = register32bitname;
				strcpy(immediate,getimm(buff,1,4,1));
				length = 5;
			}
			
		}
		else
		{
			p_register = register8bitname;
			strcpy(immediate,getimm(buff,1,1,1));
			length = 2;
		}
		sprintf(registername,p_register[b1&7]);
		optype_addmethod = 0x13;
		break;

	case 0xc0:
	case 0xc1:
		strcpy(instruction,shiftinstrarray[(b2>>3)&7]);
		goto Immediate;

	case 0xc2:
	case 0xc3:
		strcpy(instruction,"ret");
		strcpy(immediate,b1&1?"":getimm(buff,1,2,1));
		optype_addmethod = 0x22;
		length = b1&1?1:3;
		break;

	case 0xc4:
	case 0xc5:
		strcpy(instruction,b1&1?"lds":"les");
		strcpy(ptr,"fword ptr");
		p_register = register32bitname;
		goto DestIsRegister;

	case 0xc6:
	case 0xc7:
		strcpy(instruction,"mov");
		//p_register = register32bitname;
		goto Immediate;

	case 0xc8:
		flag = 0;
		strcpy(instruction,"enter");
		optype_addmethod = 0x45;
		length = 4;
		break;


	case 0xc9:
		strcpy(instruction,"leave");
		length = 1;
		break;

	case 0xca:
	case 0xcb:
		strcpy(instruction,"retf");
		strcpy(immediate,b1&1?"":getimm(buff,1,2,1));
		optype_addmethod = 0x22;
		length = b1&1?1:3;
		break;

	case 0xcc:
		strcpy(instruction,"int3");
		length = 1;
		break;
	case 0xcd:
		strcpy(instruction,"int");
		strcpy(immediate,getimm(buff,1,1,1));
		optype_addmethod = 0x22;
		length = 2;
		break;
	case 0xce:
		strcpy(instruction,"into");
		length = 1;
		break;
	case 0xcf:
		strcpy(instruction,"iretd");
		length = 1;
		break;

	case 0xd0:
	case 0xd1:
	case 0xd2:
	case 0xd3:
		strcpy(instruction,shiftinstrarray[(b2>>3)&7]);

		flag = 0;
		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&&(b2<0xc0))
			length = scale_index_base(buff,length);


		if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
			strcpy(segment,"ss");
		else
			strcpy(segment,"ds");
		
		strcpy(registername,(b1&3)>>1?"cl":"1");

		if(b2>=0xc0)
		{
			optype_addmethod = 0x31;
		}
		else
			optype_addmethod = 0x01;

		break;

	case 0xd4:
	case 0xd5:
		strcpy(instruction,b1&1?"aad":"aam");
		strcpy(immediate,getimm(buff,1,1,1));
		optype_addmethod = 0x22;
		length = 2;
		break;

	case 0xd6:
		strcpy(instruction,b1&1?"xlat":"salc");
		break;

	case 0xd7:
		strcpy(instruction,"xlat");
		strcpy(ptr,"byte ptr");
		strcpy(segment,"ds");
		strcpy(base,"ebx");
		strcpy(index,"al");
		optype_addmethod = 0x03;
		length = 1;
		break;

	case 0xdb:
		strcpy(instruction,"???");
		length = 1;
		break;

	case 0xe0:
	case 0xe1:
	case 0xe2:
	case 0xe3:
		strcpy(instruction,loopinstrarray[b1&3]);
		optype_addmethod = 0x41;
		length = 2;
		break;

	case 0xe4:
	case 0xe5:
	case 0xe6:
	case 0xe7:
		flag = 0;
		strcpy(instruction,b1&2?"out":"in");
		if(b1&2)
		{
			strcpy(immediate,getimm(buff,1,1,1));
			strcpy(registername,b1&1?"eax":"al");
			optype_addmethod = 0x21;
		}
		else
		{
			strcpy(registername,b1&1?"eax":"al");
			strcpy(immediate,getimm(buff,1,1,1));
			optype_addmethod = 0x13;
		}
		length = 2;
		break;

	case 0xe8:
	case 0xe9:
	case 0xea:
	case 0xeb:
		strcpy(instruction,jmpinstrarray[b1&3]);
		if((b1&3)>>1)
		{
			if(b1&1)
			{
				length = 2;
				optype_addmethod = 0x41;
			}
			else
			{
				optype_addmethod = 0x47;
				length = 7;
			}
		}
		else
		{
			optype_addmethod = 0x46;
			length = 5;
		}
		break;

	case 0xec:
	case 0xed:
	case 0xee:
	case 0xef:
		flag = 0;
		strcpy(instruction,b1&2?"out":"in");

		if(b1&2)
		{
			sprintf(base,"dx");
			sprintf(registername,b1&1?"eax":"al");		
		}
		else
		{
			sprintf(registername,"dx");
			sprintf(base,b1&1?"eax":"al");
		}
		optype_addmethod = 0x31;
		length = 1;
		break;

	case 0xf1:
		strcpy(instruction,"int1");
		length = 1;
		break;




	case 0xf4:
		strcpy(instruction,"hlt");
		length = 1;
		break;
	case 0xf5:
		strcpy(instruction,"cmc");
		length = 1;
		break;

	case 0xf6:
	case 0xf7:
		strcpy(instruction,unaryinstrarray[(b2>>3)&7]);
		
		flag = (((b2>>3)&7)<2)?0:1;

		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&&(b2<0xc0))
			length = scale_index_base(buff,length);


		if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
			strcpy(segment,"ss");
		else
			strcpy(segment,"ds");

		if(flag)
			length = length - (b1&1?4:1);
	
		if(b2>=0xc0)
		{
			strcpy(registername,base);
			optype_addmethod = 0x13;
		}
		else
		{
			optype_addmethod = 0x02;
		}
		break;

	case 0xf8:
	case 0xf9:
	case 0xfa:
	case 0xfb:
	case 0xfc:
	case 0xfd:
		strcpy(instruction,flagbitinstrarray[b1&7]);
		length = 1;
		break;


	case 0xfe:
	case 0xff:
		strcpy(instruction,feffgrpinstrarray[(b2>>3)&7]);

		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&&(b2<0xc0))
			length = scale_index_base(buff,length);


		if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
			strcpy(segment,"ss");
		else
			strcpy(segment,"ds");

	
		if(b2>=0xc0)
		{
			strcpy(registername,base);
			optype_addmethod = 0x12;
		}
		else
			optype_addmethod = 0x03;
		break;




	default:
		length = 0;
		break;
	}

	//specify thr segment register
	if((strcmp(base,"ebp")==0)||(strcmp(base,"esp")==0))
		strcpy(segment,"ss");
	else
		strcpy(segment,"ds");


	if(findprefix(0x66) && (!mandatory_prefix_flag))
	{
		//-------------------------------change the 32bit registername to 16bit-------------------------------//
		int len = sizeof(register32bitname)/sizeof(register32bitname[0]);
		for(int i = 0;i<len;i++)
		{
			if(strcmp(registername,register32bitname[i])==0)
			{
				strcpy(registername,register16bitname[i]);
				break;
			}
		}
		//-------------------------------change the 32bit immediate to 16bit and the dword ptr to word------------------------------//
		int s = strlen(immediate);
		if(strlen(immediate)>6)
		{
			char tempimmediate[] = "";
			for(int i = 0;i<4;i++)
			{
				tempimmediate[i] = immediate[strlen(immediate)-i-1];
			}
			sprintf(immediate,"0x%s",tempimmediate);
			length -= 2;
		}
		if(strcmp(ptr,"dword ptr")==0)
		{
			strcpy(ptr,"word ptr");
		}
	}


	if(findprefix(0x26))
		strcpy(segment,"es");
	else if(findprefix(0x2e))
		strcpy(segment,"cs");
	else if(findprefix(0x36))
		strcpy(segment,"ss");
	else if(findprefix(0x3e))
		strcpy(segment,"ds");
	else if(findprefix(0x64))
		strcpy(segment,"fs");
	else if(findprefix(0x65))
		strcpy(segment,"gs");


#if 1
	switch(optype_addmethod)
	{
	case 0x01://
		{
			sprintf(dest,"%s %s:[%s%s%s%s]",ptr,segment,base,index,scale,displacement);
			strcpy(src,registername);
		}
		break;
	case 0x02://
		{
			sprintf(dest,"%s %s:[%s%s%s%s]",ptr,segment,base,index,scale,displacement);
			strcpy(src,immediate);
		}
		break;
	case 0x03://
		{
			sprintf(dest,"%s %s:[%s%s%s%s]",ptr,segment,base,index,scale,displacement);
			strcpy(src,"");
		}
		break;
	//-------------------------------------------------------------------------------
	case 0x11://
		{
			strcpy(dest,registername);
			sprintf(src,"%s %s:[%s%s%s%s]",ptr,segment,base,index,scale,displacement);		
		}
		break;
	case 0x12://
		{
			strcpy(dest,registername);
			strcpy(src,"");
		}
		break;
	case 0x13://
		{
			strcpy(dest,registername);
			strcpy(src,immediate);
		}
		break;
	case 0x14://				
		{
			strcpy(dest,registername);
			strcpy(src,base);
		}
		break;
	//-------------------------------------------------------------------------------
	case 0x21://
		{
			strcpy(dest,immediate);
			strcpy(src,registername);
		}
		break;
	case 0x22://
		{
			strcpy(dest,immediate);
			strcpy(src,"");
		}
		break;
	//-------------------------------------------------------------------------------
	case 0x31://				
		{
			strcpy(dest,base);
			strcpy(src,registername);
		}
		break;
	case 0x32://				
		{
			strcpy(dest,base);
			strcpy(src,immediate);
		}
		break;
	case 0x33://				
		{
			strcpy(dest,base);
			strcpy(src,"");
		}
		break;
	//-------------------------------------------------------------------------------
	case 0x41://
		{
			sprintf(dest,"%08x",buff+length+buff[1]);
			strcpy(src,"");
		}
		break;
	case 0x42:
		{
			sprintf(dest,"far %04x:%08x",getimm(buff,1,2,1),getimm(buff,3,4,1));
			strcpy(src,"");
		}
		break;
	case 0x43:
		{
			if((b1&3)>>1)
				sprintf(dest,"%s ds:[esi],%s es:[edi]",ptr,ptr);
			else
				sprintf(dest,"%s es:[edi],%s ds:[esi]",ptr,ptr);
		}
		break;
	case 0x44:		
		{
			sprintf(dest,"%s %s",ptr,((b1&3)>>1)?"es:[edi]":"ds:[esi]");
			strcpy(src,"");
		}
		break;
	case 0x45:
		{
			strcpy(dest,getimm(buff,1,2,1));
			strcpy(src,getimm(buff,3,1,1));
		}
		break;
	case 0x46:	
		{
			sprintf(dest,"%08x",buff+length+(buff[1]+buff[2]*0x100+buff[3]*0x10000+buff[4]*0x1000000));
			strcpy(src,"");
		}
		break;
	case 0x47:	
		{
			sprintf(dest,"%s:%s",getimm(buff,5,2,1),getimm(buff,1,4,1));
			strcpy(src,"");
		}
		break;
	//------------------------------------------three oprand
	case 0x51:	
		{
			sprintf(dest,"%s:%s",getimm(buff,5,2,1),getimm(buff,1,4,1));
			strcpy(src,"");
		}
		break;
	default:
		{
			strcpy(dest,"");
			strcpy(src,"");
		}
		break;
	}
#endif

	//----------add prefix-length----------
	for(i = 0;i<10;i++)
	{
		if(prefix[i]==0)
			break;
	}
	length += i;
	//----------add prefix-length----------

	if(flag)
	{//one-oprand instruction
		sprintf(instrname,"%s %s",instruction,dest);
	}
	else
	{
		if((b1 == 0x69) || (b1==0x6b) ||((b1==0x0f) && (b2==0x70)) || (b2 == 0xa4) || (b2 == 0xac) || (b2 == 0xc4) || (b2 == 0xc6))
		{//three-oprand instruction
			sprintf(instrname,"%s %s,%s,%s",instruction,dest,src,immediate);
		}
		else if ((b2 == 0xa5) || (b2 == 0xad))
		{
			sprintf(instrname,"%s %s,%s,%s",instruction,dest,src,"cl");
		}
		else
		{//two-oprand instruction
			sprintf(instrname,"%s %s,%s",instruction,dest,src);	
		}
	}

	if(!mandatory_prefix_flag)
	{
		if(findprefix(0xf0))
		{
			sprintf(temp,"lock %s",instrname);
			strcpy(instrname,temp);
		}
		if(findprefix(0xf2))
		{
			sprintf(temp,"repne %s",instrname);
			strcpy(instrname,temp);
		}
		else if(findprefix(0xf3))
		{
			sprintf(temp,"rep %s",instrname);
			strcpy(instrname,temp);
		}
	}	

	if(strcmp(instruction,""))
		return length;
	else
		return 0;
}
