#include <curl/curl.h>
#include <time.h>

#include "linked_list.h"

struct sender{
    int secure_conn;
    char email[50];
    char smtp_server[50];
    int smtp_port;
    int credentials_enabled;
    char username[50];
    char password[30];
};

int send_mail(struct sender *sender, char *receivers, char *topic, char *argument, 
              char *expected_value, enum operator operator);
