#include "stdafx.h"
#include "assembler.h"


int assembler0f(unsigned char *buff,char *instrname)
{
	int length = 0;

	char temp[] = "";
	unsigned char b1,b2,b3;
	b1 = buff[0];
	b2 = buff[1];
	b3 = buff[2];

	switch(b1)
	{

	case 0x38:
		// three opcode instruction
		{
			length += (assembler0f38(buff+1,instrname)+1);
			if(length==1)//if length==1,return 0;
				return 0;		
		}
		break;

	case 0x3a:
		// three opcode instruction
		{
			length += (assembler0f3a(buff+1,instrname)+1);
			if(length==1)//if length==1,return 0;
				return 0;		
		}
		break;




	case 0x00:
		{
			strcpy(instruction,descriptorinstrarray[(b2>>3)&7]);

			strcpy(ptr,"word ptr");
			p_register = register16bitname;
			
	//nosrc:
			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);

			if(b2>=0xc0)
			{
				optype_addmethod = 0x33;
			}
			else
			{
				optype_addmethod = 0x03;
			}
		}
		break;
	
	case 0x01:
		{
			unsigned char mod,r_m,reg;
			mod = (b2>>6)&3;
			r_m = b2&7;
			reg = (b2>>3)&7;

			char *op_0f01_11b_array[8][8] = {
				{"","vmcall","vmlaunch","vmresume","vmxoff"},
				{"monitor","mwait","clac","stac",""},
				{"xgetbv","xsetbv","","","vmfunc","xend","xtest"},
				{""},
				{"smsw","smsw","smsw","smsw","smsw","smsw","smsw","smsw"},
				{""},
				{"lmsw","lmsw","lmsw","lmsw","lmsw","lmsw","lmsw","lmsw",},
				{"swapgs","rdtscp",}
			};

			if(mod == 0x3)
			{
				strcpy(instruction,op_0f01_11b_array[reg][r_m]);
				length+=2;	
			}
			else
			{
				strcpy(instruction,descriptor0f01meminstrarray[reg]);
				length += mod_reg_rm(buff,p_register);

				if(((b2&7)==4)&(b2<0xc0))
					length = scale_index_base(buff,length);

				if(reg<4)
				{
					strcpy(ptr,"fword ptr");

				}
				else if ((reg>=4)&(reg<7))
				{
					strcpy(ptr,"word ptr");
				}
				else
				{
					strcpy(ptr,"");
				}

				optype_addmethod = 0x03;
			}
		}
		break;

	case 0x02:
	case 0x03:
		{
			strcpy(instruction,b1 == 0x02?"lar":"lsl");
			
			flag = 0;
			length += mod_reg_rm(buff,p_register);
			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			if(b2>=0xc0)
				optype_addmethod = 0x31;
			else
				optype_addmethod = 0x11;

		}
		break;

	case 0x05:
		strcpy(instruction,"syscall");
		length +=1;
		break;
	case 0x06:
		strcpy(instruction,"clts");
		length +=1;
		break;
	case 0x07:
		strcpy(instruction,"sysret");
		length +=1;
		break;
	case 0x08:
		strcpy(instruction,"invd");
		length +=1;
		break;
	case 0x09:
		strcpy(instruction,"wbinvd");
		length +=1;
		break;
	case 0x0b:
		strcpy(instruction,"ud2");
		length +=1;
		break;

	case 0x0d:
		{
			strcpy(instruction,"prefetch");
			strcpy(ptr,"qword ptr");

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);

			if(b2>=0xc0)
			{
				strcpy(instruction,"");
			}
			else
			{
				optype_addmethod = 0x03;
			}

		}
		break;

	case 0x10:
	case 0x11:
		{
			if(findprefix(0x66))
				strcpy(instruction,"movupd");
			else if(findprefix(0xf3))
				strcpy(instruction,"movss");
			else if(findprefix(0xf2))
				strcpy(instruction,"movsd");
			else
				strcpy(instruction,"movups");

			strcpy(ptr,"dqword ptr");
			p_register = xmmregister;

			flag = 0;

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
				optype_addmethod = b1&1?0x31:0x14;
			else
				optype_addmethod = b1&1?0x01:0x11;
		}
		break;

	case 0x12:
	case 0x13:
		{
			if(findprefix(0x66))
				strcpy(instruction,"movlpd");
			else if(findprefix(0xf3))
				strcpy(instruction,b1==0x13?"":"movsldup");
			else if(findprefix(0xf2))
				strcpy(instruction,b1==0x13?"":"movddup");
			else
				strcpy(instruction,"movlps");
			strcpy(ptr,"qword ptr");
			p_register = xmmregister;

			flag = 0;

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
				optype_addmethod = b1&1?0x31:0x14;
			else
				optype_addmethod = b1&1?0x01:0x11;
		}
		break;


	case 0x14:
	case 0x15:
		{
			if(findprefix(0x66))
				strcpy(instruction,b1&1?"unpcklpd":"unpckhpd");
			else if(findprefix(0xf3))
				strcpy(instruction,"");
			else if(findprefix(0xf2))
				strcpy(instruction,"");
			else			
				strcpy(instruction,b1&1?"unpcklps":"unpckhps");

			strcpy(ptr,"dqword ptr");
			p_register = xmmregister;

reg_mem:
			flag = 0;

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
				optype_addmethod = 0x14;
			else
				optype_addmethod = 0x11;
		}
		break;

	case 0x16:
	case 0x17:
		{
			if(findprefix(0x66))
				strcpy(instruction,"movhps");
			else if(findprefix(0xf3))
				strcpy(instruction,b1&1?"":"movshdup");
			else if(findprefix(0xf2))
				strcpy(instruction,"");
			else
				strcpy(instruction,"movhps");
			strcpy(ptr,"qword ptr");
			p_register = xmmregister;

			flag = 0;

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
				optype_addmethod = b1&1?0x31:0x14;
			else
				optype_addmethod = b1&1?0x01:0x11;
		}
		break;


	case 0x18:
		{
			char *temp_array[4] = {"prefetchnta","prefetcht0","prefetcht1","prefetcht2"};
			strcpy(instruction,temp_array[(b2>>3)&7]);
			strcpy(ptr,"qword ptr");

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
				strcpy(instruction,"");
			else
				optype_addmethod = b1&1?0x01:0x03;
		}
		break;

	case 0x1f:
		{
			strcpy(instruction,"nop");
			strcpy(ptr,"dword ptr");

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
				optype_addmethod = 0x33;
			else
				optype_addmethod = 0x03;
		}
		break;

	case 0x20:
	case 0x22:
		{
			strcpy(instruction,"mov");
			strcpy(ptr,"dword ptr");
			p_register = crregister;

			flag = 0;

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
			{
				strcpy(base,register32bitname[b2&7]);
				optype_addmethod = (b1>>1)&1?0x14:0x31;
			}
			else
				optype_addmethod = (b1>>1)&1?0x11:0x01;
		}
		break;


	case 0x21:
	case 0x23:
		{
			strcpy(instruction,"mov");
			strcpy(ptr,"dword ptr");
			p_register = drregister;

			flag = 0;

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
			{
				strcpy(base,register32bitname[b2&7]);
				optype_addmethod = (b1>>1)&1?0x14:0x31;
			}
			else
				optype_addmethod = (b1>>1)&1?0x11:0x01;
		}
		break;

	case 0x28:
	case 0x29:
		{
			strcpy(instruction,"movaps");
			strcpy(ptr,"dqword ptr");
			p_register = xmmregister;
			
			goto reg_mem;

		}
		break;

	case 0x2a:
	case 0x2b:
		{
			strcpy(instruction,b1&1?"movntps":"cvtpi2ps");
			strcpy(ptr,b1&1?"dqword ptr":"qword ptr");
			p_register = xmmregister;

			flag = 0;

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
			{
				strcpy(base,mmregister[b2&7]);
				optype_addmethod = b1&1?0x31:0x14;
			}
			else
				optype_addmethod = b1&1?0x01:0x11;
		}
		break;


	case 0x2c:
	case 0x2d:
		{
			strcpy(instruction,b1&1?"cvtps2pi":"cvttps2pi");
			strcpy(ptr,"dqword ptr");
			p_register = mmregister;

			flag = 0;

			length += mod_reg_rm(buff,p_register);

			if(((b2&7)==4)&(b2<0xc0))
				length = scale_index_base(buff,length);
			
			if(b2>=0xc0)
			{
				strcpy(base,xmmregister[b2&7]);
				optype_addmethod = 0x14;
			}
			else
				optype_addmethod = 0x11;
		}
		break;

	case 0x2e:
	case 0x2f:
		{
			strcpy(instruction,b1&1?"comiss":"ucomiss");
			strcpy(ptr,"dword ptr");
			p_register = xmmregister;

			goto reg_mem;
		}
		break;
	
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x36:
	case 0x37:
		{
			char *temp_array[8] = {"wrmsr","rdtsc","rdmsr","rdpmc","sysenter","sysexit","","getsec"};
			strcpy(instruction,temp_array[b1&7]);
			length +=1;
		}
		break;



	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4a:
	case 0x4b:
	case 0x4c:
	case 0x4d:
	case 0x4e:
	case 0x4f:
		{
			char *temp_array[16] = {
				"cmovo",
				"cmovno",
				"cmovb",
				"cmovnb",
				"cmove",
				"cmovne",
				"cmovbe",
				"cmova",
				"cmovs",
				"cmovns",
				"cmovpe",
				"cmovpo",
				"cmovl",
				"cmovge",
				"cmovle",
				"cmovg",
			};
			strcpy(instruction,temp_array[b2&0xf]);
			strcpy(ptr,"dword ptr");
			p_register = register32bitname;

			goto reg_mem;

		}
		break;

	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
	case 0x58:
	case 0x59:
	case 0x5a:
	case 0x5b:
	case 0x5c:
	case 0x5d:
	case 0x5e:
	case 0x5f:
		{
			char *temp_array[16] = {
				"",
				"sqrtps",
				"rsqrtps",
				"rcpps",
				"andps",
				"andnps",
				"orps",
				"xorps",
				"addps",
				"mulps",
				"cvtps2pd",
				"cvtps2pd",
				"subps",
				"minps",
				"divps",
				"maxps",
			};
			strcpy(instruction,temp_array[b2&0xf]);
			strcpy(ptr,b1==0x5a?"qword ptr":"dqword ptr");
			p_register = xmmregister;

			goto reg_mem;
		}
		break;

	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	case 0x68:
	case 0x69:
	case 0x6a:
	case 0x6b:
	case 0x6f:
		{
			char *temp_array[16] = {
				"punpcklbw",
				"punpcklwd",
				"punpckldq",
				"packsswb",
				"pcmpgtb",
				"pcmpgtw",
				"pcmpgtd",
				"packuswb",
				"punpckhbw",
				"punpckhwd",
				"punpckhdq",
				"packssdw",
				"",
				"",
				"movd",
				"movq",
			};
			strcpy(instruction,temp_array[b2&0xf]);
			strcpy(ptr,b1==0x6e?"dword ptr":"qword ptr");
			p_register = mmregister;

			goto reg_mem;
		}
		break;
	
	case 0x70:
		strcpy(instruction,"pshufw");
		strcpy(ptr,"qword ptr");
		p_register = mmregister;

