#include <uci.h>
#include <stdio.h>
#include "mail_util.h"


int uci_setup(struct uci_context **context,
            struct uci_package **package, char * config_name);
void uci_parse(struct uci_context *context, struct uci_package *package, 
                struct topic_node **head, struct event_node **event_head);