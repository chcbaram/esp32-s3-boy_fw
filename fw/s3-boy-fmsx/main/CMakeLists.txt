

file(GLOB SRC_FILES 
  ap/*.c
  ap/*.cpp
)

file(GLOB_RECURSE SRC_FILES_RECURSE
  ap/fmsx/*.c
)

set(requires "esp32-s3-boy-hal")




idf_component_register(    
    SRCS "main.cpp"

    SRCS ${SRC_FILES}
    SRCS ${SRC_FILES_RECURSE}
    
    INCLUDE_DIRS ""
    INCLUDE_DIRS "ap"
    INCLUDE_DIRS "ap/fmsx/core/EMULib"
    INCLUDE_DIRS "ap/fmsx/core/fMSX"
    INCLUDE_DIRS "ap/fmsx/core/Z80"
    INCLUDE_DIRS "ap/fmsx/main"
    INCLUDE_DIRS "ap/fmsx/main/s3-boy"



    REQUIRES "${requires}"
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

    -DESP32_S3_BOY
    -DBPP16
    -DBPS16
		-DLSB_FIRST
    #-DNARROW
    ) 