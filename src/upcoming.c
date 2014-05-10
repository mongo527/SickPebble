/**********************************
    *** Created by: Mongo527 ***
**********************************/

#include "pebble.h"
#include "common.h"

static Window *upcomingWindow;
static MenuLayer *upcoming_layer;

int NUMBER_UPCOMING;
char upcoming[25][50];
char upcomingTime[25][20];

uint16_t upcoming_num_sections_callback(MenuLayer *menu_layer, void *callback_context);
uint16_t upcoming_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
void upcoming_draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context);

void upcoming_load(Window *window) {
    menu_layer_reload_data(upcoming_layer);
}

void upcoming_unload(Window *window) {
    layer_destroy(window_get_root_layer(upcomingWindow));
    menu_layer_destroy(upcoming_layer);
}

void upcoming_init() {
     upcomingWindow = window_create();
    
    window_set_window_handlers(upcomingWindow, (WindowHandlers) {
        .load = upcoming_load,
        .unload = upcoming_unload,
    });
    
    Layer *layer = window_get_root_layer(upcomingWindow);
    upcoming_layer = menu_layer_create(layer_get_bounds(layer));
    menu_layer_set_callbacks(upcoming_layer, NULL, (MenuLayerCallbacks) {
        .get_num_sections = (MenuLayerGetNumberOfSectionsCallback) upcoming_num_sections_callback,
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) upcoming_num_rows_callback,
        .draw_row = (MenuLayerDrawRowCallback) upcoming_draw_row_callback,
        //.select_click = (MenuLayerSelectCallback) select_click_callback,
    });
    
    menu_layer_set_click_config_onto_window(upcoming_layer, upcomingWindow);
    layer_add_child(layer, menu_layer_get_layer(upcoming_layer));
    window_stack_push(upcomingWindow, true);
}

uint16_t upcoming_num_sections_callback(MenuLayer *menu_layer, void *callback_context) {
    return 1;
}
 
uint16_t upcoming_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return NUMBER_UPCOMING;
}

void upcoming_draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    graphics_context_set_text_color(ctx, GColorBlack);
    menu_cell_basic_draw(ctx, cell_layer, upcoming[cell_index->row], upcomingTime[cell_index->row], NULL);
    //graphics_draw_text(ctx, shows[cell_index->row], fonts_get_system_font(FONT_KEY_GOTHIC_24), (GRect) { .origin = { 8, 0 }, .size = { 144 - 8, 28 } }, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

void upcoming_in_received_handler(DictionaryIterator *iter) {
    Tuple *up_tuple = dict_find(iter, AKEY_SB_UPCOMING);
    Tuple *upcoming_index_tuple = dict_find(iter, AKEY_SHOWS_INDEX);
    Tuple *upcoming_time_tuple = dict_find(iter, AKEY_UPCOMING_TIME);
    
    if(up_tuple && upcoming_index_tuple) {
        NUMBER_UPCOMING = upcoming_index_tuple->value->int16 + 1;
        
        strncpy(upcoming[upcoming_index_tuple->value->int16], up_tuple->value->cstring, 49);
        strncpy(upcomingTime[upcoming_index_tuple->value->int16], upcoming_time_tuple->value->cstring, 49);
        menu_layer_reload_data(upcoming_layer);
    }
}