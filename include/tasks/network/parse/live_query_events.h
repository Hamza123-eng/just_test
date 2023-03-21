#ifndef _LIVE_QUERY_H_
#define _LIVE_QUERY_H_





//extern char *parse_data_name_array[NO_OF_DATA_READ_FROM_PARSE];

void JsonParser(char *data_for_parsing);
void JsonParserSchedule(char *data_for_parsing);

int JsonParserBool(char *json_packet, char *event, char *key);
int JsonParserInt(char *json_packet, char *event, char *key);
void JsonParserEvent(char *json_packet, char *event);
void JsonParserString(char *json_packet, char *event, char *key, char *value);

#endif //_LIVE_QUERY_H_