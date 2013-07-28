/**
 * @addtogroup	FreeRTOS_Helper
 * Library for FreeRTOS helper/debugging functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/6
 * @copyright	BSDL
 ********************************************************************
 * @note		The Microblaze implementation needs a change in the
 * 				{projName}/lib/FreeRTOS/portable/GCC/MicroblazeV8/portasm.S
 * 				file where the _interrupt_handler should be renamed
 * 				to _interrupt_handler_FreeRTOS so that it can use interrupts
 * 				before the FreeRTOS scheduler starts
 */
#include <FreeRTOS.h>
#include <task.h>
#include <libMU/timer.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <driverlib/interrupt.h>

/**
 * FreeRTOS handler: Prototypes
 */
void vPortTickISR	(void* unused);
void xPortPendSVHandler( void ); /*__attribute__ (( naked )); */
void xPortSysTickHandler( void );
void vPortSVCHandler( void ); /*__attribute__ (( naked )); */
void vApplicationSetupTimerInterrupt(void);
void libMU_FreeRTOS_SetupInterruptVectors(void);
void vApplicationClearTimerInterrupt(void);
void vApplicationStackOverflowHook( xTaskHandle task, const char* taskName );
void vApplicationMallocFailedHook(void);

/**
 * Function called from vTaskStartScheduler() to initialize interrupt system and timer
 * @note	This function is called depending on the used architecture, i.e.,
 * 			ARM Cortex-M3 does not use this function (Systick timer used)
 * 			MicroblazeV8 uses this function
 * @note2	This functions is called with interrupts disabled
 */
void vApplicationSetupTimerInterrupt(void)
{

}

/**
 * Setup the required interrupt vectors for FreeRTOS
 * @note	This function should be called before vStartScheduler()
 */
void libMU_FreeRTOS_SetupInterruptVectors(void)
{
#if defined(__MICROBLAZE__)
	{
		extern void _interrupt_handler_FreeRTOS(void);
		volatile uint16_t*	handler_address = (uint16_t*)0x10;

		/* change interrupt handler */
		interrupts_disable();
		handler_address[0] = 0xB000;	/* IMM */
		handler_address[1] = ( ((uint32_t)_interrupt_handler_FreeRTOS)>>16 );
		handler_address[2] = 0xB808;	/* BRAI */
		handler_address[3] = ((uint32_t)_interrupt_handler_FreeRTOS) & 0xFFFF;
	}
	libMU_Timer_Initialize();
	libMU_Timer_SetFrequency( configTICK_RATE_HZ );
	libMU_Timer_SetHandler( vPortTickISR, NULL );
	libMU_Timer_StartAll();
#elif defined(ccs)
	IntRegister( FAULT_SYSTICK, xPortSysTickHandler );
	IntRegister( FAULT_PENDSV, 	xPortPendSVHandler );
	IntRegister( FAULT_SVCALL, 	vPortSVCHandler );
#endif
}

/**
 * Function called to clear timer interrupt request
 * @note	It can be empty if it is previously done in the interrupt handler
 */
void vApplicationClearTimerInterrupt(void)
{
}

/**
 * Function called when a task stack overflow occurs if
 * configCHECK_FOR_STACK_OVERFLOW > 0
 * @param	task		Task handle of current task
 * @param	taskName	Task name of current task
 */
void vApplicationStackOverflowHook( xTaskHandle task, const char* taskName )
{
	taskName++; taskName--;
	for(;;) {}
}

/**
 * Function called when a malloc() function call fails in FreeRTOS code if
 * configUSE_MALLOC_FAILED_HOOK > 0
 */
void vApplicationMallocFailedHook(void)
{
	xTaskHandle		task	 = xTaskGetCurrentTaskHandle();
	signed char*	taskName = pcTaskGetTaskName( task );
	taskName++; taskName--;
	for(;;) {}
}

/**
 * @}
 */
