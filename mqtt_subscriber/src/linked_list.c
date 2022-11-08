#include "linked_list.h"

char *operator_strings[] = { "==", "!=", ">", "<",  ">=", "<="};

struct topic_node* create_topic_node(char *name, int qos)
{
    struct topic_node *node;
    node = (struct topic_node*) malloc(sizeof(struct topic_node));
    node->next = NULL;
    strncpy(node->name, name, 39);
    node->name[39] = '\0';
    node->qos = qos;
    return node;
}

struct event_node* create_event_node(char *topic, char *parameter, int type,
                                    int operator, char *expected_value,
                                    char *email, char *receiver)
{
    struct event_node *node;
    node = (struct event_node*) malloc(sizeof(struct event_node));
    node->next = NULL;
    strncpy(node->topic, topic, 39);
    node->topic[39] = '\0';
    strncpy(node->parameter, parameter, 19);
    node->parameter[19] = '\0';
    node->type = type;
    node->operator = operator;
    strncpy(node->expected_value, expected_value, 999);
    node->expected_value[999] = '\0';
    strncpy(node->email, email, 39);
    node->email[39] = '\0';
    strncpy(node->receiver, receiver, 39);
    node->receiver[39] = '\0';
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
    struct topic_node *to_delete;
    while (*head != NULL) {
        to_delete = *head;
        *head = (*head)->next;
        free(to_delete);
    }
}
void free_all_event_nodes(struct event_node **head)
{
    struct event_node *to_delete;
    while (*head != NULL) {
        to_delete = *head;
        *head = (*head)->next;
        free(to_delete);
    }
}