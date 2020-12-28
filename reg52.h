// 21个特殊功能寄存器(52系列是26个)不连续地分布在128个字节的SFR存储空间中
// 地址空间为80H-FFH(128个字节)
// I/O口模块,中断模块,定时器模块和串口通信模块(串行I/O口)

/*******************************************
I/O口相关:		P1 P2 P3 P4
中断相关:		IP IE
	Interrupt Priority, Interrupt Enable
定时器相关:		TMOD TCON TL0 TH0 TL1 TH1
	Timer Mode, Timer Control, Timer Low, Timer High
串口通信相关:	PCON SBUF
********************************************/

// 输入输出口(I/O)寄存器
sfr P0    = 0x80;
sfr P1    = 0x90;
sfr P2    = 0xA0;
sfr P3    = 0xB0;

// Program Status World 程序状态字
sfr PSW   = 0xD0;

// ACCumulator 累加器
sfr ACC   = 0xE0;

// 做乘/除法时放乘数或除数,不做乘除法时,随你怎么用
// 作乘法时,A和B相乘,结果低位在A中,高位在B中		:A=42H,B=35H	作"MUL AB"之后,A=AAH,B=0DH,42H * 35H = DAAH			66 * 53 = 3498
// 作除法时,A是被除数,B是除数,结果A是商,B是余数		:A=42H,B=13H	做"DIV AB"之后,A=03H,B=09H,42H / 13H = 03H...09H	66 / 19 = 3...9
sfr B     = 0xF0;

// Stack Pointer 堆栈指针
sfr SP    = 0x81;

// Data Pointer 据指针,用来存放16位地址值
// 间接寻址或变址寻址的方式对片外数据RAM或程序存储器作64K字节范围内的数据操作
// Data Pointer Low
sfr DPL   = 0x82;
// Data Pointer High
sfr DPH   = 0x83;
// Power Control 电源控制寄存器
// 低4位用于选择不同的低功耗模式，比如空闲模式、掉电模式、时钟停止模式等
// 最高位SMOD用于串行口通信的波特率加倍
sfr PCON  = 0x87;
// Timer Control 定时器控制寄存器
sfr TCON  = 0x88;
// Timer Mode 定时器控制寄存器 不按位寻址，地址89H
sfr TMOD  = 0x89;
// Timer Low 定时器/计数器0(低8位)
sfr TL0   = 0x8A;
// Timer Low 定时器/计数器1(低8位)
sfr TL1   = 0x8B;
// Timer Low 定时器/计数器0(高8位)
sfr TH0   = 0x8C;
// Timer Low 定时器/计数器1(高8位)
sfr TH1   = 0x8D;

// Interrupt Enable 中断充许寄存器
sfr IE    = 0xA8;
// Interrupt Priority 中断优先级控制寄存器
// 中断优先级控制寄存器在51中IP.0--IP.4依次控制INT0、T0、INT1、T1、UART中断的优先级，高三位不用
sfr IP    = 0xB8;
// Serial Control 串行口控制寄存器
sfr SCON  = 0x98;
// 串行口锁存器
sfr SBUF  = 0x99;

// 52用
// T2定时器/计数器控制寄存器
sfr T2CON  = 0xC8;
// 外部输入(P1.1)计数器/自动再装入模式时初值寄存器低八位
sfr RCAP2L = 0xCA;
// 外部输入(P1.1)计数器/自动再装入模式时初值寄存器高八位
sfr RCAP2H = 0xCB;
// 定时器/计数器2(低8位)
sfr TL2    = 0xCC;
// 定时器/计数器2(高8位)
sfr TH2    = 0xCD;

// PSW各特殊位的定义,^为异或,实际为取第几位
// Carry,进位标志位,8位运算时有无进位
sbit CY    = PSW^7;
// Assistant Carry,辅助进位标志位,8位运算的低半字节(0~3位)有无进位
sbit AC    = PSW^6;
// 用户定义的一个状态标志,可以通过软件对它置位或清零。
sbit F0    = PSW^5;
// Register Select
// 00, 01, 10, 11 -> 0, 1, 2, 3区
sbit RS1   = PSW^4;		//0		0		1		1
sbit RS0   = PSW^3;		//0		1		0		1
						//0区	1区		2区		3区
						//0~7H	8~FH	10~17H	18~1FH
// Overflow
// 溢出和进位是两个不同的概念
// 进位是指无符号数运算时ACC中D7向更高位的进位
// 溢出是指带符号数补码运算时，运算结果超出8位二进制补码的表示范围
sbit OV    = PSW^2;
// Parity 奇偶校验位,ALU运算结果中二进制数位1的个数的奇偶性
sbit P     = PSW^0;		//8052 only

