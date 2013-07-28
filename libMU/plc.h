/**
 * @addtogroup	libMU_PLC
 * Library for easy access to PLC data
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 * @note
 * This library requires the libMU_Internet library to be initialized
 */
#ifndef PLC_H_
#define PLC_H_

#include <stdint.h>
#include <uip.h>
#include <uip_app/siemensTCP_app.h>

/**
 * PLC transfer data information
 */
typedef req_transfer_data_t libMU_PLC_Transfer_t;
typedef siemensTCP_Function_t libMU_PLC_Function_t;

/**
 * Union to read different data types using the same pointer (see use below)
 */
typedef union _pointer_t {
	uint8_t		u8;
	uint16_t	u16;
	uint32_t	u32;
	uint64_t	u64;
	int8_t		i8;
	int16_t		i16;
	int32_t		i32;
	int16_t		i64;
	float		f32;
	double		f64;
} libMU_PLC_pointer_t;

/**
 * Simula la lectura de un uint8_t desde una variable del PLC
 * y se envia a un servidor Siemens TCP/IP
 * @param	data	PLC transfer data
 * @param	address	address of PLC variable (MB8)
 * @param	value	value of PLC variable to send
 * @return			1 if error, 0 if ok
 */
static __inline int	libMU_PLC_Send_MB(libMU_PLC_Transfer_t* data, int address, uint8_t value)
{
	int data_end = data->address + data->num_bytes;   
	int end = address + sizeof(uint8_t);   
	if( data->address <= address && data_end >= end ) {
		uint8_t* p = (uint8_t*)(data->data - data->address + address);
		*p = value;
		return 0;
	}	
	return 1;
}

/**
 * Simula la lectura de un uint16_t desde una variable del PLC
 * y se envia a un servidor Siemens TCP/IP
 * @param	data	PLC transfer data
 * @param	address	address of PLC variable (MW8)
 * @param	value	value of PLC variable to send
 * @return			1 if error, 0 if ok
 * @note
 * The data is an special variable
 */
static __inline int	libMU_PLC_Send_MW(libMU_PLC_Transfer_t* data, int address, uint16_t value)
{
	int data_end = data->address + data->num_bytes;   
	int end = address + sizeof(uint16_t);   
	if( data->address <= address && data_end >= end ) {
		uint16_t* p = (uint16_t*)(data->data - data->address + address);
		*p = htons(value);
		return 0;
	}	
	return 1;	
}

/**
 * Simula la lectura de un uint32_t desde una variable del PLC
 * y se envia a un servidor Siemens TCP/IP
 * @param	data	PLC transfer data
 * @param	address	address of PLC variable (MD8)
 * @param	value	value of PLC variable to send
 * @return			1 if error, 0 if ok
 */
static __inline int	libMU_PLC_Send_MD(libMU_PLC_Transfer_t* data, int address, uint32_t value)
{
	int data_end = data->address + data->num_bytes;   
	int end = address + sizeof(uint32_t);   
	if( data->address <= address && data_end >= end ) {
		uint16_t* p = (uint16_t*)(data->data - data->address + address);
		/* *p = htons32(value);*/
		p[0] = htons(value >> 16);
		p[1] = htons(value);
		return 0;
	}	
	return 1;
}

/**
 * Simula la recepcion de un bit a una variable del PLC (simulada)
 * desde un servidor Siemens TCP/IP
 * @param	data	PLC receive data
 * @param	address	address of PLC variable (MB8)
 * @param	value	value of PLC variable received
 * @return			1 if error, 0 if ok
 */
static __inline int	libMU_PLC_Receive_Bit( const libMU_PLC_Transfer_t* data, int address, int bit, uint8_t* value);
static __inline int	libMU_PLC_Receive_Bit( const libMU_PLC_Transfer_t* data, int address, int bit, uint8_t* value)
{
	if( data->num_bit == bit && data->address == address && data->num_bytes > 0 ) {
/*		if( data->data[0] ) *value |= (1<<bit);
		else				*value &= (1<<bit); */
		*value = data->data[0];
		return 0;
	}	
	return 1;
}

/**
 * Simula la recepcion de un uint8_t a una variable del PLC (simulada)
 * desde un servidor Siemens TCP/IP
 * @param	data	PLC receive data
 * @param	address	address of PLC variable (MB8)
 * @param	value	value of PLC variable received
 * @return			1 if error, 0 if ok
 */
static __inline int	libMU_PLC_Receive_MB( const libMU_PLC_Transfer_t* data, int address, uint8_t* value);
static __inline int	libMU_PLC_Receive_MB( const libMU_PLC_Transfer_t* data, int address, uint8_t* value)
{
	if( data->num_bit > 7 && data->address == address && data->num_bytes == sizeof(uint8_t) ) {
		uint8_t* p = (uint8_t*)(data->data);
		*value = *p;
		return 0;
	}	
	return 1;
}

/**
 * Simula la recepcion de un uint16_t a una variable del PLC (simulada)
 * desde un servidor Siemens TCP/IP
 * @param	data	PLC receive data
 * @param	address	address of PLC variable (MW8)
 * @param	value	value of PLC variable received
 * @return			1 if error, 0 if ok
 */
static __inline int	libMU_PLC_Receive_MW( const libMU_PLC_Transfer_t* data, int address, uint16_t* value);
static __inline int	libMU_PLC_Receive_MW( const libMU_PLC_Transfer_t* data, int address, uint16_t* value)
{
	if( data->num_bit > 7 && data->address == address && data->num_bytes == sizeof(uint16_t) ) {
		uint16_t* p = (uint16_t*)(data->data);
		*value = ntohs(*p);
		return 0;
	}	
	return 1;
}

/**
 * Simula la recepcion de un uint32_t a una variable del PLC (simulada)
 * desde un servidor Siemens TCP/IP
 * @param	data	PLC receive data
 * @param	address	address of PLC variable (MD8)
 * @param	value	value of PLC variable received
 * @return			1 if error, 0 if ok
 */
static __inline int	libMU_PLC_Receive_MD( const libMU_PLC_Transfer_t* data, int address, uint32_t* value);
static __inline int	libMU_PLC_Receive_MD( const libMU_PLC_Transfer_t* data, int address, uint32_t* value)
{
	if( data->num_bit > 7 && data->address == address && data->num_bytes == sizeof(uint32_t) ) {
		uint16_t* val = (uint16_t*)value;
		uint16_t* p = (uint16_t*)(data->data);
		val[0] = ntohs(p[0]);
		val[1] = ntohs(p[1]);
		return 0;
	}	
	return 1;
}

/**
 * Set the callback function that is called whenever a new data
 * frame is received from the PLC automaton using the Siemens TCP/IP
 * protocol
 * @param	function	Pointer to callback function
 * @note
 * The prototype for the callback function should be something like
 * void PLC_ReceiveCallbackFunction(libMU_PLC_Transfer_t* tr);
 */
static __inline void libMU_PLC_SetReceiveFunction(libMU_PLC_Function_t function)
{
	siemensTCP_set_receive_function(function);
}

/**
 * Set the callback function that is called whenever a new data
 * frame has to be sent to the PLC automaton using the Siemens TCP/IP
 * protocol
 * @param	function	Pointer to callback function
 * @note
 * The prototype for the callback function should be something like
 * void PLC_TransmitCallbackFunction(libMU_PLC_Transfer_t* tr);
 */
static __inline void libMU_PLC_SetTransmitFunction(libMU_PLC_Function_t function)
{
	siemensTCP_set_transmit_function(function);
}

#endif /*PLC_H_*/
/**
 * @}
 */
