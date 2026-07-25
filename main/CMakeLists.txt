idf_component_register(
    SRCS "HTTP.c" "../ui/ui.c" "../ui/ui_helpers.c" "../ui/screens/ui_Screen1.c" "../ui/images/icons8-summer-50.c" "HTTP.c" "WiFi.c" "main.c"
    "../ui/images/icons8-clouds-50.c" "../ui/images/icons8-rain-50.c" "../ui/images/icons8-heavy-rain-50.c"
    "../ui/images/icons8-partly-cloudy-day-50.c" "../ui/images/icons8-snow-50.c" "../ui/images/icons8-fog-50.c"
    "../ui/images/icons8-cloud-lightning-50.c"
    SRCS "HTTP.c" "../ui/ui.c" "../ui/ui_helpers.c" "../ui/screens/ui_Screen1.c" "HTTP.c" "WiFi.c" "main.c"
    "SNTP/time_sync.c"
    "fake/fake_config.cpp" "fake/fake_system_status.cpp" 
    "leop/leop.cpp" "leop/fake_leop.cpp" 
    "sensor/sensor.cpp" "sensor/fake_sensor.cpp"
    "UART/UART.cpp" "UART/uart_diag_shell.cpp"
    "../ui/screens/Main_UI.c"
    "../ui/Tabs/Home/Sensor_UI.c" "../ui/Tabs/Wifi/WiFi_UI.c"
    "../ui/Tabs/Electricity/Electricity_UI.c" "../ui/Tabs/Electricity/Price_UI.c"
    "../ui/Tabs/Weather/Weather_UI.c"
    "../ui/Tabs/Settings/Settings_UI.c"
    "LEOP/Recommendation.c" "LEOP/Weather.c" "JSONParser/DataParser.c"
    "LEOP/Price.c" "Memory/Spiffs.c"
    "LEOP/LEOP_Fetcher.c" "Cache/Cache.c"
    "LEOP/Price.c"
    "LEOP/LEOP_Fetcher.c"
    "hal/bme280_sensor.cpp" "hal/bme280_sensor_v2.cpp"
    "Memory/Spiffs.c" "Memory/NVS.c" 
    "Config/AppConfig.c" "Config/WifiConfig.c"
    "Utils/TimeFormat.c"
    INCLUDE_DIRS "." "SNTP" "fake" "hal" "leop" "sensor" "UART" "Memory" "Config" "Utils"
    WHOLE_ARCHIVE
    PRIV_REQUIRES can gpio rgb_lcd_port spiffs
    REQUIRES esp_lcd nvs_flash esp_event esp_wifi esp_http_client jansson espressif__bme280 espressif__i2c_bus
    )
    
idf_component_get_property(lvgl_lib lvgl__lvgl COMPONENT_LIB)
target_compile_options(${lvgl_lib} PRIVATE -Wno-format) 
