file(GLOB SRC_FILES 
  src/*.c
  src/port/*.c
)

file(GLOB_RECURSE SRC_FILES_RECURSE
)


set(requires 
  "main" 
  "spi_flash" 
  "esp_timer"
  "esp_psram"
  "vfs"
  "fatfs"
  "nvs_flash"
  "esp_adc"
  )

if(CONFIG_EMUL_PLATFORM_GNUBOY)
  idf_component_register(    
      SRCS ${SRC_FILES}
      SRCS ${SRC_FILES_RECURSE}
      
      INCLUDE_DIRS ""
      INCLUDE_DIRS "src"
      INCLUDE_DIRS "src/port"

      REQUIRES ${requires}
      )

  target_compile_options(${COMPONENT_LIB} PRIVATE 
      -Wno-error
      -Wno-error=char-subscripts
      -Wno-error=unused-const-variable=
      -Wno-error=unused-but-set-parameter
      -Wno-error=misleading-indentation
      -Wno-error=format=
      -Wno-error=restrict
      -Wno-error=format-overflow=

      -DGNUBOY_NO_SCREENSHOT
      -DGNUBOY_NO_MINIZIP
      -DIS_LITTLE_ENDIAN
      
      ) 
else()
  idf_component_register(    
      
      INCLUDE_DIRS ""
      INCLUDE_DIRS "src"
      INCLUDE_DIRS "src/port"
      INCLUDE_DIRS "src/unzip"
      

      REQUIRES ${requires}
      )
endif()

  

