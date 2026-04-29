#include <pebble.h>

/* ── Aldo Watchface with Settings ──────────────────────────────────────
   5 color themes selectable via Pebble phone app settings.
   ──────────────────────────────────────────────────────────────────── */

#define TIME_X   22
#define TIME_Y   48
#define AMPM_X  122
#define AMPM_Y   57
#define DATE_X   23
#define DATE_Y   96

#define SETTINGS_KEY  1
#define NUM_THEMES    5

typedef struct {
  uint8_t theme;
} Settings;

typedef struct {
  GColor bg;
  GColor time_col;
  GColor ampm_col;
  GColor date_col;
} Theme;

/* Theme definitions */
static const Theme s_themes[NUM_THEMES] = {
  /* 0: Cream — current default */
  { .bg=GColorWhite,     .time_col=GColorDarkGray,  .ampm_col=GColorDarkGray,  .date_col=GColorLightGray },
  /* 1: ePaper — black bg, cream/white time, charcoal date */
  { .bg=GColorBlack,     .time_col=GColorWhite,      .ampm_col=GColorLightGray, .date_col=GColorDarkGray  },
  /* 2: Slate — dark gray bg, white time */
  { .bg=GColorDarkGray,  .time_col=GColorWhite,      .ampm_col=GColorLightGray, .date_col=GColorLightGray },
  /* 3: Warm — light yellow bg, dark time */
  { .bg=GColorPastelYellow, .time_col=GColorDarkGray, .ampm_col=GColorDarkGray, .date_col=GColorDarkGray },
  /* 4: Mint — mint green bg, dark time */
  { .bg=GColorMintGreen, .time_col=GColorDarkGray,  .ampm_col=GColorDarkGray,  .date_col=GColorDarkGray  },
};

static Settings s_settings;
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

static void apply_theme(void) {
  Theme t = s_themes[s_settings.theme];
  window_set_background_color(s_window, t.bg);
  text_layer_set_text_color(s_time_layer, t.time_col);
  text_layer_set_text_color(s_ampm_layer, t.ampm_col);
  text_layer_set_text_color(s_date_layer, t.date_col);
}

static void save_settings(void) {
  persist_write_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
}

static void load_settings(void) {
  s_settings.theme = 0;  /* default: Cream */
  persist_read_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
  if (s_settings.theme >= NUM_THEMES) s_settings.theme = 0;
}

static void inbox_received(DictionaryIterator *iter, void *ctx) {
  Tuple *t = dict_find(iter, MESSAGE_KEY_THEME);
  if (t) {
    s_settings.theme = (uint8_t)t->value->int32;
    if (s_settings.theme >= NUM_THEMES) s_settings.theme = 0;
    save_settings();
    apply_theme();
  }
}

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

  s_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALDO_50));
  s_font_ampm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALDO_13));
  s_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ALDO_16));

  s_time_layer = text_layer_create(GRect(TIME_X, TIME_Y, 112, 54));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, s_font_time);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  layer_add_child(root, text_layer_get_layer(s_time_layer));

  s_ampm_layer = text_layer_create(GRect(AMPM_X, AMPM_Y, 20, 16));
  text_layer_set_background_color(s_ampm_layer, GColorClear);
  text_layer_set_font(s_ampm_layer, s_font_ampm);
  text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentLeft);
  layer_add_child(root, text_layer_get_layer(s_ampm_layer));

  s_date_layer = text_layer_create(GRect(DATE_X, DATE_Y, 100, 20));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_font(s_date_layer, s_font_date);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  layer_add_child(root, text_layer_get_layer(s_date_layer));

  apply_theme();

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
  load_settings();
  app_message_open(64, 64);
  app_message_register_inbox_received(inbox_received);
  s_window = window_create();
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
