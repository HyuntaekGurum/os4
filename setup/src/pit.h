#ifndef __PIT_H__
#define __PIT_H__

#include <stdint.h>

void pit_mwait(uint16_t ms);
void pit_uwait(uint16_t us);

#endif /* __PIT_H__ */
