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
* @file MsbObject.c
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief Msb C client object functions
*/

#include <json-c/json.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "rest/helpers.h"

#include "MsbClientC.h"

/**
 * @brief Internal function for getting primitive type as string, all lower case
 *
 * @param pt primitive type
 *
 * @return primitive type as string
 */
const char* getDataObjectType(PrimitiveType pt);

/**
 * @brief Internal function for getting primitive format as string, all lower case
 *
 * @param pf primitive format
 *
 * @return primitive format as string
 */
const char* getDataObjectFormat(PrimitiveFormat pf);

/**
 * @brief Internal function for getting primitive type as string, all upper case
 *
 * @param pt primitive type
 *
 * @return primitive type as string
 */
const char* getDataObjectTypeUC(PrimitiveType pt);

/**
 * @brief Internal function for getting primitive type as string, all upper case
 *
 * @param pf primitive format
 *
 * @return primitive format as string
 */
const char* getDataObjectFormatUC(PrimitiveFormat pf);

void changeConfigParamValueInternal(msbObjectConfigParam* c, void* value){

    switch (c->type) {
        case MSB_ARRAY:
            break;
        case MSB_STRING:
            c->string_value = (char*) value;
            break;
        case MSB_INTEGER:
            if (c->format == MSB_INT32)
                c->int32_value = (int32_t*) value;
            else if (c->format == MSB_INT64)
                c->int64_value = (int64_t*) value;
            break;
        case MSB_BOOL:
            c->bool_value = (int*) value;
            break;
        case MSB_NUMBER:
            if (c->format == MSB_FLOAT)
                c->float_value = (float*) value;
            else if (c->format == MSB_DOUBLE)
                c->double_value = (double*) value;
            break;
        default:
            break;
    }
}

void addConfigParam(msbObject* object, char* cpName, PrimitiveType cpType, PrimitiveFormat cpFormat, void* value) {
    msbObjectConfigParam c;

    c.name = (char*) calloc(1, strlen(cpName) + 1);
    strcpy(c.name, cpName);

    if (c.name == NULL) return;

    c.format = cpFormat;
    c.type = cpType;

    changeConfigParamValueInternal(&c, value);

    schll_addEntry(object->configParamArray, cpName, 0, &c, 1, sizeof(msbObjectConfigParam));
}

void removeConfigParam(msbObject* object, char* cpName) {

    if (object == NULL || cpName == NULL) return;

    if (object->configParamArray == NULL) return;

    schll_removeEntry(object->configParamArray, cpName, 1, 0);
}

void changeConfigParamValue(msbObject* object, char* cpName, void* value) {

    msbObjectConfigParam* c;

    if (schll_getValue(object->configParamArray, (void**) &c, cpName) != SCHLL_OK) return;

    changeConfigParamValueInternal(c, value);
}

void
addFunction(msbObject* object, char* fId, char* fName, char* fDesc, void (*fPtr)(void*, void*, void*),
            PrimitiveType fType, PrimitiveFormat fFormat, bool isArray, void* contextPtr) {

    msbObjectFunction f;

    f.functionId = (char*) calloc(1, strlen(fId) + 1);
    f.name = (char*) calloc(1, strlen(fName) + 1);
    f.description = (char*) calloc(1, strlen(fDesc) + 1);

    if (f.functionId == NULL || f.name == NULL || f.description == NULL) return;

    strcpy(f.functionId, fId);
    strcpy(f.name, fName);
    strcpy(f.description, fDesc);

    f.callback = fPtr;
    f.contextPtr = contextPtr;

    json_object* dFormatRoot;
    dFormatRoot = json_object_new_object();

    if (isArray) {
        json_object* iObjectRoot;
        iObjectRoot = json_object_new_object();

        json_object_object_add(dFormatRoot, "type", json_object_new_string("array"));
        json_object_object_add(dFormatRoot, "items", iObjectRoot);

        json_object_object_add(iObjectRoot, "type", json_object_new_string(getDataObjectType(fType)));
        if (fFormat != MSB_NONE) {
            json_object_object_add(iObjectRoot, "format", json_object_new_string(getDataObjectFormat(fFormat)));
        }
    } else {
        json_object* dObjectRoot;
        dObjectRoot = json_object_new_object();
        json_object_object_add(dFormatRoot, "dataObject", dObjectRoot);

        json_object_object_add(dObjectRoot, "type", json_object_new_string(getDataObjectType(fType)));
        if (fFormat != MSB_NONE) {
            json_object_object_add(dObjectRoot, "format", json_object_new_string(getDataObjectFormat(fFormat)));
        }
    }
    f.dataFormat = dFormatRoot;
    f.atId = object->function_atId_counter++;
    f.responseEventAtIds = NULL;

    schll_addEntry(object->functionArray, f.functionId, 0, &f, 1, sizeof(msbObjectFunction));
}

