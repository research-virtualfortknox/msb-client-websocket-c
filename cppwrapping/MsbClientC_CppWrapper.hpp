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

#ifndef SRC_MSBCLIENTC_CPPWRAPPER_HPP
#define SRC_MSBCLIENTC_CPPWRAPPER_HPP

#include <string>
#include <json-c/json.h>

extern "C" {
#include "MsbClientC.h"
}

namespace MsbClientC_CPPInt{

class MsbClient{

private:
	msbClient* msbClientInstanz;

public:
	MsbClient(std::string URL, std::string objUUID, std::string objToken, std::string objClass, std::string objName, std::string objDesc,
			  bool tls, std::string client_cert, std::string client_key, std::string ca_cert);
	MsbClient(bool useIPv6, std::string targetAdr, std::string targetPort, std::string targetHostname, std::string targetPath,
			  std::string objUUID, std::string objToken, std::string objClass, std::string objName, std::string objDesc,
			  bool tls, std::string client_cert, std::string client_key, std::string ca_cert);
	
	virtual ~MsbClient();

	volatile const int* dataOutInterfaceFlag;

	void Initialise();
	void Connect();
	int Register();
	void Disconnect();
	void Deinitialise();
	int RunClientStateMachine();
	int HaltClientStateMachine();
	void SetHeartbeatInterval(std::uint16_t interval_in_ms);
	void SetStateMachineCyclePause(std::uint16_t pause_in_us);
	void EnableHostnameVerification();
	void DisableHostnameVerification();
	bool ConnectionReady();
	void SetSockJSFraming(bool false_true);
	void GenerateSockJSPath();
	void SetMaxMessageSize(std::uint32_t size);
	void SetReconnectInterval(std::uint16_t interval_in_ms);
	void EnableReconnect();
	void DisableReconnect();
	void SetEventCacheSize(std::uint32_t size);
	void SetFunctionCacheSize(std::uint32_t size);
	void EnableEventCache();
	void DisableEventCache();
	void SetDebug(bool on_off);

	void FwdCompleteFunctionCall();
	void FwdFunctionParameterOnly();
	void SetCommonCallbackFunction(void(*cbFunktion)(void*, void*));
	void ResetCommonCallbackFunction();

	void UseSockJSPath(std::string server_id, std::string session_id, std::string transport_id);
	bool AssignTLSCerts(std::string client_certificate, std::string client_key, std::string ca_certificate);

	void AddConfigParam(std::string cpId, PrimitiveType cpType, PrimitiveFormat cpFormat, void* value);
	void AddConfigParamFromString(std::string string);
	void AddConfigParamFromObject(json_object* object);
	void SetConfigParamPtr(std::string cpName, void* value);

	void AddFunction(std::string fId, std::string fName, std::string fDesc, void(*fPtr)(void*, void*, void*), PrimitiveType fType, PrimitiveFormat fFormat, bool isArray, void* contextPtr);
	void AddEmptyFunction(std::string fId, std::string fName, std::string fDesc, void(*fPtr)(void*, void*, void*), void* contextPtr);
	void AddFunctionFromString(std::string fId, std::string fName, std::string fDesc, void(*fPtr)(void*, void*, void*), std::string fFormat, void* contextPtr);
	void AddComplexFunction(std::string fId , std::string fName, std::string fDesc, void(*fPtr)(void*, void*, void*), json_object* fFormat, void* contextPtr);

	void AddResponseEventToFunction(std::string fId, std::string eId);

	void AddEvent(std::string eId, std::string eName, std::string eDesc, PrimitiveType eType, PrimitiveFormat eFormat, bool isArray);
	void AddEmptyEvent(std::string eId, std::string eName, std::string eDesc);
	void AddEventFromString(std::string eId, std::string eName, std::string eDesc, std::string eFormat, bool isArray);
	void AddComplexEvent(std::string eId, std::string eName, std::string eDesc, json_object* eFormat, bool isArray);

	void PublishEmpty(std::string eId, MessagePriority priority, std::string corrId);
	void Publish(std::string eId, MessagePriority priority, void* data, size_t array_length, std::string corrId);
	void PublishComplex(std::string eId, MessagePriority priority, json_object* pData, std::string corrId);
	void PublishFromString(std::string eId, MessagePriority priority, std::string pData, std::string corrId);
};

}
#endif /* SRC_MSBCLIENTC_CPPWRAPPER_HPP */
