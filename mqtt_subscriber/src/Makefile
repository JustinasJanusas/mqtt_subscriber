BIN:=mqtt_subscriber
LIBS:=-lmosquitto -largp -luci -lcurl -ljson-c -lsqlite3

.PHONY: all clean

all: $(BIN)

$(BIN): main.c mqtt_control.c uci_util.c linked_list.c mail_util.c event_handler.c \
		log_util.c
	$(CC) $^ -o $@ $(LIBS)

clean:
	rm -f $(BIN)