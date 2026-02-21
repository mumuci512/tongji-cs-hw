/* 2350222 ¼Æ¿Æ Ê¢êØ */
#pragma once

void FillBlank(struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi, const BLOCK_DISPLAY_INFO* const bdi);
void eliminate_middle(struct magic_ball& magic);
void eliminate_edge(struct magic_ball& magic);
void generate_array(struct magic_ball& magic);
void menu456789(char mode);

//void output_prompt(int x, int y);
//void output_array(struct magic_ball& magic);
//void output_init_array(struct magic_ball& magic);