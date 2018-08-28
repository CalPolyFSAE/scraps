#include "AVRLibrary/CPFECANLib.h"

extern CPFECANLib::MSG rxmask, txmsg, failframe;
extern uint8_t txData[8], faildata[8];

void initializeFrames(void){

	rxmask.ide = 0;
	rxmask.identifier.extended = 0;
	rxmask.rtr = 0;

	txmsg.identifier.extended = 0x220;
	txmsg.dlc = 8;
	txmsg.rtr = 0;
	txmsg.ide = 1;
	txmsg.data = txData;

	failframe.identifier.extended = 0xDEAD;
	failframe.dlc = 8;
	failframe.rtr = 0;
	failframe.ide = 1;
	failframe.data = faildata;

}