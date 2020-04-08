#include <REGX51.H>
#include <stdio.h>

#define frequencyOutputPin P3_0
#define baseDeTiempo	55535
#define fixedFrequencyValue 2000

#define RowA P2_3     //RowA
#define RowB P2_4     //RowB
#define RowC P2_5     //RowC
#define RowD P2_6     //RowD

#define C1 P2_2     //Column1
#define C2 P2_1     //Column2
#define C3 P2_0     //Column3

//Variables globales
unsigned int timer0Ticks;
unsigned int frequencyValue;
unsigned char resistanceCounter;

unsigned char ScanKeyboard()
{
	P2 = 255;
	
	C1 = 0;
	C2 = 1;
	C3 = 1;
	
	if (RowA == 0) { while (RowA==0); return 1; }
	if (RowB == 0) { while (RowB==0); return 4; }
	if (RowC == 0) { while (RowC==0); return 7; }
	//* Dont care.
	
	C1 = 1;
	C2 = 0;
	C3 = 1;
	
	if (RowA == 0) { while (RowA==0); return 2; }
	if (RowB == 0) { while (RowB==0); return 5; }
	if (RowC == 0) { while (RowC==0); return 8; }
	if (RowD == 0) { while (RowD==0); return 0; }		
	
	C1 = 1;
	C2 = 1;
	C3 = 0;
	
	if (RowA == 0) { while (RowA==0); return 3; }
	if (RowB == 0) { while (RowB==0); return 6; }
	if (RowC == 0) { while (RowC==0); return 9; }
	//# Dont care.
	
	return 10;
}

void Timer0Init()
{
	timer0Ticks = 0;
	TR0 = 0;			//Detiene el Timer0.
	TMOD &= 0xF0;	//Limpia  solo la parte baja de TMOD correspondiente a Timer0.
	TMOD |= 2;
	
	TL0 = 0;
	TH0 = 0;
	
	PT0 = 0;			//Prioridad baja.
	TR0 = 1; 			//Habilita el Timer.
	ET0 = 1;			//Habilita la interrupción por desbordamiento del Timer0.
}

void Int0Init()
{
	IT0 = 1;		//Activado por transciente negativo.
	EX0 = 1;
}

void Timer0_isr() interrupt TF0_VECTOR
{	
	ET0 = 0;
	timer0Ticks++;
	
	if(timer0Ticks == fixedFrequencyValue/frequencyValue)
	{
		frequencyOutputPin = !frequencyOutputPin;
		timer0Ticks = 0;
	}
	ET0 = 1;
}

void ChangeResistance() interrupt IE0_VECTOR
{
	EX0 = 0;
	if(resistanceCounter == 10)
		resistanceCounter = 0;
	else
		resistanceCounter++;
	
	P1 = resistanceCounter;
	EX0 = 1;
}


void main()
{
	unsigned char x, y = 10;
	
	IE = 0;			//Establece estado inicial de las interrupciones.
	frequencyOutputPin = 0;
	frequencyValue = 0;
	resistanceCounter = 0;
	P1 = 0;
	
	for(x = 0; x < 3; x++)
	{
		while(y == 10)
			y = ScanKeyboard();
		
		frequencyValue = (frequencyValue*10) + y;
		y = 10;	
	}
	
	if(frequencyValue > 100)
		frequencyValue = 100;
	
	Timer0Init();
	Int0Init();
	EA = 1; 		//Habilita las interrupciones globales
	
	while(1)
	{}
	
}