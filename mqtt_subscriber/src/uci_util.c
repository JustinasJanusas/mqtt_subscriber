
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

static char *uci_list_to_string(struct uci_list *list)
{
    size_t max_size = 100;
    size_t current_size = 1;    // This includes the null terminator
    char *result = calloc(max_size, sizeof(char));
    struct uci_element *i;

    uci_foreach_element(list, i)
    {
        // +2 for ", " after each list element
        size_t element_length = strlen(i->name) + 2;
        char name[element_length + 2];
        strcpy(name, i->name);

        if (current_size + element_length > max_size)
        {
            max_size = max_size * 2 + element_length;
            result = realloc(result, sizeof(char) * max_size);
        }

        strcat(result, strcat(name, ", "));
        current_size += element_length;
    }

    // Remove trailing ", "
    if (current_size >= 3)
        result[current_size - 3] = '\0';
    else
        result[0] = '\0';

    return result;
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
    char operator[2];
    char expected_value[20];
    char email[40];
    char receivers[200];

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
            strncpy(operator, 
                    uci_lookup_option(context, section, "operator")->v.string, 2);
            strncpy(expected_value, 
                    uci_lookup_option(context, section, "expected_value")->v.string, 
                    20);
            strncpy(email, uci_lookup_option(context, section, "email")->v.string,
                    40);
            strncpy(receivers, uci_lookup_option(context, section, "receivers")->v.string,
                    200);
            if( topic && parameter && ( type || type == 0 )  && operator &&
                expected_value && email && receivers){
                event_tmp = create_event_node(topic, parameter, type, operator,
                                            expected_value, email, receivers);
                add_new_event_node(event_head, event_tmp);
            }
        }
    }
}
