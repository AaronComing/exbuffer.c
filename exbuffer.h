/*!
 * exbuffer.c
 * yoyo 2013 https://github.com/play175/exbuffer.c
 * new BSD Licensed
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef EXBUFFER_H_INCLUDED
#define EXBUFFER_H_INCLUDED


/*!< ÿ����չ�������ĵ�λ(�ֽ�) */
#define EXTEND_BYTES 512


/*!< �ֽ����� */
enum exbuffer_endian
{
	EXBUFFER_BIG_ENDIAN,
	EXBUFFER_LITTLE_ENDIAN
};

/*!< exbuffer_t���ݽṹ */
typedef struct exbuffer_value
{
	unsigned char headLen;/*!< ��ͷ���ȣ�2/4 */
    enum exbuffer_endian endian;/*!<  */
    size_t readOffset;/*!<  */
    size_t putOffset;/*!<  */
    size_t dlen;/*!< �������ݰ����� */
	unsigned char* buffer;/*!< ������ */
	size_t bufferlen;/*!< ���������� */

    /**< ��������ʱ�õ����� */
    unsigned char *headBytes;
	union HeadBytesS
    {
        unsigned char bytes[2];
        unsigned short val;
    } headS;

    union HeadBytesL
    {
        unsigned char bytes[4];
        unsigned long val;
    } headL;

	void (*recvHandle)(unsigned char*, size_t);/*!< ���յ�����ʱ�Ļص�����ָ�� */
} exbuffer_t;


/*!< ����һ���µ�exbuffer_t���ݽṹ */
exbuffer_t* exbuffer_new();

/*!< �ͷ�exbuffer_t */
void exbuffer_free(exbuffer_t** value);

/*!< ��ӡ�ڴ����� */
void exbuffer_printHex(unsigned char* bytes,unsigned short len);

/*!< ��ӡexbuffer_t�еĻ������ڴ�(����ӡ50���ֽ�) */
void exbuffer_dump(exbuffer_t* value,unsigned short len);

/*!< ��ȡ��������Ч���ݳ��� */
size_t exbuffer_getLen(exbuffer_t* value);

/*!< ����һ�����ݽ�ȥ */
void exbuffer_put(exbuffer_t* value, unsigned char* buffer,size_t offset,size_t len);

#endif // EXBUFFER_H_INCLUDED
