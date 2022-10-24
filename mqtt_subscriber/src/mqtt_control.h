#include <mosquitto.h>

#include "log_util.h"


int setup_mqtt(struct mosquitto **mosq, char *address, int port, char *username,
                char *password, char *ca_file, int use_tls);
int subscribe_to_topics(struct mosquitto *mosq, struct topic_node *head);
int open_message_file();
void close_message_file();