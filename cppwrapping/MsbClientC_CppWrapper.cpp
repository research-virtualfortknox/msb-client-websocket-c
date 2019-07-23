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

using namespace MsbClientC_CPPInt;

MsbClient::MsbClient(std::string URL, std::string objUUID, std::string objToken, std::string objClass,
					 std::string objName, std::string objDesc,
					 bool tls, std::string client_cert, std::string client_key, std::string ca_cert) {
	this->msbClientInstanz = msbClientNewClientURL((char*) URL.c_str(), (char*) objUUID.c_str(),
													(char*) objToken.c_str(), (char*) objClass.c_str(),
													(char*) objName.c_str(),
													(char*) objDesc.c_str(), tls, (char*) client_cert.c_str(),
													(char*) client_key.c_str(), (char*) ca_cert.c_str());

	this->dataOutInterfaceFlag = &(this->msbClientInstanz->dataOutInterfaceFlag);
};

MsbClient::MsbClient(bool useIPv6, std::string targetAdr, std::string targetPort, std::string targetHostname,
					 std::string targetPath,
					 std::string objUUID, std::string objToken, std::string objClass, std::string objName,
					 std::string objDesc, bool tls, std::string client_cert, std::string client_key,
					 std::string ca_cert) {
	this->msbClientInstanz = msbClientNewClient(useIPv6, (char*) targetAdr.c_str(), (char*) targetPort.c_str(),
												 (char*) targetHostname.c_str(), (char*) targetPath.c_str(),
												 (char*) objUUID.c_str(),
												 (char*) objToken.c_str(), (char*) objClass.c_str(),
												 (char*) objName.c_str(), (char*) objDesc.c_str(), tls,
												 (char*) client_cert.c_str(), (char*) client_key.c_str(),
												 (char*) ca_cert.c_str());

	this->dataOutInterfaceFlag = &(this->msbClientInstanz->dataOutInterfaceFlag);
};

MsbClient::~MsbClient() {
	msbClientDeleteClient(this->msbClientInstanz);
}

void MsbClient::Initialise() {
	msbClientInitialise(this->msbClientInstanz);
}

void MsbClient::Connect() {
	msbClientConnect(this->msbClientInstanz);
}

int MsbClient::Register() {
	return msbClientRegister(this->msbClientInstanz);
}

void MsbClient::Disconnect() {
	msbClientDisconnect(this->msbClientInstanz);
}

void MsbClient::Deinitialise() {
	msbClientDeinitialise(this->msbClientInstanz);
}

int MsbClient::RunClientStateMachine() {
	return msbClientRunClientStateMachine(this->msbClientInstanz);
}

int MsbClient::HaltClientStateMachine() {
	return msbClientHaltClientStateMachine(this->msbClientInstanz);
}

void MsbClient::SetHeartbeatInterval(std::uint16_t interval_in_ms) {
	msbClientSetHeartbeatInterval(this->msbClientInstanz, interval_in_ms);
}

void MsbClient::SetStateMachineCyclePause(std::uint16_t pause_in_us) {
	msbClientSetStateMachineCyclePause(this->msbClientInstanz, pause_in_us);
}

void MsbClient::EnableHostnameVerification() {
	msbClientEnableHostnameVerification(this->msbClientInstanz);
}

void MsbClient::DisableHostnameVerification() {
	msbClientDisableHostnameVerification(this->msbClientInstanz);
}

bool MsbClient::ConnectionReady() {
	return msbClientConnectionReady(this->msbClientInstanz);
}

void MsbClient::SetSockJSFraming(bool false_true) {
	msbClientSetSockJSFraming(this->msbClientInstanz, false_true);
}

void MsbClient::SetMaxMessageSize(std::uint32_t size) {
	msbClientSetMaxMessageSize(this->msbClientInstanz, size);
}

void MsbClient::SetReconnectInterval(std::uint16_t interval_in_ms) {
	msbClientSetReconnectInterval(this->msbClientInstanz, interval_in_ms);
}

void MsbClient::EnableReconnect() {
	msbClientEnableReconnect(this->msbClientInstanz);
}

void MsbClient::DisableReconnect() {
	msbClientDisableReconnect(this->msbClientInstanz);
}

void MsbClient::SetEventCacheSize(std::uint32_t size) {
	msbClientSetEventCacheSize(this->msbClientInstanz, size);
}

void MsbClient::SetFunctionCacheSize(std::uint32_t size) {
	msbClientSetFunctionCacheSize(this->msbClientInstanz, size);
}

void MsbClient::EnableEventCache() {
	msbClientEnableEventCache(this->msbClientInstanz);
}

void MsbClient::DisableEventCache() {
	msbClientDisableEventCache(this->msbClientInstanz);
}

void MsbClient::FwdCompleteFunctionCall() {
	msbClientFwdCompleteFunctionCall(this->msbClientInstanz);
}

void MsbClient::FwdFunctionParameterOnly() {
	msbClientFwdParametersOnlyFunctionCall(this->msbClientInstanz);
}

void MsbClient::SetCommonCallbackFunction(void(* cbFunktion)(void*, void*)) {
	msbClientSetCommonCallbackFunction(this->msbClientInstanz, cbFunktion);
}

void MsbClient::ResetCommonCallbackFunction() {
	msbClientRemoveCommonCallbackFunction(this->msbClientInstanz);
}

void MsbClient::UseSockJSPath(std::string server_id, std::string session_id, std::string transport_id) {
	msbClientUseSockJSPath(this->msbClientInstanz, (char*) server_id.c_str(), (char*) session_id.c_str(),
							(char*) transport_id.c_str());
}

