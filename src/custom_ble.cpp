// #include "custom_ble.h"

#include <BLE.h>
#include "main.h"
#include "custom_ble.h"
#include "WiFi.h"
#include "NimBLEDevice.h"
#include "wifi_manager.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "touch_pad_custom.h"
#include "i2s_play.h"

char ssid[256];

bool deviceConnected = false;
bool pass_acquired = false;
static uint8_t counter = 0;
uint8_t scan_refresh = 0;

int txValue = 0;
String pass;
std::string value;
std::string password_input;
std::string ssid_input;
std::string device_id_1 = "TEST_DEVICE";
String networks = String(device_id_1.c_str());

unsigned long BLE_millis = millis();

BleStatus_t ble_status = kIdle;

NimBLECharacteristic *pCharacteristic_0;
NimBLECharacteristic *pCharacteristic_1;
NimBLEAdvertising *pAdvertising;

extern std::string device_id_1;

// extern void setWifiConfiguration(boolean status, std::string password, std::string ssid);

#define SERVICE_UUID_1 "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
// #define SERVICE_UUID_2 "57c6d1aa-ad06-11ec-b909-0242ac120002"

#define CHARACTERISTIC_0_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_1_UUID_TX "8ac232ac-3eaa-4392-953d-ef16588f1c38"
// #define CHARACTERISTIC_2_UUID_TX "2cd93646-8c39-4532-bf73-21584dacd499"
// #define CHARACTERISTIC_3_UUID_TX "fda83d1d-67e1-460a-b92a-48f49623e64a"
// #define CHARACTERISTIC_4_UUID_TX "57c6d1aa-ad06-11ec-b909-0242ac120002"
#define CHARACTERISTIC_5_UUID_TX "37ac4b4a-9f17-11ec-b909-0242ac120002"

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    pass_acquired = false;
  }
};

class MyCallbackHandler : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    scan_refresh = SCAN_REFRESH_TIME / (1000 * 1);

    value = pCharacteristic->getValue();
    int len = value.length();

    if (value.length() > 0)
    {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++)
      {
        Serial.print(String(value[i]));
      }
      Serial.println();
      Serial.println("*********");
    }
  }
};

