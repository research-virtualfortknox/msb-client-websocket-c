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
* @file websocket.c
* @author D. Breunig FhG IPA
* @date 8 May 2019
* @brief source file for websocket functions
*/

#include "websocket.h"

wsConnData* wsConnDataConstr(bool _useIPv6,
                             char* _targetAddr,
                             char* _targetPort,
                             char* _targetHostname,
                             char* _targetPath) {

    wsConnData* ret = (wsConnData*) calloc(1, 1 * sizeof(wsConnData));

    ret->useIPv6 = _useIPv6;

    if (_targetAddr != NULL) ret->targetAddr = string_duplicate(_targetAddr);
    if (_targetPort != NULL) ret->targetPort = string_duplicate(_targetPort);
    if (_targetHostname != NULL) ret->targetHostname = string_duplicate(_targetHostname);
    if (_targetPath != NULL) ret->targetPath = string_duplicate(_targetPath);

    return ret;
}

void wsConnDataDestr(wsConnData* prey) {
    if (prey == NULL) return;

    free(prey->targetAddr);
    free(prey->targetPort);
    free(prey->targetHostname);
    free(prey->targetPath);

    free(prey);
}

wsData* wsDataConstr(bool _useIPv6,
                     char* _targetAddr,
                     char* _targetPort,
                     char* _targetHostname,
                     char* _targetPath,
                     bool tls, char* client_cert, char* client_key, char* ca_cert,
                     int (* debugFunction_)(const char*, ...)) {

    wsData* ret = (wsData*) calloc(1, sizeof(wsData));

    ret->status = WSD_CLEAR;

    ret->flag_TLS = (tls != 0);
    if (ca_cert != NULL) ret->tls_ca_cert = string_duplicate(ca_cert);
    if (client_cert != NULL) ret->tls_client_cert = string_duplicate(client_cert);
    if (client_key != NULL) ret->tls_client_key = string_duplicate(client_key);

    ret->connData = wsConnDataConstr(_useIPv6, _targetAddr, _targetPort, _targetHostname, _targetPath);
    ret->debugFunction = debugFunction_;

    return ret;
}

void wsDataDestr(wsData* prey) {
    if (prey->debugFunction != NULL) prey->debugFunction("Websocket: wsDataDestr\n");

    wsConnDataDestr(prey->connData);

    free(prey->tls_ca_cert);
    free(prey->tls_client_cert);
    free(prey->tls_client_key);

    wsCleanup();
}

void wsPing(wsData* data) {
    nopoll_bool ping = nopoll_conn_send_ping(data->wsVerb);

    if (data->debugFunction != NULL) data->debugFunction("Websocket: wsPing %i\n", ping);
}

char* wsGetMsgContent(wsData* data) {
    char* payload;
    payload = NULL;

    noPollMsg* in = nopoll_conn_get_msg(data->wsVerb);
    if (in != NULL) {
        payload = (char*) nopoll_msg_get_payload(in);

        if (data->debugFunction != NULL) data->debugFunction("Websocket: wsGetMsgContent: Payload %s\n", payload);
    }

    return payload;
}

void wsConnect(wsData* data) {
    //if(data->status != WSD_INITIALISED) return;

    if (data->debugFunction != NULL) {
        data->debugFunction("Websocket: wsConnect: flag TLS %i, ipv6 %i, address %s, port %s, hostname %s, path %s\n",
                            data->flag_TLS, data->connData->useIPv6, data->connData->targetAddr,
                            data->connData->targetPort, data->connData->targetHostname, data->connData->targetPath);
        if (data->flag_TLS) {
            data->debugFunction("Websocket: wsConnect: tls_client_cert %s, tls_client_key %s, tls_ca_cert %s\n",
                                data->tls_client_cert, data->tls_client_key, data->tls_ca_cert);
        }
    }

    if (data->flag_TLS == 0) {
        if (data->connData->useIPv6) {
            data->wsVerb = (noPollConn*) nopoll_conn_new6(data->wsCtx, data->connData->targetAddr,
                                                          data->connData->targetPort, data->connData->targetHostname,
                                                          data->connData->targetPath, NULL, NULL);
        } else {
            data->wsVerb = (noPollConn*) nopoll_conn_new(data->wsCtx, data->connData->targetAddr,
                                                         data->connData->targetPort, data->connData->targetHostname,
                                                         data->connData->targetPath, NULL, NULL);
        }
    } else {
        if (data->connData->useIPv6) {
            data->wsVerb = (noPollConn*) nopoll_conn_tls_new6(data->wsCtx, data->wsOpts, data->connData->targetAddr,
                                                              data->connData->targetPort,
                                                              data->connData->targetHostname,
                                                              data->connData->targetPath, NULL, NULL);
        } else {
            data->wsVerb = (noPollConn*) nopoll_conn_tls_new(data->wsCtx, data->wsOpts, data->connData->targetAddr,
                                                             data->connData->targetPort, data->connData->targetHostname,
                                                             data->connData->targetPath, NULL, NULL);
        }
    }

    if(wsIsConnectionReady(data, 100000)){
        data->status = WSD_CONNECTED;
    }
}

