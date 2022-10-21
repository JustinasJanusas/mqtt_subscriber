#include "linked_list.h"

struct topic_node* create_topic_node(char *name, int qos)
{
    struct topic_node *node;
    node = (struct topic_node*) malloc(sizeof(struct topic_node));
    node->next = NULL;
    strncpy(node->name, name, 40);
    node->qos = qos;
    return node;
}
struct event_node* create_event_node(char *topic, char *parameter, int type,
                                    char *operator, char *expected_value,
                                    char *email, char *receivers)
{
    struct event_node *node;
    node = (struct event_node*) malloc(sizeof(struct event_node));
    node->next = NULL;
    strncpy(node->topic, topic, 40);
    strncpy(node->parameter, parameter, 20);
    node->type = type;
    strncpy(node->operator, operator, 2);
    strncpy(node->expected_value, expected_value, 20);
    strncpy(node->email, email, 40);
    strncpy(node->receivers, receivers, 200);
    return node;
}

void add_new_topic_node(struct topic_node **head, struct topic_node *new_node)
{
    if( !(*head) ){
        *head = new_node;
        return;
    }
    struct topic_node *tmp;
    tmp = *head;
    while( tmp->next != NULL ){
        tmp = tmp->next;
    }
    tmp->next = new_node;
}

void add_new_event_node(struct event_node **head, struct event_node *new_node)
{
    if( !(*head) ){
        *head = new_node;
        return;
    }
    struct event_node *tmp;
    tmp = *head;
    while( tmp->next != NULL ){
        tmp = tmp->next;
    }
    tmp->next = new_node;
}

void free_all_topic_nodes(struct topic_node **head)
{
    struct topic_node *tmp;
    tmp = *head;
    struct topic_node *delete;
    while( tmp->next != NULL){
        delete = tmp;
        tmp = tmp->next;
        free(delete);
    }
    if( tmp ){
        free(tmp);
    }
}
void free_all_event_nodes(struct event_node **head)
{
    struct event_node *tmp;
    tmp = *head;
    struct event_node *delete;
    while( tmp->next != NULL){
        delete = tmp;
        tmp = tmp->next;
        free(delete);
    }
    if( tmp ){
        free(tmp);
    }
}