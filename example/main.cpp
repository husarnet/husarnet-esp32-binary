#define LWIP_COMPAT_SOCKETS 0
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_heap_caps.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "websetup.h"
#include "sockets.h"
#include "port.h"
#include "util.h"

#include "default_wifi_config.h"

#include "rosnode.h"

extern "C" {
    void app_main();
}

void connected();

void bench_tcp() {
    vTaskDelay(20000 / portTICK_PERIOD_MS);

    int s = SOCKFUNC(socket)(AF_INET6, SOCK_STREAM, 0);
    sockaddr_in6 addr {};
    addr.sin6_family = AF_INET6;
    memcpy(&addr.sin6_addr, IpAddress::parse(HNET_IP).toBinary().data(), 16);
    addr.sin6_port = htons(5010);

    socklen_t addrsize = sizeof(addr);
    printf("connect...\n");
    int a = SOCKFUNC(connect)(s, (sockaddr*)&addr, addrsize);
    printf("connected finished errno=%d\n", errno);
    assert (a == 0);

    char zeroes[1000];

    while (true) {
        int64_t start = currentTime();
        int written = 0;
        while (written < 1000 * 1000) {
            int r = SOCKFUNC(write)(s, zeroes, sizeof(zeroes));
            assert (r > 0);
            written += r;
        }
        int delta = (int)(currentTime() - start);
        printf("transferred 1MB in %d ms\n", delta);
    }
}

void bench() {
    vTaskDelay(20000 / portTICK_PERIOD_MS);

    int s = SOCKFUNC(socket)(AF_INET6, SOCK_DGRAM, 0);

    sockaddr_in6 addr {};
    addr.sin6_family = AF_INET6;
    memcpy(&addr.sin6_addr, IpAddress::parse(HNET_IP).toBinary().data(), 16);
    addr.sin6_port = htons(5010);

    char zeroes[1000];
    int psize = 10;

    while (true) {
        int64_t start = currentTime();
        for (int i=0; i < 1000; i++) {
            int r = SOCKFUNC(sendto)(s, zeroes, psize, 0, (sockaddr*)&addr, sizeof(addr));
            assert (r > 0);
        }
        int delta = (int)(currentTime() - start);
        printf("transferred 1k packets in %d ms\n", delta);
    }
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        LOG("SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        LOG("SYSTEM_EVENT_STA_GOT_IP");
        LOG("got ip: %s",
            ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        static bool alreadyConnected = false;
        if (!alreadyConnected) {
            alreadyConnected = true;
            startThread(connected, "main", 5000);
            startThread(bench_tcp, "bench", 5000);
        }
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        LOG("SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    default:
        break;
    }
    return ESP_OK;
}

void wifiConnect() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config {};
    strcpy((char*)wifi_config.sta.ssid, DEFAULT_SSID);
    strcpy((char*)wifi_config.sta.password, DEFAULT_PASS);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void connected() {
    WebSetup::start("", "ESP32");
    LOG("IP: %s", WebSetup::nodeAddress.str().c_str());
    LOG("websetup secret: %s", WebSetup::sharedSecret.c_str());

    static char statsBuf[4000];

    while (true) {
        LOG("free memory: %d", (int)esp_get_free_heap_size());
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        vTaskGetRunTimeStats(statsBuf);
        LOG("stats: %s\n", statsBuf);
    }

}

extern "C" void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifiConnect();
}