void addComplexFunction(msbObject* object, char* fId, char* fName, char* fDesc, void (*fPtr)(void*, void*, void*),
                        json_object* fFormat, void* contextPtr) {

    msbObjectFunction f;

    f.functionId = (char*) calloc(1, strlen(fId) + 1);
    f.name = (char*) calloc(1, strlen(fName) + 1);
    f.description = (char*) calloc(1, strlen(fDesc) + 1);

    if (f.functionId == NULL || f.name == NULL || f.description == NULL) return;

    strcpy(f.functionId, fId);
    strcpy(f.name, fName);
    strcpy(f.description, fDesc);

    f.dataFormat = json_object_get(fFormat);
    f.atId = object->function_atId_counter++;

    f.callback = fPtr;
    f.contextPtr = contextPtr;

    f.responseEventAtIds = NULL;

    schll_addEntry(object->functionArray, f.functionId, 0, &f, 1, sizeof(msbObjectFunction));
}

void addEmptyFunction(msbObject* object, char* fId, char* fName, char* fDesc, void (* fPtr)(void*, void*, void*),
                      void* contextPtr) {

    json_object* dFormatRoot;
    dFormatRoot = json_object_new_object();

    addComplexFunction(object, fId, fName, fDesc, fPtr, dFormatRoot, contextPtr);
}

void removeFunction(msbObject* object, char* fId) {

    if (object == NULL || fId == NULL) return;

    schll_removeEntry(object->functionArray, fId, 0, 1);
}

void addResponseEventToFunction(msbObject* object, char* fId, char* eId) {

    msbObjectFunction* f;

    if (schll_getValue(object->functionArray, (void**) &f, fId) != SCHLL_OK) return;

    msbObjectEvent* e;

    if (schll_getValue(object->eventArray, (void**) &e, eId) != SCHLL_OK) return;

    if (f->responseEventAtIds == NULL) f->responseEventAtIds = list_makeListPointer();

    list_add(f->responseEventAtIds, &(e->atId), 0, 0);
}

void removeResponseEventFromFunction(msbObject* object, char* fId, char* eId) {

    if (object == NULL || fId == NULL || eId == NULL) return;

    msbObjectFunction* f;

    if (schll_getValue(object->functionArray, (void**) &f, fId) != SCHLL_OK) return;

    if (f->responseEventAtIds == NULL) return;

    msbObjectEvent* e;

    if (schll_getValue(object->eventArray, (void**) &e, eId) != SCHLL_OK) return;

    unsigned int i;
    for (i = 0; i < f->responseEventAtIds->number_entries; ++i) {
        int* atId = &e->atId;

        if (list_getValuePointer(f->responseEventAtIds, i) == atId) {
            list_pop(f->responseEventAtIds, i, 0);
            break;
        }
    }
}

