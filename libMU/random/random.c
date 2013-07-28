/**
 * @addtogroup	libMU_Random
 * Random number generation library
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/5/28
 * @copyright	BSDL
 ********************************************************************
 */

#include <libMU/random.h>
#include <stdint.h>

/**
 * The pool of entropy that has been collected.
 */
static uint32_t g_pulRandomEntropy[16];

/**
 * The random number seed, which corresponds to the most recently returned
 * random number.  This is set based on the entropy-generated random number
 * by libMU_Random_Initialize().
 */
static uint32_t g_ulRandomSeed = 0u;

/**
 * Add 8 bit value to the entropy pool.
 * @param	ulEntropy	New entropy value
 * @note 				The pool consists of 16 uint32_t values
 */
void	libMU_Random_AddEntropy(uint8_t ulEntropy)
{
	/* The index of the next byte to be added to the entropy pool. */
	static uint32_t g_ulRandomIndex = 0u;
    /* Add this byte to the entropy pool. */
    ((uint8_t*)g_pulRandomEntropy)[g_ulRandomIndex] = ulEntropy;
    /* Increment to the next byte of the entropy pool */
    g_ulRandomIndex = (g_ulRandomIndex + 1U) & 63U;
}

#define F(a, b, c, d, k, s)		do{                                      			\
        ulTemp = (a) + ((d) ^ ((b) & ((c) ^ (d)))) + g_pulRandomEntropy[(k)];	\
        (a) = (ulTemp << (s)) | (ulTemp >> (32 - (s)));                 		\
    }while(0)

#define G(a, b, c, d, k, s)		do{												\
        ulTemp = (a) + (((b) & (c)) | ((b) & (d)) | ((c) & (d))) + g_pulRandomEntropy[(k)] + \
                 0x5a827999u;                                                 	\
        (a) = (ulTemp << (s)) | (ulTemp >> (32 - (s)));							\
    }while(0)

#define H(a, b, c, d, k, s)		do{                                    			\
        ulTemp = (a) + ((b) ^ (c) ^ (d)) + g_pulRandomEntropy[(k)] + 0x6ed9eba1u; \
        (a) = (ulTemp << (s)) | (ulTemp >> (32 - (s)));							\
    }while(0)

/**
 * Seed the random number generator by running a MD4 hash on the entropy pool.
 * Note that the entropy pool may change from beneath us, but for the purposes
 * of generating random numbers that is not a concern.  Also, the MD4 hash was
 * broken long ago, but since it is being used to generate random numbers
 * instead of providing security this is not a concern.
 */
void	libMU_Random_Initialize(void)
{
    uint32_t ulA, ulB, ulC, ulD, ulTemp, ulIdx;

    /* Initialize the digest. */
    ulA = 0x67452301u;
    ulB = 0xefcdab89u;
    ulC = 0x98badcfeu;
    ulD = 0x10325476u;

    /* Perform the first round of operations. */
    for(ulIdx = 0u; ulIdx < 16u; ulIdx += 4u)
    {
        F(ulA, ulB, ulC, ulD, ulIdx + 0u, 3);
        F(ulD, ulA, ulB, ulC, ulIdx + 1u, 7);
        F(ulC, ulD, ulA, ulB, ulIdx + 2u, 11);
        F(ulB, ulC, ulD, ulA, ulIdx + 3u, 19);
    }
    /* Perform the second round of operations. */
    for(ulIdx = 0u; ulIdx < 4u; ulIdx++)
    {
        G(ulA, ulB, ulC, ulD, ulIdx + 0u, 3);
        G(ulD, ulA, ulB, ulC, ulIdx + 4u, 5);
        G(ulC, ulD, ulA, ulB, ulIdx + 8u, 9);
        G(ulB, ulC, ulD, ulA, ulIdx + 12u, 13);
    }
    /* Perform the third round of operations. */
    for(ulIdx = 0U; ulIdx < 4U; ulIdx += 2U)
    {
        H(ulA, ulB, ulC, ulD, ulIdx + 0U, 3);
        H(ulD, ulA, ulB, ulC, ulIdx + 8U, 9);
        H(ulC, ulD, ulA, ulB, ulIdx + 4U, 11);
        H(ulB, ulC, ulD, ulA, ulIdx + 12U, 15);
        if(ulIdx == 2u)
        {
            ulIdx -= 3u;
        }
    }
    /* Use the first word of the resulting digest as the random number seed. */
    g_ulRandomSeed = ulA + 0x67452301u;
}

/**
 * Generate a new random number.  The number returned would more accurately be
 * described as a pseudo-random number since a linear congruence generator is
 * being used.
 */
uint32_t libMU_Random_GetValue(void)
{
    /* Generate a new pseudo-random number with a linear congruence random
     * number generator.  This new random number becomes the seed for the next
     * random number. */
    g_ulRandomSeed = (g_ulRandomSeed * 1664525u) + 1013904223u;

    /* Return the new random number. */
    return(g_ulRandomSeed);
}
