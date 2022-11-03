#include "log_util.h"

#define LOG_FILE_PATH "/var/log/mqtt_log.db"

static sqlite3 *db_con = NULL;

static int run_single_query(char *query, int argc, char *argv[])
{
	sqlite3_stmt *res;
	int rc = 0;
	rc = sqlite3_prepare(db_con, query,
			-1, &res, 0);
	if (rc != SQLITE_OK) {
        
        	syslog(LOG_ERR, "Failed to execute query: %s\n", sqlite3_errmsg(db_con));
        	return rc;
	}    
	for(int i = 0; i < argc; i++){
		sqlite3_bind_text(res, i+1, argv[i], strlen(argv[i]) , NULL);
	}
	rc = sqlite3_step(res);
	if (rc != SQLITE_OK && rc != SQLITE_DONE)
		return rc;
	sqlite3_clear_bindings(res);
	sqlite3_reset(res);
	sqlite3_finalize(res);	
	return 0;
}

static int db_setup()
{ 
	int rc = 0;
	rc = run_single_query("CREATE TABLE IF NOT EXISTS MQTTMessage ( ID integer NOT NULL " 
			"PRIMARY KEY AUTOINCREMENT, Topic TEXT(30), " 
			" Time datetime DEFAULT CURRENT_TIMESTAMP, Message TEXT(255)) ;", 0, NULL);
	return 0;
}


int init_log() 
{
	int rc = 0;
	rc = sqlite3_open(LOG_FILE_PATH, &db_con);
	if(rc){
		fprintf(stderr, "Can't open the database: %s\n", sqlite3_errmsg(db_con));
		sqlite3_close(db_con);
		return rc;
	}
	db_setup();
	return rc;
}
int close_log()
{
	sqlite3_close(db_con);
	return 0;
}

int write_to_log(char *topic, char *message)
{
	int rc = 0;

	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "INSERT INTO MQTTMessage (Topic, "
		"Message) VALUES (?, ?)");
	char *args[3] = {topic, message};
	rc = run_single_query(buffer, 2, args);
	return rc;
}
