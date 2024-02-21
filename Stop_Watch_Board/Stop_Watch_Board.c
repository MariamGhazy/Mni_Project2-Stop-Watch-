/*
 * Stop_Watch.c
 *
 *   Created on: Feb 3, 2024
 *       Author: Mariam Ghazy
 *  Description: Mini_Project2 (Stop Watch)
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* Global variables contain the Time  */
unsigned char Sec1 = 0;
unsigned char Sec2 = 0;
unsigned char Min1 = 0;
unsigned char Min2 = 0;
unsigned char Hour1 = 0;
unsigned char Hour2 = 0;
/*============================TIMER_1_INITIALIZATION_CTC_MODE============================*/
/* Description:
 * For System Clock = 16Mhz and prescaler F_CPU/1024.
 * Timer frequency will be around 15625 HZ, Ttimer = 64us
 * Compare interrupt will be generated every one second-> second/64us
 *  so we set compare value to 15625 to count one second.
 */

/* Interrupt Service Routine for Timer1 compare mode */
ISR(TIMER1_COMPA_vect)
{
	Sec1++;
		/* Increment Sec1 by one  With every interrupt*/
		if(Sec1 == 10)
		{
			Sec1 =0;
			Sec2++;
		}
		/* When it reached 60 second increment one minute*/
		if(Sec2==6 && Sec1==0)
		{
			Sec2=0;
			Min1++;

		}
		if(Min1==10)
		{
			Min1=0;
			Min2++;
		}
		/* When it  reached 60 minute increment one hour*/
		if(Min1==0 && Min2 ==6)
		{
			Min2=0;
			Hour1++;
		}
		if(Hour1==10)
		{
			Hour1=0;
			Hour2++;
		}

}

void Timer1_CTC_Mode_Init(void)
{
	TCNT1 = 0;		/* Set timer1 initial count to zero */

	OCR1A = 15625;    /* Set the Compare value to 15625 to count one second */

	TIMSK |= (1<<OCIE1A); /* Enable Timer1 Compare A Interrupt */

	/* Configure timer control register TCCR1A
	 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
	 * 2. FOC1A=1 FOC1B=0
	 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4)
	 */
	TCCR1A = (1<<FOC1A);

	/* Configure timer control register TCCR1B
	 * 1. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
	 * 2. Prescaler = F_CPU/8 CS10=1 CS11=0 CS12=1
	 */
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
}
/*==============================EXTERNAL_INIT0_INITIALIZATION==============================*/
void INT0_Init(void)
{
	MCUCR |=(1<<ISC01);   /* Trigger INT0 with Falling edge ISCS01=1, ISC00=0 */
	DDRD &=~(1<<PD2);	 /* Configure PD2/INI0 as an Input pin*/
	PORTD |=(1<<PD2);   /* Enable Internal PULL-UP Resistor*/
	GICR |=(1<<INT0);    /*Enable external interrupt pin INT0*/
}
/* Interrupt Service Routine for External INT0 */
ISR(INT0_vect)
{
	/* Reset Stop watch  */
	Sec1=0;
	Sec2=0;
}
/*==============================EXTERNAL_INIT1_INITIALIZATION==============================*/

void INT1_Init(void)
{
	MCUCR |=(1<<ISC11) |(1<<ISC10);   /* Trigger INT0 with Rising edge ISCS10=1, ISC11=0 */
	DDRD &=~(1<<PD3);	 /* Configure PD3/INI1 as an Input pin*/
	PORTD |=(1<<PD3);   /* Enable Internal PULL-UP Resistor*/
	GICR |=(1<<INT1);    /*Enable external interrupt pin INT1*/
}
/* Interrupt Service Routine for External INT1 */
ISR(INT1_vect)
{
	/* Timer stop --> Disable its clk */
	TCCR1B &=~ (1<<CS12) &~ (1<<CS11) &~ (1<<CS10);

}
/*==============================EXTERNAL_INIT2_INITIALIZATION==============================*/

void INT2_Init(void)
{
	MCUCSR &= ~(1<<ISC2);           /*Trigger INT2 with the Falling edge ISC2=0*/
	DDRB &= ~(1<<PB2);	             /* Configure PB2/INI2 as an Input pin*/
	PORTB |=(1<<PB2);	             /* Enable Internal PULL-UP Resistor*/
	GICR |=(1<<INT2);               /*Enable external interrupt pin INT2*/
}

/* Interrupt Service Routine for External INT2 */
ISR(INT2_vect)
{
	/* Enable Timer again --> Enable its clk */
	TCCR1B |= (1<<CS12) |  (1<<CS10);

}