void addEvent(msbObject* object, char* eId, char* eName, char* eDesc, PrimitiveType eType,
              PrimitiveFormat eFormat, bool isArray) {
    msbObjectEvent e;

    e.eventId = (char*) calloc(1, strlen(eId) + 1);
    e.name = (char*) calloc(1, strlen(eName) + 1);
    e.description = (char*) calloc(1, strlen(eDesc) + 1);

    if (e.eventId == NULL || e.name == NULL || e.description == NULL) return;

    strcpy(e.eventId, eId);
    strcpy(e.name, eName);
    strcpy(e.description, eDesc);

    json_object* dFormatRoot;
    dFormatRoot = json_object_new_object();
    json_object* dObjectRoot;
    dObjectRoot = json_object_new_object();
    json_object_object_add(dFormatRoot, "dataObject", dObjectRoot);

    if (isArray) {
        json_object_object_add(dObjectRoot, "type", json_object_new_string("array"));

        json_object* itemsRoot;
        itemsRoot = json_object_new_object();
        json_object_object_add(dObjectRoot, "items", itemsRoot);

        json_object_object_add(itemsRoot, "type", json_object_new_string(getDataObjectType(eType)));
        if (eFormat != MSB_NONE) {
            json_object_object_add(itemsRoot, "format", json_object_new_string(getDataObjectFormat(eFormat)));
        }
        e.dataFormat = dFormatRoot;

        e.type = MSB_ARRAY;
        e.format = MSB_NONE;

        e.array_subtype = eType;
        e.array_subformat = eFormat;
    } else {
        e.type = eType;
        e.format = eFormat;

        e.array_subtype = eType;
        e.array_subformat = eFormat;

        json_object_object_add(dObjectRoot, "type", json_object_new_string(getDataObjectType(eType)));
        if (eFormat != MSB_NONE) {
            json_object_object_add(dObjectRoot, "format", json_object_new_string(getDataObjectFormat(eFormat)));
        }
        e.dataFormat = dFormatRoot;
    }

    e.atId = object->event_atId_counter++;

    schll_addEntry(object->eventArray, e.eventId, 0, &e, 1, sizeof(msbObjectEvent));
}

void addComplexEvent(msbObject* object, char* eId, char* eName, char* eDesc, json_object* eFormat, bool isArray) {
    msbObjectEvent e;

    e.eventId = (char*) calloc(1, strlen(eId) + 1);
    e.name = (char*) calloc(1, strlen(eName) + 1);
    e.description = (char*) calloc(1, strlen(eDesc) + 1);

    if (e.eventId == NULL || e.name == NULL || e.description == NULL) return;

    strcpy(e.eventId, eId);
    strcpy(e.name, eName);
    strcpy(e.description, eDesc);

    e.dataFormat = json_object_get(eFormat);

    e.type = MSB_STRING;
    e.format = MSB_NONE;
    e.array_subtype = MSB_STRING;
    e.array_subformat = MSB_NONE;
    e.atId = object->event_atId_counter++;

    schll_addEntry(object->eventArray, e.eventId, 0, &e, 1, sizeof(msbObjectEvent));
}

void addEmptyEvent(msbObject* object, char* eId, char* eName, char* eDesc) {
    json_object* dFormatRoot;
    dFormatRoot = json_object_new_object();

    addComplexEvent(object, eId, eName, eDesc, dFormatRoot, false);
}

void removeEvent(msbObject* object, char* eId) {

    if (object == NULL || eId == NULL) return;

    if (object->eventArray == NULL) return;

    msbObjectEvent* e;

    if (schll_getValue(object->eventArray, (void**) &e, eId) != SCHLL_OK) return;

    if (object->functionArray != NULL) {
        int* atId = &e->atId;

        unsigned int i, j;

        schll_entry* entry;
        entry = object->functionArray->first_entry;

        for (i = 0; i < object->functionArray->number_entries; ++i) {
            msbObjectFunction* f = (msbObjectFunction*) entry->value;
            list_entry* entry_l;

            if (f->responseEventAtIds == NULL) {
                continue;
            }

            entry_l = f->responseEventAtIds->first_entry;

            for (j = 0; j < f->responseEventAtIds->number_entries; ++j) {
                int* atId_f = entry_l->value;

                if (atId_f == atId) {
                    list_pop(f->responseEventAtIds, j, 0);
                    break;
                }

                entry_l = entry_l->next_entry;
            }

            entry = entry->next_entry;
        }
    }

    schll_removeEntry(object->eventArray, eId, 0, 1);
}