void wsDisconnect(wsData* data) {
    //if (data->status != WSD_CONNECTED) return;

    if (data->debugFunction != NULL) data->debugFunction("Websocket: wsDisconnect\n");

    if(wsIsConnectionOK(data)) nopoll_conn_close(data->wsVerb);

    data->status = WSD_DISCONNECTED;
}

void wsInitialise(wsData* data) {
    //if (data->status != WSD_CLEAR) return;

    if (data->debugFunction != NULL) data->debugFunction("Websocket: wsInitialise\n");

    data->wsCtx = nopoll_ctx_new();

    if (data->flag_TLS) wsTLSInitialise(data);

    data->status = WSD_INITIALISED;
}

void wsDeinitialise(wsData* data) {
    //if (data->status != WSD_INITIALISED && data->status != WSD_DISCONNECTED) return;

    if (data->debugFunction != NULL) data->debugFunction("Websocket: wsDeinitialise\n");

    nopoll_ctx_unref(data->wsCtx);
    wsCleanup();

    data->status = WSD_CLEAR;
}

int wsSend(wsData* data, const char* nachricht, size_t length) {
    uint32_t length_sent = (uint32_t) nopoll_conn_send_text(data->wsVerb, nachricht, (long) length);

    if (length_sent == length) {
        if (data->debugFunction != NULL) {
            data->debugFunction("Websocket: wsSend: message length %li, message length sent %li, message \"%s\"\n",
                                length, length_sent, nachricht);
        }

        return 0;
    }

    length_sent = (uint32_t) nopoll_conn_flush_writes(data->wsVerb, 100000, (int) length_sent);

    if (length_sent == length || length_sent == 0) {
        if (data->debugFunction != NULL) {
            data->debugFunction("Websocket: wsSend: message length %li, message length sent %li, message \"%s\"\n",
                                length, length_sent, nachricht);
        }

        return 0;
    } else {
        if (data->debugFunction != NULL) {
            data->debugFunction("Websocket: wsSend fail: message length %li, message length sent %li, message \"%s\"\n",
                                length, length_sent, nachricht);
        }

        return -1;
    }
}

bool wsIsConnectionReady(wsData* data, uint32_t timeout_in_us) {
    nopoll_bool ready = nopoll_conn_wait_until_connection_ready(data->wsVerb, timeout_in_us);

    /*if (data->debugFunction != NULL) {
        data->debugFunction("Websocket: wsIsConnectionReady: %i\n", ready);
    }*/

    return ready == 1;
}

int wsIsConnectionOK(wsData* data) {
    nopoll_bool ok = nopoll_conn_is_ok(data->wsVerb);

    if (data->debugFunction != NULL) data->debugFunction("Websocket: wsIsConnectionOK: %i\n", ok);

    return ok == 1;
}

void wsTLSInitialise(wsData* data) {
    if (data->debugFunction != NULL) data->debugFunction("Websocket: wsTLSInitialise\n");

    data->wsOpts = nopoll_conn_opts_new();
#ifndef _WIN32
    nopoll_conn_opts_set_ssl_protocol(data->wsOpts, NOPOLL_METHOD_TLSV1_2);
#else
    nopoll_conn_opts_set_ssl_protocol((*data)->wsOpts, 6);
#endif
    nopoll_conn_opts_set_reuse(data->wsOpts, nopoll_true);
}

/*zielkonfig = opts, v0 = clientzertifikat, v1 = privater schlüssel, v2 = kettenzertifikat, v3 = ca-zertifikat*/
int wsTLSCertificates(wsData* data) {
    int ssl_certs = (int) nopoll_conn_opts_set_ssl_certs(data->wsOpts, data->tls_client_cert, data->tls_client_key,
                                                         NULL, data->tls_ca_cert);

    if (data->debugFunction != NULL) data->debugFunction("Websocket: wsTLSCertificates: %i\n", ssl_certs);

    return ssl_certs;
}

void wsSetTLSVerification(wsData* data, bool on_off) {
    if (data->debugFunction != NULL) data->debugFunction("Websocket: wsSetTLSVerification: on_off %b\n", on_off);

    /*true für deaktivieren, false für aktivieren*/
    nopoll_conn_opts_ssl_peer_verify(data->wsOpts, (nopoll_bool) ((on_off - 1) * -1));
}

void wsCleanup() {
    nopoll_cleanup_library();
}
