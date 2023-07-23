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

static const char *TAG = "DHT22";
void DHT_task(void *pvParameter) {
  setDHTgpio(GPIO_NUM_23);
  ESP_LOGI(TAG, "开始 DHT22 📝\n");

  while (1) {
    int ret = readDHT();
    errorHandler(ret);
    ESP_LOGI(TAG, "💧 湿度: %.1f %%RH \x0a", getHumidity());
    ESP_LOGI(TAG, "🌞 温度: %.1f ˚C\n", getTemperature());

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
  xTaskCreate(&DHT_task, "DHT_task", 2048, NULL, 5, NULL);
}