/*==============================Function to display Time on 7-Segments====================== */
void Eta32mini_SevenSegment_Display(unsigned char value)
{
	switch(value)
	{
	case 0:
		PORTA |= (1<<1);
		PORTA |= (1<<2);
		PORTA |= (1<<3);
		PORTA |= (1<<4);
		PORTA |= (1<<5);
		PORTA |= (1<<6);
		PORTA &= ~(1<<7);
		break;
	case 1:
		PORTA &= ~(1<<1);
		PORTA |= (1<<2);
		PORTA |= (1<<3);
		PORTA &= ~(1<<4);
		PORTA &= ~(1<<5);
		PORTA &= ~(1<<6);
		PORTA &= ~(1<<7);
		break;
	case 2:
		PORTA |= (1<<1);
		PORTA |= (1<<2);
		PORTA &= ~(1<<3);
		PORTA |= (1<<4);
		PORTA |= (1<<5);
		PORTA &= ~(1<<6);
		PORTA |= (1<<7);
		break;
	case 3:
		PORTA |= (1<<1);
		PORTA |= (1<<2);
		PORTA |= (1<<3);
		PORTA |= (1<<4);
		PORTA &= ~(1<<5);
		PORTA &= ~(1<<6);
		PORTA |= (1<<7);
		break;
	case 4:
		PORTA &= ~(1<<1);
		PORTA |= (1<<2);
		PORTA |= (1<<3);
		PORTA &= ~(1<<4);
		PORTA &= ~(1<<5);
		PORTA |= (1<<6);
		PORTA |= (1<<7);
		break;
	case 5:
		PORTA |= (1<<1);
		PORTA &= ~(1<<2);
		PORTA |= (1<<3);
		PORTA |= (1<<4);
		PORTA &= ~(1<<5);
		PORTA |= (1<<6);
		PORTA |= (1<<7);
		break;
	case 6:
		PORTA |= (1<<1);
		PORTA &= ~(1<<2);
		PORTA |= (1<<3);
		PORTA |= (1<<4);
		PORTA |= (1<<5);
		PORTA |= (1<<6);
		PORTA |= (1<<7);
		break;
	case 7:
		PORTA |= (1<<1);
		PORTA |= (1<<2);
		PORTA |= (1<<3);
		PORTA &= ~(1<<4);
		PORTA &= ~(1<<5);
		PORTA &= ~(1<<6);
		PORTA &= ~(1<<7);
		break;
	case 8:
		PORTA |= (1<<1);
		PORTA |= (1<<2);
		PORTA |= (1<<3);
		PORTA |= (1<<4);
		PORTA |= (1<<5);
		PORTA |= (1<<6);
		PORTA |= (1<<7);
		break;
	case 9:
		PORTA |= (1<<1);
		PORTA |= (1<<2);
		PORTA |= (1<<3);
		PORTA &= ~(1<<4);
		PORTA &= ~(1<<5);
		PORTA |= (1<<6);
		PORTA |= (1<<7);
		break;
	}
}
void Display_7Segment(void)
{
	/* Enable PC6 in PORTC with value 1 to enable
	 * the BJT NPN transistor to connect the first 7-segment */
	PORTC = ( PORTC & 0X3F) | (1<<PC6);
	/* Then display sec1 on decoder*/
	Eta32mini_SevenSegment_Display(Sec1);
	/* Delay 2 millisecond */
	_delay_ms(2);

	/* Enable PC7 in PORTC with value 1 to enable
	 * the BJT NPN transistor to connect the second 7-segment */
	PORTC = ( PORTC & 0X3F) | (1<<PC7);
	/* Then display sec1 on decoder*/
	Eta32mini_SevenSegment_Display(Sec2);
	/* Delay 2 millisecond */
	_delay_ms(2);
}








int main(void)
{
	DDRA |= 0XFE;   /* configure pins as output from PA1 --> PA7 */
	DDRC |= (1<<PC6)|(1<<PC7);   /* Configure first 4-pins in PORTC as an output */
	PORTC &=~ (1<<PC6) &~ (1<<PC7);  /*  Disable 7-segments at the beginning*/



	SREG |=(1<<7); /*Enable I-bit Global Interrupt enable*/

	Timer1_CTC_Mode_Init(); /* Enable Timer1 */

	INT0_Init();     /*Enable External INT0*/
	INT1_Init();     /*Enable External INT1*/
	INT2_Init();     /*Enable External INT2*/
	while(1)
	{
		Display_7Segment();
	}
}

