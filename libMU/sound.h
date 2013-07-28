/**
 * @addtogroup libMU_Sound
 * Library for sound management functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 * @note		The sound management is done using a FreeRTOS helper task
 */
#ifndef	_LIBMU_SOUND_H_
#define	_LIBMU_SOUND_H_

/**
 * Note frequency constants for C4-B4 octave
 * @see http://www.phy.mtu.edu/~suits/notefreqs.html
 */
typedef enum _libMU_Sound_NoteFrequency_t {
	DO			= 262,	/* 1 */
	DO_SOST		= 277,
	RE_BEMOL	= 277,
	RE			= 294,	/* 3 */
	RE_SOST		= 311,
	MI_BEMOL	= 311,
	MI			= 330,	/* 5 */
	FA			= 349,	/* 6 */
	FA_SOST		= 370,
	SOL_BEMOL	= 370,
	SOL			= 392,	/* 8 */
	SOL_SOST	= 415,
	LA_BEMOL	= 415,
	LA			= 440,	/* 10 */
	LA_SOST		= 466,
	SI_BEMOL	= 466,
	SI			= 494,	/* 12 */
	C4 = DO,
	D4 = RE,
	E4 = MI,
	F4 = FA,
	G4 = SOL,
	A4 = LA,
	B4 = SI,
	SILENCE		= 0
} libMU_Sound_NoteFrequency_t;

/**
 * Score entry for a single note (can also be a silence)
 */
typedef struct _libMU_Sound_Note_t {
	libMU_Sound_NoteFrequency_t 	note;		/* Frequency of the note */
	uint32_t						time;		/* Duration in ms */
} libMU_Sound_Note_t;

/**
 * Score for a song/music
 */
typedef	struct _libMU_Sound_Score_t {
	libMU_Sound_Note_t*	song;		/* Notes of the score */
	uint32_t			num_notes;	/* Number of notes in the score */
} libMU_Sound_Score_t;

/**
 * Global variables
 */
extern uint8_t		libMU_Sound_Playing;	/* Playing */
extern uint32_t		libMU_Sound_Repeat;		/* Number of repetitions */

/**
 * Initialize the sound library
 * @note	This library uses the FreeRTOS operating system to generate the music timings
 */
void	libMU_Sound_Initialize(void);

/**
 * Set the song to be played
 * @param notes		Pointer to note frequencies and durations array
 * @param num_notes	Number of notes in the previous score
 */
void	libMU_Sound_SetSong( libMU_Sound_Note_t* notes, uint32_t num_notes );

/**
 * Set volume
 * @param volume	Volume of the sound to be generated (in dB)
 * @note 			The volume variable is logarithmic
 */
void	libMU_Sound_SetVolume(uint32_t volume);

#endif/*_LIBMU_SOUND_H_ */
/**
 * @}
 */
