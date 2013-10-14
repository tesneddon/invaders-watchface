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
    void handle_init(AppContextRef *ctx);
    void handle_tick(AppContextRef *ctx, PebbleTickEvent *event);

/*
** Pebble Watchapp Identification
*/
#define MY_UUID { 0x5C, 0x5A, 0x4F, 0x02,
		  0xCA, 0x9C, 0x48, 0xF0,
		  0xA8, 0xF4, 0x1C, 0x67,
		  0x84, 0xFB, 0x2C, 0x73 }

PBL_APP_INFO(MY_UUID,
             "Invaders Watchapp", "Tim Sneddon",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

/*
** Global storage
*/

    BmpContainer invaders[6];
    Window window;

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
    	.init_handler = &handle_init
    	.tick_info = {
    	    .tick_handler = &handle_tick,
    	    .tick_units = SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT,
    	},
        .deinit_handler = &handle_deinit,
    };
    app_event_loop(params, &handlers);
}

void handle_init(AppContextRef ctx) {

    window_init(&window, "Space Invaders");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    /*
    ** Load the invaders from resources and configure the layer.
    */
    resource_init_current_app(&APP_RESOURCES);

    bmp_init_container(RESOURCE_ID_, &invaders[0]);
    bmp_init_container(RESOURCE_ID_, &invaders[1]);
    bmp_init_container(RESOURCE_ID_, &invaders[2]);
    bmp_init_container(RESOURCE_ID_, &invaders[3]);
    bmp_init_container(RESOURCE_ID_, &invaders[4]);
    bmp_init_container(RESOURCE_ID_, &invaders[5]);

    layer_init(&image, GRect(6, 6, 132, 96));
    image.update_proc = &draw_invader;
    layer_add_child(&window.layer, &image);
}

void handle_tick(AppContextRef *ctx,
    	    	 PebbleTickEvent *event) {

    // what is a 

}

void handle_deinit(AppContextRef ctx) {
    char i;

    for (i = 0; i < sizeof(invaders/invaders[0]); i++) {
    	bmp_deinit_container(&invaders[i]);
    }
}
