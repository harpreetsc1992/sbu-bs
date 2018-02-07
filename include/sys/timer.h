#ifndef _TIMER_H
#define _TIMER_H

#include <sys/isr.h>
#include <sys/io_ports.h>

#define MH_FREQ 1193180

extern uint32_t tick;
extern uint64_t sleep_time;

void
timer_callback(
              );

void
init_timer(
          );

void
sleep_shell(
            int secs
           );
#endif
