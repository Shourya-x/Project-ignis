#ifndef USB_MODULE_H
#define USB_MODULE_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t usb_rx_buffer[64];
extern volatile uint8_t usb_rx_len;

void USB_Init(void);
void USB_Print(const char *msg);
void USB_SendTelemetry(void);
void USB_ProcessCommands(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_MODULE_H */
