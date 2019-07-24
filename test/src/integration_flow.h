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
* @file integration_flow.h
* @author D. Breunig FhG IPA
* @date 24 July 2019
* @brief Integration flow definition for testing
*/

#ifndef MSBCLIENTC_TEST_INTEGRATION_FLOW_H
#define MSBCLIENTC_TEST_INTEGRATION_FLOW_H

const char* integration_flow = "{"
"   \"@id\":1,"
"   \"name\": \"integration_flow\","
"   \"description\": \"\","
"   \"ownerUuid\": \"7c328ad1-cea5-410e-8dd8-6c7ca5a2e4f5\","
"   \"transitions\":["
"      {"
"         \"@id\":1,"
"         \"sources\":[],"
"         \"targets\":[],"
"         \"inputs\":["
"            {"
"               \"@id\":1,"
"               \"transitionInput\":{"
"                  \"@id\":1,"
"                  \"connectionFormat\":\"JSON\","
"                  \"inputParameters\":[1],"
"                  \"eventId\":\"Ev2\","
"                  \"service\":{"
"                     \"@id\":1,"
"                     \"uUID\": \"a209efb9-af3f-4d93-bad4-05215a983164\","
"                     \"name\": \"Unit_Test_C-Client\","
"                     \"type\": \"SMART_OBJECT\","
"                     \"events\":[1],"
"                     \"endpoints\":[]"
"                  },"
"                  \"type\":\"InputEvent\""
"               },"
"               \"processors\":["
"                  {"
"                     \"@id\":1,"
"                     \"className\":\"processor.mapping.datamappingprocessor\","
"                     \"inputs\":[1],"
"                     \"output\":{"
"                        \"@id\":2,"
"                        \"transitionOutput\":{"
"                           \"@id\":1,"
"                           \"connectionFormat\":\"JSON\","
"                           \"outputParameters\":[2],"
"                           \"functionId\":\"F2\","
"                           \"serviceEndpoint\":{"
"                              \"@id\":1,"
"                              \"endpointUri\": \"vfk.msb.interface.websocket[127.0.0.1:8084]\","
"                              \"connectionType\": \"WEBSOCKET\","
"                              \"functionCalls\":[1],"
"                              \"service\":{"
"                                 \"@id\":2,"
"                                 \"uUID\": \"a209efb9-af3f-4d93-bad4-05215a983164\","
"                                 \"name\": \"Unit_Test_C-Client\","
"                                 \"type\": \"SMART_OBJECT\","
"                                 \"events\":[],"
"                                 \"endpoints\":[1]"
"                              }"
"                           },"
"                           \"type\":\"FunctionCall\""
"                        },"
"                        \"processor\":1,"
"                        \"direction\":\"Output\""
"                     },"
"                     \"pattern\":{"
"                        \"mapping\":["
"                           {"
"                              \"source\": ["
"                                 {"
"                                    \"path\": \"/dataObject\","
"                                    \"type\": \"STRING\""
"                                 }"
"                              ],"
"                              \"target\": {"
"                                 \"path\": \"/dataObject\","
"                                 \"type\": \"STRING\","
"                                 \"template\": \"${/dataObject}\""
"                              }"
"                           }"
"                        ],"
"                        \"template\": \"{\\\"dataObject\\\":{\\\"type\\\":\\\"string\\\"}}\""
"                     }"
"                  }"
"               ],"
"               \"direction\":\"Input\""
"            }"
"         ],"
"         \"outputs\":[2],"
"         \"processors\":[1]"
"      }"
"   ],"
"   \"services\": ["
"      1,"
"      2"
"   ]"
"}";

#endif //MSBCLIENTC_TEST_INTEGRATION_FLOW_H
