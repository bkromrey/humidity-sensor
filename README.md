# CS 467 - Online Capstone Project (Winter 2026)

Project: Microcontroller Home Humidity Sensor
Team: Brianna Kromrey, John David Lopez, Eugenia Uvarov

### build for WSL 2 using ninja ( from root folder of project)
```
rm -rf ninja_build
mkdir ninja_build
cd ninja_build
cmake ../src -G Ninja -DPICO_BOARD=pico
cmake --build . -j
```
## LCD ussage
lcd_env_render(&ui); 
### initialization(main.c)
```
i2c_init(LCD_I2C_PORT, 100 * 1000);
gpio_set_function(LCD_I2C_SDA, GPIO_FUNC_I2C);
gpio_set_function(LCD_I2C_SCL, GPIO_FUNC_I2C);
gpio_pull_up(LCD_I2C_SDA);
gpio_pull_up(LCD_I2C_SCL);

lcd_init();
```

### example ussage
```
lcd_env_data_t ui = {0};
ui.mode = LCD_MODE_NORMAL;
ui.view_mode = LCD_VIEW_ENV;

ui.has_temp = true;
ui.temp_unit = TEMP_C;
ui.temp_value = 23.4f;

ui.has_humidity = true;
ui.humidity_percent = 51.0f;

lcd_env_render(&ui);
```

### no data:
```
ui.has_temp = false;
ui.has_humidity = false;
lcd_env_render(&ui);
```

### text messages
```
ui.mode = LCD_MODE_NORMAL;
ui.view_mode = LCD_VIEW_TEXT;
ui.line1 = "test message";
ui.line2 = "test message";

lcd_env_render(&ui);
```

### Error
```
ui.mode = LCD_MODE_ERROR;
ui.line1 = "error";
ui.line2 = "some error";

lcd_env_render(&ui);
```

### Test Mock up ( no real data)
mock_ui_run_10s();

### important

LCD = 16x2, line ≤ 16 Symbols

lcd_string() please don call from main.c