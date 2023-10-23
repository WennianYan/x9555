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

#ifdef PKG_USING_X9555_EXAMPLE

enum N_CARRY
{
    Binary = 0x00,
    Decimal = 0x01,
    Hexadecimal = 0x03
} n_carry;

static void string_to_data(int argc, char *argv[], char (*data_conversion_results)[2])
{
    char *endptr;

    for (int var = 0; var < argc; ++var)
    {
        // 判断输入的字符是否为十进制数字.
        if (strspn(argv[var], "0123456789") == strlen(argv[var]))
        {
            // 把参数 argv[var] 所指向的字符串根据指定的10进制，转换为一个长整数。
            data_conversion_results[var][0] = (rt_uint8_t)strtol(argv[var], &endptr, 10);
            data_conversion_results[var][1] = Decimal;
        }
        else
        {
            if (memcmp(argv[var], "0x", 2) == 0)
            {
                // 获取0x后的16进制数据字符串。
                strcpy(data_conversion_results[var], argv[var] + 2);
                // 将16进制字符串转为16进制数据。
                data_conversion_results[var][0] = (char)strtol(argv[var], &endptr, 16);
                data_conversion_results[var][1] = Hexadecimal;
            }
            else if (memcmp(argv[var], "0b", 2) == 0)
            {
                // 获取0b后的2进制数据字符串。
                strcpy(data_conversion_results[var], argv[var] + 2);
                // 将2进制字符串转为16进制数据。
                data_conversion_results[var][0] = (char)strtol(data_conversion_results[var], &endptr, 2);
                data_conversion_results[var][1] = Binary;
            }
            else
            {
                data_conversion_results[var][0] = '\0';
                data_conversion_results[var][1] = '\0';
            }
        }
    }
}

