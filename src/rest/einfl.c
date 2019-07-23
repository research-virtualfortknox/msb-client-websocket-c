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
* @file einfl.c
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief simple list implementation
*/

/*
* Implementierungen
*/

list_list* list_makeListPointer() {

    list_list* ret;
    ret = (list_list*) calloc(1, sizeof(list_list));

    pthread_mutex_init(&ret->mutex, NULL);

    return ret;

}

list_return_value list_makeList(list_list** list){

    *list = (list_list*)calloc(1, sizeof(list_list));
    if(*list != NULL) return EINFL_OK;

    pthread_mutex_init(&(*list)->mutex, NULL);

    return EINFL_LIST_NULL;

}

unsigned int list_count_entries(list_list* list){

    if (list == NULL) return -1;

    pthread_mutex_lock(&list->mutex);

    unsigned int ret;

    ret = list->number_entries;

    pthread_mutex_unlock(&list->mutex);

    return ret;

}

list_return_value list_deleteList(list_list* list, int free_value) {

    if (list == NULL) return EINFL_LIST_NULL;

    if(!list_count_entries(list)) return EINFL_NO_ENTRIES;

    pthread_mutex_lock(&list->mutex);

    list_entry* p;
    p = list->first_entry;

    while (p != NULL) {
        list_entry* t;
        t = p->next_entry;

        if (free_value) {
            free(p->value);
        }

        free(p);
        p = t;
    }

    free(list);

    pthread_mutex_unlock(&list->mutex);

    return EINFL_OK;

}

list_return_value list_add(list_list* list, void* value, int copy_value, size_t value_size) {

    if(list == NULL) return EINFL_LIST_NULL;

    if(value == NULL) return EINFL_NULL_VALUE;

    pthread_mutex_lock(&list->mutex);

    if(list->number_entries == 0){
        list->first_entry = calloc(1, sizeof(list_entry));

        list->first_entry->value = (void*)value;
        list->first_entry->next_entry = NULL;

        if (copy_value) {
            list->first_entry->value = (char*) calloc(value_size, 1);
            memcpy(list->first_entry->value, value, value_size);
        }
    }else{
        list_entry* p;
        p = list->first_entry;//->next_entry;

        unsigned int i;
        for(i = 0; i < list->number_entries-1; ++i){
            p = p->next_entry;
        }

        p->next_entry = (list_entry*) calloc(1, sizeof(list_entry));

        p->next_entry->value = (void*)value;
        p->next_entry->next_entry = NULL;

        if (copy_value) {
            p->next_entry->value = (char*) calloc(value_size, 1);
            memcpy(p->next_entry->value, value, value_size);
        }
    }



/*
    if(list->first_entry == NULL){
        list->first_entry = calloc(1, sizeof(list_entry));

        list->first_entry->value = (void*)value;
        list->first_entry->next_entry = NULL;

        if (copy_value) {
            list->first_entry->value = (char*) calloc(value_size, 1);
            memcpy(list->first_entry->value, value, value_size);
        }
    }else{
        list_entry* p;
        p = list->first_entry;

        while(p->next_entry != NULL){
            p = p->next_entry;
        }

        p->next_entry = (list_entry*) calloc(1, sizeof(list_entry));

        p->next_entry->value = (void*)value;
        p->next_entry->next_entry = NULL;

        if (copy_value) {
            p->next_entry->value = (char*) calloc(value_size, 1);
            memcpy(p->next_entry->value, value, value_size);
        }
    }*/

    list->number_entries++;

    pthread_mutex_unlock(&list->mutex);

    return EINFL_OK;

}

list_return_value list_pop(list_list* list, unsigned int index, int free_value) {

    if (list == NULL) return EINFL_LIST_NULL;

    if (list->first_entry == NULL) return EINFL_NO_ENTRIES;

    pthread_mutex_lock(&list->mutex);

    list_entry* p;

    if(index == 0){
        if(list->number_entries > 1){
            p = list->first_entry->next_entry;

            if(free_value){
                free(list->first_entry->value);
            }
            free(list->first_entry);

            list->first_entry = p;
        }else if(list->number_entries == 1){
            if(free_value){
                free(list->first_entry->value);
            }
            free(list->first_entry);
        }else{

        }
    }else{
        list_entry* vorg;

        vorg = list->first_entry;
        p = vorg->next_entry;

        unsigned int i;
        for(i = 1; i < index; ++i){
            vorg = p;
            p = p->next_entry;
        }

        vorg->next_entry = p->next_entry;
        if(free_value){
            free(p->value);
        }
        free(p);
    }

    list->number_entries--;

    pthread_mutex_unlock(&list->mutex);

    return EINFL_OK;

}

list_return_value list_popBack(list_list* liste, int free_wert) {

    return list_pop(liste, (list_count_entries(liste) - 1), free_wert);

}

list_return_value list_getValue(list_list* list, void** value, unsigned int index) {

    if (list == NULL) return EINFL_LIST_NULL;

    if (list_count_entries(list) <= index) return EINFL_NO_ENTRY;

    list_return_value ret;
    ret = EINFL_NO_ENTRY;

    pthread_mutex_lock(&list->mutex);

    unsigned int i;
    i = 0;

    list_entry* p;
    p = list->first_entry;

    while (i < index && p != NULL) {
        ++i;
        p = p->next_entry;
    }

    if (p != NULL) {
        *value = p->value;
        ret = EINFL_OK;
    }

    pthread_mutex_unlock(&list->mutex);

    return ret;

}

list_return_value list_getValueFirst(list_list* list, void** value) {

    return list_getValue(list, value, 0);

}

list_return_value list_getValueLast(list_list* list, void** value) {

    return list_getValue(list, value, (list->number_entries - 1));

}

void* list_getValuePointer(list_list* list, unsigned int index) {

    void* v;
    v = NULL;

    list_getValue(list, &v, index);

    return v;

}

void* list_getValueFirstPointer(list_list* list) {

    return list_getValuePointer(list, 0);

}

void* list_getValueLastPointer(list_list* list) {

    return list_getValuePointer(list, (list_count_entries(list) - 1));

}