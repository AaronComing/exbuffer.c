/*!
 * exbuffer.c
 * yoyo 2013 https://github.com/play175/exbuffer.c
 * new BSD Licensed
 */
#include "exbuffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

//��һλ���Ƿ����(0/1)���ڶ�λ����ǰ�����ֽ��������little��Ϊ1����Ϊ0
static unsigned char HOST_ENDIAN_LITTLE = 0;

void check_host_endian()
{
	if((HOST_ENDIAN_LITTLE & 0x01) == 0x01)
	{
		return;
	}
	HOST_ENDIAN_LITTLE = HOST_ENDIAN_LITTLE|0x01;
	unsigned short int i =0x1234;
	unsigned char* p = (unsigned char*)&i;
	if(*p == 0x12)
	{
		//printf(">>big endian\n");
		HOST_ENDIAN_LITTLE = HOST_ENDIAN_LITTLE ^ (~HOST_ENDIAN_LITTLE & (0x00<<1));
	}
	else
	{
		//printf(">>little endian\n");
		HOST_ENDIAN_LITTLE = HOST_ENDIAN_LITTLE ^ (~HOST_ENDIAN_LITTLE & (0x01<<1));
	}
	free(p);
	p = NULL;
}

unsigned long ntohl(unsigned long x,enum exbuffer_endian endian)
{
	check_host_endian();
	if(
		(endian == EXBUFFER_BIG_ENDIAN  \
		&& ((HOST_ENDIAN_LITTLE & (0x01<<1)) == (0x00<<1))//big endian
		)
		||(endian == EXBUFFER_LITTLE_ENDIAN  \
		&& ((HOST_ENDIAN_LITTLE & (0x01<<1)) == (0x01<<1))//little endian
		)
	)
	{
		return x;
	}

	return
	((unsigned long)(	\
        (((unsigned long)(x) & (unsigned long)0x000000ffUL) << 24) |	 \
        (((unsigned long)(x) & (unsigned long)0x0000ff00UL) <<  8) |	 \
        (((unsigned long)(x) & (unsigned long)0x00ff0000UL) >>  8) |	 \
        (((unsigned long)(x) & (unsigned long)0xff000000UL) >> 24)));
}

unsigned short ntohs(unsigned short x,enum exbuffer_endian endian)
{
	check_host_endian();
	if(
		(endian == EXBUFFER_BIG_ENDIAN  \
		&& ((HOST_ENDIAN_LITTLE & (0x01<<1)) == (0x00<<1))//big endian
		)
		||(endian == EXBUFFER_LITTLE_ENDIAN  \
		&& ((HOST_ENDIAN_LITTLE & (0x01<<1)) == (0x01<<1))//little endian
		)
	)
	{
		return x;
	}

	return
	((unsigned short)(	\
        (((unsigned short)(x) & (unsigned short)0x00ffU) << 8) |	 \
        (((unsigned short)(x) & (unsigned short)0xff00U) >> 8)));
}

/*!< ����һ���µ�exbuffer_t���ݽṹ */
exbuffer_t* exbuffer_new()
{
    unsigned char headLen = 2;
    enum exbuffer_endian endian =  EXBUFFER_BIG_ENDIAN;
    size_t bufferlen = 512;
	exbuffer_t* value;
	value =  (exbuffer_t *)malloc (sizeof (exbuffer_t));
	value->bufferlen = bufferlen;
	value->headLen = headLen;
	value->endian = endian;
	value->readOffset = 0;
	value->putOffset = 0;
	value->dlen = 0;
	value->recvHandle = NULL;

	value->headBytes = (unsigned char *)malloc(4);

	value->buffer = (unsigned char*)malloc(value->bufferlen);
	//memset(value->buffer,0,value->bufferlen);

	return value;
};

/*!< �ͷ�exbuffer_t */
void exbuffer_free(exbuffer_t** value)
{
	free ((*value)->buffer);
	(*value)->buffer = NULL;
	(*value)->recvHandle = NULL;
	free (*value);
	(*value) = NULL;
};

/*!< ��ӡ�ڴ����� */
void exbuffer_printHex(unsigned char* bytes,unsigned short len)
{
    if(len>50)len=50;
	unsigned short iLoop;
	for(iLoop = 0;iLoop < len;iLoop++)
    {
        printf("%02x ",bytes[iLoop]);
    }
	printf("\n");
};

/*!< ��ӡexbuffer_t�еĻ������ڴ�(����ӡ50���ֽ�) */
void exbuffer_dump(exbuffer_t* value,unsigned short len)
{
	exbuffer_printHex(value->buffer,len);
};

/*!< ��ȡ��������Ч���ݳ��� */
size_t exbuffer_getLen(exbuffer_t* value)
{
   if(value->putOffset>= value->readOffset){ // ------******-------
		return value->putOffset - value->readOffset;
	}
	return value->bufferlen - value->readOffset + value->putOffset; //***-------*********
};

