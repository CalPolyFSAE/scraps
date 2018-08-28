#define F_CPU 16000000L

#include <avr/io.h>
#include "AVRLibrary/CPFECANLib.h"
#include "frames.h"
#include "temps.h"

CPFECANLib::MSG rxbuf, rxmask, txmsg, failframe;
uint8_t txData[8];

uint8_t disable[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t enable[8] = { 0, 0, 0, 0, 1, 1, 0, 0};
uint8_t faildata[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void fail(void);
void check(void);
void temps(void);

void rxHandler(CPFECANLib::MSG *msg, uint8_t mobno){

	if(msg->identifier.extended == 0xFA57){
		enable[0] = msg->data[0];
		enable[1] = msg->data[1];
	}

	if(msg->identifier.extended & 0xC00000)
		temps();

	CPFECANLib::enableMOBAsRX(1, &rxbuf, &rxmask, 0);
}

int main(void){

	cli();

	initializeFrames();

	CPFECANLib::init(CPFECANLib::CAN_BAUDRATE::B500K, rxHandler);
	CPFECANLib::enableMOBAsRX(1, &rxbuf, &rxmask, 0);

	TCCR1A |= (1<<WGM11) | (1<<WGM10);
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS12) | (1<<CS10); 	//1024 prescaler
	// One second = 16M/1024
	// OCR1A = 0x3D09;
	// Quarter second
	OCR1A = 0xF42;


	// Output: TSMS
	DDRB |= (1<<DDB5);
	// Output: PosAIREn 
	DDRB |= (1<<DDB6);
	// Output: DC/DC disable
	DDRD |= (1<<DDD0);


	// Disable DC/DC
	PORTD |= (1<<PIN0);

	PORTB = 0;

	uint8_t invEn = 0;

	uint8_t time = 0;

	sei();

	// stage 1: startup
	while(time < 32) {

		// if(time >= 8) check();

		if(TIFR1 & (1<<OCF1A)){

			txmsg.data = disable;
			CPFECANLib::sendMsgUsingMOB(0, &txmsg);
			
			++time;

			TIFR1 |= (1<<OCF1A);

		}
			
			// CPFECANLib::sendMsgUsingMOB(0, &msg);
	}


	// stage 2: precharge
	time = 0;
	PORTB |= (1<<DDB5);

	while(time < 16) {

		check();

		if(TIFR1 & (1<<OCF1A)){

			txmsg.data = disable;
			CPFECANLib::sendMsgUsingMOB(0, &txmsg);
			
			++time;

			TIFR1 |= (1<<OCF1A);

		}
			
			// CPFECANLib::sendMsgUsingMOB(0, &msg);
	}

	PORTB |= (1<<DDB6);

	// stage 3: run

	while(1){

		check();

		if(TIFR1 & (1<<OCF1A)){

			txmsg.data = enable;
			CPFECANLib::sendMsgUsingMOB(0, &txmsg);

			TIFR1 |= (1<<OCF1A);

		}

	}

}

void fail(void){
	while(1){	
		PORTB = 0;

		if(TIFR1 & (1<<OCF1A)){
			txmsg.data = disable;
			CPFECANLib::sendMsgUsingMOB(0, &txmsg);
			CPFECANLib::sendMsgUsingMOB(2, &failframe);
			TIFR1 |= (1<<OCF1A);
			// return;
		}
	}
}

void check(void){
	// IMD OK
	if(!(PIND & (1<<PIN1))){
		failmode[0] = 1;
		fail();
	}
	// BMS OK
	if(!(PIND & (1<<PIN2))){
		faildata[0] = 2;
		fail();
	}
}