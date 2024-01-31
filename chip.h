/*
* Copyright 2019-2020 NXP.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of the NXP Semiconductor nor the names of its
* contributors may be used to endorse or promote products derived from this
* software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/

#ifndef CHIP_H
#define CHIP_H

#include "port.h"

#define MAX_FT_I2C_CHANNEL_NUMBER 4

struct device {
	char* name;
	int type;
	struct device* parent;
	struct device* child;
	int (*free)(void* p); // if there is any variable inside the device need to be free
};

#define I2C_TYPE_PAC1934	0
#define I2C_TYPE_GPIO		1
#define I2C_TYPE_PCA9548	2
#define I2C_TYPE_AT24		3
#define I2C_TYPE_TEMP		4

struct i2c_device {
	struct device device;
	int (*i2c_read)(void*, unsigned char*, int, int); //Read one byte, give ack/nack
	int (*i2c_readn)(void*, unsigned char*, int, int); //Read n bytes, give ack/nack
	int (*i2c_write)(void*, unsigned char, int); //Write one byte, check ack/nack
	int (*i2c_start)(void*);
	int (*i2c_stop)(void*); //sending Start/Stop Condition
};

#define MAX_PAC193X_CHANNELS 4
struct pac193x_reg_data {
	double	vbus[MAX_PAC193X_CHANNELS];
	double	vsense[MAX_PAC193X_CHANNELS];

	char	num_enabled_channels;
};

struct power_device {
	struct device device;
	int (*power_get_addr)(void*);
	int (*power_get_group)(void*);
	int (*power_get_sensor)(void*);
	int (*power_get_cur_res)(void*);
	int (*power_get_unused_res)(void*);
	void (*power_set_hwfilter)(void*, int);
	void (*power_set_bipolar)(void* , int);
	int (*power_write_bipolar)(void* , int);
	int (*power_write_no_skip)(void* , int);
	int (*power_set_snapshot)(void*);
	int (*power_get_data)(void*, struct pac193x_reg_data*);
	int (*switch_sensor)(void *, int i);
};

struct gpio_device {
	struct device device;
	int (*gpio_read)(void*, unsigned char*);
	int (*gpio_write)(void*, unsigned char);
	int (*gpio_toggle)(void*);//you dont have to implement unless it is used in monitor
	int (*gpio_get_output)(void*, unsigned char*);//you dont have to implement unless it is used in monitor
	unsigned char pin_bitmask; // specify which pins are output
	int active_level;
	int opendrain;
};

struct eeprom_device {
	struct device device;
	int (*eeprom_read)(void*, unsigned char*, unsigned int, int, unsigned char*);
	int (*eeprom_write)(void*, unsigned char*, unsigned int, int, unsigned char*);
	int (*eeprom_erase)(void*);
	int (*eeprom_check_board)(void*);//you dont have to implement unless it is used in monitor
};

struct temp_device {
	struct device device;
	float (*temp_read)(void*);
	int (*temp_enable)(void*, int);
};

struct name_and_init_func {
	char* name;
	void* (*create_funcptr)(char*, void*);
};

///////////////////////////////////////////////////////////////////////
struct pct2075 {
	struct temp_device temp_dev;
	int addr;
};
float temp_read(void* pct2075);
int temp_enable(void* pct2075, int enable);
void* pct2075_create(char* chip_specification, void* parent);
///////////////////////////////////////////////////////////////////////
enum eeprom_type {
	EEPROM_TYPE_AT24C02,
	EEPROM_TYPE_AT24C32,
};

struct at24cxx {
	struct eeprom_device eeprom_device;
	// int channel; //indicate which i2c channel to choose
	int addr;
	int type;
};
int at24cxx_erase(void* at24cxx);
int at24cxx_read(void* at24cxx, unsigned char* data_buffer, unsigned int startaddr, int size, unsigned char* sn_buf);
int at24cxx_write(void* at24cxx, unsigned char* data_buffer, unsigned int startaddr, int size, unsigned char* sn_buf);
int at24cxx_check_board(void* at24cxx);
void* at24cxx_create(char* chip_specification, void* parent);
///////////////////////////////////////////////////////////////////////
struct pca9548 {
	struct i2c_device i2c_device;
	int channel; //indicate which i2c channel to choose
	int addr;
};
int pca9548_read(void* pca9548, unsigned char* data_buffer, int is_nack, int type);
int pca9548_readn(void* pca9548, unsigned char* data_buffer, int type, int len);
int pca9548_write(void* pca9548, unsigned char data, int type);
int pca9548_start(void* pca9548);
int pca9548_stop(void* pca9548);
void* pca9548_create(char* chip_specification, void* parent);

int pca9548_set_channel(struct pca9548* pca9548);

///////////////////////////////////////////////////////////////////////
struct ft4232h_eeprom {
	struct eeprom_device eeprom_device;
	struct ftdi_info* ftdi_info;
	int uasize;
};
int ft4232h_eeprom_read(void* ft_eeprom, unsigned char* data_buffer, unsigned int startaddr, int size, unsigned char* sn_buf);
int ft4232h_eeprom_write(void* ft_eeprom, unsigned char* data_buffer, unsigned int startaddr, int size, unsigned char* sn_buf);
int ft4232h_eeprom_erase(void* ft_eeprom);
void* ft4232h_eeprom_create(char* chip_specification, void* parent);
///////////////////////////////////////////////////////////////////////
struct ft4232h {
	struct i2c_device i2c_device;
	struct ftdi_info* ftdi_info;
	int channel;
	unsigned char dir_bitmask;//define direction of the pins, 1 is outputm, 0 is input
	unsigned char val_bitmask; //define the output value of the bitmask, the value of each bit is only valid when the coresponding direction bit is 1
	int isinit;
};
int ft4232h_i2c_read(void* ft4232h, unsigned char* data_buffer, int is_nack, int type);
int ft4232h_i2c_readn(void* ft4232h, unsigned char* data_buffer, int type, int len);
int ft4232h_i2c_write(void* ft4232h, unsigned char data, int type);
int ft4232h_i2c_start(void* f2232h);
int ft4232h_i2c_stop(void* ft4232h);
void* ft4232h_i2c_create(char* chip_specification, void* parent);
int ft4232h_i2c_init(struct ft4232h* ft);
int ft4232h_i2c_free(void* ft4232h);
void ft4232h_i2c_remove_all(int disable_1_exit_handler);
///////////////////////////////////////////////////////////////////////

struct ft4232h_gpio {
	struct gpio_device gpio_device;
	struct ftdi_info* ftdi_info;
	int channel;
	unsigned char pin_bitmask;
	int isinit;
};
int ft4232h_gpio_read(void* ft4232h, unsigned char* bit_value_buffer);
int ft4232h_gpio_write(void* ft4232h, unsigned char bit_value);
int ft4232h_gpio_toggle(void* ft4232h);
void* ft4232h_gpio_create(char* chip_specification, void* parent);
int ft4232h_gpio_free(void* ft4232h);
///////////////////////////////////////////////////////////////////////

#define PAC1934_REG_VBUS1_ADDR 0x07
#define PAC1934_REG_VBUS1_AVG_ADDR 0x0F
#define PAC1934_REG_NEG_PWR_ADDR 0x1D
#define PAC1934_REG_CHANNEL_DIS_AND_SMBUS_ADDR 0x1C

struct pac1934 {
	struct power_device power_device;
	int group;
	int sensor;
	int addr;
	int rs1;
	int group2;
	int sensor2;
	int rs2;
	int cur_group;
	int cur_sensor;
	int cur_rs;
	int hwfilter;
	int bipolar;
	int no_skip;
};
int pac1934_switch(void *pac1934, int i);
int get_pac1934_addr(void* pac1934);
int get_pac1934_group(void* pac1934);
int get_pac1934_sensor(void* pac1934);
int get_pac1934_cur_res(void* pac1934);
int get_pac1934_unused_res(void* pac1934);
void pac1934_set_hwfilter(void* pac1934, int onoff);
void pac1934_set_bipolar(void* pac1934, int value);
int pac1934_write_bipolar(void* pac1934, int value);
int pac1934_write_no_skip(void* pac1934, int value);
int pac1934_snapshot(void* pac1934);
int pac1934_get_data(void* pac1934, struct pac193x_reg_data* pac_reg);
void* pac1934_create(char* chip_specification, void* parent);
////////////////////////////////////////////////////////////////////////
struct pca6408a {
	struct gpio_device gpio_device;
	int addr;
	unsigned char pin_bitmask; //specify which pin to use
};
int pca6408a_read(void* pca6408a, unsigned char* bit_value_buffer);
int pca6408a_write(void* pca6408a, unsigned char bit_value);
int pca6408a_toggle(void* pca6408a);
void* pca6408a_create(char* chip_specification, void* parent);
int pca6408a_set_direction(struct pca6408a* pca, unsigned char value);
int pca6408a_get_direction(void* pca6408a, unsigned char* bit_value_buffer);
int pca6408a_set_output(struct pca6408a* pca, unsigned char value);
int pca6408a_get_output(void* pca6408a, unsigned char* current_output);
////////////////////////////////////////////////////////////////////////
struct pca6416a {
	struct gpio_device gpio_device;
	int addr;
	int port;
	unsigned char pin_bitmask; //specify which pin to use
};
int pca6416a_read(void* pca6416a, unsigned char* bit_value_buffer);
int pca6416a_write(void* pca6416a, unsigned char bit_value);
int pca6416a_toggle(void* pca6416a);
void* pca6416a_create(char* chip_specification, void* parent);
int pca6416a_set_direction(struct pca6416a* pca, unsigned char value);
int pca6416a_get_direction(void* pca6416a, unsigned char* bit_value_buffer);
int pca6416a_set_output(struct pca6416a* pca, unsigned char value);
int pca6416a_get_output(void* pca6416a, unsigned char* current_output);
////////////////////////////////////////////////////////////////////////
struct pcal6524h {
	struct gpio_device gpio_device;
	int addr;
	int port;
	unsigned char pin_bitmask; //specify which pin to use
};
int pcal6524h_read(void* pcal6524h, unsigned char* bit_value_buffer);
int pcal6524h_write(void* pcal6524h, unsigned char bit_value);
int pcal6524h_toggle(void* pcal6524h);
void* pcal6524h_create(char* chip_specification, void* parent);
int pcal6524h_set_direction(struct pcal6524h* pca, unsigned char value);
int pcal6524h_set_output(struct pcal6524h* pca, unsigned char value);
int pcal6524h_get_output(void* pcal6524h, unsigned char* current_output);
////////////////////////////////////////////////////////////////////////
struct adp5585 {
	struct gpio_device gpio_device;
	int addr;
	int port;
	unsigned char pin_bitmask; //specify which pin to use
};
int adp5585_read(void* adp5585, unsigned char* bit_value_buffer);
int adp5585_write(void* adp5585, unsigned char bit_value);
int adp5585_toggle(void* adp5585);
void* adp5585_create(char* chip_specification, void* parent);
int adp5585_set_direction(struct adp5585* pca, unsigned char value);
int adp5585_set_output(struct adp5585* pca, unsigned char value);
int adp5585_get_output(void* adp5585, unsigned char* current_output);

#endif //CHIP_H
