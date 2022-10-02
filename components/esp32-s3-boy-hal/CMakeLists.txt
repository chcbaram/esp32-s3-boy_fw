file(GLOB SRC_FILES 
  bsp/*.c
  hw/*.c
  hw/driver/*.c
  hw/driver/lcd/*.c
  hw/driver/hangul/*.c
)

file(GLOB_RECURSE SRC_FILES_RECURSE
  common/*.c 
)


set(requires 
  "spi_flash" 
  "esp_timer"
  "esp_psram"
  "vfs"
  "fatfs"
  "nvs_flash"
  "esp_adc"
  )


idf_component_register(    
    SRCS ${SRC_FILES}
    SRCS ${SRC_FILES_RECURSE}
    
    INCLUDE_DIRS ""
    INCLUDE_DIRS "bsp"
    INCLUDE_DIRS "common"
    INCLUDE_DIRS "common/core"
    INCLUDE_DIRS "common/hw/include"
    INCLUDE_DIRS "hw"
    INCLUDE_DIRS "lib" 

    REQUIRES "${requires}"
    )


  