const char* msbObjectSelfDescription(const msbObject* object) {

    json_object* root;
    root = json_object_new_object();
    json_object_object_add(root, "uuid", json_object_new_string(object->objectInfo.service_uuid));
    json_object_object_add(root, "token", json_object_new_string(object->objectInfo.service_token));
    json_object_object_add(root, "name", json_object_new_string(object->objectInfo.service_name));
    json_object_object_add(root, "@class", json_object_new_string(object->objectInfo.service_class));
    json_object_object_add(root, "description", json_object_new_string(object->objectInfo.service_description));

    json_object* fcarray;
    fcarray = json_object_new_array();
    json_object_object_add(root, "functions", fcarray);

    json_object* ecarray;
    ecarray = json_object_new_array();
    json_object_object_add(root, "events", ecarray);

    json_object* configuration;
    configuration = json_object_new_object();
    json_object_object_add(root, "configuration", configuration);
    json_object* proot;
    proot = json_object_new_object();
    json_object_object_add(configuration, "parameters", proot);

    schll_entry* entry = object->functionArray->first_entry;

    while (entry != NULL) {
        msbObjectFunction* f = (msbObjectFunction*) entry->value;

        json_object* froot;
        froot = json_object_new_object();
        json_object_array_add(fcarray, froot);

        json_object_object_add(froot, "@id", json_object_new_int(f->atId));
        json_object_object_add(froot, "functionId", json_object_new_string(f->functionId));
        json_object_object_add(froot, "name", json_object_new_string(f->name));
        if (f->description != NULL) {
            json_object_object_add(froot, "description", json_object_new_string(f->description));
        }
        json_object_object_add(froot, "dataFormat", f->dataFormat);

        json_object* respArray = json_object_new_array();

        if (f->responseEventAtIds != NULL) {
            list_entry* entry_l = f->responseEventAtIds->first_entry;
            json_object_array_add(respArray, json_object_new_int(*(int*) entry_l->value));

            int i;

            for (i = 1; i < f->responseEventAtIds->number_entries; ++i) {
                entry_l = entry_l->next_entry;
                json_object_array_add(respArray, json_object_new_int(*(int*) entry_l->value));
            }
        }

        json_object_object_add(froot, "responseEvents", respArray);

        entry = entry->next_entry;
    }

    entry = object->eventArray->first_entry;

    while (entry != NULL) {
        msbObjectEvent* e = (msbObjectEvent*) entry->value;

        json_object* eroot;
        eroot = json_object_new_object();
        json_object_array_add(ecarray, eroot);
        json_object_object_add(eroot, "@id", json_object_new_int(e->atId));
        json_object_object_add(eroot, "eventId", json_object_new_string(e->eventId));
        json_object_object_add(eroot, "name", json_object_new_string(e->name));
        if (e->description != NULL) {
            json_object_object_add(eroot, "description", json_object_new_string(e->description));
        }
        json_object_object_add(eroot, "dataFormat", e->dataFormat);

        entry = entry->next_entry;
    }

    entry = object->configParamArray->first_entry;

    while (entry != NULL) {
        msbObjectConfigParam* c = (msbObjectConfigParam*) entry->value;

        json_object* param;
        param = json_object_new_object();

        json_object_object_add(param, "type", json_object_new_string(getDataObjectTypeUC(c->type)));

        if (strcmp(getDataObjectTypeUC(c->type), "BOOLEAN") == 0) {
            json_bool b = *c->bool_value;
            json_object_object_add(param, "value", json_object_new_boolean(b));
        } else if (strcmp(getDataObjectTypeUC(c->type), "STRING") == 0) {
            json_object_object_add(param, "value", json_object_new_string(c->string_value));
        } else if (strcmp(getDataObjectTypeUC(c->type), "INTEGER") == 0) {
            if (strcmp(getDataObjectFormatUC(c->format), "INT32") == 0) {
                int32_t i = *c->int32_value;
                json_object_object_add(param, "value", json_object_new_int(i));
            } else {
                int64_t i = *c->int64_value;
                json_object_object_add(param, "value", json_object_new_int64(i));
            }
        } else if (strcmp(getDataObjectTypeUC(c->type), "NUMBER") == 0) {
            if (strcmp(getDataObjectFormatUC(c->format), "FLOAT") == 0) {
                //float f = *c->float_value;
                double d = (double) *c->float_value;
                json_object_object_add(param, "value", json_object_new_double(d));
            } else {
                double d = *c->double_value;
                json_object_object_add(param, "value", json_object_new_double(d));
            }
        }

        if (strcmp(getDataObjectFormatUC(c->format), "") != 0) {
            json_object_object_add(param, "format", json_object_new_string(getDataObjectFormatUC(c->format)));
        }
        json_object_object_add(proot, c->name, param);

        entry = entry->next_entry;
    }

    const char* selfD = json_object_to_json_string_ext(root, JSON_C_TO_STRING_NOSLASHESCAPE);

    return selfD;
}

