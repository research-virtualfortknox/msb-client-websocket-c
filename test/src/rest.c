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

#include "rest/helpers.h"
#include <curl/curl.h>

char* rest_tmp_body = NULL;
int (*debugOutputFunction)(const char*, ...) = NULL;

static size_t function_pt(void *ptr, size_t size, size_t nmemb, void *stream){

    if(rest_tmp_body != NULL)
        free(rest_tmp_body);

    rest_tmp_body = string_duplicate((char*)ptr);

    return size*nmemb;
}

int rest_get(char* url, char** resp_body, char* comm){
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "accept: application/json");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        //fprintf(stderr, "%s: Response code REST GET %ld\n", comm, response_code);
        debugOutputFunction("%s: Response code REST GET %ld\n", comm, response_code);

        if(res != CURLE_OK) {
            //fprintf(stderr, "%s: get: curl_easy_perform() failed: %s\nurl %s", comm, curl_easy_strerror(res), url);
            debugOutputFunction("%s: get: curl_easy_perform() failed: %s\nurl %s\n", comm, curl_easy_strerror(res), url);
            return -1;
        }

        *resp_body = rest_tmp_body;
        curl_easy_cleanup(curl);

        return (int)response_code;
    }else{
        return -1;
    }
}

int rest_put(char* url, char** resp_body, char* accept_header, char* comm){

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, accept_header);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);

        res = curl_easy_perform(curl);

        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        //fprintf(stderr, "%s: Response code REST PUT %ld\n", comm, response_code);
        debugOutputFunction("%s: Response code REST PUT %ld\n", comm, response_code);

        if(res != CURLE_OK) {
            //fprintf(stderr, "%s: put: curl_easy_perform() failed: %s\nurl %s", comm, curl_easy_strerror(res), url);
            debugOutputFunction("%s: put: curl_easy_perform() failed: %s\nurl %s\n", comm, curl_easy_strerror(res), url);
            debugOutputFunction()
            return -1;
        }

        *resp_body = rest_tmp_body;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return (int)response_code;
    }else{
        return -1;
    }
}

int rest_post(char* url, char* post, char** resp_body, char* accept_header, char* content_header, char* comm){
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, accept_header);
        headers = curl_slist_append(headers, content_header);
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
        res = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(post));
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);
        //res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        //fprintf(stderr, "%s: Response code REST POST %ld\n", comm, response_code);
        debugOutputFunction("%s: Response code REST POST %ld\n", comm, response_code);

        if(res != CURLE_OK) {
            //fprintf(stderr, "%s: post: curl_easy_perform() failed: %s\nurl %s", comm, curl_easy_strerror(res), url);
            debugOutputFunction("%s: post: curl_easy_perform() failed: %s\nurl %s\n", comm, curl_easy_strerror(res), url);
            debugOutputFunction("%s: post: Post field: %s\n", comm, post);
            return -1;
        }

        *resp_body = rest_tmp_body;
        curl_easy_cleanup(curl);

        return (int)response_code;
    }else{
        return -1;
    }
}

int rest_delete(char* url, char* comm){
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "accept: */*");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        //res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        //fprintf(stderr, "%s: Response code REST DELETE %ld\n", comm, response_code);
        debugOutputFunction("%s: Response code REST DELETE %ld\n", comm, response_code);

        if(res != CURLE_OK) {
            //fprintf(stderr, "%s: delete: curl_easy_perform() failed: %s\nurl %s", comm, curl_easy_strerror(res), url);
            debugOutputFunction("%s: delete: curl_easy_perform() failed: %s\nurl %s\n", comm, curl_easy_strerror(res), url);
            return -1;
        }

        curl_easy_cleanup(curl);

        return (int)response_code;
    }else{
        return -1;
    }
}