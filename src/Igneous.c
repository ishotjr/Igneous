#include <pebble.h>

static Window *window;
static TextLayer *text_layer;


static Layer *canvas_layer;

static GPath *ship_path_ptr = NULL;
static const GPathInfo UP_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{7, ((168 - 20) / 2) + 7}, {28, (168 - 20) / 2}, {7, ((168 - 20) / 2) - 7}}
};


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
  layer_mark_dirty(canvas_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}


static void canvas_update_proc(Layer *this_layer, GContext *ctx) {

  //GRect bounds = layer_get_bounds(this_layer);

  // draw filled triangle
  graphics_context_set_fill_color(ctx, GColorElectricBlue);
  gpath_draw_filled(ctx, ship_path_ptr);
  graphics_context_set_stroke_color(ctx, GColorFolly);
  gpath_draw_outline(ctx, ship_path_ptr);

}


static void window_load(Window *window) {

  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  text_layer = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 20 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));


  // Create canvas layer
  canvas_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_add_child(window_layer, canvas_layer);

  // Set the update_proc
  layer_set_update_proc(canvas_layer, canvas_update_proc);

  // path setup(s)
  ship_path_ptr = gpath_create(&UP_PATH_INFO);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  layer_destroy(canvas_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
