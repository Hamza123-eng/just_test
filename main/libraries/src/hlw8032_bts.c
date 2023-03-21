#include "hlw8032_bts.h"
#include "esp_log.h"
#include "hlw8032.h"

static const char *TAG = "HLW_BTS: ";
int energy_int = 0;

//******************************************************************/
//         Multiplying Factors
//******************************************************************/

#define VoltR1 1880000 // Voltage divider Upstream resistors 470K*4  1880K
#define VoltR2 1000000 // Voltage divider downstream resistors  1K
#define CurrentRF 0.001

float VF = (float)VoltR1 / (float)VoltR2;

float Vmf = 224.3 / 220.5;

//--------------------------------------------------------------------/

//********************************************************************/
//        HLW8032 Registers
//********************************************************************/
uint32_t VoltPar;
uint32_t VoltData;

//--------------------------------------------------------------------/

//********************************************************************/
//       Electrical Energy related Variables
//********************************************************************/
/* N0thing is here*/
//-------------------------------------------------------------------/

void calc_voltage(uint8_t *HLW8032Regs, uint8_t phase_num);
void calc_current(uint8_t *HLW8032Regs, uint8_t phase_num);
void calc_power(uint8_t *HLW8032Regs, uint8_t phase_num);

bool CheckSum(uint8_t *HLW8032Regs)
{
  uint8_t check = 0, a = 0;

  for (a = 2; a <= 22; a++)
  {
    check = check + HLW8032Regs[a];
  }
  if (check == HLW8032Regs[23])
  {
    return true;
  }
  else
  {
    return false;
  }
}

void hlw_data_handle(uint8_t *HLW8032Regs, uint8_t len, uint8_t phase_num)
{

  int i = 0;
  while (HLW8032Regs[i] != 0x5A && i < len) // 'check byte' is always '0x5A' and present on index '1' of frame. (index ranges: 0 --> n)
    i++;

  if (((i - 1) + HLW_DATA_SIZE) > len || i == 0) // complete frame of 24 bytes not present in data
    return;

  // //ESP_LOGI(TAG, "Checksum");
  if (CheckSum(&HLW8032Regs[i - 1]) == false)
  {
    ESP_LOGI(TAG, "Checksum Failed");
    return;
  }

  calc_voltage(&HLW8032Regs[i - 1], phase_num);
}

//********** Voltage Calculation **********/

void calc_voltage(uint8_t *HLW8032Regs, uint8_t phase_num)
{
  VoltPar = ((uint32_t)HLW8032Regs[VP] << HS) + ((uint32_t)HLW8032Regs[VP + 1] << MS) + HLW8032Regs[VP + 2];
  if (HLW8032Regs[DUR] & (1 << 6))
  {
    VoltData = ((uint32_t)HLW8032Regs[V] << HS) + ((uint32_t)HLW8032Regs[V + 1] << MS) + HLW8032Regs[V + 2];
  }
  if (phase_num == 2)
  {
    hlw_voltage.phase_voltage[0] = (((float)VoltPar / (float)VoltData) * VF) * Vmf;
    /** Assumption if data is malfunction by hlw */
    if (hlw_voltage.phase_voltage[0] > 300)
    {
      hlw_voltage.phase_voltage[0] = 220;
    }
  }
  if (phase_num == 1)
  {
    hlw_voltage.phase_voltage[1] = (((float)VoltPar / (float)VoltData) * VF) * Vmf;

    /** Assumption if data is malfunction by hlw */

    if (hlw_voltage.phase_voltage[1] > 300)
    {
      hlw_voltage.phase_voltage[1] = 220;
    }

  }

  hlw_voltage.phase_voltage[2] = (hlw_voltage.phase_voltage[0] + hlw_voltage.phase_voltage[1]) / 2;
  // else if((hlw_voltage.phase_selected - 1) == 1)
  // {
  //   hlw_voltage.phase_voltage[1] = (((float)VoltPar / (float)VoltData) * VF) * Vmf;
  // }
  // else
  // {
  //   hlw_voltage.phase_voltage[2] = (((float)VoltPar / (float)VoltData) * VF) * Vmf;
  // }
  // ESP_LOGI("HLW VOLTAGE PHASE [%i] :%i",(phase_num +1), hlw_voltage.phase_voltage[phase_num]);
  //  printf("VOLTAGE PHASE [1] :%i VOLTAGE PHASE [2] : %i  VOLTAGE PHASE [2] : %i\n", hlw_voltage.phase_voltage[0],hlw_voltage.phase_voltage[1],hlw_voltage.phase_voltage[2]);
  //  printf("VOLTAGE PHASE [1] : %i\n",  hlw_voltage.phase_voltage[1]);
  //  printf("VOLTAGE PHASE [2] : %i\n",  hlw_voltage.phase_voltage[2]);
}