// Timer Control
// Timer1 Flow 定时器1溢出标志
sbit TF1   = TCON^7;
// Timer1 Run 允许位
sbit TR1   = TCON^6;
// Timer0 Flow 定时器0溢出标志
sbit TF0   = TCON^5;
// Timer0 Run
sbit TR0   = TCON^4;
// Interrupt1 External 外部中断1(INT1,P3.3)请示源, IE1=1时,外部中断1正在向CPU请求中断
// 当CPU响应该中断时由硬件清'0'
// (边沿触发方式)
sbit IE1   = TCON^3;
// 外部中断源1标志触发方式控制位
// IT1=0时, 外部中断1程控为电平触发方式,即:当INT1(P3.3)输入低电平时,置位IE1
sbit IT1   = TCON^2;
// 外部中断0(INT0,P3.2)请示源
sbit IE0   = TCON^1;
// 外部中断源0标志触发方式控制位
// 当INT1(P3.2)输入低电平时,置位IE1
// IT=0表示电平触发
// IT=1表示边缘触发
sbit IT0   = TCON^0;
// TCON的清零需要程序员在程序中清零,即TF0=0

// Interrupt Enable
// EA=0时,所有中断禁止(即不产生中断)
// EA=1时,各中断的产生由个别的允许位决定
// 相当于总开关
sbit EA    = IE^7;
// Enable Timer 定时2溢出中断充许 (8052用)
sbit ET2   = IE^5;
// Enable Serial 串行口中断充许(ES=1充许,ES=0禁止)
sbit ES    = IE^4;
// Enable Timer 定时1溢出中断充许
sbit ET1   = IE^3;
// Enable External 外中断INT1中断充许
sbit EX1   = IE^2;
// Enable Timer 定时0溢出中断充许
sbit ET0   = IE^1;
// Enable External 外中断INT0中断充许
sbit EX0   = IE^0;

// Interrupt Priority
// Priority Timer 2 (8052用)
sbit PT2   = IP^5;
// Priority Serial
sbit PS    = IP^4;
// Priority Timer 1
sbit PT1   = IP^3;
// Priority External 1
sbit PX1   = IP^2;
// Priority Timer 0
sbit PT0   = IP^1;
// Priority External 0
sbit PX0   = IP^0;

// Timer Mode 定时器控制寄存器 不按位寻址，地址89H
// 不按位寻址
// 不按位寻址
// 不按位寻址
/********************
定时操作开关控制位
// Timer 1
当GATE=1时,INT1引脚为高电平,同时TCON中的TR1控制位为1时,计时/计数器1才开始工作
当GATE=0时,则只要将TR1控制位设为1,计时/计数器1就开始工作
sbit Gate1 = TMOD^7;
// Counter/Timer
// C/T=1为计数器,通过外部引脚T1输入计数脉冲
// C/T=0为定时器,由内部系统时钟提供计时工作脉冲
sbit C/T1   = TMOD^6;
// Mode
// 00:a		13位计数/计时器
// 01:b		16位计数/计时器
// 10:c		8位自动加载计数/计时器,低8位是定时器初值,高8位是自动重装值,低8位计数溢出后将高8位的重装值赋值给低8位
// 11:d		仅适用于T0,T1在方式3时停止工作
sbit M11   = TMOD^5;//	0	0	1	1
sbit M10   = TMOD^4;//	0	1	0	1
//						a	b	c	d
// Timer 0
当GATE=1时,INT0引脚为高电平,同时TCON中的TR0控制位为1时,计时/计数器0才开始工作
当GATE=0时,则只要将TR0控制位设为1,计时/计数器0就开始工作
sbit Gate0 = TMOD^3;
// Counter/Timer
// C/T=1为计数器,通过外部引脚T0输入计数脉冲
// C/T=0为定时器,由内部系统时钟提供计时工作脉冲
sbit C/T0   = TMOD^2;
// Mode
// 00:a		13位计数/计时器
// 01:b		16位计数/计时器
// 10:c		8位自动加载计数/计时器
// 11:d		仅适用于T0,定时器0分为两个独立的8位定时器/计数器TH0及TL0,T1在方式3时停止工作
sbit M01   = TMOD^1;
sbit M00   = TMOD^0;
********************/

sbit RD    = P3^7;
sbit WR    = P3^6;
sbit T1    = P3^5;	// 外部引脚,输入计数脉冲,计数器
sbit T0    = P3^4;
sbit INT1  = P3^3;	// 外部中断标志,
sbit INT0  = P3^2;
sbit TXD   = P3^1;
sbit RXD   = P3^0;

