/*
 * pwm.c
 *
 *  Created on: 2021. 2. 14.
 *      Author: baram
 */


#include "pwm.h"
#include "cli.h"

#ifdef _USE_HW_PWM
#include "driver/ledc.h"


typedef struct
{
  uint16_t max_value;
  uint16_t duty;
  ledc_channel_t channel;
} pwm_tbl_t;


#ifdef _USE_HW_CLI
static void cliPwm(cli_args_t *args);
#endif

static bool is_init = false;

pwm_tbl_t  pwm_tbl[PWM_MAX_CH];



bool pwmInit(void)
{
  bool ret = true;
  
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer = 
  {
    .speed_mode       = LEDC_LOW_SPEED_MODE,
    .timer_num        = LEDC_TIMER_0,
    .duty_resolution  = LEDC_TIMER_8_BIT,
    .freq_hz          = 5000,  
    .clk_cfg          = LEDC_AUTO_CLK
  };

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel = 
  {
    .speed_mode     = LEDC_LOW_SPEED_MODE,
    .channel        = LEDC_CHANNEL_0,
    .timer_sel      = LEDC_TIMER_0,
    .intr_type      = LEDC_INTR_DISABLE,
    .gpio_num       = GPIO_NUM_4,
    .duty           = 0, // Set duty to 0%
    .hpoint         = 0
  };

  for (int i=0; i<PWM_MAX_CH; i++)
  {
    pwm_tbl[i].duty = 0;
    pwm_tbl[i].max_value = 255;
  }

  pwm_tbl[0].channel = LEDC_CHANNEL_0;
    
  if (ledc_timer_config(&ledc_timer) != ESP_OK)
  {
    logPrintf("[NG] ledc_timer_config()\n");
  }
  if (ledc_channel_config(&ledc_channel) != ESP_OK)
  {
    logPrintf("[NG] ledc_channel_config()\n");
  }

  is_init = ret;

#ifdef _USE_HW_CLI
  cliAdd("pwm", cliPwm);
#endif
  return ret;
}

bool pwmIsInit(void)
{
  return is_init;
}

void pwmWrite(uint8_t ch, uint16_t pwm_data)
{
  if (ch >= PWM_MAX_CH) return;

  pwm_tbl[ch].duty = constrain(pwm_data, 0, pwm_tbl[ch].max_value);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_tbl[ch].channel, pwm_data);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_tbl[ch].channel);
}

uint16_t pwmRead(uint8_t ch)
{
  if (ch >= HW_PWM_MAX_CH) return 0;

  return pwm_tbl[ch].duty;
}

uint16_t pwmGetMax(uint8_t ch)
{
  if (ch >= HW_PWM_MAX_CH) return 255;

  return pwm_tbl[ch].max_value;
}



#ifdef _USE_HW_CLI
void cliPwm(cli_args_t *args)
{
  bool ret = true;
  uint8_t  ch;
  uint32_t pwm;


  if (args->argc == 3)
  {
    ch  = (uint8_t)args->getData(1);
    pwm = (uint8_t)args->getData(2);

    ch = constrain(ch, 0, PWM_MAX_CH);

    if(args->isStr(0, "set"))
    {
      pwmWrite(ch, pwm);
      cliPrintf("pwm ch%d %d\n", ch, pwm);
    }
    else
    {
      ret = false;
    }
  }
  else if (args->argc == 2)
  {
    ch = (uint8_t)args->getData(1);

    if(args->isStr(0, "get"))
    {
      cliPrintf("pwm ch%d %d\n", ch, pwmRead(ch));
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cliPrintf( "pwm set 0~%d 0~255 \n", PWM_MAX_CH-1);
    cliPrintf( "pwm get 0~%d \n", PWM_MAX_CH-1);
  }

}
#endif

#endif