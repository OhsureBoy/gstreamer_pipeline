#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "http_client.h"

// 파일 입출력
static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{

  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

/* Used by some code below as an example datatype. */
static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)userp;

  char *ptr = realloc(mem->response, mem->size + realsize + 1);
  if (ptr == NULL)
    return 0; /* out of memory! */

  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;
  printf("callback :%s", (mem->response));
  return realsize;
}

void http_get_token(char *device_key, char *token)
{
  CURL *curl;
  CURLcode res;

  char post_body[128] = {};
  memset(post_body,0,128);
  snprintf(post_body, sizeof(post_body), "{\"deviceKey\":\"%s\"}", device_key);

  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);

  /* get a curl handle */
  curl = curl_easy_init();
  if (curl)
  {

    struct curl_slist *list = NULL;
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_URL, "http://bond.tlivecaster.com:23891/devices/access");
    list = curl_slist_append(list, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);

    struct memory chunk = {0};

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    res = curl_easy_perform(curl);
    curl_slist_free_all(list);

    /* Check for errors */
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    else
    {
      cJSON *root = cJSON_Parse(chunk.response);
      if (root == NULL)
      {
        printf("cJSON_Parse() fail! \n");
      }
      else
      {
        cJSON *get_token = cJSON_GetObjectItem(root, "token");
        if (get_token == NULL)
        {
          snprintf(token, sizeof(token), "NULL");
        }
        else
        {
          json_pase_string_data(token, "token", root);
        }
      }

      cJSON_Delete(root);
    }

    free_html_context_data(&chunk);
    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();
}

void json_pase_string_data(char *data, char *field, cJSON *root)
{
  cJSON *get_data = NULL;

  get_data = cJSON_GetObjectItem(root, field);
  if (get_data == NULL)
  {
    snprintf(data, sizeof(data), "null");
    printf("\n\nNULL\n\n");
  }
  else
  {
    char *temp = cJSON_Print(get_data);
    Eliminate(temp, '"');
    snprintf(data, strlen(temp) + 1, "%s", temp);
  }
}

void json_pase_int_data(int *data, char *field, cJSON *root)
{

  cJSON *get_data = NULL;

  get_data = cJSON_GetObjectItem(root, field);

  if (get_data == NULL)
  {
    printf("\n\nNULL\n\n");
  }
  else
  {
    *data = atoi(cJSON_Print(get_data));
  }
}

int http_get_device_info(char *get_token, char *device_key)
{
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;

  // char *token = malloc(sizeof(char*) * 9);

  char token[256] = {};
  snprintf(token, sizeof(token), "Authorization:%s", get_token);

  char url[256] = {};
  snprintf(url, sizeof(url), "http://bond.tlivecaster.com:23891/devices/%s/detail", device_key);
  printf("%s\n", url);
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (curl)
  {

    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* Now specify the GET data */
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    headers = curl_slist_append(headers, "Content-Type: application/json");

    headers = curl_slist_append(headers, token);

    struct memory chunk = {0};

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    else
    {
      cJSON *root = cJSON_Parse(chunk.response);
      if (root == NULL)
      {
        printf("cJSON_Parse() fail! \n");
        return 0;
      }
      else
      {
        write_device_profile("profile.cfg", chunk.response);

        // json_pase_string_data(device->userName, "userName", root);
        // json_pase_string_data(device->frameRate, "frameRate", root);
        // json_pase_string_data(device->serverPort, "serverPort", root);
        // json_pase_string_data(device->resolution, "resolution", root);
        // json_pase_string_data(device->audioPath, "audioPath", root);
        // json_pase_string_data(device->eventCode, "eventCode", root);
        // json_pase_string_data(device->bitRate, "bitRate", root);
        // json_pase_string_data(device->subStreamCodec, "subStreamCodec", root);
        // json_pase_string_data(device->mainStreamCodec, "mainStreamCodec", root);
        // json_pase_string_data(device->serverIp, "serverIp", root);
      }
      cJSON_Delete(root);
    }
    /* always cleanup */
    curl_easy_cleanup(curl);

    free_html_context_data(&chunk);
  }

  curl_slist_free_all(headers);
  curl_global_cleanup();

  return 1;
}

// int main(void)
// {
//   char real_token[518] = {};

//   http_get_token("test","test1234",real_token);

//   printf("\n\n\n real_token : %s \n\n", real_token);

//   struct device_info *p1;
//   p1 = http_get_device_info(real_token, "bbb");
//   free(p1);

//   printf("\n\n\n http_get_deivce_info is done \n\n");

//   send_device_status(real_token, "offline");
//   printf("\n\n\n send_device_status is done \n\n");

//   http_get_app_update_file(real_token,"tlc-bond");
//   printf("\n\n\n http_get_app_update_file is done \n\n");

//   char app_version[518] = {};
//   http_get_app_version(real_token,"tlc-bond",app_version);
//   printf("\n\n app_version : %s \n\n",app_version);
// }

