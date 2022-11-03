#include "mail_util.h"


static char payload_text[500];

struct upload_status {
  size_t bytes_read;
};

static void build_message(char *topic, char *argument, char *expected_value,
                    enum operator operator, char *sender, char *receiver)
{
   
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(payload_text, "Date: %d-%02d-%02d %02d:%02d:%02d\r\n"
          "To: %s\r\n"
          "From: %s\r\n"
          "Cc: \r\n"
          "Subject: \"%s\" topic event\r\n"
          "\r\n" 
          "Event: %s %s %s.\r\n"
          "\r\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                  tm.tm_min, tm.tm_sec, receiver, sender, topic, argument, 
                                operator_strings[operator], expected_value);
}

static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;
 
  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }
 
  data = &payload_text[upload_ctx->bytes_read];
 
  if(data) {
    size_t len = strlen(data);
    if(room < len){
      len = room;
    }
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;
 
    return len;
  }
 
  return 0;
}

int send_mail(struct sender *sender, char* receiver, char *topic, char *argument, 
              char *expected_value, enum operator operator)
{
  build_message(topic, argument, expected_value, operator, sender->email, receiver);
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };
    char server[60] = "smtp://";
  curl = curl_easy_init();
  if(curl) {
    if(sender->credentials_enabled){
        curl_easy_setopt(curl, CURLOPT_USERNAME, sender->username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, sender->password);
    }
    strcat(server, sender->smtp_server);

    curl_easy_setopt(curl, CURLOPT_URL, server);
    curl_easy_setopt(curl, CURLOPT_PORT, sender->smtp_port);

    if( sender->secure_conn ){
      curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
      curl_easy_setopt(curl, CURLOPT_CAPATH, "/etc/certificates/");
    }

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, sender->email);

    recipients = curl_slist_append(recipients, receiver);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
 
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 

    res = curl_easy_perform(curl);
 
    if(res != CURLE_OK){
      syslog(LOG_ERR, "curl_easy_perform() failed: %s %d\n",
              curl_easy_strerror(res), res);
    }
    curl_slist_free_all(recipients);
 
    curl_easy_cleanup(curl);
  }
 
  return (int)res;
}