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

#include "libMsbClientC.h"

#include "test.c"

#define UUID "dba"
#define NAME "nopollCClient"
#define DESCRIPTION "bschr"
#define CLASS "SmartObject"
#define TOKEN "sfsdfdf"

#ifndef _WIN32
#include <uuid/uuid.h>
#include <unistd.h>
#else
#include <objbase.h>
#endif

#define cmdlineparamvar(param, cmp_string, target, value)                               \
    if(strcmp(param, cmp_string) == 0 && strlen(param) == strlen(cmp_string)){          \
        target = value;                                                                 \
	}

int main(int argc, char **argv){

	if(argc < 4){
		printf("Missing arguments.\n");
		printf("Unit and integration tests:\n");
		printf("Either pass addresses using environment variables: ./MsbClientCTest --tests -w VAR_NAME_WEBSOCKET_INTERFACE -r_sm VAR_NAME_SMOBJ_MGMT -r_int VAR_NAME_INTFLOW_MGMT\n");
		printf("Or pass the addresses directly: ./MsbClientCTest --tests -w_d ADDRESS_WEBSOCKET_INTERFACE -r_sm_d ADDRESS_SMOBJ_MGMT -r_int_d ADDRESS_INTFLOW_MGMT\n");
		printf("Only unit tests:\n");
		printf("With environment variables: ./MsbClientCTest --unit_tests -w VAR_NAME_WEBSOCKET_INTERFACE\n");
		printf("Directly passed addresses: ./MsbClientCTest --unit_tests -w_d ADDRESS_WEBSOCKET_INTERFACE\n");
		printf("Only integration tests:\n");
		printf("With environment variables: ./MsbClientCTest --integration_tests -w VAR_NAME_WEBSOCKET_INTERFACE -r_sm VAR_NAME_SMOBJ_MGMT -r_int VAR_NAME_INTFLOW_MGMT\n");
		printf("Directly passed addresses: ./MsbClientCTest --integration_tests -w_d ADDRESS_WEBSOCKET_INTERFACE -r_sm_d ADDRESS_SMOBJ_MGMT -r_int_d ADDRESS_INTFLOW_MGMT\n");

		return -1;
	}

	char* wsAdr = NULL;
	char* restAdr_sm = NULL;
	char* restAdr_int = NULL;

	int i = 2;
	for(; i < argc; ++i){
		cmdlineparamvar(argv[i], "-w", wsAdr, getenv(argv[i+1]));
		cmdlineparamvar(argv[i], "-r_sm", restAdr_sm, getenv(argv[i+1]));
		cmdlineparamvar(argv[i], "-r_int", restAdr_int, getenv(argv[i+1]));
		cmdlineparamvar(argv[i], "-w_d", wsAdr, argv[i+1]);
		cmdlineparamvar(argv[i], "-r_sm_d", restAdr_sm, argv[i+1]);
		cmdlineparamvar(argv[i], "-r_int_d", restAdr_int, argv[i+1]);
	}

	bool unit_tests = strcmp(argv[1], "--tests") == 0 || strcmp(argv[1], "--unit_tests") == 0;
	bool integration_tests = strcmp(argv[1], "--tests") == 0 || strcmp(argv[1], "--integration_tests") == 0;

	if(integration_tests){
		if(!wsAdr){
			printf("Environment variable(s) for address of websocket interface missing.\n");
			return -1;
		}
		if(!restAdr_sm){
			printf("Environment variable(s) for address of REST interface of smart object management missing.\n");
			return -1;
		}
		if(!restAdr_int){
			printf("Environment variable(s) for address of REST interface of integration flow management missing.\n");
			return -1;
		}
	}else if(unit_tests){
		if(!wsAdr){
			printf("Environment variable(s) for address of websocket interface missing.\n");
			return -1;
		}
	}
	
	return test(unit_tests, integration_tests, wsAdr, restAdr_sm, restAdr_int);
}
