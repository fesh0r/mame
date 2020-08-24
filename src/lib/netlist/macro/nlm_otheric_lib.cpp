// license:GPL-2.0+
// copyright-holders:Couriersud

#include "netlist/devices/net_lib.h"

/*
 *   MC14584B: Hex Schmitt Trigger
 *             ON Semiconductor
 *
 *          +--------------+
 *       A1 |1     ++    14| VCC
 *       Y1 |2           13| A6
 *       A2 |3           12| Y6
 *       Y2 |4  MC14584B 11| A5
 *       A3 |5           10| Y5
 *       Y3 |6            9| A4
 *      GND |7            8| Y4
 *          +--------------+
 *
 */

static NETLIST_START(MC14584B_DIP)
	MC14584B_GATE(A)
	MC14584B_GATE(B)
	MC14584B_GATE(C)
	MC14584B_GATE(D)
	MC14584B_GATE(E)
	MC14584B_GATE(F)

	NET_C(A.VDD, B.VDD, C.VDD, D.VDD, E.VDD, F.VDD)
	NET_C(A.VSS, B.VSS, C.VSS, D.VSS, E.VSS, F.VSS)
	DIPPINS(  /*       +--------------+      */
		A.A,  /*    A1 |1     ++    14| VDD  */ A.VDD,
		A.Q,  /*    Y1 |2           13| A6   */ F.A,
		B.A,  /*    A2 |3           12| Y6   */ F.Q,
		B.Q,  /*    Y2 |4  MC14584B 11| A5   */ E.A,
		C.A,  /*    A3 |5           10| Y5   */ E.Q,
		C.Q,  /*    Y3 |6            9| A4   */ D.A,
		A.VSS,/*   VSS |7            8| Y4   */ D.Q
			  /*       +--------------+      */
	)
NETLIST_END()

//- Identifier:  NE566_DIP
//- Title: NE566 Voltage Controlled Oscillator
//- Description: The LM566CN is a general purpose voltage controlled oscillator
//-    which may be used to generate square and triangula waves, the frequency
//-    of which is a very linear function of a control voltage. The frequency
//-    is also a function of an external resistor and capacitor.
//-
//-    The LM566CN is specified for operation over the 0°C to a 70°C
//-    temperature range.
//-
//-    Applications
//-
//-    - FM modulation
//-    - Signal generation
//-    - Function generation
//-    - Frequency shift keying
//-    - Tone generation
//-
//-    Features
//-    - Wide supply voltage range: 10V to 24V
//-    - Very linear modulation characteristics
//-    - High temperature stability
//-    - Excellent supply voltage rejection
//-    - 10 to 1 frequency range with fixed capacitor
//-    - Frequency programmable by means of current, voltage, resistor or capacitor
//-
//.
//- Pinalias: GND,NC,SQUARE,TRIANGLE,MODULATION,R1,C1,VCC
//- Package: DIP
//- NamingConvention: Naming conventions follow National Semiconductor datasheet
//- Limitations:
//-    This implementation is focused on performance. There may be edge cases
//-    which lead to issues and ringing.
//.
//- Example: ne566.cpp,ne566_example
//- FunctionTable:
//-    https://www.egr.msu.edu/eceshop/Parts_Inventory/datasheets/lm566.pdf
//-
//.

static NETLIST_START(NE566_DIP)

	VCVS(VI, 1)
	CCCS(CI1, -1)
	CCCS(CI2, 1)
	SYS_COMPD(COMP)
	SYS_DSW2(SW)
	VCVS(VO, 1)
	DIODE(DC, "D")
	DIODE(DM, "D")
	RES(ROD, 5200)
	RES(ROU, 200)

	PARAM(VO.RO, 50)
	PARAM(COMP.MODEL, "FAMILY(TYPE=CUSTOM IVL=0.16 IVH=0.4 OVL=0.1 OVH=0.1 ORL=50 ORH=50)")
	PARAM(SW.GOFF, 0) // This has to be zero to block current sources

	NET_C(CI2.IN, VI.OP)
	NET_C(CI2.IP, CI1.IN)
	NET_C(COMP.Q, SW.I)
	NET_C(SW.1, CI1.OP)
	NET_C(SW.3, CI2.OP)
	NET_C(SW.2, VO.IP)
	NET_C(VO.OP, COMP.IN)

	// Avoid singular Matrix due to G=0 switch
	RES(RX1, 1e10)
	RES(RX2, 1e10)
	NET_C(RX1.1, SW.1)
	NET_C(RX2.1, SW.3)

	NET_C(COMP.GND, RX1.2, RX2.2)

	// Block if VC < V+ - ~4
	VS(VM, 3)
	PARAM(VM.RI, 10)
	NET_C(VM.1, COMP.VCC)
	NET_C(VM.2, DM.A)
	NET_C(DM.K, VI.OP)

	// Block if VC > V+
	NET_C(COMP.GND, DC.A)
	NET_C(SW.2, DC.K)

	RES(R1, 5000)
	RES(R2, 1800)
	RES(R3, 6000)

	// Square output wave
	AFUNC(FO, 2, "min(A1-1,A0 + 5)")
	NET_C(COMP.QQ, FO.A0)
	NET_C(FO.Q, ROU.1)
	NET_C(ROU.2, ROD.1)

	NET_C(COMP.GND, SW.GND, VI.ON, VI.IN, CI1.ON, CI2.ON, VO.IN, VO.ON, R2.2, ROD.2)
	NET_C(COMP.VCC, SW.VCC, R1.2)
	NET_C(COMP.IP, R1.1, R2.1, R3.1)
	NET_C(COMP.Q, R3.2)

	ALIAS(1, VI.ON) // GND
	ALIAS(3, ROD.1) // Square out
	ALIAS(4, VO.OP) // Diag out
	ALIAS(5, VI.IP) // VC
	ALIAS(6, CI1.IP) // R1
	ALIAS(7, SW.2) // C1
	ALIAS(8, COMP.VCC) // V+

	NET_C(COMP.VCC, FO.A1)


