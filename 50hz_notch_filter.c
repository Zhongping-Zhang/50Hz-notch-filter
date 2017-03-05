#include <ADuC7026.h>


void ADCinit(int time);	  //ADC�ϵ�
void DACinit(int time);	  //DAC�ϵ�
void Timer_Initiate();	  //��ʱ��
void IRQ_Handler() __irq;  //�жϺ���


int flag=0;
int count=0,x0,count50=0,count20=0;
typedef unsigned int     uint32;     // �޷���32λ���ͱ���
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
	ADCCON = 0x17a4;                    //ʱ��32��Ƶ��������Ҫ16��ʱ�����ڣ�������������ת����ADC��������ģʽ��ģ���źŵ����������룬�����������ת����
	ADCCP = 0x00;                        //��ADC0����ģ���źţ�

	while(1)
	{			
		if(flag==1)				  //һ�β���ADCת��������
		{
				y=-(x0>>2)+x1+(x2>>3)-(x2>>1)+x3-(x4>>2);  //�Ѿ��õ����˲������ݺ�����x0�����²����õ������ݣ�x1��ǰһ�β����õ������ݣ��Դ����ƣ�
                x4=x3; 
				x3=x2; 
				x2=x1; 
				x1=x0; 
                y=0xfff-y;		//��Ϊ�����·��ԭ��Ҫ��˴������ȡ�෴����				 ��
				flag=0;			 //flag���㣬�ȴ���һ��ADCת��������
		}
		DAC0DAT=y<<16;      //��y����16λ����DACDAT�У�DACDAT��27:16�Ǵ洢��Ч���ݵĵط������Ա���к�����DACת����	
	}  
}	




void ADCinit(int time)
{
	 				
	ADCCON = 0x20;		// power-on the ADC�����������ϵ�����ôд�ģ���Ҳ��֪��ΪʲôҪ��ô�裻

	while (time >=0)
	{	  				// wait for ADC to be fully powered on
    time--;
	}
	REFCON = 0x01;						// internal 2.5V reference.2.5V on Vref pin��
}

void DACinit(int time)
{
	DAC0CON = 0x12;				// DAC configuration
								// DAC0�����ѹ��Χ��0��VREF
								// DAC0 is updated with falling edge of core clock��ʹ���ڲ���ʱ�Ӹ���DAC
	DAC0DAT = 0x08000000;		// start from midscale���������꣬���м俪ʼ

	while (time >=0)
	{	  				// wait for DAC to be fully powered on
    time--;
	}
}
void Timer_Initiate()	//��ʱʱ��Ϊ10us
{
	T1CON  = 0xC0;	  //ѡ���ʱ�ӣ�41.78Mhz����Timer1�ݼ�������ʹ��Timer1��ѡ������ģʽ�����������ݸ�ʽ��CLK/1��
	T1LD   = 0x1A2;	 //�洢���Ƕ�ʱ��1ÿ�ζ�ʱ��ʱ������ͼƬ��ʽ
//	T1LD   = 0x8B;	//3.33us					    
	IRQEN  = 0x08;	  //ʹ��Timer1IRQ
}
void IRQ_Handler() __irq
{
	MyIRQState = IRQSTA;	
	if((MyIRQState & 0x08)  != 0)   //�ж��Ƿ�ΪTimer1 IRQ
	{
		count++;				  //����Timer1�жϣ�������1
		

		if(count>499)		   //��ʱ5ms������Ϊ����������200hz
		{
			 	count=0;
			
				ADCCON = 0xa4;           //ADC��ʼת��
				ADCCP = 0x00;			 //ADC0
				while (!ADCSTA){}			// wait for end of conversion
				x0 = ADCDAT>>16;		 //ת��֮������ݸ�x0
                flag=1;					 //һ�β���ת������

					
//				DAC0DAT=ADCDAT;

           }
		T1CLRI = 0;
	}

} 


