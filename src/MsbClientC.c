/****************************************************************************
 * Copyright 2019 D. Breunig, Fraunhofer IPA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ****************************************************************************/

/**
* @file MsbClientC.c
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief Msb C Client functions
*/

#include "MsbClientC.h"

#include <uuid/uuid.h>
#include "websocket/websocket.h"

#include "rest/schll.c"
#include "rest/einfl.c"

/**
 * @brief Internal function for adding a configuration parameter to an Msb object
 *
 * @param object pointer to Msb object
 * @param cpName pointer to configuration parameter name
 * @param cpType configuration parameter data type
 * @param cpFormat configuration parameter data format
 * @param value pointer to actual value
 */
void addConfigParam(msbObject* object, char* cpName, PrimitiveType cpType, PrimitiveFormat cpFormat, void* value);

/**
 * @brief Internal function for removing a configuration parameter from an Msb object
 *
 * @param object pointer to Msb object
 * @param cpName pointer to configuration parameter name
 */
void removeConfigParam(msbObject* object, char* cpName);

/**
 * @brief Internal function for changing a configuration parameter's value
 *
 * @param object pointer to Msb object
 * @param cpName pointer to configuration parameter name
 * @param value pointer to new value
 */
void changeConfigParamValue(msbObject* object, char* cpName, void* value);

/**
 * @brief Internal function for adding a function to an Msb object
 *
 * @param object pointer to Msb object
 * @param fId pointer to function name
 * @param fName name of the function
 * @param fDesc description of the function
 * @param fPtr pointer to callback function
 * @param fType function data type
 * @param fFormat function data format
 * @param isArray flag if data type is an array
 * @param contextPtr pointer context data to be provided on callbacks
 */
void addFunction(msbObject* object, char* fId, char* fName, char* fDesc, void (*fPtr)(void*, void*, void*), PrimitiveType fType, PrimitiveFormat fFormat, bool isArray, void* contextPtr);

/**
 * @brief Internal function for adding a function with a complex data format to an Msb object
 *
 * @param object pointer to Msb object
 * @param fId pointer to function name
 * @param fName name of the function
 * @param fDesc description of the function
 * @param fPtr pointer to callback function
 * @param fFormat function data format
 * @param contextPtr pointer context data to be provided on callbacks
 */
void addComplexFunction(msbObject* object, char* fId, char* fName, char* fDesc, void (*fPtr)(void*, void*, void*), json_object* fFormat, void* contextPtr);

/**
 * @brief Internal function for adding a function without any parameters to an Msb object
 *
 * @param object pointer to Msb object
 * @param fId pointer to function name
 * @param fName name of the function
 * @param fDesc description of the function
 * @param fPtr pointer to callback function
 * @param contextPtr pointer context data to be provided on callbacks
 */
void addEmptyFunction(msbObject* object, char* fId, char* fName, char* fDesc, void (*fPtr)(void*, void*, void*), void* contextPtr);

/**
 * @brief Internal function for removing a function from an Msb object
 *
 * @param object pointer to Msb object
 * @param fId pointer to function name
 */
void removeFunction(msbObject* object, char* fId);

/**
 * @brief Internal function for adding a response event to a function entry
 *
 * @param object pointer to Msb object
 * @param fId if of the function
 * @param eId id of the event
 */
void addResponseEventToFunction(msbObject* object, char* fId, char* eId);

/**
 * @brief Internal function for removing a response event from a function entry
 *
 * @param object pointer to Msb object
 * @param fId if of the function
 * @param eId id of the event
 */
void removeResponseEventFromFunction(msbObject* object, char* fId, char* eId);

/**
 * @brief Internal function for adding an event to an Msb object
 *
 * @param object pointer to Msb object
 * @param eId pointer to event name
 * @param eName name of the event
 * @param eDesc description of the event
 * @param eType event data type
 * @param eFormat event data format
 * @param isArray flag if data type is an array
 */
void addEvent(msbObject* object, char* eId, char* eName, char* eDesc, PrimitiveType eType, PrimitiveFormat eFormat, bool isArray);

/**
 * @brief Internal function for adding an event with a complex data format to an Msb object
 *
 * @param object pointer to Msb object
 * @param eId pointer to event name
 * @param eName name of the event
 * @param eDesc description of the event
 * @param eFormat event data format
 * @param isArray flag if data type is an array
 */
void addComplexEvent(msbObject* object, char* eId, char* eName, char* eDesc, json_object* eFormat, bool isArray);

/**
 * @brief Internal function for adding an event without parameters to an Msb object
 *
 * @param object pointer to Msb object
 * @param eId pointer to event name
 * @param eName name of the event
 * @param eDesc description of the event
 */
void addEmptyEvent(msbObject* object, char* eId, char* eName, char* eDesc);

/**
 * @brief Internal function for removing an event from an Msb object
 *
 * @param object pointer to Msb object
 * @param eId pointer to event name
 */
void removeEvent(msbObject* object, char* eId);

/**
 * @brief Internal function for serialising an Msb object
 *
 * @param object pointer to Msb object
 */
const char* msbObjectSelfDescription(const msbObject* object);

/**
 * @brief Internal function for creating an Msb object
 *
 * @param uuid uuid of service
 * @param token token of Msb service
 * @param service_class class of Msb service (either APPLICATION or SMART_OBJECT)
 * @param name name of service
 * @param description description of service
 *
 * @return Created Msb object
 */
msbObject* msbObjectCreateObject(char* uuid, char* token, char* service_class, char* name, char* description);

/**
 * @brief Internal function for getting an IPv4 out of an address
 *
 * @param adr address
 * @param std standard output used, if address couldn't be correctly resolved. can be null
 *
 * @return IPv4 as string
 */
char* getIPv4(char* adr, char* std);

/**
 * @brief Internal function for getting an IPv6 out of an address
 *
 * @param adr address
 * @param std standard output used, if address couldn't be correctly resolved. can be null
 *
 * @return IPv6 as string
 */
char* getIPv6(char* adr, char* std);

/**
 * @brief Internal function for getting a port out of an address
 *
 * @param adr address
 * @param std standard output used, if address couldn't be correctly resolved. can be null
 *
 * @return port as string
 */
char* getPort(char* adr, char* std);

