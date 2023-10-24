/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-01     WennianYan   the first version.
 * 2023-10-23     WennianYan   Update the package framework.
 */

#ifndef __X9555_H__
#define __X9555_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <stdlib.h>
#include <string.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME "x9555"
#define DBG_LEVEL DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#define X9555_ADDR (0x40 >> 1) // A0 A1 A2 connect GND

#define X9555_Register_Input_Port_0                  0x00
#define X9555_Register_Input_Port_1                  0x01
#define X9555_Register_Output_Port_0                 0x02
#define X9555_Register_Output_Port_1                 0x03
#define X9555_Register_Polarity_Inversion_Port_0     0x04
#define X9555_Register_Polarity_Inversion_Port_1     0x05
#define X9555_Register_Configuration_Port_0          0x06
#define X9555_Register_Configuration_Port_1          0x07

enum X9555_PIN_STATE
{
    X9555_PIN_LOW = 0x00,
    X9555_PIN_HIGH = 0x01,
    X9555_PIN_NULL = 0x02
};

enum X9555_PORT
{
    X9555_PORT_0 = 0x00,
    X9555_PORT_1 = 0x01,
    X9555_PORT_NULL = 0x02
};

enum X9555_IO_PORT_0
{
    X9555_IO_0_0 = 0,
    X9555_IO_0_1 = 1,
    X9555_IO_0_2 = 2,
    X9555_IO_0_3 = 3,
    X9555_IO_0_4 = 4,
    X9555_IO_0_5 = 5,
    X9555_IO_0_6 = 6,
    X9555_IO_0_7 = 7
};

enum X9555_IO_PORT_1
{
    X9555_IO_1_0 = 10,
    X9555_IO_1_1 = 11,
    X9555_IO_1_2 = 12,
    X9555_IO_1_3 = 13,
    X9555_IO_1_4 = 14,
    X9555_IO_1_5 = 15,
    X9555_IO_1_6 = 16,
    X9555_IO_1_7 = 17
};

enum X9555_MODE
{
    X9555_OUTPUT = 0x00,
    X9555_INPUT = 0x01,
    X9555_POLARITY_INVERSION = 0x02
};

struct x9555_device
{
    struct rt_i2c_bus_device *i2c;
    rt_mutex_t lock;
    uint8_t device_address;
    rt_base_t device_interrupt_pin;
};
typedef struct x9555_device *x9555_device_t;

extern x9555_device_t x9555_init(const char *interrupt_pin_name, const char *i2c_bus_name, uint8_t device_user_input_address);
extern void x9555_deinit(x9555_device_t device);
extern void call_input_interrupt(void *args);

extern rt_err_t x9555_port_config(x9555_device_t device, rt_uint8_t port, rt_uint8_t config_register, rt_uint8_t register_value);
extern rt_err_t x9555_interrupt_clear(x9555_device_t device, char *interrupt_get_value);

extern rt_err_t x9555_port_mode(x9555_device_t device, rt_uint8_t port, rt_uint8_t port_mode);
extern rt_err_t x9555_port_write(x9555_device_t device, rt_uint8_t port, rt_uint8_t port_value);
extern rt_uint8_t x9555_port_read(x9555_device_t device, rt_uint8_t port, rt_uint8_t port_mode);

extern rt_err_t x9555_pin_mode(x9555_device_t device, rt_uint8_t pin, rt_uint8_t pin_mode);
extern rt_err_t x9555_pin_write(x9555_device_t device, rt_uint8_t pin, rt_uint8_t pin_state);
extern rt_bool_t x9555_pin_read(x9555_device_t device, rt_uint8_t pin,rt_uint8_t pin_mode);

#endif