reg_mem_8bimm:
		flag = 0;

		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&(b2<0xc0))
			length = scale_index_base(buff,length);

		if(b2>=0xc0)
		{
			strcpy(immediate,getimm(buff,2,1,1));
			optype_addmethod = 0x14;
		}
		else
		{
			if((b2&7)==4)
			{
				if((b3&7)==5)
				{
					strcpy(immediate,getimm(buff,7,1,1));
				}
				else
				{
					strcpy(immediate,getimm(buff,3,1,1));
				}
			}
			else if((b2&7)==5)
			{
				strcpy(immediate,getimm(buff,6,1,1));
			}
			else 
			{
				strcpy(immediate,getimm(buff,2,1,1));
			}
			optype_addmethod = 0x11;
		}
		length += 1;
		break;

	case 0x74:	
	case 0x75:	
	case 0x76:
		{
			char *temp_array[3]={"pcmpeqb","pcmpeqw","pcmpeqd"};
			strcpy(instruction,temp_array[b1&3]);
			strcpy(ptr,"qword ptr");
			p_register = mmregister;

			goto reg_mem;
		}
		break;


	case 0x77:
		strcpy(instruction,"emms");
		length+=1;
		break;

	case 0x78:
	case 0x79:
		strcpy(instruction,b1&1?"vmwrite":"vmread");
		strcpy(ptr,"dword ptr");
		p_register = register32bitname;

		flag = 0;

		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&(b2<0xc0))
			length = scale_index_base(buff,length);
		
		if(b2>=0xc0)
			optype_addmethod = (b1&1?0x14:0x31);
		else
			optype_addmethod = (b1&1?0x11:0x01);
		break;

	case 0x7e:
	case 0x7f:
		strcpy(instruction,b1&1?"movq":"movd");
		strcpy(ptr,b1&1?"qword ptr":"dword ptr");
		p_register = mmregister;

