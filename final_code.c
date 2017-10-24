#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include<avr/sfr_defs.h>

/*#define LCD_DATA PORTA // LCD data port
#define ctrl PORTB
#define en PB2 // enable signal
#define rw PB1 // read/write signal
#define rs PB0 // register select signal*/
  
#define FREQ 12000000
#define prescaler 1

#define lever PORTB
#define up PB0
#define down PB1

/* 	left motor 	forward PB3
				reverse PD7
	right motor forward PD5
				reverse PD4*/

int l1=0,l2=0,l3=0,r4=0,r5=0,r6=0,cen=0;
int front_left_s,front_right_s,back_left_s,back_right_s,back_left_n,back_right_n;
int left_val,right_val,node = 0,f=0;
int b1,b2,b3,b4;

void t0_pwm_init();
void t1_pwm_init();
void t2_pwm_init();

void value();
void value_node();
void value_bump();
void value_back();

void straight_forw();
void straight_back();

void stop();

void turn_left();
void turn_right();

void forw_pwm();
void back_pwm();

void node_count();

void forw_pwm_slow();
void back_pwm_slow();

void straight_forw_slow();
void straight_back_slow();

void forw_left_drive_slow(int k);
void forw_right_drive_slow(int k);
void back_left_drive_slow(int k);
void back_right_drive_slow(int k);

void straight_forw_cust(int k);
void forw_left_drive_cust(int k, int l);
void forw_right_drive_cust(int k, int l);
void forw_pwm_cust(int k);

int main()
{
	DDRA=0xFF; // making LCD_DATA port as output port
	DDRB=0x07; // making signal as out put
	DDRD=0x00;
	t0_pwm_init();
	t1_pwm_init();
	t2_pwm_init();
	sei(); // enable global interrupt

	lever = (1<<up)|(1<<down);
	_delay_ms(50);

	for(int i=0;i<7000;i++)
	{
		forw_pwm_cust(80);
	}
	
	value_bump();
	while(b1 == 0 || b3 == 0)
	{
		value_bump();
up1:	forw_pwm_cust(50);
	}
	_delay_ms(100);
	value_bump();
	if(b1 == 0)
	goto up1;
	stop();
	_delay_ms(500);

	lever = (1<<up)|(0<<down);
	_delay_ms(2000);
	value_bump();
	while(b2 == 0)
	{
		value_bump();
	}
	lever = (1<<up)|(1<<down);		//down
	_delay_ms(1000);
	
	straight_back_slow();
	_delay_ms(1000);
	stop();

	turn_right();
	stop();
	turn_right();
	stop();
	_delay_ms(500);

	for(int i=0;i<5000;i++)
	{
		forw_pwm_cust(50);
	}
	
	for(int i=0;i<6000;i++)
	{
		forw_pwm_cust(50);
	}
	stop();

	turn_left();
	_delay_ms(500);

	value_bump();
	while(b1 == 0 || b3 == 0)
	{
		value_bump();
up2:	forw_pwm_cust(40);
	}
	_delay_ms(100);
	value_bump();
	if(b1 == 0 || b3 == 0)
	goto up2;
	stop();
	_delay_ms(500);

	lever = (0<<up)|(1<<down);
	_delay_ms(2000);
	value_bump();
	while(b2 == 0)
	{
		value_bump();
	}
	lever = (1<<up)|(1<<down);		//down
	_delay_ms(1000);

	straight_back_slow();
	_delay_ms(2000);
	stop();

	turn_right2();
	_delay_ms(500);

	for(int i=0;i<3000;i++)
	{
		forw_pwm_cust(50);
	}
	stop();

	turn_right2();
	_delay_ms(100);
	turn_right2();

	value_bump();
	while(b1 == 0)
	{
up3:	straight_back_slow();
		value_bump();
	}
	_delay_ms(100);
	value_bump();
	if(b1 == 0)
	goto up3;
	stop();

	lever = (1<<up)|(0<<down);
	_delay_ms(2000);
	value_bump();
	while(b2 == 0)
	{
		value_bump();
	}
	lever = (1<<up)|(1<<down);		//down


	return 0;


}
 
//******************************************************************//

void t0_pwm_init() 										//fun1
{
// WGM0[1:0]= 01, for Phase Correct PWM mode
// COM0[1:0]= 10, to select non inveting mode
// CS0[2:0] =001. for no prescaler 
TCCR0=(1<<WGM00)|(2<<COM00)|(1<<CS00);
DDRB|=(1<<PB3); // selcetOC0 as output pin
DDRC|=0x00;
}

void t1_pwm_init() //// intiatialize of timer0			//fun2
{
TCCR1A=(2<<COM1A0)|(2<<COM1B0)|(1<<WGM10);
TCCR1B=(0<<WGM12)|(1<<CS10);
DDRD|=(1<<PD4)|(1<<PD5);
}

