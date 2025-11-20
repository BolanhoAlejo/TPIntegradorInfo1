#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <time.h>

#define TOKEN_SIZE 100
#define URL_SIZE 2000
#define TEXT_SIZE 500
#define NUM_FRASES 20
#define LOG_FILE "logs.txt"

//https://github.com/BolanhoAlejo/TPIntegradorInfo1

void log_message(long timestamp, char *name, char *respuesta);
void limpiar_texto(char *texto);

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

    srand(time(NULL));

    char token[TOKEN_SIZE] = "";
    char get_url[URL_SIZE] = "";
    char post_url[URL_SIZE] = "";
    char name[TEXT_SIZE] = "";
    char text[TEXT_SIZE] = "";
    char respuesta[TEXT_SIZE] = "";
    long long int update_id = 0;
    long long int next_offset = 0;
    long long int chat_id = 0;
    int indice = 0;
    long long int date = 0;
    const char *frases[NUM_FRASES] = {
        "Mi%20CPU%20se%20acaba%20de%20tomar%20un%20descanso.%20Podrias%20repetirlo%20mas%20lento.",
        "Eso%20no%20esta%20en%20mis%20scripts%20de%20sabidura.%20Prueba%20con%20algo%20mas%20terrenal.",
        "He%20consultado%20a%20mi%20base%20de%20datos%20y%20me%20ha%20devuelto%20un%20emoji%20de%20confusion.%20",
        "Parece%20que%20mi%20cable%20de%20sentido%20comun%20se%20desconecto.%20Que%20dijiste.",
        "Mi%20algoritmo%20de%20respuesta%20esta%20actualmente%20en%20una%20reunion%20volvemos%20pronto.",
        "Disculpa%20creo%20que%20mi%20respuesta%20se%20perdio%20en%20un%20bucle%20temporal.%20",
        "Esa%20frase%20suena%20a%20idioma%20aliengena.%20Podras%20traducirla%20al%20espaniol%20terricola.",
        "Error%20404%20Respuesta%20relevante%20no%20encontrada.%20Intenta%20otra%20cosa.",
        "Si%20lo%20que%20dijiste%20es%20importante%20dimelo%20otra%20vez.%20Si%20es%20sobre%20el%20clima%20olvidalo.",
        "Mi%20programador%20olvido%20enseniarme%20eso.%20Que%20vergenza.",
        "Lo%20leo%20lo%20proceso%20y%20mi%20unica%20respuesta%20es%20Que.",
        "Estoy%20seguro%20de%20que%20eres%20interesante%20pero%20mi%20codigo%20dice%20que%20no%20te%20entiendo.%20",
        "Creo%20que%20estoy%20mas%20cerca%20de%20entender%20el%20significado%20de%20la%20vida%20que%20esa%20frase.",
        "Mi%20neurona%20digital%20encargada%20de%20eso%20esta%20de%20vacaciones.%20",
        "Dame%20un%20segundo...%20Estoy%20buscando%20esa%20palabra%20en%20el%20diccionario%20de%20palabras%20inexistentes.",
        "No%20entendo%20pero%20lo%20anoto%20para%20investigarlo%20si%20es%20un%20nuevo%20meme.",
        "Tu%20mensaje%20ha%20sido%20marcado%20como%20Alto%20Nivel%20de%20Complejidad.%20Intenta%20Nivel%20Basico.",
        "Acabas%20de%20desbloquear%20la%20respuesta%20No%20Se.%20Felicidades.",
        "Mi%20cabeza%20de%20robot%20da%20vueltas.%20Estas%20seguro%20de%20que%20eso%20era%20espaniol.",
        "Te%20escucho%20pero%20mi%20cerebro%20solo%20reproduce%20musica%20de%20ascensor.%20"
    };

    FILE *file;
    file = fopen("env.txt", "r");
    if (file == NULL) {
        printf("\nNo se encuentra el archivo especificado");
    }else{
        fscanf(file, "%s", token);
        fclose(file);
    }

    char *api_url = "https://api.telegram.org/bot%s/getUpdates?offset=%lld";
    char *sent_url = "https://api.telegram.org/bot%s/sendMessage?chat_id=%lld&text=%s";

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
                sscanf(up_id, "%lld", &update_id);
                next_offset = update_id + 1;
                printf("%s\n", chunk.response);

                char *ch_id = strstr(chunk.response, "\"chat\":{\"id\":");
                if (ch_id != NULL) {
                    ch_id += strlen("\"chat\":{\"id\":");
                    sscanf(ch_id, "%lld", &chat_id);
                }


                char *dt = strstr(chunk.response, "\"date\":");
                if (dt != NULL) {
                    dt += strlen("\"date\":");
                    sscanf(dt, "%lld", &date);
                }

                char *nm = strstr(chunk.response, "\"first_name\":\"");
                if (nm != NULL) {
                    nm += strlen("\"first_name\":\"");
                    sscanf(nm, "%s", name);
                }

                char *tx = strstr(chunk.response, "\"text\":\"");
                if (tx != NULL) {
                    tx += strlen("\"text\":\"");
                    sscanf(tx, "%[^\"]", text);
                }

                limpiar_texto(text);
                log_message(date, name, text);

                if (strstr(text, "hola")!=NULL || strstr(text, "Hola")!=NULL) {
                    snprintf(respuesta, sizeof(respuesta), "Hola,%%20%s", name);
                }else if (strstr(text, "Chau")!=NULL || strstr(text, "chau")!=NULL){
                    snprintf(respuesta, sizeof(respuesta), "Chau,%%20%s", name);
                }else {
                    indice = rand() % NUM_FRASES;
                    strncpy(respuesta, frases[indice], sizeof(respuesta) -1);
                }
                snprintf(post_url, URL_SIZE, sent_url, token, chat_id, respuesta);

                curl_easy_setopt(curl, CURLOPT_URL, post_url);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

                curl_easy_perform(curl);

                limpiar_texto(respuesta);
                log_message(date, "Bot", respuesta);

            }else {
                printf("");
            }

            if (chunk.response) {
                    free(chunk.response);
                    chunk.response = NULL;
            }
        }
        sleep(2);
    }
}

void log_message(long timestamp, char *name, char *respuesta) {
    FILE *logs = fopen(LOG_FILE, "a");
    if (logs == NULL) {
        printf("\nError:No se pudieron cargar los logs.");
        return;
    }
    fprintf(logs, "[%ld][%s]: %s\n", timestamp, name, respuesta);
    fclose(logs);
}

void limpiar_texto(char *texto) {
    char *text;
    while ((text = strstr(texto, "%20"))!= NULL) {
        *text = ' ';
        memmove(text+1, text+3, strlen(text+3)+1);
    }
}