/****************************************************************************
 * Copyright 2019 D. Breunig, Fraunhofer IPA                                *
 *																			*
 * Licensed under the Apache License, Version 2.0 (the "License");			*
 * you may not use this file except in compliance with the License.			*
 * You may obtain a copy of the License at									*
 *																			*
 *		http://www.apache.org/licenses/LICENSE-2.0							*
 *																			*
 * Unless required by applicable law or agreed to in writing, software		*
 * distributed under the License is distributed on an "AS IS" BASIS,		*
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.	*
 * See the License for the specific language governing permissions and		*
 * limitations under the License.											*
 ****************************************************************************/

/**
* @file libMsbClientC.h
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief Main header file for Msb C Client library
*/

#ifndef LIBMSBCLIENTC_H_
#define LIBMSBCLIENTC_H_

#ifndef _WIN32
#include <pthread.h>
#else
#include <WinSock2.h>
#include <windows.h>
#include <strsafe.h>
#include <objbase.h>
#endif // !_WIN32

#include <stdint.h>
#include <stdbool.h>

#include "json-c/json.h"

#include <stdio.h>
#include <math.h>

/**
 * @brief return value enum for linked list
 */
typedef enum schll_return_value {
	SCHLL_OK,/**< operation ok */
    SCHLL_SCHLL_EXISTS,/**< don't even know */
    SCHLL_MAP_NULL,/**< pointer to linked list is null */
    SCHLL_ALLOC_FAIL,/**< allocation failed */
    SCHLL_NAME_MISSING,/**< name is missing */
    SCHLL_NO_ENTRY,/**< no such entry */
    SCHLL_NO_ENTRIES/**< no entries at all */
} schll_return_value;

/**
 * @brief entry structure for linked list
 */
typedef struct schll_entry {
	char* name;/**< name of entry. */
	size_t name_l;/**< length of entry name */
	void* value;/**< pointer to value */

	struct schll_entry* next_entry;/**< pointer to next entry */
} schll_entry;

/**
 * @brief linked list structure
 */
typedef struct schll_list {
	struct schll_entry* first_entry;/**< pointer to first entry */
	unsigned int number_entries;/**< number of entries in the list */
} schll_list;

/**
 * @brief List function for creating a linked list
 *
 * @return Created list
 */
schll_list* schll_MakeListPointer();

/**
 * @brief Internal function for creating a linked list on a pointer
 *
 * @param list Pointer to a linked list pointer
 *
 * @return linked list operation value
 */
schll_return_value schll_MakeList(schll_list** list);

/**
 * @brief Internal function for deleting a linked list
 *
 * @param list Pointer to a linked list pointer
 * @param free_names flag whether names of the linked list have to be free'd or not
 * @param free_values flag whether values of the linked list have to be free'd or not
 *
 * @return linked list operation value
 */
schll_return_value schll_DeleteList(schll_list** list, int free_names, int free_values);

/**
 * @brief Internal function for adding an entry to a linked list
 *
 * @param list Pointer to a list
 * @param name Name of the new entry
 * @param copy_name flag whether the name string has to be copied or not
 * @param value Pointer to value
 * @param copy_value flag whether the value has to be copied or not
 * @param value_size size of the value, necessary if it has to be copied
 *
 * @return linked list operation value
 */
schll_return_value schll_addEntry(schll_list* list, const char* name, int copy_name,
							  const void* value, int copy_value, size_t value_size);
/**
 * @brief Internal function for removing an entry from a linked list
 *
 * @param list Pointer to a list
 * @param name Name of the entry
 * @param free_name flag whether the name string has to be free'd or not
 * @param free_value flag whether the value has to be free'd or not
 *
 * @return linked list operation value
 */
schll_return_value schll_removeEntry(schll_list* list, const char* name, int free_name, int free_value);

/**
 * @brief Internal function for getting a value from a linked list
 *
 * @param list Pointer to a list
 * @param value Pointer to put value into
 * @param name Name of the entry
 *
 * @return linked list operation value
 */
