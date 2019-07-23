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
* @file websocket.h
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief header file for websocket functions
*/

#ifndef SRC_WEBSOCKET_H_
#define SRC_WEBSOCKET_H_

#include <stdint.h>
#include <stdbool.h>
#include "nopoll.h"
#include "../rest/helpers.h"

typedef struct {
    bool useIPv6;

    char* targetAddr;
    char* targetPort;
    char* targetHostname;
    char* targetPath;
    char* origin;
} wsConnData;

wsConnData* wsConnDataConstr(bool _useIPv6,
                             char* _targetAddr,
                             char* _targetPort,
                             char* _targetHostname,
                             char* _targetPath,
                             char* _origin);

void wsConnDataDestr(wsConnData* prey);

enum wsDataStatus {
    WSD_CLEAR,
    WSD_INITIALISED,
    WSD_DISCONNECTED,
    WSD_CONNECTED
};

typedef struct {

    enum wsDataStatus status;

    noPollCtx* wsCtx;
    noPollConn* wsVerb;
    noPollConnOpts* wsOpts;

    wsConnData* connData;

    bool flag_TLS;
    char* tls_client_cert;
    char* tls_client_key;
    char* tls_ca_cert;

    int (*debugFunction)(const char* format, ...);

} wsData;

wsData* wsDataConstr(bool _useIPv6,
                     char* _targetAddr,
                     char* _targetPort,
                     char* _targetHostname,
                     char* _targetPath,
                     char* _origin,
                     bool tls, char* client_cert, char* client_key, char* ca_cert,
                     int (*debugFunction_)(const char*, ...));

void wsDataDestr(wsData* prey);

void wsPing(wsData* data);

char* wsGetMsgContent(wsData* data);

void wsConnect(wsData* data);

void wsDisconnect(wsData* data);

void wsInitialise(wsData* data);

void wsDeinitialise(wsData* data);

int wsSend(wsData* data, const char* nachricht, size_t length);

bool wsIsConnectionReady(wsData* data, uint32_t timeout_in_us);

int wsIsConnectionOK(wsData* data);

void wsTLSInitialise(wsData* data);

int wsTLSCertificates(wsData* data);

void wsSetTLSVerification(wsData* data, bool on_off);

void wsCleanup();

void logHandler(noPollCtx * ctx, noPollDebugLevel level, const char * log_msg, noPollPtr user_data);

//void setDebugHandling(wsData* data);

#endif /* SRC_WEBSOCKET_H_ */