void t2_pwm_init() //// intiatialize of timer0			//fun3
{
// WGM0[1:0]= 01, for Phase Correct PWM mode
// COM0[1:0]= 10, to select non inveting mode
// CS0[2:0] =001. for no prescaler 
TCCR2=(1<<WGM20)|(2<<COM20)|(1<<CS20);
DDRD|=(1<<PD7); // selcetOC0 as output pin
}

//******************************************************************//

void value()											//fun4
{
	if(bit_is_set(PINC,PC1)) //If PA0=1
	{
	l1 = 1;
	//LCD_write('A');
	}
	else
	{
	l1 = 0;
	//LCD_write('B');
	}

	if(bit_is_set(PINC,PC6)) //If PA0=1
	{
	l2 = 1;
	//LCD_write('C');
	}
	else
	{
	l2 = 0;
	//LCD_write('D');
	}

	if(bit_is_set(PINC,PC7)) //If PA0=1
	{
		l3 = 1;
	//	LCD_write('G');
	}
	else
	{
		l3 = 0;
	//	LCD_write('H');
	}

	if(bit_is_set(PINB,PB4)) //If PA0=1
	{
		cen = 1;
	//	LCD_write('G');
	}
	else
	{
		cen = 0;
	//	LCD_write('H');
	}

 	if(bit_is_set(PINB,PB5)) //If PA0=1
	{
		r4 = 1;
	//	LCD_write('I');
	}
	else
	{
		r4 = 0;
	//	LCD_write('J');
	}

 	if(bit_is_set(PINB,PB6)) //If PA0=1
	{
		r5 = 1;
	//	LCD_write('K');
	}
	else
	{
		r5 = 0;
	//	LCD_write('L');
	}

 	if(bit_is_set(PINB,PB7)) //If PA0=1
	{
		r6 = 1;
	//	LCD_write('M');
	}
	else
	{
		r6 = 0;
	//	LCD_write('N');
	}
}

void value_node()											//fun5
	{
		/*if(bit_is_set(PINB,PB7)) //If PA0=1
		{
			front_right_s = 1;
			//LCD_write('A');
		}
		else
		{
			front_right_s = 0;
			//LCD_write('B');
		}

		if(bit_is_set(PINC,PC1)) //If PA0=1
		{
			front_left_s = 1;
			//LCD_write('C');
		}
		else
		{
			front_left_s = 0;
			//LCD_write('D');
		}*/

		if(bit_is_set(PIND,PD0)) //If PA0=1
		{
			back_left_s = 1;
			//	LCD_write('G');
		}
		else
		{
			back_left_s = 0;
			//	LCD_write('H');
		}

 		if(bit_is_set(PIND,PD1)) //If PA0=1
		{
			back_right_s = 1;
			//	LCD_write('I');
		}
		else
		{
			back_right_s = 0;
		//	LCD_write('J');
		}
	}


void value_bump()											//fun6
{
	if(bit_is_set(PIND,PD6)) //If PA0=1
		{
			b1 = 1;
			//	LCD_write('I');
		}
		else
		{
			b1 = 0;
		//	LCD_write('J');
		}

		if(bit_is_set(PINC,PC0)) //If PA0=1
		{
			b2 = 1;
			//	LCD_write('I');
		}
		else
		{
			b2 = 0;
		//	LCD_write('J');
		}

		if(bit_is_set(PIND,PD0)) //If PA0=1
		{
			b3 = 1;
			//	LCD_write('G');
		}
		else
		{
			b3 = 0;
			//	LCD_write('H');
		}
		if(bit_is_set(PIND,PD1)) //If PA0=1
		{
			b4 = 1;
			//	LCD_write('I');
		}
		else
		{
			b4 = 0;
		//	LCD_write('J');
		}
}

void value_back()										//fun7
{
	if(bit_is_set(PIND,PD2)) //If PA0=1
	{
		back_left_n = 1;
	}
	else
	{
		back_left_n = 0;
	}

	if(bit_is_set(PIND,PD3)) //If PA0=1
	{
		back_right_n = 1;
	}
	else
	{
		back_right_n = 0;
	}
}

//******************************************************************//

void straight_forw()												//fun8
{
	OCR0 = (uint8_t)((80*256)/100);
	OCR2 = 0;
	OCR1A = (uint8_t)((80*256)/100);
	OCR1B = 0;
}

void straight_back()												//fun9
{
	OCR2 = (uint8_t)((80*256)/100);
	OCR0 = 0;
	OCR1B = (uint8_t)((80*256)/100);
	OCR1A = 0;
}

void straight_back_slow()												//fun9
{
	OCR2 = (uint8_t)((40*256)/100);
	OCR0 = 0;
	OCR1B = (uint8_t)((40*256)/100);
	OCR1A = 0;
}


//******************************************************************//

void stop()															//fun10
{
	OCR0 = 0;
	OCR2 = 0;
	OCR1A = 0;
	OCR1B = 0;
}

//******************************************************************//

