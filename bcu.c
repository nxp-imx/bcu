/*
* Copyright 2019 NXP.
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
#endif

#ifdef linux
#include <unistd.h>
#include <sys/ioctl.h>
#include "ftdi.h"
#endif

//common library for both OS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

#include "port.h"
#include "parser.h"
#include "chip.h"
#include "board.h"
#include "version.h"

#define DONT_RESET	0
#define RESET_NOW	1

extern int num_of_boards;
extern struct board_info board_list[];

int GV_MONITOR_TERMINATED = 0;

char* g_vt_red = (char*)"\x1B[91m";
char* g_vt_green = (char*)"\x1B[92m";
char* g_vt_yellow = (char*)"\x1B[93m";
char* g_vt_kcyn = (char*)"\x1B[36m";
char* g_vt_white = (char*)"\x1B[97m";
char* g_vt_magenta = (char*)"\x1B[35m";
char* g_vt_blue = (char*)"\x1B[34m";
char* g_vt_default = (char*)"\x1B[0m";
char* g_vt_clear = (char*)"\x1B[2J";
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
	g_vt_default = g_vt_red;
	g_vt_clear = (char*)"\n";
	g_vt_home = (char*)"\n";
}

static void print_version()
{
	printf("version %s\n", GIT_VERSION);
}

static void print_help(char* cmd)
{
	if (cmd == NULL) {
		printf("%s\n", "Usage:");
		printf("%s\n\n", "bcu command [-options]");
		printf("%s\n", "list of available commands:");
		printf("	%s%-50s%s%s\n", g_vt_default, "reset  [BOOTMODE_NAME] [-board=] [-id=]", g_vt_green, "reset the board (optional [BOOTMODE_NAME])");
		printf("	%s%-50s%s%s\n", g_vt_default, "init   [BOOTMODE_NAME] [-board=] [-id=]", g_vt_green, "enable the remote control with a boot mode");
		printf("	%s%-50s%s%s\n", g_vt_default, "deinit [BOOTMODE_NAME] [-board=] [-id=]", g_vt_green, "disable the remote control");
		printf("\n");
		printf("	%s%-50s%s%s\n", g_vt_default, "monitor", g_vt_green, "monitor power consumption");
		printf("\n");
		printf("	%s%-50s%s%s\n", g_vt_default, "set_gpio [GPIO_NAME] [1/0] [-board=] [-id=]", g_vt_green, "set pin GPIO_NAME to be high(1) or low(0)");
		printf("	%s%-50s%s%s\n", g_vt_default, "set_boot_mode [BOOTMODE_NAME] [-board=] [-id=]", g_vt_green, "set BOOTMODE_NAME as boot mode");
		printf("\n");
		printf("	%s%-50s%s%s\n", g_vt_default, "lsftdi", g_vt_green, "list all boards connected by ftdi device");
		printf("	%s%-50s%s%s\n", g_vt_default, "lsboard", g_vt_green, "list all supported board models");
		printf("	%s%-50s%s%s\n", g_vt_default, "lsbootmode [-board=]", g_vt_green, "show a list of available boot mode of a board");
		printf("	%s%-50s%s%s\n", g_vt_default, "lsgpio     [-board=]", g_vt_green, "show a list of available gpio pin of a board");
		printf("\n");
		printf("	%s%-50s%s%s%s\n", g_vt_default, "version", g_vt_green, "print version number", g_vt_default);
		// printf("%s%-50s%s%s\n", g_vt_default, "help [COMMAND_NAME]", g_vt_green, "/*show details and options of COMMAND_NAME*/");

#ifdef __linux__
		printf("\n	%s%-50s%s%s\n", g_vt_default, "flash [emmc or sd] [-board=]", g_vt_green, "flash flash.bin to EMMC or SD");
		printf("%s", g_vt_kcyn);
		printf("\n***please remember to run bcu with sudo%s\n\n\n", g_vt_default);
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

