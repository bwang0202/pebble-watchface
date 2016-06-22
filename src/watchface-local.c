#include <pebble.h>

#define KEY_ADDRESS 0

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *random_funny_text_layer;
static TextLayer *s_addr_layer;
static char suckers[] = "sup suckers";
static char emoji[] = "@ - .. - @!";
static char bangbangbang[] = "bang bang bang";
static char gd[] = "Hello World";
static char shitty[] = "Bullshit All Day";
static char nice[] = "Have a beautiful day";
static char bs[] = "Just Do It";
static int counter;

static char* allwords[6];

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  // Display random funny text on TextLayer
  counter = (counter + 1) % 6;
  text_layer_set_text(random_funny_text_layer, allwords[counter]);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  // Get weather update every 1 minutes
 if (tick_time->tm_min % 1 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 1, 1);

    // Send the message!
    app_message_outbox_send();
  }

}
static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, 17, bounds.size.w, 50));
  // Create random TextLayer with specific bounds
  random_funny_text_layer = text_layer_create(GRect(0, 57, bounds.size.w, 50));
    // Improve the layout to be more like a watchface
  text_layer_set_background_color(random_funny_text_layer, GColorClear);
  text_layer_set_text_color(random_funny_text_layer, GColorBlack);
  text_layer_set_font(random_funny_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(random_funny_text_layer, GTextAlignmentCenter);
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Create addr text layer
  s_addr_layer = text_layer_create(
    GRect(1, 75, bounds.size.w, 100));
  
  text_layer_set_background_color(s_addr_layer, GColorClear);
  text_layer_set_text_color(s_addr_layer, GColorBlack);
  text_layer_set_text_alignment(s_addr_layer, GTextAlignmentLeft);
  text_layer_set_text(s_addr_layer, "Loading...");
  text_layer_set_font(s_addr_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(random_funny_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_addr_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  // Destroy weather elements
  // Destroy
  text_layer_destroy(random_funny_text_layer);
  text_layer_destroy(s_addr_layer);
}
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char address_layer_buffer[256];
  APP_LOG(APP_LOG_LEVEL_INFO, "asldfjalf");
  // Read tuples for data
  Tuple *address_tuple = dict_find(iterator, 0);

  // If address is available
  if(address_tuple) {
	APP_LOG(APP_LOG_LEVEL_INFO, "address tuple");
    snprintf(address_layer_buffer, sizeof(address_layer_buffer), "%s", address_tuple->value->cstring);
    text_layer_set_text(s_addr_layer, address_layer_buffer);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {

  // First initialize allwords
  allwords[0] = suckers;
  allwords[1] = emoji;
  allwords[2] = bangbangbang;
  allwords[3] = gd;
  allwords[4] = shitty;
  allwords[5] = nice;
  counter = 1;
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
   
  update_time();
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

