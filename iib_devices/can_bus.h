
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAN_BUS_H__
#define CAN_BUS_H__

/////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#define INTERLOCK_MESSAGE_LEN               8
#define INTERLOCK_MESSAGE_OBJ_ID            1

#define ALARM_MESSAGE_LEN                   8
#define ALARM_MESSAGE_OBJ_ID                2

#define RESET_ITLK_MESSAGE_LEN              1
#define RESET_ITLK_MESSAGE_OBJ_ID           3

#define DATA_REQUEST_MESSAGE_LEN            4
#define DATA_REQUEST_MESSAGE_OBJ_ID         4

#define DATA_SEND_MESSAGE_LEN               8
#define DATA_SEND_OBJ_ID                    5

#define RECV_PARAM_MESSAGE_LEN              8
#define RECV_PARAM_MESSAGE_OBJ_ID           6

#define HEART_BEAT_MESSAGE_LEN              1
#define HEART_BEAT_MESSAGE_OB_ID            7

/////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {
    ItlkMsgId = 1,
    AlmMsgId,
    ResetMsgId,
    DataRequestMsgId,
    DataSendMsgId,
    ParamsSetMsgId,
    HeartBeatMsgId
}can_message_id_t;

/////////////////////////////////////////////////////////////////////////////////////////////

extern volatile uint32_t g_itlk_id;
extern volatile uint32_t g_alarm_id;

/////////////////////////////////////////////////////////////////////////////////////////////

extern void SendCanSchedule(void);
extern void SendCan(unsigned char Message);
extern void InitCan(uint32_t ui32SysClock);
extern void send_data_message(uint8_t var);
extern uint16_t get_can_address(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////
