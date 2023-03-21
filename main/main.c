
#include "main.h"
#include "hw_monitor.h"
#include "parse.h"
// #include "indications.h"
#include "Hawkbit.h"
#include "phase_sensing.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_sleep.h"

#include "phase_sensing.h"
#include "hlw8032.h"
#include "nvs_storage.h"
#include "indications.h"
#include "manage_system.h"
#include "mode_button.h"
#include "system_gpio.h"
#include "wifi_manger.h"
#include "pcf.h"
#include "freertos_handler.h"
#include "system_spiffs.h"
#include "system_time.h"
#include "dns_ping.h"

char *data = "\n"
             " _   _    ___        ______ ___ _____  \n"
             "| | | |  / \\ \\      / / __ )_ _|_   _| \n"
             "| |_| | / _ \\ \\ /\\ / /|  _ \\| |  | |   \n"
             "|  _  |/ ___ \\ V  V / | |_) | |  | |   \n"
             "|_| |_/_/   \\_\\_/\\_/  |____/___| |_|";

void app_main(void)
{
  /*< ---  SYSTEM SPEX & SYSTEM TRANSPORT LAYYER SECTION--- >*/

  printf("%s\n", data);
  Init_Spiffs(NULL);
  NvsInit(NULL);
  InitSystemTransport(NULL);

  // // /*< --- POWER SECTION --- >*/

  PhaseSensingInit(NULL);
  CheckPowerStatus(NULL);

  // /*< --- PHERIPHERIAL SECTION  --- >*/

  GpioInit(NULL);
 // HlwInit(NULL);

  //   /*< --- WIFI & INDICATION & COMMAND SECTION --- >*/

  WifiIndicationInit(NULL);
  InitWifiManger(NULL);
   DnsInit(NULL);
  InitTimeManger(NULL);
  BuzzerInit(NULL);
  InitModeButton(NULL);
  PcfInit(NULL);

  //   /*< --- USER AUTHENTICATION & NETWORK --- >*/

  MangeUserAndNetwork(NULL);

  //   /*< --- HAWKBIT --->*/
  // vTaskDelay(500);

  HawkbitStart();

  while (1)
  {
    vTaskDelay(100);
  }
}
