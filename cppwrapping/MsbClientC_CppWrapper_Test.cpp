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

#include "MsbClientC_CppWrapper.hpp"
#include "json.h"
#include "cstring"

#define UUID "444488885555"
#define NAME "nopollCClient"
#define DESCRIPTION "bschr"
#define CLASS "SmartObject"
#define TOKEN "123456"

#ifndef _WIN32
#include <unistd.h>
#else
#include <objbase.h>
#endif

using namespace MsbClientC_CPPInt;

void testFunction(void* inp, void* context){
    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;

    int_obj = json_object_get_int(json_object_object_get(obj, "dataObject"));

    printf("testFunction gerufen: %i\n", int_obj);
}

void testFunctionEmpty(void* inp, void* context){
	printf("testFunctionEmpty gerufen\n");
}

void testFunctionComplex(void* inp, void* context){
    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;
    double d;

    int_obj = json_object_get_int(json_object_object_get(json_object_object_get(obj, "dataObject"), "int"));
    d = json_object_get_double(json_object_object_get(json_object_object_get(obj, "dataObject"), "double"));

    printf("testFunctionComplex gerufen: %i, %f\n", int_obj, d);

}

void testFunctionFromString(void* inp, void* context){
    MsbClient* client = (MsbClient*)context;

    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;

    int_obj = json_object_get_int(json_object_object_get(json_object_object_get(obj, "dataObject"), "someInt"));

    printf("testFunctionFromString gerufen: %i\n", int_obj);

    int antwort = 123;
    while( *(client->dataOutInterfaceFlag) == 1){
        usleep(100);
    }

    client->Publish((std::string)"Ev1", MEDIUM, &antwort, 0, (char*)json_object_get_string(json_object_object_get(obj, "correlationId"))); //ResponseEvent Ev1 schmeißen, correlationId aus dem Funktionsaufruf durchreichen
}

int main(){

	MsbClient* msbClient = new MsbClient( "ws://172.21.5.70:8085", UUID, TOKEN, CLASS, NAME, DESCRIPTION, 0, std::string(""), "", "");
	msbClient->GenerateSockJSPath();
	msbClient->SetSockJSFraming(true);
	msbClient->SetDebug(true);

	int bl = 0;
	msbClient->AddConfigParam((std::string)"p6", MSB_BOOL, MSB_NONE, (void*)&bl);

	int32_t wert = 123456;
	msbClient->AddConfigParam((std::string)"p1", MSB_INTEGER, MSB_INT32, (void*)&wert);

	char* string = (char*)malloc(sizeof(char)*6);
	strcpy(string, "hallo\0");
	msbClient->AddConfigParam((std::string)"p2", MSB_STRING, MSB_NONE, string);

	int64_t wert2 = 3000000000;
	msbClient->AddConfigParam((std::string)"p3", MSB_INTEGER, MSB_INT64, &wert2);

	float fl = 123.456f;
	msbClient->AddConfigParam((std::string)"p4", MSB_NUMBER, MSB_FLOAT, &fl);

	double d = 321.123456789;
	msbClient->AddConfigParam((std::string)"p5", MSB_NUMBER, MSB_DOUBLE, &d);

	msbClient->AddEmptyEvent((std::string)"Ev0", "Event0", "beschreibung");
	msbClient->AddEvent((std::string)"Ev1", "Event1", "beschreibung", MSB_INTEGER, MSB_INT32, false);
	msbClient->AddEvent((std::string)"Ev2", "Event2", "beschreibung", MSB_STRING, MSB_NONE, true);

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

	msbClient->AddComplexEvent((std::string)"Ev3", "Event3", "beschreibung", dataformat, false);


    std::string str = "{\"dataObject\": {"
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

	msbClient->AddEventFromString((std::string)"Ev4", "Event4", "beschreibung", str, 0);

	msbClient->AddEmptyFunction((std::string)"F0", "Function0", "beschreibung", &testFunctionEmpty, NULL);
	msbClient->AddFunction((std::string)"F1", "Function1", "beschreibung", &testFunction, MSB_INTEGER, MSB_INT32, 0, NULL);
	msbClient->AddComplexFunction((std::string)"F2", "Function2", "beschreibung", &testFunctionComplex, dataformat, NULL);
	msbClient->AddFunctionFromString((std::string)"F3", "Function3", "beschreibung", &testFunctionFromString, str, msbClient);
	msbClient->AddResponseEventToFunction((std::string)"F3", "Ev1"); //ResponseEvent Ev1 zur Funktion F3 hinzufügen

	msbClient->FwdCompleteFunctionCall(); // gesamten Funktionsaufruf erhalten, nötig, um die correlationEvent-Id auslesen zu können

	msbClient->RunClientStateMachine(); //automat in thread

	usleep(1000000);

	for(int i = 0; i < 50; i++){
		//msbClient->PublishEmpty("Ev0", HIGH, "");

	    std::string str2 = "{\"SimpleEv\":{\"myInteger\":150}}";
	    msbClient->PublishFromString((std::string)"Ev4", HIGH, str2, "");

		while( *(msbClient->dataOutInterfaceFlag) == 1){
			usleep(1000000);
		}
	}

	msbClient->HaltClientStateMachine();

	delete msbClient;

	return 0;
}

