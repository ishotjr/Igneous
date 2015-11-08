#include <pebble.h>

static Window *window;
static TextLayer *text_layer;


static Layer *background_layer;

static GPath *ceiling_path_ptr = NULL;
static const GPathInfo CEILING_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{0, 0}, {144, 0}, {144, 7}, {0, 7}}
};

static GPath *floor_path_ptr = NULL;
static const GPathInfo FLOOR_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{0, (168 - 20)}, {144, (168 - 20)}, {144, (168 - 20) - 7}, {0, (168 - 20) - 7}}
};


static Layer *canvas_layer;

static GPath *ship_path_ptr = NULL;
static const GPathInfo SHIP_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{7, ((168 - 20) / 2) + 7}, {28, (168 - 20) / 2}, {7, ((168 - 20) / 2) - 7}}
};

static int ship_position = ((168 - 20) / 2);

static PropertyAnimation *property_animation;


static void animate_ship(void) {
  
  // Set start and end
  GRect from_frame = layer_get_frame(canvas_layer);
  GRect to_frame = GRect(0, ship_position - ((168 - 20) / 2), 144, ship_position + ((168 - 20) / 2));

  // Create the animation
  property_animation = property_animation_create_layer_frame(canvas_layer, &from_frame, &to_frame);

  // Schedule to occur ASAP with default settings
  animation_schedule((Animation*) property_animation);

}

static void explode_ship(void) {

  // TODO: differentiate from animate_ship()!

  // Set start and end
  GRect from_frame = layer_get_frame(canvas_layer);
  GRect to_frame = GRect(0, ship_position - ((168 - 20) / 2), 144, ship_position + ((168 - 20) / 2));

  // Create the animation
  property_animation = property_animation_create_layer_frame(canvas_layer, &from_frame, &to_frame);

  // Schedule to occur ASAP with default settings
  animation_schedule((Animation*) property_animation);

}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");

  // bounds checking
  ship_position -= 14;

  if (ship_position < 18) {
    ship_position = ((168 - 20) / 2);
    explode_ship();
  } else {
    animate_ship();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");

  // bounds checking
  ship_position += 14;

  if (ship_position > 130) {
    ship_position = ((168 - 20) / 2);
    explode_ship();
  } else {
    animate_ship();
  }
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

static void background_update_proc(Layer *this_layer, GContext *ctx) {

  // draw landscape
  graphics_context_set_fill_color(ctx, GColorWindsorTan);
  gpath_draw_filled(ctx, ceiling_path_ptr);
  graphics_context_set_stroke_color(ctx, GColorWindsorTan);
  gpath_draw_outline(ctx, ceiling_path_ptr);

  graphics_context_set_fill_color(ctx, GColorWindsorTan);
  gpath_draw_filled(ctx, floor_path_ptr);
  graphics_context_set_stroke_color(ctx, GColorWindsorTan);
  gpath_draw_outline(ctx, floor_path_ptr);

}


static void window_load(Window *window) {

  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  text_layer = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 20 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));



  // Create background canvas layer
  background_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_add_child(window_layer, background_layer);

  // Set the update_proc
  layer_set_update_proc(background_layer, background_update_proc);


  // Create canvas layer
  canvas_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_add_child(window_layer, canvas_layer);

  // Set the update_proc
  layer_set_update_proc(canvas_layer, canvas_update_proc);


  // path setup(s)
  ship_path_ptr = gpath_create(&SHIP_PATH_INFO);
  ceiling_path_ptr = gpath_create(&CEILING_PATH_INFO);
  floor_path_ptr = gpath_create(&FLOOR_PATH_INFO);
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
