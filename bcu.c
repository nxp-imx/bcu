/*
* Copyright 2019-2021 NXP.
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

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS //in order to use strcpy without error
#include <windows.h>
#include <processthreadsapi.h>
#endif

#if defined(linux) || defined(__APPLE__)
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <curses.h>
#include "ftdi.h"
#endif

//common library for both OS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <math.h>

#include "port.h"
#include "bcu_parser.h"
#include "chip.h"
#include "board.h"
#include "version.h"
#include "bcu_yaml.h"
#include "bcu_https.h"
#include "bcu_ftdi_eeprom.h"

#define DONT_RESET	0
#define RESET_NOW	1
#define INIT_WITHOUT_BOOTMODE	2
#define DONT_INIT	0
#define INIT_NOW	1
#define LSBOOTMODE_NSHOWID	0
#define LSBOOTMODE_SHOWID	1
#define GET_COLUMN	0
#define GET_ROW		1
#define DISPLAY_WIDTH_MODE_1	70
#define DISPLAY_WIDTH_MODE_2	84
#define DISPLAY_WIDTH_MODE_3	99
#define DISPLAY_WIDTH_MODE_4	111
#define DISPLAY_WIDTH_MODE_5	138

extern int num_of_boards;
extern struct board_info board_list[];

int GV_MONITOR_TERMINATED = 0;
static int enable_exit_handler = 0;

char* g_vt_red = (char*)"\x1B[91m";
char* g_vt_green = (char*)"\x1B[92m";
char* g_vt_yellow = (char*)"\x1B[93m";
char* g_vt_kcyn = (char*)"\x1B[36m";
char* g_vt_white = (char*)"\x1B[97m";
char* g_vt_magenta = (char*)"\x1B[35m";
char* g_vt_blue = (char*)"\x1B[34m";
#ifdef _WIN32
char* g_vt_back_enable = (char*)"\x1B[4m";
char* g_vt_back_default = (char*)"\x1B[24m";
#endif
#if defined(linux) || defined(__APPLE__)
char* g_vt_back_enable = (char*)"\x1B[100m";
char* g_vt_back_default = (char*)"\x1B[49m";
#endif
char* g_vt_default = (char*)"\x1B[0m";
char* g_vt_clear = (char*)"\x1B[2J";
char* g_vt_clear_remain = (char*)"\x1B[0J";
char* g_vt_clear_line = (char*)"\x1B[K";
char* g_vt_return_last_line = (char*)"\x1B[1A";
char* g_vt_home = (char*)"\x1B[H";

void clean_vt_color()
{
	g_vt_red = (char*)"";
	g_vt_green = g_vt_red;
	g_vt_yellow = g_vt_red;
	g_vt_kcyn = g_vt_red;
	g_vt_white = g_vt_red;
	g_vt_magenta = g_vt_red;
	g_vt_blue = g_vt_red;
	g_vt_back_enable = g_vt_red;
	g_vt_back_default = g_vt_red;
	g_vt_default = g_vt_red;
	g_vt_clear = (char*)"\n";
	g_vt_clear_remain = (char*)"\n";
	g_vt_clear_line = (char*)"\n";
	g_vt_home = (char*)"\n";
}

static void print_version()
{
	printf("version %s\n", GIT_VERSION);
}

#if defined(linux)
char* shellcmd(char* cmd, char* buff, int size)
{
	char temp[256];
	FILE* fp = NULL;
	int offset = 0;
	int len;

	fp = popen(cmd, "r");
	if(fp == NULL)
	{
		return NULL;
	}

	while(fgets(temp, sizeof(temp), fp) != NULL)
	{
		len = strlen(temp);
		if(offset + len < size)
		{
			strcpy(buff+offset, temp);
			offset += len;
		}
		else
		{
			buff[offset] = 0;
			break;
		}
	}

	if(fp != NULL)
	{
		pclose(fp);
	}

	return buff;
}
#endif

static void upgrade_bcu(struct options_setting* setting)
{
	printf("now version %s\n", GIT_VERSION);

	int res = 0;
	char version[15];
	struct latest_git_info bcu_download_info;
	strcpy(bcu_download_info.download_url_base, "https://github.com/nxp-imx/bcu/releases/download/");

	if (setting->download_pre_release == 1)
	{
		if (https_get_by_url("https://api.github.com/repos/nxp-imx/bcu/releases", &bcu_download_info))
		{
			printf("Fail to get the latest pre-released BCU!\n");
			return;
		}
	}
	else
	{
		if (https_get_by_url("https://api.github.com/repos/nxp-imx/bcu/releases/latest", &bcu_download_info))
		{
			printf("Fail to get the latest released BCU!\n");
			return;
		}
	}
	https_response_parse(&bcu_download_info);

	if (setting->download_doc)
	{
		strcpy(bcu_download_info.download_name, "BCU");
		strcpy(bcu_download_info.extension_name, ".pdf");
		https_download(&bcu_download_info);
		return;
	}

	strcpy(bcu_download_info.download_name, "bcu");
#if defined(linux)
	char sysversion[15], sys_ver = 0;

	memset(sysversion, 0, sizeof(sysversion));
	shellcmd("lsb_release -r -s", sysversion, sizeof(sysversion));
	sys_ver = (sysversion[0] - '0') * 10 + (sysversion[1] - '0');
	if (sys_ver == 20)
		strcpy(bcu_download_info.extension_name, "_Ubuntu20");
	else if (sys_ver == 18)
		strcpy(bcu_download_info.extension_name, "_Ubuntu18");
	else {
		printf("Unsupported OS version!\n");
		return;
	}
#elif defined(__APPLE__)
	strcpy(bcu_download_info.extension_name, "_mac");
#else
	strcpy(bcu_download_info.extension_name, ".exe");
#endif

	strncpy(version, GIT_VERSION, 11);
	if (compare_version(&bcu_download_info.tag_name[4], &GIT_VERSION[4]) > 0 || setting->force)
	{
		printf("\nRelease Note for %s:\n%s\n\n", bcu_download_info.tag_name, bcu_download_info.release_note);
		res = https_download(&bcu_download_info);
#if defined(linux) || defined(__APPLE__)
		if (!res)
		{
			char cmd[50] = { 0 }, filename[25] = { 0 };
			strcat(filename, bcu_download_info.tag_name);
			// strcat(filename, bcu_download_info.extension_name);
			sprintf(cmd, "chmod a+x %s", filename);
			system(cmd);
		}
#endif
	}
	else
	{
		printf("Latest release version is %s, no need to upgrade\n", bcu_download_info.tag_name);
	}
}

static void print_help(char* cmd)
{
	if (cmd == NULL) {
		printf("%s\n", "Usage:");
		printf("%s\n\n", "bcu command [-options]");
		printf("%s\n", "list of available commands:");
		printf("	%s%-60s%s%s\n", g_vt_default, "reset  [BOOTMODE_NAME] [-hold=] [-board=/-auto] [-id=]", g_vt_green, "reset the board, and then boot from BOOTMODE_NAME");
		printf("	%s%-60s%s%s\n", g_vt_default, "       [-boothex=] [-bootbin=]", g_vt_green, "or the boot mode value set by [-boothex=] [-bootbin=]");
		printf("	%s%-60s%s%s\n", g_vt_default, "onoff  [-hold=] [-board=/-auto] [-id=]", g_vt_green, "press the ON/OFF button once for -hold= time(ms)");
		printf("	%s%-60s%s%s\n", g_vt_default, "init   [BOOTMODE_NAME] [-board=/-auto] [-id=]", g_vt_green, "enable the remote control with a boot mode");
		printf("	%s%-60s%s%s\n", g_vt_default, "deinit [BOOTMODE_NAME] [-board=/-auto] [-id=]", g_vt_green, "disable the remote control");
		printf("\n");
		printf("	%s%-60s%s%s\n", g_vt_default, "monitor [-board=/-auto] [-id=]", g_vt_green, "monitor power consumption");
		printf("	%s%-60s%s%s\n", g_vt_default, "        [-dump/-dump=] [-nodisplay] [-pmt] [-stats]", g_vt_green, "");
		printf("	%s%-60s%s%s\n", g_vt_default, "        [-hz=] [-rms]", g_vt_green, "");
		printf("	%s%-60s%s%s\n", g_vt_default, "        [-hwfilter] [-unipolar]", g_vt_green, "");
		printf("	%s%-60s%s%s\n", g_vt_default, "        [-temp]", g_vt_green, "");
		printf("\n");
		printf("	%s%-60s%s%s\n", g_vt_default, "server  [-board=/-auto] [-id=]", g_vt_green, "monitor power consumption");
		printf("	%s%-60s%s%s\n", g_vt_default, "        [-hwfilter] [-unipolar]", g_vt_green, "");
		printf("\n");
		printf("	%s%-60s%s%s\n", g_vt_default, "eeprom  [-w] [-r] [-erase]", g_vt_green, "EEPROM read and program");
		printf("	%s%-60s%s%s\n", g_vt_default, "        [-wsn=] [-brev=] [-srev=]", g_vt_green, "");
		printf("\n");
		printf("	%s%-60s%s%s\n", g_vt_default, "temp    [-board=/-auto] [-id=]", g_vt_green, "Get temperature value");
		printf("	%s%-60s%s%s\n", g_vt_default, "get_level [GPIO_NAME] [-board=/-auto] [-id=]", g_vt_green, "get level state of pin GPIO_NAME");
		printf("	%s%-60s%s%s\n", g_vt_default, "set_gpio [GPIO_NAME] [1/0] [-board=/-auto] [-id=]", g_vt_green, "set pin GPIO_NAME to be high(1) or low(0)");
		printf("	%s%-60s%s%s\n", g_vt_default, "set_boot_mode [BOOTMODE_NAME] [-board=/-auto] [-id=]", g_vt_green, "set BOOTMODE_NAME as boot mode");
		printf("	%s%-60s%s%s\n", g_vt_default, "              [-boothex=] [-bootbin=]", g_vt_green, "");
		printf("	%s%-60s%s%s\n", g_vt_default, "get_boot_mode [-board=/-auto] [-id=]", g_vt_green, "read the boot mode set by BCU before");
		printf("\n");
		printf("	%s%-60s%s%s\n", g_vt_default, "lsftdi", g_vt_green, "list all boards connected by ftdi device");
		printf("	%s%-60s%s%s\n", g_vt_default, "lsboard", g_vt_green, "list all supported board models");
		printf("	%s%-60s%s%s\n", g_vt_default, "lsbootmode [-board=/-auto]", g_vt_green, "show a list of available BOOTMODE_NAME of a board");
		printf("	%s%-60s%s%s\n", g_vt_default, "lsgpio     [-board=/-auto]", g_vt_green, "show a list of available GPIO_NAME of a board");
		printf("\n");
		printf("	%s%-60s%s%s\n", g_vt_default, "upgrade    [-doc] [-f] [-pre]", g_vt_green, "get the latest BCU release");
#ifndef __APPLE__
		printf("	%s%-60s%s%s\n", g_vt_default, "uuu        [-doc]", g_vt_green, "download the latest UUU");
#endif
		printf("\n");
		printf("	%s%-60s%s%s\n", g_vt_default, "version", g_vt_green, "print version number");
		printf("	%s%-60s%s%s%s\n", g_vt_default, "-h,  help", g_vt_green, "show command details", g_vt_default);
		printf("	%s%-60s%s%s%s\n", g_vt_default, "-cp, conf_path", g_vt_green, "show config file path", g_vt_default);
		// printf("	%s%-60s%s%s%s\n", g_vt_default, "help [COMMAND_NAME]", g_vt_green, "show details and options of COMMAND_NAME", g_vt_default);

#if defined(__linux__) || defined(__APPLE__)
		printf("%s", g_vt_kcyn);
		printf("\n***please remember to run BCU with sudo or config the udev rules%s\n\n\n", g_vt_default);
#endif
	}
	else
	{
		printf("not yet added the detail explanation of each command\n");
	}
}

static void lsgpio(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	int i = 0;
	printf("\navailable gpio:\n\n");
	while (board->mappings[i].name != NULL)
	{
		if (board->mappings[i].type == gpio)
		{
			printf("	%s\n", board->mappings[i].name);
		}
		i++;
	}
}

static void lsboard(struct options_setting* setting)
{
	printf("\nlist of supported board model:\n\n");
	for (int i = 0; i < num_of_boards; i++)
	{
		if (strcmp(setting->board, board_list[i].name) == 0)
		{
			printf("%s", g_vt_green);
		}
		printf("	%s", board_list[i].name);
		if (strcmp(setting->board, board_list[i].name) == 0)
		{
			printf(" (default)");
		}
		printf("%s\n", g_vt_default);
	}
	return;
}

static int lsbootmode(struct options_setting* setting, int show_id)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return -1;
	int i = 0;
	printf("\navailable boot mode:\n\n");
	while (board->boot_modes[i].name != NULL)
	{
		if (show_id)
			printf("%d	%s\n", i, board->boot_modes[i].name);
		else
			printf("	%s\n", board->boot_modes[i].name);
		i++;
	}

	return i;
}

struct gpio_device* get_gpio(char* gpio_name, struct board_info* board)
{
	int i = 0;
	char path[MAX_PATH_LENGTH];
	void* head = NULL;
	void* end_point;
	struct gpio_device* gpio = NULL;

	if (get_path(path, gpio_name, board) == -1)
		return NULL;
	end_point = build_device_linkedlist_forward(&head, path);
	if (end_point == NULL)
	{
		printf("get_gpio: error building device linked list\n");
		return NULL;
	}
	gpio = end_point;

	return gpio;
}

int get_gpio_id(char* gpio_name, struct board_info* board)
{
	int id = 0;
	char path[MAX_PATH_LENGTH];

	id = get_path(path, gpio_name, board);

	return id;
}

int set_gpiod(struct gpio_device* gpio, int onoff)
{
	int ret = 0;

	if (gpio == NULL)
		return -1;

	if (gpio->active_level == 0)
	{
		if (onoff == 0)
			ret = gpio->gpio_write(gpio, 0xFF);
		else
			ret = gpio->gpio_write(gpio, 0x00);
	}
	else
	{
		if (onoff == 0)
			ret = gpio->gpio_write(gpio, 0x00);
		else
			ret = gpio->gpio_write(gpio, 0xFF);
	}

	return ret;
}

int get_gpiod(struct gpio_device* gpio, unsigned char* data)
{
	int ret = 0;
	unsigned char tmp = -1;

	if (gpio == NULL)
		return -1;

	if (gpio->active_level == 0)
	{
		ret = gpio->gpio_get_output(gpio, &tmp);
		if (ret < 0)
			return ret;

		if (tmp > 0)
			*data = 0;
		else
			*data = 1;
	}
	else
		ret = gpio->gpio_get_output(gpio, data);

	return ret;
}

void free_gpio(struct gpio_device* gpio)
{
	void* end_point = (void*)gpio;
	free_device_linkedlist_backward(end_point);
	gpio = NULL;
	return;
}

static void get_temp(struct options_setting* setting)//
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	void* head = NULL;
	void* end_point;
	char path[MAX_PATH_LENGTH];
	int status = -1;

	if (strlen(setting->path) == 0)
	{
		if (get_path(path, "temp", board) == -1) {
			printf("temperature: failed to find temperature path\n"); return;
		}
		end_point = build_device_linkedlist_forward(&head, path);
	}
	else
	{
		end_point = build_device_linkedlist_forward(&head, setting->path);
	}

	if (end_point == NULL)
	{
		printf("temperature: error building device linked list\n");
		return;
	}

	struct temp_device* temp = end_point;
	float degrees = 0;

	temp->temp_enable(temp, 1);
	degrees = temp->temp_read(temp);
	temp->temp_enable(temp, 0);

	printf("Temperature is %.3f Celsius.\n", degrees);

	free_device_linkedlist_backward(end_point);
}

static void set_gpio(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	void* head = NULL;
	void* end_point;
	char path[MAX_PATH_LENGTH];
	int status = -1;

	if (setting->output_state == -1)
	{
		printf("please enter a valid output state, 1 to set logic level high, 0 to set it low\n");
		return;
	}

	if (strlen(setting->path) == 0)
	{
		if (strlen(setting->gpio_name) == 0)
		{
			printf("could not detect a valid gpio name entered\n");
			printf("please enter the name of the gpio pin,\n");
			printf("to see a list of available gpio pin, please use command lsgpio\n");
			return;

		}
		if (get_path(path, setting->gpio_name, board) == -1) {
			printf("failed to find gpio path\n"); return;
		}
		end_point = build_device_linkedlist_forward(&head, path);
	}
	else
	{
		end_point = build_device_linkedlist_forward(&head, setting->path);
	}

	if (end_point == NULL)
	{
		printf("set_gpio: error building device linked list\n");
		return;
	}

	//delay
	msleep(setting->delay);

	struct gpio_device* gpio = end_point;
	if (setting->output_state == 1)
		status = gpio->gpio_write(gpio, 0xFF);
	else if (setting->output_state == 0)
		status = gpio->gpio_write(gpio, 0x00);
	else if (setting->output_state == 2)
		status = gpio->gpio_toggle(gpio);

	if (status)
		printf("set gpio failed, error = 0x%x\n", status);
	else
		printf("set gpio successfully\n");

	//hold time
	msleep(setting->hold);

	free_device_linkedlist_backward(end_point);
}

static void get_gpio_level(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	void* head = NULL;
	void* end_point;
	char path[MAX_PATH_LENGTH];
	int status = -1;
	unsigned char buff = 0;

	if (strlen(setting->path) == 0)
	{
		if (strlen(setting->gpio_name) == 0)
		{
			printf("could not detect a valid gpio name entered\n");
			printf("please enter the name of the gpio pin,\n");
			printf("to see a list of available gpio pin, please use command lsgpio\n");
			return;

		}
		if (get_path(path, setting->gpio_name, board) == -1) {
			printf("failed to find gpio path\n"); return;
		}
		end_point = build_device_linkedlist_forward(&head, path);
	}
	else
	{
		end_point = build_device_linkedlist_forward(&head, setting->path);
	}

	if (end_point == NULL)
	{
		printf("get_gpio_level: error building device linked list\n");
		return;
	}

	struct gpio_device* gpio = end_point;

	status = gpio->gpio_read(gpio, &buff);

	if (status)
		printf("get gpio failed, error = 0x%x\n", status);
	else
		printf("get %s level=%s\n", setting->gpio_name, buff ? "HIGH" : "LOW");

	free_device_linkedlist_backward(end_point);
}

static void set_boot_config(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;

	for (int config_num = 0; config_num < board->boot_cfg_byte_num; config_num++)
	{
		if (setting->boot_config_hex[config_num] == -1)
		{
			printf("could not detect a valid boot_config_hex_%d!\n", config_num);
			printf("set_boot_config failed\n");
			return;
		}
		char cfg_str[10] = "boot_cfg";
		char num_str[2] = "0";
		num_str[0] += config_num;
		strcat(cfg_str, num_str);
		gpio = get_gpio(cfg_str, board);
		if (gpio == NULL)
		{
			printf("set_boot_config: No boot_cfg%d configuration!\n", config_num);
			return;
		}

		if (get_boot_mode_offset(gpio->pin_bitmask) < 0)
		{
			free_gpio(gpio);
			return;
		}

		unsigned char hex_with_offset = setting->boot_config_hex[config_num] << (get_boot_mode_offset(gpio->pin_bitmask));
		status = gpio->gpio_write(gpio, hex_with_offset);

		if (status)
			printf("set boot config %d failed, error = 0x%x\n", config_num, status);
		else
			printf("set boot config %d successfully\n", config_num);

		free_gpio(gpio);
	}
}

static void set_boot_mode(struct options_setting* setting)
{
	if (setting->boot_mode_hex == -1)
	{
		printf("could not detect a valid boot_mode,\nplease entered a valid boot mode\n");
		printf("set_boot_mode failed\n");
		return;
	}
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;

	gpio = get_gpio("boot_mode", board);
	if (gpio == NULL)
	{
		printf("set_boot_mode: No boot_mode configuration!\n");
		return;
	}

	if (get_boot_mode_offset(gpio->pin_bitmask) < 0)
	{
		free_gpio(gpio);
		return;
	}

	unsigned char hex_with_offset = setting->boot_mode_hex << (get_boot_mode_offset(gpio->pin_bitmask));
	status = gpio->gpio_write(gpio, hex_with_offset);

	if (status)
		printf("set boot mode failed, error = 0x%x\n", status);
	else
		printf("set boot mode successfully\n");

	free_gpio(gpio);

	if (board->boot_cfg_byte_num > 0)
		set_boot_config(setting);
}

static void get_boot_config(struct options_setting* setting, unsigned char boot_modehex)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;
	unsigned char read_buf;
	int read_boot_config_hex[MAX_BOOT_CONFIG_BYTE] = { 0 };

	for (int config_num = 0; config_num < board->boot_cfg_byte_num; config_num++)
	{
		char cfg_str[10] = "boot_cfg";
		char num_str[2] = "0";
		num_str[0] += config_num;
		strcat(cfg_str, num_str);

		gpio = get_gpio(cfg_str, board);
		if (gpio == NULL)
		{
			printf("get_boot_config: No boot_mode configuration!\n");
			free_gpio(gpio);
			return;
		}
		if (get_boot_mode_offset(gpio->pin_bitmask) < 0)
		{
			free_gpio(gpio);
			return;
		}
		status = gpio->gpio_read(gpio, &read_buf);
		read_buf = read_buf >> get_boot_mode_offset(gpio->pin_bitmask);
		if (status)
			printf("get_boot_config %d failed, error = 0x%x\n", config_num, status);
		else
			read_boot_config_hex[config_num] = read_buf;

		free_gpio(gpio);
	}

	char *bootmodestr = get_boot_config_name_from_hex(board, read_boot_config_hex, boot_modehex);
	if (bootmodestr == NULL)
		printf("get_boot_config: cannot find the boot config string.\n");
	else
	{
		printf("get_boot_mode: %s%s%s, ",
			g_vt_red, bootmodestr, g_vt_default);
		printf("boot_mode_hex: %s0x%x%s, ",
			g_vt_red, boot_modehex, g_vt_default);
		for (int i = 0; i < board->boot_cfg_byte_num; i++)
			printf("boot_config_%d_hex: %s0x%x%s\n", i,
				g_vt_red, read_boot_config_hex[i], g_vt_default);
	}
}

static void get_boot_mode(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;
	unsigned char read_buf;

	gpio = get_gpio("bootmode_sel", board);
	if (gpio != NULL)
	{
		status = gpio->gpio_read(gpio, &read_buf);
		if (status)
			printf("get_boot_mode failed, error = 0x%x\n", status);
		if (read_buf > 0)
			read_buf = 1;
		if (read_buf != (board->mappings[get_gpio_id("bootmode_sel", board)].initinfo & 0xF))
		{
			printf("get_boot_mode: bootmode_sel is disabled, boot from BOOT SWITCH!\n");
			free_gpio(gpio);
			return;
		}
		free_gpio(gpio);
	}
	else
	{
		gpio = get_gpio("remote_en", board);
		if (gpio == NULL)
		{
			printf("get_boot_mode: Cannot find gpio remote_en!\n");
			free_gpio(gpio);
			return;
		}
		status = gpio->gpio_read(gpio, &read_buf);
		if (status)
			printf("get_boot_mode failed, error = 0x%x\n", status);
		if (read_buf > 0)
			read_buf = 1;
		if (read_buf != (board->mappings[get_gpio_id("remote_en", board)].initinfo & 0xF))
		{
			printf("get_boot_mode: remote_en is disabled, boot from BOOT SWITCH!\n");
			free_gpio(gpio);
			return;
		}
		free_gpio(gpio);
	}

	gpio = get_gpio("boot_mode", board);
	if (gpio == NULL)
	{
		printf("get_boot_mode: No boot_mode configuration!\n");
		free_gpio(gpio);
		return;
	}
	if (get_boot_mode_offset(gpio->pin_bitmask) < 0)
	{
		free_gpio(gpio);
		return;
	}
	status = gpio->gpio_read(gpio, &read_buf);
	read_buf = read_buf >> get_boot_mode_offset(gpio->pin_bitmask);
	free_gpio(gpio);
	if (status)
		printf("get_boot_mode failed, error = 0x%x\n", status);
	else
	{
		if (get_boot_mode_name_from_hex(board, read_buf) == NULL)
			printf("get_boot_mode hex value: %s0x%x%s, cannot find the boot mode string.\n",
				g_vt_red, read_buf, g_vt_default);
		else
		{
			if (!board->boot_cfg_byte_num)
				printf("get_boot_mode: %s%s%s, hex value: %s0x%x%s\n",
					g_vt_red, get_boot_mode_name_from_hex(board, read_buf),
					g_vt_default, g_vt_red, read_buf, g_vt_default);
			else
				get_boot_config(setting, read_buf);
		}
	}
}

static void deinitialize(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;
	int mask;

	gpio = get_gpio("remote_en", board);
	if (gpio == NULL)
	{
		printf("deinitialize: Cannot find gpio remote_en!\n");
		return;
	}
	mask = board->mappings[get_gpio_id("remote_en", board)].initinfo & 0xF;
	status = gpio->gpio_write(gpio, mask ? 0x00 : 0xFF); //set it off.
	if (!status)
		printf("%sDISABLE%s remote control: remote_en\n", g_vt_red, g_vt_default);
	free_gpio(gpio);

	gpio = get_gpio("bootmode_sel", board);
	if (gpio != NULL)
	{
		mask = board->mappings[get_gpio_id("bootmode_sel", board)].initinfo & 0xF;
		status = gpio->gpio_write(gpio, mask ? 0x00 : 0xFF); //set it off.
		if (!status)
			printf("%sDISABLE%s remote control: bootmode_sel\n", g_vt_red, g_vt_default);
		free_gpio(gpio);
	}

	gpio = get_gpio("ft_reset_boot_mode", board);
	if (gpio != NULL)
	{
		status = gpio->gpio_write(gpio, 0x00);
		msleep(50);
		status = gpio->gpio_write(gpio, 0xFF);
		if (!status)
			printf("%sRESET%s boot mode pin: ft_reset_boot_mode\n", g_vt_red, g_vt_default);
		free_gpio(gpio);
	}
}

static int initialize(struct options_setting* setting, int isreset)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return -1;
	void* head = NULL;
	void* end_point;
	char path[MAX_PATH_LENGTH];
	char name[MAX_MAPPING_NAME_LENGTH];
	int status = 0, initid = 1, output = 0, k = 0;

	while (output >= 0)
	{
		output = get_gpio_info_by_initid(name, path, initid, board);
		if (output < 0)
		{
			if (!isreset)
				printf("board initialization finished\n");
			break;
		}

		if (strcmp(name, "boot_mode") == 0)
		{
			if (setting->boot_mode_hex != -1)
				set_boot_mode(setting);
			else
			{
				if (isreset == RESET_NOW)
				{
					printf("will boot by %sBOOT SWITCH%s\n", g_vt_yellow, g_vt_default);
				}
				else if (isreset == DONT_RESET)
				{
					printf("please give boot_mode, assuming 'sd' this time\n");
					while (board->boot_modes[k].name != NULL)
					{
						if (strcmp(board->boot_modes[k].name, "sd") == 0)
						{
							setting->boot_mode_hex = board->boot_modes[k].boot_mode_hex;
							break;
						}
						k++;
					}
					if (setting->boot_mode_hex != -1)
						set_boot_mode(setting);
					else
					{
						printf("could not recognize boot mode: sd, please give boot_mode\n");
						printf("initialization failed\n");
						return -1;
					}
				}
				else
				{
					printf("will not set boot mode\n");
				}
			}

			initid++;
			continue;
		}

		end_point = build_device_linkedlist_forward(&head, path);
		if (end_point == NULL)
		{
			printf("initialize: error building device linked list\n");
			break;
		}

		struct gpio_device* gpio = end_point;
		if (output)
			status = gpio->gpio_write(gpio, 0xFF);
		else
			status = gpio->gpio_write(gpio, 0x00);

		if (status)
		{
			printf("set %s %s failed, error = 0x%x\n", name, output ? "high" : "low", status);
			printf("%sboard initialization failed!%s\n", g_vt_red, g_vt_default);
			return -1;
		}
		else
		{
			if (strcmp(name, "remote_en") == 0)
				printf("%sENABLE%s remote control\n", g_vt_green, g_vt_default);
			else
				printf("set %s %s successfully\n", name, output ? "high" : "low");
		}

		initid++;

		free_device_linkedlist_backward(end_point);
	}

	return 0;
}

static void reset(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;
	int a = 0, mask = 0;
	char sr_name[100];

	status = initialize(setting, RESET_NOW);
	if (status < 0)
	{
		printf("%sboard reset failed!%s\n", g_vt_red, g_vt_default);
		return;
	}

	printf("Set %sALL%s sense resistances to %ssmaller%s ones\n", g_vt_yellow, g_vt_default, g_vt_yellow, g_vt_default);
	while (board->mappings[a].name != NULL)
	{
		if (board->mappings[a].type == power)
		{
			strcpy(sr_name, "SR_");
			strcat(sr_name, board->mappings[a].name);
GET_GPIO:
			gpio = get_gpio(sr_name, board);
			if (gpio == NULL)
			{
				a++;
				continue;
			}
			status = set_gpiod(gpio, 1); //set it active, use bigger range
			free_gpio(gpio);

			if (strncmp(sr_name, "SR_", 3) == 0)
			{
				strcpy(sr_name, "SRD_");
				strcat(sr_name, board->mappings[a].name);
				goto GET_GPIO;
			}
		}
		a++;
	}

	printf("resetting in: %fs\n", board->reset_time_ms / 1000.0);

	gpio = get_gpio("reset", board);
	mask = board->mappings[get_gpio_id("reset", board)].initinfo & 0xF;

	if (gpio == NULL)
	{
		printf("reset: error building device linked list\n");
		return;
	}

	//delay
	msleep(setting->delay);

	status = gpio->gpio_write(gpio, mask ? 0x00 : 0xFF); //reset low
	if (setting->hold)
		msleep(setting->hold);
	else
	{
		if (board->reset_time_ms <= 1000)
			msleep(board->reset_time_ms);
		else
		{
			int i;
			for(i = board->reset_time_ms; i > 0; i = i - 1000)
			{
				printf("\b\b%ds", i / 1000);
				fflush(stdout);
				msleep(1000);
			}
		}
	}
	printf("\n%s%s", g_vt_return_last_line, g_vt_clear_line);

	status |= gpio->gpio_write(gpio, mask ? 0xFF : 0x00) << 1;//reset high

	free_gpio(gpio);

	if (setting->boot_mode_hex == -1)
	{
		if (have_gpio("bootmode_sel", board) != -1)
		{
			gpio = get_gpio("bootmode_sel", board);
			if (gpio == NULL)
			{
				printf("reset: error building device linked list\n");
				return;
			}
			status |= gpio->gpio_write(gpio, 0xFF) << 2; //bootmode_sel high to disable it.
			free_gpio(gpio);

			msleep(10);

			gpio = get_gpio("reset", board);
			mask = board->mappings[get_gpio_id("reset", board)].initinfo & 0xF;
			if (gpio == NULL)
			{
				printf("reset: error building device linked list\n");
				return;
			}
			status |= gpio->gpio_write(gpio, mask ? 0xFF : 0x00) << 3; //reset high
			free_gpio(gpio);

			if (!status)
			{
				printf("%sDISABLE%s remote bootmode control, boot by %sBOOT SWITCH%s\n", g_vt_red, g_vt_default, g_vt_yellow, g_vt_default);
				printf("remote control is still %sENABLED%s\n", g_vt_green, g_vt_default);
			}
		}
		else
		{
			gpio = get_gpio("remote_en", board);
			mask = board->mappings[get_gpio_id("remote_en", board)].initinfo & 0xF;
			if (gpio == NULL)
			{
				printf("reset: error building device linked list\n");
				return;
			}
			status |= gpio->gpio_write(gpio, mask ? 0x00 : 0xFF) << 2; //remote_en low
			if (!status)
				printf("%sDISABLE%s remote control, boot by %sBOOT SWITCH%s\n", g_vt_red, g_vt_default, g_vt_yellow, g_vt_default);
			free_gpio(gpio);
		}
	}

	if (status)
		printf("reset failed, error = 0x%x\n", status);
	else
		printf("reset successfully\n");
}

static void onoff(struct options_setting* setting, int delay_ms, int is_init)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;
	int mask;

	if (is_init)
		initialize(setting, INIT_WITHOUT_BOOTMODE);

	if (delay_ms == 0)
		delay_ms = 500;

	gpio = get_gpio("onoff", board);
	if (gpio == NULL)
	{
		printf("onoff: error building device linked list\n");
		return;
	}

	printf("onoff button will be pressed for %dms\n", delay_ms);

	mask = board->mappings[get_gpio_id("onoff", board)].initinfo & 0xF;

	status = gpio->gpio_write(gpio, mask ? 0x00 : 0xFF); //set it off.
	msleep(delay_ms);
	status = gpio->gpio_write(gpio, mask ? 0xFF : 0x00); //set it on.

	free_gpio(gpio);

	if (status)
		printf("onoff execute failed, error = 0x%x\n", status);
	else
		printf("onoff execute successfully\n");
}

static void uuu(struct options_setting* setting)
{
#ifndef __APPLE__
	int res = 0;
	struct latest_git_info uuu_download_info;
	strcpy(uuu_download_info.download_url_base, "https://github.com/nxp-imx/mfgtools/releases/download/");

	if (https_get_by_url("https://api.github.com/repos/nxp-imx/mfgtools/releases", &uuu_download_info))
	{
		printf("Fail to get the latest UUU!\n");
		return;
	}
	https_response_parse(&uuu_download_info);

	if (setting->download_doc)
	{
		strcpy(uuu_download_info.download_name, "UUU");
		strcpy(uuu_download_info.extension_name, ".pdf");
		https_download(&uuu_download_info);
		return;
	}

	strcpy(uuu_download_info.download_name, "uuu");
#if defined(linux)
	strcpy(uuu_download_info.extension_name, "");
#else
	strcpy(uuu_download_info.extension_name, ".exe");
#endif

	printf("\nRelease Note for %s:\n%s\n\n", uuu_download_info.tag_name, uuu_download_info.release_note);
	res = https_download(&uuu_download_info);
#if defined(linux)
	if (!res)
	{
		char cmd[30];
		sprintf(cmd, "chmod a+x %s", uuu_download_info.tag_name);
		system(cmd);
	}
#endif

#else
	printf("\nuuu is not support MacOS for now.\n");
#endif
}

static int monitor_size(int columns_or_rows)
{
#ifdef _WIN32
	//printf("monitor_dimension not yet implemented for windows\n");
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int columns, rows;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	//printf("columns: %d\n", columns);
	//printf("rows: %d\n", rows);
	if (columns_or_rows == 0)
		return columns;
	else
		return rows;

#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	//printf ("current lines %d\n", w.ws_row);
	//printf ("current columns %d\n", w.ws_col);
	if (columns_or_rows == GET_COLUMN)
		return w.ws_col;
	else
		return w.ws_row;
#endif
}

/*
printf a string,
if string length less than n,
print empty space until the name are filled

if string length larger than n,
printf string only up to n character
*/
static void printfpadding(char* str, size_t n)
{
	if (strlen(str) <= n) {
		printf("%s", str);

		for (size_t i = 0; i < (n - strlen(str)); i++)
			printf(" ");
	}
	else
	{
		char strcopy[MAX_NUMBER_OF_POWER * MAX_MAPPING_NAME_LENGTH];
		strncpy(strcopy, str, n);
		strcopy[n] = '\0';
		printf("%s", strcopy);
	}
}

