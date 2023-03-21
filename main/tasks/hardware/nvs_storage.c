
#include "stdlib.h"
#include "stdio.h"

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"

#include "nvs_platform.h"

#include "hw_monitor.h"
#include "parse.h"
#include "fram.h"
#include "system_spiffs.h"
#include "parse_event_controller.h"

/**
 * @brief
 *
 * @param read_write
 */
void PerformDummyFile(void *param)
{
    FillDummy(sizeof(parse_event_send_t), OFFLINE_HSITORY_SIZE);
}
/**
 * @brief
 *
 * @param param
 */
void PrintUserConf(void *param)
{
    printf("Auto Mode      : %hhu , Saftey Mode  : %hhu , Phase Fault : %hhu\n", device_config.auto_mode, device_config.safety_mode, device_config.phase_fault);
    printf("Normal Mode    : %hhu , Latch Vfd    : %hhu , Latch Time  : %hhu\n", device_config.normal_device_mode, device_config.latch_vfd, device_config.vfd_latch_time);
    printf("Under Volt Lim : %hu ,  Over_Volt Lim : %hu , Verifed User : %hhu\n", device_config.under_volt_lim, device_config.over_volt_lim, network_config.verified_user);
    // printf("FILE EVENT : %hu ,  READ POINTER : %hu , WRITE POINTER : %hhu\n", fram_config.pending_event, fram_config.read_pointer, fram_config.write_pointer);
}
/**
 * @brief
 *
 * @param param
 */

void PerformNvsMnt(void *param)
{
    /*Only for write purpose*/
    nvs_perform("lasmntstatus", (uint8_t *)&live_class.undermnt, U8, true);
}

/**
 * @brief
 *
 */
void PerformNvsPostInit(void *param)
{
    uint8_t zero = 0;
    nvs_perform("electricity", &zero, U8, 1);
    nvs_perform("lamotorrun", &zero, U8, 1);
    nvs_perform("laswitchrun", &zero, U8, 1);
    // nvs_perform("lasmntstatus", &zero, U8, read_write);
}
/**
 * @brief
 *
 */
void PerformNvsHardwareDetail(bool read_write)
{

    nvs_perform("hard_serial", &device_config.hardware_serial, U32, read_write);
    nvs_perform_str("hard_version", (char *)device_config.hardware_version, STR, read_write);
}
/**
 * @brief
 *
 */
void PerformNvsElectricity(bool read_write)
{

    nvs_perform("electricity", &device_config.last_electricity_status, U8, read_write);
    nvs_perform("lamotorrun", &device_config.last_motor_run_state, U8, read_write);
    nvs_perform("laswitchrun", &device_config.last_switch_run_state, U8, read_write);
    nvs_perform("lasmntstatus", &device_config.last_mnt_status, U8, read_write);
}
/**
 * @brief
 *
 */
void PerformNvsFeatureParamter(bool read_write)
{
    nvs_perform("normal_mode", &device_config.normal_device_mode, U8, read_write);
    nvs_perform("latch_vfd", &device_config.latch_vfd, U8, read_write);
    nvs_perform("latch_time", &device_config.vfd_latch_time, U8, read_write);

    nvs_perform("under_lim", &device_config.under_volt_lim, U16, read_write);
    nvs_perform("over_lim", &device_config.over_volt_lim, U16, read_write);

    nvs_perform("auto_mode", &device_config.auto_mode, U8, read_write);
    nvs_perform("saftey_mode", &device_config.safety_mode, U8, read_write);
    nvs_perform("phase_fault", &device_config.phase_fault, U8, read_write);
}
/**
 * @brief Remotewell_PRO
 *
 * @param read_write
 */
void PerformNvsDeviceConfigParamter(bool read_write)
{
    PerformNvsElectricity(read_write);
    PerformNvsFeatureParamter(read_write);
}
/**
 * @brief Remotewell_PRO
 *
 * @param read_write
 */
void PerformNvsNetworkParamter(bool read_write)
{
    nvs_perform_str("ssid1", (char *)network_config.ssid1, STR, read_write);
    nvs_perform_str("pswd1", (char *)network_config.pswd1, STR, read_write);

    nvs_perform_str("user_name", (char *)network_config.user_name, STR, read_write);
    nvs_perform_str("user_pass", (char *)network_config.user_pass, STR, read_write);

    nvs_perform_str("device_num", (char *)network_config.device_num, STR, read_write);

    //   printf("SSID : %s \n",network_config.ssid1);
    //   printf("PSWD : %s \n",network_config.pswd1);
    //   printf("USER NAME: %s \n",network_config.user_name);
    //   printf("user accout pass : %s \n",network_config.user_pass);
    //   printf("device num : %s \n",network_config.device_num);
}
/**
 * @brief
 *
 * @param read_write
 */
void PerformNvsVerifiedUser(bool read_write)
{

    nvs_perform("verified_user", &network_config.verified_user, U8, read_write);
}
/**
 * @brief Remotewell_PRO
 *
 * @param read_write
 */
void PerformNvsFramParameter(bool read_write)
{
    nvs_perform("write_ptr", &fram_config.write_pointer, U16, read_write);
    nvs_perform("read_ptr", &fram_config.read_pointer, U16, read_write);
    nvs_perform("event_pend", &fram_config.pending_event, U16, read_write);
}
/**
 * @brief
 *
 * @param param
 */
void PerformNvsFactoryReset(void *param)
{
    uint8_t one = 1;
    uint8_t zero = 0;
    char *ptr = "";
    uint8_t dummy_1 = 0;
    uint16_t dummy_2 = 0;

    nvs_perform("normal_mode", &one, U8, 1);
    nvs_perform("latch_vfd", &zero, U8, 1);
    dummy_1 = 10;
    nvs_perform("latch_time", &dummy_1, U8, 1);
    dummy_2 = 170;
    nvs_perform("under_lim", &dummy_2, U16, 1);
    dummy_2 = 260;
    nvs_perform("over_lim", &dummy_2, U16, 1);

    nvs_perform("electricity", &zero, U8, 1);
    nvs_perform("lamotorrun", &zero, U8, 1);
    nvs_perform("laswitchrun", &zero, U8, 1);
    nvs_perform("lasmntstatus", &zero, U8, 1);

    nvs_perform("auto_mode", &zero, U8, 1);
    nvs_perform("saftey_mode", &one, U8, 1);
    nvs_perform("phase_fault", &one, U8, 1);

    /** fram status into fram */

    dummy_2 = 0;
    nvs_perform("write_ptr", &dummy_2, U16, 1);
    nvs_perform("read_ptr", &dummy_2, U16, 1);
    nvs_perform("event_pend", &dummy_2, U16, 1);

    /** credential  */

    nvs_perform_str("ssid1", ptr, STR, 1);
    nvs_perform_str("pswd1", ptr, STR, 1);

    nvs_perform_str("user_name", ptr, STR, 1);
    nvs_perform_str("user_pass", ptr, STR, 1);

    nvs_perform_str("device_num", ptr, STR, 1);

    /*verified user */

    nvs_perform("verified_user", &zero, U8, 1);
}

/**
 * @brief
 *
 * @param param
 */

void NvsInit(void *param)
{

    nvs_init(NULL);
    bool read = false;

    /*< Necassary Paramter to start Working >*/

    PerformNvsDeviceConfigParamter(false);
    PerformNvsNetworkParamter(false);
    PerformNvsFramParameter(false);
    PerformNvsVerifiedUser(false);
    PerformNvsHardwareDetail(false);
    /** only need at end */
    PerformNvsPostInit(NULL);
    PerformDummyFile(NULL);
    // PrintUserConf(NULL);
}