/**
 * @brief Internal function for getting a path out of an address
 *
 * @param adr address
 * @param std standard output used, if address couldn't be correctly resolved. can be null
 *
 * @return path as string
 */
char* getPath(char* adr, char* std);

/**
 * @brief Internal function for getting a protocol out of an address
 *
 * @param adr address
 * @param std standard output used, if address couldn't be correctly resolved. can be null
 *
 * @return protocol as string
 */
char* getProtocol(char* adr, char* std);

/**
 * @brief Internal function for getting a hostname out of an address
 *
 * @param adr address
 * @param std standard output used, if address couldn't be correctly resolved. can be null
 *
 * @return hostname as string
 */
char* getHostname(char* adr, char* std);

/**
 * @brief Internal function for getting the type of a message
 *
 * @param inh pointer to message
 *
 * @return message type index
 */
int getMsgType(const unsigned char* inh);

/**
 * @brief Internal message handling function
 *
 * @param client Pointer to Msb client
 * @param inh pointer to incoming message
 *
 * @return 0 is msg could be handled correctly, != 0 if otherwise
 */
int msbMsgFunction(msbClient* client, char* inh);

#ifndef _WIN32
/**
 * @brief Internal function for state machine thread
 *
 * @param client Pointer to Msb client
 */
void msbClientAutomatThread(msbClient* client);
#else
DWORD WINAPI msbClientAutomatThread(msbClient*);
#endif

/**
 * @brief Internal function for sending a text over the websocket connection
 *
 * @param client Pointer to Msb client
 * @param msg Pointer to message
 * @param msg_length Message length
 *
 * @return Returns number of sent bytes, every non-positive value flags an error
 */
int msbClientSendText(msbClient* client, const char* msg, size_t msg_length);

const char* messageTypes[11] = {
        "IO",
        "NIO",
        "IO_CONNECTED",
        "IO_REGISTERED",
        "IO_PUBLISHED",
        "NIO_ALREADY_CONNECTED",
        "NIO_REGISTRATION_ERROR",
        "NIO_UNEXPECTED_REGISTRATION_ERROR",
        "NIO_UNAUTHORIZED_CONNECTION",
        "NIO_EVENT_FORWARDING_ERROR",
        "NIO_UNEXPECTED_EVENT_FORWARDING_ERROR"
};

const char* integer_to_state[8] = {
        "CREATED", "INITIALISED", "CONNECTING", "CONNECTED", "REGISTERED", "UNCONNECTED", "ERROR", "CLOSED"
};

int getMsgType(const unsigned char* inh) {

    if (inh == NULL) return -2;

    size_t l = strlen((const char*) inh);

    if (l < 2) return -1;

    int i;
    for (i = 2; i < 11; ++i)
        if(strlen((const char*) inh) == strlen(messageTypes[i]) && strcmp((const char*) inh, messageTypes[i]) == 0) return i;

    return -1;

}

int msbMsgFunction(msbClient* client, char* inh) {

    if (inh == NULL) return 0;

    bool free_inh_flag;
    free_inh_flag = false;

    if (client->sockJs_framing) {
        if(strncmp(inh, "o", 1) == 0){
            if (client->debug) client->debugFunction("Msb message function: SockJS open frame\n");
            return 0;
        }else if(strncmp(inh, "h", 1) == 0){
            if (client->debug) client->debugFunction("Msb message function: SockJS server heartbeat\n");
            return 0;
        }else if(strncmp(inh, "c", 1) == 0) {
            if (client->debug) client->debugFunction("Msb message function: SockJS close frame\n");
            client->currentStatusAutomat = CONNECTING;
            return 0;
        }else if(strncmp(inh, "a[", 2) == 0) {
            if (client->debug) client->debugFunction("Msb message function: SockJS regular message\n");

            char* tmp;
            tmp = NULL;

            if (!string_replace(inh, "a[\"", "", &tmp)) {
                if (!string_replace(tmp, "\"]", "", &inh)) {
                    free(tmp);
                    if (!string_replace(inh, "\\\"", "\"", &tmp)) {
                        free(inh);
                        inh = tmp;
                        free_inh_flag = true;
                    }
                }
            }
        }
    }

    if (client->debug) client->debugFunction("Msb message function: Incoming message: %s\n", inh);

    if (strncmp(inh, "ping", 4) == 0) {
        if (client->debug) client->debugFunction("Msb message function: Received Ping, sending pong\n");

        int ret;
        char* pong = "pong";

        ret = msbClientSendText(client, pong, strlen(pong));

        if(free_inh_flag) free(inh);
        return ret;
    }

    int mtyp = getMsgType((const unsigned char*) inh);

    if (client->debug) {
        if (0 <= mtyp && mtyp < 11)
            client->debugFunction("Msb message function: Incoming message, message type: %s\n", messageTypes[mtyp]);
        else
            client->debugFunction("Msb message function: Incoming message, message type unknown: %i\n", mtyp);
    }

    if (mtyp == 2) {
        client->currentStatusAutomat = CONNECTED;
        msbClientRegister(client);
        if(free_inh_flag) free(inh);
        return 0;
    }else if (mtyp == 3) {
        client->currentStatusAutomat = REGISTERED;
        if(free_inh_flag) free(inh);
        return 0;
    }else if (mtyp != -1) {
        if(free_inh_flag) free(inh);
        return 0;
    }

    char* tmp;
    tmp = NULL;
    string_replace(inh, "\\/", "/", &tmp);

    if(free_inh_flag) free(inh);
    inh = tmp;

    char* in = &inh[2];

    switch ((inh)[0]) {
        case 'C': {
            struct json_object* jobj;
            jobj = json_tokener_parse(in);

            msbObjectFunction* function;

            if (schll_getValue(client->msbObjectData->functionArray, (void**) &function,
                               json_object_get_string(json_object_object_get(jobj, "functionId"))) != SCHLL_OK) {
                break;
            }

            if (client->fwdCompleteFunctionCallData) {
                function->callback(client, (void*) jobj, function->contextPtr);
            } else {
                function->callback(client, (void*) json_object_object_get(jobj, "functionParameters"), function->contextPtr);
            }

            free(jobj);
            break;
        }
        case 'K': {
            struct json_object* jobj;
            jobj = json_tokener_parse(in);
            struct json_object* parameters;
            parameters = json_object_object_get(jobj, "parameters");

            if (client->configParamFunction != NULL) client->configParamFunction(client, jobj);

            json_object_put(jobj);

            msbObjectConfigParam* c;

            json_object_object_foreach(parameters, schl, wert) {
                if (schll_getValue(client->msbObjectData->configParamArray, (void**) &c, schl) == SCHLL_OK) {
                    struct json_object* param;
                    param = json_object_object_get(parameters, schl);

                    const char* type = json_object_get_string(json_object_object_get(param, "type"));
                    json_object* value = json_object_object_get(param, "value");

                    if (value == NULL || type == NULL) break;

                    if (strcmp(type, "BOOLEAN") == 0) {
                        const char* str = json_object_get_string(value);
                        *(c->bool_value) = (strcmp(str, "TRUE") == 0);
                        break;
                    } else if (strcmp(type, "STRING") == 0) {
                        const char* str = json_object_get_string(value);

                        char* ptr = (char*) calloc(1, sizeof(char) * (strlen(str) + 1));
                        if (ptr == NULL) break;

                        if (ptr != strncpy((void*) ptr, (void*) str, strlen(str))) break;

                        free(c->string_value);

                        c->string_value = ptr;

                        break;
                    } else if (strcmp(type, "INTEGER") == 0) {
                        const char* format = json_object_get_string(json_object_object_get(param, "format"));

                        if (strcmp(format, "INT32") == 0) {
                            *(c->int32_value) = json_object_get_int(value);
                        } else if (strcmp(format, "INT64") == 0) {
                            *(c->int64_value) = json_object_get_int64(value);
                        }
                        break;
                    } else if (strcmp(type, "NUMBER") == 0) {
                        const char* format = json_object_get_string(json_object_object_get(param, "format"));

                        if (strcmp(format, "FLOAT") == 0) {
                            *(c->float_value) = (float) json_object_get_double(value);
                        } else if (strcmp(format, "DOUBLE") == 0) {
                            *(c->double_value) = json_object_get_double(value);
                        }
                        break;
                    } else if (strcmp(type, "ARRAY") == 0) {
                        break;
                    }

                    break;
                }
            }

            break;
        }
        default:
            break;
    }

    free(inh);

    return 0;

}