static void lsbootmode(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	int i = 0;
	printf("\navailable boot mode:\n\n");
	while (board->boot_modes[i].name != NULL)
	{
		printf("	%s\n", board->boot_modes[i].name);
		i++;
	}
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

void free_gpio(struct gpio_device* gpio)
{
	void* end_point = (void*)gpio;
	free_device_linkedlist_backward(end_point);
	gpio = NULL;
	return;
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
}

static void deinitialize(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;

	gpio = get_gpio("remote_en", board);
	if (gpio == NULL)
	{
		printf("set_boot_mode: No boot_mode configuration!\n");
		return;
	}

	status = gpio->gpio_write(gpio, 0x00); //low
	if (!status)
		printf("%sDISABLE%s remote control\n", g_vt_red, g_vt_default);
	free_gpio(gpio);
}

static void initialize(struct options_setting* setting, int isreset)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
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
			if (isreset)
				printf("rebooting...\n");
			else
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
				else
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
						return;
					}
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
			printf("set %s %s failed, error = 0x%x\n", name, output ? "high" : "low", status);
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
}

static void reset(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	struct gpio_device* gpio = NULL;
	int status = -1;

	initialize(setting, RESET_NOW);

	gpio = get_gpio("reset", board);
	if (gpio == NULL)
	{
		printf("reset: error building device linked list\n");
		return;
	}

	//delay
	msleep(setting->delay);

	status = gpio->gpio_write(gpio, 0x00); //reset low
	msleep(500);
	if (setting->boot_mode_hex != -1)
	{
		status |= gpio->gpio_write(gpio, 0xFF) << 1;//reset high
	}
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
			status |= gpio->gpio_write(gpio, 0x00) << 2; //bootmode_sel low
			free_gpio(gpio);

			msleep(10);

			gpio = get_gpio("reset", board);
			if (gpio == NULL)
			{
				printf("reset: error building device linked list\n");
				return;
			}
			status |= gpio->gpio_write(gpio, 0xFF) << 3; //reset high
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
			if (gpio == NULL)
			{
				printf("reset: error building device linked list\n");
				return;
			}
			status |= gpio->gpio_write(gpio, 0x00) << 2; //remote_en low
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

#ifdef __linux__
#include <stdlib.h>
#include <curses.h>
static void uuu(struct options_setting* setting)
{
	struct board_info* board = get_board(setting->board);
	if (board == NULL)
		return;
	if (strcmp(board->links->flashbin, "") == 0)
	{
		printf("this board still does not support this function\n");
		return;
	}
	if (setting->boot_mode_hex == -1)
	{
		printf("please provide the flashing destination: emmc or sd\n");
		return;
	}
	struct gpio_device* gpio = NULL;
	int status = -1, i;
	char buf[256], cmd[1024] = "sudo ./uuu -b ";
	char *old_boot_mode_str;
	char ch, cmdurl[1024] = "\0";

	i = 0;
	while (board->boot_modes[i].name != NULL)
	{
		if (setting->boot_mode_hex == board->boot_modes[i].boot_mode_hex)
		{
			old_boot_mode_str = board->boot_modes[i].name;
			if (!strcmp(old_boot_mode_str, "sd") || !strcmp(old_boot_mode_str, "emmc"))
				break;
			else
			{
				printf("only support emmc or sd now\n");
				return;
			}
		}
		i++;
	}

	//get the path
	getcwd(buf,sizeof(buf));
	//find uuu
	if (access(strcat(buf, "/uuu"), 0))
	{
		system("wget https://github.com/NXPmicro/mfgtools/releases/download/uuu_1.3.126/uuu");
		system("chmod a+x uuu");
	}

	//reset to serial download
	i = 0;
	while (board->boot_modes[i].name != NULL)
	{
		if (strcmp(board->boot_modes[i].name, "usb") == 0)
		{
			setting->boot_mode_hex = board->boot_modes[i].boot_mode_hex;
			break;
		}
		i++;
	}
	reset(setting);
	printf("flash.bin is from %s\nburn this flash.bin to %s?(Y/n):", board->links->flashbin, old_boot_mode_str);
	scanf("%c", &ch);
	if (ch == 'n' || ch == 'N')
	{
		scanf("%c", &ch);
		ch = 0;
		i = 0;
		printf("please enter the new flash.bin source:");
		do
		{
			ch = getchar();
			if (ch != '\n')
				cmdurl[i] = ch;
			i++;
		}while(ch != '\n' && i < 1024);
	}
	else
		strcpy(cmdurl, board->links->flashbin);

	// printf("cmdurl=%s\n", cmdurl);
	if(cmdurl[0] == '\n' || cmdurl[0] == '\0')
		return;

	msleep(100);

	//flash by uuu to emmc/sd
	strcat(cmd, old_boot_mode_str);
	strcat(cmd, " ");
	strcat(cmd, cmdurl);
	system(cmd);

	//reset to emmc/sd
	i = 0;
	while (board->boot_modes[i].name != NULL)
	{
		if (strcmp(board->boot_modes[i].name, old_boot_mode_str) == 0)
		{
			setting->boot_mode_hex = board->boot_modes[i].boot_mode_hex;
			break;
		}
		i++;
	}
	reset(setting);
}
#endif

static int monitor_width()
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
	return columns;

#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	//printf ("current lines %d\n", w.ws_row);
	//printf ("current columns %d\n", w.ws_col);
	return w.ws_col;
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
		char strcopy[200];
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
		ch = (char)getchar();
#endif

	}
	return ch;
}

