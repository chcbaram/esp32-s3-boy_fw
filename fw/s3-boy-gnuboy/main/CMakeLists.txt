

file(GLOB SRC_FILES 
  ap/*.c
  ap/*.cpp
  ap/gnuboy/src/*.c
  ap/gnuboy/src/port/*.c
)

file(GLOB_RECURSE SRC_FILES_RECURSE
)

set(requires "esp32-s3-boy-hal")




idf_component_register(    
    SRCS "main.cpp"

    SRCS ${SRC_FILES}
    SRCS ${SRC_FILES_RECURSE}
    
    INCLUDE_DIRS ""
    INCLUDE_DIRS "ap"
    INCLUDE_DIRS "ap/gnuboy/src/port"


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

    -DGNUBOY_NO_SCREENSHOT
    -DGNUBOY_NO_MINIZIP
    -DIS_LITTLE_ENDIAN
    ) 