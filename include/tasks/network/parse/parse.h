
#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "http_client.h"

#define PARSE_SERVER_HOST_URL "143.198.190.41"
#define PARSE_SERVER_PORT "1337"
#define PARSE_SERVER_APP_ID "remoteWellAppId"
#define NO_OF_DATA_READ_FROM_PARSE 31

typedef enum
{

    APPON = 1,
    APPOFF = 2,

    MANNUELON = 3,
    MANNUELOFF = 4,

    AUTOMODEON = 5,
    AUTOMODEOFF = 6,

    TRIPON = 12,
    TRIPOFF = 8,

    UNDERMNTON = 9,
    UNDERMNTOFF = 13,

    PHASEERRORON = 10,
    PHASEERROROFF = 14,

    SCHEDULEON = 24,
    SCHEDULEOFF = 25,

    ELECTRICITYON = 31,
    ELECTRICITYOFF = 30,

    UNDERVOLTON = 32,
    OVERVOLTON = 33,
    VOLTERROROFF = 34,

    SCHEDULEAPPOFF = 35,
    SCHEDULEMNLOFF = 36,
    SCHEDULETRIPOFF = 37,
    SCHEDULEPHASEOFF = 38,
    SCHEDULEUNDEROFF = 39,
    SCHEDULEOVEROFF = 40,
    SCHEDULEMNTOFF = 41,

    /*<  only for dummy here>*/
    RUNON = 100,
    RUNOFF = 101
} device_system_event_t;

enum
{
    motor_button = 0x00,
    safety_mode = 0x01,
    auto_mode_motor = 0x02,
    under_volt_lim = 0x03,
    over_vol_lim = 0x04,
    normal_mode = 0x05,
    latched_vfd = 0x06,
    latched_delay = 0x07,
    smart_switch = 0x08,
    phase_fault = 0x09,
    motor_trip_status,
    //  phase_error_status,
    electricity_error_status,
    ping_time,
    device_mac,
    phase_error_code,
    voltage_error,
    voltage_p1,
    voltage_p2,
    voltage_p3,
    under_mnt_status,
    under_mnt_button,
    smart_switch_status,
    motor_status,
    device_ip,

    //** History
    run_time,
    history_time,
    phone_tag,
    tag,
    //** schedule
    schedule_on,
    schedule_off,
    schedule_status
} parse_column_t;
/**
 * @brief This structure contains fields that will be send to
 * the parse server
 */
typedef struct
{
    bool motor_status;
    bool trip_status;
    bool undermnt;
    bool phase_error;
    bool elcectricity;
    bool voltage_error;
    bool smart_switch_status;
    uint64_t run_time;
} live_class_t;

typedef struct
{
    char ssid1[32];
    char pswd1[32];

    char user_name[32];
    char user_pass[32];
    char device_num[4];

    uint8_t verified_user;
    uint8_t go_automode;

} parse_network_config_t;

/**
 * @brief This structure contains the server information
 * that will help in establishing a connection with
 * the parse server
 */
typedef struct
{
    char url[30];
    char appId[50];
    char port[6];
} ServerSettings_t;

/**
 * @brief This structure will contain the user information
 * and device number
 */
typedef struct
{
    char username[32];
    char password[32];
    char device_num[4];

} UserLoginInfo_t;

/**
 * @brief This structure will contain the response  information
 * after user login
 */
typedef struct
{
    char objectId[15];
    char sessionToken[100];
    char rfid_tag[20];
} UserLoginReceive_t;

/**
 * @brief This structure will contains all the URIs to communicate with the
 * parse server
 */
typedef struct
{
    char login_uri[200];
    char get_uri[200];
    char put_uri[200];
    char post_uri[200];
    char schedule_uri[200];
} ParseURI_t;

/**
 * @brief This structure contains all the URIs for websocket to communicate with the
 * parse server
 */
typedef struct
{
    char web_socket_uri[100];
    char ws_uri_real_time_data[100];
    char ws_uri_schedule[100];
} ParseWebSocketURI_t;

/**
 * @brief This structure contains all structs to be able to communicate, send,
 * and receive data from and to the parse server
 */
typedef struct
{

    ServerSettings_t server_settings;

    UserLoginInfo_t user_login_info;

    UserLoginReceive_t user_login_receive;

    ParseURI_t parse_uri;

    ParseWebSocketURI_t parse_ws_uri;
    /**
     * @brief This structure is used in get request. It will be filled after GET requrest is carried out. It is response
     * buffer will be used by the PUT request to get the objectId for real time data updation
     */
    char single_objectId[15];
    char *parse_put_post_buff;
    char mac_address[32];

} Parse_t;

typedef struct
{
    bool parse_init_flag;
    bool put_init_flag;
    bool post_init_flag;
    bool sch_init_flag;
} InitTrack_t;

extern InitTrack_t cloud_init_track;

void WebSocketInit();
void GetButtonValueTask(void *ptr);
void ReLogin();
void ParseInitTask(void *ptr);
void PutInitTask(void *ptr);
void PostInitTask(void *ptr);
void ParseSetLoginURI();
// extern TaskHandle_t xTaskToNotify;

/**
 * @brief This structure contains all structs to be able to communicate, send,
 * and receive data from and to the parse server
 */
extern Parse_t parse;

extern live_class_t live_class;

extern parse_network_config_t network_config;

extern http_header_t parse_header;

extern char *parse_data_name_array[NO_OF_DATA_READ_FROM_PARSE];

#endif // _PARSE_H_