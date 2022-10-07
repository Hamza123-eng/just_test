// #include <Client.h>
// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <ArduinoJson.h>
// #include <Firebase_ESP_Client.h>
// #include <MQTT.h>
// #include <CloudIoTCore.h>
// #include <CloudIoTCoreMqtt.h>
// #include "config.h" 

// //extern String pass;
// extern std::string value;
// extern std::string password_input;
// extern std::string ssid_input;

// RTC_DATA_ATTR bool time_synced = false;


// void messageReceived(String &topic, String &payload);

// // Initialize WiFi and MQTT for this board
// CloudIoTCoreDevice *device;
// CloudIoTCoreMqtt *mqtt;

// uint8_t wifi_try_count = 0;

// String getJwt(){
//   String jwt;
//   unsigned long iat = 0;
//   iat = time(nullptr);
//   Serial.println("Refreshing JWT");
//   jwt = device->createJWT(iat, jwt_exp_secs);
//   return jwt;
// }

// void setupWifi(){
//   // wifi_try_count++;
//   Serial.println("Starting wifi");
//  // wifi_try_count++;
//    // logln("WIFI MANAGER : GOING TO SET THE WIFI BY TRANSVERING the List");
//     WiFi.mode(WIFI_STA);
//     WiFi.begin("PTCL-BB", "zeeshan470");
//     while (1)
//     {
//         if (WiFi.status() != WL_CONNECTED)
//         {
//             printf("Waiting for WIFI\n");
//         }
//         if (WiFi.status() == WL_CONNECTED)
//         {
//             configTime(0, 0, ntp_primary, ntp_secondary); // necessary to connect to the library

//             printf("WIFI IS connected\n");
//             break;
          
//         }
//         delay(1000);
//     }

//   // WiFi.mode(WIFI_STA);

//   // for (int i = 1; i < 6; i++){
//   //   if (WiFi.status() != WL_CONNECTED) {
//   //     String ssid;
//   //     String password;
//   //     switch (i) {
//   //       case 1:
//   //         ssid = SSID_1;
//   //         password = password_1;
//   //         break;
//   //       case 2:
//   //         ssid = SSID_2;
//   //         password = password_2;
//   //         break;
//   //       case 3:
//   //         ssid = SSID_3;
//   //         password = password_3;
//   //         break;
//   //       case 4:
//   //         ssid = SSID_4;
//   //         password = password_4;
//   //         break;
//   //       case 5:
//   //         ssid = SSID_5;
//   //         password = password_5;
//   //         break;
//   //     }
//   //     Serial.println("Checking Wifi" + String(i) + String(ssid) + String(password));
//   //     if (ssid.length() > 0 && password.length() > 0 ) {
//   //       configTime(0, 0, ntp_primary, ntp_secondary); //necessary to connect to the library
//   //       WiFi.begin(ssid.c_str(), password.c_str());
//   //       Serial.println("HEAP 25  " + String(ESP.getFreeHeap()));
//   //       int count = 0;
//   //       while (WiFi.status() != WL_CONNECTED && count < 15){
//   //         Serial.print("."); 
//   //         delay(300);
//   //         count++;
//   //       }
//   //       if (WiFi.status() != WL_CONNECTED) {
//   //         Serial.println("WiFi " + String(i) + "not connected");
//   //       }
//   //       else {
//   //         wifi_try_count = 0;
//   //         Serial.println("Wifi " + String(i) + ssid + " connected");

//   //         // // configTime(0, 0, ntp_primary); //necessary to connect to the library
//   //         // // configTime(0, 0, ntp_primary, ntp_secondary); //necessary to connect to the library
//   //         // Serial.println("Waiting on time sync...");
//   //         // Serial.println("Time " + time(nullptr));
//   //         // while (time(nullptr) < 1510644967){
//   //         //   delay(10);
//   //         // }
//   //         return;
//   //       }
//   //     }
//   //     else {
//   //       Serial.println("Missing SSID or PASSWORD");
//   //     }
//   //   }
//   //   else { //meaning it is already connected
//   //     return;
//   //   }
//   // }
//   // if (WiFi.status() != WL_CONNECTED && wifi_try_count < 3) { //from a user standpoint, it would be good to make a light flash or turn red or something like that so that they could reconnect via BLE and resolve an issue
//   //   delay(5000);
//   //   setupWifi(); //should probably put some limit on this recursion. If wifi is connected, it will return to the top level, but still might want some limits
//   // }
// }

// ///////////////////////////////
// // Orchestrates various methods from preceeding code.
// ///////////////////////////////
// bool publishTelemetry(String data){
//   return mqtt->publishTelemetry(data);
// }

// // bool publishTelemetry(const char *data, int length){
// //   return mqtt->publishTelemetry(data, length);
// // }

// bool publishTelemetry(String subfolder, String data){
//   return mqtt->publishTelemetry(subfolder, data);
// }

// // bool publishTelemetry(String subfolder, const char *data, int length){
// //   return mqtt->publishTelemetry(subfolder, data, length);
// // }

// // bool publishState(String data) {
// //   return mqtt->publishState(data);
// // }

// // void connect(){
// //   Serial.println("connect() function");
// //   if (WiFi.status() != WL_CONNECTED) {
// //     setupWifi();
// //   }
// //   //mqttClient->subscribe("/sana-test");
// //   Serial.println("WiFi.status" + String(WiFi.status()));
// //   if (WiFi.status() == WL_CONNECTED){
// //     mqtt->startMQTT();
// //     // mqtt->mqttConnect();
// //   }
// //   else {
// //     Serial.println("Wifi not connected");
// //   }
// // }
// std::string private_key_str = "UNSET";
// std::string device_id ="UNSET"; 

// void setupCloudIoT(){
//   Serial.println("setupCloudIoT");
//   Client *netClient;
//   MQTTClient *mqttClient;


//   device = new CloudIoTCoreDevice(
//       project_id, location, registry_id, device_id.c_str(),
//       private_key_str.c_str());
//   netClient = new WiFiClientSecure();
//   ((WiFiClientSecure*)netClient)->setCACert(root_cert); //necessary to properly connect to IoTCore
//   mqttClient = new MQTTClient(512);
//   mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
//   mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);
//   mqtt->setUseLts(true);
//   mqtt->startMQTT();

//   mqtt->loop();
//     mqtt->mqttConnect();
// }

