#include "mail_util.h"


static char payload_text[500];

struct upload_status {
  size_t bytes_read;
};

static void build_message(char *topic, char *argument, char *expected_value,
                    enum operator operator, char *sender, char *receiver)
{
  

  sprintf(payload_text, "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n"
          "To: %s\r\n"
          "From: %s\r\n"
          "Cc: \r\n"
          "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
          "rfcpedant.example.org>\r\n"
          "Subject: \"%s\" topic event\r\n"
          "\r\n" /* empty line to divide headers from body, see RFC5322 */
          "Event: %s %s %s.\r\n"
          "\r\n", sender, receiver, topic, argument, 
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
  FILE *f = fopen("/tmp/why.txt", "w");
  fprintf(f, payload_text);
  fclose(f);
  if(payload_text) {
    size_t len = strlen(payload_text);
    
    if(room < len)
      len = room;
    memcpy(ptr, payload_text, len);
    upload_ctx->bytes_read += len;

    return len;
  }
  return 0;
}


int send_mail(char *sender, char* receiver, char *topic, char *argument, 
              char *expected_value, enum operator operator){
  CURL *curl;
  CURLcode res = CURLE_COULDNT_CONNECT;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };

  curl = curl_easy_init();
  if(curl) {
    /* This is the URL for your mailserver */
    curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.freesmtpservers.com");
    curl_easy_setopt(curl, CURLOPT_PORT, 25);
    /* Note that this option is not strictly required, omitting it will result
     * in libcurl sending the MAIL FROM command with empty sender data. All
     * autoresponses should have an empty reverse-path, and should be directed
     * to the address in the reverse-path which triggered them. Otherwise,
     * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
     * details.
     */
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, sender);
    /* Add two recipients, in this particular case they correspond to the
     * To: and Cc: addressees in the header, but they could be any kind of
     * recipient. */
    char error[500];
    recipients = curl_slist_append(recipients, receiver);
    //recipients = curl_slist_append(recipients, CC_ADDR);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt ( curl, CURLOPT_ERRORBUFFER, error );
    /* We are using a callback function to specify the payload (the headers and
     * body of the message). You could just use the CURLOPT_READDATA option to
     * specify a FILE pointer to read from. */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    build_message(topic, argument, expected_value, operator, sender, receiver);
    curl_easy_setopt(curl, CURLOPT_READDATA, &payload_text);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    /* Send the message */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      syslog(LOG_ERR, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    /* Free the list of recipients */
    curl_slist_free_all(recipients);
    
    /* curl will not send the QUIT command until you call cleanup, so you
     * should be able to re-use this connection for additional messages
     * (setting CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and
     * calling curl_easy_perform() again. It may not be a good idea to keep
     * the connection open for a very long time though (more than a few
     * minutes may result in the server timing out the connection), and you do
     * want to clean up in the end.
     */
    curl_easy_cleanup(curl);
  }
  return (int)res;
}