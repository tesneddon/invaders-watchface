/*++
**  MODULE DESCRIPTION:
**
**	This is a Pebble Watchface App based on the popular Space Invaders
**  game.  It has the following features:
**
**    * Switches invaders after one minute; and
**    * At the change of the hour, the ship will fly across the screen.
**
**  Thanks also to Sean McMains for inspiring this project.
**
**  AUTHOR:	Tim Sneddon
**
**  Copyright (c) 2013, Tim Sneddon <tim@sneddon.id.au>.
**
**  All rights reserved.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions
**  are met:
**
**      * Redistributions of source code must retain the above
**        copyright notice, this list of conditions and the following
**        disclaimer.
**      * Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**      * Neither the name of the copyright owner nor the names of any
**        other contributors may be used to endorse or promote products
**        derived from this software without specific prior written
**        permission.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
**  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
**  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
**  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
**  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
**  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
**  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**  CREATION DATE: 14-OCT-2013
**
**  MODIFICATION HISTORY:
**
**      14-OCT-2013 V1.0    Sneddon     Initial coding.
**--
*/
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

/*
** Forward Declartations
*/

    void pbl_main(void *params);
    void handle_init(AppContextRef ctx);
    void handle_tick(AppContextRef ctx, PebbleTickEvent *event);
    void handle_ship_animation_stopped(Animation *animation, bool finished,
    	    	    	    	       void *context);
    void handle_deinit(AppContextRef ctx);

/*
** Pebble Watchapp Identification
*/
#define MY_UUID { 0x5C, 0x5A, 0x4F, 0x02, \
		  0xCA, 0x9C, 0x48, 0xF0, \
		  0xA8, 0xF4, 0x1C, 0x67, \
		  0x84, 0xFB, 0x2C, 0x73 }

PBL_APP_INFO(MY_UUID,
             "Space Invaders Watchface", "Tim Sneddon",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

/*
** Global storage
*/

    int index;
    unsigned offset;
    HeapBitmap invaders[6], ship;
    BitmapLayer invader_layer, ship_layer;
    TextLayer date_layer, time_layer;
    PropertyAnimation ship_animation;
    Window window;
    char datetime[] = "";

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
    	.init_handler = &handle_init,
    	.tick_info = {
    	    .tick_handler = handle_tick,
    	    .tick_units = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT,
    	},
        .deinit_handler = handle_deinit,
    };
    app_event_loop(params, &handlers);
}

void handle_init(AppContextRef ctx) {

    PebbleTickEvent event;
    PblTm now;
    GRect to_frame = GRect(-65, 30, 65, 28);

    window_init(&window, "Invaders Watchface");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    resource_init_current_app(&APP_RESOURCES);

    /*
    ** Load the ship from resources and configure the animation.
    */
    heap_bitmap_init(&ship, RESOURCE_ID_SHIP);
    bitmap_layer_init(&ship_layer, GRect(144, 30, 65, 28));
    bitmap_layer_set_bitmap(&ship_layer, &ship.bmp);
    layer_add_child(&window.layer, &ship_layer.layer);

    property_animation_init_layer_frame(&ship_animation, &ship_layer.layer,
    	    	    	    	    	NULL, &to_frame);
    animation_set_duration(&ship_animation.animation, 1800);
    animation_set_handlers(&ship_animation.animation,
			   (AnimationHandlers) {
			       .stopped = handle_ship_animation_stopped
			   }, NULL);

    /*
    ** Load the invaders from resources and configure the layer.
    */
    index = offset = 0;

    heap_bitmap_init(&invaders[0], RESOURCE_ID_INVADER_A_1);
    heap_bitmap_init(&invaders[1], RESOURCE_ID_INVADER_A_2);
    heap_bitmap_init(&invaders[2], RESOURCE_ID_INVADER_B_1);
    heap_bitmap_init(&invaders[3], RESOURCE_ID_INVADER_B_2);
    heap_bitmap_init(&invaders[4], RESOURCE_ID_INVADER_C_1);
    heap_bitmap_init(&invaders[5], RESOURCE_ID_INVADER_C_2);

    bitmap_layer_init(&invader_layer, GRect(6, 0, 132, 96));
    layer_add_child(&window.layer, &invader_layer.layer);

    /*
    ** Configure date/time layer.
    */
    text_layer_init(&date_layer, GRect(6, 96, 132, 26));
    text_layer_set_background_color(&date_layer, GColorClear);
    text_layer_set_font(&date_layer,
    	    	    	fonts_get_system_font(FONT_KEY_GOTHIC_24));
    text_layer_set_text_color(&date_layer, GColorWhite);
    text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
    layer_add_child(&window.layer, &date_layer.layer);

    text_layer_init(&time_layer, GRect(6, 120, 132, 40));
    text_layer_set_background_color(&time_layer, GColorClear);
    text_layer_set_font(&time_layer,
    	    	    	fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_color(&time_layer, GColorWhite);
    text_layer_set_text_alignment(&time_layer, GTextAlignmentCenter);
    layer_add_child(&window.layer, &time_layer.layer);

    /*
    ** Set initial time...
    */
    event.units_changed = DAY_UNIT | SECOND_UNIT;
    event.tick_time = &now;
    get_time(&now);
    handle_tick(ctx, &event);
}

void handle_tick(AppContextRef ctx,
    	    	 PebbleTickEvent *event) {

    static char date_string[] = "14 Mar, 1983"; /* Filled out with maximum */
    static char time_string[] = "11:30:00 pm";  /* values for sizeof(). */

    /*
    ** Update the date, only if we have ticked over a day.
    */
    if (event->units_changed & DAY_UNIT) {
    	string_format_time(date_string, sizeof(date_string), "%e %b, %Y",
    	    	    	   event->tick_time);
    	text_layer_set_text(&date_layer, date_string);
    }

    /*
    ** Update the time.
    */
    string_format_time(time_string, sizeof(time_string),
		       (clock_is_24h_style() ? "%k:%M:%S" : "%l:%M:%S %P"),
    	    	       event->tick_time);
    text_layer_set_text(&time_layer, time_string);

    /*
    ** If we've ticked over a minute, then switch the invader animation.
    */
    if (event->units_changed & MINUTE_UNIT) {
    	index = 0;
	offset += 2;
    	if (offset >= sizeof(invaders)/sizeof(invaders[0])) {
    	    offset = 0;
    	}
    } else {
    	index = abs(index - 1);
    }

    /*
    ** Update the invader animation.
    */
    if (event->units_changed & HOUR_UNIT) {
    	layer_set_hidden(&invader_layer.layer, true);
    	layer_set_hidden(&ship_layer.layer, false);
    	animation_schedule(&ship_animation.animation);
    } else {
    	bitmap_layer_set_bitmap(&invader_layer, &invaders[offset+index].bmp);
    }
}

void handle_ship_animation_stopped(Animation *animation,
				   bool finished,
				   void *data) {
    /*
    ** Re-enable the invader layer after the ship has passed.
    */
    layer_set_hidden(&invader_layer.layer, false);
}

void handle_deinit(AppContextRef ctx) {
    unsigned i;

    /*
    ** Tidy up the ship.
    */
    heap_bitmap_deinit(&ship);

    /*
    ** Tidy up the invaders bitmaps.
    */
    for (i = 0; i < sizeof(invaders)/sizeof(invaders[0]); i++) {
    	heap_bitmap_deinit(&invaders[i]);
    }
}
