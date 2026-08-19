#include "app.h"

#include "main.h"
#include "mongoose.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

extern UART_HandleTypeDef huart3;

static struct mg_mgr s_mgr;
static bool s_initialized;

static const GPIO_TypeDef *const s_led_ports[] = {GPIOB, GPIOF, GPIOG};
static const uint16_t s_led_pins[] = {GPIO_PIN_0, GPIO_PIN_4, GPIO_PIN_4};

static bool button_pressed(void) {
  return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET;
}

static bool led_is_on(size_t index) {
  return HAL_GPIO_ReadPin((GPIO_TypeDef *) s_led_ports[index], s_led_pins[index]) ==
         GPIO_PIN_SET;
}

static void led_write(size_t index, bool on) {
  HAL_GPIO_WritePin((GPIO_TypeDef *) s_led_ports[index], s_led_pins[index],
                    on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void uart_log(char ch, void *param) {
  UART_HandleTypeDef *uart = (UART_HandleTypeDef *) param;
  (void) HAL_UART_Transmit(uart, (const uint8_t *) &ch, 1U, 20U);
}

static void reply_state(struct mg_connection *c) {
  mg_http_reply(c, 200, "Content-Type: application/json\r\nCache-Control: no-store\r\n",
                "{\"leds\":[%s,%s,%s],\"button\":%s}\n",
                led_is_on(0) ? "true" : "false",
                led_is_on(1) ? "true" : "false",
                led_is_on(2) ? "true" : "false",
                button_pressed() ? "true" : "false");
}

static void serve_packed_file(struct mg_connection *c,
                              struct mg_http_message *hm, const char *path) {
  struct mg_http_serve_opts opts = {.fs = &mg_fs_packed};
  mg_http_serve_file(c, hm, path, &opts);
}

static void http_handler(struct mg_connection *c, int ev, void *ev_data) {
  if (ev != MG_EV_HTTP_MSG) return;

  struct mg_http_message *hm = (struct mg_http_message *) ev_data;
  MG_INFO(("HTTP %.*s %.*s", (int) hm->method.len, hm->method.buf,
           (int) hm->uri.len, hm->uri.buf));

  if (mg_match(hm->uri, mg_str("/api/state"), NULL) &&
      mg_match(hm->method, mg_str("GET"), NULL)) {
    reply_state(c);
    return;
  }

  int led_number = 0;
  struct mg_str capture = mg_str_n(NULL, 0);
  if (mg_match(hm->method, mg_str("POST"), NULL) &&
      mg_match(hm->uri, mg_str("/api/led/#"), &capture)) {
    if (capture.len == 1U && capture.buf[0] >= '1' && capture.buf[0] <= '3') {
      led_number = capture.buf[0] - '0';
    }
    bool on = false;
    if (led_number == 0 || !mg_json_get_bool(hm->body, "$.on", &on)) {
      mg_http_reply(c, 400, "Content-Type: application/json\r\n",
                    "{\"error\":\"Expected /api/led/1..3 and JSON {on:true|false}\"}\n");
      return;
    }
    led_write((size_t) led_number - 1U, on);
    MG_INFO(("LED %d %s", led_number, on ? "on" : "off"));
    reply_state(c);
    return;
  }

  if (mg_match(hm->method, mg_str("GET"), NULL)) {
    if (mg_match(hm->uri, mg_str("/"), NULL)) {
      serve_packed_file(c, hm, "/index.html");
      return;
    }
    if (mg_match(hm->uri, mg_str("/app.css"), NULL)) {
      serve_packed_file(c, hm, "/app.css");
      return;
    }
    if (mg_match(hm->uri, mg_str("/app.js"), NULL)) {
      serve_packed_file(c, hm, "/app.js");
      return;
    }
    if (mg_match(hm->uri, mg_str("/logo.jpg"), NULL)) {
      serve_packed_file(c, hm, "/logo.jpg");
      return;
    }
  }

  mg_http_reply(c, 404, "Content-Type: application/json\r\n",
                "{\"error\":\"Not found\"}\n");
}

void App_EthernetInit(void) {
  GPIO_InitTypeDef gpio = {0};

  __HAL_RCC_ETH_CLK_ENABLE();
  __HAL_RCC_ETHTX_CLK_ENABLE();
  __HAL_RCC_ETHRX_CLK_ENABLE();
  __HAL_RCC_SBS_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  MODIFY_REG(SBS->PMCR, SBS_PMCR_ETH_SEL_PHY, SBS_PMCR_ETH_SEL_PHY_2);

  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF11_ETH;

  gpio.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &gpio);
  gpio.Pin = GPIO_PIN_15;
  HAL_GPIO_Init(GPIOB, &gpio);
  gpio.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &gpio);
  gpio.Pin = GPIO_PIN_11 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOG, &gpio);
}

void App_Init(void) {
  mg_log_set_fn(uart_log, &huart3);
  mg_log_set(MG_LL_INFO);

  MG_INFO(("NUCLEO-H563ZI Web Control starting"));
  MG_INFO(("UART: USART3 PD8/PD9, 115200 8N1"));

  mg_mem_files = mg_packed_files;
  if (mg_mem_files == NULL || mg_unpacked("/index.html").buf == NULL) {
    MG_ERROR(("Packed filesystem activation failed: /index.html missing"));
    Error_Handler();
  }
  MG_INFO(("Packed filesystem active: /index.html"));

  mg_mgr_init(&s_mgr);
  if (mg_http_listen(&s_mgr, "http://0.0.0.0:80", http_handler, NULL) == NULL) {
    MG_ERROR(("HTTP listener failed on port 80"));
    Error_Handler();
  }
  MG_INFO(("HTTP listener ready on port 80 (plain HTTP)"));
  s_initialized = true;
}

void App_Poll(void) {
  if (s_initialized) mg_mgr_poll(&s_mgr, 1U);
}

int _write(int file, char *data, int length) {
  (void) file;
  if (data == NULL || length <= 0) return 0;
  return HAL_UART_Transmit(&huart3, (const uint8_t *) data, (uint16_t) length,
                           100U) == HAL_OK
             ? length
             : -1;
}
