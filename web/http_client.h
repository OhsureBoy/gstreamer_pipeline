#include "cJSON.h"
#include <curl/curl.h>


struct device_info
{
  char userName[50];
  char serverIp[50];
  char serverPort[50];
  char resolution[50];
  char eventCode[50];
  char frameRate[50];
  char bitRate[50];
  char subStreamCodec[50];
  char mainStreamCodec[50];
  char audioPath[50];
};

struct memory
{
  char *response;
  size_t size;
};

static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp);
int http_get_device_info(char *get_token, char* device_key);
void http_get_app_version(char* get_token, char* app_title, int* app_version);
void http_get_app_update_file(char* get_token, char* app_title);
void json_pase_int_data(int* data, char* field, cJSON *root);
void json_pase_string_data(char* data, char* field, cJSON *root);
void http_get_token(char* device_key,char* token);
void Eliminate(char *str, char ch);
void send_device_status(char *get_token, char* status);
static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
void free_html_context_data(struct memory *p);


void write_device_profile(char* file_name ,char *text);
void read_device_profile(char* file_name, void* _device);