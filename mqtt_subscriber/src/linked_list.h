#include <syslog.h>
#include <stdlib.h>
#include <string.h>

struct topic_node {
    char name[40];
    int qos;
    struct topic_node *next; 
};
struct event_node{
    char topic[40];
    char parameter[20];
    int type;
    char operator[2];
    char expected_value[20];
    char email[40];
    char receivers[200];
    struct event_node *next;
};

struct event_node* create_event_node(char *topic, char *parameter, int type,
                                    char *operator, char *expected_value,
                                    char *email, char *receivers);
struct topic_node* create_topic_node(char *name, int qos);
void add_new_topic_node(struct topic_node **head, struct topic_node *new_node);
void add_new_event_node(struct event_node **head, struct event_node *new_node);
void free_all_topic_nodes(struct topic_node **head);
void free_all_event_nodes(struct event_node **head);