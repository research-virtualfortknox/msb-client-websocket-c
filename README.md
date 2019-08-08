<p align="center">
  <a href="https://research.virtualfortknox.de" target="_blank" rel="noopener noreferrer">
    <img src="https://research.virtualfortknox.de/static/cms/img/vfk_research_logo.png" alt="VFK Research Logo" height="70" >
  </a>
</p>



# MSB websocket client library for C and C++

[![Build Status](https://travis-ci.org/research-virtualfortknox/msb-client-websocket-c.svg?branch=master)](https://travis-ci.org/research-virtualfortknox/msb-client-websocket-c) [![codecov](https://codecov.io/gh/research-virtualfortknox/msb-client-websocket-c/branch/master/graph/badge.svg)](https://codecov.io/gh/research-virtualfortknox/msb-client-websocket-c)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fresearch-virtualfortknox%2Fmsb-client-websocket-c.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Fresearch-virtualfortknox%2Fmsb-client-websocket-c?ref=badge_shield)

**Compatibility Matrix**

Client version compatibility to MSB versions:

| | **1.5.x-RELEASE** |
|---|:---:|
| 1.0.x       | x |

## Welcome

If you want to contribute, please read the [Contribution Guidelines](.github/CONTRIBUTING.md).

If you want to test this client, use the [example application](test/src/example.c).

If you want to know how to use this client in your own project, read below.

## What is VFK MSB

TODO: Link to general documentation about VFK MSB

## Prerequisites

To build the client library, you need the following libraries:

* json-c
* nopoll
* uriparser

Easiest way to set up your machine on Linux/Ubuntu is to run the [Machine preparation script](c_client_prepare_machine.sh) in the project's root folder. It will also install necessary libraries for the test binaries.

## Building the library

Both library and test/example applications are built with CMake. Change the CMakeLists.txt according to your needs. At the time, both CMakeLists.txt include flags for gcov code coverage examination and do not include optimisation.

```
mkdir build  
cd build  
cmake ..  
make -j4
```

You may also run the [build script](c_client_build_cmake.sh) with sudo and a "-v 1.5.0" flag to build the library and test/example applications. This will also install the library under /usr/local/library.

## Creating a client

There are two functions for creating a client.  
You must provide  
* basic self service_description (service_uuid, service_name, service_description, class and service_token of the service),
* address parameters (either url (msbClientNewClientURL) OR ((ip-address  (msbClientNewClient) OR hostname (msbClientNewClient)) AND port. Path only if it's necessary for the targeted MSB instance, otherwise insert NULL),
* connection parameters (IPv6? TLS on/off? Certificates for TLS?)

```c
#define UUID "cde99fb5-32e1-4f1e-b358-b743b2e4a779"
#define NAME "c-client"
#define DESCRIPTION "a service_description"
#define CLASS "SmartObject" //or "Application"
#define TOKEN "b743b2e4a779"

msbClient* msbClientNewClientURL(char* url, char* service_uuid, char* service_token, char* sclass, char* service_name, char* service_description, bool tls, char* client_cert, char* client_key, char* ca_cert);

msbClient* msbClient = msbClientNewClientURL( "ws://10.3.2.123:8085", UUID, TOKEN, CLASS, NAME, DESCRIPTION, false, NULL, NULL, NULL)

msbClient* msbClientNewClient(bool ipv6, char* address, char* port, char* hostname, char* path,  
							   char* service_uuid, char* service_token, char* sclass, char* service_name, char* service_description,
							   bool tls, char* client_cert, char* client_key, char* ca_cert);  

msbClient* msbClient = msbClientNewClientURL( false, "10.3.2.123", "8085", NULL, NULL, UUID, TOKEN, CLASS, NAME, DESCRIPTION, false, NULL, NULL, NULL)
```

## Configuring the client

Usually, the MSB requires a SockJS format path, which you can generate by executing
```c
msbClientGenerateSockJSPath(msbClient);
```

Activating debug output  (printf is the standard output function)
```c
msbClientSetDebug(msbClient, true);
```

You can redirect your debug output by pointing to your own output function
```c
void msbClientSetDebugFunction(msbClient* client, int (*function)(const char*, ...));
```

To receive the complete function call data, when one of your service's functions is called, set
```c
msbClientFwdCompleteFunctionCall(msbClient);
```
Otherwise your function will only receive the data object part of the function call (which means no service_uuid and no correlation id information).

## Adding configuration parameters

You can add configuration parameters to your self-service_description by using the msbClientAddConfigParam function. The client will not copy the variable. When the client receives new parameter data from the MSB, it will change the variable's content you're pointing to.
```c
int bl = 0;
msbClientAddConfigParam(msbClient, "p6", MSB_BOOL, MSB_NONE, &bl);

int32_t wert = 123456;
msbClientAddConfigParam(msbClient, "p1", MSB_INTEGER, MSB_INT32, &wert);

char* string = (char*)calloc(1, sizeof(char)*6);
strcpy(string, "hallo");
msbClientAddConfigParam(msbClient, "p2", MSB_STRING, MSB_NONE, string);

int64_t wert2 = 3000000000;
msbClientAddConfigParam(msbClient, "p3", MSB_INTEGER, MSB_INT64, &wert2);

float fl = 123.456f;
msbClientAddConfigParam(msbClient, "p4", MSB_NUMBER, MSB_FLOAT, &fl);

double d = 321.123456789;
msbClientAddConfigParam(msbClient, "p5", MSB_NUMBER, MSB_DOUBLE, &d);
```

You can also add configuration parameters via json string or json object. String or object must provide service_name, type, format and value of a configuration parameter.
```c
void msbClientAddConfigParamFromObject(msbClient* client, json_object* object);
void msbClientAddConfigParamFromString(msbClient* client, char* string);
```

To manually change a configuration parameters value, use
```c
void msbClientChangeConfigParamValue(msbClient* client, char* cpName, void* value);
```

## Adding events

To add events, you must use on of the addevent-functions
```c
//adding an empty event without a dataobject. Provide client, event id, event service_name, event service_description
msbClientAddEmptyEvent(msbClient, "Ev0", "Event0", "beschreibung");
//adding an simple event with a single integer as dataobject. Provide client, event id, event service_name, event service_description, data type, data format, array flag
msbClientAddEvent(msbClient, "Ev1", "Event1", "beschreibung", MSB_INTEGER, MSB_INT32, false);
//adding an array event with a string array as dataobject. Provide client, event id, event service_name, event service_description, data type, data format, array flag
msbClientAddEvent(msbClient, "Ev2", "Event2", "beschreibung", MSB_STRING, MSB_NONE, true);
```

To add events with a complex data structure, you may use
```c
void msbClientAddEventFromString(msbClient* client, char* eId, char* eName, char* eDesc, char* eFormat, bool isArray);
void msbClientAddComplexEvent(msbClient* client, char* eId, char* eName, char* eDesc, json_object* eFormat, bool isArray);
```
Lookup the example.c for an example on how to use those functions.

## Adding functions

### Implementing the callback function

```c
//the function must provide two void* parameters. first one will receive either the data object of the function call or the complete function call (depends on your setting)
void callback_function(void* inp, void* context){

    //with msbClientFwdCompleteFunctionCall(msbClient* client)
    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters");
    //without msbClientFwdCompleteFunctionCall(msbClient* client)
    json_object* obj = (json_object*)inp

    //if your function's data object consists of a single simple datatype we can directly dissolve the data object, otherwise we must use the json-c-functions to disassemble the function call 
    int32_t int_obj;
    int_obj = json_object_get_int(json_object_object_get(obj, "dataObject"));

    //if your function's data object consists of several entries, an example would be
    int32_t int_obj;
    double d;
    int_obj = json_object_get_int(json_object_object_get(json_object_object_get(obj, "dataObject"), "int"));
    d = json_object_get_double(json_object_object_get(json_object_object_get(obj, "dataObject"), "double"));

    //this function call has a response event
    //to use this functionality you must set the calling client as context pointer when adding the function
    msbClient* client = (msbClient*)context;
    int resp = 123;
    while( client->dataOutInterfaceFlag == 1){
        usleep(100);
    }
    msbClientPublish(client, "Ev1", MEDIUM, &resp, 0, (char*)json_object_get_string(json_object_object_get(obj, "correlationId"))); //ResponseEvent Ev1 schmeißen, correlationId aus dem Funktionsaufruf durchreichen
}
```

### Adding the callback function to the self-service_description

To add functions, you must use on of the addfunction-functions
```c
//empty function (without dataobject). provide client, function id, function service_name, function service_description, pointer to your function, a pointer to context data you want to be given to your function when called
msbClientAddEmptyFunction(msbClient, "F0", "Function0", "service_description", &testFunctionEmpty, NULL);
//function with simple dataobject (integer). provide client, function id, function service_name, function service_description, pointer to your function, datatype, data format, array length (0 if no array), a pointer to context data you want to be given to your function when called
msbClientAddFunction(msbClient, "F1", "Function1", "service_description", &callback_function, MSB_INTEGER, MSB_INT32, 0, msbClient);
```

To add functions with a complex data structure, you may use
```c
void msbClientAddFunctionFromString(msbClient* client, char* fId, char* fName, char* fDesc, void(* fPtr)(void*, void*), char* fFormat, void* contextPtr);
void msbClientAddComplexFunction(msbClient* client, char* fId, char* fName, char* fDesc, void(* fPtr)(void*, void*), json_object* fFormat, void* contextPtr);
```
Lookup the example.c for an example on how to use those functions.

If you want to publish a response event on a function call, you must add this to the service's self service_description using
```c
//provide client, function id and event id (= will be added as response event to the service_description of the function)
msbClientAddResponseEventToFunction(msbClient, "F1", "Ev1");
```
You can add several response events to one function.

## Running your client

### Starting

Start the client in an own thread with  
```c
msbClientRunClientStateMachine(msbClient);
```

### Publishing events

First, you should look up whether the client's state machine already picked up your last event or not

```c
while( msbClient->dataOutInterfaceFlag == 1){
  usleep(50000);
}
```

After that you can publish events using the provided publish-functions. You must provide client, event id, priority, data or data object, array length, and if you wish, a pointer to a correlation id.
```c
//publishing an empty event (without actual data)
//on all publish-functions, you may provide a correlation id over the last parameter
msbClientPublishEmpty(msbClient, "Ev0", HIGH, NULL);

//publishing a simple event, providing one integer (0 --> no array)
int32_t a = 15;
msbClientPublish(msbClient, "Ev1", HIGH, &a, 0, NULL);

//publishing a string array event, providing two strings (array length = 2)
char* stringArray[] = {(char*)"string1", (char*)"string2"};
msbClientPublish(msbClient, "Ev2", HIGH, &stringArray, 2, NULL);

//publishing a complex event, providing a json_object that defines the event's data object
json_object *dataObjectA;
dataObjectA = json_object_new_object();
json_object_object_add(dataObjectA, "int", json_object_new_int(100));
json_object_object_add(dataObjectA, "double", json_object_new_double(123.321));
msbClientPublishComplex(msbClient, "Ev3", HIGH, dataObjectA, NULL);

//publishing a complex event, providing a json string that defines the event's data object
char* str2 = "{\"SimpleEv\":{\"myInteger\":150}}";
msbClientPublishFromString(msbClient, "Ev4", HIGH, str2, NULL);
```

### Adding configuration parameters, functions and events during runtime

You can add configuration parameters, function and events at every time. However, you must re-register after adding, so the self-service_description of the client is updated.
Do so by calling
```c
msbClientRegister(msbClient);
```

### Stopping

Stop the client and its own thread with
```c
msbClientHaltClientStateMachine(msbClient);
```


## License
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fresearch-virtualfortknox%2Fmsb-client-websocket-c.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Fresearch-virtualfortknox%2Fmsb-client-websocket-c?ref=badge_large)