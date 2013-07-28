/**
 * Selección del demo
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>

/**
 * Function prototypes
 */
int main(void);
extern	void main_demo_01(void);	/* Buttons & LED (+ Display) */
extern	void main_demo_02(void);	/* Analog-Digital Converter and Temperature measurements */
extern	void main_demo_03(void);	/* PWM demo */
extern	void main_demo_04(void);	/* Serial communications */
extern	void main_demo_04c(void);	/* Serial communications with interrupts */
extern	void main_demo_05(void);	/* Timer demo */
extern	void main_demo_06(void);	/* LCD screen graphical demo */
extern	void main_demo_06b(void);	/* LCD screen menu demo */
extern	void main_demo_06c(void);	/* LCD screen font management demo */
extern	void main_demo_07(void);	/* FreeRTOS demo */
extern	void main_demo_07b(void);	/* FreeRTOS demo + Thread Safe serial communications */
extern	void main_demo_08(void);	/* Music demo */
extern	void main_demo_09(void);	/* Internet client and server demo */
extern  void main_demo_glados_client(void);

int main(void)
{
	static int res = 0;
//	main_demo_01();		/* Buttons & LED (+ Display) */
//	main_demo_02();		/* Analog-Digital Converter and Temperature measurements */
//	main_demo_03();		/* PWM demo */
//	main_demo_04();		/* Serial communications */
//	main_demo_05();		/* Timer demo */
//	main_demo_06();		/* LCD screen graphical demo */
//	main_demo_06b();	/* LCD screen menu demo */
//	main_demo_07();		/* FreeRTOS demo */
//	main_demo_08();		/* Music demo */
//	main_demo_09();		/* Internet client and server demo */
	main_demo_glados_client();
	return res;
}
