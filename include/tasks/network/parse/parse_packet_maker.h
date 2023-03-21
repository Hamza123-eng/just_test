#ifndef PUT_H_
#define PUT_H_

#include <stdint.h>
#include <stdbool.h>



/**
 * @brief This function reads the mac address of the STA and loads it into a mac_sta[13] array.
 * we send this mac address in put method
 * @param None
 * @return None
 */
void ReadMacSTA();


void ParsePakcetMaker(parse_event_send_t *event_data);

/**
 * @brief This helper function creates the json packet using CJSON library for PUT request
 * @param char* to_send the final packet will be loaded into this argument
 * @param int button_status
 * @param bool motor_status
 * @param bool trip_status
 * @param bool undermnt
 * @param bool alive_status
 * @param bool phase_error
 */
void PUT_JSON_Packet(char *to_send, int button_status, bool motor_status, bool trip_status, bool undermnt, bool alive_status, bool phase_error,bool electricity_st);

#endif // POST_H_