schll_return_value schll_getValue(const schll_list* list, void** value, const char* name);

/**
 * @brief Internal function for getting a pointer to a value from a linked list
 *
 * @param list Pointer to a list
 * @param name Name of the entry
 *
 * @return pointer to value
 */
void* schll_getValuePointer(const schll_list* list, const char* name);

/**
 * @brief return value enum for list
 */
typedef enum list_return_value {
	EINFL_OK,/**< operation ok */
    EINFL_LIST_NULL,/**< pointer to list is null */
    EINFL_NO_ENTRY,/**< no such entry */
    EINFL_NO_ENTRIES,/**< no entries at all */
    EINFL_NULL_VALUE/**< pointer to value of entry is null */
} list_return_value;

/**
 * @brief entry structure for list
 */
typedef struct list_entry {
    void* value;/**< pointer to value */
	struct list_entry* next_entry;/**< pointer to next entry */
} list_entry;

/**
 * @brief list structure
 */
typedef struct list_list {
	struct list_entry* first_entry;/**< pointer to first entry */
	unsigned int number_entries;/**< number of entries in the list */
	pthread_mutex_t mutex;/**< mutex for safe data access and exchange */
} list_list;

/**
 * @brief List function for creating a list
 *
 * @return Created list
 */
list_list* list_makeListPointer();

/**
 * @brief Internal function for creating a list on a pointer
 *
 * @param list Pointer to a list pointer
 *
 * @return list operation value
 */
list_return_value list_makeList(list_list** list);

/**
 * @brief Internal function for deleting a list
 *
 * @param list Pointer to a list
 * @param free_value flag whether values of the list have to be free'd or not
 *
 * @return list operation value
 */
list_return_value list_deleteList(list_list* list, int free_value);

/**
 * @brief Internal function for adding a value to a list
 *
 * @param list Pointer to a list
 * @param value Pointer to value
 * @param copy_value flag whether values of the list have to be free'd or not
 * @param value_size size of the value
 *
 * @return list operation value
 */
list_return_value list_add(list_list* list, void* value, int copy_value, size_t value_size);

/**
 * @brief Internal function for popping a value from a list
 *
 * @param list Pointer to a list
 * @param index index of the value
 * @param free_value flag whether the value has to be free'd or not
 *
 * @return list operation value
 */
list_return_value list_pop(list_list* list, unsigned int index, int free_value);

/**
 * @brief Internal function for getting a value from a list
 *
 * @param list Pointer to a list
 * @param value pointer in which the value shall be put
 * @param index index of the value
 *
 * @return list operation value
 */
list_return_value list_getValue(list_list* list, void** value, unsigned int index);

/**
 * @brief Internal function for getting the first value from a list
 *
 * @param list Pointer to a list
 * @param value pointer in which the value shall be put
 *
 * @return list operation value
 */
list_return_value list_getValueFirst(list_list* list, void** value);

/**
 * @brief Internal function for getting the last value from a list
 *
 * @param list Pointer to a list
 * @param value pointer in which the value shall be put
 *
 * @return list operation value
 */
list_return_value list_getValueLast(list_list* list, void** value);

/**
 * @brief Internal function for getting a value value from a list
 *
 * @param list Pointer to a list
 * @param index index of the value
 *
 * @return pointer to value
 */
void* list_getValuePointer(list_list* list, unsigned int index);

/**
 * @brief Internal function for getting the first value from a list
 *
 * @param list Pointer to a list
 *
 * @return pointer to value
 */
void* list_getValueFirstPointer(list_list* list);

/**
 * @brief Internal function for getting the last value from a list
 *
 * @param list Pointer to a list
 *
 * @return pointer to value
 */
void* list_getValueLastPointer(list_list* list);

/**
 * @brief Internal function for getting the number of values in a list
 *
 * @param list Pointer to a list
 *
 * @return number of entries
 */
unsigned int list_count_entries(list_list* list);

/**
 * @brief type of data in MSB event/function
 */
