

file(GLOB SRC_FILES 
  ap/*.c
  ap/*.cpp
  ap/doom/src/*.c
  ap/doom/src/port/*.c
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
    INCLUDE_DIRS "ap/doom/src"
    INCLUDE_DIRS "ap/doom/src/port"


    REQUIRES "${requires}"
    )

target_compile_options(${COMPONENT_LIB} PRIVATE 
    -Wno-error
    -Wno-error=char-subscripts
    -Wno-error=unused-const-variable=
    -Wno-error=unused-but-set-parameter
    ) 