#define publish_entry \
json_object* root;\
root = json_object_new_object();\
msbObjectEvent* ev;\
if (schll_getValue(client->msbObjectData->eventArray, (void**) &ev, eId) != SCHLL_OK) return;\
json_object* dataJ;\
dataJ = NULL;

#define publish_common_inf \
json_object_object_add(root, "eventId", json_object_new_string(ev->eventId));\
json_object_object_add(root, "uuid", json_object_new_string(client->msbObjectData->objectInfo.service_uuid));\
json_object_object_add(root, "priority", json_object_new_int(mp));\
char tBuff[26];\
getDateTime(tBuff);\
json_object_object_add(root, "postDate", json_object_new_string(tBuff));

#define publish_make_correlation_id_WIN32 \
char corr_uuid[37] = {0};\
if (corrId == NULL){\
    GUID uuid_gen;\
    CoCreateGuid(&uuid_gen);\
    sprintf_s(corr_uuid, "%08lx-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",\
                uuid_gen.Data1, uuid_gen.Data2, uuid_gen.Data3,\
                uuid_gen.Data4[0], uuid_gen.Data4[1], uuid_gen.Data4[2], uuid_gen.Data4[3],\
                uuid_gen.Data4[4], uuid_gen.Data4[5], uuid_gen.Data4[6], uuid_gen.Data4[7]);\
    corrId = corr_uuid;\
}\
json_object_object_add(root, "correlationId", json_object_new_string(corrId));\

#define publish_make_correlation_id \
char corr_uuid[37] = {0};\
if (corrId == NULL){\
    uuid_t uuid_t;\
    uuid_generate(uuid_t);\
    uuid_unparse(uuid_t, corr_uuid);\
    corrId = corr_uuid;\
}\
json_object_object_add(root, "correlationId", json_object_new_string(corrId));\

#define publish_send_and_close \
if(client->outgoing_data != NULL){\
    json_object_object_add(root, "dataObject", dataJ);\
    char* msg = string_combine("E ", json_object_to_json_string(root));\
    /*pthread_mutex_lock(&client->outgoing_data->mutex);*/\
    if ((client->currentStatusAutomat == REGISTERED || client->dataOutCache == 1) &&\
        list_count_entries(client->outgoing_data) < client->maxNumberDataOutgoing) { \
        if (strlen(msg) < client->maxMessageSize) { \
            list_add(client->outgoing_data, msg, 1, strlen(msg) + 1);\
        }\
    }\
    /*pthread_mutex_unlock(&client->outgoing_data->mutex);*/\
    free(msg);\
    free(root);\
}

//pthread_mutex_lock(&client->mutex);
//pthread_mutex_unlock(&client->mutex);\

#define publish_send_and_close_WIN32 \
if(client->outgoing_data != NULL){\
json_object_object_add(root, "dataObject", dataJ);\
char* msg = string_combine("E ", json_object_to_json_string(root));\
DWORD dwWaitResult = WaitForSingleObject(\
        client->mutex,\
        INFINITE);\
if(dwWaitResult == WAIT_OBJECT_0){\
if ((client->currentStatusAutomat == REGISTERED || client->dataOutCache == 1) &&\
list_count_entries(client->outgoing_data) < client->maxNumberDataOutgoing) { \
if (strlen(msg) < client->maxMessageSize) { \
list_add(client->outgoing_data, msg, 1, strlen(msg) + 1);\
 }}\
}\
ReleaseMutex(client->mutex);\
#endif\
free(msg);\
free(root);\
}