typedef enum {
	MSB_INTEGER,/**< data is Integer */
	MSB_NUMBER,/**< data is a floating point number */
	MSB_STRING,/**< data is a String */
	MSB_BOOL,/**< data is Boolean */
	MSB_ARRAY/**< data is an Array */
} PrimitiveType;

/**
 * @brief format of data in MSB event/function
 */
typedef enum {
	MSB_INT32,/**< format of data is Integer32 */
	MSB_INT64,/**< format of data is Integer64 */
	MSB_FLOAT,/**< format of data is Float */
	MSB_DOUBLE,/**< format of data is Double */
	MSB_BYTE,/**< format of data is Byte */
	MSB_DATE,/**< format of data is Date */
	MSB_DATE_TIME,/**< format of data is DateTime */
	MSB_NONE/**< data has no format */
} PrimitiveFormat;

/**
 * @brief msb event structure
 */
typedef struct {
	char* eventId;/**< id of the event */
	char* name;/**< name of the event */
	char* description;/**< description of the event */
	char* postDate;/**< postdate of the event */
	json_object* dataFormat;/**< dataformat of the event */
	char* data;/**< data of the event */

	PrimitiveType type;/**< datatype of the event */
	PrimitiveFormat format;/**< dataformat of the event */

	PrimitiveType array_subtype;/**< sub datatype of the event if main type is Array */
	PrimitiveFormat array_subformat;/**< sub dataformat of the event if main type is Array */

	int atId;/**< the @ id of the event used in the self-description (necessary for referencing) */
} msbObjectEvent;

/**
 * @brief msb function structure
 */
typedef struct {
	char* functionId;/**< id of the function */
	char* name;/**< name of the function */
	char* description;/**< description of the function */
	json_object* dataFormat;/**< dataformat of the function */

	void (*callback)(void* client, void* data, void* context);/**< pointer to the actual function to be called on callbacks*/

	void* contextPtr;/**< pointer to context data that will be delivered during callbacks */

	int atId;/**< the @ id of the function used in the self-description (necessary for referencing) */

	list_list* responseEventAtIds;/**< list of @ ids of events used as response events */
} msbObjectFunction;

/**
 * @brief msb config parameter structure
 *
 * only one pointer of the various data pointers will be used at a time
 */
typedef struct {
	char* name;/**< name of the configuration parameter */
	int* bool_value;/**< pointer to a bool value */
	int32_t* int32_value;/**< pointer to a int32 value */
	int64_t* int64_value;/**< pointer to a int64 value */
	char* string_value;/**< pointer to a string value */
	float* float_value;/**< pointer to a float value */
	double* double_value;/**< pointer to a double value */
	PrimitiveType type;/**< data type of the configuration parameter value */
	PrimitiveFormat format;/**< data format of the configuration parameter value */
} msbObjectConfigParam;

/**
 * @brief Self-description core meta data
 */
typedef struct {
	char* service_uuid;/**< uuid of the Msb Service */
	char* service_token;/**< token of the Msb Service */
	char* service_class;/**< class of the Msb Service (either APPLICATION or SMART_OBJECT */
	char* service_name;/**< name of the Msb Service */
	char* service_description;/**< description of the Msb Service */
} msbObjectInfo;

/**
 * @brief Self-description structure
 */
typedef struct {
	schll_list* functionArray;/**< linked list of the Service's functions */
	schll_list* eventArray;/**< linked list of the Service's events */
	schll_list* configParamArray;/**< linked list of the Service's configuration parameters */
	msbObjectInfo objectInfo;/**< self-description core meta data of the service */

	unsigned int event_atId_counter;/**< internal counter for @ ids of events */
    unsigned int function_atId_counter;/**< internal counter for @ ids of functions */
} msbObject;

/**
 * @brief priority definition for MSB event publishing
 */
typedef enum {
	LOW,/**< LOW PRIORITY */
	MEDIUM,/**< MEDIUM PRIORITY */
	HIGH/**< HIGH PRIORITY */
} MessagePriority;

