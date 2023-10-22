/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-01     WennianYan   the first version.
 */

#include "x9555.h"

#ifdef PKG_USING_X9555

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

        if ((config_register != X9555_Register_Configuration_Port_0) &&
            (config_register != X9555_Register_Configuration_Port_1) &&
            (config_register != X9555_Register_Polarity_Inversion_Port_0) &&
            (config_register != X9555_Register_Polarity_Inversion_Port_1))
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
            else
            {
                LOG_E("The x9555 config register is not consistent with the port. Please try again.");
                result = -RT_ERROR;
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
            else
            {
                LOG_E("The x9555 config register is not consistent with the port. Please try again.");
                result = -RT_ERROR;
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

RT_WEAK void call_input_interrupt(void *args)
{
    rt_kprintf("Complete your own interrupt service program in the other file.\n");
}

rt_err_t x9555_interrupt_clear(x9555_device_t device, char *interrupt_get_value)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(device);

    result = rt_mutex_take(device->lock, RT_WAITING_FOREVER);

    rt_uint8_t read_value_buff[2] = {'\0'};

    if (result == RT_EOK)
    {
        result = x9555_read_one_byte(device, X9555_Register_Input_Port_0, read_value_buff);
        interrupt_get_value[0] = *read_value_buff;

        result &= x9555_read_one_byte(device, X9555_Register_Input_Port_1, read_value_buff);
        interrupt_get_value[1] = *read_value_buff;
    }
    else
    {
        LOG_E("The x9555 could not respond  at this time. Please try again.");
        result = -RT_ERROR;
    }

    if (result != RT_EOK)
    {
        rt_memset(interrupt_get_value, '\0', sizeof(interrupt_get_value));
    }

    rt_mutex_release(device->lock);

    return result;
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
#endif // PKG_USING_X9555
