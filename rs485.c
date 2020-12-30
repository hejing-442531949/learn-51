/**********************************************************************************
---RS485---
一般的单片机多机通讯的情况是：
1.PC向特定单片机发送命令，该单片机收到后回应PC，也就是说不存在几个单片机同时向PC发数据的情况。
2.PC以广播的形式发送命令，也就是所有单片机都收到命令，在这种情况下一般都是对单片机进行初始设置（此时所有单片机一般都不做应答）
**********************************************************************************/

#include<reg51.h>
#include<string.h>

#define uchar	unsigned char
#define uint	unsigned int
     
/* 通信命令 */
#define _FLAG_START_		0x0c	// 起始标志位
#define _FLAG_STOP_			0xc0	// 结束标志位
#define _ASK_IS_ACTIVE_		0x01	// 主机询问从机是否存在
#define _MASTER_GETDATA_	0x02	// 主机发送读设备请求
#define _RESPONSE_OK_		0x03	// 从机应答
#define _RESPONSE_STATUS_	0x04	// 从机发送设备状态信息

/****************************帧格式****************************
 -目的地址-命令字-数据-校验码- 至少4个字节
 主机地址为0x00，从机地址为1-244，广播地址255

 转义字符的处理：
 0xdb 0xdd 代表 0xdb
 0xdb 0xdc 代表 0xc0
 0xdb 0xde 代表 0x0c
*************************************************************/

#define MAXSIZE 0x0a						// 缓冲区长度10
uchar send_buf[MAXSIZE]={0xdb,0x0c,0xc0};	// 该缓冲区用于保存设备状态信息
uchar rec_buf1[MAXSIZE];					// 保存接收到的帧
uchar rec_buf2[MAXSIZE];					// 保存处理过的帧

uchar dev;			// 保存本机设备号
uchar len ;			// 保存发送设备信息的长度
uchar type;			// 保存命令字

sbit M_DE = P1^0;	// 驱动器使能，1有效
sbit M_RE = P1^1;	// 接收器使能，0有效

void get_status();							// 调用该函数获得设备状态信息，函数代码未给出
void send_data(uchar len, uchar *send_buf);	// 发送数据帧
bit recv_cmd();								// 接收主机命令，主机请求仅包含命令信息
void send_byte(uchar da);					// 该函数发送一帧数据中的一个字节，由send_data()函数调用

uchar Recv_Flag,Recv_Over_Flag;	//接受允许标志，接受完成标志
uchar uart_i;					//帧数据计数

/* 系统初始化 */
void init_serial() {
	dev =0xff;		//设备号
	TMOD = 0x20;	// 定时器T1使用工作方式2
	TH1 = 0xfd;		// 设置初值
	TL1 = 0xfd;
	TR1 = 1;		// 开始计时
	// PCON = 0x00;	// bps不倍增
	SMOD = 0;		// bps不倍增
	SCON = 0x50;	// 工作方式1，波特率9600bps，允许接收
	ES = 1;			// 开串口中断
	EA = 1;			// 开启中断
	M_DE = 0;		// 置发送禁止，接收允许
	M_RE = 0;
}

void do_message() {
	while(1) {
		if (Recv_Over_Flag) {
			if (recv_cmd()) {
				switch(type) {
				case _ASK_IS_ACTIVE_:
					send_data(_RESPONSE_OK_, 0, send_buf);
					break;
				case _MASTER_GETDATA_:
					len=strlen(send_buf);
					send_data(_RESPONSE_STATUS_, len, send_buf);
					break;
				default:
					break;
				}
				Recv_Over_Flag=0;
				uart_i=0;
			}
		}
	}
}

void main() {
	init_serial();
	do_message();
}