static void handle_sigint(int sig)
{
	printf("\nterminating monitor command\n");
	GV_MONITOR_TERMINATED = 1;
}

static int get_msecond(unsigned long* current_time)
{
#ifdef _WIN32

	SYSTEMTIME time;
	FILETIME ftime;
	GetSystemTime(&time);
	SystemTimeToFileTime(&time, &ftime);
	*current_time = (ftime.dwLowDateTime + ((UINT64)ftime.dwHighDateTime << 32)) / 10000;

	return 0;
#else
	struct timespec current;
	clock_gettime(CLOCK_REALTIME, &current);
	*current_time = current.tv_sec * 1000.00 + current.tv_nsec / 1000000;
	return 0;
#endif

}

static int eeprom(struct options_setting* setting)
{
	void* head = NULL;
	void* end_point;
	int j = 0;

	struct board_info* board=get_board(setting->board);
	if (board == NULL)
		return -1;

	while(board->mappings[j].name != NULL)
	{
		if(board->mappings[j].type == ftdi_eeprom || board->mappings[j].type == bcu_eeprom)
		{
			printf("\n>>>>>> Registered %s EEPROM on board >>>>>>\n", board->mappings[j].type == ftdi_eeprom ? "FTDI" : "AT24Cxx");
			end_point = build_device_linkedlist_forward(&head, board->mappings[j].path);
			if (end_point == NULL)
			{
				printf("eeprom: error building device linked list\n");
				return -2;
			}

			struct eeprom_device* eeprom = end_point;
			switch (setting->eeprom_function)
			{
			case PARSER_EEPROM_READ_AND_PRINT:
				bcu_ftdi_eeprom_print(eeprom);
				break;
			case PARSER_EEPROM_READ_TO_FILE:
				break;
			case PARSER_EEPROM_WRITE_DEFAULT:
				if (!bcu_ftdi_eeprom_write_default(eeprom, board->eeprom_data))
					printf("Write %s default values to FTDI EEPROM successfully\n", setting->board);
				if (setting->eeprom_usr_sn)
					if (bcu_ftdi_eeprom_update_usr_sn(eeprom, setting->eeprom_usr_sn))
						return -1;
				if (setting->eeprom_board_rev[0] != 0)
					if (bcu_ftdi_eeprom_update_board_rev(eeprom, setting->eeprom_board_rev))
						return -1;
				if (setting->eeprom_soc_rev[0] != 0)
					if (bcu_ftdi_eeprom_update_soc_rev(eeprom, setting->eeprom_soc_rev))
						return -1;
				bcu_ftdi_eeprom_print(eeprom);
				break;
			case PARSER_EEPROM_WRITE_FROM_FILE:
				break;
			case PARSER_EEPROM_UPDATE_USER_SN:
				bcu_ftdi_eeprom_update_usr_sn(eeprom, setting->eeprom_usr_sn);
				bcu_ftdi_eeprom_print(eeprom);
				break;
			case PARSER_EEPROM_ERASE:
				bcu_ftdi_eeprom_erase(eeprom);
			default:
				break;
			}
		}
		j++;
	}

	return 0;
}

