#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "http.h"
#include "itoa.h"

// Original Source by Antonio Asaro
// https://github.com/antonioasaro/Antonio-Mail_to_SMS

#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x04, 0x9F, 0x49, 0xC0, 0x99, 0xAD }
PBL_APP_INFO(HTTP_UUID,
    "wemo999", "dezign999 / Antonio Asaro",
    1, 0, /* App version */
    RESOURCE_ID_MENU_ICON,
    APP_INFO_STANDARD_APP);

///////////  personalize this part  ///////////

#define TOTAL_NAM 2 //Number of Wemo Switches
const char *nam_list[] = {"WemoSwitchName",    "WemoSwitchName",    "WemoSwitchName"};
//add your wemo switch names to the line above separated by a comma except for the last 
//entry as shown in the example. Limit 10 characters. Ex - "Livingroom"

const char *who_list[] = {"trigger@ifttt.com", "trigger@ifttt.com", "trigger@ifttt.com"};
//add "trigger@ifttt.com" address for each Wemo Switch in the line above separated by a 
//comma except for the last entry as shown in the example.

///////////////////////////////////////////////

#define TOTAL_MSG 3
int TOTAL_WHO = TOTAL_NAM;

const char *msg_list[] = {"Toggle", "On", "Off"};
const char *tmp_list[] = {"Toggle", "On", "Off"};

BmpContainer background_image_container;

#define MAIL_TO_SMS_COOKIE   359997
#define TIME_DELAY 5000
	
Window mainWindow;
TextLayer whoLayer, msgLayer, cmdLayer;
int nam_sel = 0;
int who_sel = 0;
int msg_sel = 0;
static char nam_text[64];
static char msg_text[64];

void request_mail_to_sms() {
	DictionaryIterator *body;
	static char nam[64];
	static char who[64];
	static char msg[64];
	static char url[256];
	
	///////////  personalize this part  ///////////
	strcpy(url, "https://github.com/focustom/wemotoggle.git/wemo.php?cmd=send");
	///////////////////////////////////////////////
	strcpy(nam, "&nam="); strcat(nam, nam_list[nam_sel]); 
	strcpy(who, "&who="); strcat(who, who_list[who_sel]); 
	strcpy(msg, "&msg="); strcat(msg, msg_list[msg_sel]);
	strcat(url, nam);strcat(url, who); strcat(url, msg);
	if (http_out_get(url, false, MAIL_TO_SMS_COOKIE, &body) != HTTP_OK ||
		http_out_send() != HTTP_OK) {
	}
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	who_sel++; if (who_sel == TOTAL_WHO) who_sel = 0;
	nam_sel++; if (nam_sel == TOTAL_NAM) nam_sel = 0;
	strcpy(nam_text, ""); strcat(nam_text, nam_list[who_sel]);
	text_layer_set_text(&whoLayer, nam_text);
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	//    text_layer_set_text(&cmdLayer, "select_single_click");
}

void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
	text_layer_set_text(&cmdLayer, "Sending");
	request_mail_to_sms();
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	msg_sel++; if (msg_sel == TOTAL_MSG) msg_sel = 0;
	strcpy(msg_text, ""); strcat(msg_text, tmp_list[msg_sel]);
	text_layer_set_text(&msgLayer, msg_text);
}

void config_provider(ClickConfig **config, Window *window) {
	// single click / repeat-on-hold config:
	config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) select_single_click_handler;
	config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
	config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) down_single_click_handler;
	
	// long click config:
	config[BUTTON_ID_DOWN]->long_click.handler = (ClickHandler) select_long_click_handler;
	config[BUTTON_ID_UP]->click.repeat_interval_ms = 500;
	config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 500;
	config[BUTTON_ID_DOWN]->long_click.delay_ms = 700;
}

void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
	text_layer_set_text(&cmdLayer, "Send");
}

void failure(int32_t cookie, int http_status, void *ctx) {
	text_layer_set_text(&cmdLayer, "Tried");
	if (cookie == MAIL_TO_SMS_COOKIE) {
		text_layer_set_text(&cmdLayer, "Failed");
	}
	app_timer_send_event(ctx, TIME_DELAY, 42); 
}

void success(int32_t cookie, int http_status, DictionaryIterator *dict, void *ctx) {
	if (cookie == MAIL_TO_SMS_COOKIE) {
		text_layer_set_text(&cmdLayer, "Completed");
	}
	app_timer_send_event(ctx, TIME_DELAY, 42); 
}

void handle_init(AppContextRef ctx) {
	
	//Main Window init :: BEGIN
	window_init(&mainWindow, "Status");
	window_stack_push(&mainWindow, true /* Animated */);
	window_set_fullscreen(&mainWindow, true);

	
	  resource_init_current_app(&VERSION);
	
		// Set up a layer for the static watch face background
	bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image_container);
	layer_add_child(&mainWindow.layer, &background_image_container.layer.layer);
	
	text_layer_init(&whoLayer, GRect(19, 10,  100, 60));
	layer_add_child(&mainWindow.layer, &whoLayer.layer);
	text_layer_set_background_color(&whoLayer, GColorClear);
	text_layer_set_font(&whoLayer,fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(&whoLayer, GTextAlignmentCenter);
	
	text_layer_init(&msgLayer, GRect(19, 64,  100, 60));
	layer_add_child(&mainWindow.layer, &msgLayer.layer);
	text_layer_set_background_color(&msgLayer, GColorClear);
	text_layer_set_font(&msgLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(&msgLayer, GTextAlignmentCenter);
	
	text_layer_init(&cmdLayer, GRect(19, 119, 100, 40));
	layer_add_child(&mainWindow.layer, &cmdLayer.layer);
	text_layer_set_background_color(&cmdLayer, GColorClear);
	text_layer_set_font(&cmdLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(&cmdLayer, GTextAlignmentCenter);

	
	window_set_click_config_provider(&mainWindow, (ClickConfigProvider) config_provider);
	//Main Window init :: END
	
	//Main Window init :: BEGIN
	strcpy(nam_text, ""); strcat(nam_text, nam_list[who_sel]);
	strcpy(msg_text, ""); strcat(msg_text, tmp_list[msg_sel]);
	text_layer_set_text(&whoLayer, nam_text);
	text_layer_set_text(&cmdLayer, "Send");
	text_layer_set_text(&msgLayer, msg_text);
	
	http_set_app_id(39152173);
	http_register_callbacks((HTTPCallbacks){
		.success=success,
		.failure=failure
	}, (void*)ctx);
	
}

void handle_deinit(AppContextRef ctx) {
	(void)ctx;
	bmp_deinit_container(&background_image_container);
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.timer_handler = handle_timer,
		.deinit_handler = &handle_deinit,
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 256,
				.outbound = 256,
			}
		},
	};
	app_event_loop(params, &handlers);
}
