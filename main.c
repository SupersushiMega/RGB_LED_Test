#include <stdlib.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define GreenPlus !(PIND & (1<<PD4))	//Button for increasing Brightness of green LED
#define GreenMinus !(PINB & (1<<PB6))	//Button for decreasing Brightness of green LED
#define RedPlus !(PIND & (1<<PD3))	//Button for increasing Brightness of red LED
#define RedMinus !(PIND & (1<<PD6))	//Button for decreasing Brightness of red LED
#define BluePlus !(PIND & (1<<PD5))	//Button for increasing Brightness of blue LED
#define BlueMinus !(PINB & (1<<PB7))	//Button for decreasing Brightness of blue LED

#define LedRedON PORTB |= (1<<PB2)	//Set Output for Red LED to active
#define LedGreenON PORTB |= (1<<PB1)	//Set Output for Green LED to active
#define LedBlueON PORTB |= (1<<PB3)	//Set Output for Blue LED to active

#define LedRedOFF PORTB &= ~(1<<PB2)	//Set Output for Red LED to inactive
#define LedGreenOFF PORTB &= ~(1<<PB1)	//Set Output for Green LED to inactive
#define LedBlueOFF PORTB &= ~(1<<PB3)	//Set Output for Blue LED to inactive

uint8_t ISR_zaehler = 0;	//Counter used for TIMER0
ISR (TIMER0_OVF_vect)
{
	TCNT0 = 0;
	ISR_zaehler++;	//Increase ISR_zaehler by 1
	if(ISR_zaehler == 120)	//Reset ISR_zaehler when it reaches 120
	{
		ISR_zaehler = 0;
	}
}//End of ISR


