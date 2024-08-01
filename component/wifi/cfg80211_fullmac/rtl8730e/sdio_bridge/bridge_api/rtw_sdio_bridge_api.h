


typedef void (*bridge_wifi_event_cb_t)(int event);

int bridge_wifi_connect(char *ssid, char *pwd);
int bridge_wifi_disconnect(void);
int bridge_wifi_scan(void);
int bridge_wifi_get_scanres(void);
pthread_t   *bridge_wifi_event_callback_init(void);
void bridge_wifi_event_callback_deinit(pthread_t *handle);
int bridge_wifi_register_event_callback(int event, bridge_wifi_event_cb_t handler_func);


