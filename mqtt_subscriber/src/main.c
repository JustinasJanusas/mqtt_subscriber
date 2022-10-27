#include <signal.h>
#include <unistd.h>
#include <argp.h>

#include "mqtt_control.h"

#define CONFIG_NAME "mqtt_subscriber"
#define FILE_PATH "/tmp/subscribe.txt"

volatile sig_atomic_t deamonize = 1;

const char *argp_program_version =  "mqtt_subscriber 1.0.0";
const char *argp_program_bug_address =  "<bug-gnu-utils@gnu.org>";
static char doc[] = "iot subscriber -- subscribes to topics and reacts to events";


static struct argp_option options[] = {
    {"host",  'h', "host", 0, "Broker address" },
    {"port", 'p', "port", 0, "Port"},
	{"username", 'u', "username", 0, "Username"},
	{"password", 'P', "password", 0, "Password"},
	{"ca_file", 'c', "ca_file", 0, "CA file"}, 
	{"use_tls", 't', "use_tls", 0, "Use TLS"},
    { 0 }
};

struct arguments{
    char *host;
    int port;
	char *username;
	char *password;
	char *ca_file;
	int use_tls;
};



static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	switch ( key ){

        case 'h':
            arguments->host = arg;
            break;
        case 'p':
            arguments->port = atoi(arg);
            break;
		case 'u':
			arguments->username = arg;
			break;
		case 'P':
			arguments->password = arg;
			break;
		case 'c':
			arguments->ca_file = arg;
			break;
		case 't':
			arguments->use_tls = atoi(arg);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
			
	}
	return 0;
}

static struct argp argp = { options, parse_opt, 0, doc };

static void term_proc(int sigterm) 
{
	deamonize = 0;
}

int main(int argc, char **argv)
{
	openlog("mqtt_subscriber", LOG_PID, LOG_USER);

	//arguments
	int rc = 0;
    struct mosquitto *mosq = NULL;
	struct sigaction action;
	struct uci_context *context;
	struct uci_package *package;
	struct arguments arguments;
	struct topic_node *head = NULL;
	struct event_node *event_head = NULL;
	FILE *file;
	//set sigaction
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term_proc;
	sigaction(SIGTERM, &action, NULL);

	//parse args
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	//handle uci
	rc = uci_setup(&context, &package, CONFIG_NAME);
	if( rc ){
		syslog(LOG_ERR, "uci setup failed");
		goto end_close_log;
	}
	
	uci_parse(context, package, &head, &event_head);
	if( head == NULL )
		goto end_free_context;
	set_event_head(&event_head);
	//json
	//check_for_events(NULL, NULL);

	//connect to broker
    rc = setup_mqtt(&mosq, arguments.host, arguments.port, arguments.username,
					arguments.password, arguments.ca_file, arguments.use_tls);
    if( rc )
        goto end_free_context;
	rc = subscribe_to_topics(mosq, head);
	if( rc )
		goto end_destroy_mosquitto;
	rc = init_log();
	if( rc ){
		syslog(LOG_ERR, "Failed to initiate log: %d", rc);
		goto end_destroy_mosquitto;
	}
	rc = mosquitto_loop_start(mosq);
    if( rc ){
        syslog(LOG_ERR, "Failed to start loop: %d", rc);
        goto end_close_message_log;
    }
    syslog(LOG_INFO, "mqtt_subscriber started successfully");

	//method
	while( deamonize ) {
		if( !file ){
			file = freopen(FILE_PATH, "a", stdout);
			if( !file ){
				syslog(LOG_ERR, "Failed to open the file");
				goto end_destroy_mosquitto;
			}
		}
		sleep(1);
	}

	//close
	mosquitto_loop_stop(mosq, true);
	end_close_message_log:
		close_log();
	end_destroy_mosquitto:
		mosquitto_disconnect(mosq);
		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
	end_free_context:	
		uci_free_context(context);
		free_all_topic_nodes(&head);
		free_all_event_nodes(&event_head);
	end_close_log:
		syslog(LOG_INFO, "mqtt_subscriber was stopped");
		closelog();
	return rc;
}
