idf_component_register(SRC_DIRS "." "tasks" "tasks/network/hawkbit" "libraries/src" "tasks/hardware" "tasks/network/parse" "tasks/network/hotspot"
    INCLUDE_DIRS "../include" "../include/tasks" "../include/tasks/network/hawkbit" "../include/tasks/hardware" "../include/tasks/network"
    "../include/tasks/network/hotspot" "../include/tasks/network/parse" "libraries/headers"
)

# REQUIRES wpa_supplicant esp_http_client nvs_flash
spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)