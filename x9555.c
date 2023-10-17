/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-01     WennianYan   the first version.
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <string.h>
#include <stdlib.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME "x9555"
#define DBG_LEVEL DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "x9555.h"

#define PKG_USING_X9555
#define USING_X9555_Example

#ifdef PKG_USING_X9555
#define X9555_ADDR (0x40 >> 1) // A0 A1 A2 connect GND

/****************************************************************************************/

static rt_err_t x9555_read_one_byte(x9555_device_t device, rt_uint8_t register_address,
                                    rt_uint8_t *read_register_value)
{
    if (rt_i2c_master_send(device->i2c, device->device_address, RT_NULL, &register_address, 1) == 1)
    {
        if (rt_i2c_master_recv(device->i2c, device->device_address, RT_NULL, read_register_value, 1) == 1)
        {
            return RT_EOK;
        }
    }
    return -RT_ERROR;
}

static rt_err_t x9555_write_one_byte(x9555_device_t device, rt_uint8_t register_address,
                                     rt_uint8_t send_register_value)
{
    rt_uint8_t buf[2];

    buf[0] = register_address;
    buf[1] = send_register_value;

    if (rt_i2c_master_send(device->i2c, device->device_address, RT_NULL, buf, 2) == 2)
    {
        return RT_EOK;
    }
    return RT_ERROR;
}

rt_err_t x9555_port_config(x9555_device_t device, rt_uint8_t port, rt_uint8_t config_register,
                           rt_uint8_t register_value)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    if (result == RT_EOK)
    {
        if ((port != X9555_PORT_0) && (port != X9555_PORT_1))
        {
            LOG_E("The x9555 port don't found. Please try again.");
            rt_mutex_release(device->lock);
            result = -RT_ERROR;
            return result;
        }

        if ((config_register != X9555_Register_Configuration_Port_0) && (config_register != X9555_Register_Configuration_Port_1) && (config_register != X9555_Register_Polarity_Inversion_Port_0) && (config_register != X9555_Register_Polarity_Inversion_Port_1))
        {
            LOG_E("The x9555 config register don't found. Please try again.");
            rt_mutex_release(device->lock);
            result = -RT_ERROR;
            return result;
        }

        if (port == X9555_PORT_0)
        {
            if (config_register == X9555_Register_Configuration_Port_0)
            {
                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_0, register_value);
            }
            else if (config_register == X9555_Register_Polarity_Inversion_Port_0)
            {
                result = x9555_write_one_byte(device, X9555_Register_Polarity_Inversion_Port_0, register_value);
            }
        }
        else if (port == X9555_PORT_1)
        {
            if (config_register == X9555_Register_Configuration_Port_1)
            {
                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_1, register_value);
            }
            else if (config_register == X9555_Register_Polarity_Inversion_Port_1)
            {
                result = x9555_write_one_byte(device, X9555_Register_Polarity_Inversion_Port_1, register_value);
            }
        }
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = RT_ERROR;
    }

    rt_mutex_release(device->lock);
    return result;
}

rt_err_t x9555_port_mode(x9555_device_t device, rt_uint8_t port, rt_uint8_t port_mode)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    if (result == RT_EOK)
    {
        if ((port != X9555_PORT_0) && (port != X9555_PORT_1))
        {
            LOG_E("The x9555 port don't found. Please try again.");

            rt_mutex_release(device->lock);

            result = -RT_ERROR;
            return result;
        }

        if ((port_mode != X9555_INPUT) && (port_mode != X9555_OUTPUT) && (port_mode != X9555_POLARITY_INVERSION))
        {
            LOG_E("The x9555 port mode don't found. Please try again.");

            rt_mutex_release(device->lock);

            result = -RT_ERROR;
            return result;
        }

        if (port_mode == X9555_INPUT)
        {
            rt_uint8_t send_register_value = 0xff;

            if (port == X9555_PORT_0)
            {
                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_0, send_register_value);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_1, send_register_value);
            }
        }
        else if (port_mode == X9555_OUTPUT)
        {
            rt_uint8_t send_register_value = 0x00;

            if (port == X9555_PORT_0)
            {
                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_0, send_register_value);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_1, send_register_value);
            }
        }
        else if (port_mode == X9555_POLARITY_INVERSION)
        {
            rt_uint8_t send_register_value = 0xff;

            if (port == X9555_PORT_0)
            {
                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_0, send_register_value);
                result = x9555_write_one_byte(device, X9555_Register_Polarity_Inversion_Port_0, send_register_value);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_1, send_register_value);
                result = x9555_write_one_byte(device, X9555_Register_Polarity_Inversion_Port_1, send_register_value);
            }
        }
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = -RT_ERROR;
    }

    rt_mutex_release(device->lock);
    return result;
}

