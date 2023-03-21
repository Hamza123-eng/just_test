#ifndef _PHASE_SENSING_
#define _PHASE_SENSING_

#define INTR_PIN_MASK (1ULL << PHASE1_OPAMP_PIN | 1ULL << PHASE2_OPAMP_PIN | 1ULL << PHASE3_OPAMP_PIN)

#define TIMER_DIVIDER (100)                          //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER) // convert counter value to second
#define ESP_INTR_FLAG_DEFAULT 0

#define TOL_DEG 6;      // Tolerencef degree
#define DIFF_1_DEG 59;  // deviation of phase 1
#define DIFF_2_DEG 119; // deviation of phase 2

void PhaseSensingInit(void *param);
void CheckPowerStatus(void *param);

typedef enum
{
    PHASE1 = 0x01,
    PHASE2 = 0x02,
    PHASE3 = 0x04,
    PHASE_FAULT = 0x08,

    ELECTRICITY_ST = 0x10,
    PHASE_FAULT_ENABLE = 0x20
} phase_status_t;

// #define   RELAY_PIN GPIO_NUM_19
// #define   LED_PIN GPIO_NUM_21

extern float Electricity_ST;

#endif //_PHASE_SENSING_