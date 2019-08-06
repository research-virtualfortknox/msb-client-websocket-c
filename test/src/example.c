/*
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
 */

#include "libMsbClientC.h"
#include <string.h>

#define UUID "dba"
#define NAME "nopollCClient"
#define DESCRIPTION "bschr"
#define CLASS "SmartObject"
#define TOKEN "sfsdfdf"

#ifndef _WIN32
#include <uuid/uuid.h>
#include <unistd.h>
#else
#include <objbase.h>
#endif

/*
volatile int threadC = 0;

#ifndef _WIN32
void func2(void* inp) {
#else
DWORD WINAPI func2(void* inp) {
#endif


	threadC--;
#ifndef _WIN32
	pthread_exit(NULL);
#endif
}


void func1(void* inp){

	if(threadC >= 4)
		return;
#ifndef _WIN32
	pthread_t thread;

	if(pthread_create(&thread, NULL, (void*)func2, inp) == 0)
		threadC++;
#else
	HANDLE thread = CreateThread(NULL, 0, func2, inp, 0, NULL);

	return 0;
#endif
}
*/


void testFunction(void* client, void* inp, void* context){
    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;

    int_obj = json_object_get_int(json_object_object_get(obj, "dataObject"));

    printf("testFunction gerufen: %i\n", int_obj);
}

void testFunctionEmpty(void* client, void* inp, void* context){
    printf("testFunctionEmpty gerufen\n");
}

void testFunctionComplex(void* client, void* inp, void* context){
    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;
    double d;

    int_obj = json_object_get_int(json_object_object_get(json_object_object_get(obj, "dataObject"), "int"));
    d = json_object_get_double(json_object_object_get(json_object_object_get(obj, "dataObject"), "double"));

    printf("testFunctionComplex gerufen: %i, %f\n", int_obj, d);

}

void testFunctionFromString(void* client, void* inp, void* context){
    msbClient* msbClient = client;

    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;

    int_obj = json_object_get_int(json_object_object_get(json_object_object_get(obj, "dataObject"), "someInt"));

    printf("testFunctionFromString gerufen: %i\n", int_obj);

    int antwort = 123;
    while( msbClient->dataOutInterfaceFlag == 1){
        usleep(100);
    }
    msbClientPublish(msbClient, "Ev1", MEDIUM, &antwort, 0, (char*)json_object_get_string(json_object_object_get(obj, "correlationId"))); //ResponseEvent Ev1 schmeißen, correlationId aus dem Funktionsaufruf durchreichen
}