mem_reg:
		flag = 0;

		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&(b2<0xc0))
			length = scale_index_base(buff,length);
		
		if(b2>=0xc0)
			optype_addmethod = 0x31;
		else
			optype_addmethod = 0x01;
		break;


	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:
	case 0x88:
	case 0x89:
	case 0x8a:
	case 0x8b:
	case 0x8c:
	case 0x8d:
	case 0x8e:
	case 0x8f:
		optype_addmethod = 0x46;
		strcpy(instruction,jccinstrarray[b1&0x0f]);
		length += 5;
		break;

	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
	case 0x98:
	case 0x99:
	case 0x9a:
	case 0x9b:
	case 0x9c:
	case 0x9d:
	case 0x9e:
	case 0x9f:
		strcpy(ptr,"byte ptr");
		strcpy(instruction,setccinstrarray[b1&0xf]);
		p_register = register8bitname;

		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&(b2<0xc0))
			length = scale_index_base(buff,length);
	
		if(b2>=0xc0)
			optype_addmethod = 0x33;
		else
			optype_addmethod = 0x03;
		break;

	case 0xa0:
	case 0xa1:
	case 0xa8:
	case 0xa9:
		strcpy(instruction,b1&1?"pop":"push");
		strcpy(registername,((b1>>3)&1)?"gs":"fs");
		length += 1;
		break;
	
	case 0xa2:
	case 0xaa:
		strcpy(instruction,(b1>>3)&1?"rsm":"cpuid");
		length += 1;
		break;

	case 0xa3:
	case 0xab:
		strcpy(instruction,(b1>>3)&1?"bts":"bt");
		strcpy(ptr,"dword ptr");
		p_register = register32bitname;
		
		goto mem_reg;


	case 0xa4:
	case 0xac:
		strcpy(instruction,(b1>>3)&1?"shrd":"shld");
		strcpy(ptr,"dword ptr");
		p_register = register32bitname;
		goto mem_reg_8bimm;


