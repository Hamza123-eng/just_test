
## Folder contents

The project **project** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   └── libraries
│   └── tasks
│       └── hardware
            └── hw_monitor.c
            └── rfid_uart.c
        └── network
            └── hawkbit
            └── hotspot
                └── data_store_nvs.c
                └── hotspot.c
                └── http_server.c
                └── scan.c
            └── parse
                └── get.c
                └── http_event_handler.c
                └── live_query_events.c
                └── parse_websocket.c
                └── parse.c
                └── post.c
                └── put.c
        └── history_event.c
│   └── main.c
│   ├── CMakeLists.txt
│   └── main.c
│   └── esp_gpio.c
|  
└── README.md    This is the file you are currently reading
```
Additionally, the project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
