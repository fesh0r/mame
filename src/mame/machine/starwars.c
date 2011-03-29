/***************************************************************************

    Atari Star Wars hardware

    This file is Copyright Steve Baines.
    Modified by Frank Palazzolo for sound support

***************************************************************************/

#include "emu.h"
#include "includes/starwars.h"
#include "machine/x2212.h"


/* Control select values for ADC_R */
#define kPitch		0
#define kYaw		1
#define kThrust		2

/* Constants for matrix processor operations */
#define NOP			0x00
#define LAC			0x01
#define READ_ACC	0x02
#define M_HALT		0x04
#define INC_BIC		0x08
#define CLEAR_ACC	0x10
#define LDC			0x20
#define LDB			0x40
#define LDA			0x80

/* Debugging flag */
#define MATHDEBUG	0

#define MASTER_CLOCK (12096000)



static TIMER_CALLBACK( math_run_clear )
{
	starwars_state *state = machine.driver_data<starwars_state>();
	state->math_run = 0;
}


/*************************************
 *
 *  X2212 nvram store
 *
 *************************************/

WRITE8_HANDLER( starwars_nstore_w )
{
	space->machine().device<x2212_device>("x2212")->store(0);
	space->machine().device<x2212_device>("x2212")->store(1);
	space->machine().device<x2212_device>("x2212")->store(0);
}

/*************************************
 *
 *  Output latch
 *
 *************************************/

WRITE8_HANDLER( starwars_out_w )
{
	starwars_state *state = space->machine().driver_data<starwars_state>();
	switch (offset & 7)
	{
		case 0:		/* Coin counter 1 */
			coin_counter_w(space->machine(), 0, data);
			break;

		case 1:		/* Coin counter 2 */
			coin_counter_w(space->machine(), 1, data);
			break;

		case 2:		/* LED 3 */
			set_led_status(space->machine(), 2, ~data & 0x80);
			break;

		case 3:		/* LED 2 */
			set_led_status(space->machine(), 1, ~data & 0x80);
			break;

		case 4:		/* bank switch */
			memory_set_bank(space->machine(), "bank1", (data >> 7) & 1);
			if (state->is_esb)
				memory_set_bank(space->machine(), "bank2", (data >> 7) & 1);
			break;
		case 5:		/* reset PRNG */
			break;

		case 6:		/* LED 1 */
			set_led_status(space->machine(), 0, ~data & 0x80);
			break;

		case 7:		/* NVRAM array recall */
			space->machine().device<x2212_device>("x2212")->recall(~data & 0x80);
			break;
	}
}



/*************************************
 *
 *  Input port 1
 *
 *************************************/

CUSTOM_INPUT( matrix_flag_r )
{
	starwars_state *state = field->port->machine().driver_data<starwars_state>();
	/* set the matrix processor flag */
	return state->math_run ? 1 : 0;
}



/*************************************
 *
 *  ADC input and control
 *
 *************************************/

READ8_HANDLER( starwars_adc_r )
{
	starwars_state *state = space->machine().driver_data<starwars_state>();
	/* pitch */
	if (state->control_num == kPitch)
		return input_port_read(space->machine(), "STICKY");

	/* yaw */
	else if (state->control_num == kYaw)
		return input_port_read(space->machine(), "STICKX");

	/* default to unused thrust */
	else
		return 0;
}


WRITE8_HANDLER( starwars_adc_select_w )
{
	starwars_state *state = space->machine().driver_data<starwars_state>();
	state->control_num = offset;
}



/*************************************
 *
 *  Matrix Processor initialization
 *
 *************************************/

void starwars_mproc_init(running_machine &machine)
{
	starwars_state *state = machine.driver_data<starwars_state>();
	UINT8 *src = machine.region("user2")->base();
	int cnt, val;

	state->PROM_STR = auto_alloc_array(machine, UINT8, 1024);
	state->PROM_MAS = auto_alloc_array(machine, UINT8, 1024);
	state->PROM_AM = auto_alloc_array(machine, UINT8, 1024);

	for (cnt = 0; cnt < 1024; cnt++)
	{
		/* translate PROMS into 16 bit code */
		val  = (src[0x0c00 + cnt]      ) & 0x000f; /* Set LS nibble */
		val |= (src[0x0800 + cnt] <<  4) & 0x00f0;
		val |= (src[0x0400 + cnt] <<  8) & 0x0f00;
		val |= (src[0x0000 + cnt] << 12) & 0xf000; /* Set MS nibble */

		/* perform pre-decoding */
		state->PROM_STR[cnt] = (val >> 8) & 0x00ff;
		state->PROM_MAS[cnt] =  val       & 0x007f;
		state->PROM_AM[cnt]  = (val >> 7) & 0x0001;
	}

	state->math_timer = machine.scheduler().timer_alloc(FUNC(math_run_clear));
}



