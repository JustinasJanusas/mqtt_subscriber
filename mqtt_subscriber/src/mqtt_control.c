#include "mqtt_control.h"

static int connection_flag = 0;
static int subscribe_flag = 0;

int is_connected()
{
    return connection_flag;
}

int is_not_subscribed()
{
    return subscribe_flag;
}

void set_subscribed_flag(int n)
{
    subscribe_flag = n;
}

static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    syslog(LOG_INFO, "Message received: %s", (char *) msg->payload);
    int rc = write_to_log( (char *) msg->topic, (char *) msg->payload);
    if( rc ){
        syslog(LOG_ERR, "Failed to save message to log: %d", rc);
    }
    check_for_events(msg->topic, msg->payload);
}

static void on_connect(struct mosquitto *mosq, void * pt, int rc)
{
    syslog(LOG_INFO, "MQTT_SUBSCRIBER connected to broker");
    connection_flag = 1;
    subscribe_flag = 1;
}

static void on_disconnect(struct mosquitto *mosq, void * pt, int rc)
{
    syslog(LOG_INFO, "MQTT_SUBSCRIBER lost connection to broker");
    connection_flag = 0;
}

static void mosquitto_log_callback(struct mosquitto *mosq, void *userdata, int level, 
                                const char *str)
{
    int log_type;
    switch( level ){
        case MOSQ_LOG_WARNING:
            log_type = LOG_WARNING;
            break;
        case MOSQ_LOG_ERR: 
            log_type = LOG_ERR;
            break;
        default:
            return;
    }
    syslog(LOG_INFO, "MQTT LOG: %d %s", level, str);
}

int setup_mqtt(struct mosquitto **mosq, char *address, int port, char *username,
                char *password, char *ca_file, int use_tls)
{
    int rc = 0;
    mosquitto_lib_init();
    *mosq = mosquitto_new(NULL, true, NULL);
    if( !*mosq ){
        syslog(LOG_ERR, "Failed to create new mosquitto instance");
        goto error;
    }
    mosquitto_log_callback_set(*mosq, mosquitto_log_callback);
    mosquitto_message_callback_set(*mosq, on_message);
    mosquitto_disconnect_callback_set(*mosq, on_disconnect);
    mosquitto_connect_callback_set(*mosq, on_connect);
    if( username ){
        rc = mosquitto_username_pw_set(*mosq, username, password);
        if(rc){
            goto error_destroy;
        }
    }
    if( use_tls ){
        rc = mosquitto_tls_set(*mosq, ca_file, NULL, NULL, NULL, NULL);
        if( rc ){
            syslog(LOG_ERR, "Failed to set TLS: %d", rc);
            goto error_destroy;
        }
    }
    rc = mosquitto_connect(*mosq, address, port, 30);
    if( rc ){
        syslog(LOG_ERR, "Failed to connect to broker: %d", rc);
        goto error_destroy;
    }
    return 0;
    error_destroy:
        mosquitto_destroy(*mosq);
    error:
        mosquitto_lib_cleanup();
        return rc;
}

int subscribe_to_topics(struct mosquitto *mosq, struct topic_node *head)
{
    if( head == NULL ){
        return 1;
    }
    int rc = 0;
    struct topic_node *tmp = head;
    while( tmp != NULL ){
        rc = mosquitto_subscribe(mosq, NULL, tmp->name, tmp->qos);
        if(rc){
            syslog(LOG_ERR, "Failed to subscribe to \"%s\" topic: %d", 
                    tmp->name, rc);
            return rc;
        }
        syslog(LOG_INFO, "Subscribed to \"%s\" topic", 
                    tmp->name);
        tmp = tmp->next;
    }
    return 0;
}