// Serial Control
// Serial Mode
// <SM0,SM1>:
// <0, 0>:方式0,波特率=f/12
// <0, 1>:方式1,波特率=1/16或1/32*f/(12*(256-X)), X = 256 - f / ( 波特率 * 16) / 12
// <1, 0>:方式2,波特率=2^SMOD * f / 64
// <1, 1>:方式3,波特率=1/16或1/32*f/(12*(256-X))
sbit SM0   = SCON^7;
sbit SM1   = SCON^6;
// 多机通信控制位,主要用于2和3,仅用于接收
// a.若SM2=1,允许多机通信,只有当接收到第9位数(RB8)为1时,才接收前8位数送入SBUF
//		并向RI位产生中断请求,否则前8位数丢弃
// b.SM2=0时,无论RB8是0/1,都将前8位数装入SBUF中并产生中断请求
// c.在方式0,不用SM2时,必须设为0
// d.在方式1时,若SM2=1时,则只有接收到有效停止位时RI才置1
sbit SM2   = SCON^5;
// 允许接收位, REN=1允许接收,否则不允许
sbit REN   = SCON^4;
// Tx Bit8
// 为发送的第9位数(在方式2,3时),可用作校验位
// 在多机通信中,用TB8的状态表示主机发送的是地址还是数据
//		TB8=1时表示地址,TB8=0时表示数据
sbit TB8   = SCON^3;
// Rd Bi8
// 是接收数据第9位(方式2,3),还代表接收数据的特征,可能是校验或地址/数据标志
sbit RB8   = SCON^2;
// Tx Interrupt
// 发关中断标志位
sbit TI    = SCON^1;
// Rx Interrupt
// 接收中断标志位,由软件清0,串行接收必须满足REN=1,RI=0.
sbit RI    = SCON^0;

// 
sbit T2EX  = P1^1; // 8052 only
sbit T2    = P1^0; // 8052 only

// Power Control
// Serial Mode 1表示加倍
sbit SMOD   = PCON^7;
sbit SMOD0  = PCON^6;// 特有
sbit LVDF   = PCON^5;// 特有
sbit P0F    = PCON^4;// 特有
// 通用,可自定义使用
sbit GF1    = PCON^3;
sbit GF0    = PCON^2;
// Power Down
// PD = 0:正常工作
// PD = 1:掉电模式,外部晶振,CPU,定时器,串口停止工作,只有外部中断继续工作
sbit PD     = PCON^1;
// 空闲模式设置
// IDL = 0:正常工作
// IDL = 1:CPU不工作,其他正常工作,可由中断或硬件复位唤醒
sbit IDL    = PCON^0;

// Timwe Control (8052)
// Timer Flow 必须由用户程序清0,当T2作为串口波特率发生器时,不会被置1
sbit TF2    = T2CON^7;
// External Flow 外部中断标志
// EXEN2为1时,当T2EX(P1.1)发生负跳变时置1中断标志DXF2,EXF2必须由用户程序清0
sbit EXF2   = T2CON^6;
// Recv Clock 串行接口的接收时钟选择标志位,RCLK=1时,T2工作于波特率发生器方式
sbit RCLK   = T2CON^5;
// Tx Clock 串行接口的发送时钟选择标志,TCLK=1时,T2工作于波特率发生器方式
sbit TCLK   = T2CON^4;
// External Enable
sbit EXEN2  = T2CON^3;
// Timer Run T2计数/定时控制位,TR2为1时充许计数,为0时禁止计数
sbit TR2    = T2CON^2;
// Counter/Timer 外部计数器/定时器选择位
// C_T2=1时,T2为外部事件计数器,计数脉冲来自T2(P1.0)
// C_T2=0时,T2为定时器,振荡脉冲的十二分频信号作为计数信号
sbit C_T2   = T2CON^1;
// Capture XX 捕捉和常数自动再装入方式选择位
// 为1时工作于捕捉方式
// 为0时T2工作于常数自动再装入方式
// 当TCLK或RCLK为1时,CP/RL2被忽略,T2总是工作于常数自动再装入方式
sbit CP_RL2 = T2CON^0;

#endif

// 中断服务程序写法：
// void func_name(void) interrupt n using m
// n:0-5共六种:外部中断0、定时器0、外部中断1、定时器1、串行口、定时器2
//				INT0、T0、INT1、T1、UART、T2
// m:0-3共四组寄存器,0~7H,8~FH,10~17H,18~1FH,避免相互影响

// 波特率,SMOD = 1, f = 11.0592MHZ,1个机器周期=12个时钟周期
// 9600 = 1/16*f/(12*(256-X)) -> 256 - f / (9600 * 16) / 12 -> X = 6

// 定时器 timer
// TMOD, TR(0~2), TH(0~2), TL(0~2)

// 串口 serial
// SCON, (timer), SBUF

// 中断
// IE, IP

// 晶振(时钟频率)fosc:			12MHz				11.0592MHz		fosc
// 时钟周期: 1/fosc				1/12us				1/11.0592us		1/fosc
// 机器周期: 一般为12*时钟周期	12/12=1us			12/11.0592us	12/fosc
// SMOD:波特率加倍标志
// 溢出时间: f_time = (0x100-初始值)*(12/fosc)
// 溢出率:	 1/f_time=(fosc/12)/(0x100-初始值)
// 波特率:方式0,波特率=f/12
//		  方式2,波特率=f/64 or f/32(当SMOD=1时)
//		  方式1,波特率=((2^SMOD)/32 )*溢出率 -> 溢出率/32 or 溢出率/16(SMOD=1时)
//					  =fosc/(12*32*x)	or fosc/(12*16*x)