rt_err_t x9555_port_write(x9555_device_t device, rt_uint8_t port, rt_uint8_t port_value)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    if (result == RT_EOK)
    {
        if ((port != X9555_PORT_0) && (port != X9555_PORT_1))
        {
            LOG_E("The x9555 port don't found. Please try again.");

            rt_mutex_release(device->lock);

            result = -RT_ERROR;
            return result;
        }

        if (port == X9555_PORT_0)
        {
            result = x9555_write_one_byte(device, X9555_Register_Output_Port_0, port_value);
        }
        else if (port == X9555_PORT_1)
        {
            result = x9555_write_one_byte(device, X9555_Register_Output_Port_1, port_value);
        }
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = -RT_ERROR;
    }

    rt_mutex_release(device->lock);
    return result;
}

rt_uint8_t x9555_port_read(x9555_device_t device, rt_uint8_t port, rt_uint8_t port_mode)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    rt_uint8_t read_value_buff[2] = {'\0'};

    if (result == RT_EOK)
    {
        if ((port != X9555_PORT_0) && (port != X9555_PORT_1))
        {
            LOG_E("The x9555 port don't found. Please try again.");

            rt_mutex_release(device->lock);

            result = -RT_ERROR;
            return result;
        }

        if ((port_mode != X9555_INPUT) && (port_mode != X9555_OUTPUT) && (port_mode != X9555_POLARITY_INVERSION))
        {
            LOG_E("The x9555 port mode don't found. Please try again.");

            rt_mutex_release(device->lock);

            result = -RT_ERROR;
            return result;
        }

        if (port_mode == X9555_INPUT)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Input_Port_0, read_value_buff);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Input_Port_1, read_value_buff);
            }
        }
        else if (port_mode == X9555_OUTPUT)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Output_Port_0, read_value_buff);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Output_Port_1, read_value_buff);
            }
        }
        else if (port_mode == X9555_POLARITY_INVERSION)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Polarity_Inversion_Port_0, read_value_buff);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Polarity_Inversion_Port_0, read_value_buff);
            }
        }
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = -RT_ERROR;
    }

    rt_mutex_release(device->lock);

    if (result != RT_EOK)
    {
        rt_memset(read_value_buff, '\0', sizeof(read_value_buff));
    }

    return *read_value_buff;
}

/****************************************************************************************/
static rt_err_t x9555_pin_port_switch(const rt_uint8_t pin)
{
    rt_uint8_t _port = X9555_PORT_NULL;

    switch (pin)
    {
    case X9555_IO_0_0:
    case X9555_IO_0_1:
    case X9555_IO_0_2:
    case X9555_IO_0_3:
    case X9555_IO_0_4:
    case X9555_IO_0_5:
    case X9555_IO_0_6:
    case X9555_IO_0_7:
    {
        _port = X9555_PORT_0;
        break;
    }
    case X9555_IO_1_0:
    case X9555_IO_1_1:
    case X9555_IO_1_2:
    case X9555_IO_1_3:
    case X9555_IO_1_4:
    case X9555_IO_1_5:
    case X9555_IO_1_6:
    case X9555_IO_1_7:
    {
        _port = X9555_PORT_1;
        break;
    }
    default:
    {
        _port = X9555_PORT_NULL;
        break;
    }
    }
    return _port;
}

