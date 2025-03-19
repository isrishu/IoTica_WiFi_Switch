#ifndef __MODULE_CONFIG_H__
#define __MODULE_CONFIG_H__

uint8_t packet_header[] = {0xAA, 0x55};
uint8_t packet_footer[] = {0x55, 0xAA};

enum {
    MODULE_BELL = 0,
    MODULE_SWITCH,
    MODULE_CURTAIN,
    MODULE_IR_BLASTER,
    MODULE_SMART_CUBE
};

char* modules[] = {"BELL", "SWITCH", "CURTAIN", "IR_BLASTER", "SMART_CUBE"};

#define HEADER_LEN 2
#define FOOTER_LEN 2
#define HEADER_FOOTER_LEN (HEADER_LEN + FOOTER_LEN)

#define MODULE_LEN 1
#define SWITCH_NO_LEN 1

#define SW_IN_MIN_PACKET_LEN 3
#define SW_IN_PACKET_LEN (HEADER_LEN + SW_IN_MIN_PACKET_LEN + FOOTER_LEN)
#define SW_OUT_MIN_PACKET_LEN 3
#define SW_OUT_PACKET_LEN (HEADER_LEN + SW_OUT_MIN_PACKET_LEN + FOOTER_LEN)

#define MASTER_SWITCH_NO 0
#define MASTER_NOS 1
#define TOGGLE_SW_NOS 8
#define FAN_NOS 2
#define DIMMER_NOS 4

#define SW_MASTER_OUT_PACKET_LEN (MODULE_LEN + SWITCH_NO_LEN + MASTER_NOS + TOGGLE_SW_NOS + FAN_NOS + DIMMER_NOS)

#define IN_PACKET_ARRAY_MAX SW_IN_PACKET_LEN
#define OUT_PACKET_ARRAY_MAX (HEADER_LEN + SW_MASTER_OUT_PACKET_LEN + FOOTER_LEN)

#define PACKET_STRING_MAX 64
#define PACKET_LEN 13


struct SwitchParams {
    uint8_t sw_no;
    uint8_t value;
};

struct ModuleParams {
    uint8_t module;
    void *param;
};

volatile ModuleParams inModuleParams = {0};
volatile ModuleParams outModuleParams = {0};

#endif