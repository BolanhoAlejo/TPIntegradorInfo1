#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

#define TOKEN_SIZE 100
#define URL_SIZE 2000

struct memory {
    char *response;
    size_t size;
};
static size_t cb(char *data, size_t size, size_t nmemb, void *clientp)
{
    size_t realsize = nmemb;
    struct memory *mem = clientp;
    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if(!ptr)
        return 0; /* out of memory */
    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;
    return realsize;
}
int main(void) {

    char token[TOKEN_SIZE] = "";
    char get_url[URL_SIZE] = "";
    char post_url[URL_SIZE] = "";
    int update_id = 0;
    int next_offset = 0;
    int chat_id = 0;

    FILE *file;
    file = fopen("env.txt", "r");
    if (file == NULL) {
        printf("\nNo se encuentra el archivo especificado");
    }else{
        fscanf(file, "%s", token);
        fclose(file);
    }

    char *api_url = "https://api.telegram.org/bot%s/getUpdates?offset=%d";
    char *sent_url = "https://api.telegram.org/bot%s/sendMessage?chat_id=%d&text=Hola";

    CURL *curl = curl_easy_init();

    while (1) {
        snprintf(get_url, URL_SIZE, api_url, token, next_offset);
        struct memory chunk = {0};
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, get_url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

            CURLcode res = curl_easy_perform(curl);

            char *up_id = strstr(chunk.response, "\"update_id\":");
            if (up_id != NULL) {
                up_id += strlen("\"update_id\":");
                sscanf(up_id, "%d", &update_id);
                next_offset = update_id + 1;
                printf("%s\n", chunk.response);

                char *ch_id = strstr(chunk.response, "\"from\":{\"id\":");
                if (ch_id != NULL) {
                    ch_id += strlen("\"from\":{\"id\":");
                    sscanf(ch_id, "%d", &chat_id);
                    printf("%d", chat_id);
                }

                snprintf(post_url, URL_SIZE, sent_url, token, chat_id);

                curl_easy_setopt(curl, CURLOPT_URL, post_url);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

                curl_easy_perform(curl);

            }else {
                printf(".");
            }

        }

        sleep(2);
    }
}