void exbuffer_proc(exbuffer_t* value)
{
	unsigned short count = 0;
	size_t i;
	unsigned char rlen = 0;

	while(1)
	{
		count++;
		if(count>1000)
		{
			fprintf(stderr,"count>1000\n");
			break;//1000�λ�û����??
		}
		if(value->dlen == 0)
		{
			//printf("value->dlen == 0\n");
			//printf("��Ч����:%d\n",exbuffer_getLen(value));
			if(exbuffer_getLen(value) < value->headLen)
			{
				//printf("����ͷ��������:%d\n",value->dlen);
				break;//����ͷ��������
			}
			if(value->bufferlen - value->readOffset >= value->headLen)//***********[**]
			{
				//printf("***********[**]\n");
				for(i=0;i<value->headLen;i++)
				{
					value->headBytes[i] = value->buffer[value->readOffset+i];
				}
				value->readOffset += value->headLen;
				//printf("value->readOffset=%d\n",value->readOffset);
			}
			else //*]**----------********[*
			{
				//printf("*]**----------********[*\n");
				for(i = 0;i<(value->bufferlen - value->readOffset);i++)
				{
					value->headBytes[i] = value->buffer[value->readOffset+i];
					rlen++;
				}
				value->readOffset = 0;
				for(i = 0;i<(value->headLen - rlen);i++)
				{
					value->headBytes[rlen+i] = value->buffer[value->readOffset+i];
				}
				value->readOffset += (value->headLen - rlen);
			}
			//�������峤��
			if(value->headLen==2)
			{
				value->headS.bytes[0] = value->headBytes[0];
				value->headS.bytes[1] = value->headBytes[1];
				value->dlen = ntohs(value->headS.val,value->endian);//�������ֽ��򻻳������ֽ���
			}
			else
			{
				value->headL.bytes[0] = value->headBytes[0];
				value->headL.bytes[1] = value->headBytes[1];
				value->headL.bytes[2] = value->headBytes[2];
				value->headL.bytes[3] = value->headBytes[3];
				value->dlen = ntohl(value->headL.val,value->endian);//�������ֽ��򻻳������ֽ���
			}
			//exbuffer_printHex(value->headBytes,2);
			//printf("value->dlen=%d\n",value->dlen);
			//printf("value->readOffset=%d\n",value->readOffset);
		}

		//printf("value->dlen:%d\n",value->dlen);
		//������
		if(exbuffer_getLen(value) >= value->dlen)
		{
			unsigned char *dbuff = (unsigned char *)malloc(value->dlen);
			if(value->readOffset + value->dlen > value->bufferlen)//***---*****
			{
				size_t len1 = value->bufferlen - value->readOffset;
				if (len1 > 0)
				{
					memcpy(dbuff,value->buffer + value->readOffset,len1);
				}
				value->readOffset = 0;

				size_t len2 = value->dlen - len1;
				memcpy(dbuff + len1,value->buffer + value->readOffset,len2);
				value->readOffset += len2;
			}
			else
			{
				memcpy(dbuff,value->buffer + value->readOffset,value->dlen);
				value->readOffset += value->dlen;
			}
			size_t dlen = value->dlen;
			value->dlen = 0;

			if(value->recvHandle == NULL)
			{
				printf("receive packet:%d\n",dlen);
				//exbuffer_printHex(dbuff,dlen);
			}
			else
			{
				value->recvHandle(dbuff,dlen);
			}
			free(dbuff);
			dbuff = NULL;

			if (value->readOffset == value->putOffset)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}

/*!< ����һ�����ݽ�ȥ */
void exbuffer_put(exbuffer_t* value, unsigned char* buffer,size_t offset,size_t len)
{
	//exbuffer_dump(value);
	//printf(">>receive bytes:%d\n",len);
	//��ǰ�������Ѿ������������������
	if(len + exbuffer_getLen(value) > value->bufferlen){
		size_t rn1 = (len + exbuffer_getLen(value))/EXTEND_BYTES;
		if((len + exbuffer_getLen(value))%EXTEND_BYTES>0)rn1+=1;
		size_t ex = rn1 * EXTEND_BYTES;//ÿ����չEXTEND_BYTES�ı���
		size_t exlen = ex - value->bufferlen;//���ӵĳ���
		unsigned char* tmp = (unsigned char*)malloc(ex);
		//memset(tmp,0,ex);
		memcpy(tmp,value->buffer,value->bufferlen);
		//�ͷ�ԭ�ڴ���
		free(value->buffer);
		value->buffer = NULL;
		//����ָ�����ڴ�
		value->buffer = tmp;
		value->bufferlen = ex;

		//�����ڴ�
		if (value->putOffset < value->readOffset) //***** ---********-------
		{
			size_t cpylen;
			size_t cpystctstart;
			size_t cpydeststart;
			if (value->putOffset <= exlen) //***** ---********-------
			{
				cpystctstart = 0;
				cpydeststart = ex - exlen;
				cpylen = value->putOffset;
			}
			else //********* ---********-----
			{
				cpystctstart = 0;
				cpydeststart = ex - exlen;
				cpylen = exlen;
			}
			memcpy(value->buffer + cpystctstart,value->buffer + cpydeststart,cpylen);
		}
	}

	if(exbuffer_getLen(value) == 0){
		value->putOffset = value->readOffset = 0;
	}
	//�ж��Ƿ�����_bufferβ��
	if((value->putOffset + len) > value->bufferlen){
		//�����δ� һ���ִ������ݺ��� һ���ִ�������ǰ��
		size_t len1 = value->bufferlen - value->putOffset;
		memcpy(value->buffer + value->putOffset,buffer + offset,len1);
		offset += len1;
		size_t len2 = len - len1;
		memcpy(value->buffer,buffer + offset,len2);
		value->putOffset = len2;
	}else{
		memcpy(value->buffer + value->putOffset,buffer + offset,len);
		value->putOffset += len;
	}
	//exbuffer_dump(value);
	//printf("value->putOffset:%d\n",value->putOffset);
	//printf("value->readOffset:%d\n",value->readOffset);
	exbuffer_proc(value);
};

#ifdef __cplusplus
}
#endif