/*************************************
 *
 *  Matrix Processor reset
 *
 *************************************/

void starwars_mproc_reset(running_machine &machine)
{
	starwars_state *state = machine.driver_data<starwars_state>();
	state->MPA = state->BIC = 0;
	state->math_run = 0;
}



/*************************************
 *
 *  Matrix Processor execution
 *
 *************************************/

static void run_mproc(running_machine &machine)
{
	starwars_state *state = machine.driver_data<starwars_state>();

	int RAMWORD = 0;
	int MA_byte;
	int tmp;
	int M_STOP = 100000; /* Limit on number of instructions allowed before halt */
	int MA;
	int IP15_8, IP7, IP6_0; /* Instruction PROM values */
	int mptime;

	logerror("Running Matrix Processor...\n");

	mptime = 0;
	state->math_run = 1;

	/* loop until finished */
	while (M_STOP > 0)
	{

		/* each step of the matrix processor takes five clock cycles */
		mptime += 5;

		/* fetch the current instruction data */
		IP15_8 = state->PROM_STR[state->MPA];
		IP7    = state->PROM_AM[state->MPA];
		IP6_0  = state->PROM_MAS[state->MPA];

#if (MATHDEBUG)
		mame_printf_debug("\n(MPA:%x), Strobe: %x, IP7: %d, IP6_0:%x\n",state->MPA, IP15_8, IP7, IP6_0);
		mame_printf_debug("(BIC: %x), A: %x, B: %x, C: %x, ACC: %x\n",state->BIC,state->A,state->B,state->C,state->ACC);
#endif

		/* construct the current RAM address */
		if (IP7 == 0)
			MA = (IP6_0 & 3) | ((state->BIC & 0x01ff) << 2);	/* MA10-2 set to BIC8-0 */
		else
			MA = IP6_0;

		/* convert RAM offset to eight bit addressing (2kx8 rather than 1k*16)
            and apply base address offset */

		MA_byte = MA << 1;
		RAMWORD = (state->mathram[MA_byte + 1] & 0x00ff) | ((state->mathram[MA_byte] & 0x00ff) << 8);

//      logerror("MATH ADDR: %x, CPU ADDR: %x, RAMWORD: %x\n", MA, MA_byte, RAMWORD);

		/*
         * RAMWORD is the sixteen bit Math RAM value for the selected address
         * MA_byte is the base address of this location as seen by the main CPU
         * IP is the 16 bit instruction word from the PROM. IP7_0 have already
         * been used in the address selection stage
         * IP15_8 provide the instruction strobes
         */


		/* The accumulator is built from two ls299 (msb) and two ls164
         * (lsb). You can only read/write the 16 msb. The lsb are
         * used while adding up multiplication results giving better
         * accuracy.
         */

		/* 0x10 - CLEAR_ACC */
		if (IP15_8 & CLEAR_ACC)
		{
			state->ACC = 0;
		}

		/* 0x01 - LAC (also clears lsb)*/
		if (IP15_8 & LAC)
			state->ACC = (RAMWORD << 16);

		/* 0x02 - READ_ACC */
		if (IP15_8 & READ_ACC)
		{
			state->mathram[MA_byte+1] = ((state->ACC >> 16) & 0xff);
			state->mathram[MA_byte  ] = ((state->ACC >> 24) & 0xff);
		}

		/* 0x04 - M_HALT */
		if (IP15_8 & M_HALT)
			M_STOP = 0;

		/* 0x08 - INC_BIC */
		if (IP15_8 & INC_BIC)
			state->BIC = (state->BIC + 1) & 0x1ff; /* Restrict to 9 bits */

		/* 0x20 - LDC*/
		if (IP15_8 & LDC)
		{
			state->C = RAMWORD;

			/* This is a serial subtractor - multiplier (74ls384) -
             * accumulator. For the full calculation 33 GMCLK pulses
             * are generated. The calculation performed is:
             *
             * ACC = ACC + (A - B) * C
             *
             * 1. pulse: Bit 0 of A and B are subtracted. Bit 0 of the
             * multiplication between multiplicand C and 0 is
             * calculated (bit 0 of A-B is not yet at the multiplier
             * input). Bit 0 of ACC is added to 0 (again, 'real' results
             * from the previous operations are no yet there).
             *
             * 2. pulse: Bit 1 of A-B is calculated. Bit 1 of
             * mutliplication is calculated based on bit 0 of A-B and
             * bit 1 of C. Bit 1 of ACC is added to the multiplication
             * result from first pulse.
             *
             * 3. pulse: Bit 2 of A-B is calculated. Bit 2 of
             * mutliplication is calculated based on bit 1 of A-B and
             * bit 2 of C. Bit 2 of ACC is added to the multiplication
             * between bit 1 of C and bit 0 of A-B.
             *
             * etc.
             *
             * This pipeline causes the shifts between A-B, C and ACC.
             * The 32 bit ACC and one bit adder form a ring so it
             * takes 33 clock pulses to do a full rotation.
             */

			state->ACC += (((INT32)(state->A - state->B) << 1) * state->C) << 1;

			/* A and B are sign extended (requred by the ls384). After
             * multiplication they just contain the sign.
             */
			state->A = (state->A & 0x8000)? 0xffff: 0;
			state->B = (state->B & 0x8000)? 0xffff: 0;

			/* The multiply-add holds the main matrix processor counter
             * for 33 cycles
             */
			mptime += 33;
		}

		/* 0x40 - LDB */
		if (IP15_8 & LDB)
			state->B = RAMWORD;

		/* 0x80 - LDA */
		if (IP15_8 & LDA)
			state->A = RAMWORD;

		/*
         * Now update the PROM address counter
         * Done like this because the top two bits are not part of the counter
         * This means that each of the four pages should wrap around rather than
         * leaking from one to another.  It may not matter, but I've put it in anyway
         */
		tmp = state->MPA + 1;
		state->MPA = (state->MPA & 0x0300) | (tmp & 0x00ff); /* New MPA value */

		M_STOP--; /* Decrease count */
	}

	state->math_timer->adjust(attotime::from_hz(MASTER_CLOCK) * mptime, 1);
}



