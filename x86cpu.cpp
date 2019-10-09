#include "stdafx.h"
#include "windows.h"
#include "malloc.h"



//--------------------------------------------------------------------------------------------------//

int _displacement = 0;
unsigned int _immediate = 0;
unsigned int null_pointer = 0;
unsigned int * _base = &null_pointer;
unsigned int * _registername = &null_pointer;
unsigned int * _index = &null_pointer;
unsigned char _scale = 1;



struct _x86Register
{
	unsigned int _register[8];
	unsigned int _eip;
	unsigned int _eflags;
};

//-------------------------------------------------------------------------------------------------//
int assembler(unsigned char *buff,char *instrname);
//-------------------------------------------------------------------------------------------------//



__declspec(naked) int test()
{
	__asm
	{
		ret 0x8;
	}
}


//-------------------------------------------------------------------------------------------------//
#if 0
int _set_eflags(_x86Register _cpu386,unsigned int before,unsigned int after)
{
	//cf
	unsigned char sign;
	sign = before
	_cpu386->_eflags = _cpu386->_eflags&(before > after ? 0xfffffffe:0xffffffff);
	//pf
	_cpu386->_eflags = _cpu386->_eflags&((*(unsigned char*)_base)%2 ? 0xfffffffb:0xffffffff);
	//af
	_cpu386->_eflags = _cpu386->_eflags&((unsigned char)temp_judge<*(unsigned char *)_base ? 0xffffffef:0xffffffff);
	//zf
	_cpu386->_eflags = _cpu386->_eflags&(*_base ? 0xffffffbf:0xffffffff);
	//sf
	_cpu386->_eflags = _cpu386->_eflags&((int)*_base>=0 ? 0xffffff7f:0xffffffff);
	//of
	_cpu386->_eflags = _cpu386->_eflags&( ? 0xffffff7f:0xffffffff);
	return 0;
}
#endif

 
#if 0
unsigned int _getimm(unsigned char * buff,int start,int size,int isunsigned)
{
	unsigned int imm = 0 ;
	for(int i = start+size-1;i>=start;i--)
	{
		imm = imm<<8;
		imm +=buff[i];
	}
	int sign;
	if(size == 1)
	{
		sign = imm&0x80;
		if(!isunsigned)
			imm = imm&0x7f;
	}
	else if (size == 4)
	{
		sign = imm&0x80000000;

		if(!isunsigned)
			imm = sign?(0-imm):imm;
	}else if(size == 2)
	{
		sign = imm&0x8000;

		if(isunsigned)
			imm = sign?(0-imm):imm;
	}
	return imm;
}
#else

int _getimm(unsigned char * buff,int start,int size,int isunsigned)
{
	if(!size)
		return 0;

	unsigned int imm = 0 ;
	for(int i = start+size-1;i>=start;i--)
	{
		imm = imm<<8;
		imm += buff[i];
	}
	if(size == 1)
	{
		imm = (char) imm;
	}
	else if( size == 2)
	{
		imm = (short) imm;
	}
	return (int)imm;
}

#endif

#if 1
int _mod_reg_rm(_x86Register *_cpu386,unsigned char data_size)//data_size: 0(8bit)  1(16bit)  2(32bit)
{
	int length = 2;
	unsigned char mod;
	unsigned char r_m;
	unsigned char reg;

	unsigned char * buff;
	buff = (unsigned char *)_cpu386->_eip;
	//skip prefix 0x66 and 0xf3 and 0x0f
	if(*buff == 0x66)
		buff++;
	if(*buff == 0xf2)
		buff++;
	if(*buff == 0x0f)
		buff++;

	mod = (buff[1]>>6)&3;
	r_m = buff[1]&7;
	reg = (buff[1]>>3)&7;
	int _immediate_size = 0;//0:no-_immediate; 1:8bit _immediate;  2:16bit _immediate; 4:32bit _immediate;

	if((buff[0]==0x80) || (buff[0]==0x82) || (buff[0]==0x83) || (buff[0]==0xc0) || (buff[0]==0xc1) || (buff[0]==0xc6) || (buff[0]==0xf6))
	{
		_immediate_size = 1;
	}
	else if((buff[0]==0x81) || (buff[0]==0xc7) || (buff[0]==0xf7))
	{
		if(data_size == 1)
			_immediate_size = 2;
		else
			_immediate_size = 4;
	}
	else
	{
		_immediate_size = 0;
		if(data_size == 0)
			*(unsigned char **)&_registername = ((unsigned char *)&_cpu386->_register[reg&3])+((reg>>2)&1);
		else if(data_size == 1)
			*(unsigned short **)&_registername = (unsigned short *)&_cpu386->_register[reg];
		else if(data_size == 2)
			_registername = &_cpu386->_register[reg];
	}
	if(buff[0]==0x6b)
		_immediate_size = 1;

	switch(mod)
	{		

	case 0:
		{

			if(r_m == 4)
			{
				_immediate = _getimm(buff,3,_immediate_size,1);
			}
			else if(r_m == 5)
			{
				_displacement = _getimm(buff,2,4,1);
				_immediate = _getimm(buff,6,_immediate_size,1);
				length += 4;
			}
			else
			{
				_base = &_cpu386->_register[r_m];
				_immediate = _getimm(buff,2,_immediate_size,1);
			}
		}
		break;
	case 1:
		{

			if(r_m == 4)
			{
				_displacement = _getimm(buff,3,1,0);
				_immediate = _getimm(buff,4,_immediate_size,0);
				length += 1;
			}
			else
			{
				_displacement = _getimm(buff,2,1,0);
				_immediate = _getimm(buff,3,_immediate_size,1);
				_base = &_cpu386->_register[r_m];
				length += 1;
			}
		}
		break;
	case 2:
		{
			if(r_m == 4)
			{
				_displacement = _getimm(buff,3,4,0);
				_immediate = _getimm(buff,7,_immediate_size,1);
				length += 4;
			}
			else
			{
				_displacement = _getimm(buff,2,4,0);
				_immediate = _getimm(buff,6,_immediate_size,1);
				_base = &_cpu386->_register[r_m];
				length += 4;
			}
		}
		break;
	case 3:
		{
			_immediate = _getimm(buff,2,_immediate_size,1);
			if(data_size == 0)
				*(unsigned char **)&_base = ((unsigned char *)&_cpu386->_register[r_m&3])+((r_m>>2)&1);
			else if(data_size == 1)
				*(unsigned short **)&_base = (unsigned short *)&_cpu386->_register[r_m];
			else if(data_size == 2)
				_base = &_cpu386->_register[r_m];
		}
		break;
	default:
		break;

	}
	length += _immediate_size;
	return length;
}

int _s_i_b(_x86Register *_cpu386,int length)
{
	unsigned char * buff;
	buff = (unsigned char *)_cpu386->_eip;

	//skip prefix 0x66 and 0xf3 and 0x0f
	if(*buff == 0x66)
		buff++;
	if(*buff == 0xf2)
		buff++;
	if(*buff == 0x0f)
		buff++;

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
			_displacement = _getimm(buff,3,4,0);
			length += 4;
		}
		else
			_base = &_cpu386->_register[5];
	}
	else
		_base = &_cpu386->_register[b];

	if(i != 4)
	{
		_index = &_cpu386->_register[i];
		unsigned char product = 1;
		for(int j = 1;j<=s;j++)
			product = 2*product;
		
		_scale = product;
	}
	
	return length+1;
}
#endif


//-------------------------------------------------------------------------------------------------//

