#include<reg52.h>

typedef unsigned char	UCAHR;

UCHAR g_read_char;
bit enable_send = 0;

void init_serial() {
	// serial control
	SCON = 0x70;		// 0x60 | 0x10 0x60:9位可变, 0x10:REN,read enable
	SMOD = 1;			// 加倍

	// baud = fosc/(12*16*x) -> 9600=11059200/192x -> x=6 -> TL1=256-6=250
	TMOD = 0x20;		// timer1 8位重装
	TH1 = 250;
	TL1 = 250;

	// enable interrupt
	ET1 = 1;	// enable timer1
	ES = 1;		// enable serial
	TR1 = 1;	// timer run 1
	EA = 1;		// enable all
}

void send_uchar(UCHAR ch) {
	SBUF=ch;
	while(!TI);		// TI为1时,表示发送完成,此时等待发送完成
	TI = 0;
}

void send_buf(UCHAR *buf, UCHAR len) {
	UCHAR *p = buf;
	while (len--) {
		send_uchar(*p++);
	}
}

void serial_interrupt() using 4 interrupt 0 {
	// 重装,不需要赋初值
	if (TI)
		TI = 0;
	if (RI) {
		RI = 0;
		g_read_char = SBUF;
		enable_send = 1;
	}
}

void main() {
	while (enable_send) {
		send_uchar(g_read_char);
		enable_send = 0;
	}
}