rt_err_t x9555_pin_mode(x9555_device_t device, rt_uint8_t pin, rt_uint8_t pin_mode)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    if (result == RT_EOK)
    {
        rt_uint8_t port = X9555_PORT_NULL;
        rt_uint8_t read_value_buff[2] = {'\0'};
        rt_uint8_t send_register_value;

        port = x9555_pin_port_switch(pin);

        if (port == X9555_PORT_NULL)
        {
            LOG_E("The x9555 pin don't found. Please try again.");

            rt_mutex_release(device->lock);
            result = -RT_ERROR;
            return result;
        }

        if (pin_mode == X9555_INPUT)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Configuration_Port_0, read_value_buff);

                send_register_value = *read_value_buff | (1 << pin);

                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_0, send_register_value);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Configuration_Port_1, read_value_buff);

                send_register_value = *read_value_buff | (1 << (pin % 10));

                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_1, send_register_value);
            }
        }
        else if (pin_mode == X9555_OUTPUT)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Configuration_Port_0, read_value_buff);

                send_register_value = (*read_value_buff) & (~(1 << pin));

                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_0, send_register_value);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Configuration_Port_1, read_value_buff);

                send_register_value = (*read_value_buff) & (~(1 << (pin % 10)));

                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_1, send_register_value);
            }
        }
        else if (pin_mode == X9555_POLARITY_INVERSION)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Configuration_Port_0, read_value_buff);

                send_register_value = *read_value_buff | (1 << pin);

                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_0, send_register_value);

                result = x9555_read_one_byte(device, X9555_Register_Polarity_Inversion_Port_0, read_value_buff);

                send_register_value = *read_value_buff | (1 << pin);

                result = x9555_write_one_byte(device, X9555_Register_Polarity_Inversion_Port_0, send_register_value);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Configuration_Port_1, read_value_buff);

                send_register_value = *read_value_buff | (1 << (pin % 10));

                result = x9555_write_one_byte(device, X9555_Register_Configuration_Port_1, send_register_value);

                result = x9555_read_one_byte(device, X9555_Register_Polarity_Inversion_Port_1, read_value_buff);

                send_register_value = *read_value_buff | (1 << (pin % 10));

                result = x9555_write_one_byte(device, X9555_Register_Polarity_Inversion_Port_1, send_register_value);
            }
        }
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = -RT_ERROR;
    }

    rt_mutex_release(device->lock);
    return result;
}

rt_err_t x9555_pin_write(x9555_device_t device, rt_uint8_t pin, rt_uint8_t pin_state)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    if (result == RT_EOK)
    {
        rt_uint8_t port = X9555_PORT_NULL;
        rt_uint8_t read_value_buff[2] = {'\0'};
        rt_uint8_t send_pin_state;

        port = x9555_pin_port_switch(pin);

        if (port == X9555_PORT_NULL)
        {
            LOG_E("The x9555 pin don't found. Please try again.");

            rt_mutex_release(device->lock);
            result = -RT_ERROR;
            return result;
        }

        if ((pin_state != X9555_PIN_HIGH) && (pin_state != X9555_PIN_LOW))
        {
            LOG_E("The x9555 pin state don't found. Please try again.");

            rt_mutex_release(device->lock);
            result = -RT_ERROR;
            return result;
        }

        if (port == X9555_PORT_0)
        {
            result = x9555_read_one_byte(device, X9555_Register_Output_Port_0, read_value_buff);

            if (pin_state == X9555_PIN_HIGH)
            {
                send_pin_state = *read_value_buff | (1 << pin);
            }
            else if (pin_state == X9555_PIN_LOW)
            {
                send_pin_state = (*read_value_buff) & (~(1 << pin));
            }
            result = x9555_write_one_byte(device, X9555_Register_Output_Port_0, send_pin_state);
        }
        else if (port == X9555_PORT_1)
        {
            result = x9555_read_one_byte(device, X9555_Register_Output_Port_1, read_value_buff);

            if (pin_state == X9555_PIN_HIGH)
            {
                send_pin_state = *read_value_buff | (1 << (pin % 10));
            }
            else if (pin_state == X9555_PIN_LOW)
            {
                send_pin_state = (*read_value_buff) & (~(1 << (pin % 10)));
            }
            result = x9555_write_one_byte(device, X9555_Register_Output_Port_1, send_pin_state);
        }
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = -RT_ERROR;
    }

    rt_mutex_release(device->lock);
    return result;
}