/* 该函数接收一帧数据并进行检测，无论该帧是否错误，函数均会返回
 * 函数参数type保存接收到的命令字
 * 当接收到数据帧错误或其地址位不为0时（非主机发送帧），函数返回0，反之返回1
*/
bit recv_cmd() {
	bit db = 0;		// 当接收到的上一个字节为0xdb时，该位置位
	bit c0 = 0;		// 当接收到的上一个字节为0xc0时，该位置位
	uchar sum = 0;
	uchar i,j;
	M_DE = 1;		// 置发送允许，接收禁止
	M_RE = 1;

	/* 接收一帧数据 */
	for(i=0,j=0; i< uart_i; i++) {			// 循环直至帧接收完毕  
		if(db == 1) {						// 接收到的上一个字节为0xdb
			switch(rec_buf1[i]) {
			case 0xdd:
				rec_buf2[j] = 0xdb;			// 0xdbdd表示0xdb
				db = 0;
				break;
			case 0xdc:
				rec_buf2[j] = 0xc0;			// 0xdbdc表示0xc0
				db = 0;
				break;
			case 0xde:
				rec_buf2[j] = 0x0c;			// 0xdbdc表示0xc0
				db = 0;
				break;
			default :
				return 0;					// 帧错误，返回
			}
			j++;
		} else {
			switch(rec_buf1[i]) {			// 正常情况
			case 0xdb:						// 检测到转义字符
				db = 1;
				break;
			default:						// 普通数据
				rec_buf2[j] = rec_buf1[i];	// 保存数据
				j++;						// 计算校验字节
			}
		}
	}

	/* 判断帧是否错误 */
	for(i=0;i < j-1; i++)
		sum+=rec_buf2[i];
	sum=sum%256;
	if(sum !=rec_buf2[j-1] )	// 校验错误，返回
		return 0;

	if(rec_buf2[0] != dev)		// 非访问本机命令，错误，返回
		return 0;

	type = rec_buf2[1];		// 获得命令字
	return 1;					// 函数成功返回
}

void send_char(uchar ch) {
	TI = 0;
	SBUF = ch;
	while (!TI);
	TI = 0;
}

// 该函数发送一帧数据帧，参数type为命令字、len为数据长度、buf为要发送的数据内容
void send_data(uchar len, uchar *send_buf) {
	uchar i;
	uchar sum = 0;			// 该字节用于保存校验字节
	ES=0;					//关串口中断!!!!
	M_DE = 1;				// 置发送允许，接收禁止
	M_RE = 1;
	send_byte(0x0c);
	send_byte(0x00);		// 发送目的地址
	sum+=0x00;				//假设主机地址为0x00
	send_byte(type);		// 发送命令字
	sum+=type;
	
	for(i=0; i<len; i++) {	// 发送数据
		send_byte(*send_buf);
		sum =sum+ (*send_buf);
		send_buf++;
	}
	sum%=256;
	send_byte(sum);			// 发送校验字节
	send_byte(0xc0);
	ES=1;					//开串口中断
}

//该函数发送一个数据字节，若该字节为0xdb，则发送0xdbdd，若该字节为0xc0则，发送0xdbdc
void send_byte(uchar da) {
	switch(da) {
	case 0xdb:		// 字节为0xdb，发送0xdbdd
		send_char(0xdb);
		send_char(0xdd);
		break;  
	case 0x0c:		// 字节为0x0c，发送0xdbde
		send_char(0xdb);
		send_char(0xde);
		break;
	case 0xc0:		// 字节为0xc0，发送0xdbdc
		send_char(0xdb);
		send_char(0xdc);
		break;
	default:		// 普通数据则直接发送
		send_char(da);
		break;
	}
}

void uart( ) interrupt 4 {				//串口中断
	uchar tmp;
	if(RI) {
		RI = 0;
		tmp = SBUF;						//接收数据
		if(tmp ==_FLAG_START_) {		//帧起始标志
			uart_i=0;
			Recv_Flag=1;				//接受数据标志
		} else if (tmp==_FLAG_STOP_) {	//帧结束标志
			if(Recv_Flag==1)
				Recv_Over_Flag=1;
			Recv_Flag=0;
		} else if(Recv_Flag==1) {		//开始接受帧数据
			rec_buf1[uart_i]=tmp;
			uart_i++;
		}
		if( uart_i >= MAXSIZE ) {		//超过缓冲区长度
			uart_i = 0;
			Recv_Flag=0;
			Recv_Over_Flag = 1;			//接收一帧数据完毕
		}
	}
}
