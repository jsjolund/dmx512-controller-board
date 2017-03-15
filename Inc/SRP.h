/*
 * RTFM_RSP.h
 *
 * Per Lindgren (c) 2013/2014
 *
 * Kernel primitives for the ARM Cortex M0 Architecture
 * Assuming c-code definitions
 * J_Code 	 	c-code for job J
 * R_Ceiling	resource ceiling for R
*/

#define Ceiling(R)		(R##_Ceiling)
#define Code(J)			(J##_Code)
#define IRQn(J) 		(J##_IRQn)
#define H(X)			((1 << __NVIC_PRIO_BITS)-X)
#define IRQh(J) 		(J##_IRQh)

#define BARRIER_LOCK 	{ asm volatile("dsb\n" "isb\n" ::: "memory"); }
#define BARRIER_UNLOCK 	{ asm volatile("" ::: "memory"); }
#define JOB_REQUEST(J)	{ NVIC_SetPendingIRQ(IRQn(J)); }

#define LOCK(R) { 							\
	int old_en = __get_BASEPRI(); 			\
	__set_BASEPRI_MAX((H(Ceiling(R)) << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL); 	\
	BARRIER_LOCK;

#define UNLOCK(R) 							\
	BARRIER_UNLOCK;							\
	__set_BASEPRI(old_en);	 				\
}

#define SETPRIO(J)		{ NVIC_SetPriority(IRQn(J), H(J##_Prio)); }
#define ENABLE(J)		{ NVIC_EnableIRQ(IRQn(J)); }

#define TASK(J)			void IRQh(J) ()

#define BREAKPOINT      { asm volatile("nop" ::: "memory"); }