/*************************************
 *
 *  Pseudo-RNG read
 *
 *************************************/

READ8_HANDLER( starwars_prng_r )
{
	/*
     * The PRNG is a modified 23 bit LFSR. Taps are at 4 and 22 so the
     * resulting LFSR polynomial is,
     *
     * x^5 + x^{23} + 1
     *
     * which is prime. It has a loop length of 8388607. The feedback
     * bit is inverted so the PRNG can start with 0. Only 8 bits from
     * bit 8 to 15 can be read by the CPU. The PRNG runs constantly at
     * a clock speed of 3 MHz.
     */

	/* Use MAME's PRNG for now */
	return space->machine().rand();
}



/*************************************
 *
 *  Starwars divider
 *
 *************************************/

READ8_HANDLER( starwars_div_reh_r )
{
	starwars_state *state = space->machine().driver_data<starwars_state>();
	return (state->quotient_shift & 0xff00) >> 8;
}


READ8_HANDLER( starwars_div_rel_r )
{
	starwars_state *state = space->machine().driver_data<starwars_state>();
	return state->quotient_shift & 0x00ff;
}


WRITE8_HANDLER( starwars_math_w )
{
	starwars_state *state = space->machine().driver_data<starwars_state>();
	int i;

	data &= 0xff;	/* ASG 971002 -- make sure we only get bytes here */
	switch (offset)
	{
		case 0:	/* mw0 */
			state->MPA = data << 2;	/* Set starting PROM address */
			run_mproc(space->machine());			/* and run the Matrix Processor */
			break;

		case 1:	/* mw1 */
			state->BIC = (state->BIC & 0x00ff) | ((data & 0x01) << 8);
			break;

		case 2:	/* mw2 */
			state->BIC = (state->BIC & 0x0100) | data;
			break;

		case 4: /* dvsrh */
			state->divisor = (state->divisor & 0x00ff) | (data << 8);
			state->dvd_shift = state->dividend;
			state->quotient_shift = 0;
			break;

		case 5: /* dvsrl */
			/* Note: Divide is triggered by write to low byte.  This is */
			/*       dependant on the proper 16 bit write order in the  */
			/*       6809 emulation (high bytes, then low byte).        */
			/*       If the Tie fighters look corrupt, he byte order of */
			/*       the 16 bit writes in the 6809 are backwards        */

			state->divisor = (state->divisor & 0xff00) | data;

			/*
             * Simple restoring division as shown in the
             * schematics. The algorithm produces the same "wrong"
             * results as the hardware if state->divisor < 2*state->dividend or
             * state->divisor > 0x8000.
             */
			for (i = 1; i < 16; i++)
			{
				state->quotient_shift <<= 1;
				if (((INT32)state->dvd_shift + (state->divisor ^ 0xffff) + 1) & 0x10000)
				{
					state->quotient_shift |= 1;
					state->dvd_shift = (state->dvd_shift + (state->divisor ^ 0xffff) + 1) << 1;
				}
				else
				{
					state->dvd_shift <<= 1;
				}
			}
			break;

		case 6: /* dvddh */
			state->dividend = (state->dividend & 0x00ff) | (data << 8);
			break;

		case 7: /* dvddl */
			state->dividend = (state->dividend & 0xff00) | (data);
			break;

		default:
			break;
	}
}
