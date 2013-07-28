/**
 * @addtogroup	libMU_Internet
 * Librería para el uso sencillo de internet
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 * @note
 * Añadir nuevos protocolos no es trivial (basado en la librería uIP)
 * Los procotolos soportados son:
 * - DHCP (solo en reset, no hay posibilidad de reconectar a una nueva red)
 * - DNS (se guardan un máximo de 10 direcciones)
 * - UDP (3 conecxiones como máximo)
 * - TCP (3 conecxiones como máximo)
 * - Cliente HTTP
 * - Servidor HTTP
 * - Cliente telnet mínimo (soporte parcial)
 * - Protocolo Siemens TCP/IP
 */

#ifndef LIBMU_INTERNET_ES_H_
#define LIBMU_INTERNET_ES_H_

#include <libMU/internet.h>

/**
 * Tipo de dato que se usa para guardar las direcciones IPv4
 */
#define IPDir_t 	IPAddress_t

/**
 * Puntero a la función para el tratamiento de datos descargados de internet.
 * @param	data	Puntero a los datos descargados
 * @param	len		Tamaño de los datos descargados
 * @note
 * - Debido a limitaciones de memoria, la página descargada no se almacena y hay
 *   que procesarla según llegan los datos.
 * - Esta función se llamará varias veces por cada página excepto para aquellas
 *   páginas que tengan un tamaño menor de 1kB (aprox.).
 * @see		libMU_Internet_LeerPagina()
 */
typedef libMU_Internet_PageReaderFunction_t libMU_Internet_FuncionLecturaPagina_t;

/**
 * Puntero a la función para el tratamiento de formularios con metodo (GET/POST)
 * @param	cmd_param	Parámetros del comando GET/POST
 * @param	len			Puntero a un entero donde se guarda la longitud de datos de la respuesta
 * @return				Puntero al texto con el que tiene que responder el servidor web
 * @note
 * - Si se devuelve NULL el servidor no responde nada
 * @see		libMU_Internet_Servidor_FijarInformacionDeProcesoComandos()
 */
typedef libMU_Internet_CommandProcessingFunction_t libMU_Internet_FuncionProcesamientoComandos_t;

/**
 * Puntero a la función principalque se ha de ejecutar en paralelo con las tareas de comunicación
 * @param	param		Puntero genérico a datos (no se usa)
 */
typedef libMU_Task_t libMU_Tarea_t;

/**
 * Inicializa la libreria de comunicaciones de internet
 * @param	tarea	Puntero a la función de tarea que se ejecuta junto a la tarea de procesamiento de la pila TCP/IP
 * @note
 * - Se usa FreeRTOS para crear y ejecutar las tareas
 *   > Se configura la rutina de servicion de interrupciones de Ethernet para recibir los paquetes
 *   > Se crea la tarea de procesamiento de la pula TCP/IP
 *   > Se crea la tarea 'tarea'
 *   > Arranca el sistema FreeRTOS
 * - Esta función nunca debería terminar.
 *   Termina únicamente en el caso de que FreeRTOS no pudo empezar
 *   (problemas de reserva de memoria)
 */
static inline void libMU_Internet_Inicializa(libMU_Tarea_t tarea)
{
	return libMU_Internet_Initialize(tarea);
}

/**
 * @return	Estado de la conexión
 * @note
 * Valores devueltos: NETWORK_IP_MISSING, NETWORK_IP_OK, NETWORK_ERROR
 */
static inline int libMU_Internet_ObtenEstado(void)
{
	return libMU_Internet_GetStatus();
}

/**
 * @return	IP local configurada
 */
static inline IPDir_t libMU_Internet_ObtenIPDispositivo(void)
{
	return libMU_Internet_GetDeviceIP();
}

/**
 * @param	unidades_de_tiempo	Tiempo a esperar en ms
 */
static inline void libMU_Internet_Retardo(unsigned long unidades_de_tiempo)
{
	libMU_Internet_Delay(unidades_de_tiempo);
}

/**
 * @return	Número de tramas IP recibidas
 */
static inline int libMU_Internet_ObtenNumeroDeTramasRecibidas(void)
{
	return libMU_Internet_GetNumberOfFramesReceived();
}

/**
 * @return	Número de tramas IP enviadas
 */
static inline int libMU_Internet_ObtenNumeroDeTramasEnviadas(void)
{
	return libMU_Internet_GetNumberOfFramesSent();
}

/**
 * Obtiene la dirección IP del un nodo de internet usado DNS
 * @param	host		Nombre del dispositivo del cual se quiere obtener la dirección IP
 * @param	ip			Dirección de la variable donde queremos guardar la dirección IP
 * @param	timeout_ms	Tiempo de espera máximo en milisegundos
 * @return				Devuelve 1 si se procesa correctamente.
 */