int run(_x86Register *_cpu386)
{
	_displacement = 0;
	_immediate = 0;
	null_pointer = 0;
	_base = &null_pointer;
	_registername = &null_pointer;
	_index = &null_pointer;
	_scale = 1;


	int iLength;
	int i;
	unsigned char *_eip;
	unsigned char prefix[0x10] = {0};
	int _eflags_array[32] = {0};
	int _cf = 0;
	int _pf = 0;
	int _af = 0;
	int _zf = 0;
	int _sf = 0;
	int _tf = 0;
	int _if = 0;
	int _df = 0;
	int _of = 0;

	int prefix_66 = 0;

	_eip = (unsigned char*)_cpu386->_eip;


	iLength = 0;

	
	//set prefix
	int prefix_length = 0;
	if(*_eip == 0x66)
	{
		prefix_66 = 1;
		prefix[prefix_length] = 0x66;
		_eip++;
		prefix_length++;
	}
	if(*_eip == 0xf3)
	{
		prefix[prefix_length] = 0xf3;
		_eip++;
		prefix_length++;
	}

rep:
	switch(*_eip)
	{


	case 0x0f:
		{
			switch(_eip[1])
			{

			case 0x84://je
				iLength = 6;
				if(_cpu386->_eflags & 0x40)//zf=1
					_cpu386->_eip = (int)_eip + _getimm(_eip,2,4,0);
				break;
			
			case 0x85://jne
				iLength = 6;
				if(!(_cpu386->_eflags & 0x40))//zf=0
					_cpu386->_eip = (int)_eip + _getimm(_eip,2,4,0);
				break;

			case 0x87://ja
				iLength = 6;
				if(!(_cpu386->_eflags & 0x40) && !(_cpu386->_eflags & 0x01))//zf=0 and cf=0
					_cpu386->_eip = (int)_eip + _getimm(_eip,2,4,0);
				break;

			case 0x8c://jl
				iLength = 6;
				if(!(_cpu386->_eflags & 0x40) && ((_cpu386->_eflags & 0x800) != (_cpu386->_eflags & 0x80)))
					_cpu386->_eip = (int)_eip + _getimm(_eip,2,4,0);
				break;

			case 0x8d://jge
				iLength = 6;
				if((_cpu386->_eflags & 0x40) || ((_cpu386->_eflags & 0x800) == (_cpu386->_eflags & 0x80)))
					_cpu386->_eip = (int)_eip + _getimm(_eip,2,4,0);
				break;
			
			case 0x94://sete
				{
					iLength += 1;
					iLength += _mod_reg_rm(_cpu386,0);

					if(((_eip[2]&7)==4)&&(_eip[2]<0xc0))
						iLength = _s_i_b(_cpu386,iLength);

					if(_eip[2]>=0xc0)
						*(unsigned char *) _base = _cpu386->_eflags & 0x40 ? 1:0;
					else
						*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = _cpu386->_eflags & 0x40 ? 1:0;

				}
				break;

			case 0xbe:
			case 0xbf://movsx
				{
					iLength += 1;
					iLength += _mod_reg_rm(_cpu386,_eip[1] & 1? 1:0);

					if(((_eip[2]&7)==4)&&(_eip[2]<0xc0))
						iLength = _s_i_b(_cpu386,iLength);

					if(_eip[2]>=0xc0)
					{
						if(_eip[1] & 1)
							*_registername = (int)(*_base & 0xffff);
						else
							*_registername = (int)(*_base & 0xff);
					}
					else
					{
						if(_eip[1] & 1)
							*_registername = (int)(*(unsigned int *)(*_base + (*_index * _scale) + _displacement) & 0xffff);
						else
							*_registername = (int)(*(unsigned int *)(*_base + (*_index * _scale) + _displacement) & 0xff);
					}
				}
				break;
			default:
				iLength = 0;
				break;
			}


		}
		break;

	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
		{//add
			iLength += _mod_reg_rm(_cpu386,*_eip&1?2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned int before = 0;
			unsigned int after = 0;
			if(_eip[1]>=0xc0)
			{
				if((*_eip&0x3)<2)
				{
					if(*_eip&1)
					{
						before = *_base;
						*_base = *_base + *_registername;
						after = *_base;
					}	
					else
					{
						*(unsigned char *) &before = *(unsigned char *) _base;
						*(unsigned char *) _base = *(unsigned char *) _base + *(unsigned char *) _registername;
						*(unsigned char *) &after = *(unsigned char *) _base;
					}
				}
				else
				{
					if(*_eip&1)
					{
						before = *_registername;
						*_registername = *_registername + *_base;
						after = *_registername;
					}
					else
					{
						*(unsigned char *) &before = *(unsigned char *) _registername;
						*(unsigned char *) _registername = *(unsigned char *)_registername + *(unsigned char *)_base;
						*(unsigned char *) &after = *(unsigned char *) _registername;
					}
				}
			}
			else
			{
				if((*_eip&0x3)<2)
				{
					if(*_eip&1)
					{
						before = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) + *_registername;
						after = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
					}
					else
					{
						*(unsigned char *) &before = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) + *(unsigned char *)_registername;					
						*(unsigned char *) &after = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
					}
				}
				else
				{
					if(*_eip&1)
					{
						before = *_registername;
						*_registername = *_registername + *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						after = *_registername;
					}
					else
					{
						*(unsigned char *) &before = *(unsigned char *)_registername;
						*(unsigned char *)_registername = *(unsigned char *)_registername + *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *) &after = *(unsigned char *)_registername;
					}
				}
			}
			//| to 1 ,other bit set 0,dest set 1
			//& to 0 ,other bit set 1, dest set 0
			//cf
			_cpu386->_eflags = before > after ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;
			//pf

#if 0
			int i,j;
			int s = 1;
			for(i=0,j=0;i<8;i++)
			{
				if(((unsigned char)after)&s)
					j++;
				s = s*2;
			}
#else

			int pf;
			pf = (after ^ (after>>4));
			pf = (pf ^ (pf>>2));
			pf = 1^(1&(pf ^ (pf>>1)));

#endif
			_cpu386->_eflags = pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
			//af
			_cpu386->_eflags = (before&0xf) <= (after&0xf) ? _cpu386->_eflags&0xffffffef:_cpu386->_eflags|0x10;
			//zf
			_cpu386->_eflags = after ? _cpu386->_eflags&0xffffffbf:_cpu386->_eflags|0x40;

			if(*_eip & 1)
			{	
				if(0)					//TODO:fill prefix 0x66 component
				{//16bit
					//sf
					_cpu386->_eflags = (short)after>=0 ? _cpu386->_eflags&0xffffff7f:_cpu386->_eflags|0x80;
					//of
					_cpu386->_eflags = (before > after)^(((short)before&0x7fff) > ((short)after&0x7fff)) ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
				}
				else
				{//32bit	
					//sf
					_cpu386->_eflags = (int)after>=0 ? _cpu386->_eflags&0xffffff7f:_cpu386->_eflags|0x80;
					//of
					_cpu386->_eflags = (before > after)^((before&0x7fffffff) > (after&0x7fffffff)) ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
				}
			}
			else
			{//8bit
				//sf
				_cpu386->_eflags = (char)after>=0 ? _cpu386->_eflags&0xffffff7f:_cpu386->_eflags|0x80;
				//of
				_cpu386->_eflags = (before > after)^(((char)before&0x7f) > ((char)after&0x7f)) ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
			}		
		}
		break;

	case 0x08:
	case 0x0a://or
		{//8bit
			iLength += _mod_reg_rm(_cpu386,0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned char result;

			if(_eip[1]>=0xc0)
			{
				if(*_eip == 0x08)
				{
					*(unsigned char *)_base = *(unsigned char *)_base | *(unsigned char *)_registername;	
					result = *(unsigned char *)_base;		
				}
				else
				{
					*(unsigned char *)_registername = *(unsigned char *)_registername | *(unsigned char *)_base;
					result = *(unsigned char *)_registername;
				}

			}
			else
			{
				if(*_eip == 0x08)
				{
					*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) | *(unsigned char *)_registername;	
					result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
				}
				else
				{
					*(unsigned char *)_registername = *(unsigned char *)_registername | *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
					result = *(unsigned char *)_registername;
				}
			}
			//cf
			_cpu386->_eflags = _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _cpu386->_eflags&0xfffff7ff;

			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = (char)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		}
		break;


	case 0x09:
	case 0x0b://or
			iLength += _mod_reg_rm(_cpu386,2);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned int result;

			if(_eip[1]>=0xc0)
			{
				if(*_eip == 0x09)
				{
					*_base = *_base | *_registername;	
					result = *_base;		
				}
				else
				{
					*_registername = *_registername | *_base;
					result = *_registername;
				}

			}
			else
			{
				if(*_eip == 0x09)
				{
					*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) | *(unsigned int *)_registername;
					result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
				}
				else
				{
					*_registername = *_registername | *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
					result = *_registername;
				}
			}
			//cf
			_cpu386->_eflags = _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _cpu386->_eflags&0xfffff7ff;

			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = (int)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		break;

	
	case 0x0c:
	case 0x0d://or
		{
			iLength = *_eip&1 ? 5:2; 
			
			if(*_eip & 1)
			{
				_cpu386->_register[0] = _cpu386->_register[0] | _getimm(_eip,1,4,1);
				result = (int)_cpu386->_register[0];
			}
			else
			{
				*(unsigned char *)&_cpu386->_register[0] = (unsigned char)_cpu386->_register[0] | _getimm(_eip,1,4,1);
				result = (char)_cpu386->_register[0];
			}
			//cf
			_cpu386->_eflags = _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _cpu386->_eflags&0xfffff7ff;

			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = (int)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		}
		break;


	case 0x18:
	case 0x19:
	case 0x1a:
	case 0x1b://sbb
		{

			iLength += _mod_reg_rm(_cpu386,*_eip&1 ? 2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned int operand1 = 0;
			unsigned int operand2 = 0;
			unsigned int result = 0;
			unsigned int cf_operand3 = _cpu386->_eflags & 0x01;
			if(_eip[1]>=0xc0)
			{
				if(*_eip&0x2)
				{
					if(*_eip&1)
					{
						operand1 = *_registername;
						operand2 = *_base;
						*_registername = *_registername - (*_base + cf_operand3);
						result = *_base;
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
					}	
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *) _registername;
						*(unsigned char *) &operand2 = *(unsigned char *) _base;
						*(unsigned char *) _registername = *(unsigned char *) _registername - (*(unsigned char *)_base + cf_operand3);	
						*(unsigned char *) &result = *(unsigned char *) _registername;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
				else
				{
					if(*_eip&1)
					{
						operand1 = *_base;
						operand2 = *_registername;
						*_base = *_base - (*_registername + cf_operand3);	
						result = *_base;
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
					}	
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *) _base;
						*(unsigned char *) &operand2 = *(unsigned char *) _registername;
						*(unsigned char *) _base = *(unsigned char *) _base - (*(unsigned char*)_registername + cf_operand3);	
						*(unsigned char *) &result = *(unsigned char *) _base;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
			}
			else
			{
				if(*_eip&2)
				{
					if(*_eip&1)
					{
						operand1 = *_registername;
						operand2 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						*_registername = *_registername - (*(unsigned int *)(*_base + (*_index * _scale) + _displacement) + cf_operand3);
						result = *_registername;
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
						
					}
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *)_registername;
						*(unsigned char *) &operand2 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *) _registername = *(unsigned char *) _registername - (*(unsigned char *)(*_base + (*_index * _scale) + _displacement) + cf_operand3);
						*(unsigned char *) &result = *(unsigned char *) _registername;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
				else
				{
					if(*_eip&1)
					{
						operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						operand2 = *_registername;
						*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) - (*_registername + cf_operand3);
						result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
						
					}
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *) &operand2 = *(unsigned char *)_registername;
						*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) - (*(unsigned char *)_registername + cf_operand3);
						*(unsigned char *) &result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
				
			}
			_cf = operand2 > operand1 ? 1:0;
			_af = (operand2&0xf) > (operand1&0xf)? 1:0;
			_zf = !result?1:0;
			if(*_eip&1)
				_sf = (int)result<0 ? 1:0;
			else
				_sf = (char)result<0 ? 1:0;
			_of = _cf ^ _of;

			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
				
			
				
			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;
			//pf
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
			//af
			_cpu386->_eflags = _af ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;
			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
		}
		break;


	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23://and
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&1 ?2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned int result;

			if(_eip[1]>=0xc0)
			{
				if(*_eip&0x2)
				{
					if(*_eip&1)
					{
						*_registername = *_registername & *_base;	
						result = *_registername;		
					}
					else
					{
						 *(unsigned char *)_registername = *(unsigned char *)_registername & *(unsigned char *)_base;	
						result = *(unsigned char *)_registername;		
					}
					
				}
				else
				{
					if(*_eip&1)
					{
						*_base = *_base & *_registername;
						result = *_base;
					}
					else
					{
						*(unsigned char *)_base = *(unsigned char *)_base & *(unsigned char *)_registername;	
						result = *(unsigned char *)_base;		
					}
				}

			}
			else
			{
				if(*_eip & 0x02)
				{
					if(*_eip & 1)
					{
						*_registername = *_registername & *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						result = *_registername;
					}
					else
					{
						*(unsigned char *)_registername = *(unsigned char *)_registername & *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						result = *(unsigned char *)_registername;
					}
					
				}
				else
				{
					if(*_eip & 1)
					{
						*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) & *_registername;
						result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
					}
					else
					{
						*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) & *(unsigned char *)_registername;
						result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
					}
				}
			}
			//cf
			_cpu386->_eflags = _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _cpu386->_eflags&0xfffff7ff;

			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = (int)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		}
		break;









	case 0x24://and
		{
			*(unsigned char *)&_cpu386->_register[0] = *(unsigned char *)&_cpu386->_register[0] & (unsigned char)_getimm(_eip,1,4,1);
			unsigned int result = *(unsigned char *)&_cpu386->_register[0];
			iLength = 1;
			_cf = 0;
			_zf = !result?1:0;
			_sf = (char)result<0 ? 1:0;
			_of = 0;
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;
			//pf
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
		}
		break;
	case 0x25://and
		{
			_cpu386->_register[0] = _cpu386->_register[0] & _getimm(_eip,1,4,1);
			unsigned int result = _cpu386->_register[0];
			iLength = 5;
			_cf = 0;
			_zf = !result?1:0;
			_sf = (int)result<0 ? 1:0;
			_of = 0;
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;
			//pf
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
		}
		break;





	case 0x28:
	case 0x29:
	case 0x2a:
	case 0x2b://sub
		{

			iLength += _mod_reg_rm(_cpu386,*_eip&1 ? 2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned int operand1 = 0;
			unsigned int operand2 = 0;
			unsigned int result = 0;
			if(_eip[1]>=0xc0)
			{
				if(*_eip&0x2)
				{
					if(*_eip&1)
					{
						operand1 = *_registername;
						operand2 = *_base;
						*_registername = *_registername - *_base;	
						result = *_base;
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
					}	
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *) _registername;
						*(unsigned char *) &operand2 = *(unsigned char *) _base;
						*(unsigned char *) _registername = *(unsigned char *) _registername - *(unsigned char *)_base;	
						*(unsigned char *) &result = *(unsigned char *) _registername;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
				else
				{
					if(*_eip&1)
					{
						operand1 = *_base;
						operand2 = *_registername;
						*_base = *_base - *_registername;	
						result = *_base;
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
					}	
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *) _base;
						*(unsigned char *) &operand2 = *(unsigned char *) _registername;
						*(unsigned char *) _base = *(unsigned char *) _base - *(unsigned char*)_registername;	
						*(unsigned char *) &result = *(unsigned char *) _base;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
			}
			else
			{
				if(*_eip&2)
				{
					if(*_eip&1)
					{
						operand1 = *_registername;
						operand2 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						*_registername = *_registername - *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						result = *_registername;
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
						
					}
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *)_registername;
						*(unsigned char *) &operand2 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *) _registername = *(unsigned char *) _registername - *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *) &result = *(unsigned char *) _registername;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
				else
				{
					if(*_eip&1)
					{
						operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						operand2 = *_registername;
						*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) - *_registername;
						result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
						
					}
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *) &operand2 = *(unsigned char *)_registername;
						*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) - *(unsigned char *)_registername;
						*(unsigned char *) &result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
				
			}
			_cf = operand2 > operand1 ? 1:0;
			_af = (operand2&0xf) > (operand1&0xf)? 1:0;
			_zf = !result?1:0;
			_sf = (int)result<0 ? 1:0;
			_of = _cf ^ _of;

			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
				
			
				
			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;
			//pf
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
			//af
			_cpu386->_eflags = _af ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;
			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
		}
		break;

	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33://xor
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&1 ?2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned int result;

			if(_eip[1]>=0xc0)
			{
				if(*_eip&0x2)
				{
					if(*_eip&1)
					{
						*_registername = *_registername ^ *_base;	
						result = *_registername;		
					}
					else
					{
						 *(unsigned char *)_registername = *(unsigned char *)_registername ^ *(unsigned char *)_base;	
						result = *(unsigned char *)_registername;		
					}
					
				}
				else
				{
					if(*_eip&1)
					{
						*_base = *_base ^ *_registername;
						result = *_base;
					}
					else
					{
						*(unsigned char *)_base = *(unsigned char *)_base ^ *(unsigned char *)_registername;	
						result = *(unsigned char *)_base;		
					}
				}

			}
			else
			{
				if(*_eip & 0x02)
				{
					if(*_eip & 1)
					{
						*_registername = *_registername ^ *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						result = *_registername;
					}
					else
					{
						*(unsigned char *)_registername = *(unsigned char *)_registername ^ *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						result = *(unsigned char *)_registername;
					}
					
				}
				else
				{
					if(*_eip & 1)
					{
						*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) ^ *_registername;
						result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
					}
					else
					{
						*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) ^ *(unsigned char *)_registername;
						result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
					}
				}
			}
			//cf
			_cpu386->_eflags = _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _cpu386->_eflags&0xfffff7ff;

			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = (int)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		}
		break;


	case 0x38:
	case 0x39:
	case 0x3a:
	case 0x3b://cmp
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&1 ?2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);


			unsigned int operand1 = 0;
			unsigned int operand2 = 0;
			unsigned int result = 0;
			if(_eip[1]>=0xc0)
			{
				if(*_eip&2)
				{
					if(*_eip&1)
					{
						operand1 = *_registername;
						operand2 = *_base;
						result = *_registername - *_base;	
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
					}	
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *) _registername;
						*(unsigned char *) &operand2 = *(unsigned char *) _base;
						
						*(unsigned char *) &result = *(unsigned char *)_registername - *(unsigned char *)_base;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
				else
				{
					if(*_eip&1)
					{
						operand1 = *_base;
						operand2 = *_registername;
						result = *_base - *_registername;	
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
					}	
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *) _base;
						*(unsigned char *) &operand2 = *(unsigned char *)_registername;
						*(unsigned char *) &result = *(unsigned char *) _base - *(unsigned char *)_registername;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}

				}
			}
			else
			{
				if(*_eip&2)
				{
					if(*_eip&1)
					{
						operand1 = *_registername;
						operand2 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						
						result = *_registername - *(unsigned int *)(*_base + (*_index * _scale) + _displacement) ;
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
						
					}
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *)_registername;
						*(unsigned char *) &operand2 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						
						*(unsigned char *) &result = *(unsigned char *)_registername - *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}
				else
				{
					if(*_eip&1)
					{
						operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						operand2 = *_registername;
						result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) - *_registername;
						_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
						
					}
					else
					{
						*(unsigned char *) &operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *) &operand2 = *(unsigned char *)_registername;
						*(unsigned char *) &result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) - *(unsigned char *)_registername;
						_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
					}
				}

				
			}
			_cf = operand2 > operand1 ? 1:0;
			_af = (operand2&0xf) > (operand1&0xf)? 1:0;
			_zf = !result?1:0;
			_sf = (int)result<0 ? 1:0;
			_of = _cf ^ _of;

			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;

			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));

			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;

			//af
			_cpu386->_eflags = _af ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;

			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;

			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
		}
		break;


	case 0x3c:
	case 0x3d://cmp
		{
			iLength = *_eip&1 ? 5:2;


			unsigned int operand1 = 0;
			unsigned int operand2 = 0;
			unsigned int result = 0;


			if(*_eip&1)
			{
				operand1 = _cpu386->_register[0];
				operand2 = _getimm(_eip,1,4,1);
				result = operand1 - operand2;	
				_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
				_sf = (int)result<0 ? 1:0;
			}	
			else
			{
				operand1 = (unsigned char) _cpu386->_register[0];
				operand2 = _getimm(_eip,1,1,1);	
				result = (unsigned char)operand1 - (unsigned char)operand2;
				_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
				_sf = (char)result<0 ? 1:0;
			}


			_cf = operand2 > operand1 ? 1:0;
			_af = (operand2&0xf) > (operand1&0xf)? 1:0;
			_zf = !result?1:0;	
			_of = _cf ^ _of;

			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;

			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));

			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;

			//af
			_cpu386->_eflags = _af ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;

			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;

			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
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
		{//inc
			iLength = 1;
			unsigned int operand1 = _cpu386->_register[*_eip&0x7];
			_cpu386->_register[*_eip&0x7] += 1;
			unsigned int result = _cpu386->_register[*_eip&0x7];

			//cf is not affected
			_cf = _cpu386->_eflags&1;
			//of
			_cpu386->_eflags = _cf^((int)operand1 > (int)result) ? _cpu386->_eflags|0x800:_cpu386->_eflags&0xfffff7ff;

			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = (int)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;

			//af
			_cpu386->_eflags = (result&0xf) < (operand1&0xf) ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		}
		break;

	case 0x48:
	case 0x49:
	case 0x4a:
	case 0x4b:
	case 0x4c:
	case 0x4d:
	case 0x4e:
	case 0x4f:
		{//dec
			iLength = 1;
			unsigned int operand1 = _cpu386->_register[*_eip&0x7];
			_cpu386->_register[*_eip&0x7] -= 1;
			unsigned int result = _cpu386->_register[*_eip&0x7];

			//cf is not affected
			_cf = _cpu386->_eflags&1;
			//of
			_cpu386->_eflags = _cf^((int)operand1 < 1 ) ? _cpu386->_eflags|0x800:_cpu386->_eflags&0xfffff7ff;

			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = (int)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;

			//af
			_cpu386->_eflags = (result&0xf) < (operand1&0xf) ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		}
		break;

	case 0x50:
	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
		_cpu386->_register[4] -= 4;
		*(unsigned int*)_cpu386->_register[4] = _cpu386->_register[*_eip&0x7];
		iLength =1;
		break;


	case 0x58:
	case 0x59:
	case 0x5a:
	case 0x5b:
	case 0x5c:
	case 0x5d:
	case 0x5e:
	case 0x5f:
		_cpu386->_register[*_eip&0x7] = *(unsigned int*)_cpu386->_register[4];
		_cpu386->_register[4] += 4;
		iLength =1;
		break;


	case 0x68://push 32imm
		_cpu386->_register[4] -= 4;
		*(unsigned int*)_cpu386->_register[4] = _getimm(_eip,1,4,0);
		iLength += 5;
		break;

	case 0x6a://push 8imm
		_cpu386->_register[4] -= 4;
		*(unsigned int*)_cpu386->_register[4] = (unsigned char)_getimm(_eip,1,1,0);
		iLength += 2;
		break;

	case 0x6b://imul
		{
			unsigned int operand1 = 0;
			unsigned int operand2 = 0;
			unsigned int result = 0;

			iLength += _mod_reg_rm(_cpu386,2);

			if(((_eip[1]&7)==4) && (_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			if(_eip[1] >= 0xc0)
			{
				operand1 = *_base;
				operand2 = _immediate;
				*_registername = *_base * _immediate;
				result = *_registername;
			}
			else
			{
				*_registername = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) * _immediate;
				result = *_registername;
			}
			_sf = (int)result < 0 ? 1:0;
			_cf = (result >= operand1) && (result >= operand2) ? 0:1;
			_of = _cf ^ !(((int)result >= (int)operand1) && ((int)result >= (int)operand2)) ? 1:0;
			
			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;

			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;

			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	
			
			
		}
		break;

	case 0x72://jb
		iLength = 2;
		if(_cpu386->_eflags & 0x01)
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x73://jnb
		iLength = 2;
		if(!(_cpu386->_eflags & 0x01))
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x74://je
		iLength = 2;
		if(_cpu386->_eflags & 0x40)
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x75://jnz
		iLength = 2;
		if(!(_cpu386->_eflags & 0x40))
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x76://jbe
		iLength = 2;
		if((_cpu386->_eflags & 0x40) || (_cpu386->_eflags & 0x01))
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x77://jnbe
		iLength = 2;
		if(!(_cpu386->_eflags & 0x40) && !(_cpu386->_eflags & 0x01))
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x7c://jl
		iLength = 2;
		if(!(_cpu386->_eflags & 0x40) && ((_cpu386->_eflags & 0x800) != (_cpu386->_eflags & 0x80)))
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x7d://jge
		iLength = 2;
		if((_cpu386->_eflags & 0x40) || ((_cpu386->_eflags & 0x800) == (_cpu386->_eflags & 0x80)))
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x7e://jle
		iLength = 2;
		if((_cpu386->_eflags & 0x40) || ((_cpu386->_eflags & 0x800) != (_cpu386->_eflags & 0x80)))
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;

	case 0x7f://jg
		iLength = 2;
		if(!(_cpu386->_eflags & 0x40) && ((_cpu386->_eflags & 0x800) == (_cpu386->_eflags & 0x80)))
			_cpu386->_eip = (int)_eip + (char)_getimm(_eip,1,1,0);
		break;


	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&1? 2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);
			
			switch((_eip[1]>>3)&7)
			{
			case 0x00://add
				{
					unsigned int operand1 = 0;
					unsigned int operand2 = 0;
					unsigned int result = 0;
					if(_eip[1]>=0xc0)
					{
						if(*_eip&1)
						{
							operand1 = *_base;
							operand2 = _immediate;
							*_base = *_base + _immediate;	
							result = *_base;
							
						}	
						else
						{
							*(unsigned char *) &operand1 = *(unsigned char *) _base;
							*(unsigned char *) &operand2 = (unsigned char)_immediate;
							*(unsigned char *) _base = *(unsigned char *) _base + (unsigned char)_immediate;	
							*(unsigned char *) &result = *(unsigned char *) _base;
							
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							operand2 = _immediate;
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) + _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							
							
						}
						else
						{
							*(unsigned char *) &operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned char *) &operand2 = (unsigned char)_immediate;
							*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) + (unsigned char)_immediate;
							*(unsigned char *) &result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							
						}
					}
					_cf = operand1 > result ? 1:0;
					_af = (operand2&0xf) > (operand1&0xf)? 1:0;
					_zf = !result?1:0;
					_sf = (int)result<0 ? 1:0;
					_of = _cf ^ ((int)operand1 > (int)result) ? 1:0;
				}
				break;
			case 0x01://or
				{
					unsigned int operand1 = 0;
					unsigned int operand2 = 0;
					unsigned int result = 0;
					if(_eip[1]>=0xc0)
					{
						if(*_eip&1)
						{
							*_base = *_base | _immediate;	
							result = *_base;
						}	
						else
						{
							*(unsigned char *) _base = *(unsigned char *) _base | (unsigned char)_immediate;	
							*(unsigned char *) &result = *(unsigned char *) _base;				
						}
					}
					else
					{
						if(*_eip&1)
						{
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) | _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);						
						}
						else
						{
							*(unsigned char *) (*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) | (unsigned char)_immediate;
							*(unsigned char *) &result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						}
					}
					_cf = 0;
					_of = 0;
					_zf = !result?1:0;
					_sf = (int)result<0 ? 1:0;
				}
				break;
			case 0x02://adc
				iLength = 0;
				break;
			case 0x03://sbb
				iLength = 0;
				break;
			case 0x04://and
				{
					unsigned int operand1 = 0;
					unsigned int operand2 = 0;
					unsigned int result = 0;
					if(_eip[1]>=0xc0)
					{
						if(*_eip&1)
						{
							operand1 = *_base;
							operand2 = _immediate;
							*_base = *_base & _immediate;	
							result = *_base;
						}	
						else
						{
							*(unsigned char *) &operand1 = *(unsigned char *) _base;
							*(unsigned char *) &operand2 = (unsigned char)_immediate;
							*(unsigned char *) _base = *(unsigned char *) _base & (unsigned char)_immediate;	
							*(unsigned char *) &result = *(unsigned char *) _base;
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							operand2 = _immediate;
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) & _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						}
						else
						{
							*(unsigned char *) &operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned char *) &operand2 = (unsigned char)_immediate;
							*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) & (unsigned char)_immediate;
							*(unsigned char *) &result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						}
					}
					_cf = 0;
					_af = 0;
					_zf = !result?1:0;
					_sf = (int)result<0 ? 1:0;
					_of = 0;
				}
				break;
			case 0x05://sub
				{
					unsigned int operand1 = 0;
					unsigned int operand2 = 0;
					unsigned int result = 0;
					if(_eip[1]>=0xc0)
					{
						if(*_eip&1)
						{
							operand1 = *_base;
							operand2 = _immediate;
							*_base = *_base - _immediate;	
							result = *_base;
							_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
						}	
						else
						{
							*(unsigned char *) &operand1 = *(unsigned char *) _base;
							*(unsigned char *) &operand2 = (unsigned char)_immediate;
							*(unsigned char *) _base = *(unsigned char *) _base - (unsigned char)_immediate;	
							*(unsigned char *) &result = *(unsigned char *) _base;
							_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							operand2 = _immediate;
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) - _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
							
						}
						else
						{
							*(unsigned char *) &operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned char *) &operand2 = (unsigned char)_immediate;
							*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) - (unsigned char)_immediate;
							*(unsigned char *) &result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
						}
					}
					_cf = operand2 > operand1 ? 1:0;
					_af = (operand2&0xf) > (operand1&0xf)? 1:0;
					_zf = !result?1:0;
					_sf = (int)result<0 ? 1:0;
					_of = _cf ^ _of;
				}
				break;
			case 0x06://xor
				{
					unsigned int result;

					if(_eip[1]>=0xc0)
					{
						if(*_eip & 1)
						{
							*_base = *_base ^ _immediate;
							result = *_base;
						}
						else
						{
							*(unsigned char *)_base = *(unsigned char *)_base ^ (unsigned char)_immediate;	
							result = *(unsigned char *)_base;		
						}
					}
					else
					{
						if(*_eip & 1)
						{
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) ^ _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						}
						else
						{
							*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) ^ (unsigned char)_immediate;
							result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						}
						
					}		
					
					_cf = 0;
					_of = 0;
					_zf = result == 0? 1:0;
					_sf = (int)result<0 ? 1:0;			
				}
				break;
			case 0x07://cmp
				{
					unsigned int operand1 = 0;
					unsigned int operand2 = 0;
					unsigned int result = 0;
					if(_eip[1]>=0xc0)
					{
						if(*_eip&1)
						{
							operand1 = *_base;
							operand2 = _immediate;
							result = *_base - _immediate;	
							_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
						}	
						else
						{
							*(unsigned char *) &operand1 = *(unsigned char *) _base;
							*(unsigned char *) &operand2 = (unsigned char)_immediate;
							*(unsigned char *) &result = *(unsigned char *) _base - (unsigned char)_immediate;
							_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							operand2 = _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) - _immediate;
							_of = ((int)operand2&0x7fffffff) > ((int)operand1&0x7fffffff) ? 1:0;
							
						}
						else
						{
							*(unsigned char *) &operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned char *) &operand2 = (unsigned char)_immediate;
							*(unsigned char *) &result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) - (unsigned char)_immediate;					
							_of = ((char)operand2&0x7f) > ((char)operand1&0x7f) ? 1:0;
						}
					}
					_cf = operand2 > operand1 ? 1:0;
					_af = (operand2&0xf) > (operand1&0xf)? 1:0;
					_zf = !result?1:0;
					_sf = (int)result<0 ? 1:0;
					_of = _cf ^ _of;
				}
				break;
			}
				
			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;

			//pf
			
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));

			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;

			//af
			_cpu386->_eflags = _af ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;

			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;

			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;	

		}
		break;

	case 0x84:
	case 0x85://test
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&1? 2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);
			
			unsigned int result = 0;

			if(_eip[1]>=0xc0)
			{
				if(*_eip & 1)
				{
					result = *_base & *_registername;
				}
				else
				{
					result = *(unsigned char *)_base & *(unsigned char *)_registername;
				}
			}
			else
			{
				if(*_eip & 1)
				{
					result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) & *_registername;
				}
				else
				{
					*(unsigned char *)&result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) & *(unsigned char *)_registername;
				}
			}

			//cf
			_cpu386->_eflags = _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _cpu386->_eflags&0xfffff7ff;

			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			if(*_eip & 1)
				_cpu386->_eflags = (int)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			else
				_cpu386->_eflags = (char)result<0 ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		}
		break;




	case 0x88:
	case 0x89:
	case 0x8a:
	case 0x8b:
		{
			if(prefix_66)
				iLength += _mod_reg_rm(_cpu386,1);
			else
				iLength += _mod_reg_rm(_cpu386,*_eip&1?2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			if(_eip[1]>=0xc0)
			{
				if((*_eip&0x3)<2)
				{
					if(*_eip&1)
					{
						if(prefix_66)
							*(unsigned short *)_base = *(unsigned short *)_registername;
						else
							*_base = *_registername;

					}
					else
						*(unsigned char *) _base = *(unsigned char *) _registername;
				}
				else
				{
					if(*_eip&1)
					{
						if(prefix_66)
							*(unsigned short *)_registername = *(unsigned short *)_base;
						else
							*_registername = *_base;
					}
					else
						*(unsigned char *)_registername = *(unsigned char *)_base;
				}
			}
			else
			{
				if((*_eip&0x3)<2)
				{
					if(*_eip&1)
					{
						if(prefix_66)
						{
							*(unsigned short *)(*_base + (*_index * _scale) + _displacement) = *(unsigned short *)_registername;
						}
						else
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *_registername;
						
					}
					else
						*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)_registername;					
				}
				else
				{
					if(*_eip&1)
					{
						if(prefix_66)
						{
							*(unsigned short *)_registername = *(unsigned short *)(*_base + (*_index * _scale) + _displacement);
						}
						else
						{
							*_registername = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						}
					}
					else
						*(unsigned char *)_registername = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
				}
			}
		}
		break;


	case 0x8d:
		{
			iLength += _mod_reg_rm(_cpu386,2);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			if(_eip[1]>=0xc0)
			{
				*_registername = *_base;		
			}
			else
			{
				*_registername = *_base + (*_index * _scale) + _displacement;
			}
		}
		break;

	case 0x99://cdq
		{
			iLength = 1;
			_cpu386->_register[2] = (int)_cpu386->_register[0] < 0 ? 0xffffffff:0;
		}
		break;



	case 0xa0:
		*(unsigned char *)&_cpu386->_register[0] = *(unsigned char *)_getimm(_eip,1,4,1);
		iLength = 5;
		break;
	case 0xa1:
		*(unsigned int *)&_cpu386->_register[0] = *(unsigned int *)_getimm(_eip,1,4,1);
		iLength = 5;
		break;
	case 0xa2:
		*(unsigned char *)_getimm(_eip,1,4,1) = *(unsigned char *)&_cpu386->_register[0];
		iLength = 5;
		break;
	case 0xa3:
		*(unsigned int *)_getimm(_eip,1,4,1) = *(unsigned int *)&_cpu386->_register[0];
		iLength = 5;
		break;

	case 0xa8:
	case 0xa9://test
		{
			unsigned int result = 0;
			
			if(*_eip & 1)			
			{
				result = _cpu386->_register[0] &  _getimm(_eip,1,4,1);	
				_sf = (int)result < 0 ? 1:0;
				iLength = 5;
			}
			else			
			{
				result = (unsigned char)_cpu386->_register[0] & (unsigned char)_getimm(_eip,1,1,1);	
				_sf = (char)result < 0 ? 1:0;
				iLength = 2;
			}
			//cf
			_cpu386->_eflags = _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _cpu386->_eflags&0xfffff7ff;
			//zf
			_cpu386->_eflags = result == 0 ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
		}
		break;


	case 0xaa:
		{
			*(unsigned char *) _cpu386->_register[7] = (unsigned char)_cpu386->_register[0];
			_cpu386->_register[7] = _cpu386->_register[7]+1;
			iLength = 1;
		}
		break;

	case 0xab:
		{
			if(prefix_66)
			{
				*(unsigned short *) _cpu386->_register[7] = (unsigned short)_cpu386->_register[0];
				_cpu386->_register[7] = _cpu386->_register[7]+2;
			}
			else
			{
				*(unsigned int *) _cpu386->_register[7] = _cpu386->_register[0];
				_cpu386->_register[7] = _cpu386->_register[7]+4;
			}
			iLength = 1;
		}
		break;

	case 0xb8:
	case 0xb9:
	case 0xba:
	case 0xbb:
	case 0xbc:
	case 0xbd:
	case 0xbe:
	case 0xbf:
		_cpu386->_register[*_eip&0x7] = *(unsigned int*)(_eip+1);
		iLength = 5;
		break;


	case 0xc0:
	case 0xc1:
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&0x02 ? 2:1);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned int result = 0;
			unsigned int operand1 = 0;
			unsigned int operand2 = 0;

			switch((_eip[1]>>3)&0x7)
			{
			case 0x0://rol
				iLength = 0;
				break;
			case 0x1://ror
				{
					iLength = 0;

				}
				break;
			case 0x2://rcl
				iLength = 0;
				break;
			case 0x3://rcr
				iLength = 0;
				break;
			case 0x4://shl
				{
					if(_eip[1] >= 0xc0)
					{

						if(*_eip&1)
						{
							operand1 = *_base;
							*_base = *_base << _immediate;
							result = *_base;
							_cf = (operand1 << (_immediate-1))&0x7fffffff ? 1:0;
						}
						else
						{
							operand1 = *(unsigned char *)_base;
							*(unsigned char *)_base = *(unsigned char *)_base << _immediate;
							result = *(unsigned char *)_base;
							_cf = (operand1 << (_immediate-1))&0x7f ? 1:0;
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) << _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							_cf = (operand1 << (_immediate-1))&0x7fffffff ? 1:0;
						}
						else
						{
							operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) << _immediate;
							result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							_cf = (operand1 << (_immediate-1))&0x7f ? 1:0;
						}
					}
					_sf = (int)result < 0 ? 1:0;
					_zf = !result ? 1:0;
					_of = _cf ^ ((int)result < 0) ? 1:0;
				}
				break;
			case 0x5://shr
				{

					if(_eip[1] >= 0xc0)
					{

						if(*_eip&1)
						{
							operand1 = *_base;
							*_base = *_base >> _immediate;
							result = *_base;
							
						}
						else
						{
							operand1 = *(unsigned char *)_base;
							*(unsigned char *)_base = *(unsigned char *)_base >> _immediate;
							result = *(unsigned char *)_base;
							
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) >> _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							
						}
						else
						{
							operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) >> _immediate;
							result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							
						}
					}
					_cf = (operand1 >> (_immediate-1))&1 ? 1:0;
					_sf = (int)result < 0 ? 1:0;
					_zf = !result ? 1:0;
					_of = _cf ^ ((int)result > (int)operand1) ? 1:0;
				}
				break;
			case 0x6://sal
				iLength = 0;
				break;
			case 0x7://sar
				{
					if(_eip[1] >= 0xc0)
					{

						if(*_eip&1)
						{
							operand1 = *_base;
							*_base = (int)*_base >> _immediate;
							result = *_base;
							
						}
						else
						{
							operand1 = (char)(*(unsigned char *)_base);
							*(char *)_base = *(char *)_base >> _immediate;
							result = *(unsigned char *)_base;
							
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							*(int *)(*_base + (*_index * _scale) + _displacement) = *(int *)(*_base + (*_index * _scale) + _displacement) >> _immediate;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							
						}
						else
						{
							operand1 =(char)(*(unsigned char *)(*_base + (*_index * _scale) + _displacement));
							*(char *)(*_base + (*_index * _scale) + _displacement) = *(char *)(*_base + (*_index * _scale) + _displacement) >> _immediate;
							result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							
						}
					}
					_cf = (operand1 >> (_immediate-1))&1 ? 1:0;
					_sf = (int)result < 0 ? 1:0;
					_zf = !result ? 1:0;
					if(_immediate == 1)
						_of = 0;
				}
				break;

			};

			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;
			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
			//af
			_cpu386->_eflags = _af ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;
		}

		break;


	case 0xc2://ret
		iLength = 3;
		_cpu386->_eip = *(unsigned int*)(_cpu386->_register[4]);
		_cpu386->_register[4] += (4 + _getimm(_eip,1,2,1));
		return iLength;//set 0 ,otherwise,eip will add this length again

	case 0xc3://ret
		iLength = 1;
		_cpu386->_eip = *(unsigned int*)(_cpu386->_register[4]);
		_cpu386->_register[4] += 4;
		return iLength;//set 0 ,otherwise,eip will add this length again

	case 0xc6:
	case 0xc7:
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&1?2:0);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			if(_eip[1]>=0xc0)
			{
				if(*_eip&1)
				{
					*_base = _immediate;
				}
				else
				{
					*(unsigned char *) _base = _immediate;
				}
				
			}
			else
			{
				if(*_eip&1)
					*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = _immediate;
				else
					*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = _immediate;
			}
		}
		break;
	
	case 0xc9://leave
		//mov esp,ebp
		_cpu386->_register[4] = _cpu386->_register[5];
		//pop ebp
		_cpu386->_register[5] = *(unsigned int*)_cpu386->_register[4];
		_cpu386->_register[4] += 4;
		iLength =1;
		break;


	case 0xd0:
	case 0xd1:
	case 0xd2:
	case 0xd3:
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&0x02 ? 2:1);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			unsigned int result = 0;
			unsigned int operand1 = 0;
			unsigned int operand2 = *_eip&0x02 ? (unsigned char)_cpu386->_register[1] : 1;//cl or 1

			switch((_eip[1]>>3)&0x7)
			{
			case 0x0://rol
				iLength = 0;
				break;
			case 0x1://ror
				iLength = 0;
				break;
			case 0x2://rcl
				iLength = 0;
				break;
			case 0x3://rcr
				iLength = 0;
				break;
			case 0x4://shl
				{
					if(_eip[1] >= 0xc0)
					{

						if(*_eip&1)
						{
							operand1 = *_base;
							*_base = *_base << operand2;
							result = *_base;
							_cf = (operand1 << (operand2-1))&0x7fffffff ? 1:0;
						}
						else
						{
							operand1 = *(unsigned char *)_base;
							*(unsigned char *)_base = *(unsigned char *)_base << operand2;
							result = *(unsigned char *)_base;
							_cf = (operand1 << (operand2-1))&0x7f ? 1:0;
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) << operand2;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							_cf = (operand1 << (operand2-1))&0x7fffffff ? 1:0;
						}
						else
						{
							operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) << operand2;
							result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							_cf = (operand1 << (operand2-1))&0x7f ? 1:0;
						}
					}
					_sf = (int)result < 0 ? 1:0;
					_zf = !result ? 1:0;
					_of = _cf ^ ((int)result < 0) ? 1:0;
				}
				break;
			case 0x5://shr
				{

					if(_eip[1] >= 0xc0)
					{

						if(*_eip&1)
						{
							operand1 = *_base;
							*_base = *_base >> operand2;
							result = *_base;
							
						}
						else
						{
							operand1 = *(unsigned char *)_base;
							*(unsigned char *)_base = *(unsigned char *)_base >> operand2;
							result = *(unsigned char *)_base;
							
						}
					}
					else
					{
						if(*_eip&1)
						{
							operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) >> operand2;
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
							
						}
						else
						{
							operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) >> operand2;
							result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
							
						}
					}
					_cf = (operand1 >> (operand2-1))&1 ? 1:0;
					_sf = (int)result < 0 ? 1:0;
					_zf = !result ? 1:0;
					_of = _cf ^ ((int)result > (int)operand1) ? 1:0;
				}
				break;
			case 0x6://sal
				iLength = 0;
				break;
			case 0x7://sar
				iLength = 0;
				break;

			};

			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;
			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
			//af
			_cpu386->_eflags = _af ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;
		}

		break;


	case 0xe8://call
		iLength = 5;
		_cpu386->_register[4] -= 4;
		*(unsigned int *)_cpu386->_register[4] = _cpu386->_eip + iLength;
		_cpu386->_eip = (unsigned int)_eip + _getimm(_eip,1,4,0);		
		break;


	case 0xe9://jmp
		_cpu386->_eip += *(unsigned int*)(_eip+1);
		iLength = 5;
		break;


	case 0xeb:
		iLength = 2;
		_cpu386->_eip += _getimm(_eip,1,1,1);
		break;

	case 0xf6:
	case 0xf7:
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&0x02 ? 2:1);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);
			unsigned int result = 0;
			unsigned int operand1 = 0;
			unsigned int operand2 = 0;

			switch((_eip[1]>>3)&0x7)
			{
			case 0x0:
			case 0x1://test
				{					
					unsigned int result = 0;

					if(_eip[1]>=0xc0)
					{
						if(*_eip & 1)
						{
							result = *_base & _immediate;
						}
						else
						{
							result = *(unsigned char *)_base & _immediate;
						}
					}
					else
					{
						if(*_eip & 1)
						{
							result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement) & _immediate;
						}
						else
						{
							*(unsigned char *)&result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement) & _immediate;
						}
					}
					_cf = 0;
					_of = 0;
					_zf= result == 0 ? 1:0;
					
					if(*_eip & 1)
						_sf = (int)result < 0 ? 1:0;
					else
						_sf = (char)result < 0 ? 1:0;
				}
				break;
			case 0x2://not
				iLength = 0;
				break;
			case 0x3://neg
				if(_eip[1] >= 0xc0)
				{

					if(*_eip&1)
					{
						operand1 = *_base;
						*_base = 0 - *_base;
						result = *_base;
						iLength -= 4;
					}
					else
					{
					
						operand1 = *(unsigned char *)_base;
						*(unsigned char *)_base = 0 - *(unsigned char *)_base;
						result = *(unsigned char *)_base;
						iLength -= 1;
					}
				}
				else
				{
					if(*_eip&1)
					{
					
						operand1 = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned int *)(*_base + (*_index * _scale) + _displacement) = 0 - *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						result = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
						iLength -= 4;
					}
					else
					{
					
						operand1 = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						*(unsigned char *)(*_base + (*_index * _scale) + _displacement) = 0 - *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						result = *(unsigned char *)(*_base + (*_index * _scale) + _displacement);
						iLength -= 1;
					}
				}
				_cf = operand1 ? 1:0;
				_sf = (int)result < 0 ? 1:0;
				_zf = !result ? 1:0;
				_af = (result&0xf) < (operand1&0xf) ? 1:0;
				_of = _cf ^ (0<(int)operand1) ? 1:0;
				break;
			case 0x4://mul
				iLength = 0;
				break;
			case 0x5://imul
				iLength = 0;
				break;
			case 0x6://div
				{
					
					if(*_eip & 1)
					{
						iLength -= 4;
						//edx:eax
						unsigned __int64 operand1 = _cpu386->_register[2] * 0x100000000 + _cpu386->_register[0];
						if(_eip[1] >= 0xc0)
						{
							_cpu386->_register[0] = (unsigned int)(operand1 / *_base);
							_cpu386->_register[2] = (unsigned int)(operand1 % *_base);
						}
						else
						{
							_cpu386->_register[0] = (unsigned int)(operand1 / *(unsigned int *)(*_base + (*_index * _scale) + _displacement));
							_cpu386->_register[2] = (unsigned int)(operand1 % *(unsigned int *)(*_base + (*_index * _scale) + _displacement));
						}

					}
					else
					{
						iLength -= 1;
						//ax
						if(_eip[1] >= 0xc0)
						{
							*(unsigned char *) _cpu386->_register = (unsigned char)((unsigned short)_cpu386->_register[0] / (unsigned char)*_base);//al
							*((unsigned char *) _cpu386->_register + 1)= (unsigned char)((unsigned short)_cpu386->_register[0] % (unsigned char)*_base);//ah
						}
						else
						{
							*(unsigned char *) _cpu386->_register = (unsigned short)_cpu386->_register[0] / *(unsigned char *)(*_base + (*_index * _scale) + _displacement);//al
							*((unsigned char *) _cpu386->_register + 1) = (unsigned short)_cpu386->_register[0] % *(unsigned char *)(*_base + (*_index * _scale) + _displacement);//ah
						}
					}
					
				}
				break;
			case 0x7://idiv
				iLength = 0;
				break;

			};

			//cf
			_cpu386->_eflags = _cf ? _cpu386->_eflags|0x1 : _cpu386->_eflags&0xfffffffe;
			//of
			_cpu386->_eflags = _of ? _cpu386->_eflags|0x800 : _cpu386->_eflags&0xfffff7ff;
			//zf
			_cpu386->_eflags = _zf ? _cpu386->_eflags|0x40 : _cpu386->_eflags&0xffffffbf;
			//sf
			_cpu386->_eflags = _sf ? _cpu386->_eflags|0x80 : _cpu386->_eflags&0xffffff7f;
			//pf
			_pf = (result ^ (result>>4));
			_pf = (_pf ^ (_pf>>2));
			_pf = 1^(1&(_pf ^ (_pf>>1)));
			_cpu386->_eflags = _pf ? _cpu386->_eflags|0x4 : _cpu386->_eflags&0xfffffffb;
			//af
			_cpu386->_eflags = _af ? _cpu386->_eflags|0x10 : _cpu386->_eflags&0xffffffef;
		}

		break;


	case 0xff:
		{
			iLength += _mod_reg_rm(_cpu386,*_eip&0x02 ? 2:1);

			if(((_eip[1]&7)==4)&&(_eip[1]<0xc0))
				iLength = _s_i_b(_cpu386,iLength);

			switch((_eip[1]>>3)&0x7)
			{
			case 0x00:
				iLength = 0;
				break;
			case 0x01:
				iLength = 0;
				break;
			case 0x02:
				iLength = 0;
				break;
			case 0x03:
				iLength = 0;
				break;
			case 0x04://jmp
				_cpu386->_eip = *(unsigned int *)(*_base + (*_index * _scale) + _displacement);
				return iLength;
			case 0x05:
				iLength = 0;
				break;
			case 0x06:
				iLength = 0;
				break;
			case 0x07:
				iLength = 0;
				break;
			}
		}
		break;


	default:
		iLength = 0;
		break;
	};


	for(i = 0;i<sizeof(prefix);i++)
	{
		if(!prefix[i])
			break;
		if(prefix[i] == 0xf3)
		{
			_cpu386->_register[1] -= 1;
			if(_cpu386->_register[1] != 0)
			{
				goto rep;
			}
		}	
	}
	



	iLength += prefix_length;

	_cpu386->_eip += iLength;
	

	return iLength;
}



