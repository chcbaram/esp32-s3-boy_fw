# esp32-s3-boy_fw


## set-target
```
idf.py set-target esp32s3
```


## build
```
idf.py build
```

## flash
```
idf.py -p com1 flash
```

## debug
```
 ~/.espressif/tools/openocd-esp32/v0.11.0-esp32-20220706/openocd-esp32/bin/openocd -f board/esp32s3-builtin.cfg
```