mem_reg_8bimm:
		flag = 0;

		length += mod_reg_rm(buff,p_register);

		if(((b2&7)==4)&(b2<0xc0))
			length = scale_index_base(buff,length);

		if(b2>=0xc0)
		{
			strcpy(immediate,getimm(buff,2,1,1));
			optype_addmethod = 0x31;
		}
		else
		{
			if((b2&7)==4)
			{
				if((b3&7)==5)
				{
					strcpy(immediate,getimm(buff,7,1,1));
				}
				else
				{
					strcpy(immediate,getimm(buff,3,1,1));
				}
			}
			else if((b2&7)==5)
			{
				strcpy(immediate,getimm(buff,6,1,1));
			}
			else 
			{
				strcpy(immediate,getimm(buff,2,1,1));
			}
			optype_addmethod = 0x01;
		}
		break;


	case 0xa5:
	case 0xad:
		strcpy(instruction,(b1>>3)&1?"shrd":"shld");
		strcpy(ptr,"dword ptr");
		p_register = register32bitname;

		goto mem_reg;
	
	case 0xae://TODO:don't finish
		{
			char* temp_array[8] = {
				"fxsave",
				"fxrstor",
				"ldmxcsr",
				"stmxcsr",
				"xsave",
				"xrstor",
				"",
				"clflush",
			};
			strcpy(instruction,temp_array[(b2>>3)&7]);
			length+=1;
		}
		break;

	case 0xaf:
		strcpy(instruction,"imul");
		strcpy(ptr,"dword ptr");
		p_register = register32bitname;

		goto reg_mem;

	case 0xb0:
	case 0xb1:
		strcpy(instruction,"cmpxchg");

		goto mem_reg;

	case 0xb2:
		strcpy(instruction,"lss");
		strcpy(ptr,"fword ptr");
		p_register = register32bitname;

		goto reg_mem;

	case 0xb3:
		strcpy(instruction,"btr");

		goto mem_reg;

	case 0xb4:
	case 0xb5:
		strcpy(instruction,b1&1?"lgs":"lfs");
		strcpy(ptr,"fword ptr");
		p_register = register32bitname;

		goto reg_mem;

	case 0xb6:
	case 0xb7:
		strcpy(instruction,"movzx");
		strcpy(ptr,b1&1?"word ptr":"byte ptr");
		p_register = register32bitname;

		goto reg_mem;

	case 0xbb:
		strcpy(instruction,"btc");

		goto reg_mem;

	case 0xbc:
	case 0xbd:
		strcpy(instruction,b1&1?"bsr":"bsf");
		strcpy(ptr,"dword ptr");
		p_register = register32bitname;

		goto reg_mem;


	case 0xbe:
	case 0xbf:
		strcpy(instruction,"movsx");
		strcpy(ptr,b1&1?"word ptr":"byte ptr");
		p_register = register32bitname;

		goto reg_mem;

	case 0xc0:
	case 0xc1:
		strcpy(instruction,"movsx");
		strcpy(ptr,b1&1?"word ptr":"byte ptr");
		p_register = register32bitname;

		goto reg_mem;

	case 0xc2:
		strcpy(instruction,"cmpeqps");
		strcpy(ptr,"dqword ptr");
		p_register = xmmregister;

		goto reg_mem;

	case 0xc3:
		strcpy(instruction,"movnti");

		goto mem_reg;

	case 0xc4:
	case 0xc6:
		strcpy(instruction,b1==0xc4?"pinsrw":"shufps");
		strcpy(ptr,b1==0xc4?"word ptr":"dqword ptr");
		p_register = b1==0xc4?mmregister:xmmregister;
		
		goto reg_mem_8bimm;


	case 0xc8:
	case 0xc9:
	case 0xca:
	case 0xcb:
	case 0xcc:
	case 0xcd:
	case 0xce:
	case 0xcf:
		strcpy(instruction,"bswap");
		strcpy(registername,register32bitname[b1&7]);
	
		length +=1;
		optype_addmethod = 0x12;
		break;

	case 0xd1:
	case 0xd2:
	case 0xd3:
	case 0xd4:
	case 0xd5:
	case 0xd8:
	case 0xd9:
	case 0xda:
	case 0xdb:
	case 0xdc:
	case 0xdd:
	case 0xde:
	case 0xdf:
		{
			char *temp_array[0x10]={
				"",
				"psrlw",
				"psrld",
				"psrlq",
				"paddq",
				"pmullw",
				"",
				"",
				"psubusb",
				"psubusw",
				"pminub",
				"pand",
				"paddusb",
				"paddusw",
				"pmaxub",
				"pandn",
			};
			strcpy(instruction,temp_array[b1&7]);
			strcpy(ptr,"qword ptr");
			p_register = mmregister;

			goto reg_mem;
		}		

	case 0xe7:
		strcpy(instruction,"movntq");
		strcpy(ptr,"qword ptr");
		p_register = mmregister;

		goto mem_reg;

	case 0xe0:
	case 0xe1:
	case 0xe2:
	case 0xe3:
	case 0xe4:
	case 0xe5:
	case 0xe8:
	case 0xe9:
	case 0xea:
	case 0xeb:
	case 0xec:
	case 0xed:
	case 0xee:
	case 0xef:
		{
			char *temp_array[0x10] = {
				"pavgb",
				"psraw",
				"psrad",
				"pavgw",
				"pmulhuw",
				"pmulhw",
				"",
				"",
				"psubsb",
				"psubsw",
				"pminsw",
				"por",
				"paddsb",
				"paddsw",
				"pmaxsw",
				"pxor",
			};
			strcpy(instruction,temp_array[b1&7]);
			strcpy(ptr,"qword ptr");
			p_register = mmregister;

			goto reg_mem;
		}

	case 0xf1:
	case 0xf2:
	case 0xf3:
	case 0xf4:
	case 0xf5:
	case 0xf6:
	case 0xf8:
	case 0xf9:
	case 0xfa:
	case 0xfb:
	case 0xfc:
	case 0xfd:
	case 0xfe:
		{
			char *temp_array[0x10] = {
				"",
				"psllw",
				"pslld",
				"psllq",
				"pmuludq",
				"pmaddwd",
				"psadbw",
				"",
				"psubb",
				"psubw",
				"psubd",
				"psubq",
				"paddb",
				"paddw",
				"paddd",
				"",
			};
			strcpy(instruction,temp_array[b1&7]);
			strcpy(ptr,"qword ptr");
			p_register = mmregister;

			goto reg_mem;
		}



	default:
		length += 0;
		break;
	}

	return length;
}