void turn_left()														//fun15
{
	_delay_ms(100);
	OCR2 = (uint8_t)((38*256)/100);
	OCR0 = 0;
	OCR1A = (uint8_t)((38*256)/100);
	OCR1B = 0;
	_delay_ms(1000);
	value();
	while(cen != 1)
	{
		_delay_ms(50);
		value();
upleft:	OCR2 = (uint8_t)((38*256)/100);
		OCR0 = 0;
		OCR1A = (uint8_t)((38*256)/100);
		OCR1B = 0;
	}

	OCR2 = 0;
	OCR0 = 0;
	OCR1A = 0;
	OCR1B = 0;

	_delay_ms(100);
	value();
	if(cen == 0)
	goto upleft;
}	

void turn_right()														//fun16
{
	_delay_ms(100);
	OCR0 = (uint8_t)((35*256)/100);
	OCR2 = 0;
	OCR1B = (uint8_t)((35*256)/100);
	OCR1A = 0;
	_delay_ms(700);
	value();
	while(cen != 1)
	{
		_delay_ms(50);
		value();
upright:OCR0 = (uint8_t)((35*256)/100);
		OCR2 = 0;
		OCR1B = (uint8_t)((35*256)/100);
		OCR1A = 0;
	}
	OCR2 = 0;
	OCR0 = 0;
	OCR1A = 0;
	OCR1B = 0;

	_delay_ms(50);
	value();
	if(cen == 0)
	goto upright;
}

void turn_right2()														//fun16
{
	_delay_ms(100);
	OCR0 = (uint8_t)((20*256)/100);
	OCR2 = 0;
	OCR1B = (uint8_t)((20*256)/100);
	OCR1A = 0;
	_delay_ms(1000);
	value();
	while(cen != 1)
	{
		_delay_ms(50);
		value();
upright:OCR0 = (uint8_t)((20*256)/100);
		OCR2 = 0;
		OCR1B = (uint8_t)((20*256)/100);
		OCR1A = 0;
	}
	OCR2 = 0;
	OCR0 = 0;
	OCR1A = 0;
	OCR1B = 0;

	_delay_ms(100);
	value();
	if(cen == 0)
	goto upright;
}

//******************************************************************//

void node_count()																	//fun19
{
	value();
	left_val = l1*4 + l2*2 + l3*1;
	right_val = r4*1 + r5*2 + r6*4;
	if(left_val >= 4)
	{
		if(f == 1)
		{
			node = node+1;
			f=0;
//			LCD_write(node+48);
		}
	}
	else
	{
		f=1;
	}
}

//******************************************************************//

void straight_forw_cust(int k)
{
	int duty_cycle = k;
	OCR0 = (uint8_t)((duty_cycle*256)/100);
	OCR2 = 0;
	OCR1A = (uint8_t)((duty_cycle*256)/100);
	OCR1B = 0;
}

void forw_left_drive_cust(int k, int l)											//fun24
{
	int duty_cycle1, duty_cycle2;
	duty_cycle1 = k;
	duty_cycle2 = l;
	OCR0 = (uint8_t)((duty_cycle2*256)/100);
	OCR2 = 0;
	OCR1A = (uint8_t)((duty_cycle1*256)/100);
	OCR1B = 0;
}

void forw_right_drive_cust(int k, int l)													//fun25
{
	int duty_cycle1, duty_cycle2;
	duty_cycle1 = k;
	duty_cycle2 = l;
	OCR1A = (uint8_t)((duty_cycle2*256)/100);
	OCR1B = 0;
	OCR0 = (uint8_t)((duty_cycle1*256)/100);
	OCR2 = 0;
}

void forw_pwm_cust(int k)													//fun20
	{
		int duty_cycle1;
		value();
		left_val = l1*4 + l2*2 + l3*1;
		right_val = r4*1 + r5*2 + r6*4;
		duty_cycle1 = k;
					
		if(left_val == 0 && right_val == 0)
		{
			straight_forw_cust(duty_cycle1);
		}
		else if(cen == 1)
		{
			straight_forw_cust(duty_cycle1);
		}
		else if(left_val == 1)
		{
			forw_left_drive_cust(duty_cycle1,duty_cycle1 - 5);
		}
		else if(left_val == 2 || left_val == 3)
		{
			 forw_left_drive_cust(duty_cycle1,duty_cycle1 - 10);
		}
		else if(left_val >= 4)
		{
			forw_left_drive_cust(duty_cycle1,duty_cycle1 - 30);
		}
		else if(right_val == 1)
		{
			forw_right_drive_cust(duty_cycle1,duty_cycle1 - 5);
		}
		else if(right_val == 2 || right_val == 3)
		{
			 forw_right_drive_cust(duty_cycle1,duty_cycle1 - 10);
		}
		else if(right_val >= 4)
		{
			forw_right_drive_cust(duty_cycle1,duty_cycle1 - 30);
		}
		else if(right_val >= 4 && left_val >= 4)
		{
			straight_forw_cust(duty_cycle1);
		}
		else 
		{
			straight_forw_cust(duty_cycle1);
		}
    }
