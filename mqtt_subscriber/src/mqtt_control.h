#include <mosquitto.h>
#include <syslog.h>
#include "uci_util.h"



int setup_mqtt(struct mosquitto **mosq, char *address, int port, char *username,
                char *password);
int subscribe_to_topics(struct mosquitto *mosq, struct topic_node *head);
int open_message_file();
void close_message_file();