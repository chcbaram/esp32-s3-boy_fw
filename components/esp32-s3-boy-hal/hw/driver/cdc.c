#include "cdc.h"
#include "qbuffer.h"
#include "driver/usb_serial_jtag.h"


static bool is_init = false;
static usb_serial_jtag_driver_config_t usb_serial_jtag_config;
static qbuffer_t rx_q;
static uint8_t rx_buf[512];
static uint8_t rx_usb_buf[512];




bool cdcInit(void)
{
  bool ret;
  esp_err_t esp_ret;


  qbufferCreate(&rx_q, rx_buf, 512);

  usb_serial_jtag_config.tx_buffer_size = 128;
  usb_serial_jtag_config.rx_buffer_size = 128;

  esp_ret = usb_serial_jtag_driver_install(&usb_serial_jtag_config);
  if (esp_ret == ESP_OK)
  {
    is_init = true;
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool cdcIsInit(void)
{
  return is_init;
}

uint32_t cdcAvailable(void)
{
  uint32_t wr_len;
  int      rd_len;
  uint32_t ret_len;

  wr_len = rx_q.len - qbufferAvailable(&rx_q) - 1;
  rd_len = usb_serial_jtag_read_bytes(rx_usb_buf, wr_len, 0);
  
  qbufferWrite(&rx_q, rx_usb_buf, rd_len);

  ret_len = qbufferAvailable(&rx_q);

  return ret_len;
}

uint8_t cdcRead(void)
{
  uint8_t ret;

  qbufferRead(&rx_q, &ret, 1);

  return ret;
}

void cdcDataIn(uint8_t rx_data)
{
  cdcWrite(&rx_data, 1);
}

uint32_t cdcWrite(uint8_t *p_data, uint32_t length)
{
  int ret;

  ret = usb_serial_jtag_write_bytes(p_data, length, 100);
  
  return (uint32_t)ret;
}

uint32_t cdcGetBaud(void)
{
  return 115200;
}