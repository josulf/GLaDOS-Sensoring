/**
 * @addtogroup libMU_Sound
 * Libreria para la generación de sonidos
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef	LIBMU_SONIDO_H_
#define	LIBMU_SONIDO_H_

/**
 * Frecuencia de las notas en la octava C4-B4
 * @see http://www.phy.mtu.edu/~suits/notefreqs.html
	DO			= 262,
	DO_SOST		= 277,
	RE_BEMOL	= 277,
	RE			= 294,
	RE_SOST		= 311,
	MI_BEMOL	= 311,
	MI			= 330,
	FA			= 349,
	FA_SOST		= 370,
	SOL_BEMOL	= 370,
	SOL			= 392,
	SOL_SOST	= 415,
	LA_BEMOL	= 415,
	LA			= 440,
	LA_SOST		= 466,
	SI_BEMOL	= 466,
	SI			= 494,
	C4 = DO,
	D4 = RE,
	E4 = MI,
	F4 = FA,
	G4 = SOL,
	A4 = LA,
	B4 = SI,
	SILENCE		= 0
*/
typedef libMU_Sound_NoteFrequency_t libMU_Sonido_FrecuenciasNota_t;

/**
 * Información de una nota para partituras (puede ser también un silencio)
 */
typedef struct _libMU_Sonido_Nota_t {
	libMU_Sonido_Note_t 	note;		/* Frequencia de la nota */
	uint32_t				time;		/* Duración in ms */
};
typedef libMU_Sound_Note_t libMU_Sonido_Nota_t;

/**
 * Partitura de una canción (sin harmonicos)
 */
typedef	struct _libMU_Sonido_Partitura_t {
	libMU_Sonido_Nota_t*	song;		/* Notas de la partitura */
	uint32_t				num_notes;	/* Número de notas en la partitura */
};
typedef libMU_Sound_Score_t libMU_Sonido_Partitura_t;

/**
 * Global variables
 */
#define libMU_Sonido_Reproduciendo	libMU_Sonido_Playing	/* Reproduciendo */
#define	libMU_Sonido_Repeticiones	libMU_Sonido_Repeat		/* Número de repeticiones */

/**
 * Initialize the sound library
 * @note	This library uses the FreeRTOS operating system to generate the music timings
 */
void	libMU_Sonido_Initialize(void);

/**
 * Set the song to be played
 * @param	notes		Pointer to note frequencies and durations array
 * @param	num_notes	Number of notes in the previous score
 */
void	libMU_Sonido_SetSong( libMU_Sonido_ScoreNote_t* notes, uint32_t num_notes );


#endif/*LIBMU_SONIDO_H_*/
/**
 * @}
 */