NETLIST_END()

//- Identifier:  NE555_DIP
//- Title: NE555 PRECISION TIMERS
//- Description:
//-   These devices are precision timing circuits capable of producing accurate
//-   time delays or oscillation. In the time-delay or monostable mode of
//-   operation, the timed interval is controlled by a single external resistor
//-   and capacitor network. In the astable mode of operation, the frequency and
//-   duty cycle can be controlled independently with two external resistors and
//-   a single external capacitor.
//-
//-   The threshold and trigger levels normally are two-thirds and one-third,
//-   respectively, of V CC. These levels can be altered by use of the
//-   control-voltage terminal. When the trigger input falls below the trigger
//-   level, the flip-flop is set and NC – No internal connection the output
//-   goes high. If the trigger input is above the trigger level and the
//-   threshold input is above the threshold level, the flip-flop is reset and
//-   the output is low. The reset (RESET) input can override all other
//-   inputs and can be used to initiate a new timing cycle. When RESET goes
//-   low, the flip-flop is reset and the output goes low. When the output is low,
//-   a low-impedance path is provided between discharge (DISCH) and ground.
//-
//-   The output circuit is capable of sinking or sourcing current up to 200 mA.
//-   Operation is specified for supplies of 5 V to 15 V. With a 5-V supply,
//-   output levels are compatible with TTL inputs.
//-
//-   The NE555 is characterized for operation from 0°C to 70°C. The SA555 is
//-   characterized for operation from –40°C to 85°C. The SE555 is characterized
//-   for operation over the full military range of –55°C to 125°C.
//-
//- Pinalias: GND,TRIG,OUT,RESET,CONT,THRES,DISCH,VCC
//- Package: DIP
//- NamingConvention: Naming conventions follow Texas instrument datasheet
//- Limitations: Internal resistor network currently fixed to 5k
//-   If TRIG and TRESH are connected overshoot compensation will be enabled.
//-   The approach is raw but delivers results (at 5 to 10 steps per discharge/charge)
//-   within a couple of percent. Please take into account that any datasheet
//-   formulas are idealistic. Neither capacitor, resistor, internal resistor
//-   tolerances are taken into account. Nor are ambient temperature and chip
//-   temperature. Thus the result is considered acceptable.
//-   The datasheet states a maximum discharge of 200mA, this is not modelled
//-   Instead an impedance of 1 Ohm is used.
//-
//- Example: ne555_astable.c,ne555_example
//- FunctionTable:
//-
//-    |RESET|TRIGGER VOLTAGE|THRESHOLD VOLTAGE|OUTPUT|DISCHARGE SWITCH|
//-    |:---:|:-------------:|:---------------:|:----:|:--------------:|
//-    |Low  | Irrelevant    | Irrelevant      |  Low |    On          |
//-    |High | <1/3 VDD      | Irrelevant      | High |    Off         |
//-    |High | >1/3 VDD      | >2/3 VDD        | Low  |    On          |
//-    |High | >1/3 VDD      | <2/3 VDD        | As previously established||
//-
static NETLIST_START(NE555_DIP)

	NE555(A)

	ALIAS(1, A.GND)      // Pin 1
	ALIAS(2, A.TRIG)     // Pin 2
	ALIAS(3, A.OUT)      // Pin 3
	ALIAS(4, A.RESET)    // Pin 4
	ALIAS(5, A.CONT)     // Pin 5
	ALIAS(6, A.THRESH)   // Pin 6
	ALIAS(7, A.DISCH)    // Pin 7
	ALIAS(8, A.VCC)      // Pin 8

NETLIST_END()

static NETLIST_START(MC1455P_DIP)

	MC1455P(A)

	ALIAS(1, A.GND)      // Pin 1
	ALIAS(2, A.TRIG)     // Pin 2
	ALIAS(3, A.OUT)      // Pin 3
	ALIAS(4, A.RESET)    // Pin 4
	ALIAS(5, A.CONT)     // Pin 5
	ALIAS(6, A.THRESH)   // Pin 6
	ALIAS(7, A.DISCH)    // Pin 7
	ALIAS(8, A.VCC)      // Pin 8

NETLIST_END()

NETLIST_START(otheric_lib)
	TRUTHTABLE_START(MC14584B_GATE, 1, 1, "")
		TT_HEAD(" A | Q ")
		TT_LINE(" 0 | 1 |100")
		TT_LINE(" 1 | 0 |100")
		// 2.1V negative going and 2.7V positive going at 5V
		TT_FAMILY("FAMILY(TYPE=CMOS IVL=0.42 IVH=0.54 OVL=0.05 OVH=0.05 ORL=10.0 ORH=10.0)")
	TRUTHTABLE_END()

	LOCAL_LIB_ENTRY(MC14584B_DIP)
	LOCAL_LIB_ENTRY(NE566_DIP)
	LOCAL_LIB_ENTRY(NE555_DIP)
	LOCAL_LIB_ENTRY(MC1455P_DIP)
NETLIST_END()

