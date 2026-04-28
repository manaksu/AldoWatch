#include <pebble.h>

#define TIME_X   21
#define TIME_Y   62
#define AMPM_X  106
#define AMPM_Y   66
#define DATE_X   21
#define DATE_Y  104

static Window      *s_window;
static TextLayer   *s_time_layer;
static TextLayer   *s_ampm_layer;
static TextLayer   *s_date_layer;
static GFont        s_font_time;
static GFont        s_font_ampm;
static GFont        s_font_date;

static char s_time_buf[6];
static char s_ampm_buf[3];
static char s_date_buf[15];

static void update_time(struct tm *t) {
  strftime(s_time_buf, sizeof(s_time_buf), "%H:%M", t);
  strftime(s_ampm_buf, sizeof(s_ampm_buf), "%p", t);

  char day[4], mday[3], mon[4], yr[3];
  strftime(day,  sizeof(day),  "%a", t);
  strftime(mday, sizeof(mday), "%d", t);
  strftime(mon,  sizeof(mon),  "%b", t);
  strftime(yr,   sizeof(yr),   "%y", t);
  for (int i=0;day[i];i++) if(day[i]>='a'&&day[i]<='z') day[i]-=32;
  for (int i=0;mon[i];i++) if(mon[i]>='a'&&mon[i]<='z') mon[i]-=32;
  snprintf(s_date_buf, sizeof(s_date_buf), "%s,%s %s %s", day, mday, mon, yr);

  text_layer_set_text(s_time_layer, s_time_buf);
  text_layer_set_text(s_ampm_layer, s_ampm_buf);
  text_layer_set_text(s_date_layer, s_date_buf);
}

static void tick_handler(struct tm *t, TimeUnits u) { update_time(t); }

static void window_load(Window *w) {
  Layer *root = window_get_root_layer(w);

  s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALDO_42));
  s_font_ampm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALDO_16));
  s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALDO_15));

  s_time_layer = text_layer_create(GRect(TIME_X, TIME_Y, 85, 46));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorDarkGray);
  text_layer_set_font(s_time_layer, s_font_time);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  layer_add_child(root, text_layer_get_layer(s_time_layer));

  s_ampm_layer = text_layer_create(GRect(AMPM_X, AMPM_Y, 22, 20));
  text_layer_set_background_color(s_ampm_layer, GColorClear);
  text_layer_set_text_color(s_ampm_layer, GColorLightGray);
  text_layer_set_font(s_ampm_layer, s_font_ampm);
  text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentLeft);
  layer_add_child(root, text_layer_get_layer(s_ampm_layer));

  s_date_layer = text_layer_create(GRect(DATE_X, DATE_Y, 81, 20));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorLightGray);
  text_layer_set_font(s_date_layer, s_font_date);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  layer_add_child(root, text_layer_get_layer(s_date_layer));

  time_t now = time(NULL);
  update_time(localtime(&now));
}

static void window_unload(Window *w) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_ampm_layer);
  text_layer_destroy(s_date_layer);
  fonts_unload_custom_font(s_font_time);
  fonts_unload_custom_font(s_font_ampm);
  fonts_unload_custom_font(s_font_date);
}

static void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load, .unload = window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) { init(); app_event_loop(); deinit(); }
