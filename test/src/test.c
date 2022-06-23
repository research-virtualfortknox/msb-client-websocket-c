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

#include <stdio.h>

#include <uuid/uuid.h>
#include <unistd.h>

#include "libMsbClientC.h"
#include "rest.c"
#include "sput-1.4.0/sput.h"
#include "integration_flow.h"

static FILE* debug_fPtr = NULL;
static const char* debug_fName = "debugOutput";

#define TESTCORRELATIONID "testCorrelationId"
#define TESTDATAOBJECT "teststring"

int writeDebug(const char* format, ... ){

    va_list arg;
    int done;
    va_start (arg, format);
    done = vfprintf(debug_fPtr, format, arg);
    va_end (arg);

    return done;
}

// Replace function from https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

void test_Function(void* client, void* inp, void* context){
    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;

    int_obj = json_object_get_int(json_object_object_get(obj, "dataObject"));
}

static char* test_var_functionCall_string;
static char* test_var_functionCall_corrId;

static bool test_functionCall_callHappened = false;

static int test_var_integration_flow_id = -1;

static char test_var_integration_flow_uuid[36] = {0};

void test_FunctionString(void* client, void* inp, void* context) {
    json_object* obj = json_object_object_get((json_object*) inp,
                                              "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

	writeDebug("TEST: test_FunctionString called: %s\n", json_object_to_json_string(inp));

    test_var_functionCall_string = (char*)json_object_get_string(json_object_object_get(obj, "dataObject"));
    test_var_functionCall_corrId = (char*)json_object_get_string(json_object_object_get((json_object*)inp, "correlationId"));

    test_functionCall_callHappened = true;
}

void test_FunctionEmpty(void* client, void* inp, void* context){
}

void test_FunctionComplex(void* client, void* inp, void* context){
    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;
    double d;

    int_obj = json_object_get_int(json_object_object_get(json_object_object_get(obj, "dataObject"), "int"));
    d = json_object_get_double(json_object_object_get(json_object_object_get(obj, "dataObject"), "double"));
}

void test_FunctionFromString(void* client, void* inp, void* context){
    msbClient* msb_client = (msbClient*)client;

    json_object* obj = json_object_object_get((json_object*)inp, "functionParameters"); //nötig, wenn der gesamte Funktionsaufruf durchgeleitet wird. Ist FwdCompleteFunctionCall nicht aktiv, reicht json_object* obj = (json_object*)inp;

    int32_t int_obj;

    int_obj = json_object_get_int(json_object_object_get(json_object_object_get(obj, "dataObject"), "someInt"));

    int antwort = 123;
    unsigned int versuche = 1000;
    while( msb_client->dataOutInterfaceFlag == 1 && versuche){
        usleep(100);
        --versuche;
    }
    msbClientPublish(msb_client, "Ev1", MEDIUM, &antwort, 0, (char*)json_object_get_string(json_object_object_get(obj, "correlationId"))); //ResponseEvent Ev1 schmeißen, correlationId aus dem Funktionsaufruf durchreichen
}

static char* test_var_UUID = "a209efb9-af3f-4d93-bad4-05215a983164";
static char* test_var_NAME = "Unit_Test_C-Client";
static char* test_var_DESCRIPTION = "beschreibung";
static char* test_var_CLASS = "SmartObject";
static char* test_var_TOKEN = "05215a983164";

msbClient* test_var_msbClient = NULL;

static char* test_var_websocketAdress;
static char* test_var_restAddress_smMgmt;
static char* test_var_restAddress_intMgmt;
static char* test_var_ownerUUID;

static void test_helper_einfl(){
    list_list* ptr;
    ptr = NULL;

    ptr = list_makeListPointer();

    sput_fail_if(ptr == NULL, "Test if list was created");

    if(ptr != NULL){
        char* string1 = "teststring1";
        char* string2 = "teststring2";

        sput_fail_unless(list_add(ptr, string1, 1, (strlen(string1)+1)*sizeof(char)) == EINFL_OK, "Test if adding was successful");
        sput_fail_unless(list_add(ptr, string2, 1, (strlen(string2)+1)*sizeof(char)) == EINFL_OK, "Test if adding was successful");

        void* ptr1;
        void* ptr2;

        sput_fail_unless(list_getValue(ptr, &ptr1, 0) == EINFL_OK, "Test if value was read");
        sput_fail_unless(list_getValue(ptr, &ptr2, 1) == EINFL_OK, "Test if value was read");

        sput_fail_unless(strncmp(ptr1, string1, strlen(string1)) == 0, "Test if string was copied correctly");
        sput_fail_unless(strncmp(ptr2, string2, strlen(string2)) == 0, "Test if string was copied correctly");

        char* string3 = "teststring3";
        char* string4 = "teststring4";

        sput_fail_unless(list_add(ptr, string3, 0, 0) == EINFL_OK, "Test if adding was successful");
        sput_fail_unless(list_add(ptr, string4, 0, 0) == EINFL_OK, "Test if adding was successful");

        sput_fail_unless(list_pop(ptr, 0, 1) == EINFL_OK, "Test if value was deleted");
        sput_fail_unless(list_pop(ptr, 0, 1) == EINFL_OK, "Test if value was deleted");

        sput_fail_unless(list_getValue(ptr, &ptr1, 0) == EINFL_OK, "Test if value was read");
        sput_fail_unless(list_getValue(ptr, &ptr2, 1) == EINFL_OK, "Test if value was read");

        sput_fail_unless(ptr1 == string3, "Test if right pointer is copied");
        sput_fail_unless(ptr2 == string4, "Test if right pointer is copied");

        sput_fail_unless(list_deleteList(ptr, 0)  == EINFL_OK, "Test for deletion of the list");
    }
}

static void test_helper_schll(){
    schll_list* ptr;
    ptr = NULL;

    ptr = schll_MakeListPointer();

    sput_fail_if(ptr == NULL, "Test if list was created");

    if(ptr != NULL){
        char* string1 = "teststring1";
        char* string2 = "teststring2";

        sput_fail_unless(schll_addEntry(ptr, string1, 1, string1, 1, (strlen(string1)+1)*sizeof(char)) == SCHLL_OK, "Test if adding was successful");
        sput_fail_unless(schll_addEntry(ptr, string2, 1, string2, 1, (strlen(string2)+1)*sizeof(char)) == SCHLL_OK, "Test if adding was successful");

        void* ptr1;
        void* ptr2;

        sput_fail_unless(schll_getValue(ptr, &ptr1, string1) == SCHLL_OK, "Test if value was read");
        sput_fail_unless(schll_getValue(ptr, &ptr2, string2) == SCHLL_OK, "Test if value was read");

        sput_fail_unless(strncmp(ptr1, string1, strlen(string1)) == 0, "Test if string was copied correctly");
        sput_fail_unless(strncmp(ptr2, string2, strlen(string2)) == 0, "Test if string was copied correctly");

        char* string3 = "teststring3";
        char* string4 = "teststring4";

        sput_fail_unless(schll_addEntry(ptr, string3, 0, string3, 0, (strlen(string1)+1)*sizeof(char)) == SCHLL_OK, "Test if adding was successful");
        sput_fail_unless(schll_addEntry(ptr, string4, 0, string4, 0, (strlen(string1)+1)*sizeof(char)) == SCHLL_OK, "Test if adding was successful");

        sput_fail_unless(schll_removeEntry(ptr, string1, 1, 1) == SCHLL_OK, "Test if value was deleted");
        sput_fail_unless(schll_removeEntry(ptr, string2, 1, 1) == SCHLL_OK, "Test if value was deleted");

        sput_fail_unless(schll_getValue(ptr, &ptr1, string3) == SCHLL_OK, "Test if value was read");
        sput_fail_unless(schll_getValue(ptr, &ptr2, string4) == SCHLL_OK, "Test if value was read");

        sput_fail_unless(ptr1 == string3, "Test if right pointer is copied");
        sput_fail_unless(ptr2 == string4, "Test if right pointer is copied");

        sput_fail_unless(schll_DeleteList(&ptr, 0, 0)  == EINFL_OK, "Test for deletion of the list");
    }
}

#define initialisation                                      \


static void test_initialisation(){
    //test_var_msbClient = msbClientNewClientURL(test_var_websocketAdress, test_var_UUID, test_var_TOKEN, test_var_CLASS, test_var_NAME, test_var_DESCRIPTION, 0, NULL, NULL, NULL);
    test_var_msbClient = msbClientNewClientURL("ws://127.0.0.1:8085", NULL, test_var_UUID, test_var_TOKEN, test_var_CLASS, test_var_NAME, test_var_DESCRIPTION, 0, NULL, NULL, NULL);

    msbClientSetDebug(test_var_msbClient, true);
    msbClientSetDebugFunction(test_var_msbClient, &writeDebug);
    msbClientGenerateSockJSPath(test_var_msbClient);
    msbClientSetFunctionCacheSize(test_var_msbClient, 100);
    msbClientSetEventCacheSize(test_var_msbClient, 100);
}

static void test_string_functions(){

    char* string1 = "[\\""nachricht\\""]";
    char* string2 = "[\"nachricht\"]";
    char* res = NULL;

    string_replace(string1, "\\""", "\"", &res);

    sput_fail_unless(strcmp(res, string2) == 0 && strlen(res) == strlen(string2), "Test if string part were replaced correctly");

    if(res != NULL) free(res);

    char* string3 = "R [\"nachricht\"]";

    res = string_combine("R ", string2);

    sput_fail_unless(strcmp(res, string3) == 0 && strlen(res) == strlen(string3), "Test if strings were combined correctly");

    if(res != NULL) free(res);

    res = string_duplicate(string1);

    sput_fail_unless(strcmp(res, string1) == 0 && strlen(res) == strlen(string1), "Test if string was duplicated correctly");

    if(res != NULL) free(res);

}

static void test_adding_config_parameters(){

    msbObjectConfigParam *cpArr;
    size_t used;

    int32_t* wert = (int32_t*)malloc(sizeof(int32_t));
    *wert = 123456;
    msbClientAddConfigParam(test_var_msbClient, "p1", MSB_INTEGER, MSB_INT32, wert);

    char* string = (char*)malloc(sizeof(char)*6);
    strcpy(string, "hallo\0");
    msbClientAddConfigParam(test_var_msbClient, "p2", MSB_STRING, MSB_NONE, string);

    int64_t* wert2 = (int64_t*)malloc(sizeof(int64_t));
    *wert2 = 3000000000;
    msbClientAddConfigParam(test_var_msbClient, "p3", MSB_INTEGER, MSB_INT64, wert2);

    float* fl = (float*)malloc(sizeof(float));
    *fl = 123.456f;
    msbClientAddConfigParam(test_var_msbClient, "p4", MSB_NUMBER, MSB_FLOAT, fl);

    double* d = (double*)malloc(sizeof(double));
    *d = 321.123456789;
    msbClientAddConfigParam(test_var_msbClient, "p5", MSB_NUMBER, MSB_DOUBLE, d);

    int* bl = (int*)malloc(sizeof(int));
    *bl = 1;
    msbClientAddConfigParam(test_var_msbClient, "p6", MSB_BOOL, MSB_NONE, bl);

    sput_fail_if(test_var_msbClient->msbObjectData->configParamArray->number_entries != 6, "Test if six parameters exist");

    schll_entry* entry;
    entry = test_var_msbClient->msbObjectData->configParamArray->first_entry;

    msbObjectConfigParam* c;
    c = (msbObjectConfigParam*)entry->value;

    sput_fail_if(strcmp("p1", c->name) != 0, "Parameter 0 == p1");
    sput_fail_if(*c->int32_value != 123456, "Parameter 0 value == 123456");

    entry = entry->next_entry;
    c = (msbObjectConfigParam*)entry->value;

    sput_fail_if(strcmp("p2", c->name) != 0, "Parameter 1 == p1");
    sput_fail_if(strcmp(c->string_value, "hallo") != 0, "Parameter 1 value test");

    entry = entry->next_entry;
    c = (msbObjectConfigParam*)entry->value;

    sput_fail_if(strcmp("p3", c->name) != 0, "Parameter 2 == p2");
    sput_fail_if(*c->int64_value != 3000000000, "Parameter 2 value == 3000000000");

    entry = entry->next_entry;
    c = (msbObjectConfigParam*)entry->value;

    sput_fail_if(strcmp("p4", c->name) != 0, "Parameter 3 == p3");
    sput_fail_if(*c->float_value != 123.456f, "Parameter 3 value == 123.456f");

    entry = entry->next_entry;
    c = (msbObjectConfigParam*)entry->value;

    sput_fail_if(strcmp("p5", c->name) != 0, "Parameter 4 == p4");
    sput_fail_if(*c->double_value != 321.123456789, "Parameter 4 value == 321.123456789");

    entry = entry->next_entry;
    c = (msbObjectConfigParam*)entry->value;

    sput_fail_if(strcmp("p6", c->name) != 0, "Parameter 5 == p5");
    sput_fail_if(*c->bool_value != 1, "Parameter 5 value == 1");

}

static void test_adding_events(){
    msbClientAddEmptyEvent(test_var_msbClient, "Ev0", "Event0", "beschreibung");

    msbClientAddEvent(test_var_msbClient, "Ev1", "Event1", "beschreibung", MSB_INTEGER, MSB_INT32, false);
    msbClientAddEvent(test_var_msbClient, "Ev2", "Event2", "beschreibung", MSB_STRING, MSB_NONE, false);

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

    msbClientAddComplexEvent(test_var_msbClient, "Ev3", "Event3", "beschreibung", dataformat, false);

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

    msbClientAddEventFromString(test_var_msbClient, "Ev4", "Event4", "beschreibung", str, 0);

    sput_fail_if(test_var_msbClient->msbObjectData->eventArray->number_entries != 5, "Test if five events exist");

    schll_entry* entry;
    entry = test_var_msbClient->msbObjectData->eventArray->first_entry;

    msbObjectEvent* e;
    e = (msbObjectEvent*)entry->value;

    sput_fail_if(strcmp("Ev0", e->eventId) != 0, "Event 0 id == Ev0");
    sput_fail_unless(json_object_object_length(e->dataFormat) == 0, "Event 0 dataFormat == empty");

    entry = entry->next_entry;
    e = (msbObjectEvent*)entry->value;

    sput_fail_if(strcmp("Ev1", e->eventId) != 0, "Event 1 id == Ev1");
    sput_fail_unless(e->type == MSB_INTEGER, "Event 1 type == integer");

    entry = entry->next_entry;
    e = (msbObjectEvent*)entry->value;

    sput_fail_if(strcmp("Ev2", e->eventId) != 0, "Event 2 id == Ev2");
    sput_fail_unless(e->type == MSB_STRING, "Event 2 type == string");

    entry = entry->next_entry;
    e = (msbObjectEvent*)entry->value;

    sput_fail_if(strcmp("Ev3", e->eventId) != 0, "Event 3 id == Ev3");
    sput_fail_if(json_object_equal(e->dataFormat, dataformat) != 1, "Event 3 dataFormat == dataformat");

    entry = entry->next_entry;
    e = (msbObjectEvent*)entry->value;

    json_object* test_var_tmp_object = json_tokener_parse(str);

    sput_fail_if(strcmp("Ev4", e->eventId) != 0, "Event 4 id == Ev4");
    sput_fail_if(json_object_equal(e->dataFormat, test_var_tmp_object) != 1, "Event 4 dataFormat == dataformat");

}

static void test_adding_functions(){

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


    msbClientAddEmptyFunction(test_var_msbClient, "F0", "Function0", "beschreibung", &test_FunctionEmpty, NULL);
    msbClientAddFunction(test_var_msbClient, "F1", "Function1", "beschreibung", &test_Function, MSB_INTEGER, MSB_INT32, 0, NULL);
    msbClientAddFunction(test_var_msbClient, "F2", "Function2", "beschreibung", &test_FunctionString, MSB_STRING, MSB_NONE, 0, NULL);
    msbClientAddComplexFunction(test_var_msbClient, "F3", "Function3", "beschreibung", &test_FunctionComplex, dataformat, NULL);
    msbClientAddFunctionFromString(test_var_msbClient, "F4", "Function4", "beschreibung", &test_FunctionFromString, str, test_var_msbClient); //der Client als Kontext, damit wir die nötigen Informationen für das ResponseEvent haben
    msbClientAddResponseEventToFunction(test_var_msbClient, "F4", "Ev1"); //ResponseEvent Ev1 zur Funktion F3 hinzufügen

    msbClientFwdCompleteFunctionCall(test_var_msbClient);

    sput_fail_if(test_var_msbClient->msbObjectData->functionArray->number_entries != 5, "Test if five events exist");

    schll_entry* entry;
    entry = test_var_msbClient->msbObjectData->functionArray->first_entry;

    msbObjectFunction* f;
    f = (msbObjectFunction*)entry->value;

    sput_fail_if(strcmp("F0", f->functionId) != 0, "Function 0 id == F0");
    sput_fail_if(f->dataFormat == NULL, "Function 0 dataFormat == empty");

    entry = entry->next_entry;
    f = (msbObjectFunction*)entry->value;

    sput_fail_if(strcmp("F1", f->functionId) != 0, "Function 1 id == F1");
    //sput_fail_if(test_var_msbClient->msbObjectData->functionArray.fArr[1]. == MSB_INTEGER, "Function 1 type == integer");

    entry = entry->next_entry;
    f = (msbObjectFunction*)entry->value;

    sput_fail_if(strcmp("F2", f->functionId) != 0, "Function 2 id == F2");
    //sput_fail_if(test_var_msbClient->msbObjectData->functionArray.fArr[1]. == MSB_INTEGER, "Function 1 type == integer");

    entry = entry->next_entry;
    f = (msbObjectFunction*)entry->value;

    sput_fail_if(strcmp("F3", f->functionId) != 0, "Function 3 id == F3");
    sput_fail_if(json_object_equal(f->dataFormat, dataformat) != 1, "Function 3 dataFormat == dataformat");

    entry = entry->next_entry;
    f = (msbObjectFunction*)entry->value;

    json_object* test_var_tmp_object = json_tokener_parse(str);

    sput_fail_if(strcmp("F4", f->functionId) != 0, "Function 4 id == F4");
    sput_fail_if(json_object_equal(f->dataFormat, test_var_tmp_object) != 1, "Function 4 dataFormat == dataformat");

}

static void test_start_client(){

    int r = msbClientRunClientStateMachine(test_var_msbClient);

    sput_fail_unless(!r, "Starting client");

}

static void test_client_registration(){

    int i = 0;

    for(; i < 5; ++i) {
        sleep(3);

        switch (test_var_msbClient->currentStatusAutomat) {
            case CREATED: {
                break;
            }
            case INITIALISED: {
                break;
            }
            case CONNECTING: {
                break;
            }
            case CONNECTED: {
                msbClientRegister(test_var_msbClient);
                break;
            }
            case REGISTERED: {
                i = 5;
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
    }

    sput_fail_unless(test_var_msbClient->currentStatusAutomat == REGISTERED, "Test if client has changed status to REGISTERED");

}

static void test_event_queue(){

    msbClientEnableEventCache(test_var_msbClient);

    msbClientRunClientStateMachine(test_var_msbClient); //automat in thread

    sleep(3);

    int i, j;
    for(j = 0; j < 3; ++j){
        msbClientPublishEmpty(test_var_msbClient, "Ev0", MEDIUM, NULL);

        for(i = 0; i < 10 && test_var_msbClient->dataOutInterfaceFlag; ++i){
            usleep(100000);
        }
    }

    sput_fail_unless(test_var_msbClient->outgoing_data->number_entries == 3, "Test if event queue has three events");

}

static void test_verify_client(){

    char* post;

    char url[512] = {0};
    strcpy(url, test_var_restAddress_smMgmt);
    strcat(url, "/smartobject/verify/");
    strcat(url, test_var_ownerUUID);

    int r = -1, i = 0;

    for(; i < 3 && r != 201; ++i){
        r = rest_post(url, test_var_TOKEN, &post, "accept: */*", "Content-Type: text/plain", "TEST: verify_client");
        if(r != 201)
            sleep(5);
    }

    sput_fail_unless(r == 201, "Client is verified");

}

static void test_create_integration_flow(){

    int r = -1;

    FILE *fptr;

    /*if ((fptr = fopen("integration_flow.json","r")) == NULL){
        printf("Error! opening file");
    }else{
        char tmp[10240] = {0};
        fscanf(fptr,"%s", tmp);

        fseek(fptr, 0, SEEK_END);
        long fsize = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);  //same as rewind(f);

        fread(tmp, fsize, 1, fptr);
        fclose(fptr);*/

        char* post;

        char url[512] = {0};
        strcpy(url, test_var_restAddress_intMgmt);
        strcat(url, "/integrationFlow/create?deploy=true");

        char* prep_integration_flow_owner = NULL;
        char* prep_integration_flow_uuid1 = NULL;
        char* prep_integration_flow_uuid2 = NULL;

        prep_integration_flow_owner = str_replace((char*)integration_flow, "%%%OwnerUUID%%%", test_var_ownerUUID);
        if (prep_integration_flow_owner == NULL)
        {
            prep_integration_flow_owner = (char*)integration_flow;
        }
        
        prep_integration_flow_uuid1 = str_replace((char*)prep_integration_flow_owner, "%%%Service1UUID%%%", test_var_UUID);
        if (prep_integration_flow_uuid1 == NULL)
        {
            prep_integration_flow_uuid1 = (char*)prep_integration_flow_owner;
        }

        prep_integration_flow_uuid2 = str_replace((char*)prep_integration_flow_uuid1, "%%%Service2UUID%%%", test_var_UUID);
        if (prep_integration_flow_uuid2 == NULL)
        {
            prep_integration_flow_uuid2 = (char*)prep_integration_flow_uuid1;
        }

        int i = 0;
        for(; i < 3 && r != 201; ++i){
            //r = rest_post(url, tmp, &post, "accept: application/json", "Content-Type: application/json", "TEST: create_integration_flow");            
            //r = rest_post(url, (char*)integration_flow, &post, "accept: application/json", "Content-Type: application/json", "TEST: create_integration_flow");
            r = rest_post(url, (char*)prep_integration_flow_uuid2, &post, "accept: application/json", "Content-Type: application/json", "TEST: create_integration_flow");

            if(r != 201)
                sleep(5);
        }

        if ((char*)prep_integration_flow_uuid2 != NULL && (char*)prep_integration_flow_uuid2 != (char*)prep_integration_flow_uuid1)
        {
            free(prep_integration_flow_uuid2);
        }
        if ((char*)prep_integration_flow_uuid1 != NULL && (char*)prep_integration_flow_uuid2 != (char*)prep_integration_flow_owner)
        {
            free(prep_integration_flow_uuid1);
        }
        if ((char*)prep_integration_flow_owner != NULL && (char*)prep_integration_flow_owner != (char*)integration_flow)
        {
            free(prep_integration_flow_owner);
        }

        if(r == 201){
            json_object* j_resp = json_tokener_parse(post);

            if(j_resp != NULL){
                //json_object* j_resp_i = json_object_object_get(j_resp, "id");
                //if(j_resp_i != NULL)
                //    test_var_integration_flow_id = json_object_get_int(j_resp_i);
                json_object* j_resp_i = json_object_object_get(j_resp, "uuid");
                if(j_resp_i != NULL)
                    strncpy(test_var_integration_flow_uuid, json_object_get_string(j_resp_i), sizeof(test_var_integration_flow_uuid));
                else
                {
                    j_resp_i = json_object_object_get(j_resp, "id");
                    if(j_resp_i != NULL)
                        test_var_integration_flow_id = json_object_get_int(j_resp_i);
                }
            }
        }
    //}

    sput_fail_unless(r == 201, "Integration flow created");
    //sput_fail_unless(test_var_integration_flow_id != -1, "Integration flow read");
    //sput_fail_unless(!uuid_is_valid(test_var_integration_flow_uuid), "Integration flow could be read and has a valid UUID");
}

static void test_activate_integration_flow(){

	int r = -1;

    char url[512] = {0};
    strcpy(url, test_var_restAddress_intMgmt);
    strcat(url, "/integrationFlow/");

    //char str[12] = {0};
    //sprintf(str, "%d", test_var_integration_flow_id);
    //strcat(url, str);
    strncat(url, test_var_integration_flow_uuid, sizeof(test_var_integration_flow_uuid));

    strcat(url, "/deploy");

    char* put;

	int i = 0;
	for(; i < 3 && r != 200; ++i){
		r = rest_put(url, &put, "accept: */*", "TEST: activate_integration_flow");
        if(r != 200)
            sleep(5);
	}

    sput_fail_unless(r == 200, "Integration flow deployed");

}

static void test_publish_event(){

    sleep(10);

    msbClientPublish(test_var_msbClient, "Ev2", MEDIUM, TESTDATAOBJECT, 0, TESTCORRELATIONID);

    unsigned int i = 0;
    for(; i < 20 && !test_functionCall_callHappened; ++i) sleep(1);

    sput_fail_unless(test_functionCall_callHappened, "Test if function was called");

    if(test_functionCall_callHappened){
        sput_fail_if(test_var_functionCall_string == NULL, "Test if string was set");
        sput_fail_if(test_var_functionCall_corrId == NULL, "Test if correlation id was set");

        if(test_var_functionCall_string != NULL){
            sput_fail_unless(strncmp(test_var_functionCall_string, TESTDATAOBJECT, strlen(TESTDATAOBJECT)) == 0 && strlen(TESTDATAOBJECT) == strlen(test_var_functionCall_string), "Test if string was set correctly");
        }

        if(test_var_functionCall_corrId != NULL){
            sput_fail_unless(strncmp(test_var_functionCall_corrId, TESTCORRELATIONID, strlen(TESTCORRELATIONID)) == 0 && strlen(TESTCORRELATIONID) == strlen(test_var_functionCall_corrId), "Test if correlation id was set correctly");
        }
    }
}

static void test_stop_client(){

    int r = msbClientHaltClientStateMachine(test_var_msbClient);

    sput_fail_unless(!r, "End client");

}

static void test_delete_integration_flow(){

    int r = -1;

    char url[512] = {0};
    strcpy(url, test_var_restAddress_intMgmt);
    strcat(url, "/integrationFlow/");
    if (test_var_integration_flow_uuid[0] != 0)
    {
        strncat(url, test_var_integration_flow_uuid, sizeof(test_var_integration_flow_uuid));
    } else {
        char str[256] = {0};
        sprintf(str, "%d", test_var_integration_flow_id);
        strcat(url, str);
    }
    
    int i = 0;
    for(; i < 3 && r != 200; ++i){
        r = rest_delete(url, "TEST: delete_integration_flow");
        if(r != 200)
            sleep(5);
    }
    sput_fail_unless(r == 200, "Integration flow deleted");

}

static void test_delete_client(){

    int r = -1;

    char url[512] = {0};
    strcpy(url, test_var_restAddress_smMgmt);
    strcat(url, "/smartobject/");
    strcat(url, test_var_UUID);

    int i = 0;
    for(; i < 3 && r != 200; ++i){
        r = rest_delete(url, "TEST: delete_client");
        if(r != 200)
            sleep(5);
    }

    sput_fail_unless(r == 200, "Client deleted");

}

int test(bool unit_tests, bool integration_tests, char* websocketAdress, char* restAddress_smMgmt, char* restAddress_intMgmt, char* ownerUUID, char* serviceUUID){

    if(!unit_tests && !integration_tests){
        printf("No tests to do. Aborting.\n");
        return -1;
    }

    test_var_websocketAdress = websocketAdress;
    test_var_restAddress_smMgmt = restAddress_smMgmt;
    test_var_restAddress_intMgmt = restAddress_intMgmt;
    test_var_ownerUUID = ownerUUID;
    test_var_UUID = serviceUUID;

    debug_fPtr = fopen(debug_fName, "w");

    if(!debug_fPtr){
        printf("Couldn't create debug file. Aborting.\n");
        return 1;
    }

    debugOutputFunction = &writeDebug; //REST debug output umlenken

    sput_start_testing();
    if(unit_tests){
        sput_enter_suite("MSB.Client.Websocket.C.Test.Unit");
        sput_run_test(test_helper_einfl);
        sput_run_test(test_helper_schll);
        sput_run_test(test_string_functions);
        sput_run_test(test_initialisation);

        sput_run_test(test_adding_config_parameters);
        sput_run_test(test_adding_events);
        sput_run_test(test_adding_functions);
        sput_run_test(test_start_client);

        sleep(3);

        sput_run_test(test_event_queue);
        sput_run_test(test_stop_client);
    }

    if(integration_tests){
        sput_enter_suite("MSB.Client.Websocket.C.Test.Integration");

        if(!unit_tests){
            sput_run_test(test_initialisation);
            sput_run_test(test_adding_config_parameters);
            sput_run_test(test_adding_events);
            sput_run_test(test_adding_functions);
        }

	    bool tls = false;
	    if(!strncmp(test_var_websocketAdress, "wss", 3) || !strncmp(test_var_websocketAdress, "https", 5)) tls = true;

        /* msbClientChangeURL(test_var_msbClient, test_var_websocketAdress, test_var_websocketAdress, true, NULL, NULL, NULL); mit Origin */
        msbClientChangeURL(test_var_msbClient, test_var_websocketAdress, NULL, tls, NULL, NULL, NULL);
        test_var_msbClient->nopoll_debug = true;

        msbClientGenerateSockJSPath(test_var_msbClient);

        sput_run_test(test_start_client);

        sleep(3);

        sput_run_test(test_verify_client);
        sput_run_test(test_create_integration_flow);
        //sput_run_test(test_activate_integration_flow);
        sput_run_test(test_client_registration);
        sput_run_test(test_publish_event);
        sput_run_test(test_stop_client);
        sput_run_test(test_delete_integration_flow);
        sput_run_test(test_delete_client);
    }

    sput_finish_testing();

    sput_create_report_xml("report.xml");

    fclose(debug_fPtr);

    printf("\n");

    debug_fPtr = fopen(debug_fName, "r");

    char c;
    if (debug_fPtr) {
        while ((c = getc(debug_fPtr)) != EOF)
            putchar(c);
        fclose(debug_fPtr);
    }

    return sput_get_return_value();

}