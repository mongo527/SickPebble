/**********************************
    *** Created by: Mongo527 ***
**********************************/

#include "pebble.h"
#include "common.h"
#include "showsList.h"
#include "upcoming.h"
#include "history.h"

static Window *window;
static Window *optionsWindow;

static SimpleMenuLayer *main_layer;
static SimpleMenuSection main_sections[1];
static SimpleMenuItem main_items[4];

static SimpleMenuLayer *options_layer;
static SimpleMenuSection options_sections[1];
static SimpleMenuItem options_items[4];

static void shows_callback (int index, void *ctx) {
	Tuplet shows_tuple = TupletCString(AKEY_SHOWS, "shows&sort=name");
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
		return;
	}
	
	dict_write_tuplet(iter, &shows_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
    
    shows_init();
}

static void upcoming_callback(int index, void *ctx) {
    Tuplet upcoming_tuple = TupletCString(AKEY_UPCOMING, "upcoming");
    
    DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
		return;
	}
	
	dict_write_tuplet(iter, &upcoming_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
    
    upcoming_init();
}

static void history_callback(int index, void *ctx) {
    Tuplet history_tuple = TupletCString(AKEY_HISTORY, "history");
    
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    if(iter == NULL) {
        return;
    }
    
    dict_write_tuplet(iter, &history_tuple);
    dict_write_end(iter);
    
    app_message_outbox_send();
    
    history_init();
}

static void backlog_select_callback(int index, void *ctx) {
	Tuplet backlog_tuple = TupletCString(AKEY_BACKLOG, "sb.forcesearch");
	
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
		return;
	}
	
	dict_write_tuplet(iter, &backlog_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

static void restart_select_callback(int index, void *ctx) {
    Tuplet restart_tuple = TupletCString(AKEY_RESTART, "sb.restart");
    
    DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
		return;
	}
	
	dict_write_tuplet(iter, &restart_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

static void ping_select_callback() {
    Tuplet ping_tuple = TupletCString(AKEY_PING, "sb.ping");
    
    DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
		return;
	}
	
	dict_write_tuplet(iter, &ping_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

static void shutdown_select_callback(int index, void *ctx) {
    Tuplet shutdown_tuple = TupletCString(AKEY_SHUTDOWN, "sb.shutdown");
    
    DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	if (iter == NULL) {
		return;
	}
	
	dict_write_tuplet(iter, &shutdown_tuple);
	dict_write_end(iter);
	
	app_message_outbox_send();
}

static void options_load(Window *window) {
    Layer *layer = window_get_root_layer(optionsWindow);
    options_layer = simple_menu_layer_create(layer_get_bounds(layer), optionsWindow, options_sections, 1, NULL);
	layer_add_child(layer, simple_menu_layer_get_layer(options_layer));
}

static void options_unload(Window *window) {
    layer_destroy(window_get_root_layer(optionsWindow));
    simple_menu_layer_destroy(options_layer);
}

static void options_callback(int index, void *ctx) {
    
    options_items[0] = (SimpleMenuItem) {
        .title = "Force Backlog",
        .callback = backlog_select_callback,
    };
    
    options_items[1] = (SimpleMenuItem) {
        .title = "Test Connection",
        .callback = ping_select_callback,
    };
    
    options_items[2] = (SimpleMenuItem) {
        .title = "Restart SB",
        .callback = restart_select_callback,
    };
    
    options_items[3] = (SimpleMenuItem) {
        .title = "Shutdown SB",
        .callback = shutdown_select_callback,
    };
    
    options_sections[0] = (SimpleMenuSection) {
        //.title = "Options",
		.num_items = 4,
		.items = options_items,
	};
    
    optionsWindow = window_create();
    
    window_set_window_handlers(optionsWindow, (WindowHandlers) {
        .load = options_load,
        .unload = options_unload,
    });
    
    window_stack_push(optionsWindow, true);
}

static void window_load(Window *window) {
	// Set up the first menu list (Options Menu List)	
	main_items[0] = (SimpleMenuItem) {
		.title = "Shows",
		.callback = shows_callback,
	};
    
    main_items[1] = (SimpleMenuItem) {
        .title = "Upcoming",
        .callback = upcoming_callback,
    };
    
    main_items[2] = (SimpleMenuItem) {
        .title = "History",
        .callback = history_callback,
    };
    
    main_items[3] = (SimpleMenuItem) {
		.title = "Options",
		.callback = options_callback,
	};
	
	main_sections[0] = (SimpleMenuSection) {
		//.title = "Hello World",
		.num_items = 4,
		.items = main_items,
	};
	
	Layer *layer = window_get_root_layer(window);
	main_layer = simple_menu_layer_create(layer_get_frame(layer), window, main_sections, 1, NULL);
	layer_add_child(layer, simple_menu_layer_get_layer(main_layer));
}

void window_unload(Window *window) {
    layer_destroy(window_get_root_layer(window));
	simple_menu_layer_destroy(main_layer);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *sb_tuple = dict_find(iter, AKEY_SB_SHOWS);
    Tuple *sb_up_tuple = dict_find(iter, AKEY_SB_UPCOMING);
    Tuple *sb_hs_tuple = dict_find(iter, AKEY_SB_HISTORY);
    Tuple *backlog_tuple = dict_find(iter, AKEY_BACKLOG);
    Tuple *ping_tuple = dict_find(iter, AKEY_PING);
    Tuple *restart_tuple = dict_find(iter, AKEY_RESTART);
    Tuple *shutdown_tuple = dict_find(iter, AKEY_SHUTDOWN);
    
    if (sb_tuple) {
        shows_in_received_handler(iter);
    }
    
    else if (sb_up_tuple) {
        upcoming_in_received_handler(iter);
    }
    
    else if(sb_hs_tuple) {
        history_in_received_handler(iter);
    }
    
    else if (backlog_tuple) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Subtitle: %s", backlog_tuple->value->cstring);
        options_items[0].subtitle = backlog_tuple->value->cstring;
        
        options_items[1].subtitle = NULL;
        options_items[2].subtitle = NULL;
        options_items[3].subtitle = NULL;
        
        layer_mark_dirty(simple_menu_layer_get_layer(options_layer));
    }
    
    else if (ping_tuple) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Subtitle: %s", ping_tuple->value->cstring);
        options_items[1].subtitle = ping_tuple->value->cstring;
        
        options_items[0].subtitle = NULL;
        options_items[2].subtitle = NULL;
        options_items[3].subtitle = NULL;
        
        layer_mark_dirty(simple_menu_layer_get_layer(options_layer));
    }
    
    else if (restart_tuple) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Subtitle: %s", restart_tuple->value->cstring);
        options_items[2].subtitle = restart_tuple->value->cstring;
        
        options_items[0].subtitle = NULL;
        options_items[1].subtitle = NULL;
        options_items[3].subtitle = NULL;
        
        layer_mark_dirty(simple_menu_layer_get_layer(options_layer));
    }
    
    else if (shutdown_tuple) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Subtitle: %s", shutdown_tuple->value->cstring);
        options_items[3].subtitle = shutdown_tuple->value->cstring;
        
        options_items[0].subtitle = NULL;
        options_items[1].subtitle = NULL;
        options_items[2].subtitle = NULL;
        
        layer_mark_dirty(simple_menu_layer_get_layer(options_layer));
    }
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Inbound Message was Dropped: %d", reason);
}

static void app_message_init(void) {
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	//app_message_register_outbox_failed(out_failed_handler);
	
	app_message_open(512, 512);
}

int main(void) {
	window = window_create();
	app_message_init();
	
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	window_stack_push(window, false);
	app_event_loop();
} 