rt_bool_t x9555_pin_read(x9555_device_t device, rt_uint8_t pin, rt_uint8_t pin_mode)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    rt_bool_t read_state = X9555_PIN_NULL;
    rt_uint8_t port = X9555_PORT_NULL;
    rt_uint8_t read_value_buff[2] = {'\0'};

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    if (result == RT_EOK)
    {
        port = x9555_pin_port_switch(pin);

        if (port == X9555_PORT_NULL)
        {
            LOG_E("The x9555 pin don't found. Please try again.");

            rt_mutex_release(device->lock);
            result = -RT_ERROR;
            return result;
        }

        if ((pin_mode != X9555_PIN_HIGH) && (pin_mode != X9555_PIN_LOW))
        {
            LOG_E("The x9555 pin state don't found. Please try again.");

            rt_mutex_release(device->lock);
            result = -RT_ERROR;
            return result;
        }

        if (pin_mode == X9555_INPUT)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Input_Port_0, read_value_buff);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Input_Port_1, read_value_buff);
            }
        }
        else if (pin_mode == X9555_OUTPUT)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Output_Port_0, read_value_buff);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Output_Port_1, read_value_buff);
            }
        }
        else if (pin_mode == X9555_POLARITY_INVERSION)
        {
            if (port == X9555_PORT_0)
            {
                result = x9555_read_one_byte(device, X9555_Register_Polarity_Inversion_Port_0, read_value_buff);
            }
            else if (port == X9555_PORT_1)
            {
                result = x9555_read_one_byte(device, X9555_Register_Polarity_Inversion_Port_0, read_value_buff);
            }
        }

        if (result == RT_EOK)
        {
            if (port == X9555_PORT_0)
            {
                read_state = (*read_value_buff & (1 << pin)) > 0 ? X9555_PIN_HIGH : X9555_PIN_LOW;
            }
            else if (port == X9555_PORT_1)
            {
                read_state = (*read_value_buff & (1 << (pin % 10))) > 0 ? X9555_PIN_HIGH : X9555_PIN_LOW;
            }
        }
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = -RT_ERROR;
    }

    rt_mutex_release(device->lock);
    return read_state;
}

/****************************************************************************************/

__WEAK void call_input_interrupt(void *args)
{
    rt_kprintf("Complete your own interrupt service program in the other file.\r\n");
}

char *read_port_value;
char *x9555_interrupt_clear(x9555_device_t device)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    read_port_value = (char *)rt_calloc(2, sizeof(char *));
    rt_uint8_t read_value_buff[2] = {'\0'};

    if (result == RT_EOK)
    {
        result = x9555_read_one_byte(device, X9555_Register_Input_Port_0, read_value_buff);
        read_port_value[0] = *read_value_buff;

        result = x9555_read_one_byte(device, X9555_Register_Input_Port_1, read_value_buff);
        read_port_value[1] = *read_value_buff;
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = -RT_ERROR;
    }

    if (result != RT_EOK)
    {
        rt_memset(read_port_value, '\0', sizeof(read_port_value));
    }

    rt_mutex_release(device->lock);

    return read_port_value;
}

