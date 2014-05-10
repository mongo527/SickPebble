/**********************************
    *** Created by: Mongo527 ***
**********************************/

#include "pebble.h"
#include "common.h"

static Window *showsWindow;
static MenuLayer *shows_layer;

int NUMBER_SHOWS;
char shows[50][50];
char showStatus[50][12];

uint16_t num_sections_callback(MenuLayer *menu_layer, void *callback_context);
uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context);

void shows_load(Window *window) {
    menu_layer_reload_data(shows_layer);
}

void shows_unload(Window *window) {
    layer_destroy(window_get_root_layer(showsWindow));
    menu_layer_destroy(shows_layer);
}

void shows_init() {
     showsWindow = window_create();
    
    window_set_window_handlers(showsWindow, (WindowHandlers) {
        .load = shows_load,
        .unload = shows_unload,
    });
    
    Layer *layer = window_get_root_layer(showsWindow);
    shows_layer = menu_layer_create(layer_get_bounds(layer));
    menu_layer_set_callbacks(shows_layer, NULL, (MenuLayerCallbacks) {
        .get_num_sections = (MenuLayerGetNumberOfSectionsCallback) num_sections_callback,
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback,
        .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
        //.select_click = (MenuLayerSelectCallback) select_click_callback,
    });
    
    menu_layer_set_click_config_onto_window(shows_layer, showsWindow);
    layer_add_child(layer, menu_layer_get_layer(shows_layer));
    window_stack_push(showsWindow, true);
}

uint16_t num_sections_callback(MenuLayer *menu_layer, void *callback_context) {
    return 1;
}
 
uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return NUMBER_SHOWS;
}

void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    graphics_context_set_text_color(ctx, GColorBlack);
    menu_cell_basic_draw(ctx, cell_layer, shows[cell_index->row], showStatus[cell_index->row], NULL);
    //graphics_draw_text(ctx, shows[cell_index->row], fonts_get_system_font(FONT_KEY_GOTHIC_24), (GRect) { .origin = { 8, 0 }, .size = { 144 - 8, 28 } }, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

void shows_in_received_handler(DictionaryIterator *iter) {
    Tuple *sb_tuple = dict_find(iter, AKEY_SB_SHOWS);
    Tuple *shows_index_tuple = dict_find(iter, AKEY_SHOWS_INDEX);
    Tuple *shows_status_tuple = dict_find(iter, AKEY_SHOWS_STATUS);
    
    if(sb_tuple && shows_index_tuple) {
        NUMBER_SHOWS = shows_index_tuple->value->int16 + 1;
        
        strncpy(shows[shows_index_tuple->value->int16], sb_tuple->value->cstring, 49);
        strncpy(showStatus[shows_index_tuple->value->int16], shows_status_tuple->value->cstring, 49);
        menu_layer_reload_data(shows_layer);
    }
}