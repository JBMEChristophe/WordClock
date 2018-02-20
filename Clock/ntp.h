#ifndef NTP_H
#define NTP_H

#include <time.h>
#include <ESP8266WiFi.h>
#include "configuration.h"

/**
 * @brief      { configures the time so that the time can be retrieved  }
 *
 * @param[in]  timezone_hours        The timezone to which the Unix Time Epoch will be corrected
 * @param[in]  daylightOffset_hours  adds daylight saving time
 *
 * @return     { return a 0 on success }
 */
uint8_t ntp_init(float timezone_hours, uint8_t daylightOffset_hours);

/**
 * @brief      Gets the time.
 *
 * @param      reference point to a time struct
 *
 * @return     return 0 on sucess 1 on failure
 */
uint8_t ntp_getTime(struct* tm time)

#endif