void Eliminate(char *str, char ch)
{
  for (; *str != '\0'; str++) //종료 문자를 만날 때까지 반복
  {
    if (*str == ch) // ch와 같은 문자일 때
    {
      strcpy(str, str + 1);
      str--;
    }
  }
}

void send_device_status(char *get_token, char *status)
{
  CURL *curl;
  CURLcode res;

  struct curl_slist *list = NULL;
  char post_body[256] = {};

  /* In windows, this will init the winsock stuff */
  curl_global_init(CURL_GLOBAL_ALL);

  snprintf(post_body, sizeof(post_body), "{\"token\":\"%s\", \"status\":\"%s\"}", get_token, status);

  /* get a curl handle */
  curl = curl_easy_init();
  if (curl)
  {

    list = curl_slist_append(list, "Content-Type: application/json"); // content-type 정의 내용 list에 저장
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);                 // content-type 설정

    curl_easy_setopt(curl, CURLOPT_URL, "http://bond.tlivecaster.com:23891/devices/stream/status");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);

    struct memory chunk = {0};

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    else
    {
      cJSON *root = cJSON_Parse(chunk.response);

      cJSON *get_status = cJSON_GetObjectItem(root, "httpStatus");
      if (get_status == NULL)
      {
        printf("failed send device \n");
      }
      else
      {
        printf("send deivce status : %s \n", cJSON_Print(get_status));
      }

      cJSON_Delete(root);
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    free_html_context_data(&chunk);
  }
  curl_slist_free_all(list);
  curl_global_cleanup();
}

void http_get_app_version(char *get_token, char *app_title, int *app_version)
{
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;

  char token[256] = {};
  snprintf(token, sizeof(token), "Authorization:%s", get_token);

  char url[256] = {};
  snprintf(url, sizeof(url), "http://bond.tlivecaster.com:23891/devices/%s/version", app_title);

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* Now specify the GET data */
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, token);

    struct memory chunk = {0};

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    else
    {
      printf("\n%s\n",chunk.response);
      cJSON *root = cJSON_Parse(chunk.response);
      if (root == NULL)
      {
        printf("cJSON_Parse() fail! \n");
        app_version = -1;
      }
      else
      {
        cJSON *appVersion = cJSON_GetObjectItem(root, "appVersion");
        if (appVersion == NULL)
        {
          printf("\n http_get_app_version NULL!!! \n");
        }
         else
        {
          json_pase_int_data(app_version, "versionCode", root);
        }
      }

      cJSON_Delete(root);
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
    free_html_context_data(&chunk);
  }

  curl_slist_free_all(headers);
  curl_global_cleanup();
}

void http_get_app_update_file(char *get_token, char *app_title)
{
  CURL *curl;
  CURLcode res;
  FILE *fp;
  struct curl_slist *headers = NULL;
  char outfilename[FILENAME_MAX] = "./update/TLC_BOND.zip";

  char token[256] = {};
  snprintf(token, sizeof(token), "Authorization:%s", get_token);

  char url[256] = {};
  snprintf(url, sizeof(url), "http://bond.tlivecaster.com:23891/devices/%s/update", app_title);

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl)
  {
    fp = fopen(outfilename, "w");
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* Now specify the GET data */
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, token);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  curl_slist_free_all(headers);
  curl_global_cleanup();
  fclose(fp);
  free(fp);
}

void free_html_context_data(struct memory *p)
{
  p ? (free(p->response), p->response = NULL, p->size = 0) : 1;
}

void write_device_profile(char *file_name, char *text)
{
  FILE *fp = fopen(file_name, "w"); // hello.txt 파일을 쓰기 모드(w)로 열기.
                                    // 파일 포인터를 반환
  fputs(text, fp);                  // 파일에 문자열 저장

  fclose(fp); // 파일 포인터 닫기
}

void read_device_profile(char *file_name, void *_device)
{
  struct device_info *device = (struct device_info *)_device;
  char read_data[1024] = {};
  FILE *fp = fopen(file_name, "r"); // hello.txt 파일을 읽기 모드로 열기.
                                    // 파일 포인터를 반환

  fgets(read_data, sizeof(read_data), fp); // hello.txt에서 문자열을 읽음

  cJSON *root = cJSON_Parse(read_data);
  if (root == NULL)
  {
    printf("cJSON_Parse() fail! \n");
    return 0;
  }
  else
  {
    json_pase_string_data(device->userName, "userName", root);
    json_pase_string_data(device->frameRate, "frameRate", root);
    json_pase_string_data(device->serverPort, "serverPort", root);
    json_pase_string_data(device->resolution, "resolution", root);
    json_pase_string_data(device->audioPath, "audioPath", root);
    json_pase_string_data(device->eventCode, "eventCode", root);
    json_pase_string_data(device->bitRate, "bitRate", root);
    json_pase_string_data(device->subStreamCodec, "subStreamCodec", root);
    json_pase_string_data(device->mainStreamCodec, "mainStreamCodec", root);
    json_pase_string_data(device->serverIp, "serverIp", root);
  }
  cJSON_Delete(root);

  fclose(fp); // 파일 포인터 닫기
}