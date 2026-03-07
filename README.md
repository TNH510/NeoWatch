# NeoWatch

## Install EDP-IDF V5.5.2

- Link: <https://dl.espressif.com/dl/esp-idf/>

- Add IDF_TOOLS_PATH to environment variable containing the path of esp-idf tools, e.g.`D:\program_file\Espressif`

- Add IFX_TOOLBOX_UUID, e.g. a56bec4e59ef018eaef7c905297cc5b4 (See this number when you run ESP-IDF CMD)

## Add dependency

```bash
idf.py add-dependency "lvgl/lvgl"

idf.py update-dependencies
```

## Build and flash

```bash
idf.py build    
idf.py flash
```