bool ScanNetwork()
{
  logln("BLE SETUP : SCAN NETWORK");

  bool status = true;
  int network_available = -1;
  int scan_tries = 0;

  while (network_available == -1 && scan_tries < 5)
  {
    network_available = WiFi.scanNetworks();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    scan_tries += 1;
  }

  status = (scan_tries < 5) ? 1 : 0;

  // print the list of networks seen:
  if (status)
  {
    networks.clear();
    networks = String(device_id_1.c_str());

    Serial.print("number of available networks:");
    Serial.println(network_available);

    const int limit = min(network_available, 15);

    // print the network number and name for each network found:
    for (int thisNet = 0; thisNet < limit; thisNet++)
    {
      networks = networks + "__" + String(WiFi.SSID(thisNet));
    }
    return 1;
  }
  else
  {
    return 0;
  }
}
void BleTask(void *param)
{
  Serial.println("Initializing BLE\n");
  Serial.println("device id: " + String(device_id_1.c_str()));
  WiFi.mode(WIFI_MODE_NULL);
  Serial.println("Turn off Wifi");
  NimBLEDevice::init("Retarduino");
  // MTU Request here
  NimBLEDevice::setMTU(500);

  NimBLEServer *pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  NimBLEService *pService = pServer->createService(SERVICE_UUID_1);

  pCharacteristic_0 = pService->createCharacteristic(
      CHARACTERISTIC_0_UUID_TX,
      NIMBLE_PROPERTY::NOTIFY);

  pCharacteristic_1 = pService->createCharacteristic(
      CHARACTERISTIC_1_UUID_TX,
      NIMBLE_PROPERTY::NOTIFY);

  NimBLECharacteristic *pCharacteristic_5 = pService->createCharacteristic( // The client would write to this characteristic
      CHARACTERISTIC_5_UUID_TX,
      NIMBLE_PROPERTY::WRITE);

  pCharacteristic_5->setCallbacks(new MyCallbackHandler());

  pService->start();
  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID_1);
  pAdvertising->start();

  Serial.println("Networks" + networks);
  Serial.println("Networks" + networks);

  Serial.println("deviceConnected" + String(deviceConnected));
  Serial.println("Waiting for a BLE connection...");

  bool timeOut = true;
  BLE_millis = millis();

  /*DEVICE CONDITION*/
  ble_status = kIdle;

  while (1)
  {
    if (!deviceConnected)
    {
      logln("Waiting for User to Available");
      if (millis() - BLE_millis > BLE_WAIT)
      {
        ble_status = kTimeOut;
        break;
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    else if (deviceConnected)
    {
      /* if Device is connected*/
      logln("Device Is Available Going for Scan");
      while (1)
      {
        if (ScanNetwork())
        {
          scan_refresh = SCAN_REFRESH_TIME / (1000 * 1);

          memset(ssid, 0, 256);
          strcpy(ssid, networks.c_str());

          Serial.println(ssid);

          pCharacteristic_0->setValue(ssid);
          pCharacteristic_5->setValue(pass.c_str());

          pCharacteristic_0->notify();
          pCharacteristic_5->notify();

          while (!pass_acquired && scan_refresh > 0 && deviceConnected)
          {
            pCharacteristic_0->notify();
            std::string passwordAndNetwork = pCharacteristic_5->getValue(); // expecting a value from client side
            std::string delim = "__";
            int length = passwordAndNetwork.length();
            Serial.println("Password Length : " + String(length));
            vTaskDelay(500 / portTICK_PERIOD_MS);

            if (length < 8) // is this an arbitrary number os is 8 a standard password minimum length?
            {
              Serial.println("Password incomplete , try again !");
            }
            else
            {
              Serial.println("Password acquired !");
              size_t pos = 0;
              while ((pos = passwordAndNetwork.find(delim)) != std::string::npos)
              {
                ssid_input = passwordAndNetwork.substr(0, pos);
                passwordAndNetwork.erase(0, pos + delim.length());
                password_input = passwordAndNetwork;
              }
              pass_acquired = true; /*data receive break*/
              ble_status = kGotdata;
            }

            scan_refresh -= 1;
            vTaskDelay(1000 / portTICK_PERIOD_MS);
          } // pass acquire
          /* Why come from Data Get Loop*/
          if (!deviceConnected)
          {
            break; /* OUT FROM SCAN LOOP */
          }
          if (ble_status == kGotdata)
          {
            ble_status = kSuccess;
            pAdvertising->stop();
            wifi_check();
            Serial.println("BLE ADs ceased !"); // proceed to setup wifi
            break;                              /* SCAN BREAK*/
          }
          /* Why Come from the data Get loop*/
        }
        else
        {
          logln("BLE SETUP: NO NETWORK IN SCANNING");
          ble_status = kScanFailed;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
      } /*END OF the Scanner Of Wifi LOOP while*/
      if (ble_status == kSuccess)
      {
        break;
      }
    } /*End Of else if if device connected*/
    else
    {
      /*Going to fullfill the CXX Compilence*/
    }
  }                           // while loop final
  NimBLEDevice::deinit(true); // completely disable BLE controller and free up memory
  logln("DONE WITh BLE");
}

void wifi_handling()
{
  WiFi.mode(WIFI_MODE_NULL); // Turn off the WiFI completely !!
  Serial.println("WiFi turned off, initializing BLE again !");
  BLE_setup(); // re-initializing BLE

  // Serial.println("Restarting...");
  // ESP.restart();
}

void BLE_wifi_status_report(boolean status)
{
  Serial.println("Initializing BLE with Wifi Status" + String(status) + "\n");

  char wifi_status[10];
  strcpy(wifi_status, String(status).c_str());

  pAdvertising->start();

  Serial.println("deviceConnected" + String(deviceConnected));
  Serial.println("Waiting for a BLE connection...");

  pCharacteristic_1->setValue(wifi_status);

  Serial.println("Set Value: " + String(status));

  while (!deviceConnected)
  {
  }

  while (deviceConnected)
  {
    delay(500);
    Serial.println("notify");
    pCharacteristic_1->notify();
  }

  if (!deviceConnected)
  {
    Serial.println("!deviceConnected");
    NimBLEDevice::deinit(true);
    Serial.println("BLE ADs ceased !");
  }
}

void wifi_check()
{
  WiFi.begin(ssid_input.c_str(), password_input.c_str());
  boolean WifiStatus;
  Serial.println("Checking WiFi");
  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 30)
  {
    Serial.print(".");
    delay(300);
    count++;
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wifi Not Connected");
    WifiStatus = false;
  }
  else
  {
    Serial.println("Wifi Connected");
    WifiStatus = true;
  }
  WiFi.mode(WIFI_MODE_NULL);
  Serial.println("Wifi Turned Off");
  BLE_wifi_status_report(WifiStatus);
  setWifiConfiguration(WifiStatus, password_input, ssid_input); // to save the correct password on the toy
}

void BleInit(void *param)
{
  /*necessary thing here*/
  bool order;
  while (1)
  {
    if (xQueueReceive(xQueueBleOperation, &(order), (2000 / portTICK_PERIOD_MS)) == 1)
    {
      ButtonPress_t rec_button=kTouch4;
      xQueueSendToBack(xQueueAudioPlay,&(rec_button), 0);
      BleTask((void *)NULL);
      xQueueReset(xQueueAudioPlay);
    }
  }
}
