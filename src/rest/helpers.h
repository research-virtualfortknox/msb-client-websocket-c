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
* @file helpers.h
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief some helpers for msb c client
*/

#ifndef SRC_HELPERS_H_
#define SRC_HELPERS_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#ifndef _WIN32
#include <time.h>
#include <sys/time.h>
#else
#include <WinSock2.h>
#include <Windows.h>

typedef struct {
	long tv_sec;
	long tv_usec;
} timeval;
#endif

static inline int string_replace(char* original_string, char* part_to_replace, char* part_to_insert, char** output) {

	if (!original_string || !part_to_replace || !part_to_insert) return -1;

	size_t l_eing = strlen(original_string);
	size_t l_sub = strlen(part_to_replace);
	size_t l_ers = strlen(part_to_insert);

	int i = 0;

	char* ptr = strstr(original_string, part_to_replace);
	for(; ptr != NULL; ptr = strstr(ptr, part_to_replace)){
		++i;
		++ptr;
	}

	size_t l_neu = l_eing + (l_ers-l_sub)*i;

	*output = (char*)calloc(sizeof(char), l_neu+1);

	if(*output == NULL) return -1;

	char* stand = original_string;
	char* stand_a = *output;

	ptr = strstr(original_string, part_to_replace);
	for(; i >= 0; --i) {
		if (ptr != NULL) {
			strncpy(stand_a, stand, ptr - stand);
			stand_a += (ptr-stand);
			strncpy(stand_a, part_to_insert, l_ers);
			stand_a += l_ers;
			stand = ptr+l_sub;
			ptr = strstr(stand, part_to_replace);
		}else{
			strcpy(stand_a, stand);
		}
	}

	return 0;
}

static inline char* string_combine(const char* s1, const char* s2) {
	size_t l1 = 0, l2 = 0;

	if(s1 != NULL) l1 = strlen(s1);
	if(s2 != NULL) l2 = strlen(s2);

	char *result = (char*)calloc(1, (l1+l2+1)*sizeof(char) );

	if (result == NULL) return NULL;

	if(l1) strcat(result, s1);
	if(l2) strcat(result, s2);

	return result;
}

static inline char* string_duplicate(const char* string){
	char* p = (char*)calloc(1, strlen(string)+1);

	if(p == NULL) return NULL;

	return memcpy((void*)p, (const void*)string, strlen(string)+1);
}

#ifdef _WIN32
static inline int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);

	return 0;
}
#endif

static inline void getDateTime(char buffer[]) {
	long millisec = 0;
	struct tm* tm_info;
	struct timeval tv;
	gettimeofday(&tv, NULL);

	millisec = lrint(tv.tv_usec/1000.0); /*// Round to nearest millisec*/
	if (millisec>=1000) { /*// Allow for rounding up to nearest second*/
		millisec -=1000;
		tv.tv_sec++;
	}

	tm_info = localtime(&tv.tv_sec);
	strftime(buffer, 30, "%Y-%m-%dT%H:%M:%S:::::%z", tm_info);
	sprintf(&buffer[19], ".%03li", millisec);
	buffer[23] = buffer[24];
	buffer[24] = buffer[25];
	buffer[25] = buffer[26];
	buffer[26] = ':';
}

#endif /* SRC_HELPERS_H_ */
