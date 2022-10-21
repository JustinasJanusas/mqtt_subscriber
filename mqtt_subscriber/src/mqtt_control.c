#include "mqtt_control.h"


#define CA_FILE "/etc/certificates/cert.cert.pem"
#define CLIENT_CERT "/etc/certificates/mqtt_subscriber.cert.pem"
#define CLIENT_KEY "/etc/certificates/mqtt_subscriber.key.pem"

static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    syslog(LOG_INFO, "Message received: %s", (char *) msg->payload);
    fprintf(stdout, "%s\n", (char *) msg->payload);
    fflush(stdout);

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
                char *password)
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
    if( strlen(username) > 0 && strlen(password) > 0 ){
        rc = mosquitto_username_pw_set(*mosq, username, password);
        if(rc){
            syslog(LOG_DEBUG, "Failed to set the authentication data");
            goto error_destroy;
        }
    }
    rc = mosquitto_tls_set(*mosq, CA_FILE, NULL, CLIENT_CERT, CLIENT_KEY, NULL);
    if( rc ){
        syslog(LOG_ERR, "Failed to set TLS: %d", rc);
        goto error_destroy;
    }
    rc = mosquitto_connect(*mosq, address, port, 60);
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
    if( head == NULL )
        return 1;
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