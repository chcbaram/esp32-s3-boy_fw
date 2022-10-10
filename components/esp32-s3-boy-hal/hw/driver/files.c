#include "files.h"
#include "cli.h"


#ifdef _USE_HW_FILES
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <dirent.h>



#ifdef _USE_HW_CLI
static void cliFiles(cli_args_t *args);
#endif


static esp_vfs_fat_sdmmc_mount_config_t mount_config = 
{
  .format_if_mount_failed = false,
  .max_files = 5,
  .allocation_unit_size = 16 * 1024
};
static sdmmc_card_t *p_sdcard;
static sdmmc_host_t host = SDMMC_HOST_DEFAULT();
static sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

#define MOUNT_POINT "/sdcard"



bool filesInit(void)
{
  esp_err_t ret;

  slot_config.width = 1;
  slot_config.cd  = GPIO_NUM_17;
  slot_config.clk = GPIO_NUM_8;
  slot_config.cmd = GPIO_NUM_9;
  slot_config.d0  = GPIO_NUM_18;

  const char mount_point[] = MOUNT_POINT;

  ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &p_sdcard);

  if (ret == ESP_OK) 
  {
    logPrintf("filesInit OK\n");
  }
  else
  {
    logPrintf("filesInit Fail\n");
  }

#ifdef _USE_HW_CLI
  cliAdd("files", cliFiles);
#endif

  return true;
}




#ifdef _USE_HW_CLI
void cliFiles(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    DIR * d = opendir("/sdcard"); 
    struct dirent * dir;

    if (d != NULL)
    {
      cliPrintf("opendir ok\n");
      while ((dir = readdir(d)) != NULL) 
      {
        if (dir-> d_type != DT_DIR) 
        {
          cliPrintf("%s\n",dir->d_name);
        }
        else if (dir -> d_type == DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ) 
        {
          cliPrintf("[%s]\n", dir->d_name); 
        }
      }

      closedir(d);
    }
    else
    {
      cliPrintf("opendir fail\n");
    }

    ret = true;
  }



  if (ret != true)
  {
    cliPrintf("files info\n");
  }
}
#endif

#endif