/**
 * @brief state enums for Msb state machine
 */
typedef enum {
	CREATED,/**< client is created */
    INITIALISED,/**< client is initialised */
    CONNECTING,/**< client is connecting */
    CONNECTED,/**< client is connected */
    REGISTERED,/**< client is registered */
    UNCONNECTED,/**< client isn't connected */
    ERROR,/**< some errors has happened */
    CLOSED/**< client is closed */
} statusAutomat;

/**
 * @brief Msb client main structure
 */
typedef struct {
	uint32_t maxNumberDataIncoming;/**< maximum number of incoming messages */
	uint32_t maxNumberDataOutgoing;/**< maximum number of outgoing messages */

	uint32_t maxMessageSize;/**< maximum message size */

	list_list* incoming_data;/**< internal list for incoming function calls */
	list_list* outgoing_data;/**< internal list for outgoing events */

	volatile char* dataOutInterface;/**< exchange point for outgoing messages */
	volatile int dataOutInterfaceFlag;/**< exchange point flag for outgoing messages */
	volatile int dataOutCache;/**< cache for outgoing messages */

	void (*commonCallBackFunction)(void*, void*);/**< common function for Msb function callbacks */

	bool fwdCompleteFunctionCallData;/**< flag for handing over the complete function call when running a callback function */

	void (*configParamFunction)(void*, void*);/**< pointer for callback function for configuration parameter changes */

	void* websocketData;/**< websocket connection data */

	msbObject* msbObjectData;/**< service data */

	statusAutomat currentStatusAutomat;/**< status of state machine */
	statusAutomat statusAutomatChange;/**< former status of state machine */
	bool force_exit;/**< exit flag for state machine */
	uint32_t pause_automat;/**< pause for state machine cycle */

#ifndef _WIN32
	pthread_t thread;/**< thread for state machine */
	pthread_attr_t thread_attr;/**< thread_attributes for state machine thread */
    pthread_mutex_t mutex;/**< mutex for safe data access and exchange */
#else
	HANDLE thread;
	HANDLE mutex;
#endif // !_WIN32

	uint32_t interval_reconnect;/**< interval for automatic reconnect attempt */

	bool flag_reconnect;/**< flag for automatic reconnect */
	bool sockJs_framing;/**< flag for sockjs message framing */

	int (*debugFunction)(const char* format, ... );/**< pointer to debug output function */
	bool debug;/**< flag for debug output activation */

} msbClient;

/**
 * @brief Create an Msb client
 *
 * @param url URL to MSB
 * @param uuid Service UUID
 * @param token Service token
 * @param service_class Service class ("Application" or "Smart Object")
 * @param name Service name
 * @param description Service description
 * @param tls Use tls
 * @param client_cert Path to client certificate
 * @param client_key Path to client key
 * @param ca_cert  Path to ca certificate
 *
 * @return Created client
 */
msbClient* msbClientNewClientURL(char* url, char* uuid, char* token, char* service_class, char* name,
								  char* description,
								  bool tls, char* client_cert, char* client_key, char* ca_cert);

/**
 * @brief Create an Msb client
 *
 * @param uuid Service UUID
 * @param token Service token
 * @param service_class Service class ("Application" or "Smart Object")
 * @param name Service name
 * @param description Service description
 * @param tls Use tls
 * @param client_cert Path to client certificate
 * @param client_key Path to client key
 * @param ca_cert  Path to ca certificate
 * @param ipv6 Use IPv6 (>0) or not (0)
 * @param address MSB address (use either targetAddr or targetHostname)
 * @param port MSB port
 * @param hostname MSB hostname (use either targetAddr or targetHostname)
 * @param path MSB path
 *
 * @return Created client
 */
msbClient* msbClientNewClient(bool ipv6, char* address, char* port, char* hostname,
							   char* path,
							   char* uuid, char* token, char* service_class, char* name, char* description,
							   bool tls, char* client_cert, char* client_key, char* ca_cert);

