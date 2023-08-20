#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include "DHT22.h"
#include "sdkconfig.h"
#include "ssd1306.h"

static const char *TAG = "DHT22";
void DHT_task(void *pvParameter) {
  setDHTgpio(GPIO_NUM_25);
  SSD1306_t dev;
  i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
  ssd1306_init(&dev, 128, 64);
  ssd1306_clear_screen(&dev, false);
  ssd1306_contrast(&dev, 0xff);
  while (1) {
    int ret = readDHT();
    errorHandler(ret);
    float h = getHumidity();
    char humidity[14];
    ESP_LOGI(TAG, "💧 湿度: %.1f %%\x0a",
             h); // 🤷‍♂️不知道为什么，需要打印不然读取的湿度会错误  DHT:
                 // CheckSum error❌
    sprintf(humidity, "%.2f %%", h);
    float t = getTemperature();
    char temperature[14];
    ESP_LOGI(TAG, "🌞 温度: %.1f degC\n",
             t); // 🤷‍♂️不知道为什么，需要打印不然读取的温度会错误  DHT:
                 // CheckSum error❌
    sprintf(temperature, "%.2f degC", t);

    ssd1306_display_text(&dev, 0, humidity, 8, false);
    ssd1306_display_text(&dev, 2, temperature, 12, false);

    // -- wait at least 2 sec before reading again ------------
    // The interval of whole process must be beyond 2 seconds !!
    vTaskDelay(6000 / portTICK_PERIOD_MS);
  }
}

void app_main(void) {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  esp_log_level_set("*", ESP_LOG_INFO);
  vTaskDelay(6000 / portTICK_PERIOD_MS);
  xTaskCreate(&DHT_task, "DHT_task", 2048 * 2, NULL, 5, NULL);
}
