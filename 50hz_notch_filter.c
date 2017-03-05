#include <ADuC7026.h>


void ADCinit(int time);	  //ADC上电
void DACinit(int time);	  //DAC上电
void Timer_Initiate();	  //定时器
void IRQ_Handler() __irq;  //中断函数


int flag=0;
int count=0,x0,count50=0,count20=0;
typedef unsigned int     uint32;     // 无符号32位整型变量
	uint32 MyIRQState;

int main (void)  
{
	
	//int y0,y1,y2,y3, i=1,j=1,r0,r1;
	int x0=0,x1=0,x2=0,x3=0,x4=0,y=0;

	ADCinit(20000);					// CONFIGURATION on ADC
	DACinit(20000);					// CONFIGURATION on DAC



	//GP0CON = 0x100000; 					// Enable ADCbusy on P0.5
	Timer_Initiate();
 	// configures GPIO to flash LED P4.2
	//GP4DAT = 0x01000000;				// P4.2 configured as an output. LED is turned on	
	ADCCON = 0x17a4;                    //时钟32分频，采样需要16个时钟周期，启动任意类型转换，ADC进入正常模式，模拟信号单端连接输入，连续软件触发转换；
	ADCCP = 0x00;                        //从ADC0输入模拟信号；

	while(1)
	{			
		if(flag==1)				  //一次采样ADC转换结束；
		{
				y=-(x0>>2)+x1+(x2>>3)-(x2>>1)+x3-(x4>>2);  //已经得到的滤波器传递函数，x0是最新采样得到的数据，x1是前一次采样得到的数据，以此类推；
                x4=x3; 
				x3=x2; 
				x2=x1; 
				x1=x0; 
                y=0xfff-y;		//因为输出电路的原因，要求此处的输出取相反数；				 、
				flag=0;			 //flag清零，等待下一次ADC转换结束；
		}
		DAC0DAT=y<<16;      //将y左移16位放入DACDAT中（DACDAT的27:16是存储有效数据的地方），以便进行后续的DAC转换；	
	}  
}	




void ADCinit(int time)
{
	 				
	ADCCON = 0x20;		// power-on the ADC，书上例子上电是这么写的，我也不知道为什么要这么设；

	while (time >=0)
	{	  				// wait for ADC to be fully powered on
    time--;
	}
	REFCON = 0x01;						// internal 2.5V reference.2.5V on Vref pin；
}

void DACinit(int time)
{
	DAC0CON = 0x12;				// DAC configuration
								// DAC0输出电压范围是0到VREF
								// DAC0 is updated with falling edge of core clock，使用内部核时钟更新DAC
	DAC0DAT = 0x08000000;		// start from midscale，调整坐标，从中间开始

	while (time >=0)
	{	  				// wait for DAC to be fully powered on
    time--;
	}
}
void Timer_Initiate()	//定时时间为10us
{
	T1CON  = 0xC0;	  //选择核时钟（41.78Mhz），Timer1递减计数，使能Timer1，选择周期模式，二进制数据格式，CLK/1；
	T1LD   = 0x1A2;	 //存储的是定时器1每次定时的时长，见图片公式
//	T1LD   = 0x8B;	//3.33us					    
	IRQEN  = 0x08;	  //使能Timer1IRQ
}
void IRQ_Handler() __irq
{
	MyIRQState = IRQSTA;	
	if((MyIRQState & 0x08)  != 0)   //判断是否为Timer1 IRQ
	{
		count++;				  //若是Timer1中断，计数加1
		

		if(count>499)		   //计时5ms满，因为采样周期是200hz
		{
			 	count=0;
			
				ADCCON = 0xa4;           //ADC开始转换
				ADCCP = 0x00;			 //ADC0
				while (!ADCSTA){}			// wait for end of conversion
				x0 = ADCDAT>>16;		 //转换之后的数据给x0
                flag=1;					 //一次采样转换结束

					
//				DAC0DAT=ADCDAT;

           }
		T1CLRI = 0;
	}

} 


