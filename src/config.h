// // Wifi network details.
// // Can save up to 5 networks - should we make this unlimited somehow?
// // WIll these need to be RTC Memory or perhaps in a text file?

// std::string UNSET = "UNSET";


// //Might be done better in a dynamically allocated array, but will do this for now
// String SSID_1 = "";
// String password_1 = "";
// String SSID_2 = "";
// String password_2 = "";
// String SSID_3 = "";
// String password_3 = "";
// String SSID_4 = "";
// String password_4 = "";
// String SSID_5 = "";
// String password_5 = "";


// // Cloud IoT details.
// const char *project_id = "sana-e4e82";
// const char *location = "us-central1";
// const char *registry_id = "alpha-registry";
// RTC_DATA_ATTR std::string device_id = UNSET;


// // Configuration for NTP
// const char* ntp_primary = "pool.ntp.org";
// const char* ntp_secondary = "time.nist.gov";

// #ifndef LED_BUILTIN
// #define LED_BUILTIN 13
// #endif

// // To get the private key run (where private-key.pem is the ec private key
// // used to create the certificate uploaded to google cloud iot):
// // openssl ec -in <private-key.pem> -noout -text
// // and copy priv: part.
// // The key length should be exactly the same as the key length bellow (32 pairs
// // of hex digits). If it's bigger and it starts with "00:" delete the "00:". If
// // it's smaller add "00:" to the start. If it's too big or too small something
// // is probably wrong with your key.
// // RTC_DATA_ATTR const char* private_key_str = UNSET;
// RTC_DATA_ATTR std::string private_key_str = UNSET;





// // Time (seconds) to expire token += 20 minutes for drift
// const int jwt_exp_secs = 60*20; // Maximum 24H (3600*24)

// // To get the certificate for your region run:
// //   openssl s_client -showcerts -connect mqtt.googleapis.com:8883
// // for standard mqtt or for LTS:
// //   openssl s_client -showcerts -connect mqtt.2030.ltsapis.goog:8883
// // Copy the certificate (all lines between and including ---BEGIN CERTIFICATE---
// // and --END CERTIFICATE--) to root.cert and put here on the root_cert variable.

// const char *root_cert =
// "-----BEGIN CERTIFICATE-----\n"
// "MIIDDDCCArKgAwIBAgIUXIRd61ARosjr5tpYAQK1udlptnswCgYIKoZIzj0EAwIw\n"
// "RDELMAkGA1UEBhMCVVMxIjAgBgNVBAoTGUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBM\n"
// "TEMxETAPBgNVBAMTCEdUUyBMVFNYMB4XDTIxMDUyNTAwMDAwMFoXDTIyMDUyNDAw\n"
// "MDAwMFowbTELMAkGA1UEBhMCVVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNV\n"
// "BAcMDU1vdW50YWluIFZpZXcxEzARBgNVBAoMCkdvb2dsZSBMTEMxHDAaBgNVBAMM\n"
// "EyouMjAzMC5sdHNhcGlzLmdvb2cwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARR\n"
// "VX2FBT6/ZCFlCwom7Pr7jtlh99RHfH0cxO51PZ0gifi8mo2UasKfsw0ikuZvaEkG\n"
// "busnKgGwa6TrBElBabLNo4IBVzCCAVMwEwYDVR0lBAwwCgYIKwYBBQUHAwEwDgYD\n"
// "VR0PAQH/BAQDAgeAMB4GA1UdEQQXMBWCEyouMjAzMC5sdHNhcGlzLmdvb2cwDAYD\n"
// "VR0TAQH/BAIwADAfBgNVHSMEGDAWgBSzK6ugSBx+E4rJCMRAQiKiNlHiCjBpBggr\n"
// "BgEFBQcBAQRdMFswLwYIKwYBBQUHMAKGI2h0dHA6Ly9wa2kuZ29vZy9ndHNsdHNy\n"
// "L2d0c2x0c3guY3J0MCgGCCsGAQUFBzABhhxodHRwOi8vb2NzcC5wa2kuZ29vZy9H\n"
// "VFNMVFNYMCEGA1UdIAQaMBgwDAYKKwYBBAHWeQIFAzAIBgZngQwBAgIwMAYDVR0f\n"
// "BCkwJzAloCOgIYYfaHR0cDovL2NybC5wa2kuZ29vZy9HVFNMVFNYLmNybDAdBgNV\n"
// "HQ4EFgQUxp0CLjzIieJCqFTXjDc9okXUP80wCgYIKoZIzj0EAwIDSAAwRQIgAIuJ\n"
// "1QvJqFZwy6sZCP1+dXOX4YTWAbum6FtqyJwOKIACIQDENBALkXPS9jo0g8X5+eT9\n"
// "MlOQcPMMtbXGtK/ENpE2rw==\n"
// "-----END CERTIFICATE-----\n"
// "-----BEGIN CERTIFICATE-----\n"
// "MIIC0TCCAnagAwIBAgINAfQKmcm3qFVwT0+3nTAKBggqhkjOPQQDAjBEMQswCQYD\n"
// "VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzERMA8G\n"
// "A1UEAxMIR1RTIExUU1IwHhcNMTkwMTIzMDAwMDQyWhcNMjkwNDAxMDAwMDQyWjBE\n"
// "MQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExM\n"
// "QzERMA8GA1UEAxMIR1RTIExUU1gwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARr\n"
// "6/PTsGoOg9fXhJkj3CAk6C6DxHPnZ1I+ER40vEe290xgTp0gVplokojbN3pFx07f\n"
// "zYGYAX5EK7gDQYuhpQGIo4IBSzCCAUcwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQW\n"
// "MBQGCCsGAQUFBwMBBggrBgEFBQcDAjASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1Ud\n"
// "DgQWBBSzK6ugSBx+E4rJCMRAQiKiNlHiCjAfBgNVHSMEGDAWgBQ+/v/MUuu/ND49\n"
// "80DQ5CWxX7i7UjBpBggrBgEFBQcBAQRdMFswKAYIKwYBBQUHMAGGHGh0dHA6Ly9v\n"
// "Y3NwLnBraS5nb29nL2d0c2x0c3IwLwYIKwYBBQUHMAKGI2h0dHA6Ly9wa2kuZ29v\n"
// "Zy9ndHNsdHNyL2d0c2x0c3IuY3J0MDgGA1UdHwQxMC8wLaAroCmGJ2h0dHA6Ly9j\n"
// "cmwucGtpLmdvb2cvZ3RzbHRzci9ndHNsdHNyLmNybDAdBgNVHSAEFjAUMAgGBmeB\n"
// "DAECATAIBgZngQwBAgIwCgYIKoZIzj0EAwIDSQAwRgIhAPWeg2v4yeimG+lzmZAC\n"
// "DJOlalpsiwJR0VOeapY8/7aQAiEAiwRsSQXUmfVUW+N643GgvuMH70o2Agz8w67f\n"
// "SX+k+Lc=\n"
// "-----END CERTIFICATE-----\n";



// // In case we ever need extra topics
// const int ex_num_topics = 0;
// const char* ex_topics[ex_num_topics];
// //const int ex_num_topics = 1;
// //const char* ex_topics[ex_num_topics] = {
// //  "/devices/my-device/tbd/#"
// //};
