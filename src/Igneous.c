#include <pebble.h>

static Window *window;
static TextLayer *text_layer;


static Layer *background_layer;

static GPath *ceiling_path_ptr = NULL;
static const GPathInfo CEILING_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{0, 0}, {3 * 144, 0}, {3 * 144, 7}, {0, 7}}
};

static GPath *floor_path_ptr = NULL;
static const GPathInfo FLOOR_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{0, (168 - 20)}, {3 * 144, (168 - 20)}, {3 * 144, (168 - 20) - 7}, {0, (168 - 20) - 7}}
};

static GPath *star1_path_ptr = NULL;
static const GPathInfo STAR1_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{144 + 7 - 2, 21}, {144 + 7, 21 - 2}, {144 + 7 + 2, 21}, {144 + 7, 21 + 2}}
};
static GPath *star2_path_ptr = NULL;
static const GPathInfo STAR2_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{144 + 42 - 2, 56}, {144 + 42, 56 - 2}, {144 + 42 + 2, 56}, {144 + 42, 56 + 2}}
};
static GPath *star3_path_ptr = NULL;
static const GPathInfo STAR3_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{144 + 70 - 2, 98}, {144 + 70, 98 - 2}, {144 + 70 + 2, 98}, {144 + 70, 98 + 2}}
};
static GPath *star4_path_ptr = NULL;
static const GPathInfo STAR4_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{144 + 105 - 2, 70}, {144 + 105, 70 - 2}, {144 + 105 + 2, 70}, {144 + 105, 70 + 2}}
};
static GPath *star5_path_ptr = NULL;
static const GPathInfo STAR5_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{144 + 119 - 2, 35}, {144 + 119, 35 - 2}, {144 + 119 + 2, 35}, {144 + 119, 35 + 2}}
};


static Layer *canvas_layer;

static GPath *ship_path_ptr = NULL;
static const GPathInfo SHIP_PATH_INFO = {
  .num_points = 3,
  .points = (GPoint []) {{7, ((168 - 20) / 2) + 7}, {28, (168 - 20) / 2}, {7, ((168 - 20) / 2) - 7}}
};

static int ship_position = ((168 - 20) / 2);

static PropertyAnimation *property_animation;
static PropertyAnimation *background_property_animation;


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

static void animate_background(void) {
  
  // Set start and end
  //GRect from_frame = layer_get_frame(background_layer);
  /*
  GRect from_frame = GRect(144 + (144 / 2), 0, 144 / 2, (168 - 20));
  GRect to_frame = GRect(-144 / 2, 0, 144 / 2, (168 - 20));
  */
  GRect from_frame = GRect(0, 0, 144 * 3, (168 - 20));
  GRect to_frame = GRect(144 * -2, 0, 144 * 3, (168 - 20));

  // Create the animation
  background_property_animation = property_animation_create_layer_frame(background_layer, &from_frame, &to_frame);
  animation_set_duration((Animation*) background_property_animation, 10000);
  animation_set_curve((Animation*) background_property_animation, AnimationCurveLinear);

  animation_set_handlers((Animation*) background_property_animation, (AnimationHandlers) {
    .stopped = (AnimationStoppedHandler) animate_background,
  }, NULL);

  // Schedule to occur ASAP with default settings
  animation_schedule((Animation*) background_property_animation);

}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");

  animate_background();
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

  // draw stars
  graphics_context_set_fill_color(ctx, GColorIcterine);
  gpath_draw_filled(ctx, star1_path_ptr);
  graphics_context_set_stroke_color(ctx, GColorPastelYellow);
  gpath_draw_outline(ctx, star1_path_ptr);

  graphics_context_set_fill_color(ctx, GColorIcterine);
  gpath_draw_filled(ctx, star2_path_ptr);
  graphics_context_set_stroke_color(ctx, GColorPastelYellow);
  gpath_draw_outline(ctx, star2_path_ptr);

  graphics_context_set_fill_color(ctx, GColorRajah);
  gpath_draw_filled(ctx, star3_path_ptr);
  graphics_context_set_stroke_color(ctx, GColorChromeYellow);
  gpath_draw_outline(ctx, star3_path_ptr);

  graphics_context_set_fill_color(ctx, GColorIcterine);
  gpath_draw_filled(ctx, star4_path_ptr);
  graphics_context_set_stroke_color(ctx, GColorPastelYellow);
  gpath_draw_outline(ctx, star4_path_ptr);

  graphics_context_set_fill_color(ctx, GColorIcterine);
  gpath_draw_filled(ctx, star5_path_ptr);
  graphics_context_set_stroke_color(ctx, GColorPastelYellow);
  gpath_draw_outline(ctx, star5_path_ptr);

}


static void window_load(Window *window) {

  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  text_layer = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 20 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));



  // Create background canvas layer - note triple-width!
  background_layer = layer_create(GRect(0, 0, bounds.size.w * 3, bounds.size.h));
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
  star1_path_ptr = gpath_create(&STAR1_PATH_INFO);
  star2_path_ptr = gpath_create(&STAR2_PATH_INFO);
  star3_path_ptr = gpath_create(&STAR3_PATH_INFO);
  star4_path_ptr = gpath_create(&STAR4_PATH_INFO);
  star5_path_ptr = gpath_create(&STAR5_PATH_INFO);
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
