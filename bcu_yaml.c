/*
* Copyright 2020 NXP.
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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "bcu_yaml.h"

void writeConf(void)
{
	FILE *fp = fopen("config.yaml", "w+");
	if (fp == NULL) {
		printf("Failed to open file!\n");
		return;
	}

	char text[255];
	char chip_specification[MAX_PATH_LENGTH];
	int i = 0, j;

	sprintf(text, "# show_id can set the display order of the rail. Start from 1.\n# If show_id: 0, it means that this rail will not be displayed.\n");
	fputs(text, fp);

	for(j = 0; j < get_board_numer(); j++)
	{
		struct board_info* board=get_board_by_id(j);

		sprintf(text, "boardname: %s\n", board->name);
		fputs(text, fp);
		sprintf(text, "mappings:\n");
		fputs(text, fp);
		i = 0;
		while (board->mappings[i].name != NULL)
		{
			if(board->mappings[i].type == power)
			{
				sprintf(text, "    - %-20s: ", board->mappings[i].name);
				fputs(text, fp);

				int rs1, rs2;
				char char_pac[7] = "pac1934";

				get_chip_specification_by_chipname(board->mappings[i].path, chip_specification, "pac1934");
				rs1 = extract_parameter_value(chip_specification, "rsense1");
				rs2 = extract_parameter_value(chip_specification, "rsense2");

				sprintf(text, "{ ");
				fputs(text, fp);

				sprintf(text, "rsense1: %-8d, ", rs1);
				fputs(text, fp);
				sprintf(text, "rsense2: %-8d, ", rs2);
				fputs(text, fp);

				sprintf(text, "show_id: %-2d", i + 1);
				fputs(text, fp);

				sprintf(text, "}\n");
				fputs(text, fp);
			}
			i++;
		}
	}

	fclose(fp);
}

int updateRsense(struct board_info* board, char* rail_name, char* rs1, char* rs2)
{
	char path[MAX_PATH_LENGTH];

	if (get_path(path, rail_name, board) < 0)
	{
		printf("updateRsense: rail path not found!\n");
		return -1;
	}

	update_parameter_string(path, "rsense1", rs1);
	update_parameter_string(path, "rsense2", rs2);

	if (set_path(path, rail_name, board) < 0)
	{
		printf("updateRsense: set rail path failed!\n");
		return -1;
	}
	return 0;
}

int readConf(char* boardname)
{
	FILE* fh = fopen("config.yaml", "r");
	if (fh == NULL)
	{
		printf("Failed to open file!\n");
		return -1;
	}

	yaml_parser_t parser;
	yaml_token_t token;

	struct board_info *yaml_board_power_list = malloc(sizeof(struct board_info));

	if (!yaml_parser_initialize(&parser))
	{
		printf("Failed to initialize parser!\n");
		return -1;
	}

	yaml_parser_set_input_file(&parser, fh);

	/*
	* state = 0 -> key
	* state = 1 -> value
	*/
	int state = 0;
	char* tk;
	int i = -1;
	int now_status = -1;
	struct board_info* now_board;
	char now_rail[MAX_MAPPING_NAME_LENGTH];
	char rs1[10], rs2[10];

	do
	{
		yaml_parser_scan(&parser, &token);

		switch (token.type)
		{
		case YAML_KEY_TOKEN: state = 0; break;
		case YAML_VALUE_TOKEN: state = 1; break;
		case YAML_SCALAR_TOKEN:
			tk = token.data.scalar.value;

			if (state == 0)
			{
				if (!strcmp(tk, "boardname"))
					now_status = STATUS_CHANGE_BOARD;
				else if (now_status == STATUS_WAITING_WANTED_BOARD)
					break;
				else if (!strcmp(tk, "mappings"))
					now_status = -1;
				else if (!strcmp(tk, "rsense1"))
					now_status = STATUS_CHANGE_RSENSE1;
				else if (!strcmp(tk, "rsense2"))
					now_status = STATUS_CHANGE_RSENSE2;
				else if (!strcmp(tk, "show_id"))
					now_status = STATUS_CHANGE_SHOWID;
				else if (!strcmp(tk, "show_sel"))
					now_status = -1;
				else
				{
					now_status = STATUS_CHANGE_RAIL;
					//printf("railname: %s\n", tk);
					strcpy(now_rail, tk);
				}
			}
			else
			{
				switch (now_status)
				{
				case STATUS_WAITING_WANTED_BOARD: break;
				case STATUS_CHANGE_BOARD:
				{
					//printf("boardname: %s\n", tk);
					if (strcmp(tk, boardname))
					{
						now_status = STATUS_WAITING_WANTED_BOARD;
						break;
					}
					now_board = get_board(tk);
					if (now_board == NULL)
						return -1;
				}break;
				case STATUS_CHANGE_RAIL:
				{
					//printf("railname: %s\n", tk);
				}break;
				case STATUS_CHANGE_RSENSE1:
				{
					//printf("rse1: %s\n", tk);
					strcpy(rs1, tk);
				}break;
				case STATUS_CHANGE_RSENSE2:
				{
					//printf("rse2: %s\n", tk);
					strcpy(rs2, tk);
					updateRsense(now_board, now_rail, rs1, rs2);
				}break;
				case STATUS_CHANGE_SHOWID:
				{
					//printf("id: %s\n", tk);
					int item = get_item_location(now_rail, now_board);
					if (item < 0)
						return -1;
					now_board->mappings[item].initinfo = atoi(tk);
				}break;
				default:
					break;
				}
			}
			break;
		}
		if (token.type != YAML_STREAM_END_TOKEN)
			yaml_token_delete(&token);

	} while (token.type != YAML_STREAM_END_TOKEN);

	yaml_token_delete(&token);
	yaml_parser_delete(&parser);

	fclose(fh);

	return 0;
}