/**
 * @brief Delete an Msb Client
 *
 * @param client Pointer to Msb client
 */
void msbClientDeleteClient(msbClient* client);

/**
 * @brief Change the target url of an existing Msb Client
 *
 * @param client Pointer to Msb client
 * @param url URL to MSB
 * @param tls Use tls
 * @param client_cert Path to client certificate
 * @param client_key Path to client key
 * @param ca_cert  Path to ca certificate
 */
void msbClientChangeURL(msbClient* client, char* url, bool tls, char* client_cert, char* client_key, char* ca_cert);

/**
 * @brief Change the target address of an existing Msb Client
 *
 * @param client Pointer to Msb client
 * @param ipv6 Use IPv6 (>0) or not (0)
 * @param address MSB address (use either targetAddr or targetHostname)
 * @param port MSB port
 * @param hostname MSB hostname (use either targetAddr or targetHostname)
 * @param path MSB path
 * @param tls Use tls
 * @param client_cert Path to client certificate
 * @param client_key Path to client key
 * @param ca_cert  Path to ca certificate
 */
void msbClientChangeAddress(msbClient* client, bool ipv6, char* address, char* port, char* hostname, char* path, bool tls, char* client_cert, char* client_key, char* ca_cert);

/**
 * @brief Use a SockJS-compatible path when connecting
 *
 * @param client Pointer to Msb client
 * @param server_id Strings for path creation ($server_id/$session_id/$transport_id)
 * @param session_id Strings for path creation ($server_id/$session_id/$transport_id)
 * @param transport_id Strings for path creation ($server_id/$session_id/$transport_id)
 */
void msbClientUseSockJSPath(msbClient* client, char* server_id, char* session_id, char* transport_id);

/**
 * @brief Generate a SockJS-compatible path when connecting
 *
 * @param client Pointer to Msb client
 */
void msbClientGenerateSockJSPath(msbClient* client);

/**
 * @brief Activate/deactivate Sock-JS-compatible message framing
 *
 * @param client Pointer to Msb client
 * @param on_off 0-deactivate, 1-activate
 */
void msbClientSetSockJSFraming(msbClient* client, bool on_off);

/**
 * @brief Set maximum message length
 *
 * @param client Pointer to Msb client
 * @param size_in_bytes length
 */
void msbClientSetMaxMessageSize(msbClient* client, uint32_t size_in_bytes);

/**
 * @brief Assign certificates for TLS
 *
 * @param client Pointer to Msb client
 * @param client_cert Path to client certificate
 * @param client_key Path to client key
 * @param ca_cert  Path to ca certificate
 */
bool msbClientAssignTLSCerts(msbClient* client, const char* client_cert, const char* client_key, const char* ca_cert);

/**
 * @brief Initialise Msb client
 *
 * @param client Pointer to Msb client
 */
void msbClientInitialise(msbClient* client);

/**
 * @brief Connect to Msb
 *
 * @param client Pointer to Msb client
 */
void msbClientConnect(msbClient* client);

/**
 * @brief Register (client usually registers itself. Only necessary when self-description has been updated after connecting
 *
 * @param client Pointer to Msb client
 */
int msbClientRegister(msbClient* client);

/**
 * @brief Disconnect
 *
 * @param client Pointer to Msb client
 */
void msbClientDisconnect(msbClient* client);

/**
 * @brief De-initialise (clear memory etc.)
 *
 * @param client Pointer to Msb client
 */
void msbClientDeinitialise(msbClient* client);

/**
 * @brief Start Msb client thread
 *
 * @param client Pointer to Msb client
 */
int msbClientRunClientStateMachine(msbClient* client);

/**
 * @brief Stop Msb client thread
 *
 * @param client Pointer to Msb client
 */
int msbClientHaltClientStateMachine(msbClient* client);

/**
 * @brief Add a configuration parameter to the self description
 *
 * @param client Pointer to Msb client
 * @param cpName Name of configuration parameter
 * @param cpType Type of configuration parameter
 * @param cpFormat Format of configuration parameter
 * @param value Initial value of configuration parameter
 */
