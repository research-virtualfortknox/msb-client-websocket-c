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
* @file schll.c
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief linked list implementation
*/

#ifndef SCHLL_H
#define SCHLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Definitionen
 */

schll_list* schll_MakeListPointer(){

	schll_list* ret = (schll_list*)calloc(1, sizeof(schll_list));
	return ret;

}

schll_return_value schll_MakeList(schll_list** list){

	*list = (schll_list*)calloc(1, sizeof(schll_list));
	if(*list != NULL) return SCHLL_OK;

	return SCHLL_ALLOC_FAIL;

}

schll_return_value schll_DeleteList(schll_list** list, const int free_names, const int free_values){

	if(list == NULL) return SCHLL_MAP_NULL;

	if(*list == NULL) return SCHLL_MAP_NULL;

	schll_entry* p = (*list)->first_entry;

	while(p != NULL) {
		schll_entry* t = p->next_entry;

		if(free_names){
			free(p->name);
		}

		if(free_values){
			free(p->value);
		}

		free(p);

		p = t;
	}

	free(*list);
	*list = NULL;

	return SCHLL_OK;

}

schll_return_value schll_addEntry(schll_list* list, const char* name, const int copy_name,
								  const void* value, const int copy_value, size_t value_size){

	if(list == NULL) return SCHLL_MAP_NULL;


	schll_entry neuer_eintrag;
	neuer_eintrag.name = (char*)name;
	neuer_eintrag.name_l = strlen(name)+1;
	neuer_eintrag.value = (void*)value;
	neuer_eintrag.next_entry = NULL;

	schll_entry** p = &(list->first_entry);

	while(*p != NULL) {
		if ((*p)->name_l == neuer_eintrag.name_l) {
			if (strncmp((*p)->name, neuer_eintrag.name, neuer_eintrag.name_l) == 0) {
				return SCHLL_SCHLL_EXISTS;
			}
		}

		p = &((*p)->next_entry);
	}

	(*p) = (schll_entry*)calloc(1, sizeof(schll_entry));

	if((*p) == NULL) return SCHLL_ALLOC_FAIL;

	(*p)->name_l = neuer_eintrag.name_l;

	if(copy_name){
		(*p)->name = (char*)calloc(neuer_eintrag.name_l, 1);

		if((*p)->name == NULL){
			free(p);
			return SCHLL_ALLOC_FAIL;
		}

		strcat((*p)->name, neuer_eintrag.name);
	}else{
		(*p)->name = neuer_eintrag.name;
	}

	if(copy_value){
		(*p)->value = (char*)calloc(value_size, 1);

		if((*p)->value == NULL){
			if(copy_name){
				free((*p)->name);
			}
			free(p);
			return SCHLL_ALLOC_FAIL;
		}

		memcpy((*p)->value, value, value_size);
	}else{
		(*p)->value = neuer_eintrag.value;
	}

	list->number_entries++;

	return SCHLL_OK;
}

schll_return_value schll_removeEntry(schll_list* list, const char* name, const int free_name,
									 const int free_value){

	if(list == NULL) return SCHLL_MAP_NULL;

	if(name == NULL) return SCHLL_NAME_MISSING;

	if(list->first_entry == NULL) return SCHLL_NO_ENTRIES;

	schll_entry* p_ = NULL;
	schll_entry* p = list->first_entry;

	size_t l;
	l = strlen(name)+1;

	while(p != NULL) {
		if (p->name_l == l) {
			if (strncmp(p->name, name, l) == 0) {

				if(p_ == NULL){
					list->first_entry = list->first_entry->next_entry;
				}else{
					p_->next_entry = p->next_entry;
				}

				if(free_name){
					free(p->name);
				}

				if(free_value){
					free(p->value);
				}

				free(p);
				list->number_entries--;

				return SCHLL_OK;
			}
		}

		p_ = p;
		p = p->next_entry;

	}

	return SCHLL_NO_ENTRY;

}

schll_return_value schll_getValue(const schll_list* list, void** value, const char* name){

	schll_entry* e;
	e = NULL;

	size_t l;
	l = strlen(name)+1;

	schll_entry* p = list->first_entry;

	while(p != NULL) {
		if (p->name_l == l) {
			if (strncmp(p->name, name, l) == 0) {
				*value = p->value;
				return SCHLL_OK;
			}
		}

		p = p->next_entry;
	}

	return SCHLL_NO_ENTRY;
}

void* schll_getValuePointer(const schll_list* list, const char* name){

	void* v;
	v = NULL;

	schll_getValue(list, &v, name);
	return v;

}

#ifdef __cplusplus
}
#endif

#endif