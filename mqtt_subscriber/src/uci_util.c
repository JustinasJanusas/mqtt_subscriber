
#include "uci_util.h"
#include <string.h>


static void handle_strings(struct uci_context *context, struct uci_section *section,
                            char *name, int len, char *var)
{
    if(uci_lookup_option(context, section, name)){
        strncpy(var, uci_lookup_option(context, section, name)->v.string, len-1);
        var[len-1] = '\0';
    }
} 


static void handle_int(struct uci_context *context, struct uci_section *section,
                            char *name, int *var)
{
    if(uci_lookup_option(context, section, name)){
        *var = atoi(uci_lookup_option(context, section, name)->v.string);
    }
    else{
        *var = -1;
    }
} 


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
    char expected_value[1000];
    char email[40];
    char receiver[40];
    char temp[5];
    uci_foreach_element(&package->sections, i){
        struct uci_section *section = uci_to_section(i);
        char *section_type = section->type;
        if( !strcmp(section_type, "topic") ){
            handle_strings(context, section, "topic", 40, name);
            handle_int(context, section, "qos", &qos);
            if( name && (qos >= 0 && qos < 3 ) ){
                tmp = create_topic_node(name, qos);
                add_new_topic_node(head, tmp);
            }
        }
        else if( !strcmp(section_type, "event")){
            handle_strings(context, section, "topic", 40, topic);
            handle_strings(context, section, "parameter", 10, parameter);
            handle_int(context, section, "type", &type);
            handle_int(context, section, "operator", &operator);
            handle_strings(context, section, "expected_value", 1000, expected_value);
            handle_strings(context, section, "email", 40, email);
            handle_strings(context, section, "receiver", 20, receiver);
            if( topic && parameter && ( type >= 0 && type < 2)  && 
                (operator >= 0 && operator < 6) &&
                expected_value && email && receiver){
                event_tmp = create_event_node(topic, parameter, type, operator,
                                            expected_value, email, receiver);
                add_new_event_node(event_head, event_tmp);
            }
        }
    }
}

static void parse_sender_config(struct uci_context *context, struct uci_package *package,
                                struct sender **sender, char *name)
{
    struct uci_element *i, *j;
    uci_foreach_element(&package->sections, i){
        struct uci_section *section = uci_to_section(i);
        char *section_type = section->type;
        if( !strcmp(section_type, "email") ){
            if( !strcmp(name, uci_lookup_option(context, section, "name")->v.string)){
                strncpy((*sender)->email, uci_lookup_option(context, section,
                                                                "senderemail")->v.string, 49);
                (*sender)->email[49] = '\0';
                strncpy((*sender)->smtp_server, uci_lookup_option(context, section,
                                                                "smtp_ip")->v.string, 49);
                (*sender)->smtp_server[49] = '\0'; 
                (*sender)->smtp_port = atoi(uci_lookup_option(context, section,
                                                                "smtp_port")->v.string);
                (*sender)->secure_conn = atoi(uci_lookup_option(context, section,
                                                                "secure_conn")->v.string);
                (*sender)->credentials_enabled = atoi(uci_lookup_option(context, section,
                                                                "credentials")->v.string);
                if( (*sender)->credentials_enabled )
                {
                    strncpy((*sender)->username, uci_lookup_option(context, section,
                                                                "username")->v.string, 49);
                    (*sender)->username[49] = '\0';
                    strncpy((*sender)->password, uci_lookup_option(context, section,
                                                                    "password")->v.string, 29);
                    (*sender)->password[29] = '\0';
                }
                break;
            }
        }
        
    }
}

int uci_get_sender_info(struct uci_context *context, struct uci_package *package,
                        struct sender **sender, char *name)
{
    struct uci_element *i, *j;
    uci_foreach_element(&package->sections, i){
        struct uci_section *section = uci_to_section(i);
        char *section_type = section->type;
        if( !strcmp(section_type, "email") ){
            if( !strcmp(name, uci_lookup_option(context, section, "name")->v.string)){
                strncpy((*sender)->email, uci_lookup_option(context, section,
                                                                "senderemail")->v.string, 49);
                (*sender)->email[49] = '\0';
                strncpy((*sender)->smtp_server, uci_lookup_option(context, section,
                                                                "smtp_ip")->v.string, 49);
                (*sender)->smtp_server[49] = '\0';
                (*sender)->smtp_port = atoi(uci_lookup_option(context, section,
                                                                "smtp_port")->v.string);
                (*sender)->secure_conn = atoi(uci_lookup_option(context, section,
                                                                "secure_conn")->v.string);
                (*sender)->credentials_enabled = atoi(uci_lookup_option(context, section,
                                                                "credentials")->v.string);
                if( (*sender)->credentials_enabled ){
                    strncpy((*sender)->username, uci_lookup_option(context, section,
                                                                "username")->v.string, 49);
                    (*sender)->username[49] = '\0';
                    strncpy((*sender)->password, uci_lookup_option(context, section,
                                                                    "password")->v.string, 29);
                    (*sender)->password[29] = '\0';
                }
                return 0;
            }
        }
        
    }
    return 1;
}
