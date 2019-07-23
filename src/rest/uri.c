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
* @file uri.c
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief uri parsing functions
*/

#include <Uri.h>
#include <string.h>

#define uriPart                                             \
            UriParserStateA state;                          \
            UriUriA uri;                                    \
            state.uri = &uri;                               \
            if (uriParseUriA(&state, adr) != URI_SUCCESS) return std;

#define stdCmpPart                                          \
            if (std != NULL) {                              \
                if (strncmp(ret, std, l) == 0) {            \
                    free(ret);                              \
                    return std;                             \
                }                                           \
            }

#define allocCopyPart(last, first)                          \
            unsigned long l = last - first;                 \
            char* ret = (char*) calloc(1, (l+1)*sizeof(char));\
            int i = 0;                                      \
            for(; i < l; ++i){                              \
                ret[i] = first[i];                          \
            }

char* getIPv4(char* adr, char* std) {

    uriPart

    if (uri.hostText.first == NULL) return std;
    if (!uri.hostData.ip4) return std;

    allocCopyPart(uri.hostText.afterLast, uri.hostText.first)

    stdCmpPart

    return ret;
}

char* getIPv6(char* adr, char* std) {

    uriPart

    if (uri.hostText.first == NULL) return std;
    if (!uri.hostData.ip6) return std;

    allocCopyPart(uri.hostText.afterLast, uri.hostText.first)

    stdCmpPart

    return ret;

}

char* getPort(char* adr, char* std) {

    uriPart

    if (uri.portText.first == NULL) return std;

    allocCopyPart(uri.portText.afterLast, uri.portText.first)

    stdCmpPart

    return ret;

}

char* getPath(char* adr, char* std) {

    uriPart

    if (uri.pathHead == NULL) return std;
    if (uri.pathHead->text.first == NULL) return std;

    unsigned long l = strlen(uri.pathHead->text.first);
    char* ret = (char*) calloc(1, (l + 2) * sizeof(char));
    strncpy(&ret[1], uri.pathHead->text.first, l);

    ret[0] = '/';

    stdCmpPart

    return ret;

}

char* getProtocol(char* adr, char* std) {

    uriPart

    if (uri.scheme.first == NULL) return std;

    allocCopyPart(uri.scheme.afterLast, uri.scheme.first)

    stdCmpPart

    return ret;

}

char* getHostname(char* adr, char* std) {

    uriPart

    if (uri.hostText.first == NULL) return std;

    allocCopyPart(uri.hostText.afterLast, uri.hostText.first)

    stdCmpPart

    return ret;

}