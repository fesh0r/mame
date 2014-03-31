// license:MAME|LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    Joystick port

    Now explicitly implemented as a slot device
    A joystick port allows for plugging in a single or twin joystick, or
    a Mechatronics mouse.

    The TI-99/4 also offers an infrared handset, connected to this port. For
    this reason we also need an interrupt line.

    Michael Zapf

    June 2012

*****************************************************************************/

#ifndef __JOYPORT__
#define __JOYPORT__

#include "emu.h"

extern const device_type JOYPORT;

class joyport_device;

/********************************************************************
    Common parent class of all devices attached to the joystick port
********************************************************************/
class joyport_attached_device : public device_t
{
	friend class joyport_device;
public:
	joyport_attached_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source)
	: device_t(mconfig, type, name, tag, owner, clock, shortname, source) { }

protected:
	virtual void device_config_complete();
	joyport_device* m_joyport;

private:
	virtual UINT8 read_dev() =0;
	virtual void write_dev(UINT8 data) =0;
};

/********************************************************************
    Joystick port
********************************************************************/
class joyport_device : public device_t, public device_slot_interface
{
public:
	joyport_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	UINT8   read_port();
	void    write_port(int data);
	void    set_interrupt(int state);

	template<class _Object> static devcb2_base &static_set_int_callback(device_t &device, _Object object) { return downcast<joyport_device &>(device).m_interrupt.set_callback(object); }

protected:
	void device_start();
	void device_config_complete();

private:
	devcb2_write_line           m_interrupt;
	joyport_attached_device*    m_connected;
};

SLOT_INTERFACE_EXTERN(joystick_port);
SLOT_INTERFACE_EXTERN(joystick_port_994);
SLOT_INTERFACE_EXTERN(joystick_port_gen);

#define MCFG_JOYPORT_INT_HANDLER( _intcallb ) \
	devcb = &joyport_device::static_set_int_callback( *device, DEVCB2_##_intcallb );

#define MCFG_GENEVE_JOYPORT_ADD( _tag, _clock )  \
	MCFG_DEVICE_ADD(_tag, JOYPORT, _clock) \
	MCFG_DEVICE_SLOT_INTERFACE(joystick_port_gen, "twinjoy", false)

#define MCFG_TI_JOYPORT4A_ADD( _tag, _clock )    \
	MCFG_DEVICE_ADD(_tag, JOYPORT, _clock) \
	MCFG_DEVICE_SLOT_INTERFACE(joystick_port, "twinjoy", false)

#define MCFG_TI_JOYPORT4_ADD( _tag, _clock ) \
	MCFG_DEVICE_ADD(_tag, JOYPORT, _clock) \
	MCFG_DEVICE_SLOT_INTERFACE(joystick_port_994, "twinjoy", false)

#endif /* __JOYPORT__ */
