BIN:=mqtt_subscriber
LIBS:=-lmosquitto -largp -luci -lcurl

.PHONY: all clean

all: $(BIN)

$(BIN): main.c mqtt_control.c uci_util.c linked_list.c
	$(CC) $^ -o $@ $(LIBS)

clean:
	rm -f $(BIN)