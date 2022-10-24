#include <sqlite3.h>
#include "uci_util.h"


int write_to_log(char *topic, char *message);
int close_log();
int init_log();