void x9555(int argc, char *argv[])
{
    static x9555_device_t device = RT_NULL;
    extern char *itoa(int value, char *string, int radix);
    char value_to_binary_string[8] = {'\0'};
    char data_conversion_results[6][2] = {'\0'};
    string_to_data(argc, argv, data_conversion_results);

    if (argc > 1)
    {
        rt_kprintf("user input command is : x9555 %s, parameters count are : %d \n", argv[1], argc - 2);

        if (!strcmp(argv[1], "create"))
        {
            if (argc > 4)
            {
                if (!device ||
                    strcmp(device->i2c->parent.parent.name, argv[3]) ||
                    (device->device_interrupt_pin != rt_pin_get(argv[2])) ||
                    (device->device_address != (X9555_ADDR | *data_conversion_results[4])))
                {
                    rt_kprintf("create ...\n");
                    /* deinit the old device */
                    if (device)
                    {
                        x9555_deinit(device);
                    }
                    device = x9555_init(argv[2], argv[3], *data_conversion_results[4]);
                }

                if (device)
                {
                    itoa(*data_conversion_results[4], value_to_binary_string, 2);
                    rt_kprintf("x9555 device create done.\n"
                               "device interrupt set pin name : %s, get pin number : [%d].\n"
                               "device bus is : %s.\n"
                               "devices address is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                               argv[2], device->device_interrupt_pin,
                               argv[3],
                               *data_conversion_results[4], *data_conversion_results[4], value_to_binary_string);
                }
            }
            else
            {
                rt_kprintf("cmd is : x9555 create <device interrupt pin> <device_name> <device address>\n"
                           "--device_name is : x9555 interrupt pin name.\n"
                           "--device_name is : I2C bus name.\n"
                           "--device address is : 0x0[A2 A1 A0]\n\n");
            }
        }
        else
        {
            if (!device)
            {
                rt_kprintf("Please using command : x9555 create <device name> <device address>' first.\n\n");
            }
            else if ((!strcmp(argv[1], "interrupt_clear")) && (argc > 2))
            {
                char interrupt_get_value[2] = {'\0'};

                x9555_interrupt_clear(device, interrupt_get_value);

                rt_kprintf("x9555 interrupt clear done:\n");

                itoa(interrupt_get_value[0], value_to_binary_string, 2);
                rt_kprintf("read port0 value :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                           interrupt_get_value[0], interrupt_get_value[0], value_to_binary_string);

                itoa(interrupt_get_value[1], value_to_binary_string, 2);
                rt_kprintf("read port1 value :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                           interrupt_get_value[1], interrupt_get_value[1], value_to_binary_string);
            }
            else if ((!strcmp(argv[1], "port_config")) && (argc > 4))
            {
                x9555_port_config(device, *data_conversion_results[2], *data_conversion_results[3], *data_conversion_results[4]);

                rt_kprintf("x9555 port config done:\n");

                itoa(*data_conversion_results[2], value_to_binary_string, 2);
                rt_kprintf("port number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                           *data_conversion_results[2], *data_conversion_results[2], value_to_binary_string);

                itoa(*data_conversion_results[3], value_to_binary_string, 2);
                rt_kprintf("config register address is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                           *data_conversion_results[3], *data_conversion_results[3], value_to_binary_string);

                itoa(*data_conversion_results[4], value_to_binary_string, 2);
                rt_kprintf("cofig register value is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                           *data_conversion_results[4], *data_conversion_results[4], value_to_binary_string);
            }
            else if ((!strcmp(argv[1], "port_mode")) && (argc > 3))
            {
                x9555_port_mode(device, *data_conversion_results[2], *data_conversion_results[3]);

                rt_kprintf("set x9555 port mode done:\n");

                itoa(*data_conversion_results[2], value_to_binary_string, 2);
                rt_kprintf("port number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                           *data_conversion_results[2], *data_conversion_results[2], value_to_binary_string);

                itoa(*data_conversion_results[3], value_to_binary_string, 2);
                rt_kprintf("port mode is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                           *data_conversion_results[3], *data_conversion_results[3], value_to_binary_string);
            }
            else if ((!strcmp(argv[1], "port_write")) && (argc > 3))
            {
                x9555_port_write(device, *data_conversion_results[2], *data_conversion_results[3]);

                rt_kprintf("x9555 port write done:\n");

                itoa(*data_conversion_results[2], value_to_binary_string, 2);
                rt_kprintf("port number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                           *data_conversion_results[2], *data_conversion_results[2], value_to_binary_string);

                itoa(*data_conversion_results[3], value_to_binary_string, 2);
                rt_kprintf("port value is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                           *data_conversion_results[3], *data_conversion_results[3], value_to_binary_string);
            }
            else if ((!strcmp(argv[1], "port_read")) && (argc > 3))
            {
                rt_uint8_t read_buffer[2] = {'\0'};

                if (argc >= 3)
                {
                    read_buffer[0] = x9555_port_read(device, *data_conversion_results[2], *data_conversion_results[3]);

                    rt_kprintf("x9555 port read done:\n");

                    itoa(*data_conversion_results[2], value_to_binary_string, 2);
                    rt_kprintf("port number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                               *data_conversion_results[2], *data_conversion_results[2], value_to_binary_string);

                    itoa(*data_conversion_results[3], value_to_binary_string, 2);
                    rt_kprintf("port mode is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                               *data_conversion_results[3], *data_conversion_results[3], value_to_binary_string);

                    itoa(*read_buffer, value_to_binary_string, 2);
                    rt_kprintf("read port value :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                               *read_buffer, *read_buffer, value_to_binary_string);
                }
            }
            else if ((!strcmp(argv[1], "pin_mode")) && (argc > 3))
            {
                x9555_pin_mode(device, *data_conversion_results[2], *data_conversion_results[3]);

                rt_kprintf("set x9555 pin mode done:\n");

                itoa(*data_conversion_results[2], value_to_binary_string, 2);
                rt_kprintf("pin number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                           *data_conversion_results[2], *data_conversion_results[2], value_to_binary_string);

                itoa(*data_conversion_results[3], value_to_binary_string, 2);
                rt_kprintf("pin mode is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                           *data_conversion_results[3], *data_conversion_results[3], value_to_binary_string);
            }
            else if ((!strcmp(argv[1], "pin_write")) && (argc > 3))
            {
                x9555_pin_write(device, *data_conversion_results[2], *data_conversion_results[3]);

                rt_kprintf("x9555 pin write done:\n");

                itoa(*data_conversion_results[2], value_to_binary_string, 2);
                rt_kprintf("pin number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                           *data_conversion_results[2], *data_conversion_results[2], value_to_binary_string);

                itoa(*data_conversion_results[3], value_to_binary_string, 2);
                rt_kprintf("pin state is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                           *data_conversion_results[3], *data_conversion_results[3], value_to_binary_string);
            }
            else if ((!strcmp(argv[1], "pin_read")) && (argc > 3))
            {
                rt_uint8_t read_buffer[2] = {'\0'};

                if (argc >= 3)
                {
                    read_buffer[0] = x9555_pin_read(device, *data_conversion_results[2], *data_conversion_results[3]);

                    rt_kprintf("x9555 pin read done:\n");

                    itoa(*data_conversion_results[2], value_to_binary_string, 2);
                    rt_kprintf("pin number is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                               *data_conversion_results[2], *data_conversion_results[2], value_to_binary_string);

                    itoa(*data_conversion_results[3], value_to_binary_string, 2);
                    rt_kprintf("pin mode is :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n",
                               *data_conversion_results[3], *data_conversion_results[3], value_to_binary_string);

                    itoa(*read_buffer, value_to_binary_string, 2);
                    rt_kprintf("read pin value :[Decimal %d]->[Hexadecimal 0x%x]->[Binary %s].\n\n",
                               *read_buffer, *read_buffer, value_to_binary_string);
                }
            }
            else
            {
                rt_kprintf("command don't found or not enough parameters. Please enter 'x9555' for help.\n\n");
            }
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf(
            "x9555 create <device interrupt pin> <device name> <device address> \t - create x9555 by given name.\n"
            "Example 1 :x9555 create PA.00 i2c1 0x01\n"
            "Example 2 :x9555 create RT_NULL i2c1 0x01\n\n");

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
                   "X9555_POLARITY_INVERSION \t 0x02\n\n");

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
#endif // PKG_USING_X9555_EXAMPLE
