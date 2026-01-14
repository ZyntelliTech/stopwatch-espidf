idf_component_register(SRCS "nvs_storage.c" "main.c" "rtc_timer.c" "oled.c" "stopwatch.c" "nvs_storage.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_i2c esp_driver_spi esp_driver_gpio u8g2 u8g2-hal-esp-idf esp_timer nvs_flash)
