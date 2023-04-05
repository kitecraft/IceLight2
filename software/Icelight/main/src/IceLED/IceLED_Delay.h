#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"


/// Class to ensure that a minimum amount of time has kicked since the last time run - and delay if not enough time has passed yet
/// this should make sure that chipsets that have
template<int WAIT> class CMinWait {
	uint64_t mLastMicros;
public:
	CMinWait() { mLastMicros = 0; }

	void wait() {
		// how long I been waiting
		uint64_t waited = esp_timer_get_time() - mLastMicros;
		// fast path, waited long enough
		if (waited >= WAIT) return;
		// delay vs spin
		if ((WAIT - waited) > (portTICK_PERIOD_MS * 1000)) {
			int tickDelay = ((WAIT - waited) / 1000) / portTICK_PERIOD_MS;
			//printf("cMinWait: %llu micros means delay %d ticks\n",(WAIT - waited),tickDelay);
			vTaskDelay( tickDelay );
		}
		else { /*buzz is only other option outch */
			do {
				waited = esp_timer_get_time() - mLastMicros;
			} while( waited > WAIT);
		}
	}

	void mark() { mLastMicros = esp_timer_get_time(); }
};


////////////////////////////////////////////////////////////////////////////////////////////
//
// Clock cycle counted delay loop
//
////////////////////////////////////////////////////////////////////////////////////////////

// Default is now just 'nop', with special case for AVR

// ESP32 core has it's own definition of NOP, so undef it first

#undef NOP
#undef NOP2
#  define FL_NOP __asm__ __volatile__ ("nop\n");
#  define FL_NOP2 __asm__ __volatile__ ("nop\n\t nop\n");



// predeclaration to not upset the compiler
template<int CYCLES> inline void delaycycles();
template<int CYCLES> inline void delaycycles_min1() {
  delaycycles<1>();
  delaycycles<CYCLES-1>();
}


template<int CYCLES> __attribute__((always_inline)) inline void delaycycles() {
	// _delaycycles_ARM<CYCLES / 3, CYCLES % 3>();
	FL_NOP; delaycycles<CYCLES-1>();
}

// pre-instantiations for values small enough to not need the loop, as well as sanity holders
// for some negative values.
template<> __attribute__((always_inline)) inline void delaycycles<-10>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-9>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-8>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-7>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-6>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-5>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-4>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-3>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-2>() {}
template<> __attribute__((always_inline)) inline void delaycycles<-1>() {}
template<> __attribute__((always_inline)) inline void delaycycles<0>() {}
template<> __attribute__((always_inline)) inline void delaycycles<1>() {FL_NOP;}
template<> __attribute__((always_inline)) inline void delaycycles<2>() {FL_NOP2;}
template<> __attribute__((always_inline)) inline void delaycycles<3>() {FL_NOP;FL_NOP2;}
template<> __attribute__((always_inline)) inline void delaycycles<4>() {FL_NOP2;FL_NOP2;}
template<> __attribute__((always_inline)) inline void delaycycles<5>() {FL_NOP2;FL_NOP2;FL_NOP;}

// Some timing related macros/definitions

// Macro to convert from nano-seconds to clocks and clocks to nano-seconds
// #define NS(_NS) (_NS / (1000 / (F_CPU / 1000000L)))
#define F_CPU_MHZ (F_CPU / 1000000L)

// #define NS(_NS) ( (_NS * (F_CPU / 1000000L))) / 1000
#define NS(_NS) (((_NS * F_CPU_MHZ) + 999) / 1000)
#define CLKS_TO_MICROS(_CLKS) ((long)(_CLKS)) / (F_CPU / 1000000L)

//  Macro for making sure there's enough time available
#define NO_TIME(A, B, C) (NS(A) < 3 || NS(B) < 3 || NS(C) < 6)