int main(void)
{
	DDRB |= (1<<PB1) | (1<<PB2) | (1<<PB3);
	DDRB &= ~((1<<PB6) | (1<<PB7));
	DDRD &= ~((1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6)); 
	
	//Konfiguration Timer Overflow
	//==================================================================
	TCCR0	= 0x00;
	TCCR0	= 0x04;
	TIMSK	|= (1 << TOIE0);
	TIFR |= (1 << TOV0);
	sei();
	//==================================================================
	
	//declare and initialise Variables
	//==================================================================
	uint8_t EepromLoaded = 0;	//Variable used to Check if eeprom has been loaded
	
	uint8_t PWMcount = 0;	//Counter used to generate PWM signal
	
	uint8_t PWMon_green = 1;	//Green value for PWM
	uint8_t PWMon_red = 1;	//Red value for PWM
	uint8_t PWMon_blue = 1;	//Blue value for PWM
	
	int8_t GreenActive = 1;	//Variable used to check if Green is active
	int8_t RedActive = 1;	//Variable used to check if Red is active
	int8_t BlueActive = 1;	//Variable used to check if Blue is active
	
	uint8_t GreenPushedPlus = 0;	//Variable used to check if GreenPlus has been Pushed on the last Check
	uint8_t RedPushedPlus = 0;	//Variable used to check if RedPlus has been Pushed on the last Check
	uint8_t BluePushedPlus = 0;	//Variable used to check if BluePlus has been Pushed on the last Check
	
	uint8_t GreenPushedMinus = 0;	//Variable used to check if GreenMinus has been Pushed on the last Check
	uint8_t RedPushedMinus = 0;	//Variable used to check if RedMinus has been Pushed on the last Check
	uint8_t BluePushedMinus = 0;	//Variable used to check if BlueMinus has been Pushed on the last Check
	
	uint8_t ActiviationCounterGreenPlus = 0;	//Counter used to store the amount of times GreenPlus has been pushed in the last Second
	uint8_t ActiviationCounterRedPlus = 0;	//Counter used to store the amount of times RedPlus has been pushed in the last Second
	uint8_t ActiviationCounterBluePlus = 0;	//Counter used to store the amount of times BluePlus has been pushed in the last Second
	
	uint8_t ActiviationCounterGreenMinus = 0;	//Counter used to store the amount of times GreenMinus has been pushed in the last Second
	uint8_t ActiviationCounterRedMinus = 0;		//Counter used to store the amount of times RedMinus has been pushed in the last Second
	uint8_t ActiviationCounterBlueMinus = 0;	//Counter used to store the amount of times BlueMinus has been pushed in the last Second
	
	uint8_t hasExecuted = 0;	//Variable used to check if the input of the last second has been Proccessed
	
	uint16_t cycle = 0;	//Variable used to reduce the amount of times the Input gets checked.
	//==================================================================
	
	while(1)
	{ 
		//Turn Green Led on or off
		//==============================================================
		if(GreenActive == 1)	//If GreenActive is a one check if Output should be active
		{
			if(PWMcount < PWMon_green)	//if PWMcount is smaller than PWMon_green turn Output active else output is inactive 
			{
				LedGreenON;	//Set Output of Green Led to active
			}
			else
			{
				LedGreenOFF;	//Set Output of Green Led to inactive
			}
		}
		else
		{
			LedGreenOFF;	//Set Output of Green Led to inactive
		}
		//==============================================================
		
		//Turn Red Led on or off
		//==============================================================
		if(RedActive == 1)	//If RedActive is a one check if Output should be active
		{
			if(PWMcount < PWMon_red)	//if PWMcount is smaller than PWMon_red turn Output active else output is inactive 
			{
				LedRedON;	//Set Output of Red Led to active
			}
			else
			{
				LedRedOFF;	//Set Output of Red Led to inactive
			}
		}
		else
		{
			LedRedOFF;	//Set Output of Red Led to inactive
		}
		//==============================================================
		
		//Turn Blue Led on or off
		//==============================================================
		if(BlueActive == 1)	//If BlueActive is a one check if Output should be active
		{
			if(PWMcount < PWMon_blue)	//if PWMcount is smaller than PWMon_blue turn Output active else output is inactive 
			{
				LedBlueON;	//Set Output of Blue Led to active
			}
			else
			{
				LedBlueOFF;	//Set Output of Blue Led to inactive
			}
		}
		else
		{
			LedBlueOFF;	//Set Output of Blue Led to inactive
		}
		//==============================================================
		PWMcount++;	//Increase PWMcount by 1
		//==============================================================
		
		cycle++;	//increase cycle by one
		if (cycle % 2000 == 0)	//Check for input every 2000 cycles
		{
			//Check for Inputs on Plus Inputs
			//==============================================================
			if(GreenPlus && (GreenPushedPlus == 0))	//if GreenPlus is pushed and has not been pushed previously
			{
				ActiviationCounterGreenPlus++;	//Increase counter by 1
				GreenPushedPlus = 1;	//set Pushed to 1
			}
			else
			{
				if(!GreenPlus)	//if GreenPlus is not Pushed reset Pushed variable
				{
					GreenPushedPlus = 0;	//Reset Pushed Variable
				}
			}
			
			if(RedPlus && (RedPushedPlus == 0))	//if RedPlus is pushed and has not been pushed previously
			{
				ActiviationCounterRedPlus++;	//Increase counter by 1
				RedPushedPlus = 1;	//set Pushed to 1
			}
			else
			{
				if(!RedPlus)	//if RedPlus is not Pushed reset Pushed variable
				{
					RedPushedPlus = 0;	//Reset Pushed Variable
				}
			}
			
			if(BluePlus && (BluePushedPlus == 0))	//if BluePlus is pushed and has not been pushed previously
			{
				ActiviationCounterBluePlus++;	//Increase counter by 1
				BluePushedPlus = 1;	//set Pushed to 1
			}
			else
			{
				if(!BluePlus)	//if BluePlus is not Pushed reset Pushed variable
				{
					BluePushedPlus = 0;	//Reset Pushed Variable
				}
			}
			//==============================================================
			
			//Check for Inputs on Minus Inputs
			//==============================================================
			if(GreenMinus && (GreenPushedMinus == 0))	//if GreenMinus is pushed and has not been pushed previously
			{
				ActiviationCounterGreenMinus++;	//Increase counter by 1
				GreenPushedMinus = 1;	//set Pushed to 1
			}
			else
			{
				if(!GreenMinus)	//if GreenMinus is not Pushed reset Pushed variable
				{
					GreenPushedMinus = 0;	//Reset Pushed Variable
				}
			}
			
			if(RedMinus && (RedPushedMinus == 0))	//if RedMinus is pushed and has not been pushed previously
			{
				ActiviationCounterRedMinus++;	//Increase counter by 1
				RedPushedMinus = 1;	//set Pushed to 1
			}
			else
			{
				if(!RedMinus)	//if RedMinus is not Pushed reset Pushed variable
				{
					RedPushedMinus = 0;	//Reset Pushed Variable
				}
			}
			
			if(BlueMinus && (BluePushedMinus == 0))	//if BlueMinus is pushed and has not been pushed previously
			{
				ActiviationCounterBlueMinus++;	//Increase counter by 1
				BluePushedMinus = 1;	//set Pushed to 1
			}
			else
			{
				if(!BlueMinus)	//if BlueMinus is not Pushed reset Pushed variable
				{
					BluePushedMinus = 0;	//Reset Pushed Variable
				}
			}
			//==============================================================
		}
		
		if(ISR_zaehler < 10)	//if ISR_zaehler is equal to 0 one second has passed and the code can be executed
		{
			if (hasExecuted == 0)	//Check if inputs have already been poccessed
			{
				//Proccess Combined Inputs
				//======================================================
				if ((ActiviationCounterGreenMinus > 0) && (ActiviationCounterRedMinus > 0) && (ActiviationCounterBlueMinus > 0))	//when all Minus inputs have been pushed Store current data
				{
					eeprom_write_byte((uint8_t*)0, PWMon_green);	//Write green Value to eeprom
					eeprom_write_byte((uint8_t*)8, PWMon_red);	//Write red Value to eeprom
					eeprom_write_byte((uint8_t*)16, PWMon_blue);	//Write blue Value to eeprom
				}
				
				if ((ActiviationCounterGreenPlus > 0) && (ActiviationCounterRedPlus > 0) && (ActiviationCounterBluePlus > 0))	//when all Plus inputs have been pushed reset eeprom
				{
					eeprom_write_byte((uint8_t*)0, 0);	//reset green Value in eeprom
					eeprom_write_byte((uint8_t*)8, 0);	//reset red Value in eeprom
					eeprom_write_byte((uint8_t*)16, 0);	//reset blue Value in eeprom
				}
				//Proccess Plus Input
				//==========================================================
				if(ActiviationCounterGreenPlus == 1)	//if GreenPlus has been pushed once increase PWMon_green by 1
				{
					PWMon_green += 10;	//increase PWMon_green by 10
				}
				else
				{
					if(ActiviationCounterGreenPlus > 1)	//if GreenPlus has been pushed more than once set GreenActive to 1
					{
						GreenActive = 1;	//set GreenActive to 1
					}
				}
				
				if(ActiviationCounterRedPlus == 1)	//if RedPlus has been pushed once increase PWMon_red by 1
				{
					PWMon_red += 10;	//increase PWMon_red by 10
				}
				else
				{
					if(ActiviationCounterRedPlus > 1)	//if RedPlus has been pushed more than once set RedActive to 1
					{
						RedActive = 1;	//set RedActive to 1
					}
				}
				
				if(ActiviationCounterBluePlus == 1)	//if BluePlus has been pushed once increase PWMon_blue by 1
				{
					PWMon_blue += 10;	//increase PWMon_blue by 10
				}
				else
				{
					if(ActiviationCounterBluePlus > 1)	//if BluePlus has been pushed more than once set BlueActive to 1
					{
						BlueActive = 1;	//set BlueActive to 1
					}
				}
				//==========================================================
				
				//Proccess Minus Input
				//==========================================================
				if(ActiviationCounterGreenMinus == 1)	//if GreenMinus has been pushed once increase PWMon_green by 1
				{
					PWMon_green -= 10;	//decrease PWMon_green by 10
				}
				else
				{
					if(ActiviationCounterGreenMinus > 1)	//if GreenMinus has been pushed more than once set GreenActive to 1
					{
						GreenActive = 0;	//set GreenActive to 0
					}
				}
				
				if(ActiviationCounterRedMinus == 1)	//if RedMinus has been pushed once increase PWMon_red by 1
				{
					PWMon_red -= 10;	//decrease PWMon_red by 10
				}
				else
				{
					if(ActiviationCounterRedMinus > 1)	//if RedMinus has been pushed more than once set RedActive to 1
					{
						RedActive = 0;	//set RedActive to 0
					}
				}
				
				if(ActiviationCounterBlueMinus == 1)	//if BlueMinus has been pushed once increase PWMon_blue by 1
				{
					PWMon_blue -= 10;	//decrease PWMon_blue by 10
				}
				else
				{
					if(ActiviationCounterBlueMinus > 1)	//if BlueMinus has been pushed more than once set BlueActive to 1
					{
						BlueActive = 0;	//set BlueActive to 0
					}
				}
				//======================================================
				
				//Reset Activation Counters
				//======================================================
				ActiviationCounterGreenPlus = 0;
				ActiviationCounterRedPlus = 0;
				ActiviationCounterBluePlus = 0;
				ActiviationCounterGreenMinus = 0;
				ActiviationCounterRedMinus = 0;
				ActiviationCounterBlueMinus = 0;
				hasExecuted = 1;
				//======================================================
			}
		}
		else
		{
			hasExecuted = 0;	//reset variable used to check if Inputs have been proccesed to 0
		}
		//Load Data
		//==============================================================
		if (EepromLoaded == 0)	//if Eeprom has not been loaded
		{
			PWMon_green = eeprom_read_byte((uint8_t*)0);	//load green value from eeprom
			PWMon_red = eeprom_read_byte((uint8_t*)8);	//load red value from eeprom
			PWMon_blue = eeprom_read_byte((uint8_t*)16);	//load blue value from eeprom
			EepromLoaded = 1;	//reset variable used to check if eeprom has been loaded to 1
		}
		//==============================================================
	} //end while
}//end of main




