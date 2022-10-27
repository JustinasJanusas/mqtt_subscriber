#include <curl/curl.h>

#include "linked_list.h"

int send_mail(char *sender, char *receivers, char *topic, char *argument, 
              char *expected_value, enum operator operator);