x9555_device_t x9555_init(const char *interrupt_pin_name, const char *i2c_bus_name, uint8_t device_user_input_address)
{
    x9555_device_t device;
    rt_err_t result = RT_EOK;

    RT_ASSERT(i2c_bus_name);

    device = rt_calloc(1, sizeof(struct x9555_device));
    if (device == RT_NULL)
    {
        LOG_E("Can't allocate memory for x9555 device on '%s' .", i2c_bus_name);
        return RT_NULL;
    }

    device->i2c = rt_i2c_bus_device_find(i2c_bus_name);
    if (device->i2c == RT_NULL)
    {
        LOG_E("Can't find x9555 device on '%s' .", i2c_bus_name);
        rt_free(device);
        return RT_NULL;
    }

    device->lock = rt_mutex_create("mutex_x9555", RT_IPC_FLAG_FIFO);
    if (device->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for x9555 device on '%s' .", i2c_bus_name);
        rt_free(device);
        return RT_NULL;
    }

    device->device_address = X9555_ADDR | device_user_input_address;

    device->device_interrupt_pin = rt_pin_get(interrupt_pin_name);

    if (device->device_interrupt_pin > -1)
    {
        rt_pin_mode(device->device_interrupt_pin, PIN_MODE_INPUT_PULLUP);
        result = rt_pin_attach_irq(device->device_interrupt_pin, PIN_IRQ_MODE_LOW_LEVEL, call_input_interrupt, RT_NULL);
        result = rt_pin_irq_enable(device->device_interrupt_pin, PIN_IRQ_ENABLE);
    }
    else if (strcmp(interrupt_pin_name, "RT_NULL"))
    {
        LOG_E("get device '%s' interrupt pin fail.", interrupt_pin_name);
        result = RT_ERROR;
    }

    if (result != RT_EOK)
    {
        LOG_E("create device '%s' interrupt fail.", interrupt_pin_name);
        rt_free(device);
        return RT_NULL;
    }
    return device;
}

/**
 * This function releases memory and deletes mutex lock
 *
 * @param device the pointer of device driver structure
 */
void x9555_deinit(x9555_device_t device)
{
    RT_ASSERT(device);

    rt_mutex_delete(device->lock);

    rt_free(device);
}

/****************************************************************************************/
#ifdef USING_X9555_Example

enum N_CARRY
{
    Binary = 0x00,
    Decimal = 0x01,
    Hexadecimal = 0x03
} n_carry;

char **parameters;
char **string_to_data(int argc, char *argv[])
{
    char *endptr;

    // 初始化parameters行空间。
    parameters = (char **)rt_calloc(argc, sizeof(char *));
    // 初始化parameters列空间。
    for (int var = 0; var < argc; ++var)
    {
        parameters[var] = (char *)rt_calloc(2, sizeof(char));
    }

    for (int var = 0; var <= argc; ++var)
    {
        // 判断输入的字符是否为十进制数字.
        if (strspn(argv[var], "0123456789") == strlen(argv[var]))
        {
            // 把参数 argv[var] 所指向的字符串根据指定的10进制，转换为一个长整数。
            parameters[var][0] = (rt_uint8_t)strtol(argv[var], &endptr, 10);
            parameters[var][1] = Decimal;
        }
        else
        {
            if (memcmp(argv[var], "0x", 2) == 0)
            {
                // 获取0x后的16进制数据字符串。
                strcpy(parameters[var], argv[var] + 2);
                // 将16进制字符串转为16进制数据。
                parameters[var][0] = (char)strtol(argv[var], &endptr, 16);
                parameters[var][1] = Hexadecimal;
            }
            else if (memcmp(argv[var], "0b", 2) == 0)
            {
                // 获取0b后的2进制数据字符串。
                strcpy(parameters[var], argv[var] + 2);
                // 将2进制字符串转为16进制数据。
                parameters[var][0] = (char)strtol(parameters[var], &endptr, 2);
                parameters[var][1] = Binary;
            }
            else
            {
                parameters[var][0] = '\0';
                parameters[var][1] = '\0';
            }
        }
    }
    return parameters;
}

