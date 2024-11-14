#ifndef _CPU_CYCLE_COUNT_H
#define _CPU_CYCLE_COUNT_H

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

#define CPU_FRQ 2e9 // # hz

#define cycle2ms(time) printf("%.5f", 1.0 * time / CPU_FRQ * 1000 );

struct Timer {
    unsigned long begin_high, begin_low, end_high, end_low;
};

uint64_t calc_diff(struct Timer *timer) {
    uint64_t begin, end;
    begin = ((uint64_t) timer->begin_low) | (((uint64_t) timer->begin_high) << 32);
    end =  (uint64_t)(timer->end_high<<32) | timer->end_low;
    return end - begin;
}

// No function call, may incur more cost
// volatile: do not reuse result!
#define sync_tsc() asm volatile("cpuid" : : : "%rax", "%rbx", "%rcx", "%rdx");

#define start_timer(timer) \
  __asm__ volatile ("rdtsc" : "=a" ((timer)->begin_low), "=d" ((timer)->begin_high) : : "%rbx", "%rcx");

#define end_timer(timer) \
  __asm__ volatile ("rdtscp" : "=a" ((timer)->end_low), "=d" ((timer)->end_high) : : "%rbx", "%rcx");

#endif //_CPU_CYCLE_COUNT_H