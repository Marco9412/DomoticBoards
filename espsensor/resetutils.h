
#ifndef RESET_UTILS
#define RESET_UTILS

/*
  
enum rst_reason {
  REASON_DEFAULT_RST    = 0,  // normal startup by power on
  REASON_WDT_RST      = 1,  // hardware watch dog reset
  REASON_EXCEPTION_RST  = 2,  // exception reset, GPIO status won’t change
  REASON_SOFT_WDT_RST     = 3,  // software watch dog reset, GPIO status won’t change
  REASON_SOFT_RESTART   = 4,  // software restart ,system_restart , GPIO status won’t change
  REASON_DEEP_SLEEP_AWAKE = 5,  // wake up from deep-sleep
  REASON_EXT_SYS_RST      = 6   // external system reset
}; 

*/

inline void espDeepSleepMs(int milliseconds) {
  system_deep_sleep(milliseconds * 1000);
}

inline boolean isAwakeFromDeepSleep() {
  volatile const rst_info * resetInfo = system_get_rst_info();
  return resetInfo->reason == 5;
}

#endif
