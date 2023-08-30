# esp32-s3-boy_fw

## windows powershell
```
powershell -noexit -executionpolicy bypass -File  C:\Espressif\frameworks\esp-idf-master\export.ps1
```

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


## fw bin blash
```
esptool.py -p com1 write_flash  0x490000 ./build/s3-boy-fmsx.bin
```

## debug
```
 ~/.espressif/tools/openocd-esp32/v0.11.0-esp32-20220706/openocd-esp32/bin/openocd -f board/esp32s3-builtin.cfg
```