void
msbClientAddConfigParam(msbClient* client, char* cpName, PrimitiveType cpType, PrimitiveFormat cpFormat, void* value);

/**
 * @brief Add a configuration parameter to the self description
 *
 * @param client Pointer to Msb client
 * @param object name, type, format and value of a configuration parameter in a json object
 */
void msbClientAddConfigParamFromObject(msbClient* client, json_object* object);

/**
 * @brief Add a configuration parameter to the self description
 *
 * @param client Pointer to Msb client
 * @param string Name, Type, Format and Value of a configuration parameter in a json string
 */
void msbClientAddConfigParamFromString(msbClient* client, char* string);

/**
 * @brief Remove a configuration parameter from the self description
 *
 * @param client Pointer to Msb client
 * @param cpName Name of configuration parameter
 *
 * @warning EXPERIMENTAL
 */
void msbClientRemoveConfigParam(msbClient* client, char* cpName);

/**
 * @brief Change a configuration parameter's value
 *
 * @param client Pointer to Msb client
 * @param cpName Name of configuration parameter
 * @param value new value of configuration parameter
 */
void msbClientChangeConfigParamValue(msbClient* client, char* cpName, void* value);

/**
 * @brief Add a function with a simple input parameter
 *
 * @param client Pointer to Msb client
 * @param fId Function id
 * @param fName function name
 * @param fDesc function description
 * @param fPtr Pointer to actual function
 * @param fType Type of input parameter
 * @param fFormat Format of input parameter
 * @param isArray Is the input parameter an array?
 * @param contextPtr Pointer to user-defined data that will be forwarded to the function when called
 */
void msbClientAddFunction(msbClient* client, char* fId, char* fName, char* fDesc, void(*fPtr)(void*, void*, void*),
						   PrimitiveType fType, PrimitiveFormat fFormat, bool isArray, void* contextPtr);

/**
 * @brief Add a function without any input parameters
 *
 * @param client Pointer to Msb client
 * @param fId Function id
 * @param fName function name
 * @param fDesc function description
 * @param fPtr Pointer to actual function
 * @param contextPtr Pointer to user-defined data that will be forwarded to the function when called
 */
void msbClientAddEmptyFunction(msbClient* client, char* fId, char* fName, char* fDesc, void(*fPtr)(void*, void*, void*),
								void* contextPtr);

/**
 * @brief Add a function with complex input parameters, described by a json string
 *
 * @param client Pointer to Msb client
 * @param fId Function id
 * @param fName function name
 * @param fDesc function description
 * @param fPtr Pointer to actual function
 * @param fFormat dataformat of the function's input parameters as a json string
 * @param contextPtr Pointer to user-defined data that will be forwarded to the function when called
 */
void msbClientAddFunctionFromString(msbClient* client, char* fId, char* fName, char* fDesc, void(*fPtr)(void*, void*, void*),
									 char* fFormat, void* contextPtr);

/**
 * @brief Add a function with complex input parameters, described by a json object
 *
 * @param client Pointer to Msb client
 * @param fId Function id
 * @param fName function name
 * @param fDesc function description
 * @param fPtr Pointer to actual function
 * @param fFormat dataformat of the function's input parameters as a json object
 * @param contextPtr Pointer to user-defined data that will be forwarded to the function when called
 */
void msbClientAddComplexFunction(msbClient* client, char* fId, char* fName, char* fDesc, void(*fPtr)(void*, void*, void*),
								  json_object* fFormat, void* contextPtr);

 /**
 * @brief Remove a function
 *
 * @param client Pointer to Msb client
 * @param fId Function id
 *
 * @warning EXPERIMENTAL
 */
void msbClientRemoveFunction(msbClient* client, char* fId);

/**
 * @brief Add a response event to a function
 *
 * @param client Pointer to Msb client
 * @param fId Id of the function
 * @param eId Id of the event that shall be described as a response event in the object's self description
 */
