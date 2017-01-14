// ***** 0. Documentation Section *****
// TableTrafficLight.c for (Lab 10 edX), Lab 5 EE319K
// Runs on LM4F120/TM4C123
// Program written by: put your names here
// Date Created: 1/24/2015 
// Last Modified: 3/2/2016 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 5
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********
// east/west red light connected to PA4
// east/west yellow light connected to PA3
// east/west green light connected to PA2
// north/south facing red light connected to PA7
// north/south facing yellow light connected to PA6
// north/south facing green light connected to PA5
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include <stdint.h>
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"


// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define WaitSouth 0
#define GoSouth 1
#define WaitWest 2
#define GoWest 3
#define WaitWalkSouth 4
#define WaitWalkWest 5
#define Walk 6
#define FinishWalkOn1 7
#define FinishWalkOff1 8
#define FinishWalkOn2 9
#define FinishWalkOff2 10
#define FinishWalk 11

void Init(void);

uint8_t input;

struct state {
	uint8_t LightOut;
	uint8_t WalkOut;
	uint32_t wait;
	uint8_t next[8];
};

typedef struct state state_t;

state_t FSM[12] = {
	{0x88, 0x02, 25, {GoSouth, GoSouth, GoSouth, GoSouth, GoSouth, GoSouth, GoSouth, GoSouth}}, //WaitSouth
	{0x30, 0x02, 150, {GoSouth, WaitWest, GoSouth, WaitWest, WaitWalkSouth, WaitWalkSouth, WaitWalkSouth, WaitWest}}, //GoSouth
	{0x50, 0x02, 25, {GoWest, GoWest, GoWest, GoWest, GoWest, GoWest, GoWest, GoWest}}, //WaitWest
	{0x84, 0x02, 150, {GoWest, GoWest, WaitSouth, WaitSouth, WaitWalkWest, WaitWalkWest, WaitWalkWest, WaitWalkWest}}, //GoWest
	{0x50, 0x02, 25, {Walk, Walk, Walk, Walk, Walk, Walk, Walk, Walk}}, //WaitWalkSouth
	{0x88, 0x02, 25, {Walk, Walk, Walk, Walk, Walk, Walk, Walk, Walk}}, //WaitWalkWest
	{0x90, 0x08, 75, {FinishWalkOn1, FinishWalkOn1, FinishWalkOn1, FinishWalkOn1, FinishWalkOn1, FinishWalkOn1, FinishWalkOn1, FinishWalkOn1}}, //Walk
	{0x90, 0x02, 15, {FinishWalkOff1, FinishWalkOff1, FinishWalkOff1, FinishWalkOff1, FinishWalkOff1, FinishWalkOff1, FinishWalkOff1, FinishWalkOff1}},
	{0x90, 0x00, 15, {FinishWalkOn2, FinishWalkOn2, FinishWalkOn2, FinishWalkOn2, FinishWalkOn2, FinishWalkOn2, FinishWalkOn2, FinishWalkOn2}},
	{0x90, 0x02, 15, {FinishWalkOff2, FinishWalkOff2, FinishWalkOff2, FinishWalkOff2, FinishWalkOff2, FinishWalkOff2, FinishWalkOff2, FinishWalkOff2}},
	{0x90, 0x00, 15, {FinishWalk, FinishWalk, FinishWalk, FinishWalk, FinishWalk, FinishWalk, FinishWalk, FinishWalk}},
	{0x90, 0x02, 15, {Walk, GoWest, GoSouth, GoSouth, Walk, GoWest, GoSouth, GoSouth}}
};

uint8_t cstate;

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
 
	SysTick_Init();
	
  Init();	//initialize ports
	
	cstate = GoSouth;
	
	EnableInterrupts();
	
  while(1){
									//masking here to make everything friendly
		GPIO_PORTA_DATA_R = (GPIO_PORTA_DATA_R & 0x03) | FSM[cstate].LightOut; 		//output
		GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & 0xF5) | FSM[cstate].WalkOut;			//output
		SysTick_Wait10ms(FSM[cstate].wait);																				//wait		
		input = GPIO_PORTE_DATA_R;																								//input
		cstate = FSM[cstate].next[input];																					//go to next state
		
  }
}

void Init(){
	SYSCTL_RCGC2_R |= 0x31;
	
  SysTick_Wait10ms(1);		
	
	GPIO_PORTA_DIR_R |= 0xFC;		//PA 2-7 outputs
	GPIO_PORTA_AFSEL_R &= 0x03;	
	GPIO_PORTA_DEN_R |= 0xFC;
	
	GPIO_PORTE_DIR_R &= 0xF8; // PE 0-2 inputs
	GPIO_PORTE_AFSEL_R &= 0xF8;	
	GPIO_PORTE_DEN_R |= 0x07;
	
	GPIO_PORTF_DIR_R |= 0x0A;		//PF 1,3 outputs
	GPIO_PORTF_AFSEL_R &= 0xF5;	
	GPIO_PORTF_DEN_R |= 0x0A; 	
}