void msbClientPublish(msbClient* client, char* eId, MessagePriority mp, void* data, size_t arr_l, char* corrId) {

    publish_entry
    publish_common_inf

#ifdef _WIN32
    publish_make_correlation_id_WIN32
#else
    publish_make_correlation_id
#endif

    switch (ev->type) {
        case MSB_ARRAY:
            if (arr_l <= 0) {
                client->debugFunction("msbClientPublish: Event is of type array, but arraylength is invalid. arr_l = %i\n",
                       (int) arr_l);
                return;
            }

            size_t i;
            i = 0;

            dataJ = json_object_new_array();
            switch (ev->array_subtype) {
                case MSB_INTEGER:
                    if (ev->array_subformat == MSB_INT32) {
                        int32_t (*arr_p)[arr_l] = data;
                        for (; i < arr_l; ++i) json_object_array_add(dataJ, json_object_new_int((*arr_p)[i]));
                    } else if (ev->array_subformat == MSB_INT64) {
                        int64_t (*arr_p)[arr_l] = data;
                        for (; i < arr_l; ++i) json_object_array_add(dataJ, json_object_new_int64((*arr_p)[i]));
                    }
                    break;
                case MSB_NUMBER:
                    if (ev->format == MSB_FLOAT || ev->format == MSB_DOUBLE) {
                        double (*arr_p)[arr_l] = data;
                        for (; i < arr_l; ++i) json_object_array_add(dataJ, json_object_new_double((*arr_p)[i]));
                    } else if (ev->format == MSB_BYTE) {
                        int32_t (*arr_p)[arr_l] = data;
                        for (; i < arr_l; ++i) json_object_array_add(dataJ, json_object_new_int((*arr_p)[i]));
                    }
                    break;
                case MSB_BOOL: {
                    json_bool (*arr_p)[arr_l] = data;
                    for (; i < arr_l; ++i) json_object_array_add(dataJ, json_object_new_boolean((*arr_p)[i]));
                }
                    break;
                case MSB_STRING: {
                    char* (*arr_p)[arr_l] = data;
                    for (; i < arr_l; ++i) json_object_array_add(dataJ, json_object_new_string((*arr_p)[i]));
                }
                    break;
                default:
                    break;
            }
            break;
        case MSB_INTEGER:
            if (ev->format == MSB_INT32)
                dataJ = json_object_new_int(*(int32_t*) data);
            else if (ev->format == MSB_INT64)
                dataJ = json_object_new_int64(*(int64_t*) data);
            break;
        case MSB_NUMBER:
            if (ev->format == MSB_FLOAT || ev->format == MSB_DOUBLE)
                dataJ = json_object_new_double(*(double*) data);
            else if (ev->format == MSB_BYTE)
                dataJ = json_object_new_int(*(int32_t*) data);
            break;
        case MSB_BOOL:
            dataJ = json_object_new_boolean(*(json_bool*) data);
            break;
        case MSB_STRING:
            dataJ = json_object_new_string((char*) data);
            break;
        default:
            break;
    }

#ifdef _WIN32
    publish_send_and_close_WIN32
#else
    publish_send_and_close
#endif
}

void msbClientPublishEmpty(msbClient* client, char* eId, MessagePriority mp, char* corrId) {

    publish_entry
    publish_common_inf

#ifdef _WIN32
    publish_make_correlation_id_WIN32
    publish_send_and_close_WIN32
#else
    publish_make_correlation_id
    publish_send_and_close
#endif
}

void msbClientPublishComplex(msbClient* client, char* eId, MessagePriority mp, json_object* obj, char* corrId) {

    publish_entry
    publish_common_inf

#ifdef _WIN32
    publish_make_correlation_id_WIN32
#else
    publish_make_correlation_id
#endif

    dataJ = obj;

#ifdef _WIN32
    publish_send_and_close_WIN32
#else
    publish_send_and_close
#endif
}

void msbClientPublishFromString(msbClient* client, char* eId, MessagePriority mp, char* string, char* corrId) {

    json_object* obj = json_tokener_parse(string);

    msbClientPublishComplex(client, eId, mp, obj, corrId);

}

int msbClientRunClientStateMachine(msbClient* client) {
    client->force_exit = false;
#ifndef _WIN32
    pthread_attr_init(&(client->thread_attr));
    return pthread_create(&(client->thread), &(client->thread_attr), (void*) msbClientAutomatThread, client);
#else
    client->thread = CreateThread(NULL, 0, msbClientAutomatThread, client, 0, NULL);
#endif
}

int msbClientHaltClientStateMachine(msbClient* client) {
    client->force_exit = true;
#ifndef _WIN32
    pthread_join(client->thread, NULL);
    return pthread_attr_destroy(&(client->thread_attr));
#endif
}

