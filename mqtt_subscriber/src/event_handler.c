#include "event_handler.h"

#define EMAIL_CONFIG "user_groups"

static struct event_node **head;
static struct uci_context *context;
static struct uci_package *package;



static int check_char(const char *var, enum operator operator, char *val)
{
    switch (operator)
    {
        case eq:
            return strcmp(var, val) == 0;
        case ne:
            return strcmp(var, val) != 0;
        default:
            return 0;
    }
}


static int check_int(int var, enum operator operator, int val)
{
    switch (operator)
    {
        case eq:
            return var == val;
        case ne:
            return var != val;
        case gt:
            return var > val;
        case lt:
            return var < val;
        case ge:
            return var >= val;
        case le:
            return var <= val;
        default:
            return 0;
    }
}

int setup_email_config()
{
    int rc = 0;
    rc = uci_setup(&context, &package, EMAIL_CONFIG);
    if( rc ){
        syslog(LOG_ERR, "Failed to setup email config");
        if( context ){
            uci_free_context(context);
        }
    }
    return rc;
}

void free_email_config()
{
    uci_free_context(context);
}

void set_event_head(struct event_node **event_head)
{
    head = event_head;
}

void check_for_events(char *topic, char *message)
{
    struct event_node *tmp;
    tmp = *head;
    json_object *jobj;
    jobj = json_tokener_parse(message);
    json_object *j;
    int check = 0;
    int rc = 0;

    while ( tmp != NULL ){
        if( !strcmp(tmp->topic, topic) ){
            j = json_object_object_get(jobj, tmp->parameter);
            if(j){
                if(tmp->type && tmp->operator < 2){
                    //handle char
                    check = check_char(json_object_get_string(j), (enum operator)tmp->operator,
                                tmp->expected_value);
                }
                else if(!tmp->type){
                    //handle int
                    check = check_int(json_object_get_int(j), (enum operator)tmp->operator,
                                atoi(tmp->expected_value));
                }
                if(check){
                    struct sender *sender = (struct sender *)malloc(sizeof(struct sender));
                    
                    int rc = 0;
                    rc = uci_get_sender_info(context, package, &sender, tmp->email);
                    if( rc ){
                        syslog(LOG_ERR, "Didn't find email params in config");
                        goto free_sender_pointer;
                    }
                    rc = send_mail(sender, tmp->receiver, tmp->topic, 
                                    tmp->parameter, tmp->expected_value, tmp->operator);
                        
                    free_sender_pointer:
                        free(sender);
                    
                        if( rc ){
                            syslog(LOG_ERR, "Event happened but failed to send an email");
                        }
                        else{
                            syslog(LOG_INFO, "Event happened and an email has been sent");
                        }
                }
            }
        }
        tmp = tmp->next;
    }
    json_object_put(jobj);
    json_object_put(j);
}