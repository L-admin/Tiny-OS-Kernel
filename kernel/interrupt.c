#include "interrupt.h"
#include "stdint.h"
#include "global.h"

#define IDT_DESC_CNT 0x21	// 

struct gate_sesc
{
	uint16_t func_offset_low_word;
	uint16_t selector;
	uint8_t dcount;
	
};