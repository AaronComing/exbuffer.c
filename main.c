#include "exbuffer.h"

void recvHandle(unsigned char *rbuf,size_t len)
{
	printf("�յ�����:%d\n",len);
	exbuffer_printHex(rbuf,len);
}


int main(int argc, char **argv)
{
	exbuffer_t* value;
	value = exbuffer_new();
	//value->headLen = 2;//���ð�ͷ���ȣ�Ĭ����2��2��4�ֱ��Ӧshort��int���ɱ�ʾ65535/�Լ�2~32�η����ȵ����ݣ�
	value->recvHandle = recvHandle;

	//test ntohl
	//printf("%d\n",ntohl(1,EXBUFFER_LITTLE_ENDIAN));

	unsigned char buf[] = {0,2,3,4,0,1,5,0};
	exbuffer_put(value,(unsigned char*)buf,0,8);

	unsigned char buf2[] = {3};
	exbuffer_put(value,(unsigned char*)buf2,0,1);

	unsigned char buf3[] = {6,6,6};
	exbuffer_put(value,(unsigned char*)buf3,0,3);

	//printf("��Ч���ݳ���:%d\n",exbuffer_getLen(value));
	//printf("����������:%d\n",value->bufferlen);
	printf("������:\n");
	exbuffer_dump(value,value->bufferlen);
	exbuffer_free(&value);
	//system("pause");
	//getchar();
	return EXIT_SUCCESS;
}
