
#include "uci_util.h"
#include <string.h>

int uci_setup(struct uci_context **context,
            struct uci_package **package, char * config_name)
{
    int rc = 0;
    *context = uci_alloc_context();
    if( !(*context) ){
		syslog(LOG_ERR, "Failed to create uci context");
        return 1;
	}
    rc = uci_load(*context, config_name, package);
    if( rc ){
        syslog(LOG_ERR, "Failed to load config file: %d", rc);
        return rc;
    }
    return 0;
}

void uci_parse(struct uci_context *context, struct uci_package *package, 
                struct topic_node **head, struct event_node **event_head)
{
    struct uci_element *i, *j;
    struct topic_node *tmp;
    struct event_node *event_tmp;

    //topic values
    char name[40];
    int qos;

    //event values
    char topic[40];
    char parameter[20];
    int type;
    int operator;
    char expected_value[20];
    char email[40];
    char receiver[40];
    uci_foreach_element(&package->sections, i){
        struct uci_section *section = uci_to_section(i);
        char *section_type = section->type;
        if( !strcmp(section_type, "topic") ){
            strncpy(name, uci_lookup_option(context, section, "topic")->v.string, 40);
            qos = atoi(uci_lookup_option(context, section, "qos")->v.string);
            if( name && ( qos || qos == 0 ) ){
                tmp = create_topic_node(name, qos);
                add_new_topic_node(head, tmp);
            }
        }
        else if( !strcmp(section_type, "event")){
            strncpy(topic, 
                    uci_lookup_option(context, section, "topic")->v.string, 40);
            strncpy(parameter, 
                    uci_lookup_option(context, section, "parameter")->v.string, 20);
            type = atoi(uci_lookup_option(context, section, "type")->v.string);
            operator = atoi(uci_lookup_option(context, section, "operator")->v.string);
            strncpy(expected_value, 
                    uci_lookup_option(context, section, "expected_value")->v.string, 
                    20);
            strncpy(email, uci_lookup_option(context, section, "email")->v.string,
                    40);
            strncpy(receiver, uci_lookup_option(context, section, "receiver")->v.string,
                    40);
            if( topic && parameter && ( type || type == 0 )  && 
                (operator || operator == 0 ) &&
                expected_value && email && receiver){
                event_tmp = create_event_node(topic, parameter, type, operator,
                                            expected_value, email, receiver);
                add_new_event_node(event_head, event_tmp);
            }
        }
    }
}