void MsbClient::GenerateSockJSPath() {
	msbClientGenerateSockJSPath(this->msbClientInstanz);
}

bool MsbClient::AssignTLSCerts(std::string client_certificate, std::string client_key, std::string ca_certificate) {
	return msbClientAssignTLSCerts(this->msbClientInstanz, client_certificate.c_str(), client_key.c_str(),
									ca_certificate.c_str());
}

void MsbClient::AddConfigParam(std::string cpId, PrimitiveType cpType, PrimitiveFormat cpFormat, void* value) {
	msbClientAddConfigParam(this->msbClientInstanz, (char*) cpId.c_str(), cpType, cpFormat, value);
}

void MsbClient::AddConfigParamFromString(std::string string) {
	msbClientAddConfigParamFromString(this->msbClientInstanz, (char*) string.c_str());
}

void MsbClient::AddConfigParamFromObject(json_object* object){
	msbClientAddConfigParamFromObject(this->msbClientInstanz, object);
}

void MsbClient::SetConfigParamPtr(std::string cpName, void* value) {
	msbClientChangeConfigParamValue(this->msbClientInstanz, (char*) cpName.c_str(), value);
}

void MsbClient::AddFunction(std::string fId, std::string fName, std::string fDesc, void (* fPtr)(void*, void*, void*),
							PrimitiveType fType, PrimitiveFormat fFormat, bool isArray, void* contextPtr) {
	msbClientAddFunction(this->msbClientInstanz, (char*) fId.c_str(), (char*) fName.c_str(), (char*) fDesc.c_str(),
						  fPtr, fType, fFormat, isArray, contextPtr);
}

void MsbClient::AddEmptyFunction(std::string fId, std::string fName, std::string fDesc, void (* fPtr)(void*, void*, void*),
								 void* contextPtr) {
	msbClientAddEmptyFunction(this->msbClientInstanz, (char*) fId.c_str(), (char*) fName.c_str(),
							   (char*) fDesc.c_str(), fPtr, contextPtr);
}

void
MsbClient::AddFunctionFromString(std::string fId, std::string fName, std::string fDesc, void (* fPtr)(void*, void*, void*),
								 std::string fFormat, void* contextPtr) {
	msbClientAddFunctionFromString(this->msbClientInstanz, (char*) fId.c_str(), (char*) fName.c_str(),
									(char*) fDesc.c_str(), fPtr, (char*) fFormat.c_str(), contextPtr);
}

void MsbClient::AddComplexFunction(std::string fId, std::string fName, std::string fDesc, void (* fPtr)(void*, void*, void*),
								   json_object* fFormat, void* contextPtr) {
	msbClientAddComplexFunction(this->msbClientInstanz, (char*) fId.c_str(), (char*) fName.c_str(),
								 (char*) fDesc.c_str(), fPtr, fFormat, contextPtr);
}

void MsbClient::AddResponseEventToFunction(std::string fId, std::string eId) {
	msbClientAddResponseEventToFunction(this->msbClientInstanz, (char*) fId.c_str(), (char*) eId.c_str());
}

void
MsbClient::AddEvent(std::string eId, std::string eName, std::string eDesc, PrimitiveType eType, PrimitiveFormat eFormat,
					bool isArray) {
	msbClientAddEvent(this->msbClientInstanz, (char*) eId.c_str(), (char*) eName.c_str(), (char*) eDesc.c_str(), eType,
					   eFormat, isArray);
}

void MsbClient::AddEmptyEvent(std::string eId, std::string eName, std::string eDesc) {
	msbClientAddEmptyEvent(this->msbClientInstanz, (char*) eId.c_str(), (char*) eName.c_str(), (char*) eDesc.c_str());
}

void MsbClient::AddEventFromString(std::string eId, std::string eName, std::string eDesc, std::string eFormat,
								   bool isArray) {
	msbClientAddEventFromString(this->msbClientInstanz, (char*) eId.c_str(), (char*) eName.c_str(),
								 (char*) eDesc.c_str(), (char*) eFormat.c_str(), isArray);
}

void
MsbClient::AddComplexEvent(std::string eId, std::string eName, std::string eDesc, json_object* eFormat, bool isArray) {
	msbClientAddComplexEvent(this->msbClientInstanz, (char*) eId.c_str(), (char*) eName.c_str(), (char*) eDesc.c_str(),
							  eFormat, isArray);
}

void MsbClient::PublishEmpty(std::string eId, MessagePriority prio, std::string corrId) {
	msbClientPublishEmpty(this->msbClientInstanz, (char*) eId.c_str(), prio,
						   !corrId.empty() ? (char*) corrId.c_str() : NULL);
}

void
MsbClient::Publish(std::string eId, MessagePriority priority, void* data, size_t array_length, std::string corrId) {
	msbClientPublish(this->msbClientInstanz, (char*) eId.c_str(), priority, data, array_length,
					 !corrId.empty() ? (char*) corrId.c_str() : NULL);
}

void MsbClient::PublishComplex(std::string eId, MessagePriority priority, json_object* pData, std::string corrId) {
	msbClientPublishComplex(this->msbClientInstanz, (char*) eId.c_str(), priority, pData,
							!corrId.empty() ? (char*) corrId.c_str() : NULL);
}

void MsbClient::PublishFromString(std::string eId, MessagePriority priority, std::string pData, std::string corrId) {
	msbClientPublishFromString(this->msbClientInstanz, (char*) eId.c_str(), priority, (char*) pData.c_str(),
							   !corrId.empty() ? (char*) corrId.c_str() : NULL);
}

void MsbClient::SetDebug(bool on_off) {
	msbClientSetDebug(this->msbClientInstanz, on_off);
}