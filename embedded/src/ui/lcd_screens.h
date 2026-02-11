#pragma once

#include <stdint.h>
#include <stdbool.h>


#include "data_flow/data_flow.h"  

void ui_lcd_init(void);
void ui_show_loading(void);
void ui_show_custom(const char *line1, const char *line2);

void ui_show_dht20_c(const Payload_Data *p);
void ui_show_dht20_f(const Payload_Data *p);
void ui_show_photores(const Payload_Data *p);
void ui_show_error(const char *line1, const char *line2);