void msbClientAddResponseEventToFunction(msbClient* client, char* fId, char* eId);

/**
 * @brief Remove a response event from a function
 *
 * @param client Pointer to Msb client
 * @param fId Id of the function
 * @param eId Id of the event that is a response event in the object's self description
 *
 * @warning EXPERIMENTAL
 */
void msbClientRemoveResponseEventFromFunction(msbClient* client, char* fId, char* eId);

/**
 * @brief Add an event with a simple output parameter
 *
 * @param client Pointer to Msb client
 * @param eId Event id
 * @param eName Event name
 * @param eDesc Event description
 * @param eType Type of the output parameter
 * @param eFormat Format of the output parameter
 * @param isArray true if output parameter is an array
 */
void
msbClientAddEvent(msbClient* client, char* eId, char* eName, char* eDesc, PrimitiveType eType, PrimitiveFormat eFormat,
				   bool isArray);

/**
 * @brief Add an event without any output parameters
 *
 * @param client Pointer to Msb client
 * @param eId Event id
 * @param eName Event name
 * @param eDesc Event description
 */
void msbClientAddEmptyEvent(msbClient* client, char* eId, char* eName, char* eDesc);

/**
 * @brief Add an event with output parameters as described by a json string
 *
 * @param client Pointer to Msb client
 * @param eId Event id
 * @param eName Event name
 * @param eDesc Event description
 * @param eFormat output parameters described in a json string
 * @param isArray true if output parameter is an array
 */
void msbClientAddEventFromString(msbClient* client, char* eId, char* eName, char* eDesc, char* eFormat, bool isArray);

/**
 * @brief Add an event with output parameters as described by a json object
 *
 * @param client Pointer to Msb client
 * @param eId Event id
 * @param eName Event name
 * @param eDesc Event description
 * @param eFormat output parameters described in a json object
 * @param isArray true if output parameter is an array
 */
void
msbClientAddComplexEvent(msbClient* client, char* eId, char* eName, char* eDesc, json_object* eFormat, bool isArray);

/**
 * @brief Remove an event from the self-description
 *
 * @param client Pointer to Msb client
 * @param eId Event id
 *
 * @warning EXPERIMENTAL
 */
void msbClientRemoveEvent(msbClient* client, char* eId);

/**
 * @brief Publish an event
 *
 * @param client Pointer to Msb client
 * @param eId Id of the event that shall be used
 * @param priority Message priority
 * @param data Data to be sent
 * @param array_length Number of elements if data is an array
 * @param corrId Correlation Id as a string. May be left empty, then the client will generate its own. Use for response events.
 */
void msbClientPublish(msbClient* client, char* eId, MessagePriority priority, void* data, size_t array_length,
					   char* corrId);

/**
 * @brief Publish an empty event
 *
 * @param client Pointer to Msb client
 * @param eId Id of the event that shall be used
 * @param priority Message priority
 * @param corrId Correlation Id as a string. May be left empty, then the client will generate its own. Use for response events.
 */
void msbClientPublishEmpty(msbClient* client, char* eId, MessagePriority priority, char* corrId);

/**
 * @brief Publish a complex event with data put in over a json string
 *
 * @param client Pointer to Msb client
 * @param eId Id of the event that shall be used
 * @param priority Message priority
 * @param pData Data to be sent as a json string
 * @param corrId Correlation Id as a string. May be left empty, then the client will generate its own. Use for response events.
 */
void msbClientPublishFromString(msbClient* client, char* eId, MessagePriority priority, char* pData, char* corrId);

/**
 * @brief Publish a complex event with data put in over a json object
 *
 * @param client Pointer to Msb client
 * @param eId Id of the event that shall be used
 * @param priority Message priority
 * @param pData Data to be sent as a json object
 * @param corrId Correlation Id as a string. May be left empty, then the client will generate its own. Use for response events.
 */
void msbClientPublishComplex(msbClient* client, char* eId, MessagePriority priority, json_object* pData, char* corrId);

