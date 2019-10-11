#include "stdafx.h"
#include "assembler.h"




int assembler0f38(unsigned char *buff,char *instrname)
{
	int length = 0;

	char temp[] = "";
	unsigned char b1,b2,b3;
	b1 = buff[0];
	b2 = buff[1];
	b3 = buff[2];

	switch(b1)
	{
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
		{
			char* temp_array[0x10] = {
				"pshufb",
				"phaddw",
				"phaddd",
				"phaddsw",
				"pmaddubsw",
				"phsubw",
				"phsubd",
				"phsubsw",
				"psignb",
				"psignw",
				"psignd",
				"pmulhrsw",
				"",
				"",
				"",
				"",
			};
			strcpy(instruction,temp_array[b1&7]);
			strcpy(ptr,"qword ptr");
			p_register = mmregister;
			goto reg_mem;

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
	
	default:
		length = 0;
		break;
	}


	return length;
}


int assembler0f3a(unsigned char *buff,char *instrname)
{
	int length = 0;

	char temp[] = "";
	unsigned char b1,b2,b3;
	b1 = buff[0];
	b2 = buff[1];
	b3 = buff[2];

	switch(b1)
	{
	case 0x01:
		break;
	
	default:
		length = 0;
		break;
	}

	return length;
}