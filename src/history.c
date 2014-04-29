/**********************************
    *** Created by: Mongo527 ***
**********************************/

#include "pebble.h"
#include "common.h"

static Window *historyWindow;
static MenuLayer *history_layer;

int NUMBER_HISTORY;
char history[50][50];
char historyType[50][12];

uint16_t history_num_sections_callback(MenuLayer *menu_layer, void *callback_context);
uint16_t history_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
void history_draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context);

void history_load(Window *window) {
    menu_layer_reload_data(history_layer);
}

void history_unload(Window *window) {
    layer_destroy(window_get_root_layer(historyWindow));
    menu_layer_destroy(history_layer);
    window_destroy(historyWindow);
}

void history_init() {
    historyWindow = window_create();
    
    window_set_window_handlers(historyWindow, (WindowHandlers) {
        .load = history_load,
        .unload = history_unload,
    });
    
    Layer *layer = window_get_root_layer(historyWindow);
    history_layer = menu_layer_create(layer_get_bounds(layer));
    menu_layer_set_callbacks(history_layer, NULL, (MenuLayerCallbacks) {
        .get_num_sections = (MenuLayerGetNumberOfSectionsCallback) history_num_sections_callback,
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) history_num_rows_callback,
        .draw_row = (MenuLayerDrawRowCallback) history_draw_row_callback,
    });
    
    menu_layer_set_click_config_onto_window(history_layer, historyWindow);
    layer_add_child(layer, menu_layer_get_layer(history_layer));
    window_stack_push(historyWindow, true);
}

uint16_t history_num_sections_callback(MenuLayer *menu_layer, void *callback_context) {
    return 1;
}
 
uint16_t history_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return NUMBER_HISTORY;
}

void history_draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    graphics_context_set_text_color(ctx, GColorBlack);
    menu_cell_basic_draw(ctx, cell_layer, history[cell_index->row], historyType[cell_index->row], NULL);
    //graphics_draw_text(ctx, shows[cell_index->row], fonts_get_system_font(FONT_KEY_GOTHIC_24), (GRect) { .origin = { 8, 0 }, .size = { 144 - 8, 28 } }, GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

void history_in_received_handler(DictionaryIterator *iter) {
    Tuple *sb_history = dict_find(iter, AKEY_SB_HISTORY);
    Tuple *history_index_tuple = dict_find(iter, AKEY_SHOWS_INDEX);
    Tuple *history_type_tuple = dict_find(iter, AKEY_HISTORY_TYPE);

    if(sb_history && history_index_tuple && history_type_tuple) {
        NUMBER_HISTORY = history_index_tuple->value->int16 + 1;
        
        strncpy(history[history_index_tuple->value->int16], sb_history->value->cstring, 49);
        strncpy(historyType[history_index_tuple->value->int16], history_type_tuple->value->cstring, 49);
        menu_layer_reload_data(history_layer);
    }
}