/*
test code for testing monitor
void monitor_test(struct options_setting* setting)
{
	signal(SIGINT, handle_sigint);
	struct board_info* board=get_board(setting->board);
	char* previous_path=NULL;
	void* head=NULL;
	void* end_point=NULL;

	float voltage=0;
	float current=0;
	float power=0;

	//if(1)
	while(!GV_MONITOR_TERMINATED)
	{
		int i=0;//i is the index of all mappings
		while(board->mappings[i].name!=NULL)
		{
			if(board->mappings[i].type==power)
			{
				end_point=build_device_linkedlist_smart2(&head,board->mappings[i].path,head,previous_path);
				previous_path=board->mappings[i].path;

				if(end_point==NULL){
					printf("monitor:failed to build device linkedlist\n");
					return;
				}
				//struct power_device* pd= end_point;
				//pd->power_get_voltage(pd,&voltage);
				//msleep(1);
				//pd->power_get_current(pd,&current);
				//float power=current*voltage;


			}
			i++;
		}
		msleep(500);

	}
	free_device_linkedlist_backward(end_point);
	return;
}
*/

#ifdef _WIN32
#include <conio.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#endif
/*check if keyboard is hit*/
static int kb_hit() 
{
#ifdef _WIN32
	return _kbhit();
#else	
	struct termios term;
	tcgetattr(0, &term);

	struct termios term2 = term;
	term2.c_lflag &= ~ICANON;
	tcsetattr(0, TCSANOW, &term2);

	int byteswaiting;
	ioctl(0, FIONREAD, &byteswaiting);

	tcsetattr(0, TCSANOW, &term);

	return byteswaiting > 0;
#endif
}

static char catch_input_char()
{
	char ch = ' ';
	if (kb_hit())
	{
#ifdef _WIN32
		ch = _getch();
#else
#ifdef __APPLE__
		system("stty -icanon");
#endif
		ch = (char)getchar();
#endif

	}
	return ch;
}

static char catch_input_char_block()
{
	char ch = ' ';

#ifdef _WIN32
		ch = _getch();
#else
		ch = (char)getchar();
#endif

	return ch;
}