static void monitor(struct options_setting* setting)
{
	signal(SIGINT, handle_sigint);
	struct board_info* board = get_board(setting->board);
	if (board == NULL) {
		printf("entered board model are not supported.\n");
		return;
	}

	char* previous_path = NULL;
	void* head = NULL;
	void* end_point = NULL;

	float voltage = 0;
	float current = 0;
	float power = 0;
	char ch = ' ';
	char sr_path[MAX_PATH_LENGTH];
	char sr_path2[MAX_PATH_LENGTH];
	char sr_path3[MAX_PATH_LENGTH];

	char sr_name[100];
	unsigned long start;
	get_msecond(&start);
	FILE* fptr = NULL;

	if (setting->dump == 1)
	{
		fptr = fopen("monitor_record.csv", "w+");

		/*print first row*/
		int i = 0;
		fprintf(fptr, "time(ms),");
		while (board->mappings[i].name != NULL)
		{
			if (board->mappings[i].type == power)
			{
				fprintf(fptr, "%s voltage(V),%s current(mA),", board->mappings[i].name, board->mappings[i].name);
			}
			i++;
		}
		fprintf(fptr, "\n");
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
	float vnow[MAX_NUMBER_OF_POWER]; float vavg[MAX_NUMBER_OF_POWER]; float vmax[MAX_NUMBER_OF_POWER]; float vmin[MAX_NUMBER_OF_POWER];
	float cnow[MAX_NUMBER_OF_POWER]; float cavg[MAX_NUMBER_OF_POWER]; float cmax[MAX_NUMBER_OF_POWER]; float cmin[MAX_NUMBER_OF_POWER];
	float pnow[MAX_NUMBER_OF_POWER]; float pavg[MAX_NUMBER_OF_POWER]; float pmax[MAX_NUMBER_OF_POWER]; float pmin[MAX_NUMBER_OF_POWER];
	float data_size[MAX_NUMBER_OF_POWER];
	int sr_level[MAX_NUMBER_OF_POWER];

	//initialize
	for (int i = 0; i < MAX_NUMBER_OF_POWER; i++)
	{
		vavg[i] = 0; vmax[i] = 0; vmin[i] = 99999;
		cavg[i] = 0; cmax[i] = 0; cmin[i] = 99999;
		pavg[i] = 0; pmax[i] = 0; pmin[i] = 99999;
		data_size[i] = 0;
		name[i] = 0;
	}
	int previous_width = monitor_width();

	//initialize all available SR(sense resistor) switch logic level
	for (int k = 0; k < n; k++)
	{
		strcpy(sr_name, "SR_");
		strcat(sr_name, board->mappings[name[k]].name);
		if (get_path(sr_path3, sr_name, board) != -1)
		{
			end_point = build_device_linkedlist_smart(&head, sr_path3, head, previous_path);
			previous_path = sr_path;
			if (end_point == NULL) {
				printf("monitor:failed to build device linkedlist\n");
				return;
			}
			struct gpio_device* gd = end_point;
			gd->gpio_write(gd, 0xFF);
		}
	}

	//power groups
	struct group groups[MAX_NUMBER_OF_POWER];
	char group_string[MAX_NUMBER_OF_POWER * MAX_MAPPING_NAME_LENGTH];
	groups_init(groups, MAX_NUMBER_OF_POWER);
	//initialize string
	strcpy(group_string, "");

	int num_of_groups = 0;
	if (board->power_groups != NULL)
		strcat(group_string, board->power_groups);
	if (strlen(setting->groups) > 0)
		strcat(group_string, setting->groups);

	num_of_groups = parse_groups(group_string, groups, board);
	if (num_of_groups == -1)
	{
		free_device_linkedlist_backward(end_point);
		if (setting->dump == 1)
		{
			fclose(fptr);
		}
		return;
	}

	while (!GV_MONITOR_TERMINATED)
	{
		//first calculate the value and store them in array
		int i = 0;//i is the index of all mappings
		int j = 0;//j is the index of the power related mapping only
		while (board->mappings[i].name != NULL)
		{
			if (board->mappings[i].type == power)
			{
				end_point = build_device_linkedlist_smart(&head, board->mappings[i].path, head, previous_path);
				previous_path = board->mappings[i].path;

				if (end_point == NULL) {
					printf("monitor:failed to build device linkedlist\n");
					if (setting->dump == 1)
					{
						fclose(fptr);
					}
					return;
				}
				struct power_device* pd = end_point;
				pd->power_get_voltage(pd, &voltage);
				msleep(1);
				pd->power_get_current(pd, &current);
				float power = current * voltage;
				name[j] = i;
				vnow[j] = voltage;
				cnow[j] = current;
				pnow[j] = power;
				vmin[j] = (voltage < vmin[j]) ? voltage : vmin[j];
				cmin[j] = (current < cmin[j]) ? current : cmin[j];
				pmin[j] = (power < pmin[j]) ? power : pmin[j];
				vmax[j] = (voltage > vmax[j]) ? voltage : vmax[j];
				cmax[j] = (current > cmax[j]) ? current : cmax[j];
				pmax[j] = (power > pmax[j]) ? power : pmax[j];
				cavg[j] = (data_size[j] * cavg[j] + current) / (float)(data_size[j] + 1);
				vavg[j] = (data_size[j] * vavg[j] + voltage) / (float)(data_size[j] + 1);
				pavg[j] = (data_size[j] * pavg[j] + power) / ((float)(data_size[j] + 1));
				data_size[j] = data_size[j] + 1;

				j++;
			}
			i++;
		}

		//get groups data
		for (int k = 0; k < num_of_groups; k++)
		{
			groups[k].sum = 0;
			for (int x = 0; x < groups[k].num_of_members; x++)
			{
				int mi = groups[k].member_index[x];
				groups[k].sum += pnow[mi];
			}
			groups[k].max = (groups[k].sum > groups[k].max) ? groups[k].sum : groups[k].max;
			groups[k].min = (groups[k].sum < groups[k].min) ? groups[k].sum : groups[k].min;
			groups[k].avg = (groups[k].avg_data_size * groups[k].avg + groups[k].sum) / (groups[k].avg_data_size + 1);
			groups[k].avg_data_size++;
		}

		//get SR(sense resistor) switch logic level

		for (int k = 0; k < n; k++)
		{
			strcpy(sr_name, "SR_");
			strcat(sr_name, board->mappings[name[k]].name);
			if (get_path(sr_path, sr_name, board) != -1)
			{
				end_point = build_device_linkedlist_smart(&head, sr_path, head, previous_path);
				previous_path = sr_path;
				if (end_point == NULL) {
					printf("monitor:failed to build device linkedlist\n");
					return;
				}
				struct gpio_device* gd = end_point;
				unsigned char data;
				gd->gpio_get_output(gd, &data);
				if (data == 0)
					sr_level[k] = 0;
				else
					sr_level[k] = 1;
			}
			else
			{
				sr_level[k] = -1;
			}
		}

		//then display
		int available_width = monitor_width();
		printf("%s", g_vt_green); //set the word as green
		printf("%s", g_vt_clear);
		printf("%s", g_vt_home); //move cursor to the 0,0

		if (available_width < 60)
		{
			printf("the command line window's width is too narrow\n");
			printf("current width: %d\n", available_width);
			printf("please set window's width to be at least 80 character wide\n");
			msleep(1000);
			continue;
		}

		int max_length = (get_max_power_name_length(board) <= 25) ? get_max_power_name_length(board) : 25;
		max_length = (max_length < 8) ? 8 : max_length; //the word "location" has a minimum of 8 letters
		int location_length = max_length + 3;//1 for letter, 1 for space between letter and location_name, 1 for space between location and '|''

		printfpadding(" ", location_length);
		if (available_width - max_length > 87)
		{
			printf("%s", g_vt_green);
			printf("%-21s", "|Voltage(V)");
			printf("%s", g_vt_yellow);
			printf("%-29s", "|Current(mA)");
			printf("%s", g_vt_kcyn);
			printf("%-29s", "|Power(mWatt)");
			printf("%s", g_vt_red);
			printf("%-6s", "|Extra");
			printf("\n");
			printf("%s", g_vt_white);
			printfpadding("location", location_length);
			printf("%s", g_vt_green);
			printf("|%-4s %-4s %-4s %-4s", "now", "avg", "max", "min");
			printf("%s", g_vt_yellow);
			printf(" |%-6s %-6s %-6s %-6s", "now", "avg", "max", "min");
			printf("%s", g_vt_kcyn);
			printf(" |%-6s %-6s %-6s %-6s", "now", "avg", "max", "min");
		}
		else if (available_width - max_length > 77)
		{
			printf("%s", g_vt_green);
			printf("%-11s", "|Voltage(V)");
			printf("%s", g_vt_yellow);
			printf("%-29s", "|Current(mA)");
			printf("%s", g_vt_kcyn);
			printf("%-29s", "|Power(mWatt)");
			printf("%s", g_vt_red);
			printf("%-6s", "|Extra");
			printf("\n");
			printf("%s", g_vt_white);
			printfpadding("location", location_length);
			printf("%s", g_vt_green);

			printf("|%-4s %-4s", "now", "avg");
			printf("%s", g_vt_yellow);

			printf(" |%-6s %-6s %-6s %-6s", "now", "avg", "max", "min");
			printf("%s", g_vt_kcyn);

			printf(" |%-6s %-6s %-6s %-6s", "now", "avg", "max", "min");
		}
		else if (available_width - max_length > 67)
		{
			printf("%s", g_vt_green);
			printf("%-11s", "|Voltage(V)");
			printf("%s", g_vt_yellow);
			printf("%-15s", "|Current(mA)");
			printf("%s", g_vt_kcyn);
			printf("%-29s", "|Power(mWatt)");
			printf("%s", g_vt_red);
			printf("%-6s", "|Extra");
			printf("\n");
			printf("%s", g_vt_white);
			printfpadding("location", location_length);
			printf("%s", g_vt_green);
			printf("|%-4s %-4s", "now", "avg");
			printf("%s", g_vt_yellow);
			printf(" |%-6s %-6s", "now", "avg");
			printf("%s", g_vt_kcyn);
			printf(" |%-6s %-6s %-6s %-6s", "now", "avg", "max", "min");
		}
		else
		{
			printf("%s", g_vt_green);

			printf("%-11s", "|Voltage(V)");
			printf("%s", g_vt_yellow);

			printf("%-15s", "|Current(mA)");
			printf("%s", g_vt_kcyn);

			printf("%-15s", "|Power(mWatt)");
			printf("%s", g_vt_red);
			printf("%-6s", "|Extra");
			printf("\n");
			printf("%s", g_vt_white);
			printfpadding("location", location_length);
			printf("%s", g_vt_green);
			printf("|%-4s %-4s", "now", "avg");
			printf("%s", g_vt_yellow);
			printf(" |%-6s %-6s", "now", "avg");
			printf("%s", g_vt_kcyn);
			printf(" |%-6s %-6s", "now", "avg");
		}
		printf("%s", g_vt_red);
		printf("%-6s", " |SR");
		printf("\n");
		printf("%s", g_vt_white);
		printfpadding("-----------------------------------------------------------------------------------------------------------------------------------------------", available_width);

		for (int k = 0; k < n; k++)
		{
			if (setting->dump == 1)
			{
				if ((k + 1) < n) {
					if (k == 0) {
						unsigned long now;
						get_msecond(&now);
						fprintf(fptr, "%ld,", (long)now - start);//add time before the first element
					}
					fprintf(fptr, "%f,%f,", vnow[k], cnow[k]);
				}
				else
					fprintf(fptr, "%f,%f\n", vnow[k], cnow[k]);
			}

			//printf("%-10s|",board->mappings[name[k]].name);
			printf("%s", g_vt_white);
			printf("%c ", 65 + k);//print corresponding letters, start with A
			printfpadding(board->mappings[name[k]].name, max_length);

			printf("%s", g_vt_green);
			printf(" |");

			printf("%s", g_vt_green);
			printf("%-4.2f ", vnow[k]);
			printf("%-4.2f ", vavg[k]);
			if (available_width - max_length > 87)
			{
				printf("%-4.2f ", vmax[k]);
				printf("%-4.2f ", vmin[k]);
			}

			printf("%s", g_vt_yellow);
			printf("|");

			printf("%s", g_vt_yellow);
			printf("%-6.1f ", cnow[k]);
			printf("%-6.1f ", cavg[k]);
			if (available_width - max_length > 77)
			{
				printf("%-6.1f ", cmax[k]);
				printf("%-6.1f ", cmin[k]);
			}

			printf("%s", g_vt_kcyn);
			printf("|");

			printf("%s", g_vt_kcyn);
			printf("%-6.1f ", pnow[k]);
			printf("%-6.1f ", pavg[k]);
			if (available_width - max_length > 67)
			{
				printf("%-6.1f ", pmax[k]);
				printf("%-6.1f ", pmin[k]);
			}
			printf("%s", g_vt_red);
			if (sr_level[k] == -1)
			{
				printf("|N/A");
			}
			else
			{
				printf("|%d", sr_level[k]);
			}

			printf("\n");
		}

		//then display group
		int max_group_length = 15;
		if (num_of_groups > 0)
		{
			//display groups
			printf("%s", g_vt_white);
			printf("\n\n");
			printfpadding("-----------------------------------------------------------------------------------------------------------------------------------------------", available_width);
			printfpadding(" ", max_group_length + 1);
			printf("%s", g_vt_blue);
			printf("|Power(mWatt)");
			printf("\n");
			printf("%s", g_vt_white);
			printfpadding("group", max_group_length);
			printf("%s", g_vt_blue);
			printf(" |%-6s %-6s %-6s %-6s", "now", "avg", "max", "min");
			printf("%s", g_vt_white);
			printf("  group members\n");
			printf("%s", g_vt_white);
			printfpadding("-----------------------------------------------------------------------------------------------------------------------------------------------", available_width);
		}
		for (int k = 0; k < num_of_groups; k++)
		{
			printfpadding(groups[k].name, max_group_length);
			printf("%s", g_vt_blue);
			printf(" |");
			printf("%-6.1f ", groups[k].sum);
			printf("%-6.1f ", groups[k].avg);
			printf("%-6.1f ", groups[k].max);
			printf("%-6.1f", groups[k].min);
			printf("%s", g_vt_white);
			//printf(" |");
			printf("  ");
			if (strlen(groups[k].member_list) < (size_t)(monitor_width() - max_group_length - 30))
				printf("%s\n", groups[k].member_list);
			else
			{
				printfpadding(groups[k].member_list, monitor_width() - max_group_length - 30 - 4);
				printf("...\n");
			}
		}

		//printf("width: %d \n",monitor_width());
		printf("\n");
		printf("%s", g_vt_white);
		printf("press the letter on keyboard to control coresponding extra sense resistor(Extra SR)\n");
		printf("Ctrl C to exit...\n");

		//finally,switch the SR
		ch = catch_input_char();
		//printf("\npressed: %c\n",ch);
		if (isalpha(ch))
		{
			int sr_index = (int)ch - 97;//97 is the ascii code for letter a
			if (sr_index < n && sr_index < MAX_NUMBER_OF_POWER && sr_index >= 0)
			{
				strcpy(sr_name, "SR_");
				if (board->mappings[name[sr_index]].name == NULL)
					return;
				strcat(sr_name, board->mappings[name[sr_index]].name);

				if (get_path(sr_path2, sr_name, board) != -1)
				{
					end_point = build_device_linkedlist_smart(&head, sr_path2, head, previous_path);
					previous_path = sr_path2;
					if (end_point == NULL) {
						printf("monitor:failed to build device linkedlist\n");
						return;
					}
					struct gpio_device* gd = end_point;
					unsigned char data;
					gd->gpio_get_output(gd, &data);
					if (data == 0)
						gd->gpio_write(gd, 0xFF);
					else
						gd->gpio_write(gd, 0x00);
				}
			}
		}
		if (!isalpha(ch))
			msleep(1000);
	}

	free_device_linkedlist_backward(end_point);
	if (setting->dump == 1)
	{
		fclose(fptr);
	}
	printf("%s", g_vt_clear);
	printf("%s", g_vt_home); //move cursor to the 0,0
	return;
}

static void lsftdi()
{
	ft_list_devices();
	return;
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

int main(int argc, char** argv)
{
	print_version();

	if (enable_vt_mode())
	{
		printf("Your console don't support VT mode, fail back to verbose mode");
	}

	if (argc == 1)
	{
		print_help(NULL);
		return 0;
	}

	char* cmd = argv[1];
	struct options_setting setting;
	memset(&setting, 0, sizeof(struct options_setting));//initialized to zero
	set_options_default(&setting);
	if (parse_options(argc, argv, &setting) == -1) {
		return 0;
	}

	if (strcmp(cmd, "lsgpio") == 0)
	{
		lsgpio(&setting);
	}
	else if (strcmp(cmd, "set_gpio") == 0)
	{
		set_gpio(&setting);
	}
	else if (strcmp(cmd, "set_boot_mode") == 0)
	{
		set_boot_mode(&setting);
	}
	else if (strcmp(cmd, "help") == 0)
	{
		if (argc == 2)
			print_help(NULL);
		if (argc == 3)
			print_help(argv[2]);
	}
	else if (strcmp(cmd, "monitor") == 0)
	{
		monitor(&setting);
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
		lsftdi();
	}
	else if (strcmp(cmd, "lsboard") == 0)
	{
		lsboard(&setting);
	}
	else if (strcmp(cmd, "lsbootmode") == 0)
	{
		lsbootmode(&setting);
	}
	else if (strcmp(cmd, "reset") == 0)
	{
		reset(&setting);
	}
	else if (strcmp(cmd, "init") == 0)
	{
		initialize(&setting, DONT_RESET);
	}
	else if (strcmp(cmd, "deinit") == 0)
	{
		deinitialize(&setting);
	}
#ifdef __linux__
	else if (strcmp(cmd, "flash") == 0)
	{
		uuu(&setting);
	}
#endif
	else if (strcmp(cmd, "version") == 0)
	{
		print_version();
	}
	else
	{
		printf("%s is a invalid command\n\n", cmd);
		print_help(NULL);
	}

	printf("done\n");
	return 0;
}