/**
 * @brief Set Heartbeat interval
 *
 * @param client Pointer to Msb client
 * @param interval_in_ms Heartbeat interval in milliseconds
 */
void msbClientSetHeartbeatInterval(msbClient* client, uint16_t interval_in_ms);

/**
 * @brief Set pause for Msb client status machine cycle
 *
 * @param client Pointer to Msb client
 * @param pause_in_ms Pause in milliseconds
 */
void msbClientSetStateMachineCyclePause(msbClient* client, uint16_t pause_in_ms);

/**
 * @brief Enable TLS hostname verification
 *
 * @param client Pointer to Msb client
 */
void msbClientEnableHostnameVerification(msbClient* client);

/**
 * @brief Disable TLS hostname verification
 *
 * @param client Pointer to Msb client
 */
void msbClientDisableHostnameVerification(msbClient* client);

/**
 * @brief Test if connection is ready
 *
 * @param client Pointer to Msb client
 */
bool msbClientConnectionReady(msbClient* client);

/**
 * @brief Enable automatic reconnect feature
 *
 * @param client Pointer to Msb client
 */
void msbClientEnableReconnect(msbClient* client);

/**
 * @brief Disable automatic reconnect feature
 *
 * @param client Pointer to Msb client
 */
void msbClientDisableReconnect(msbClient* client);

/**
 * @brief Enable/disable automatic reconnect feature
 *
 * @param client Pointer to Msb client
 * @param interval_in_ms Reconnect interval in milliseconds
 */
void msbClientSetReconnectInterval(msbClient* client, uint16_t interval_in_ms);

/**
 * @brief Enable event cache
 *
 * @param client Pointer to Msb client
 */
void msbClientEnableEventCache(msbClient* client);

/**
 * @brief Disable event cache
 *
 * @param client Pointer to Msb client
 */
void msbClientDisableEventCache(msbClient* client);

/**
 * @brief Set event cache size
 *
 * @param client Pointer to Msb client
 * @param size Size of event cache
 */
void msbClientSetEventCacheSize(msbClient* client, uint32_t size);

/**
 * @brief Set function cache size
 *
 * @param client Pointer to Msb client
 * @param size Size of function cache
 */
void msbClientSetFunctionCacheSize(msbClient* client, uint32_t size);

/**
 * @brief Set a common callback function for all function calls
 *
 * @param client Pointer to Msb client
 * @param fPtr Pointer to callback function
 */
void msbClientSetCommonCallbackFunction(msbClient* client, void(*fPtr)(void*, void*));

/**
 * @brief Remove a common callback function from all function calls
 *
 * @param client Pointer to Msb client
 */
void msbClientRemoveCommonCallbackFunction(msbClient* client);

/**
 * @brief Set a common callback function for configuration parameter changes
 *
 * @param client Pointer to Msb client
 * @param fPtr Pointer to callback function
 */
void msbClientSetConfigParamCallbackFunction(msbClient* client, void(*fPtr)(void*, void*));

/**
 * @brief Remove a common callback function for configuration parameter changes
 *
 * @param client Pointer to Msb client
 */
void msbClientRemoveConfigParamCallbackFunction(msbClient* client);

/**
 * @brief Forward the complete function call to a callback function
 *
 * @param client Pointer to Msb client
 */
void msbClientFwdCompleteFunctionCall(msbClient* client);

/**
 * @brief Forward only parameters to a callback function
 *
 * @param client Pointer to Msb client
 */
void msbClientFwdParametersOnlyFunctionCall(msbClient* client);

/**
 * @brief Enable/disable debug output
 *
 * @param client Pointer to Msb client
 * @param on_off debug flag
 */
void msbClientSetDebug(msbClient* client, bool on_off);

/**
 * @brief Set debug function
 *
 * @param client Pointer to Msb client
 * @param function Pointer to debug function
 */
void msbClientSetDebugFunction(msbClient* client, int (*function)(const char*, ...));

#endif /* LIBMSBCLIENTC_H_ */