msbObject* msbObjectCreateObject(char* uuid, char* token, char* service_class, char* name, char* description) {

    msbObject* ret = (msbObject*) calloc(1, 1 * sizeof(msbObject));

    if (ret != NULL) {
        ret->objectInfo.service_uuid = string_duplicate(uuid);
        ret->objectInfo.service_token = string_duplicate(token);
        ret->objectInfo.service_class = string_duplicate(service_class);
        ret->objectInfo.service_name = string_duplicate(name);
        ret->objectInfo.service_description = string_duplicate(description);

        ret->eventArray = schll_MakeListPointer();
        ret->functionArray = schll_MakeListPointer();
        ret->configParamArray = schll_MakeListPointer();
    }

    return ret;
}

const char* getDataObjectType(PrimitiveType pt) {
    switch (pt) {
        case MSB_INTEGER:
            return "integer";
        case MSB_NUMBER:
            return "number";
        case MSB_STRING:
            return "string";
        case MSB_BOOL:
            return "boolean";
        case MSB_ARRAY:
            return "array";
        default:
            return "";
    }
}

const char* getDataObjectFormat(PrimitiveFormat pf) {
    switch (pf) {
        case MSB_INT32:
            return "int32";
        case MSB_INT64:
            return "int64";
        case MSB_FLOAT:
            return "float";
        case MSB_DOUBLE:
            return "double";
        case MSB_BYTE:
            return "byte";
        case MSB_DATE:
            return "date";
        case MSB_DATE_TIME:
            return "date-time";
        case MSB_NONE:
            return "";
        default:
            return "";
    }
}

const char* getDataObjectTypeUC(PrimitiveType pt) {
    switch (pt) {
        case MSB_INTEGER:
            return "INTEGER";
        case MSB_NUMBER:
            return "NUMBER";
        case MSB_STRING:
            return "STRING";
        case MSB_BOOL:
            return "BOOLEAN";
        case MSB_ARRAY:
            return "ARRAY";
        default:
            return "";
    }
}

const char* getDataObjectFormatUC(PrimitiveFormat pf) {
    switch (pf) {
        case MSB_INT32:
            return "INT32";
        case MSB_INT64:
            return "INT64";
        case MSB_FLOAT:
            return "FLOAT";
        case MSB_DOUBLE:
            return "DOUBLE";
        case MSB_BYTE:
            return "BYTE";
        case MSB_DATE:
            return "DATE";
        case MSB_DATE_TIME:
            return "DATE-TIME";
        case MSB_NONE:
            return "";
        default:
            return "";
    }
}