#ifndef _WIN32
void msbClientAutomatThread(msbClient* client) {
#else
    DWORD WINAPI msbClientAutomatThread(msbClient* client){
#endif

    wsData* ws;
    ws = client->websocketData;

    client->currentStatusAutomat = CREATED;

    client->incoming_data = list_makeListPointer();
    client->outgoing_data = list_makeListPointer();

    list_list* incoming_data = client->incoming_data;
    list_list* outgoing_data = client->outgoing_data;

    while (!client->force_exit) {
        if (client->debug && client->statusAutomatChange != client->currentStatusAutomat) {
            if (client->currentStatusAutomat <= CLOSED && client->statusAutomatChange <= CLOSED)
                client->debugFunction("Msb client state machine: Status change: From %s to %s\n", integer_to_state[client->statusAutomatChange],
                       integer_to_state[client->currentStatusAutomat]);
            else
                client->debugFunction("Msb client state machine: Status change: From %i to %i\n", client->statusAutomatChange, client->currentStatusAutomat);
        }

        client->statusAutomatChange = client->currentStatusAutomat;

        switch (client->currentStatusAutomat) {
            case CREATED: {
                client->currentStatusAutomat = INITIALISED;
                msbClientInitialise(client, client->nopoll_debug);
                usleep(1000000);
                break;
            }
            case INITIALISED: {
                client->currentStatusAutomat = CONNECTING;
                msbClientConnect(client);
                usleep(100000);
                break;
            }
            case CONNECTING: {
                if (msbClientConnectionReady(client)) {
                    //if(!client->sockJs_framing)
                    //msbClientRegister(client);
                    client->currentStatusAutomat = CONNECTED;
                }else {
                    msbClientConnect(client);
                }
                usleep(100000);
                break;
            }
            case CONNECTED: {
                //Verbindung iO?
                if (!msbClientConnectionReady(client)) {
                    if (client->flag_reconnect == 1)
                        client->currentStatusAutomat = CONNECTING;
                    else
                        client->currentStatusAutomat = UNCONNECTED;
                    break;
                }

                if (list_count_entries(incoming_data) < client->maxNumberDataIncoming) {
                    char* msg = wsGetMsgContent(client->websocketData);
                    if(msg != NULL){
/*#ifndef _WIN32
                        pthread_mutex_lock(&client->incoming_data->mutex);
#else
                        DWORD dwWaitResult = WaitForSingleObject(client->mutex, INFINITE);
if(dwWaitResult == WAIT_OBJECT_0){
#endif*/
                        list_add(incoming_data, msg, 1, strlen(msg) + 1);/*
#ifndef _WIN32
                        pthread_mutex_unlock(&client->incoming_data->mutex);
#else
                        ReleaseMutex(client->mutex);
#endif*/
                    }
                } else
                    wsGetMsgContent(
                            client->websocketData); //falls der Puffer für eingehende Nachrichten voll ist, wird der Eingangsspeicher ohne neue Ablage abgerufen

                if (list_count_entries(incoming_data) > 0) {
                    if (msbMsgFunction(client, (char*) list_getValueFirstPointer(incoming_data)) == 0) {
/*#ifndef _WIN32
                        pthread_mutex_lock(&client->mutex);
#else
                        DWORD dwWaitResult = WaitForSingleObject(client->mutex, INFINITE);
if(dwWaitResult == WAIT_OBJECT_0){
#endif*/
                        list_pop(incoming_data, 0, 1);
/*#ifndef _WIN32
                        pthread_mutex_unlock(&client->mutex);
#else
                        ReleaseMutex(client->mutex);
#endif*/
                    }
                    break;
                }
            }
            case REGISTERED: {
                //Verbindung iO?
                if (!msbClientConnectionReady(client)) {
                    if (client->flag_reconnect == 1)
                        client->currentStatusAutomat = CONNECTING;
                    else
                        client->currentStatusAutomat = UNCONNECTED;
                    break;
                }

                if (list_count_entries(incoming_data) < client->maxNumberDataIncoming) {
                    char* msg = wsGetMsgContent(client->websocketData);
                    if(msg != NULL){
/*#ifndef _WIN32
                        pthread_mutex_lock(&client->mutex);
#else
                        DWORD dwWaitResult = WaitForSingleObject(client->mutex, INFINITE);
if(dwWaitResult == WAIT_OBJECT_0){
#endif*/
                        list_add(incoming_data, msg, 1, strlen(msg) + 1);
/*#ifndef _WIN32
                        pthread_mutex_unlock(&client->mutex);
#else
                        ReleaseMutex(client->mutex);
#endif*/
                    }
                } else
                    wsGetMsgContent(client->websocketData);

                if (list_count_entries(outgoing_data) > 0) {
                    char* ab = list_getValueFirstPointer(outgoing_data);
                    if (msbClientSendText(client, ab, strlen(ab)) == 0) {
/*#ifndef _WIN32
                        pthread_mutex_lock(&client->mutex);
#else
                        DWORD dwWaitResult = WaitForSingleObject(client->mutex, INFINITE);
if(dwWaitResult == WAIT_OBJECT_0){
#endif*/
                        list_pop(outgoing_data, 0, 1);
/*#ifndef _WIN32
                        pthread_mutex_unlock(&client->mutex);
#else
                        ReleaseMutex(client->mutex);
#endif*/
                    }
                }

                if (list_count_entries(incoming_data) > 0) {
                    if (msbMsgFunction(client, list_getValueFirstPointer(incoming_data)) == 0) {
/*#ifndef _WIN32
                        pthread_mutex_lock(&client->mutex);
#else
                        DWORD dwWaitResult = WaitForSingleObject(client->mutex, INFINITE);
if(dwWaitResult == WAIT_OBJECT_0){
#endif*/
                        list_pop(incoming_data, 0, 1);
/*#ifndef _WIN32
                        pthread_mutex_unlock(&client->mutex);
#else
                        ReleaseMutex(client->mutex);
#endif*/
                    }
                }

                break;
            }
            case UNCONNECTED:
                break;
            case ERROR:
                break;
            case CLOSED:
                break;
            default:
                break;
        }
#ifndef _WIN32
        usleep(client->pause_automat);
#else
        Sleep(client->pause_automat / 1000);
#endif
    }

    list_deleteList(incoming_data, 1);
    list_deleteList(outgoing_data, 1);

    msbClientDisconnect(client);
    msbClientDeinitialise(client);
    client->currentStatusAutomat = CLOSED;
#ifndef _WIN32
    /*if (&(client->thread) != NULL)
        pthread_exit(NULL);
    else*/
        return;
#else
    return 0;
#endif
}

msbClient* msbClientNewClientURL(char* url, char* origin, char* uuid, char* token, char* service_class, char* name,
                                 char* description,
                                 bool tls, char* client_cert, char* client_key, char* ca_cert) {

    char* stdProt = "ws";
    char* stdAdr = NULL;
    char* stdPort;
    char* stdPath = "/websocket/data/websocket";

    char* proto = getProtocol(url, stdProt);
    char* address = getHostname(url, stdAdr);

    if (strncmp(proto, "wss", 3) == 0 && strlen(proto) == 3)
        stdPort = "8084";
    else if (strncmp(proto, "ws", 2) == 0)
        stdPort = "8085";
    else if (strncmp(proto, "https", 5) == 0)
        stdPort = "443";
    else if (strncmp(proto, "http", 4) == 0)
        stdPort = "80";
    else
        stdPort = "8085";

    char* port = getPort(url, stdPort);
    char* path = getPath(url, stdPath);

    msbClient* ret = msbClientNewClient(0, address, port, address, path, origin, uuid, token, service_class,
                                        name,
                                        description, tls, client_cert, client_key, ca_cert);

    if (stdProt != NULL)
        if (strcmp(proto, stdProt) != 0)
            free(proto);
    if (stdAdr != NULL)
        if (strcmp(address, stdAdr) != 0)
            free(address);
    if (stdPort != NULL)
        if (strcmp(port, stdPort) != 0)
            free(port);
    if (stdPath != NULL)
        if (strcmp(path, stdPath) != 0)
            free(path);

    return ret;

}

msbClient* msbClientNewClient(bool ipv6, char* address, char* port, char* hostname, char* path, char* origin,
                              char* uuid, char* token, char* service_class, char* name,
                              char* description,
                              bool tls, char* client_cert, char* client_key, char* ca_cert) {

    msbClient* ret = (msbClient*) calloc(1, sizeof(msbClient));

    if (ret == NULL) return NULL;

    ret->maxNumberDataIncoming = 100;
    ret->maxNumberDataOutgoing = 100;

    ret->dataOutInterface = NULL;
    ret->dataOutInterfaceFlag = 0;

    ret->outgoing_data = NULL;
    ret->incoming_data = NULL;

    ret->maxMessageSize = 1000000;

    ret->msbObjectData = msbObjectCreateObject(uuid, token, service_class, name, description);

    ret->currentStatusAutomat = CREATED;
    ret->statusAutomatChange = CREATED;
    ret->force_exit = false;

    ret->pause_automat = 10000; //usleep im zyklus in µs

    ret->flag_reconnect = true;
    ret->interval_reconnect = 10000000; //intervall zwischen wiederverbindungen in µs

    ret->sockJs_framing = false;

    ret->fwdCompleteFunctionCallData = false;

    ret->debug = 0;
    ret->debugFunction = &printf;

    ret->websocketData = wsDataConstr(ipv6, address, port, hostname, path, origin, tls, client_cert,
                                      client_key, ca_cert, ret->debugFunction);

    ret->nopoll_debug = false;

    wsData* wsdebug = ret->websocketData;

#ifndef _WIN32
    pthread_mutex_init(&ret->mutex, NULL);
#else
    ret->mutex = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex
#endif

    return ret;
}

void msbClientDeleteClient(msbClient* client) {

    if (client == NULL) return;

    msbClientDeinitialise(client);

    wsDataDestr(client->websocketData);

#ifdef _WIN32
    CloseHandle(ret->mutex);
#endif

    free(client);
}

void msbClientChangeURL(msbClient* client, char* url, char* origin, bool tls, char* client_cert, char* client_key, char* ca_cert){

    if (client == NULL) return;

    //prüfung ob verbunden und disconnect?
    wsDataDestr(client->websocketData);

    char* stdProt = "ws";
    char* stdAdr = NULL;
    char* stdPort = NULL;
    char* stdPath = "/websocket/data/websocket";

    char* proto = getProtocol(url, stdProt);
    char* address = getHostname(url, stdAdr);

    if (strncmp(proto, "wss", 3) == 0 && strlen(proto) == 3)
        stdPort = "8084";
    else if (strncmp(proto, "ws", 2) == 0)
        stdPort = "8085";
    else if (strncmp(proto, "https", 5) == 0)
        stdPort = "443";
    else if (strncmp(proto, "http", 4) == 0)
        stdPort = "80";
    else
        stdPort = "8085";

    char* port = getPort(url, stdPort);
    char* path = getPath(url, stdPath);

    //wsData* ws = wsDataConstr(0, address, port, address, path, tls, client_cert,
           //                    client_key, ca_cert, client->debugFunction);

    client->websocketData = wsDataConstr(0, address, port, address, path, origin, tls, client_cert,
                                         client_key, ca_cert, client->debugFunction);

    if (stdProt != NULL)
        if (strcmp(proto, stdProt) != 0)
            free(proto);
    if (stdAdr != NULL)
        if (strcmp(address, stdAdr) != 0)
            free(address);
    if (stdPort != NULL)
        if (strcmp(port, stdPort) != 0)
            free(port);
    if (stdPath != NULL)
        if (strcmp(path, stdPath) != 0)
            free(path);
}

void msbClientChangeAddress(msbClient* client, bool ipv6, char* address, char* port, char* hostname, char* path, char* origin, bool tls, char* client_cert, char* client_key, char* ca_cert){

    if (client == NULL) return;

    wsDataDestr(client->websocketData);

    client->websocketData = wsDataConstr(ipv6, address, port, hostname, path, origin, tls, client_cert,
                                         client_key, ca_cert, client->debugFunction);
}

#define if_free_duplicate(former, newer)            \
    if(former != NULL && newer != NULL){            \
        if(strcpy(former, newer)){                  \
            free(former);                           \
            former = string_duplicate(newer);       \
        }                                           \
    }

void msbClientChangeSelfDescription(msbClient* client, char* uuid, char* token, char* service_class, char* name, char* description){

    if (client == NULL) return;

    if_free_duplicate(client->msbObjectData->objectInfo.service_uuid, uuid);
    if_free_duplicate(client->msbObjectData->objectInfo.service_token, token);
    if_free_duplicate(client->msbObjectData->objectInfo.service_class, service_class);
    if_free_duplicate(client->msbObjectData->objectInfo.service_name, name);
    if_free_duplicate(client->msbObjectData->objectInfo.service_description, description);
}

void msbClientUseSockJSPath(msbClient* client, char* serverId, char* sessionId, char* transport) {

    if (client == NULL || serverId == NULL || sessionId == NULL || transport == NULL) return;

    char* p = (char*) calloc(1, sizeof(char) *
                                (strlen("/websocket/data/") + strlen(serverId) + strlen("/") + strlen(sessionId) +
                                 strlen("/") + strlen(transport) + 1));

    strcpy(p, "/websocket/data/");
    strcat(p, serverId);
    strcat(p, "/");
    strcat(p, sessionId);
    strcat(p, "/");
    strcat(p, transport);

    ((wsData*) client->websocketData)->connData->targetPath = p;

    if (client->debug) client->debugFunction("Msb client use SockJS path: using SockJS path %s\n", p);
}

void msbClientGenerateSockJSPath(msbClient* client) {
    int r = rand() % 1000;
    char ber[12] = {0};
    sprintf(ber, "%03u", r);

    char t[33] = { 0 };

#ifndef _WIN32
    uuid_t uuid;
    uuid_generate(uuid);
    char unp[37];
    uuid_unparse(uuid, unp);
    int i = 0, j = 0;
    for (; i < 37; ++i)
        if (unp[i] != '-')
            t[j++] = unp[i];
#else
    GUID uuid_gen;
    CoCreateGuid(&uuid_gen);

    sprintf_s(t, "%08lx%04hx%04hx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
        uuid_gen.Data1, uuid_gen.Data2, uuid_gen.Data3,
        uuid_gen.Data4[0], uuid_gen.Data4[1], uuid_gen.Data4[2], uuid_gen.Data4[3],
        uuid_gen.Data4[4], uuid_gen.Data4[5], uuid_gen.Data4[6], uuid_gen.Data4[7]);
#endif

    msbClientUseSockJSPath(client, ber, t, "websocket");
    client->sockJs_framing = true;
}

void msbClientSetSockJSFraming(msbClient* client, bool on_off) {
    client->sockJs_framing = on_off;
}

void msbClientSetMaxMessageSize(msbClient* client, uint32_t size) {
    client->maxMessageSize = size;
}

void msbClientSetEventCacheSize(msbClient* client, uint32_t size) {
    client->maxNumberDataIncoming = size;
}

void msbClientSetFunctionCacheSize(msbClient* client, uint32_t size) {
    client->maxNumberDataOutgoing = size;
}

bool msbClientAssignTLSCerts(msbClient* client, const char* client_certificate, const char* client_key,
                             const char* ca_certificate) {

    if (client == NULL || client_certificate == NULL || client_key == NULL) return false;

    ((wsData*) client->websocketData)->tls_client_cert = string_duplicate(client_certificate);
    ((wsData*) client->websocketData)->tls_client_key = string_duplicate(client_key);

    if (ca_certificate != NULL) ((wsData*) client->websocketData)->tls_ca_cert = string_duplicate(ca_certificate);

    return true;
}

void msbClientInitialise(msbClient* client, bool nopoll_debug) {
    wsData* data = (wsData*) client->websocketData;
    wsInitialise(data, nopoll_debug);
}

void msbClientConnect(msbClient* client) {
    wsConnect(client->websocketData);
}

int msbClientRegister(msbClient* client) {

    if (client->debug)
        client->debugFunction("Msb client register: Registering Service (Name: %s, UUID: %s)\n", client->msbObjectData->objectInfo.service_name,
               client->msbObjectData->objectInfo.service_uuid);

    int ret = 0;

    char* msg = string_combine("R ", msbObjectSelfDescription(client->msbObjectData));
    ret = msbClientSendText(client, msg, strlen(msg));
    free((void*) msg);

    return ret;
}

void msbClientDisconnect(msbClient* client) {
    wsDisconnect(client->websocketData);
}

void msbClientDeinitialise(msbClient* client) {
    wsDeinitialise(client->websocketData);
}

void
msbClientAddConfigParam(msbClient* client, char* cpName, PrimitiveType cpType, PrimitiveFormat cpFormat, void* value) {
    addConfigParam(client->msbObjectData, cpName, cpType, cpFormat, value);
}

void msbClientAddConfigParamFromObject(msbClient* client, json_object* object) {

    PrimitiveType cpType_t = MSB_INTEGER;
    PrimitiveFormat cpFormat_t = MSB_NONE;
    void* val = NULL;

    char* cpName = (char*) json_object_get_string(json_object_object_get(object, "Name"));
    char* cpType = (char*) json_object_get_string(json_object_object_get(object, "PrimitiveType"));
    char* cpFormat = (char*) json_object_get_string(json_object_object_get(object, "PrimitiveFormat"));

    if (strcmp(cpType, "MSB_INTEGER") == 0) {
        cpType_t = MSB_INTEGER;
        if (strcmp(cpFormat, "MSB_INT32") == 0) {
            cpFormat_t = MSB_INT32;
            val = calloc(1, sizeof(int32_t));
            *(int32_t*) val = json_object_get_int(json_object_object_get(object, "value"));
        } else if (strcmp(cpFormat, "MSB_INT64") == 0) {
            cpFormat_t = MSB_INT64;
            val = calloc(1, sizeof(int64_t));
            *(int64_t*) val = json_object_get_int64(json_object_object_get(object, "value"));
        }
    } else if (strcmp(cpType, "MSB_NUMBER") == 0) {
        cpType_t = MSB_NUMBER;
        if (strcmp(cpFormat, "MSB_FLOAT") == 0) {
            cpFormat_t = MSB_FLOAT;
            val = calloc(1, sizeof(float));
            *(float*) val = (float) json_object_get_double(json_object_object_get(object, "value"));
        } else if (strcmp(cpFormat, "MSB_DOUBLE") == 0) {
            cpFormat_t = MSB_DOUBLE;
            val = calloc(1, sizeof(double));
            *(double*) val = json_object_get_double(json_object_object_get(object, "value"));
        }
    } else if (strcmp(cpType, "MSB_STRING") == 0) {
        cpType_t = MSB_STRING;
        if (strcmp(cpFormat, "MSB_NONE") == 0) {
            cpFormat_t = MSB_NONE;
            const char* tmp = json_object_get_string(json_object_object_get(object, "value"));
            val = calloc(1, (strlen(tmp) + 1) * sizeof(char));
            strcpy(val, tmp);
        }
    } else if (strcmp(cpType, "MSB_BOOL") == 0) {
        cpType_t = MSB_BOOL;
        if (strcmp(cpFormat, "MSB_NONE") == 0) {
            cpFormat_t = MSB_NONE;
            val = calloc(1, sizeof(bool));
            json_bool b = json_object_get_boolean(json_object_object_get(object, "value"));
            *(bool*) val = (b == 1);
        }
    } else if (strcmp(cpType, "MSB_ARRAY") == 0) {
        cpType_t = MSB_ARRAY;
        cpFormat_t = MSB_NONE;
    } else
        return;

    if (val == NULL)
        return;

    msbClientAddConfigParam(client, cpName, cpType_t, cpFormat_t, val);
}

void msbClientAddConfigParamFromString(msbClient* client, char* string) {
    json_object* p = json_tokener_parse(string);

    if (p != NULL) msbClientAddConfigParamFromObject(client, p);
}

/* EXPERIMENTAL */
void msbClientRemoveConfigParam(msbClient* client, char* cpName) {
    removeConfigParam(client->msbObjectData, cpName);
}

void msbClientChangeConfigParamValue(msbClient* client, char* cpName, void* value) {
    changeConfigParamValue(client->msbObjectData, cpName, value);
}

void msbClientAddFunction(msbClient* client, char* fId, char* fName, char* fDesc, void(*fPtr)(void*, void*, void*),
                          PrimitiveType fType, PrimitiveFormat fFormat, bool isArray, void* contextPtr) {
    addFunction(client->msbObjectData, fId, fName, fDesc, fPtr, fType, fFormat, isArray, contextPtr);
}

void msbClientAddEmptyFunction(msbClient* client, char* fId, char* fName, char* fDesc, void(*fPtr)(void*, void*, void*),
                               void* contextPtr) {
    addEmptyFunction(client->msbObjectData, fId, fName, fDesc, fPtr, contextPtr);
}

void msbClientAddFunctionFromString(msbClient* client, char* fId, char* fName, char* fDesc, void(*fPtr)(void*, void*, void*),
                                    char* dataFormat, void* contextPtr) {
    json_object* obj = json_object_new_object();
    obj = json_tokener_parse(dataFormat);

    if (obj != NULL) addComplexFunction(client->msbObjectData, fId, fName, fDesc, fPtr, obj, contextPtr);
}

void msbClientAddComplexFunction(msbClient* client, char* fId, char* fName, char* fDesc, void(*fPtr)(void*, void*, void*),
                                 json_object* dataFormat, void* contextPtr) {
    addComplexFunction(client->msbObjectData, fId, fName, fDesc, fPtr, dataFormat, contextPtr);
}

/* EXPERIMENTAL */
void msbClientRemoveFunction(msbClient* client, char* fId){
    removeFunction(client->msbObjectData, fId);
}

void msbClientAddResponseEventToFunction(msbClient* client, char* fId, char* eId) {
    addResponseEventToFunction(client->msbObjectData, fId, eId);
}

/* EXPERIMENTAL */
void msbClientRemoveResponseEventFromFunction(msbClient* client, char* fId, char* eId) {
    removeResponseEventFromFunction(client->msbObjectData, fId, eId);
}

void msbClientAddEvent(msbClient* client, char* eId, char* eName, char* eDesc, PrimitiveType dObjectType,
                       PrimitiveFormat dObjectFormat, bool isArray) {
    addEvent(client->msbObjectData, eId, eName, eDesc, dObjectType, dObjectFormat, isArray);
}

void msbClientAddEmptyEvent(msbClient* client, char* eId, char* eName, char* eDesc) {
    addEmptyEvent(client->msbObjectData, eId, eName, eDesc);
}

void
msbClientAddEventFromString(msbClient* client, char* eId, char* eName, char* eDesc, char* dataFormat, bool isArray) {
    json_object* obj = json_tokener_parse(dataFormat);
    msbClientAddComplexEvent(client, eId, eName, eDesc, obj, isArray);
}

void msbClientAddComplexEvent(msbClient* client, char* eId, char* eName, char* eDesc, json_object* dataFormat,
                              bool isArray) {
    addComplexEvent(client->msbObjectData, eId, eName, eDesc, dataFormat, isArray);
}

/* EXPERIMENTAL */
void msbClientRemoveEvent(msbClient* client, char* eId) {
    removeEvent(client->msbObjectData, eId);
}

int msbClientSendText(msbClient* client, const char* msg, size_t msg_length) {

    if (client == NULL) return -1;

    int ret;

    if (client->sockJs_framing) {
        /*int j = 0, i = 0;
        for (; i < strlen(msg); ++i)
            if (msg[i] == '"')
                ++j;

        size_t l = strlen("[\"") + strlen(msg) + j + strlen("\"]") + 1;

        char msg[l];
        j = 0;

        msg[j++] = 0x5b;
        msg[j++] = 0x22;

        for (i = 0; i < strlen(msg); ++i)
            if (msg[i] == '"') {
                msg[j++] = 0x5c;
                msg[j++] = 0x22;
            } else
                msg[j++] = msg[i];

        msg[j++] = 0x22;
        msg[j++] = 0x5d;
        msg[j] = 0;*/

        char* msg_t;

        if (!string_replace((char*)msg, "\"", "\\\"", &msg_t)) {
            char* tmp_msg1 = string_combine("[\"", msg_t);
            char* tmp_msg2 = string_combine(tmp_msg1, "\"]");

            ret = wsSend(client->websocketData, tmp_msg2, strlen(tmp_msg2));

            if(tmp_msg1 != NULL) free(tmp_msg1);

            if(tmp_msg2 != NULL) free(tmp_msg2);

        } else {
            ret = -1;
        }

        if(msg_t != NULL) free(msg_t);

    } else
        ret = wsSend(client->websocketData, msg, msg_length);

    return ret;

}

void msbClientSetStateMachineCyclePause(msbClient* client, uint16_t pause_in_ms) {
    client->pause_automat = pause_in_ms * (uint16_t) 1000;
}

void msbClientSetReconnectInterval(msbClient* client, uint16_t interval_in_ms) {
    client->interval_reconnect = interval_in_ms * (uint16_t) 1000;
}

void msbClientEnableHostnameVerification(msbClient* client) {
    wsSetTLSVerification(client->websocketData, 1);
}

void msbClientDisableHostnameVerification(msbClient* client) {
    wsSetTLSVerification(client->websocketData, 0);
}

bool msbClientConnectionReady(msbClient* client) {
    return wsIsConnectionReady(client->websocketData, 1000000);
}

void msbClientEnableReconnect(msbClient* client) {
    client->flag_reconnect = true;
}

void msbClientDisableReconnect(msbClient* client) {
    client->flag_reconnect = false;
}

void msbClientEnableEventCache(msbClient* client) {
    client->dataOutCache = 1;
}

void msbClientDisableEventCache(msbClient* client) {
    client->dataOutCache = 0;
}

void msbClientSetCommonCallbackFunction(msbClient* client, void(*fPtr)(void*, void*)) {
    client->commonCallBackFunction = fPtr;
}

void msbClientRemoveCommonCallbackFunction(msbClient* client) {
    client->commonCallBackFunction = NULL;
}

void msbClientSetConfigParamCallbackFunction(msbClient* client, void(*fPtr)(void*, void*)) {
    client->configParamFunction = fPtr;
}

void msbClientRemoveConfigParamCallbackFunction(msbClient* client) {
    client->configParamFunction = NULL;
}

void msbClientFwdCompleteFunctionCall(msbClient* client) {
    client->fwdCompleteFunctionCallData = true;
}

void msbClientFwdParametersOnlyFunctionCall(msbClient* client) {
    client->fwdCompleteFunctionCallData = false;
}

void msbClientSetDebug(msbClient* client, bool on_off) {
    client->debug = on_off;
    ((wsData*)client->websocketData)->debugFunction = NULL;
}

void msbClientSetDebugFunction(msbClient* client, int (*function)(const char*, ...)){
    client->debugFunction = function;
    ((wsData*)client->websocketData)->debugFunction = function;
}