static inline int	libMU_Internet_ObtenIP(const char *host, IPDir_t* ip, int timeout_ms )
{
	return libMU_Internet_DNS_resolution(host,ip,timeout_ms);
}

/**
 * Lee una pagina de internet y llama a una función para ir tratando la página
 * @param	url			Dirección URL del cual se quiere obtener la página
 * @param	funcion		Función que se llama por cada trozo de página que se descarga
 * @return				Devuelve 1 si se inicia la conexión.
 * @note
 * Esta función no es capaz de leer la página completa por limitaciones de memoria.
 */
static inline int	libMU_Internet_GetPage( char *url, libMU_Internet_FuncionLecturaPagina_t funcion )
{
	return libMU_Internet_GetPage(url,funcion);
}

/**
 * Indica si ha terminado de leer la página pedida
 * @return	Devuelve verdadero si ha terminado de procesar la pagina
 * @note
 * Esta función sirve para descubrir si se ha terminado de leer la página pedida usando
 * la función libMU_Internet_GetPage()
 */
static inline int	libMU_Internet_LecturaPaginaTerminada( void )
{
	return libMU_Internet_IsPageReadingFinished();
}

/**
 * @return	Devuelve el primer byte de la dirección IP
 */
#define libMU_IP_1_		libMU_IP_1

/**
 * @return	Devuelve el segundo byte de la dirección IP
 */
#define libMU_IP_2_		libMU_IP_2

/**
 * @return	Devuelve el tercer byte de la dirección IP
 */
#define libMU_IP_3_		libMU_IP_3

/**
 * @return	Devuelve el cuarto byte de la dirección IP
 */
#define libMU_IP_4_		libMU_IP_4

/**
 * Guarda los datos con los que tiene que responder el servidor web (en el puerto 80)
 * si se envia un comando GET con el tag especificado 
 * @param	tag			Nombre con el que se identifica la petición
 * @param	datos		Datos con los que hay que responder (si es NULL borra el tag correspondiente)
 * @param	len			Longitud de los datos de la respuesta (si es 0 borra el tag correspondiente)
 * @return				Devuelve si ha podido guardar la respuesta
 */
static inline int libMU_Internet_Servidor_FijarPagina( char* tag, char* datos, int len )
{
	return libMU_Internet_Server_SetPage(tag,datos,len);
}

/**
 * Especifica la funcion para procesar datos de un formulario que se reciben en el servidor web (en el puerto 80)
 * si se envia un comando GET/POST con el tag especificado
 * @param	tag			Nombre con el que se identifica la petición
 * @param	funcion		Funcion de procesamiento de datos del formulario
 * @return				Devuelve si ha podido guardar la respuesta
 */
static inline int libMU_Internet_Servidor_FijarInformacionDeProcesoComandos( char* tag, libMU_Internet_FuncionProcesamientoComandos_t funcion )
{
	return libMU_Internet_Server_SetCommandProcessingInfo(tag,funcion);
}

/**
 * Interpreta la secuencia de parámetros enviada desde un formulario al servidor
 * para buscar un valor entero para un parámetro especificado
 * @param	cmd_params	Secuencia de parámetros recibida en el servidor
 * @param	param_tag	Parámetro especifico que se busca (p.e., "x=")
 * @param	value		Ubicación donde se guardará el valor en caso de que se encuentre el parámetro
 * @return				Devuelve 1 si se ha encontrado el parámetro
 */
static inline int libMU_Internet_ObtenValorEnteroDelFormulario( const char* cmd_params, const char* param_tag, int* value )
{
	return libMU_Internet_GetFormIntegerValue(cmd_params,param_tag,value);
}

/**
 * Interpreta la secuencia de parámetros enviada desde un formulario al servidor
 * para buscar una cadena de caracteres para un parámetro especificado
 * @param	cmd_params	Secuencia de parámetros recibida en el servidor
 * @param	param_tag	Parámetro especifico que se busca (p.e., "x=")
 * @param	value		Ubicación donde se guardará el valor en caso de que se encuentre el parámetro
 * @param	value_size	Tamaño máximo de datos que admite el parámetro
 * @return				Devuelve 1 si se ha encontrado el parámetro
 */
static inline int libMU_Internet_ObtenValorCadenaDelFormulario( const char* cmd_params, const char* param_tag, char* value, int value_size )
{
	return libMU_Internet_GetFormStringValue(cmd_params,param_tag,value,value_size);
}

/**
 * Obtener la dirección IP del router WiFi usando el protocolo telnet
 * @return	Dirección IP del router WiFi
 * @note 	Esta función tarda 15 segundos en volver en el peor de los casos
 */
static inline IPDir_t	libMU_Internet_ObtenIPRouterWiFi( void )
{
	return libMU_Internet_GetWiFiRouterIP();
}

#endif /*LIBMU_INTERNET_ES_H_*/
/**
 * @}
 */