int mymain(int arc,char *argv[])
{

	unsigned stackbuff[0x1000];
	int flg = 1;

	_x86Register _cpu386;

	_cpu386._register[0]  = 0; 
	_cpu386._register[1]  = 0;
	_cpu386._register[2]  = 0;
	_cpu386._register[3]  = 0;
	_cpu386._register[4]  = (int)(stackbuff+0x1000);
	_cpu386._register[5]  = 0;
	_cpu386._register[6]  = 0;
	_cpu386._register[7]  = 0;
	
	char param_instr[0x80] = {0};
	unsigned char param_buff[0x10] = {0x66,0x8b,0x01};


	//push parameter2, instrname
	_cpu386._register[4] -= 4;
	*(unsigned int *)_cpu386._register[4] = (unsigned int)param_instr;	

	//push parameter1, buff
	_cpu386._register[4] -= 4;
	*(unsigned int *)_cpu386._register[4] = (unsigned int)param_buff;	


	//call function
	_cpu386._register[4] -= 4;
	*(unsigned int*)_cpu386._register[4] = 0x12345678;	
	_cpu386._eip = (int)&assembler;
	_cpu386._eflags = 0x2;





	int j = 0;
#if 0
	char instrname[0x30] = {0};
	int length = 0;
	int i = 0;
	
	//--------------------log--------------------
	
	FILE *pfile = NULL;
	pfile = fopen("log.txt","w+");
	if(!pfile)
		printf("failed open file");
	char * logstr;
	logstr = (char *)malloc(0x100);
	//--------------------log--------------------
	do
	{

iterator_start:

		fprintf(pfile,"%d\nEIP = 0x%08x\n",++j,_cpu386._eip);
		printf("%d\nEIP = 0x%08x\n",j,_cpu386._eip);


		//set debugger pointer
		if(j == 990)
			printf("");

		unsigned char *_eip = (unsigned char *)_cpu386._eip;

		//print instruction
		//assembler((unsigned char *)_cpu386._eip,instrname);
		fprintf(pfile,"\n%s\n",instrname);
		printf("\n%s\n",instrname);

		//run
		length = run(&_cpu386);

		//print opcode
		if(!length)
		{
			for(i = 0;i<4;i++)
			{
				fprintf(pfile,"%02x ",_eip[i]);
				printf("%02x ",_eip[i]);
			}
			break;
		}
		for(i = 0;i<length;i++)
		{
			fprintf(pfile,"%02x ",_eip[i]);
			printf("%02x ",_eip[i]);
		}
		
		fprintf(pfile,"\nEAX = 0x%08x\nECX = 0x%08x\nEDX = 0x%08x\nEBX = 0x%08x\nESP = 0x%08x\nEBP = 0x%08x\nESI = 0x%08x\nEDI = 0x%08x\nEFLAGS = 0x%08x\n栈顶数据为：0x%08x\n",_cpu386._register[0],_cpu386._register[1],_cpu386._register[2],_cpu386._register[3],_cpu386._register[4],_cpu386._register[5],_cpu386._register[6],_cpu386._register[7],_cpu386._eflags,*(unsigned int *)_cpu386._register[4]);

		fprintf(pfile,"==============================================\n");

//-------------------------------------------------------------------------
		printf("\nEAX = 0x%08x\nECX = 0x%08x\nEDX = 0x%08x\nEBX = 0x%08x\nESP = 0x%08x\nEBP = 0x%08x\nESI = 0x%08x\nEDI = 0x%08x\nEFLAGS = 0x%08x\n栈顶数据为：0x%08x\n",_cpu386._register[0],_cpu386._register[1],_cpu386._register[2],_cpu386._register[3],_cpu386._register[4],_cpu386._register[5],_cpu386._register[6],_cpu386._register[7],_cpu386._eflags,*(unsigned int *)_cpu386._register[4]);

		printf("==============================================\n");

		//jump to second-time
		if(flg)
		{
			flg = 0;
			goto iterator_start;
		}
		
	}while(_cpu386._eip != 0x12345678);
	
	//close file
	fclose(pfile);

#else
do
	{

iterator_start:

		unsigned char *_eip = (unsigned char *)_cpu386._eip;

		//set debugger pointer
		if(_cpu386._eip == 0x4024d3)
			printf("");
		j++;
		if(j == 990)
			printf("");

		//run
		run(&_cpu386);

		//jump to second-time
		if(flg)
		{
			flg = 0;
			goto iterator_start;
		}
		
	}while(_cpu386._eip != 0x12345678);

#endif

	printf("instrname : %s\nlength : %d\n",param_instr,_cpu386._register[0]);

	return 0;
}


int main()
{

	unsigned int _regi[10] ={0x12345678,0x9abcdef0,3,4,5,6,7,8,};
	*((unsigned char *)_regi + 1) = 0xff;

	//--------------------------

	unsigned char instrname[0x30];
	unsigned char buff[0x10] = {0x66,0x8b,0x01};
	int len = assembler((unsigned char *)buff,(char *)instrname);
	printf("instrname = %s\nlength = %d\n============================================\n",instrname,len);

	//--------------------------

	mymain(0,NULL);
	
	
	return 0;
}
