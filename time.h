#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)

typedef struct  {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Wday;   // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;   // offset from 1970;
} tmElements_t;

void time_init(void);
unsigned long millis();
unsigned long makeTime(tmElements_t &tm);
unsigned long now();
void setTime(unsigned long t);
//tmElements_t timeFromStamp(unsigned long timeInput);
tmElements_t getTimeOnly();

#ifdef __cplusplus
}
#endif
