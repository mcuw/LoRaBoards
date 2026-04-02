#ifndef LG_NTC_H
#define LG_NTC_H

#ifdef HAS_NTC
float getTempForNTC();

static uint32_t calibrate_one(rtc_cal_sel_t cal_clk, const char *name);
static void enable_slow_clock();

#endif


#endif /* LG_NTC_H */