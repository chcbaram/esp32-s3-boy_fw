#include "fatfs.h"
#include "cli.h"
#include "sd.h"


#ifdef _USE_HW_FATFS
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "diskio_impl.h"
#include "diskio_sdmmc.h"


#define lock()      xSemaphoreTake(mutex_lock, portMAX_DELAY);
#define unLock()    xSemaphoreGive(mutex_lock);


#ifdef _USE_HW_CLI
static void cliFatfs(cli_args_t *args);
#endif

static bool fatfsMount(void);
static bool fatfsUnMount(void);
static void fatfsSdEvent(sd_event_t);


static bool is_init = false;
static bool is_mounted = false;

static SemaphoreHandle_t mutex_lock;
static char *base_path = "/sdcard";
static BYTE drive_number = FF_DRV_NOT_USED;
static sdmmc_card_t *p_sdcard = NULL;
static esp_vfs_fat_sdmmc_mount_config_t fat_mount_config = 
    {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024
    };





bool fatfsInit(void)
{
  mutex_lock = xSemaphoreCreateMutex();


  ff_diskio_get_drive(&drive_number);
  logPrintf("[%s] ff_diskio_get_drive()\n", drive_number != FF_DRV_NOT_USED ? "OK" : "NG");
  if (drive_number == FF_DRV_NOT_USED) 
  {
    return false;
  }

  sdAddEventFunc(fatfsSdEvent);

#ifdef _USE_HW_CLI
  cliAdd("fatfs", cliFatfs);
#endif

  is_init = true;

  return true;
}

bool fatfsIsInit(void)
{
  return is_init;
}

bool fatfsIsMounted(void)
{
  return is_mounted;
}

bool fatfsLock(void)
{
  lock();
  return true;
}

bool fatfsUnLock(void)
{
  unLock();
  return true;
}

void fatfsSdEvent(sd_event_t sd_event)
{
  switch(sd_event.sd_state)
  {
    case SDCARD_CONNECTED:
      p_sdcard = sd_event.sd_arg;
      lock();
      fatfsMount();
      unLock();
      break;
  
    case SDCARD_DISCONNECTED:
      p_sdcard = sd_event.sd_arg;
      lock();
      fatfsUnMount();
      unLock();
      break;

    case SDCARD_ERROR:
      p_sdcard = sd_event.sd_arg;
      lock();
      fatfsUnMount();
      unLock();
      break;

    default:
      break;
  }
}

bool fatfsMount(void)
{
  bool ret = true;
  FATFS* fs = NULL;
  esp_err_t err;

  if (p_sdcard == NULL) return false;
  if (drive_number == FF_DRV_NOT_USED) return false; 
  if (is_mounted == true)
  {
    fatfsUnMount();
  }
    

  ff_diskio_register_sdmmc(drive_number, p_sdcard);
  ff_sdmmc_set_disk_status_check(drive_number, fat_mount_config.disk_status_check_enable);
  logPrintf("[__] fatfsMount : using pdrv=%i\n", drive_number);

  char drv[3] = {(char)('0' + drive_number), ':', 0};

  do
  {
    // connect FATFS to VFS
    err = esp_vfs_fat_register(base_path, drv, fat_mount_config.max_files, &fs);
    if (err != ESP_OK) 
    {
      logPrintf("[NG] esp_vfs_fat_register()\n");
      ret = false;
      break;
    }

    FRESULT res = f_mount(fs, drv, 1);
    if (res == FR_OK)
    {
      logPrintf("[OK] f_mount()\n");
    }
    else
    {
      ret = false;
      logPrintf("[NG] f_mount()\n");
      break;
    }
  } while(0);


  if (ret == false)
  {
    if (fs) 
    {
      f_mount(NULL, drv, 0);
    }
    esp_vfs_fat_unregister_path(base_path);
    ff_diskio_unregister(drive_number);
    logPrintf("[NG] fatfsUnMount()\n");
  }
  else
  {
    is_mounted = true;
    logPrintf("[OK] fatfsMount()\n");
  }

  return ret;
}

bool fatfsUnMount(void)
{
  bool ret = true;

  if (is_mounted == false) return false;
  is_mounted = false;

  // unmount
  char drv[3] = {(char)('0' + drive_number), ':', 0};
  f_mount(0, drv, 0);
  // release SD driver
  ff_diskio_unregister(drive_number);
  esp_vfs_fat_unregister_path(base_path);

  logPrintf("[OK] fatfsUnMount()\n");

  return ret;
}


#ifdef _USE_HW_CLI
void cliFatfs(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf("fatfs init      : %d\n", is_init);
    cliPrintf("fatfs mount     : %d\n", is_mounted);
    

    ret = true;
  }

  if (args->argc >= 1 && args->isStr(0, "dir") == true)
  {
    DIR * d = opendir(base_path); 
    struct dirent * dir;
    char buffer[300];

    if (args->argc == 1)
      snprintf(buffer, 256, "%s", base_path);
    else
      snprintf(buffer, 256, "%s/%s", base_path, args->getStr(1));

    cliPrintf("\n");
    cliPrintf("dir %s\n", buffer);
    if (d != NULL)
    {
      while ((dir = readdir(d)) != NULL) 
      {
        if (dir-> d_type != DT_DIR) 
        {
          FILE *f;
          int file_len = 0;

          sprintf(buffer, "%s/%s", base_path, dir->d_name);
          f = fopen(buffer, "r");
          if (f)
          {
            fseek(f, 0, SEEK_END);
            file_len = ftell(f);
            fclose(f);
          }
          if (strlen(dir->d_name) >= 28)
          {
            dir->d_name[28] = '.';
            dir->d_name[29] = '.';
            dir->d_name[30] = '.';
            dir->d_name[31] = 0;
          }
          cliPrintf("%-32s %dKB\n", dir->d_name, file_len/1024);
        }
        else if (dir -> d_type == DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ) 
        {
          cliPrintf("[%s]\n", dir->d_name); 
        }
      }
      closedir(d);
    }
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("fatfs info\n");
    cliPrintf("fatfs dir [name]\n");
  }
}
#endif

#endif