void x9555(int argc, char *argv[])
{
    static x9555_device_t device = RT_NULL;
    extern char *itoa(int value, char *string, int radix);
    char value_to_binary_string[8] = {'\0'};

    if (argc > 1)
    {
        if (!strcmp(argv[1], "create"))
        {
            if (argc > 2)
            {
                char **parameters = string_to_data(argc, argv);

                /* initialize the sensor when first probe */
                if (!device || strcmp(device->i2c->parent.parent.name, argv[3]) || (device->device_interrupt_pin != rt_pin_get(argv[2])) || (device->device_address != (X9555_ADDR | *parameters[4])))
                {
                    rt_kprintf("create ...\n");
                    /* deinit the old device */
                    if (device)
                    {
                        x9555_deinit(device);
                    }
                    device = x9555_init(argv[2], argv[3], *parameters[4]);
                }

                if (device)
                {
                    itoa(*parameters[4], value_to_binary_string, 2);
                    rt_kprintf("x9555 device create done.\r\n"
                               "device interrupt set pin name : %s, get pin number : [%d].\r\n"
                               "device bus is : %s.\r\n"
                               "devices address is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n",
                               argv[2],
                               device->device_interrupt_pin, argv[3], *parameters[4], *parameters[4], value_to_binary_string);

                    rt_free(parameters);
                }
            }
            else
            {
                rt_kprintf("cmd is : x9555 create <device interrupt pin> <device_name> <device address>\r\n"
                           "--device_name is : x9555 interrupt pin name.\r\n"
                           "--device_name is : I2C bus name.\r\n"
                           "--device address is : 0x0[A2 A1 A0]\r\n\r\n");
            }
        }
        else if (!strcmp(argv[1], "interrupt_clear"))
        {
            if (device)
            {
                char read_buffer[2] = {'\0'};

                /* read the x9555 port data */
                strcpy(read_buffer, x9555_interrupt_clear(device));

                rt_kprintf("x9555 interrupt clear done:\r\n");

                itoa(read_buffer[0], value_to_binary_string, 2);
                rt_kprintf("read port0 value :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n", read_buffer[0],
                           read_buffer[0], value_to_binary_string);

                itoa(read_buffer[1], value_to_binary_string, 2);
                rt_kprintf("read port1 value :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n", read_buffer[1],
                           read_buffer[1], value_to_binary_string);

                rt_free(read_port_value);
                rt_free(parameters);
            }
            else
            {
                rt_kprintf("Please using command : x9555 create <device name> <device address>' first.\r\n\r\n");
            }
        }
        else if (!strcmp(argv[1], "port_config"))
        {
            if (argc >= 4)
            {
                char **parameters = string_to_data(argc, argv);

                x9555_port_config(device, *parameters[2], *parameters[3], *parameters[4]);

                rt_kprintf("x9555 port config done:\r\n");
                itoa(*parameters[2], value_to_binary_string, 2);
                rt_kprintf("port number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[2],
                           *parameters[2], value_to_binary_string);
                itoa(*parameters[3], value_to_binary_string, 2);
                rt_kprintf("config register address is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n",
                           *parameters[3], *parameters[3], value_to_binary_string);
                itoa(*parameters[4], value_to_binary_string, 2);
                rt_kprintf("cofig register value is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n",
                           *parameters[4], *parameters[4], value_to_binary_string);

                rt_free(parameters);
            }
        }
        else if (!strcmp(argv[1], "port_mode"))
        {
            if (argc >= 3)
            {
                char **parameters = string_to_data(argc, argv);

                x9555_port_mode(device, *parameters[2], *parameters[3]);

                rt_kprintf("set x9555 port mode done:\r\n");
                itoa(*parameters[2], value_to_binary_string, 2);
                rt_kprintf("port number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[2],
                           *parameters[2], value_to_binary_string);
                itoa(*parameters[3], value_to_binary_string, 2);
                rt_kprintf("port mode is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n", *parameters[3],
                           *parameters[3], value_to_binary_string);

                rt_free(parameters);
            }
        }
        else if (!strcmp(argv[1], "port_write"))
        {
            if (argc >= 3)
            {
                char **parameters = string_to_data(argc, argv);

                x9555_port_write(device, *parameters[2], *parameters[3]);

                rt_kprintf("x9555 port write done:\r\n");
                itoa(*parameters[2], value_to_binary_string, 2);
                rt_kprintf("port number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[2],
                           *parameters[2], value_to_binary_string);
                itoa(*parameters[3], value_to_binary_string, 2);
                rt_kprintf("port value is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n", *parameters[3],
                           *parameters[3], value_to_binary_string);

                rt_free(parameters);
            }
        }
        else if (!strcmp(argv[1], "port_read"))
        {
            if (device)
            {
                rt_uint8_t read_buffer[2] = {'\0'};

                if (argc >= 3)
                {
                    char **parameters = string_to_data(argc, argv);

                    /* read the x9555 port data */
                    read_buffer[0] = x9555_port_read(device, *parameters[2], *parameters[3]);

                    rt_kprintf("x9555 port read done:\r\n");
                    itoa(*parameters[2], value_to_binary_string, 2);
                    rt_kprintf("port number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[2],
                               *parameters[2], value_to_binary_string);
                    itoa(*parameters[3], value_to_binary_string, 2);
                    rt_kprintf("port mode is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[3],
                               *parameters[3], value_to_binary_string);
                    itoa(*read_buffer, value_to_binary_string, 2);
                    rt_kprintf("read port value :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n", *read_buffer,
                               *read_buffer, value_to_binary_string);

                    rt_free(parameters);
                }
            }
            else
            {
                rt_kprintf("Please using command : x9555 create <device name> <device address>' first.\r\n\r\n");
            }
        }
        else if (!strcmp(argv[1], "pin_mode"))
        {
            if (argc >= 3)
            {
                char **parameters = string_to_data(argc, argv);

                x9555_pin_mode(device, *parameters[2], *parameters[3]);

                rt_kprintf("set x9555 pin mode done:\r\n");
                itoa(*parameters[2], value_to_binary_string, 2);
                rt_kprintf("pin number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[2],
                           *parameters[2], value_to_binary_string);
                itoa(*parameters[3], value_to_binary_string, 2);
                rt_kprintf("pin mode is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n", *parameters[3],
                           *parameters[3], value_to_binary_string);

                rt_free(parameters);
            }
        }
        else if (!strcmp(argv[1], "pin_write"))
        {
            if (argc >= 3)
            {
                char **parameters = string_to_data(argc, argv);

                x9555_pin_write(device, *parameters[2], *parameters[3]);

                rt_kprintf("x9555 pin write done:\r\n");
                itoa(*parameters[2], value_to_binary_string, 2);
                rt_kprintf("pin number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[2],
                           *parameters[2], value_to_binary_string);
                itoa(*parameters[3], value_to_binary_string, 2);
                rt_kprintf("pin state is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n", *parameters[3],
                           *parameters[3], value_to_binary_string);

                rt_free(parameters);
            }
        }
        else if (!strcmp(argv[1], "pin_read"))
        {
            if (device)
            {
                rt_uint8_t read_buffer[2] = {'\0'};

                if (argc >= 3)
                {
                    char **parameters = string_to_data(argc, argv);

                    /* read the x9555 pin data */
                    read_buffer[0] = x9555_pin_read(device, *parameters[2], *parameters[3]);

                    rt_kprintf("x9555 pin read done:\r\n");
                    itoa(*parameters[2], value_to_binary_string, 2);
                    rt_kprintf("pin number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[2],
                               *parameters[2], value_to_binary_string);
                    itoa(*parameters[3], value_to_binary_string, 2);
                    rt_kprintf("pin mode is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n", *parameters[3],
                               *parameters[3], value_to_binary_string);
                    itoa(*read_buffer, value_to_binary_string, 2);
                    rt_kprintf("read pin value :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\r\n\r\n", *read_buffer,
                               *read_buffer, value_to_binary_string);

                    rt_free(parameters);
                }
            }
            else
            {
                rt_kprintf("Please using command : x9555 create <device name> <device address>' first.\r\n\r\n");
            }
        }
        else
        {
            rt_kprintf("command don't found. Please enter 'x9555' for help.\n\r\n");
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf(
            "x9555 create <device interrupt pin> <device name> <device address> \t - create x9555 by given name.\n"
            "Example 1 :x9555 create PA.00 i2c2 0x02\n"
            "Example 2 :x9555 create RT_NULL i2c2 0x02\n\n");

        rt_kprintf("x9555 interrupt_clear \t\t\t\t\t\t - x9555 interrupt clear.\n");
        rt_kprintf("x9555 port_config <port> <config register> <register value> \t - config x9555 register.\n");
        rt_kprintf("x9555 port_mode <port> <port mode> \t\t\t\t - set x9555 port mode.\n");
        rt_kprintf("x9555 port_write <port> <port value> \t\t\t\t - set x9555 port output.\n");
        rt_kprintf("x9555 port_read <port> <port mode> \t\t\t\t - get x9555 port input.\n");
        rt_kprintf("x9555 pin_mode <pin> <pin mode> \t\t\t\t - set x9555 io mode.\n");
        rt_kprintf("x9555 pin_write <pin> <pin state> \t\t\t\t - set x9555 io output.\n");
        rt_kprintf("x9555 pin_read <pin> <pin mode> \t\t\t\t - get x9555 io input.\n\n");

        rt_kprintf("X9555 Register:\n"
                   "X9555_Register_Input_Port_0 \t\t\t 0x00\n"
                   "X9555_Register_Input_Port_1 \t\t\t 0x01\n"
                   "X9555_Register_Output_Port_0 \t\t\t 0x02\n"
                   "X9555_Register_Output_Port_1 \t\t\t 0x03\n");

        rt_kprintf("X9555_Register_Polarity_Inversion_Port_0 \t 0x04\n"
                   "X9555_Register_Polarity_Inversion_Port_1 \t 0x05\n"
                   "X9555_Register_Configuration_Port_0 \t\t 0x06\n"
                   "X9555_Register_Configuration_Port_1 \t\t 0x07\n\n");

        rt_kprintf("X9555 Pin State:\n"
                   "X9555_PIN_LOW \t 0x00\n"
                   "X9555_PIN_HIGH \t 0x01\n\n");

        rt_kprintf("X9555 Port Number:\n"
                   "X9555_PORT_0 \t\t 0x00\n"
                   "X9555_PORT_1 \t\t 0x01\n\n");

        rt_kprintf("X9555 (pin or port) Mode:\n"
                   "X9555_OUTPUT \t\t\t 0x00\n"
                   "X9555_INPUT \t\t\t 0x01\n"
                   "X9555_POLARITY_INVERSION \t 0x03\n\n");

        rt_kprintf("X9555 IO PORT 0:\n"
                   "X9555_IO_0_0 \t\t 0\n"
                   "X9555_IO_0_1 \t\t 1\n"
                   "X9555_IO_0_2 \t\t 2\n"
                   "X9555_IO_0_3 \t\t 3\n"
                   "X9555_IO_0_4 \t\t 4\n"
                   "X9555_IO_0_5 \t\t 5\n"
                   "X9555_IO_0_6 \t\t 6\n"
                   "X9555_IO_0_7 \t\t 7\n\n");

        rt_kprintf("X9555 IO PORT 1:\n"
                   "X9555_IO_1_0 \t\t 10\n"
                   "X9555_IO_1_1 \t\t 11\n"
                   "X9555_IO_1_2 \t\t 12\n"
                   "X9555_IO_1_3 \t\t 13\n"
                   "X9555_IO_1_4 \t\t 14\n"
                   "X9555_IO_1_5 \t\t 15\n"
                   "X9555_IO_1_6 \t\t 16\n"
                   "X9555_IO_1_7 \t\t 17\n\n");
    }
}

MSH_CMD_EXPORT(x9555, x9555 test.);
#endif // USING_X9555_Example
/****************************************************************************************/
#endif // PKG_USING_X9555
