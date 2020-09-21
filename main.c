#include <stdlib.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define GreenPlus !(PINC & (1<<PC0))
#define GreenMinus !(PINC & (1<<PC1))
#define RedPlus !(PINC & (1<<PC2))
#define RedMinus !(PINC & (1<<PC3))
#define BluePlus !(PINC & (1<<PC4))
#define BlueMinus !(PINC & (1<<PC5))

#define LedGreenON PORTB |= (1<<PB1)
#define LedRedON PORTB |= (1<<PB2)
#define LedBlueON PORTB |= (1<<PB3)

#define LedGreenOFF PORTB &= ~(1<<PB1)
#define LedRedOFF PORTB &= ~(1<<PB2)
#define LedBlueOFF PORTB &= ~(1<<PB3)

uint8_t ISR_zaehler = 0;
ISR (TIMER0_OVF_vect)
{
	TCNT0 = 0;
	ISR_zaehler++;
	if(ISR_zaehler == 120)
	{
		ISR_zaehler = 0;
	}
}//End of ISR


int main(void)
{
	DDRB |= (1<<PB1) | (1<<PB2) | (1<<PB3);
	DDRD &= ~((1<<PC0) | (1<<PD1) | (1<<PD2) | (1<<PC3) | (1<<PD4) | (1<<PD5)); 
	
	//Konfiguration Timer Overflow
	//==================================================================
	TCCR0A	= 0x00;
	TCCR0B	= 0x04;
	TIMSK0	|= (1 << TOIE0);
	TIFR0 |= (1 << TOV0);
	sei();
	//==================================================================
	
	uint8_t EepromLoaded = 0;
	
	uint8_t PWMcount = 0;
	
	uint8_t PWMon_green = 0;
	uint8_t PWMon_red = 0;
	uint8_t PWMon_blue = 0;
	
	uint8_t GreenActive = 1;
	uint8_t RedActive = 1;
	uint8_t BlueActive = 1;
	
	uint8_t GreenPushedPlus = 0;
	uint8_t RedPushedPlus = 0;
	uint8_t BluePushedPlus = 0;
	
	uint8_t GreenPushedMinus = 0;
	uint8_t RedPushedMinus = 0;
	uint8_t BluePushedMinus = 0;
	
	uint8_t ActiviationCounterGreenPlus = 0;
	uint8_t ActiviationCounterRedPlus = 0;
	uint8_t ActiviationCounterBluePlus = 0;
	
	uint8_t ActiviationCounterGreenMinus = 0;
	uint8_t ActiviationCounterRedMinus = 0;
	uint8_t ActiviationCounterBlueMinus = 0;
	
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
			GreenPushedPlus = 1;	//set Pushed to 1
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
		
		if(RedPlus && (RedPushedMinus == 0))	//if RedMinus is pushed and has not been pushed previously
		{
			ActiviationCounterRedMinus++;	//Increase counter by 1
			GreenPushedMinus = 1;	//set Pushed to 1
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
		
		if(ISR_zaehler == 0)	//if ISR_zaehler is equal to 0 one second has passed and the code can be executed
		{
			//Proccess Plus Input
			//==========================================================
			if(ActiviationCounterGreenPlus == 1)	//if GreenPlus has been pushed once increase PWMon_green by 1
			{
				PWMon_green++;	//increase PWMon_green by 1
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
				PWMon_red++;	//increase PWMon_red by 1
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
				PWMon_blue++;	//increase PWMon_blue by 1
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
				PWMon_green--;	//increase PWMon_green by 1
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
				PWMon_red--;	//increase PWMon_red by 1
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
				PWMon_blue--;	//increase PWMon_blue by 1
			}
			else
			{
				if(ActiviationCounterBlueMinus > 1)	//if BlueMinus has been pushed more than once set BlueActive to 1
				{
					BlueActive = 0;	//set BlueActive to 0
				}
			}
			if(!GreenPlus)
			{
				ActiviationCounterGreenPlus = 0;
			}
			
			if(!RedPlus)
			{
				ActiviationCounterRedPlus = 0;
			}
			
			if(!BluePlus)
			{
				ActiviationCounterBluePlus = 0;
			}
			
			if(!GreenMinus)
			{
				ActiviationCounterGreenMinus = 0;
			}
			
			if(!RedMinus)
			{
				ActiviationCounterRedMinus = 0;
			}
			
			if(!BlueMinus)
			{
				ActiviationCounterBlueMinus = 0;
			}
			_delay_us(1);
		}
	} //end while
}//end of main