static void monitor(struct options_setting* setting)
{
	signal(SIGINT, handle_sigint);
	struct board_info* board = get_board(setting->board);
	if (board == NULL) {
		// printf("entered board model are not supported.\n");
		return;
	}

	char previous_path[MAX_PATH_LENGTH];
	void* head = NULL;
	void* end_point = NULL;

	double voltage = 0;
	double current = 0;
	double power = 0;
	char ch = ' ';
	char sr_path[MAX_PATH_LENGTH];

	char sr_name[100];
	unsigned long start, avgstart, maxminstart;
	get_msecond(&start);
	avgstart = start;
	maxminstart = start;
	unsigned long times = 1;
	FILE* fptr = NULL;
	int reset_flag = 0;

	if (setting->dump == 1)
	{
		fptr = fopen(setting->dumpname, "w+");
		if (fptr == NULL)
		{
			printf("\nOpen file ERROR!\nPlease check if the \"%s\" file is still opened.\nExit...\n", setting->dumpname);
			return;
		}

		/*print first row*/
		int i = 1, index_n;
		fprintf(fptr, "time(ms)");
		index_n = get_power_index_by_showid(i, board);
		while (index_n != -1)
		{
			if (board->mappings[index_n].type == power && board->mappings[index_n].initinfo != 0)
			{
				if (!setting->pmt)
					fprintf(fptr, ",%s voltage(V),%s current(mA)", board->mappings[index_n].name, board->mappings[index_n].name);
				else
					fprintf(fptr, ",%s voltage(V),%s current(mA),%s power(mW)", board->mappings[index_n].name, board->mappings[index_n].name, board->mappings[index_n].name);
			}
			i++;
			index_n = get_power_index_by_showid(i, board);
		}
	}

	int n = 0; //n is the number of variables need to be measured
	int index = 0;
	while (board->mappings[index].name != NULL)
	{
		if (board->mappings[index].type == power)
			n++;
		index++;
	}
	//ideally I should be able to use n as the size of array, but Visual studio does not allow variable length array declaration...
	// so 100 maximum variable for now...I could use dynamic memory allocation instead, but then I will have to free it one by one in the end..
	int name[MAX_NUMBER_OF_POWER];
	double vnow[MAX_NUMBER_OF_POWER]; double vavg[MAX_NUMBER_OF_POWER]; double vmax[MAX_NUMBER_OF_POWER]; double vmin[MAX_NUMBER_OF_POWER];
	double cnow[MAX_NUMBER_OF_POWER]; double cavg[MAX_NUMBER_OF_POWER]; double cmax[MAX_NUMBER_OF_POWER]; double cmin[MAX_NUMBER_OF_POWER];
	double pnow[MAX_NUMBER_OF_POWER]; double pavg[MAX_NUMBER_OF_POWER]; double pmax[MAX_NUMBER_OF_POWER]; double pmin[MAX_NUMBER_OF_POWER];
	double data_size[MAX_NUMBER_OF_POWER];
	double cnow_fwrite[MAX_NUMBER_OF_POWER];
	double pnow_fwrite[MAX_NUMBER_OF_POWER];
	int sr_level[MAX_NUMBER_OF_POWER];
	int range_control = 0;
	int range_level[MAX_NUMBER_OF_POWER] = {0};
	double cur_range[MAX_NUMBER_OF_POWER];
	double unused_range[MAX_NUMBER_OF_POWER];
	float degrees = 0;

	//initialize
	for (int i = 0; i < MAX_NUMBER_OF_POWER; i++)
	{
		vavg[i] = 0; vmax[i] = 0; vmin[i] = 99999;
		cavg[i] = 0; cmax[i] = 0; cmin[i] = 99999;
		pavg[i] = 0; pmax[i] = 0; pmin[i] = 99999;
		data_size[i] = 0;
		name[i] = 0;
	}
	int previous_width = monitor_size(GET_COLUMN);
#if 0
	//initialize all available SR(sense resistor) switch logic level
	for (int k = 0; k < index; k++)
	{
		strcpy(sr_name, "SR_");
		strcat(sr_name, board->mappings[k].name);
		if (get_path(sr_path, sr_name, board) != -1)
		{
			end_point = build_device_linkedlist_smart(&head, sr_path, head, NULL);
			strcpy(previous_path, sr_path);
			if (end_point == NULL) {
				printf("monitor:failed to build device linkedlist\n");
				return;
			}
			struct gpio_device* gd = end_point;
			gd->gpio_write(gd, 0xFF);
		}
	}
#endif

	//power groups
	struct group groups[MAX_NUMBER_OF_POWER];
	groups_init(groups, MAX_NUMBER_OF_POWER);

	int num_of_groups = parse_groups(groups, board);
	if (num_of_groups == -1)
	{
		free_device_linkedlist_backward(end_point);
		if (setting->dump == 1)
		{
			fclose(fptr);
		}
		return;
	}

	if (setting->dump == 1)
	{
		for (int i = 0; i < num_of_groups; i++)
		{
			fprintf(fptr, ",%s Power(mW)", board->power_groups[i].group_name);
		}
		if (setting->temperature)
			fprintf(fptr, ",Temperature (°C)");
		fprintf(fptr, "\n");
	}

	//get first channels of all groups of rails
	int a = 0, pac_group_num = 0, last_pac_group = 0, pac_channel_num = 0;
	struct pac193x_reg_data pac_data[MAX_NUMBER_OF_POWER];
	char pac193x_group_path[MAX_NUMBER_OF_POWER][MAX_PATH_LENGTH];
	while (board->mappings[a].name != NULL)
	{
		if (board->mappings[a].type == power && board->mappings[a].initinfo != 0) //-------------------------------------------------------------------------
		{
			end_point = build_device_linkedlist_smart(&head, board->mappings[a].path, head, previous_path);
			strcpy(previous_path, board->mappings[a].path);

			if (end_point == NULL) {
				printf("monitor:failed to build device linkedlist\n");
				if (setting->dump == 1)
				{
					fclose(fptr);
				}
				return;
			}
			struct power_device* pd = end_point;

			int now_pac_group = pd->power_get_group(pd);
			if(last_pac_group != now_pac_group)
			{
				strcpy(pac193x_group_path[now_pac_group - 1], board->mappings[a].path);
				pac_group_num++;
				last_pac_group = now_pac_group;
			}

			pac_channel_num++;

			if (setting->rangefixed == 1)
			{
				strcpy(sr_name, "SR_");
				strcat(sr_name, board->mappings[a].name);
				if (get_path(sr_path, sr_name, board) != -1)
				{
					end_point = build_device_linkedlist_smart(&head, sr_path, head, previous_path);
					strcpy(previous_path, sr_path);

					if (end_point == NULL) {
						printf("monitor:failed to build device linkedlist\n");
						if (setting->dump == 1)
						{
							fclose(fptr);
						}
						return;
					}
					struct gpio_device* gd = end_point;
					if ((board->mappings[a].initinfo & 0x3) == 0)
					{
						sr_level[a] = 0;
						set_gpiod(gd, 0);
					}
					else if ((board->mappings[a].initinfo & 0x3) == 1)
					{
						sr_level[a] = 1;
						set_gpiod(gd, 1);
					}
				}
				else
				{
					sr_level[a] = -1;
				}
			}
			//get SR(sense resistor) switch logic level
			else
			{
				strcpy(sr_name, "SR_");
				strcat(sr_name, board->mappings[a].name);
GET_PATH1:
				if (get_path(sr_path, sr_name, board) != -1)
				{
					end_point = build_device_linkedlist_smart(&head, sr_path, head, previous_path);
					strcpy(previous_path, sr_path);

					if (end_point == NULL) {
						printf("monitor:failed to build device linkedlist\n");
						if (setting->dump == 1)
						{
							fclose(fptr);
						}
						return;
					}
					struct gpio_device* gd = end_point;
					unsigned char data;
					
					set_gpiod(gd, 1); //initialize all pins to 1

					get_gpiod(gd, &data);

					if (data == 0)
						sr_level[a] = 0;
					else
						sr_level[a] = 1;

					if (strncmp(sr_name, "SR_", 3) == 0)
					{
						strcpy(sr_name, "SRD_");
						strcat(sr_name, board->mappings[a].name);
						goto GET_PATH1;
					}
				}
				else
				{
					if (strncmp(sr_name, "SRD_", 4) != 0)
						sr_level[a] = -1;
				}
			}
		}
		a++;
	}

	int pac_group_count = pac_group_num;
	for(a = 0; a < MAX_NUMBER_OF_POWER; a++)
	{
		if (strlen(pac193x_group_path[a]) < 10)
		{
			continue;
		}
		if (pac_group_count <= 0)
			break;
		end_point = build_device_linkedlist_smart(&head, pac193x_group_path[a], head, previous_path);
		strcpy(previous_path, pac193x_group_path[a]);

		if (end_point == NULL) {
			printf("monitor:failed to build device linkedlist\n");
			if (setting->dump == 1)
			{
				fclose(fptr);
			}
			return;
		}
		struct power_device* pd = end_point;

		if (setting->use_bipolar)
			pd->power_write_bipolar(pd, 1);
		else
			pd->power_write_bipolar(pd, 0);

		if (setting->use_no_skip) //added "no skip" functionality to make possible to use PAC1932 with same library for PAC1934
			pd->power_write_no_skip(pd, 1);
		else
			pd->power_write_no_skip(pd, 0);

		//snapshot once to skip the first data that may not match the polarity configuration
		pd->power_set_snapshot(pd);

		pac_group_count--;
	}

	printf("%s", g_vt_clear);

#if 1
	unsigned long last_display = 0, now_display;
	while (!GV_MONITOR_TERMINATED)
	{
		get_msecond(&now_display);
		int candisplay = 0, interval;
		interval = now_display - last_display;
		if (interval > setting->refreshms || last_display == 0)
		{
			candisplay = 1;
			last_display = now_display;
		}
		else
		{
			candisplay = 0;
		}
		//first refresh all pac1934's
		pac_group_count = pac_group_num;
		for(a = 0; a < MAX_NUMBER_OF_POWER; a++)
		{
			if (strlen(pac193x_group_path[a]) < 10)
			{
				continue;
			}
			if (pac_group_count <= 0)
				break;
			end_point = build_device_linkedlist_smart(&head, pac193x_group_path[a], head, previous_path);
			strcpy(previous_path, pac193x_group_path[a]);

			if (end_point == NULL) {
				printf("monitor:failed to build device linkedlist\n");
				if (setting->dump == 1)
				{
					fclose(fptr);
				}
				return;
			}
			struct power_device* pd = end_point;

			pd->power_set_snapshot(pd);
			pac_group_count--;
		}
		if (pac_group_num < 4)
			msleep(1);
		int cannotacc = 0;
		pac_group_count = pac_group_num;
		for(a = 0; a < MAX_NUMBER_OF_POWER; a++)
		{
			if (pac_group_count <= 0)
				break;

			if (strlen(pac193x_group_path[a]) < 10)
			{
				continue;
			}
			end_point = build_device_linkedlist_smart(&head, pac193x_group_path[a], head, previous_path);
			strcpy(previous_path, pac193x_group_path[a]);

			if (end_point == NULL) {
				printf("monitor:failed to build device linkedlist\n");
				if (setting->dump == 1)
				{
					fclose(fptr);
				}
				return;
			}
			struct power_device* pd = end_point;

			if (setting->use_hwfilter)
				pd->power_set_hwfilter(pd, 1);
			else
				pd->power_set_hwfilter(pd, 0);

			if (setting->use_bipolar)
				pd->power_set_bipolar(pd, 1);
			else
				pd->power_set_bipolar(pd, 0);

			int b;
			for (b = 0; b < 50; b++)
			{
				if (pd->power_get_data(pd, &pac_data[a]) >= 0)
				{
					break;
				}
				else
					cannotacc = 1;
			}
			if (b == 50)
			{
				printf("PAC1934 cannot access!\n");
				return;
			}
			else if(cannotacc == 1)
				break;
			pac_group_count--;
		}
		if(cannotacc == 1)
			continue;
		//calculate the value and store them in array
		int i = 0;//i is the index of all mappings
		int j = 0;//j is the index of the power related mapping only
		while (board->mappings[i].name != NULL)
		{
			if (board->mappings[i].type == power && board->mappings[i].initinfo != 0)
			{
				name[j] = i;

				if (reset_flag == 1)
				{
					strcpy(sr_name, "SR_");
					strcat(sr_name, board->mappings[name[j]].name);
GET_PATH2:
					if (get_path(sr_path, sr_name, board) != -1)
					{
						end_point = build_device_linkedlist_smart(&head, sr_path, head, previous_path);
						strcpy(previous_path, sr_path);

						if (end_point == NULL) {
							printf("monitor:failed to build device linkedlist\n");
							if (setting->dump == 1)
							{
								fclose(fptr);
							}
							return;
						}
						struct gpio_device* gd = end_point;
						unsigned char data;
						get_gpiod(gd, &data);

						if (data == 0)
							sr_level[j] = 0;
						else
							sr_level[j] = 1;

						if (strncmp(sr_name, "SR_", 3) == 0)
						{
							strcpy(sr_name, "SRD_");
							strcat(sr_name, board->mappings[name[j]].name);
							goto GET_PATH2;
						}
					}
					else
					{
						if (strncmp(sr_name, "SRD_", 4) != 0)
							sr_level[j] = -1;
					}
				}

				end_point = build_device_linkedlist_smart(&head, board->mappings[i].path, head, previous_path);
				strcpy(previous_path, board->mappings[i].path);

				if (end_point == NULL) {
					printf("monitor:failed to build device linkedlist\n");
					if (setting->dump == 1)
					{
						fclose(fptr);
					}
					return;
				}
				struct power_device* pd = end_point;

				if(sr_level[j] == 0)
					pd->switch_sensor(pd, 1);
				else
					pd->switch_sensor(pd, 0);

				int group = pd->power_get_group(pd) - 1;
				int sensor = pd->power_get_sensor(pd) - 1;
				voltage = pac_data[group].vbus[sensor] - (pac_data[group].vsense[sensor] / 1000000);
				current = pac_data[group].vsense[sensor] / pd->power_get_cur_res(pd);
				cnow_fwrite[j] = current;
				pnow_fwrite[j] = current * voltage;

				if (range_control == 0)
				{
					range_level[j] = (char)(0 | range_level[j] << 4);  //mA
				}
				else if (range_control == 2)
				{
					current *= 1000;
					range_level[j] = (char)(1 | range_level[j] << 4);  //uA
				}
				else
				{
					if ( (!(range_level[j] & 0xf) && cavg[j] < 1) || ((range_level[j] & 0xf) && cavg[j] < 1000))
					{
						current *= 1000;
						range_level[j] = (char)(1 | range_level[j] << 4);  //uA
					}
					else
						range_level[j] = (char)(0 | range_level[j] << 4);  //mA
				}

				cur_range[j] = 100000.0 / pd->power_get_cur_res(pd);
				unused_range[j] = 100000.0 / pd->power_get_unused_res(pd);
				// cur_range[j] = pac_data[pd->power_get_group(pd) - 1].vsense[pd->power_get_sensor(pd) - 1];

				// printf("current %f\n", current);
				double power = current * voltage;
				vnow[j] = voltage;
				cnow[j] = current;
				pnow[j] = power;
				if (range_level[j] == 0x0 || range_level[j] == 0x11)
				{
					vmin[j] = (voltage < vmin[j]) ? voltage : vmin[j];
					cmin[j] = (current < cmin[j]) ? current : cmin[j];
					pmin[j] = (power < pmin[j]) ? power : pmin[j];
					vmax[j] = (voltage > vmax[j]) ? voltage : vmax[j];
					cmax[j] = (current > cmax[j]) ? current : cmax[j];
					pmax[j] = (power > pmax[j]) ? power : pmax[j];
					vavg[j] = (data_size[j] * vavg[j] + voltage) / (double)(data_size[j] + 1);
					pavg[j] = (data_size[j] * pavg[j] + power) / ((double)(data_size[j] + 1));
				}
				else if (range_level[j] == 0x01)
				{
					vmin[j] = (voltage < vmin[j]) ? voltage : vmin[j];
					cmin[j] = (current < cmin[j] * 1000) ? current : cmin[j] * 1000;
					pmin[j] = (power < pmin[j] * 1000) ? power : pmin[j] * 1000;
					vmax[j] = (voltage > vmax[j]) ? voltage : vmax[j];
					cmax[j] = (current > cmax[j] * 1000) ? current : cmax[j] * 1000;
					pmax[j] = (power > pmax[j] * 1000) ? power : pmax[j] * 1000;
					cavg[j] *= 1000;
					vavg[j] = (data_size[j] * vavg[j] + voltage) / (double)(data_size[j] + 1);
					pavg[j] = (data_size[j] * pavg[j] * 1000 + power) / ((double)(data_size[j] + 1));
				} 
				else if (range_level[j] == 0x10)
				{
					vmin[j] = (voltage < vmin[j]) ? voltage : vmin[j];
					cmin[j] = (current < cmin[j] / 1000) ? current : cmin[j] / 1000;
					pmin[j] = (power < pmin[j] / 1000) ? power : pmin[j] / 1000;
					vmax[j] = (voltage > vmax[j]) ? voltage : vmax[j];
					cmax[j] = (current > cmax[j] / 1000) ? current : cmax[j] / 1000;
					pmax[j] = (power > pmax[j] / 1000) ? power : pmax[j] / 1000;
					cavg[j] /= 1000;
					vavg[j] = (data_size[j] * vavg[j] + voltage) / (double)(data_size[j] + 1);
					pavg[j] = (data_size[j] * pavg[j] / 1000 + power) / ((double)(data_size[j] + 1));
				}

				if (setting->use_rms)
				{
					cavg[j] = sqrt((data_size[j] * cavg[j] * cavg[j] + current * current) / (double)(data_size[j] + 1));
					pavg[j] = cavg[j] * vavg[j];
				}
				else
					cavg[j] = (data_size[j] * cavg[j] + current) / (double)(data_size[j] + 1);

				data_size[j] = data_size[j] + 1;

				j++;
			}
			else
				j++;
			i++;
		}

		if (reset_flag == 1)
			reset_flag = 0;

#if 1
		//get groups data
		for (int k = 0; k < num_of_groups; k++)
		{
			groups[k].sum = 0;
			for (int x = 0; x < groups[k].num_of_members; x++)
			{
				int mi = groups[k].member_index[x];
				if (range_level[mi] == 0x01 || range_level[mi] == 0x11)
					pnow[mi] /= 1000;
				groups[k].sum += pnow[mi];
			}
			groups[k].max = (groups[k].sum > groups[k].max) ? groups[k].sum : groups[k].max;
			groups[k].min = (groups[k].sum < groups[k].min) ? groups[k].sum : groups[k].min;
			groups[k].avg = (groups[k].avg_data_size * groups[k].avg + groups[k].sum) / (groups[k].avg_data_size + 1);
			groups[k].avg_data_size++;
		}

		if (setting->temperature)
		{
			char tpath[MAX_PATH_LENGTH];
			if (get_path(tpath, "temp", board) == -1) {
				printf("temperature: failed to find temperature path\n");
				setting->temperature = 0;
			}
			else
			{
				end_point = build_device_linkedlist_smart(&head, tpath, head, previous_path);

				struct temp_device* temp = end_point;

				degrees = temp->temp_read(temp);
			}
		}

		//dump data to file
		if (setting->dump == 1)
		{
			unsigned long now;
			get_msecond(&now);
			fprintf(fptr, "%ld", (long)now - start);//add time before the first element
			for (int m = 0; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;
				if (board->mappings[k].initinfo != 0)
				{
					if (!setting->pmt)
						fprintf(fptr, ",%lf,%lf", vnow[k], cnow_fwrite[k]);
					else
						fprintf(fptr, ",%lf,%lf,%lf", vnow[k], cnow_fwrite[k], pnow_fwrite[k]);
				}
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				fprintf(fptr, ",%lf", groups[k].sum);
			}
			if (setting->temperature)
				fprintf(fptr, ",%.3f", degrees);
			fprintf(fptr, "\n");
		}

		times++;

		//then display
		int max_length, location_length, available_width, available_height;
		available_width = monitor_size(GET_COLUMN);
		available_height = monitor_size(GET_ROW);
		if (candisplay == 1)
		{
			printf("%s", g_vt_home); //move cursor to the 0,0
			printf("%s", g_vt_clear_line);

		}
		if (setting->nodisplay == 0 && candisplay == 1)
		{
			printf("%s", g_vt_green); //set the word as green

			max_length = (get_max_power_name_length(board) <= 25) ? get_max_power_name_length(board) : 25;
			max_length = (max_length < 8) ? 8 : max_length; //the word "location" has a minimum of 8 letters
			location_length = max_length + 3;//1 for letter, 1 for space between letter and location_name, 1 for space between location and '|''
			if (range_control)
				location_length++; //1 for '*'

			if (available_width - max_length < DISPLAY_WIDTH_MODE_1 || (available_width - max_length <= DISPLAY_WIDTH_MODE_5 && range_control == 2))
			{
				printf("%s", g_vt_clear);
				printf("the command line window's width is too narrow\n");
				printf("current width: %d\n", available_width);
				if (range_control == 2)
					printf("please set window's width to be at least 155 character wide\n");
				else
					printf("please set window's width to be at least 85 character wide\n");
				msleep(1000);
				continue;
			}

			printfpadding(" ", location_length);
			if (available_width - max_length > DISPLAY_WIDTH_MODE_4)
			{
				printf("%s", g_vt_green);
				printf("%-25s", "|Voltage(V)");
				printf("%s", g_vt_yellow);
				printf("%s", "|Current(mA)/");
				printf("%s", g_vt_back_enable);
				printf("%s", "(uA)");
				printf("%s", g_vt_back_default);
				printf("%13s", " ");
				printf("%s", g_vt_kcyn);
				printf("%s", "|Power(mWatt)/");
				printf("%s", g_vt_back_enable);
				printf("%s", "(uWatt)");
				printf("%s", g_vt_back_default);
				printf("%9s", " ");
				printf("%s", g_vt_red);
				printf("%-s", "|Max Range Select(mA)");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
				printfpadding("location", location_length);
				printf("%s", g_vt_green);
				printf("|%-5s %-5s %-5s %-5s", "now", "avg", "max", "min");
				printf("%s", g_vt_yellow);
				printf(" |%-6s %-6s %-7s %-6s", "now", setting->use_rms ? "rms" : "avg", "max", "min");
				printf("%s", g_vt_kcyn);
				printf(" |%-6s %-6s %-7s %-6s", "now", setting->use_rms ? "rms" : "avg", "max", "min");
				printf("%s", g_vt_red);
				printf("%-s", " |Range1     |Range2");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
			}
			else if (available_width - max_length > DISPLAY_WIDTH_MODE_3)
			{
				printf("%s", g_vt_green);
				printf("%-13s", "|Voltage(V)");
				printf("%s", g_vt_yellow);
				printf("%s", "|Current(mA)/");
				printf("%s", g_vt_back_enable);
				printf("%s", "(uA)");
				printf("%s", g_vt_back_default);
				printf("%13s", " ");
				printf("%s", g_vt_kcyn);
				printf("%s", "|Power(mWatt)/");
				printf("%s", g_vt_back_enable);
				printf("%s", "(uWatt)");
				printf("%s", g_vt_back_default);
				printf("%9s", " ");
				printf("%s", g_vt_red);
				printf("%-6s", "|Max Range Sel(mA)");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
				printfpadding("location", location_length);
				printf("%s", g_vt_green);

				printf("|%-5s %-5s", "now", "avg");
				printf("%s", g_vt_yellow);
				printf(" |%-6s %-6s %-7s %-6s", "now", setting->use_rms ? "rms" : "avg", "max", "min");
				printf("%s", g_vt_kcyn);

				printf(" |%-6s %-6s %-7s %-6s", "now", setting->use_rms ? "rms" : "avg", "max", "min");
				printf("%s", g_vt_red);
				printf("%-s", " |Range1     |Range2");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
			}
			else if (available_width - max_length > DISPLAY_WIDTH_MODE_2)
			{
				printf("%s", g_vt_green);
				printf("%-11s", "|Voltage(V)");
				printf("%s", g_vt_yellow);
				printf("%s", "|Current(mA)/");
				printf("%s", g_vt_back_enable);
				printf("%s", "(uA)");
				printf("%s", g_vt_back_default);
				printf("%s", g_vt_kcyn);
				printf("%s", "|Power(mWatt)/");
				printf("%s", g_vt_back_enable);
				printf("%s", "(uWatt)");
				printf("%s", g_vt_back_default);
				printf("%s", "         ");
				printf("%s", g_vt_red);
				printf("%-6s", "|Max Range Sel(mA)");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
				printfpadding("location", location_length);
				printf("%s", g_vt_green);
				printf("|%-5s %-5s", "now", "avg");
				printf("%s", g_vt_yellow);
				printf(" |%-6s %-6s", "now", setting->use_rms ? "rms" : "avg");
				printf("%s", g_vt_kcyn);
				printf(" |%-6s %-6s %-7s %-6s", "now", setting->use_rms ? "rms" : "avg", "max", "min");
				printf("%s", g_vt_red);
				printf("%-s", " |Range1     |Range2");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
			}
			else
			{
				printf("%s", g_vt_green);

				printf("%-13s", "|Voltage(V)");
				printf("%s", g_vt_yellow);

				printf("%s", "|Current(mA)/");
				printf("%s", g_vt_back_enable);
				printf("%s", "(uA)");
				printf("%s", g_vt_back_default);
				printf("%s", g_vt_kcyn);

				printf("%s", "|Power(mW)/");
				printf("%s", g_vt_back_enable);
				printf("%s", "(uW)");
				printf("%s", g_vt_back_default);
				printf("%s", g_vt_red);
				printf("%-6s", "|Max Range Sel(mA)");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
				printfpadding("location", location_length);
				printf("%s", g_vt_green);
				printf("|%-5s %-5s", "now", "avg");
				printf("%s", g_vt_yellow);
				printf(" |%-6s %-6s", "now", setting->use_rms ? "rms" : "avg");
				printf("%s", g_vt_kcyn);
				printf(" |%-6s %-6s", "now", setting->use_rms ? "rms" : "avg");
				printf("%s", g_vt_red);
				printf("%-s", " |Range1     |Range2");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
			}
			printfpadding("---------------------------------------------------------------------------------------------------------------------------------------------------------", available_width);

			for (int m = 1; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;

				//printf("%-10s|",board->mappings[name[k]].name);
				printf("%s", g_vt_default);
				if (sr_level[k] > -1)
				{
					if (m < 27)
						printf("%c ", 65 + m - 1);//print corresponding letters, start with A
					else
						printf("%c ", 97 + m - 27);//print corresponding letters, start with a
				}
				else
				{
					printf("  ");
				}

				if (range_level[k] == 0x01 || range_level[k] == 0x11)
					printf("*");
				printfpadding(board->mappings[name[k]].name, max_length);

				printf("%s", g_vt_green);
				if (!(range_level[k] == 0x01 || range_level[k] == 0x11) && range_control)
					printf(" ");
				printf(" |");

				printf("%s", g_vt_green);
				printf("%-5.2f ", vnow[k]);
				printf("%-5.2f ", vavg[k]);
				if (available_width - max_length > DISPLAY_WIDTH_MODE_4)
				{
					printf("%-5.2f ", vmax[k]);
					printf("%-5.2f ", vmin[k]);
				}

				printf("%s", g_vt_yellow);
				printf("|");

				if(range_level[k] == 0x01 || range_level[k] == 0x11)
					printf("%s", g_vt_back_enable);
				printf("%s", g_vt_yellow);

				if (range_control != 2)
				{
					printf("%-6.2f ", cnow[k]);
					printf("%-6.2f ", cavg[k]);
					if (available_width - max_length > DISPLAY_WIDTH_MODE_3)
					{
						printf("%-7.2f ", cmax[k]);
						printf("%-6.2f ", cmin[k]);
					}
				}
				else
				{
					printf("%-9.2f ", cnow[k]);
					printf("%-9.2f ", cavg[k]);
					if (available_width - max_length > DISPLAY_WIDTH_MODE_3)
					{
						printf("%-9.2f ", cmax[k]);
						printf("%-9.2f ", cmin[k]);
					}
				}

				printf("%s", g_vt_kcyn);
				printf("|");

				printf("%s", g_vt_kcyn);
				if (range_control != 2)
				{
					printf("%-6.2f ", pnow[k]);
					printf("%-6.2f ", pavg[k]);
					if (available_width - max_length > DISPLAY_WIDTH_MODE_2)
					{
						printf("%-7.2f ", pmax[k]);
						printf("%-6.2f ", pmin[k]);
					}
				}
				else
				{
					printf("%-10.2f ", pnow[k]);
					printf("%-10.2f ", pavg[k]);
					if (available_width - max_length > DISPLAY_WIDTH_MODE_2)
					{
						printf("%-10.2f ", pmax[k]);
						printf("%-10.2f ", pmin[k]);
					}
				}
				if(range_level[k] == 0x01 || range_level[k] == 0x11)
					printf("%s", g_vt_back_default);

				printf("%s", g_vt_red);
				if (sr_level[k] == -1)
				{
					printf("|[*]%-8.2f", cur_range[k]);
				}
				else
				{
					printf("|[%s]%-8.2f [%s]%-8.2f", sr_level[k] ? "*" : " ", sr_level[k] ? cur_range[k] : unused_range[k], sr_level[k] ? " " : "*", sr_level[k] ? unused_range[k] : cur_range[k]);
				}

				printf("%s\n", g_vt_clear_line);
			}

			//then display group
			int max_group_length = 17;
			if (num_of_groups > 0)
			{
				//display groups
				printf("%s", g_vt_default);
				// printf("\n\n");
				printfpadding("---------------------------------------------------------------------------------------------------------------------------------------------------------", available_width);
				printfpadding(" ", max_group_length + 1);
				printf("%s", g_vt_kcyn);
				printf("|Power(mWatt)");
				printf("%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
				printfpadding("group", max_group_length);
				printf("%s", g_vt_kcyn);
				if(range_control)
					printf(" |%-9s %-9s %-9s %-9s", "now", setting->use_rms ? "rms" : "avg", "max", "min");
				else
					printf(" |%-6s %-6s %-6s %-6s", "now", setting->use_rms ? "rms" : "avg", "max", "min");
				printf("%s", g_vt_default);
				printf(" |Group members%s\n", g_vt_clear_line);
				printf("%s", g_vt_default);
				printfpadding("---------------------------------------------------------------------------------------------------------------------------------------------------------", available_width);
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				printfpadding(groups[k].name, max_group_length);
				printf("%s", g_vt_kcyn);
				printf(" |");
				if(range_control)
				{
					printf("%-9.4f ", groups[k].sum);
					printf("%-9.4f ", groups[k].avg);
					printf("%-9.4f ", groups[k].max);
					printf("%-9.4f", groups[k].min);
				}
				else
				{
					printf("%-6.2f ", groups[k].sum);
					printf("%-6.2f ", groups[k].avg);
					printf("%-6.2f ", groups[k].max);
					printf("%-6.2f", groups[k].min);
				}
				printf("%s", g_vt_default);
				printf(" |");
				// printf("  ");
				if(range_control)
				{
					if (strlen(groups[k].member_list) < (size_t)(monitor_size(GET_COLUMN) - max_group_length - 42))
						printf("%s%s\n", groups[k].member_list, g_vt_clear_line);
					else
					{
						printfpadding(groups[k].member_list, monitor_size(GET_COLUMN) - max_group_length - 42 - 10);
						printf("...%s\n", g_vt_clear_line);
					}
				}
				else
				{
					if (strlen(groups[k].member_list) < (size_t)(monitor_size(GET_COLUMN) - max_group_length - 30))
						printf("%s%s\n", groups[k].member_list, g_vt_clear_line);
					else
					{
						printfpadding(groups[k].member_list, monitor_size(GET_COLUMN) - max_group_length - 30 - 10);
						printf("...%s\n", g_vt_clear_line);
					}
				}
			}

			//printf("width: %d \n",monitor_size(GET_COLUMN));
			printf("%s\n", g_vt_clear_line);
			printf("%s", g_vt_default);
			unsigned long now;
			get_msecond(&now);
			int cap_interval;
			if (setting->temperature)
				printf("SoC Temp: %.3f °C   ", degrees);
			printf("Capture time:");
			cap_interval = now - avgstart;
			if (cap_interval > 10000)
				printf("   %s:%.3fs      ", setting->use_rms ? "RMS" : "Avg", cap_interval / 1000.0);
			else
				printf("   %s:%dms      ", setting->use_rms ? "RMS" : "Avg", cap_interval);
			cap_interval = now - maxminstart;
			if (cap_interval > 10000)
				printf("MinMax:%.3fs      ", cap_interval / 1000.0);
			else
				printf("MinMax:%dms      ", cap_interval);

			if (available_width < 106)
				printf("\n                ");

			if (last_display != 0)
				printf("Display freq: %.2fHz      Total sampling times: %ld%s\n", (1000.0 / interval), times, g_vt_clear_line);
		}

		if (candisplay == 1 && setting->dump == 1)
		{
			unsigned long now;
			get_msecond(&now);
			printf("Dump data to \"%s\" for %ldms, %ld times, %ld times/sec%s\n", setting->dumpname, now - start, times, 1000 / ((now - start) / times), g_vt_clear_line);
			if (setting->nodisplay == 1)
				printf("Ctrl-C to exit...%s\n", g_vt_clear_line);
			printf("%s\n", g_vt_clear_line);
		}
		
		//finally,switch the SR
		if (!setting->nodisplay)
			ch = catch_input_char();
		else
			ch = 0;
		if (setting->nodisplay == 0 && candisplay == 1 && available_height >= 43)
		{
			printf("Hot-key: 1=reset %s; 2=reset MaxMin; 3=reset %s and MaxMin; 4=switch show mA/auto/uA;%s\n",
						setting->use_rms ? "RMS" : "Avg", setting->use_rms ? "RMS" : "Avg", g_vt_clear_line);
			printf("         5=reset board; 6=resume the board; Ctrl-C to exit...%s\n", g_vt_clear_line);
			printf("press the letter on keyboard to control coresponding extra sense resistor(Extra SR)%s\n", g_vt_clear_line);
			printf("pressed %s(Please pay attention to letter case)%s: %c%s\n", g_vt_red, g_vt_default, ch, g_vt_clear_line);
		}
		else if (setting->nodisplay == 0 && candisplay == 1 && available_height < 43 && setting->rangefixed == 0)
		{
			printf("press letter to switch sense resistor%s(Please pay attention to letter case)%s; Ctrl-C to exit...%s\n", g_vt_red, g_vt_default, g_vt_clear_line);
		}
		if (isxdigit(ch))
		{
			int hotkey_index = (int)ch - '0';
			int bootmodenum = 0;
			int input_num = 0;
			switch (hotkey_index)
			{
			case 1:
				for (int k = 0; k < n; k++)
				{
					cavg[k] = 0;
					vavg[k] = 0;
					pavg[k] = 0;
					data_size[k] = 0;
				}
				for (int k = 0; k < num_of_groups; k++)
				{
					groups[k].avg = 0;
					groups[k].avg_data_size = 0;
				}
				get_msecond(&avgstart);
				if (setting->dump == 1 && !setting->pmt)
					fprintf(fptr, "HOT-KEY %d PRESSED: Reset AVG value\n", hotkey_index);
				break;
			case 2:
				for (int k = 0; k < n; k++)
				{
					cmin[k] = 99999;
					vmin[k] = 99999;
					pmin[k] = 99999;
					cmax[k] = 0;
					vmax[k] = 0;
					pmax[k] = 0;
				}
				for (int k = 0; k < num_of_groups; k++)
				{
					groups[k].max = 0;
					groups[k].min = 99999;
				}
				get_msecond(&maxminstart);
				if (setting->dump == 1 && !setting->pmt)
					fprintf(fptr, "HOT-KEY %d PRESSED: Reset MAXMIN value\n", hotkey_index);
				break;
			case 3:
				for (int k = 0; k < n; k++)
				{
					cavg[k] = 0;
					vavg[k] = 0;
					pavg[k] = 0;
					data_size[k] = 0;
					cmin[k] = 99999;
					vmin[k] = 99999;
					pmin[k] = 99999;
					cmax[k] = 0;
					vmax[k] = 0;
					pmax[k] = 0;
				}
				for (int k = 0; k < num_of_groups; k++)
				{
					groups[k].avg = 0;
					groups[k].avg_data_size = 0;
					groups[k].max = 0;
					groups[k].min = 99999;
				}
				get_msecond(&maxminstart);
				if (setting->dump == 1 && !setting->pmt)
					fprintf(fptr, "HOT-KEY %d PRESSED: Reset AVG and MAXMIN value\n", hotkey_index);
				avgstart = maxminstart;
				break;
			case 4:
				if (!setting->dump_statistics)
				{
					range_control++;
					if (range_control > 2)
						range_control = 0;
				}
				break;
			case 5:
				bootmodenum = lsbootmode(setting, LSBOOTMODE_SHOWID);

				printf("Enter   boot from BOOT SWITCH%s\n", g_vt_clear_line);
				printf("\nPlease select the boot mode after reset: ");
				if (!setting->nodisplay)
					input_num = catch_input_char_block() - '0';

				setting->boot_mode_hex = board->boot_modes[input_num].boot_mode_hex;
				if (board->boot_configs != NULL)
				{
					for (int bootcfg_n = 0; bootcfg_n < MAX_BOOT_CONFIG_BYTE; bootcfg_n++)
					{
						setting->boot_config_hex[bootcfg_n] = board->boot_configs[input_num].boot_config_hex[bootcfg_n];
					}
				}
				if (input_num >= bootmodenum || input_num < 0)
				{
					printf("Will boot from BOOT SWITCH, input=0x%x%s\n", input_num, g_vt_clear_line);
					setting->boot_mode_hex = -1;
					if (setting->dump == 1 && !setting->pmt)
						fprintf(fptr, "HOT-KEY %d PRESSED: Reset the board from BOOT SWITCH\n", hotkey_index);
				}
				else
				{
					if (setting->dump == 1 && !setting->pmt)
						fprintf(fptr, "HOT-KEY %d PRESSED: Reset the board from %s\n", hotkey_index, board->boot_modes[input_num].name);
				}

				reset(setting);
				ft4232h_i2c_remove_all(enable_exit_handler);
				strcpy(previous_path, "");

				msleep(300); //wait PMIC power on

				printf("reset Avg/Max/Min values%s\n", g_vt_clear_line);
				//reset AVG/MIN/MAX
				for (int k = 0; k < n; k++)
				{
					cavg[k] = 0;
					vavg[k] = 0;
					pavg[k] = 0;
					data_size[k] = 0;
					cmin[k] = 99999;
					vmin[k] = 99999;
					pmin[k] = 99999;
					cmax[k] = 0;
					vmax[k] = 0;
					pmax[k] = 0;
				}
				get_msecond(&maxminstart);
				avgstart = maxminstart;
				reset_flag = 1;

				break;
			case 6:
				printf("\nSimulate pressing the ON/OFF button once shortly%s\n", g_vt_clear_line);
				onoff(setting, 500, DONT_INIT);
				if (setting->dump == 1 && !setting->pmt)
					fprintf(fptr, "HOT-KEY %d PRESSED: Resume the board\n", hotkey_index);
				break;
			default:
				break;
			}
		}
		if (isalpha(ch) && setting->rangefixed == 0)
		{
			int sr_index;
			// ch = toupper(ch);
			ch = (int)ch - 64;
			if (ch < 27)
				sr_index = get_power_index_by_showid((int)ch, board);//is the ascii code for letter A
			else
			{
				sr_index = get_power_index_by_showid((int)ch - 32 + 26, board);//is the ascii code for letter a
			}
			if (sr_index < n && sr_index < MAX_NUMBER_OF_POWER && sr_index >= 0)
			{
				strcpy(sr_name, "SR_");
				if (board->mappings[name[sr_index]].name == NULL)
					return;
				strcat(sr_name, board->mappings[name[sr_index]].name);
GET_PATH3:
				if (get_path(sr_path, sr_name, board) != -1)
				{
					end_point = build_device_linkedlist_smart(&head, sr_path, head, previous_path);
					strcpy(previous_path, sr_path);
					if (end_point == NULL) {
						printf("monitor:failed to build device linkedlist\n");
						return;
					}
					struct gpio_device* gd = end_point;
					unsigned char data;
					get_gpiod(gd, &data);

					if (data == 0)
						set_gpiod(gd, 1);
					else
						set_gpiod(gd, 0);

					msleep(2);
					// sr_level[sr_index] = (!data == 0) ? 0 : 1;
					reset_flag = 1; //to force refresh sr_level, if some rails share SR_ pin
				}

				if (strncmp(sr_name, "SR_", 3) == 0)
				{
					strcpy(sr_name, "SRD_");
					strcat(sr_name, board->mappings[name[sr_index]].name);
					goto GET_PATH3;
				}
			}
		}
		// if (!isalpha(ch))
		// 	msleep(100);
		printf("%s", g_vt_clear_remain);
#endif
	}

	free_device_linkedlist_backward(end_point);
	if (setting->dump == 1)
	{
		if (GV_MONITOR_TERMINATED && setting->dump_statistics && !setting->pmt)
		{
			fprintf(fptr, "AVG%s", setting->use_rms ? "(RMS for current)" : "");
			for (int m = 0; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;
				if (board->mappings[k].initinfo != 0)
				{
					if (!setting->pmt)
						fprintf(fptr, ",%lf,%lf", vavg[k], cavg[k]);
					else
						fprintf(fptr, ",%lf,%lf,%lf", vavg[k], cavg[k], pavg[k]);
				}
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				fprintf(fptr, ",%lf", groups[k].avg);
			}
			fprintf(fptr, "\n");

			fprintf(fptr, "MAX");
			for (int m = 0; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;
				if (board->mappings[k].initinfo != 0)
				{
					if (!setting->pmt)
						fprintf(fptr, ",%lf,%lf", vmax[k], cmax[k]);
					else
						fprintf(fptr, ",%lf,%lf,%lf", vmax[k], cmax[k], pmax[k]);
				}
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				fprintf(fptr, ",%lf", groups[k].max);
			}
			fprintf(fptr, "\n");

			fprintf(fptr, "MIN");
			for (int m = 0; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;
				if (board->mappings[k].initinfo != 0)
				{
					if (!setting->pmt)
						fprintf(fptr, ",%lf,%lf", vmin[k], cmin[k]);
					else
						fprintf(fptr, ",%lf,%lf,%lf", vmin[k], cmin[k], pmin[k]);
				}
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				fprintf(fptr, ",%lf", groups[k].min);
			}
			fprintf(fptr, "\n");
		}
		fclose(fptr);
		free(setting->dumpname);
	}
	printf("%s", g_vt_clear);
	printf("%s", g_vt_home);
#endif
	return;
}

#if defined(linux) || defined(__APPLE__)
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#define PORT 65432

char *iso8601(char *local_time)
{
	time_t t_time;
	struct tm *t_tm;
 
	t_time = time(NULL);
	t_tm = localtime(&t_time);
	if (t_tm == NULL)
		return NULL;
 
	if (strftime(local_time, 32, "%FT%T%z", t_tm) == 0)
		return NULL;

	local_time[25] = local_time[24];
	local_time[24] = local_time[23];
	local_time[22] = ':';
	local_time[26] = '\0';
	return local_time;
}

static int server_monitor(struct options_setting* setting)
{
	signal(SIGINT, handle_sigint);
	struct board_info* board = get_board(setting->board);
	if (board == NULL) {
		// printf("entered board model are not supported.\n");
		return -1;
	}

	int ret = 0;

	char previous_path[MAX_PATH_LENGTH];
	void* head = NULL;
	void* end_point = NULL;

	double voltage = 0;
	double current = 0;
	double power = 0;
	char ch = ' ';
	char sr_path[MAX_PATH_LENGTH];

	char sr_name[100];
	unsigned long start, avgstart, maxminstart;
	get_msecond(&start);
	avgstart = start;
	maxminstart = start;
	unsigned long times = 1;
	FILE* fptr = NULL;
	int reset_flag = 0;

	int serverSocket;

	struct sockaddr_in server_addr;
	struct sockaddr_in clientAddr;
	int addr_len = sizeof(clientAddr);
	int client;
	char sendbuf[4096] = { 0 };
	char recvbuf[4096] = { 0 };
	int iDataNum;
	char logtime[32] = { 0 };
	int accepted;

#ifdef _WIN32
	WSADATA wsaData;
	unsigned long ul = 1;
	// Initialize Winsock
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		printf("WSAStartup failed with error: %d\n", ret);
		return 1;
	}
#endif

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
	if (serverSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
#else
	if (serverSocket < 0)
	{
		perror("socket");
#endif
		return -1;
	}

#ifdef _WIN32
	ret = ioctlsocket(serverSocket, FIONBIO, (unsigned long*)&ul);
	if (ret == SOCKET_ERROR)
	{
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return -10;
	}
#endif

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(serverSocket, (struct sockaddr*)&server_addr, sizeof(server_addr));
#ifdef _WIN32
	if (ret == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
#else
	if (ret < 0)
	{
		perror("connect");
#endif
		return -11;
	}

	ret = listen(serverSocket, 5);
#ifdef _WIN32
	if (ret == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
#else
	if (ret < 0)
	{
		perror("listen");
#endif
		return -12;
	}

retry:
	printf("[%s] listening Port %d on all IPs.\n", iso8601(logtime), PORT);
	accepted = 0;
	do {
		client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
#ifdef _WIN32
		if (client == INVALID_SOCKET)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(serverSocket);
				WSACleanup();
				return -10;
			}
#else
		if (client < 0)
		{
			perror("accept");
			return -10;
#endif
		}
		else
		{
			accepted = 1;
		}
	} while (!accepted);
	printf("[%s] IP is %s\n", iso8601(logtime), inet_ntoa(clientAddr.sin_addr));
	printf("[%s] Port is %d\n", iso8601(logtime), htons(clientAddr.sin_port));
	printf("[%s] Start collecting data...\n", iso8601(logtime));
	printf("[%s] Waiting request message...\n", iso8601(logtime));

	if (setting->dump == 1)
	{
		fptr = fopen(setting->dumpname, "w+");
		if (fptr == NULL)
		{
			printf("\nOpen file ERROR!\nPlease check if the \"%s\" file is still opened.\nExit...\n", setting->dumpname);
			return -1;
		}

		/*print first row*/
		int i = 1, index_n;
		fprintf(fptr, "time(ms)");
		index_n = get_power_index_by_showid(i, board);
		while (index_n != -1)
		{
			if (board->mappings[index_n].type == power && board->mappings[index_n].initinfo != 0)
			{
				if (!setting->pmt)
					fprintf(fptr, ",%s voltage(V),%s current(mA)", board->mappings[index_n].name, board->mappings[index_n].name);
				else
					fprintf(fptr, ",%s voltage(V),%s current(mA),%s power(mW)", board->mappings[index_n].name, board->mappings[index_n].name, board->mappings[index_n].name);
			}
			i++;
			index_n = get_power_index_by_showid(i, board);
		}
	}

	int n = 0; //n is the number of variables need to be measured
	int index = 0;
	while (board->mappings[index].name != NULL)
	{
		if (board->mappings[index].type == power)
			n++;
		index++;
	}
	//ideally I should be able to use n as the size of array, but Visual studio does not allow variable length array declaration...
	// so 100 maximum variable for now...I could use dynamic memory allocation instead, but then I will have to free it one by one in the end..
	int name[MAX_NUMBER_OF_POWER];
	double vnow[MAX_NUMBER_OF_POWER]; double vavg[MAX_NUMBER_OF_POWER]; double vmax[MAX_NUMBER_OF_POWER]; double vmin[MAX_NUMBER_OF_POWER];
	double cnow[MAX_NUMBER_OF_POWER]; double cavg[MAX_NUMBER_OF_POWER]; double cmax[MAX_NUMBER_OF_POWER]; double cmin[MAX_NUMBER_OF_POWER];
	double pnow[MAX_NUMBER_OF_POWER]; double pavg[MAX_NUMBER_OF_POWER]; double pmax[MAX_NUMBER_OF_POWER]; double pmin[MAX_NUMBER_OF_POWER];
	double data_size[MAX_NUMBER_OF_POWER];
	double pavg_server_data_size[MAX_NUMBER_OF_POWER];
	double cnow_fwrite[MAX_NUMBER_OF_POWER];
	double pnow_fwrite[MAX_NUMBER_OF_POWER];
	int sr_level[MAX_NUMBER_OF_POWER];
	int range_control = 0;
	int range_level[MAX_NUMBER_OF_POWER] = {0};
	double cur_range[MAX_NUMBER_OF_POWER];
	double unused_range[MAX_NUMBER_OF_POWER];
	double pavg_server[MAX_NUMBER_OF_POWER];


	//initialize
	for (int i = 0; i < MAX_NUMBER_OF_POWER; i++)
	{
		vavg[i] = 0; vmax[i] = 0; vmin[i] = 99999;
		cavg[i] = 0; cmax[i] = 0; cmin[i] = 99999;
		pavg[i] = 0; pmax[i] = 0; pmin[i] = 99999;
		pavg_server[i] = 0;
		data_size[i] = 0;
		pavg_server_data_size[i] = 0;
		name[i] = 0;
	}
	int previous_width = monitor_size(GET_COLUMN);

	//power groups
	struct group groups[MAX_NUMBER_OF_POWER];
	groups_init(groups, MAX_NUMBER_OF_POWER);

	int num_of_groups = parse_groups(groups, board);
	if (num_of_groups == -1)
	{
		free_device_linkedlist_backward(end_point);
		if (setting->dump == 1)
		{
			fclose(fptr);
		}
		return -1;
	}

	if (setting->dump == 1)
	{
		for (int i = 0; i < num_of_groups; i++)
		{
			fprintf(fptr, ",%s Power(mW)", board->power_groups[i].group_name);
		}
		fprintf(fptr, "\n");
	}

	//get first channels of all groups of rails
	int a = 0, pac_group_num = 0, last_pac_group = 0, pac_channel_num = 0;
	struct pac193x_reg_data pac_data[MAX_NUMBER_OF_POWER];
	char pac193x_group_path[MAX_NUMBER_OF_POWER][MAX_PATH_LENGTH];
	while (board->mappings[a].name != NULL)
	{
		if (board->mappings[a].type == power && board->mappings[a].initinfo != 0) //-------------------------------------------------------------------------
		{
			end_point = build_device_linkedlist_smart(&head, board->mappings[a].path, head, previous_path);
			strcpy(previous_path, board->mappings[a].path);

			if (end_point == NULL) {
				printf("monitor:failed to build device linkedlist\n");
				if (setting->dump == 1)
				{
					fclose(fptr);
				}
				return -1;
			}
			struct power_device* pd = end_point;

			int now_pac_group = pd->power_get_group(pd);
			if(last_pac_group != now_pac_group)
			{
				strcpy(pac193x_group_path[now_pac_group - 1], board->mappings[a].path);
				pac_group_num++;
				last_pac_group = now_pac_group;
			}

			pac_channel_num++;

			if (setting->rangefixed == 1)
			{
				strcpy(sr_name, "SR_");
				strcat(sr_name, board->mappings[a].name);
				if (get_path(sr_path, sr_name, board) != -1)
				{
					end_point = build_device_linkedlist_smart(&head, sr_path, head, previous_path);
					strcpy(previous_path, sr_path);

					if (end_point == NULL) {
						printf("monitor:failed to build device linkedlist\n");
						if (setting->dump == 1)
						{
							fclose(fptr);
						}
						return -1;
					}
					struct gpio_device* gd = end_point;
					if ((board->mappings[a].initinfo & 0x3) == 0)
					{
						sr_level[a] = 0;
						set_gpiod(gd, 0);
					}
					else if ((board->mappings[a].initinfo & 0x3) == 1)
					{
						sr_level[a] = 1;
						set_gpiod(gd, 1);
					}
				}
				else
				{
					sr_level[a] = -1;
				}
			}
			//get SR(sense resistor) switch logic level
			else
			{
				strcpy(sr_name, "SR_");
				strcat(sr_name, board->mappings[a].name);
				if (get_path(sr_path, sr_name, board) != -1)
				{
					end_point = build_device_linkedlist_smart(&head, sr_path, head, previous_path);
					strcpy(previous_path, sr_path);

					if (end_point == NULL) {
						printf("monitor:failed to build device linkedlist\n");
						if (setting->dump == 1)
						{
							fclose(fptr);
						}
						return -1;
					}
					struct gpio_device* gd = end_point;
					unsigned char data;
					get_gpiod(gd, &data);

					if (data == 0)
						sr_level[a] = 0;
					else
						sr_level[a] = 1;
				}
				else
				{
					sr_level[a] = -1;
				}
			}
		}
		a++;
	}

	int pac_group_count = pac_group_num;
	for(a = 0; a < MAX_NUMBER_OF_POWER; a++)
	{
		if (strlen(pac193x_group_path[a]) < 10)
		{
			continue;
		}
		if (pac_group_count <= 0)
			break;
		end_point = build_device_linkedlist_smart(&head, pac193x_group_path[a], head, previous_path);
		strcpy(previous_path, pac193x_group_path[a]);

		if (end_point == NULL) {
			printf("monitor:failed to build device linkedlist\n");
			if (setting->dump == 1)
			{
				fclose(fptr);
			}
			return -1;
		}
		struct power_device* pd = end_point;

		if (setting->use_bipolar)
			pd->power_write_bipolar(pd, 1);
		else
			pd->power_write_bipolar(pd, 0);

		//snapshot once to skip the first data that may not match the polarity configuration
		pd->power_set_snapshot(pd);

		pac_group_count--;
	}

#if 1
	unsigned long last_display = 0, now_display;
	while (!GV_MONITOR_TERMINATED)
	{
		get_msecond(&now_display);
		int candisplay = 0, interval;
		interval = now_display - last_display;
		if (interval > setting->refreshms || last_display == 0)
		{
			candisplay = 1;
			last_display = now_display;
		}
		else
		{
			candisplay = 0;
		}
		//first refresh all pac1934's
		pac_group_count = pac_group_num;
		for(a = 0; a < MAX_NUMBER_OF_POWER; a++)
		{
			if (strlen(pac193x_group_path[a]) < 10)
			{
				continue;
			}
			if (pac_group_count <= 0)
				break;
			end_point = build_device_linkedlist_smart(&head, pac193x_group_path[a], head, previous_path);
			strcpy(previous_path, pac193x_group_path[a]);

			if (end_point == NULL) {
				printf("monitor:failed to build device linkedlist\n");
				if (setting->dump == 1)
				{
					fclose(fptr);
				}
				return -1;
			}
			struct power_device* pd = end_point;

			pd->power_set_snapshot(pd);
			pac_group_count--;
		}
		if (pac_group_num < 4)
			msleep(1);
		int cannotacc = 0;
		pac_group_count = pac_group_num;
		for(a = 0; a < MAX_NUMBER_OF_POWER; a++)
		{
			if (pac_group_count <= 0)
				break;

			if (strlen(pac193x_group_path[a]) < 10)
			{
				continue;
			}
			end_point = build_device_linkedlist_smart(&head, pac193x_group_path[a], head, previous_path);
			strcpy(previous_path, pac193x_group_path[a]);

			if (end_point == NULL) {
				printf("monitor:failed to build device linkedlist\n");
				if (setting->dump == 1)
				{
					fclose(fptr);
				}
				return -1;
			}
			struct power_device* pd = end_point;

			if (setting->use_hwfilter)
				pd->power_set_hwfilter(pd, 1);
			else
				pd->power_set_hwfilter(pd, 0);

			if (setting->use_bipolar)
				pd->power_set_bipolar(pd, 1);
			else
				pd->power_set_bipolar(pd, 0);

			int b;
			for (b = 0; b < 50; b++)
			{
				if (pd->power_get_data(pd, &pac_data[a]) >= 0)
				{
					break;
				}
				else
					cannotacc = 1;
			}
			if (b == 50)
			{
				printf("PAC1934 cannot access!\n");
				return -1;
			}
			else if(cannotacc == 1)
				break;
			pac_group_count--;
		}
		if(cannotacc == 1)
			continue;
		//calculate the value and store them in array
		int i = 0;//i is the index of all mappings
		int j = 0;//j is the index of the power related mapping only
		while (board->mappings[i].name != NULL)
		{
			if (board->mappings[i].type == power && board->mappings[i].initinfo != 0)
			{
				name[j] = i;

				if (reset_flag == 1)
				{
					strcpy(sr_name, "SR_");
					strcat(sr_name, board->mappings[name[j]].name);
					if (get_path(sr_path, sr_name, board) != -1)
					{
						end_point = build_device_linkedlist_smart(&head, sr_path, head, previous_path);
						strcpy(previous_path, sr_path);

						if (end_point == NULL) {
							printf("monitor:failed to build device linkedlist\n");
							if (setting->dump == 1)
							{
								fclose(fptr);
							}
							return -1;
						}
						struct gpio_device* gd = end_point;
						unsigned char data;
						get_gpiod(gd, &data);

						if (data == 0)
							sr_level[j] = 0;
						else
							sr_level[j] = 1;
					}
					else
					{
						sr_level[j] = -1;
					}
				}

				end_point = build_device_linkedlist_smart(&head, board->mappings[i].path, head, previous_path);
				strcpy(previous_path, board->mappings[i].path);

				if (end_point == NULL) {
					printf("monitor:failed to build device linkedlist\n");
					if (setting->dump == 1)
					{
						fclose(fptr);
					}
					return -1;
				}
				struct power_device* pd = end_point;
				
				if(sr_level[j] == 0)
					pd->switch_sensor(pd, 1);
				else
					pd->switch_sensor(pd, 0);

				int group = pd->power_get_group(pd) - 1;
				int sensor = pd->power_get_sensor(pd) - 1;
				voltage = pac_data[group].vbus[sensor] - (pac_data[group].vsense[sensor] / 1000000);
				current = pac_data[group].vsense[sensor] / pd->power_get_cur_res(pd);
				cnow_fwrite[j] = current;
				pnow_fwrite[j] = current * voltage;

				if (range_control == 0)
				{
					range_level[j] = (char)(0 | range_level[j] << 4);  //mA
				}
				else if (range_control == 2)
				{
					current *= 1000;
					range_level[j] = (char)(1 | range_level[j] << 4);  //uA
				}
				else
				{
					if ( (!(range_level[j] & 0xf) && cavg[j] < 1) || ((range_level[j] & 0xf) && cavg[j] < 1000))
					{
						current *= 1000;
						range_level[j] = (char)(1 | range_level[j] << 4);  //uA
					}
					else
						range_level[j] = (char)(0 | range_level[j] << 4);  //mA
				}

				cur_range[j] = 100000.0 / pd->power_get_cur_res(pd);
				unused_range[j] = 100000.0 / pd->power_get_unused_res(pd);
				// cur_range[j] = pac_data[pd->power_get_group(pd) - 1].vsense[pd->power_get_sensor(pd) - 1];

				// printf("current %f\n", current);
				double power = current * voltage;
				vnow[j] = voltage;
				cnow[j] = current;
				pnow[j] = power;
				pavg_server[j] = (pavg_server_data_size[j] * pavg_server[j] + power) / ((double)(pavg_server_data_size[j] + 1));
				if (range_level[j] == 0x0 || range_level[j] == 0x11)
				{
					vmin[j] = (voltage < vmin[j]) ? voltage : vmin[j];
					cmin[j] = (current < cmin[j]) ? current : cmin[j];
					pmin[j] = (power < pmin[j]) ? power : pmin[j];
					vmax[j] = (voltage > vmax[j]) ? voltage : vmax[j];
					cmax[j] = (current > cmax[j]) ? current : cmax[j];
					pmax[j] = (power > pmax[j]) ? power : pmax[j];
					vavg[j] = (data_size[j] * vavg[j] + voltage) / (double)(data_size[j] + 1);
					pavg[j] = (data_size[j] * pavg[j] + power) / ((double)(data_size[j] + 1));
				}
				else if (range_level[j] == 0x01)
				{
					vmin[j] = (voltage < vmin[j]) ? voltage : vmin[j];
					cmin[j] = (current < cmin[j] * 1000) ? current : cmin[j] * 1000;
					pmin[j] = (power < pmin[j] * 1000) ? power : pmin[j] * 1000;
					vmax[j] = (voltage > vmax[j]) ? voltage : vmax[j];
					cmax[j] = (current > cmax[j] * 1000) ? current : cmax[j] * 1000;
					pmax[j] = (power > pmax[j] * 1000) ? power : pmax[j] * 1000;
					cavg[j] *= 1000;
					vavg[j] = (data_size[j] * vavg[j] + voltage) / (double)(data_size[j] + 1);
					pavg[j] = (data_size[j] * pavg[j] * 1000 + power) / ((double)(data_size[j] + 1));
				} 
				else if (range_level[j] == 0x10)
				{
					vmin[j] = (voltage < vmin[j]) ? voltage : vmin[j];
					cmin[j] = (current < cmin[j] / 1000) ? current : cmin[j] / 1000;
					pmin[j] = (power < pmin[j] / 1000) ? power : pmin[j] / 1000;
					vmax[j] = (voltage > vmax[j]) ? voltage : vmax[j];
					cmax[j] = (current > cmax[j] / 1000) ? current : cmax[j] / 1000;
					pmax[j] = (power > pmax[j] / 1000) ? power : pmax[j] / 1000;
					cavg[j] /= 1000;
					vavg[j] = (data_size[j] * vavg[j] + voltage) / (double)(data_size[j] + 1);
					pavg[j] = (data_size[j] * pavg[j] / 1000 + power) / ((double)(data_size[j] + 1));
				}

				if (setting->use_rms)
				{
					cavg[j] = sqrt((data_size[j] * cavg[j] * cavg[j] + current * current) / (double)(data_size[j] + 1));
					pavg[j] = cavg[j] * vavg[j];
				}
				else
					cavg[j] = (data_size[j] * cavg[j] + current) / (double)(data_size[j] + 1);

				data_size[j] = data_size[j] + 1;
				pavg_server_data_size[j] = pavg_server_data_size[j] + 1;

				j++;
			}
			else
				j++;
			i++;
		}

		if (reset_flag == 1)
			reset_flag = 0;

#if 1
		//get groups data
		for (int k = 0; k < num_of_groups; k++)
		{
			groups[k].sum = 0;
			for (int x = 0; x < groups[k].num_of_members; x++)
			{
				int mi = groups[k].member_index[x];
				if (range_level[mi] == 0x01 || range_level[mi] == 0x11)
					pnow[mi] /= 1000;
				groups[k].sum += pnow[mi];
			}
			groups[k].max = (groups[k].sum > groups[k].max) ? groups[k].sum : groups[k].max;
			groups[k].min = (groups[k].sum < groups[k].min) ? groups[k].sum : groups[k].min;
			groups[k].avg = (groups[k].avg_data_size * groups[k].avg + groups[k].sum) / (groups[k].avg_data_size + 1);
			groups[k].avg_data_size++;
		}
		//dump data to file
		if (setting->dump == 1)
		{
			unsigned long now;
			get_msecond(&now);
			fprintf(fptr, "%ld", (long)now - start);//add time before the first element
			for (int m = 0; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;
				if (board->mappings[k].initinfo != 0)
				{
					if (!setting->pmt)
						fprintf(fptr, ",%lf,%lf", vnow[k], cnow_fwrite[k]);
					else
						fprintf(fptr, ",%lf,%lf,%lf", vnow[k], cnow_fwrite[k], pnow_fwrite[k]);
				}
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				fprintf(fptr, ",%lf", groups[k].sum);
			}
			fprintf(fptr, "\n");
		}

#ifdef _WIN32
		iDataNum = recv(client, recvbuf, 1024, 0);
#else
		iDataNum = recv(client, recvbuf, 1024, MSG_DONTWAIT);
#endif
		if (iDataNum > 0)
		{
			// printf("Received data: [%s]\n", recvbuf);
			if (strcmp(recvbuf, "data request") == 0)
			{
				printf("[%s] Data Request.\n", iso8601(logtime));
				strcpy(sendbuf, "");

				char time[32] = {0};
				strcat(sendbuf, iso8601(time));
				strcat(sendbuf, ";");

				for (int m = 0; m < n + 1; m++)
				{
					int k = get_power_index_by_showid(m, board);
					if (k < 0)
						continue;
					if (board->mappings[k].initinfo != 0)
					{
						strcat(sendbuf, board->mappings[k].name);
						strcat(sendbuf, ":");
						char temp[20] = {0};
						sprintf(temp, "%lf", pavg_server[k]);
						strcat(sendbuf, temp);
						strcat(sendbuf, ";");
					}
				}
				
				send(client, sendbuf, strlen(sendbuf), 0);

				for (int i = 0; i < MAX_NUMBER_OF_POWER; i++)
				{
					pavg_server[i] = 0;
					pavg_server_data_size[i] = 0;
				}
			}
			else if (strcmp(recvbuf, "quit") == 0)
			{
				printf("[%s] Client requests to exit server monitor...\n", iso8601(logtime));
				ret = 1;
				break;
			} else {
				send(client, "Unsupported request!", strlen("unsupported request!"), 0);
			}
		} else if (iDataNum == 0){
			printf("[%s] Client disconnects! Stop collecting data...\n", iso8601(logtime));
			ret = 2;
			goto retry;
		}

		memset(recvbuf, 0, sizeof recvbuf);

		times++;
#endif
	}

#ifdef _WIN32
	WSACleanup();
#endif

	free_device_linkedlist_backward(end_point);
	if (setting->dump == 1)
	{
		if (GV_MONITOR_TERMINATED && setting->dump_statistics && !setting->pmt)
		{
			fprintf(fptr, "AVG%s", setting->use_rms ? "(RMS for current)" : "");
			for (int m = 0; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;
				if (board->mappings[k].initinfo != 0)
				{
					if (!setting->pmt)
						fprintf(fptr, ",%lf,%lf", vavg[k], cavg[k]);
					else
						fprintf(fptr, ",%lf,%lf,%lf", vavg[k], cavg[k], pavg[k]);
				}
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				fprintf(fptr, ",%lf", groups[k].avg);
			}
			fprintf(fptr, "\n");

			fprintf(fptr, "MAX");
			for (int m = 0; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;
				if (board->mappings[k].initinfo != 0)
				{
					if (!setting->pmt)
						fprintf(fptr, ",%lf,%lf", vmax[k], cmax[k]);
					else
						fprintf(fptr, ",%lf,%lf,%lf", vmax[k], cmax[k], pmax[k]);
				}
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				fprintf(fptr, ",%lf", groups[k].max);
			}
			fprintf(fptr, "\n");

			fprintf(fptr, "MIN");
			for (int m = 0; m < n + 1; m++)
			{
				int k = get_power_index_by_showid(m, board);
				if (k < 0)
					continue;
				if (board->mappings[k].initinfo != 0)
				{
					if (!setting->pmt)
						fprintf(fptr, ",%lf,%lf", vmin[k], cmin[k]);
					else
						fprintf(fptr, ",%lf,%lf,%lf", vmin[k], cmin[k], pmin[k]);
				}
			}
			for (int k = 0; k < num_of_groups; k++)
			{
				fprintf(fptr, ",%lf", groups[k].min);
			}
			fprintf(fptr, "\n");
		}
		fclose(fptr);
		free(setting->dumpname);
	}
#endif
	return 0;
}

int check_board_eeprom(struct board_info* board, int retmode)
{
	void* head = NULL;
	void* end_point;
	int status, j;

	j = 0;
	while(board->mappings[j].name!=NULL)
	{
		if(board->mappings[j].type == bcu_eeprom || board->mappings[j].type == ftdi_eeprom)
		{
			end_point = build_device_linkedlist_forward(&head, board->mappings[j].path);
			if (end_point == NULL)
			{
				printf("find_board_by_eeprom: error building device linked list\n");
				return -2;
			}

			struct eeprom_device* eeprom = end_point;
			status = bcu_eeprom_checkboard(eeprom, board->eeprom_data);
			if (board->mappings[j].type == bcu_eeprom)
				free_device_linkedlist_backward(end_point);
			ft4232h_i2c_remove_all(enable_exit_handler);

			if (status == 0)
			{
				return 0;
			}
			else
			{
				j++;
				continue;
			}
		}
		j++;
	}

	if (retmode)
		return status;
	return -1;
}

int find_board_by_eeprom(struct options_setting* setting)
{
	int status, i;

	for (i = 0; i < num_of_boards; i++)
	{
		strcpy(setting->board, board_list[i].name);
		// struct board_info* board=get_board(setting->board);
		struct board_info* board = get_board_by_id(i);
		status = check_board_eeprom(board, 0);
		if (status != -1)
			return status;
	}

	return -1;
}

static int lsftdi(struct options_setting* setting)
{
	// int temp = 0;
	// ft_list_devices(NULL, &temp, LIST_DEVICE_MODE_PRINT);


	int board_num = 0;
	char location_id_str[MAX_NUMBER_OF_USB_DEVICES][MAX_LOCATION_ID_LENGTH];
	if (!setting->auto_find_board)
		ft_list_devices(location_id_str, &board_num, LIST_DEVICE_MODE_PRINT);
	else
	{	ft_list_devices(location_id_str, &board_num, LIST_DEVICE_MODE_OUTPUT);

		if (board_num > 1)
		{
			printf("There are %d boards on this host. Please add [-board=] and [-id=] option.\n", board_num);
			for (int j = 0; j < board_num; j++)
			{
				strcpy(GV_LOCATION_ID, location_id_str[j]);
				switch (find_board_by_eeprom(setting))
				{
				case 0:
					printf("Auto recognized the board %s is on location_id=%s\n", setting->board, GV_LOCATION_ID);
					break;
				case -1:
				{
					printf("Can't auto recognize the board on location_id=%s...\n", GV_LOCATION_ID);
					// strcpy(setting.board, "");
				}break;
				case -2:
				{
					printf("Can't open FTDI channel on location_id=%s...\n", GV_LOCATION_ID);
					return -2;
				}break;
				
				default:
					break;
				}
			}

			return -1;
		}
		else
		{
			switch (find_board_by_eeprom(setting))
			{
			case 0:
				printf("Auto recognized the board: %s\n", setting->board);
				break;
			case -1:
			{
				printf("Can't auto recognize the board...Please try to add [-board=] option.\n");
				return -2;
				// printf("For now, only 8MPLUSLPD4-CPU don't have eeprom. Assuming use \"imx8mpevk\"...\n");
				// printf("Please also notice if there is any other board connected to this host.\n");
				// printf("Try \"bcu lsftdi\" to find the right -id=...\n");
				// strcpy(setting.board, "imx8mpevk");
			}break;
			case -2:
			{
				printf("Can't open FTDI channel...Please try to add [-board=] option.\n");
				return -2;
			}break;
			
			default:
				break;
			}
		}
	}

	return 0;
}

static int enable_vt_mode()
{
#ifdef _WIN32
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		clean_vt_color();
		return -1;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		clean_vt_color();
		return -1;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		clean_vt_color();
		return -1;
	}
	return 0;
#else
	return 0;
#endif
}

void terminateBCU(void)
{
	ft4232h_i2c_remove_all(enable_exit_handler);
}

int main(int argc, char** argv)
{
	print_version();

	if (enable_vt_mode())
	{
		printf("Your console don't support VT mode, fall back to verbose mode");
	}

	char yamfile[128] = {0};
	get_yaml_file_path(yamfile);

	switch (argc)
	{
	case 1:
		print_help(NULL);
		return 0;
	case 2: 
		if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0)
		{
			print_help(NULL);
			return 0;
		}
		if (strcmp(argv[1], "conf_path") == 0 || strcmp(argv[1], "-cp") == 0)
		{
			printf("\nBCU Config file path: %s\n\n", yamfile);
			return 0;
		}
		break;
	default:
		break;
	}

	atexit(terminateBCU);
#ifdef _WIN32
	SetThreadPriority(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	char* cmd = argv[1];
	struct options_setting setting;
	memset(&setting, 0, sizeof(struct options_setting));//initialized to zero
	set_options_default(&setting);

	int board_num = 0;
	char location_id_str[MAX_NUMBER_OF_USB_DEVICES][MAX_LOCATION_ID_LENGTH];
	int find_id = -1;
	char origin_board_name[100];
	unsigned int find_board_count = 0;

	switch (parse_board_id_options(argc, argv, &setting))
	{
	case NO_BOARD_AND_ID:
		ft_list_devices(location_id_str, &board_num, LIST_DEVICE_MODE_OUTPUT);

		if (board_num > 1)
		{
			printf("There are %d boards on this host. Please add [-board=] or [-id=] option.\n", board_num);
			for (int j = 0; j < board_num; j++)
			{
				strcpy(GV_LOCATION_ID, location_id_str[j]);
				switch (find_board_by_eeprom(&setting))
				{
				case 0:
					printf("Auto recognized the board %s is on location_id=%s\n", setting.board, GV_LOCATION_ID);
					break;
				case -1:
				{
					printf("Can't auto recognize the board on location_id=%s...\n", GV_LOCATION_ID);
					// strcpy(setting.board, "");
				}break;
				case -2:
				{
					printf("Can't open FTDI channel on location_id=%s...\n", GV_LOCATION_ID);
					return -2;
				}break;
				
				default:
					break;
				}
			}

			return -1;
		}
		else
		{
			switch (find_board_by_eeprom(&setting))
			{
			case 0:
				printf("Auto recognized the board: %s\n", setting.board);
				break;
			case -1:
			{
				printf("Can't auto recognize the board...Please try to add [-board=] option.\n");
				return -2;
				// printf("For now, only 8MPLUSLPD4-CPU don't have eeprom. Assuming use \"imx8mpevk\"...\n");
				// printf("Please also notice if there is any other board connected to this host.\n");
				// printf("Try \"bcu lsftdi\" to find the right -id=...\n");
				// strcpy(setting.board, "imx8mpevk");
			}break;
			case -2:
			{
				printf("Can't open FTDI channel...Please try to add [-board=] option.\n");
				return -2;
			}break;
			
			default:
				break;
			}
		}
		break;
	case NO_BOARD:
		switch (find_board_by_eeprom(&setting))
		{
		case 0:
			printf("Auto recognized the board %s is on location_id=%s\n", setting.board, GV_LOCATION_ID);
			break;
		case -1:
		{
			printf("Can't auto recognize the board on location_id=%s...\n", GV_LOCATION_ID);
			// strcpy(setting.board, "");
		}break;
		case -2:
		{
			printf("Can't open FTDI channel on location_id=%s...\n", GV_LOCATION_ID);
			return -2;
		}break;
		
		default:
			break;
		}
		break;
	case NO_ID:
		strcpy(origin_board_name, setting.board);

		ft_list_devices(location_id_str, &board_num, LIST_DEVICE_MODE_OUTPUT);

		if (board_num > 1 && strcmp(origin_board_name, "imx8mpevk"))
		{
			for (int j = 0; j < board_num; j++)
			{
				strcpy(GV_LOCATION_ID, location_id_str[j]);
				switch (find_board_by_eeprom(&setting))
				{
				case 0:
					if (strcmp(origin_board_name, setting.board) == 0)
					{
						printf("Auto recognized the board %s is on location_id=%s\n", setting.board, GV_LOCATION_ID);
						find_board_count++;
						find_id = j;
					}
					// printf("Auto recognized the board: %s\n", setting.board);
					break;
				case -1:
				{
					printf("Can't auto recognize the board on location_id=%s...\n", GV_LOCATION_ID);
					// strcpy(setting.board, "");
				}break;
				case -2:
				{
					printf("Can't open FTDI channel...Please try to add [-board=] option.\n");
					return -2;
				}break;
				
				default:
					break;
				}
			}
			if (find_board_count > 1)
			{
				printf("There are %d boards named %s. Please add [-id=] option.\n", find_board_count, origin_board_name);
				return -1;
			}
			else
			{
				if (find_id >= 0)
					strcpy(GV_LOCATION_ID, location_id_str[find_id]);
			}
			strcpy(setting.board, origin_board_name);
		}
		break;
	case NO_USE_AUTO_FIND:
	default:
		break;
	}

	if (parse_options(cmd, argc, argv, &setting) == -1) {
		return 0;
	}

	if (strcmp(setting.board, "imx8dxlevk") == 0 ||
	    strcmp(setting.board, "imx8dxlevkc1") == 0 ||
	    strcmp(setting.board, "imx8dxl_ddr3_evk") == 0)
		enable_exit_handler = 1;

	if (setting.restore == 1)
		enable_exit_handler = 1;

	//check board eeprom
	if (strcmp(setting.board, "imx8mpevk") &&
	    strcmp(setting.board, "imx8dxl_ddr3_evk") &&
	    strcmp(setting.board, "imx8mpddr3l") &&
	    strcmp(setting.board, "imx8mpddr4") &&
	    strcmp(cmd, "eeprom") &&
	    strcmp(cmd, "lsbootmode") &&
	    strcmp(cmd, "lsgpio") &&
	    strcmp(cmd, "upgrade") &&
	    strcmp(cmd, "uuu") &&
	    strcmp(cmd, "lsftdi") &&
	    strcmp(cmd, "lsboard") &&
	    strcmp(cmd, "version") &&
	    strcmp(cmd, "help"))
	{
		struct board_info* board=get_board(setting.board);
		int val = check_board_eeprom(board, 1);
		if (val == -11)
		{
			printf("%sboard rev in EEPROM and provided <-board=> MISMATCH!%s\n", g_vt_red, g_vt_default);
			printf("%sIf the board has been reworked to a new revision, please use the below command to update the value in EEPROM:%s\n", g_vt_yellow, g_vt_default);
			printf("\n%s# ./bcu eeprom -w -board=[new BOARDNAME showed in cmd lsboard]%s\n", g_vt_red, g_vt_default);
			return -1;
		}
		if (val < 0 && val != -10)
		{
			printf("%sThis board support EEPROM but it is EMPTY.\nPlease use below command to program the EEPROM.%s\n", g_vt_yellow, g_vt_default);
			printf("\n%s# ./bcu eeprom -w -board=%s%s\n", g_vt_red, setting.board, g_vt_default);
			printf("%s\nThen you can choose to replace option <-board=> with <-auto>.%s\n", g_vt_yellow, g_vt_default);
			printf("%sNOTE:%s If other boards are also connected to the same host, <-auto> may break its ttyUSB function temporarily.%s\n", g_vt_red, g_vt_yellow, g_vt_default);
		}
	}

	switch (readConf(setting.board, &setting))
	{
	case 0:
		break;
	case -1:
		printf("Trying to create new config file to %s ...\n", yamfile);
		writeConf();
		if (readConf(setting.board, &setting) < 0)
			return -1;
		break;
	case -2:
		printf("config file read error, please check or delete config file: %s and try again.\n", yamfile);
		return -2;
		break;
	case -3:
		return -3;
		break;
	default:
		break;
	}

	if (strcmp(cmd, "lsgpio") == 0)
	{
		lsgpio(&setting);
	}
	else if (strcmp(cmd, "set_gpio") == 0)
	{
		set_gpio(&setting);
	}
	else if (strcmp(cmd, "get_level") == 0)
	{
		get_gpio_level(&setting);
	}
	else if (strcmp(cmd, "set_boot_mode") == 0)
	{
		set_boot_mode(&setting);
	}
	else if (strcmp(cmd, "get_boot_mode") == 0)
	{
		get_boot_mode(&setting);
	}
	else if (strcmp(cmd, "help") == 0)
	{
		if (argc == 2)
			print_help(NULL);
		if (argc == 3)
			print_help(argv[2]);
	}
	else if (strcmp(cmd, "eeprom") == 0)
	{
		eeprom(&setting);
	}
	else if (strcmp(cmd, "monitor") == 0)
	{
		monitor(&setting);
	}
	else if (strcmp(cmd, "server") == 0)
	{
		server_monitor(&setting);
	}
	else if (strcmp(cmd, "temp") == 0)
	{
		get_temp(&setting);
	}
	/*
	due to the unavoidable large chunk size of monitor() function,
	it is easier to debug monitor function with a simplified version
	which is what monitor_test is for
	*/
	/*
	else if(strcmp(cmd, "monitor_test")==0)
	{
		monitor_test(&setting);
	}
	*/
	else if (strcmp(cmd, "lsftdi") == 0)
	{
		lsftdi(&setting);
	}
	else if (strcmp(cmd, "lsboard") == 0)
	{
		lsboard(&setting);
	}
	else if (strcmp(cmd, "lsbootmode") == 0)
	{
		lsbootmode(&setting, LSBOOTMODE_NSHOWID);
	}
	else if (strcmp(cmd, "reset") == 0)
	{
		reset(&setting);
	}
	else if (strcmp(cmd, "onoff") == 0)
	{
		onoff(&setting, setting.hold, INIT_NOW);
	}
	else if (strcmp(cmd, "init") == 0)
	{
		initialize(&setting, DONT_RESET);
	}
	else if (strcmp(cmd, "deinit") == 0)
	{
		deinitialize(&setting);
	}
	else if (strcmp(cmd, "uuu") == 0)
	{
		uuu(&setting);
	}
	else if (strcmp(cmd, "version") == 0)
	{
		print_version();
	}
	else if (strcmp(cmd, "upgrade") == 0)
	{
		upgrade_bcu(&setting);
	}
	else
	{
		printf("%s is a invalid command\n\n", cmd);
		print_help(NULL);
	}

	printf("done\n");
	return 0;
}