int main(int argc, char **argv){

    msbClient* msbClient = msbClientNewClient(false, "10.15.26.11", "8085", NULL, "/websocket/data/websocket", NULL, UUID, TOKEN, CLASS, NAME, DESCRIPTION, false, NULL, NULL, NULL);

    //msbClient* msbClient = msbClientNewClientURL("http://ws.msb.ipa.cell.vfk.fraunhofer.de", NULL, UUID, TOKEN, CLASS, NAME, DESCRIPTION, false, NULL, NULL, NULL);

    //msbClientChangeURL(msbClient, "http://ws.msb.ipa.cell.vfkl.fraunhofer.de", NULL, false, NULL, NULL, NULL);

    msbClientGenerateSockJSPath(msbClient);

    msbClientSetDebug(msbClient, true);

    msbClientSetFunctionCacheSize(msbClient, 100);
    msbClientSetEventCacheSize(msbClient, 100);
    msbClientEnableEventCache(msbClient);

    int bl = 0;
    msbClientAddConfigParam(msbClient, "p6", MSB_BOOL, MSB_NONE, &bl);

    int32_t wert = 123456;
    msbClientAddConfigParam(msbClient, "p1", MSB_INTEGER, MSB_INT32, &wert);

    char* string = (char*)malloc(sizeof(char)*6);
    strcpy(string, "hallo\0");
    msbClientAddConfigParam(msbClient, "p2", MSB_STRING, MSB_NONE, string);

    int64_t wert2 = 3000000000;
    msbClientAddConfigParam(msbClient, "p3", MSB_INTEGER, MSB_INT64, &wert2);

    float fl = 123.456f;
    msbClientAddConfigParam(msbClient, "p4", MSB_NUMBER, MSB_FLOAT, &fl);

    double d = 321.123456789;
    msbClientAddConfigParam(msbClient, "p5", MSB_NUMBER, MSB_DOUBLE, &d);

    msbClientAddEmptyEvent(msbClient, "Ev0", "Event0", "beschreibung");
    msbClientAddEvent(msbClient, "Ev1", "Event1", "beschreibung", MSB_INTEGER, MSB_INT32, false);
    msbClientAddEvent(msbClient, "Ev2", "Event2", "beschreibung", MSB_STRING, MSB_NONE, true);

    json_object* dataformat = json_object_new_object();

    json_object *dataObject;
    dataObject = json_object_new_object();
    json_object_object_add(dataformat, "dataObject", dataObject);
    json_object_object_add(dataObject, "$ref", json_object_new_string("#/definitions/complexevent"));

    json_object* event = json_object_new_object();
    json_object_object_add(event, "type", json_object_new_string("object"));
    json_object_object_add(event, "additionalProperties", json_object_new_boolean(false));

    json_object* required = json_object_new_array();
    json_object_array_add(required, json_object_new_string("int"));
    json_object_array_add(required, json_object_new_string("double"));

    json_object_object_add(event, "required", required);

    json_object* properties = json_object_new_object();
    json_object* prop1 = json_object_new_object();
    json_object_object_add(prop1, "type", json_object_new_string("integer"));
    json_object_object_add(prop1, "format", json_object_new_string("int32"));
    json_object* prop2 = json_object_new_object();
    json_object_object_add(prop2, "type", json_object_new_string("number"));
    json_object_object_add(prop2, "format", json_object_new_string("double"));

    json_object_object_add(properties, "int", prop1);
    json_object_object_add(properties, "double", prop2);
    json_object_object_add(event, "properties", properties);

    json_object_object_add(dataformat, "complexevent", event);

    msbClientAddComplexEvent(msbClient, "Ev3", "Event3", "beschreibung", dataformat, false);

    char* str = "{\"dataObject\": {"
                "\"$ref\": \"#/definitions/NestedEvent\""
                "},"
                "\"NestedEvent\": {"
                "\"type\": \"object\","
                "\"additionalProperties\": false,"
                "\"required\": ["
                "\"someInt\""
                "],"
                "\"properties\": {"
                "\"SimpleEv\": {"
                "\"$ref\": \"#/definitions/SimpleEvent\""
                "},"
                "\"Si2\": {"
                "\"$ref\": \"#/definitions/SimpleEvent\""
                "},"
                "\"someInt\": {"
                "\"type\": \"integer\","
                "\"format\": \"int32\""
                "},"
                "\"allTypesObject\": {"
                "\"$ref\": \"#/definitions/AllDataTypesObject\""
                "}"
                "}"
                "},"
                "\"SimpleEvent\": {"
                "\"type\": \"object\","
                "\"additionalProperties\": false,"
                "\"required\": ["
                "\"myInteger\","
                "\"myFloat\","
                "\"myDouble\""
                "],"
                "\"properties\": {"
                "\"myInteger\": {"
                "\"type\": \"integer\","
                "\"format\": \"int32\""
                "},"
                "\"myFloat\": {"
                "\"type\": \"number\","
                "\"format\": \"double\""
                "},"
                "\"myDouble\": {"
                "\"type\": \"number\","
                "\"format\": \"double\""
                "},"
                "\"myString\": {"
                "\"type\": \"string\""
                "}"
                "}"
                "},"
                "\"AllDataTypesObject\": {"
                "\"type\": \"object\","
                "\"additionalProperties\": false,"
                "\"required\": ["
                "\"simpleByte\","
                "\"simpleChar\","
                "\"simpleInt16\","
                "\"simpleInt32\","
                "\"simpleInt64\","
                "\"simpleFloat\","
                "\"simpleDouble\","
                "\"simpleDateTime\""
                "],"
                "\"properties\": {"
                "\"simpleString\": {"
                "\"type\": \"string\""
                "},"
                "\"simpleByte\": {"
                "\"type\": \"integer\","
                "\"format\": \"byte\""
                "},"
                "\"simpleChar\": {"
                "\"type\": \"string\""
                "},"
                "\"simpleInt16\": {"
                "\"type\": \"integer\""
                "},"
                "\"simpleInt32\": {"
                "\"type\": \"integer\","
                "\"format\": \"int32\""
                "},"
                "\"simpleInt64\": {"
                "\"type\": \"integer\","
                "\"format\": \"int64\""
                "},"
                "\"simpleFloat\": {"
                "\"type\": \"number\","
                "\"format\": \"double\""
                "},"
                "\"simpleDouble\": {"
                "\"type\": \"number\","
                "\"format\": \"double\""
                "},"
                "\"simpleDateTime\": {"
                "\"type\": \"string\","
                "\"format\": \"date-time\""
                "},"
                "\"stringList\": {"
                "\"type\": \"array\","
                "\"items\": {"
                "\"type\": \"string\""
                "}"
                "},"
                "\"dictionaryStringDouble\": {"
                "\"type\": \"object\","
                "\"additionalProperties\": {"
                "\"type\": \"number\","
                "\"format\": \"double\""
                "}"
                "}"
                "}"
                "}"
                "}";

    msbClientAddEventFromString(msbClient, "Ev4", "Event4", "beschreibung", str, 0);

    msbClientAddEmptyFunction(msbClient, "F0", "Function0", "beschreibung", &testFunctionEmpty, NULL);
    msbClientAddFunction(msbClient, "F1", "Function1", "beschreibung", &testFunction, MSB_INTEGER, MSB_INT32, 0, NULL);
    msbClientAddComplexFunction(msbClient, "F2", "Function2", "beschreibung", &testFunctionComplex, dataformat, NULL);
    msbClientAddFunctionFromString(msbClient, "F3", "Function3", "beschreibung", &testFunctionFromString, str, msbClient); //der Client als Kontext, damit wir die nötigen Informationen für das ResponseEvent haben
    msbClientAddResponseEventToFunction(msbClient, "F3", "Ev1"); //ResponseEvent Ev1 zur Funktion F3 hinzufügen

    msbClientFwdCompleteFunctionCall(msbClient); //beim Funktionsaufruf wollen wir nicht nur Parameter erhalten, sondern den gesamten Aufruf vom MSB; nur so haben wir auch die CorrelationId, die wir beim ResponseEvent mitgeben müssen

    msbClientRunClientStateMachine(msbClient); //automat in thread

    usleep(10000000);

    unsigned int i = 0;
    for(; i < 50; i++){
        msbClientPublishEmpty(msbClient, "Ev0", HIGH, NULL);

        while( msbClient->dataOutInterfaceFlag == 1){
            usleep(50000);
        }

        int32_t a = 15;

        msbClientPublish(msbClient, "Ev1", HIGH, &a, 0, NULL);

        while( msbClient->dataOutInterfaceFlag == 1){
            usleep(50000);
        }

        char* stringArray[] = {(char*)"string1", (char*)"string2"};
        msbClientPublish(msbClient, "Ev2", HIGH, &stringArray, 2, NULL);

        while( msbClient->dataOutInterfaceFlag == 1){
            usleep(50000);
        }

        json_object *dataObjectA;
        dataObjectA = json_object_new_object();
        json_object_object_add(dataObjectA, "int", json_object_new_int(100));
        json_object_object_add(dataObjectA, "double", json_object_new_double(123.321));

        msbClientPublishComplex(msbClient, "Ev3", HIGH, dataObjectA, NULL);

        while( msbClient->dataOutInterfaceFlag == 1){
            usleep(50000);
        }

        char* str2 = "{\"SimpleEv\":{\"myInteger\":150}}";
        msbClientPublishFromString(msbClient, "Ev4", HIGH, str2, NULL);

        while( msbClient->dataOutInterfaceFlag == 1){
            usleep(50000);
        }

        usleep(100000);
    }

    msbClientHaltClientStateMachine(msbClient);

    return 0;
}
