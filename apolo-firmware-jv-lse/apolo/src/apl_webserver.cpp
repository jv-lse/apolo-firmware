#include <string.h>
#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"              
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "apl_wifi.hpp"
#include "apl_storage.hpp"
#include "apl_camera.hpp"
#include "apl_sdcard.hpp"

#include "apl_task_led.hpp"



#define APOLO_SERVER_CHANNEL                    (4)
#define APOLO_SERVER_MAX_CONNECTIONS_ALLOWED    (2)
#define MAX_STRING_LENGTH_TO_TEXT_ENTRY         (32)
#define WEBSERVER_HTTP_PAGE_STACK_SIZE          (4096*3)

#define MODO_CADASTRO

#define DECIMAL_BASE (10)

static const char *APL_WIFI_SOFTAP_TAG = "ACCESS_POINT";
static const char *APL_WEBSERVER_TAG = "WEBSERVER";

static const char *APL_WEBSERVER_TAG_UART_CONFIG = "WEBSERVER_UART_ID";

static const std::string APOLO_SERVER_PASS = "apolo_pass";
static const std::string APOLO_SERVER_SSID = "APOLO_MIAS";

static const std::string WAKE_UP_INTERVAL_FIELD = "wake_up_interval_field";
static const std::string SERVER_ADDR_FIELD = "server_addr_field";
static const std::string SSID_FIELD = "ssid_field";
static const std::string PASSWORD_FIELD = "password_field";

static std::string device_id;

static const std::string boilerplate_img = "/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAcFBQYFBAcGBQYIBwcIChELCgkJChUPEAwRGBUaGRgVGBcbHichGx0lHRcYIi4iJSgpKywrGiAvMy8qMicqKyr/2wBDAQcICAoJChQLCxQqHBgcKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKir/wAARCACXAaEDASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD6RooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiigAooooAKKKKACiiig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";

static std::string end_string_webserver = "\">\
<div class=\"vertical-form-item-label\">\
                  <form  class=\"form-image\" action=\"/get/take_picture\">\
                  <button  class=\"button7\">\
                    <div class=\"icon-wrapper11\">\
                      <img\
                        class=\"wrapper-icon11\"\
                        alt=\"\"\
                        src=\"./public/wrapper-11@2x.png\"\
                      />\
                    </div>\
                    <div class=\"title-wrapper\">Capturar Imagem</div>\
                  </button>\
                  </form>\
                </div>\
              </div>\
            </div>\
          </div>\
        </section>\
        <div class=\"adjuster\">\
          <div class=\"ignore2\"></div>\
          <div class=\"adjust-btn\">\
            <div class=\"ignore3\"></div>\
          </div>\
        </div>\
      </main>\
    </div>\
</body>\
</html>";





static std::string initial_string_webserver =  "<!DOCTYPE html>\
<html>\
<head>\
	<meta charset=\"UTF-8\">\
	<title>CONFIGURAÇÃO DE DISPOSITIVO</title>\
	<style type=\"text/css\">\
      body {\
      margin: 0;\
      line-height: normal;\
    }\
    :root {\
      /* fonts */\
      --body-regular: Roboto;\
      /* font sizes */\
      --body-regular-size: 14px;\
      --h4-medium-size: 20px;\
      /* Colors */\
      --neutral-1: #fff;\
      --primary-6: #1890ff;\
      --neutral-2: #fafafa;\
      --character-secondary-45: rgba(0, 0, 0, 0.45);\
      --character-title-85: rgba(0, 0, 0, 0.85);\
      --conditional-divider: rgba(0, 0, 0, 0.06);\
      --hitbox: rgba(255, 255, 255, 0);\
      --color-gainsboro: #d9d9d9;\
      /* Gaps */\
      --gap-xl: 20px;\
      --gap-3xs: 10px;\
      --gap-mini-6: 14.6px;\
      --gap-9xs: 4px;\
      --gap-5xs: 8px;\
      /* Paddings */\
      --padding-5xl: 24px;\
      --padding-7xs: 6px;\
      --padding-8xs: 5px;\
      --padding-xl: 20px;\
      --padding-9xs: 4px;\
      --padding-mini: 15px;\
      --padding-5xs: 8px;\
      --padding-4xs: 9px;\
      --padding-2xs: 11px;\
      --padding-xs: 12px;\
      --padding-base: 16px;\
      /* Border radiuses */\
      --br-11xs: 2px;\
      /* Effects */\
      --drop-shadow-button-secondary: 0px 2px 0px rgba(0, 0, 0, 0.02);\
      --border-divider-divider: 0px -1px 0px #f0f0f0 inset;\
      --drop-shadow-button-primary: 0px 2px 0px rgba(0, 0, 0, 0.04);\
    }\
		.quadrado {\
			width: 600px;\
			padding: 20px;\
			background-color: #e0f0ff;\
			border-radius: 10px;\
			display: flex;\
			justify-content: space-between;\
			border: 2px solid black;\
		}\
		.campos-container {\
			width: 45%;\
			text-align: left;\
		}\
		.quadrado-acima {\
			width: 50%;\
			height: 226px;\
			background-color: #ffffff;\
			border-radius: 0;\
			margin-bottom: 10px;\
			margin-top: 65px;\
			border: 2px solid black;\
			display: flex;\
			justify-content: center;\
			align-items: center;\
			flex-direction: column;\
			position: relative;\
		}\
		.quadrado-acima img {\
			max-width: 100%;\
			min-height: 100%;\
		}\
		.quadrado-acima>p {\
			position: absolute;\
			top: -40px;\
			left: 48%;\
			transform: translateX(-50%);\
		}\
		h1,\
		p,\
		span,\
		input {\
			font-family: Verdana, Geneva, sans-serif;\
			font-size: 14px;\
		}\
		input {\
			display: block;\
			width: 100%;\
			margin-bottom: 10px;\
			border: 2px solid black;\
		}\
		input[type=\"submit\"] {\
			width: auto;\
			padding: 5px 15px;\
			background-color: #4CAF50;\
			color: white;\
			border: none;\
			border-radius: 4px;\
			cursor: pointer;\
		}\
		input[type=\"file\"] {\
			margin-top: 20px;\
		}\
		.button-container {\
			display: flex;\
			justify-content: space-between;\
			margin-top: 10px;\
			margin-right: 100px;\
		}\
		.sep-int-wakeup {\
			margin-right: 1px;\
			align-items: center;\
		}\
		.error-message {\
			color: red;\
			font-size: 12px;\
			margin-top: 5px;\
		}\
		.st-item-label {\
	align-self: stretch;\
	position: relative;\
	line-height: 22px;\
	display: flex;\
	align-items: center;\
	white-space: nowrap;\
	}\
	.st-item {\
	height: 22px;\
	display: none;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.separator {\
	position: relative;\
	line-height: 22px;\
	}\
	.componentsseparator {\
	overflow: hidden;\
	display: none;\
	flex-direction: column;\
	align-items: center;\
	justify-content: flex-start;\
	padding: 0 var(--padding-5xs);\
	}\
	.last-item-label {\
	align-self: stretch;\
	position: relative;\
	line-height: 22px;\
	display: flex;\
	align-items: center;\
	white-space: nowrap;\
	}\
	.breadcrumb,\
	.last-item {\
	display: none;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.last-item {\
	height: 22px;\
	color: var(--character-title-85);\
	}\
	.breadcrumb {\
	width: 343px;\
	overflow: hidden;\
	max-width: 100%;\
	}\
	.configurao-dispositivo,\
	.device-id-deviceid {\
	margin: 0;\
	}\
	.configurao-dispositivo-devic-container {\
	margin: 0;\
	position: relative;\
	font-size: inherit;\
	line-height: 28px;\
	font-weight: 500;\
	font-family: inherit;\
	}\
	.heading-left,\
	.wrapper {\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.wrapper {\
	padding: var(--padding-7xs) 0;\
	}\
	.heading-left {\
	height: 48px;\
	}\
	.wrapper-icon {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper {\
	height: 14px;\
	background-color: var(--hitbox);\
	overflow: hidden;\
	display: flex;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	}\
	.heading-extra1 {\
	width: 39px;\
	position: relative;\
	font-size: var(--body-regular-size);\
	line-height: 22px;\
	font-family: var(--body-regular);\
	color: var(--neutral-1);\
	text-align: center;\
	display: inline-block;\
	min-width: 39px;\
	}\
	.button {\
	cursor: pointer;\
	border: 1px solid var(--primary-6);\
	padding: var(--padding-8xs) var(--padding-mini);\
	background-color: var(--primary-6);\
	flex: 1;\
	border-radius: var(--br-11xs);\
	box-shadow: var(--drop-shadow-button-primary);\
	overflow: hidden;\
	justify-content: center;\
	gap: var(--gap-5xs);\
	}\
	.button,\
	.heading,\
	.heading-extra {\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	}\
	.heading-extra {\
	width: 93px;\
	justify-content: flex-start;\
	padding: var(--padding-9xs) 0;\
	box-sizing: border-box;\
	}\
	.heading {\
	align-self: stretch;\
	justify-content: space-between;\
	padding: 0 0 var(--padding-9xs);\
	gap: var(--gap-xl);\
	font-size: var(--h4-medium-size);\
	color: var(--character-title-85);\
	}\
	.advanced-forms-are {\
	align-self: stretch;\
	position: relative;\
	line-height: 131%;\
	white-space: nowrap;\
	}\
	.ignore,\
	.ignore1 {\
	height: 0;\
	width: 0;\
	position: relative;\
	}\
	.width-scrubber {\
	width: 400px;\
	height: 0;\
	overflow: hidden;\
	flex-shrink: 0;\
	display: flex;\
	flex-direction: row;\
	align-items: flex-start;\
	justify-content: space-between;\
	max-width: 100%;\
	gap: var(--gap-xl);\
	}\
	.content,\
	.head,\
	.page-headerlegacy {\
	display: flex;\
	flex-direction: column;\
	align-items: flex-start;\
	justify-content: flex-start;\
	}\
	.content {\
	align-self: stretch;\
	overflow: hidden;\
	max-width: 100%;\
	font-size: 13px;\
	color: #333;\
	}\
	.head,\
	.page-headerlegacy {\
	background-color: var(--neutral-1);\
	padding: var(--padding-base) var(--padding-5xl);\
	box-sizing: border-box;\
	font-family: var(--body-regular);\
	}\
	.page-headerlegacy {\
	margin-left: -32px;\
	width: 1456px;\
	gap: var(--gap-5xs);\
	max-width: 105%;\
	flex-shrink: 0;\
	text-align: left;\
	font-size: var(--body-regular-size);\
	color: var(--character-secondary-45);\
	}\
	.head {\
	width: 100%;\
	border: 0;\
	outline: 0;\
	align-self: stretch;\
	height: 56px;\
	box-shadow: var(--border-divider-divider);\
	font-weight: 500;\
	font-size: 16px;\
	color: var(--character-title-85);\
	min-width: 250px;\
	}\
	.aspect-ratio-keeper-rotated {\
	align-self: stretch;\
	height: 0;\
	transform: rotate(-38.7deg);\
	}\
	.cover {\
	width: 240px;\
	display: none;\
	flex-direction: column;\
	align-items: flex-start;\
	justify-content: flex-start;\
	background-image: url(./public/cover@3x.png);\
	background-size: cover;\
	background-repeat: no-repeat;\
	background-position: top;\
	}\
	.wrapper-icon1 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper1 {\
	height: 14px;\
	background-color: var(--hitbox);\
	overflow: hidden;\
	display: none;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	}\
	.text {\
	align-self: stretch;\
	position: relative;\
	line-height: 22px;\
	}\
	.button1 {\
	height: 30px;\
	width: 368px;\
	border-radius: var(--br-11xs);\
	overflow: hidden;\
	display: none;\
	flex-direction: row;\
	align-items: center;\
	justify-content: center;\
	padding: var(--padding-9xs) var(--padding-mini);\
	box-sizing: border-box;\
	gap: var(--gap-3xs);\
	max-width: 100%;\
	z-index: 4;\
	}\
	.title {\
	flex: 1;\
	position: relative;\
	line-height: 22px;\
	display: inline-block;\
	max-width: 100%;\
	}\
	.label {\
	align-self: stretch;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	padding: 0 0 var(--padding-5xs);\
	box-sizing: border-box;\
	max-width: 100%;\
	}\
	.text1 {\
	position: relative;\
	line-height: 22px;\
	}\
	.input-addonlabel,\
	.wrapper1 {\
	overflow: hidden;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.wrapper1 {\
	height: 30px;\
	display: flex;\
	}\
	.input-addonlabel {\
	align-self: stretch;\
	border-radius: var(--br-11xs) 0 0 var(--br-11xs);\
	background-color: var(--neutral-2);\
	border-top: 1px solid var(--color-gainsboro);\
	border-bottom: 1px solid var(--color-gainsboro);\
	border-left: 1px solid var(--color-gainsboro);\
	display: none;\
	padding: 0 var(--padding-xs);\
	}\
	.input-prefix-icon {\
	height: 14px;\
	width: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	display: none;\
	}\
	.input1 {\
	width: 100%;\
	border: 0;\
	outline: 0;\
	font-family: var(--body-regular);\
	font-size: var(--body-regular-size);\
	background-color: transparent;\
	height: 22px;\
	flex: 1;\
	position: relative;\
	line-height: 22px;\
	color: var(--character-title-85);\
	text-align: left;\
	display: flex;\
	align-items: center;\
	min-width: 186px;\
	padding: 0;\
	}\
	.input-suffix-icon {\
	height: 14px;\
	width: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	display: none;\
	}\
	.input {\
	flex: 1;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	border: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	padding: var(--padding-8xs) var(--padding-2xs);\
	gap: var(--gap-9xs);\
	max-width: 100%;\
	}\
	.wrapper-icon2 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper2,\
	.input-addonicon {\
	overflow: hidden;\
	align-items: center;\
	justify-content: center;\
	}\
	.icon-wrapper2 {\
	height: 14px;\
	background-color: var(--hitbox);\
	display: flex;\
	flex-direction: column;\
	}\
	.input-addonicon {\
	width: 38px;\
	border-radius: 0 var(--br-11xs) var(--br-11xs) 0;\
	background-color: var(--neutral-2);\
	border-top: 1px solid var(--color-gainsboro);\
	border-right: 1px solid var(--color-gainsboro);\
	border-bottom: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	flex-shrink: 0;\
	display: none;\
	flex-direction: row;\
	padding: var(--padding-4xs) var(--padding-2xs);\
	}\
	.field,\
	.vertical-form-iteminput {\
	display: flex;\
	justify-content: flex-start;\
	box-sizing: border-box;\
	max-width: 100%;\
	}\
	.field {\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	border: 1px solid var(--color-gainsboro);\
	overflow: hidden;\
	flex-direction: row;\
	align-items: center;\
	text-align: center;\
	}\
	.vertical-form-iteminput {\
	flex: 1;\
	flex-direction: column;\
	align-items: flex-start;\
	padding: 0 0 var(--padding-5xl);\
	min-width: 217px;\
	}\
	.title1 {\
	flex: 1;\
	position: relative;\
	line-height: 22px;\
	display: inline-block;\
	max-width: 100%;\
	}\
	.label1 {\
	align-self: stretch;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	padding: 0 0 var(--padding-5xs);\
	box-sizing: border-box;\
	max-width: 100%;\
	}\
	.text2 {\
	position: relative;\
	line-height: 22px;\
	}\
	.input-addonlabel1,\
	.wrapper2 {\
	overflow: hidden;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.wrapper2 {\
	height: 30px;\
	display: flex;\
	}\
	.input-addonlabel1 {\
	align-self: stretch;\
	border-radius: var(--br-11xs) 0 0 var(--br-11xs);\
	background-color: var(--neutral-2);\
	border-top: 1px solid var(--color-gainsboro);\
	border-bottom: 1px solid var(--color-gainsboro);\
	border-left: 1px solid var(--color-gainsboro);\
	display: none;\
	padding: 0 var(--padding-xs);\
	}\
	.input-prefix-icon1 {\
	height: 14px;\
	width: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	display: none;\
	}\
	.input3 {\
	width: 100%;\
	border: 0;\
	outline: 0;\
	font-family: var(--body-regular);\
	font-size: var(--body-regular-size);\
	background-color: transparent;\
	height: 22px;\
	flex: 1;\
	position: relative;\
	line-height: 22px;\
	color: var(--character-title-85);\
	text-align: left;\
	display: flex;\
	align-items: center;\
	min-width: 186px;\
	padding: 0;\
	}\
	.input-suffix-icon1 {\
	height: 14px;\
	width: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	display: none;\
	}\
	.input2 {\
	flex: 1;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	border: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	padding: var(--padding-8xs) var(--padding-2xs);\
	gap: var(--gap-9xs);\
	max-width: 100%;\
	}\
	.wrapper-icon3 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper3,\
	.input-addonicon1 {\
	overflow: hidden;\
	align-items: center;\
	justify-content: center;\
	}\
	.icon-wrapper3 {\
	height: 14px;\
	background-color: var(--hitbox);\
	display: flex;\
	flex-direction: column;\
	}\
	.input-addonicon1 {\
	width: 38px;\
	border-radius: 0 var(--br-11xs) var(--br-11xs) 0;\
	background-color: var(--neutral-2);\
	border-top: 1px solid var(--color-gainsboro);\
	border-right: 1px solid var(--color-gainsboro);\
	border-bottom: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	flex-shrink: 0;\
	display: none;\
	flex-direction: row;\
	padding: var(--padding-4xs) var(--padding-2xs);\
	}\
	.field1,\
	.label-divider,\
	.vertical-form-iteminput1 {\
	display: flex;\
	justify-content: flex-start;\
	max-width: 100%;\
	}\
	.field1 {\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	border: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	overflow: hidden;\
	flex-direction: row;\
	align-items: center;\
	text-align: center;\
	}\
	.label-divider,\
	.vertical-form-iteminput1 {\
	align-items: flex-start;\
	}\
	.vertical-form-iteminput1 {\
	flex: 1;\
	flex-direction: column;\
	padding: 0 0 var(--padding-5xl);\
	box-sizing: border-box;\
	min-width: 217px;\
	}\
	.label-divider {\
	width: 100%;\
	margin: 0 !important;\
	position: absolute;\
	top: 0;\
	left: 0;\
	flex-direction: row;\
	gap: var(--gap-mini-6);\
	text-align: left;\
	color: var(--character-title-85);\
	}\
	.title2 {\
	flex: 1;\
	position: relative;\
	line-height: 22px;\
	display: inline-block;\
	max-width: 100%;\
	}\
	.label2 {\
	align-self: stretch;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	padding: 0 0 var(--padding-5xs);\
	box-sizing: border-box;\
	max-width: 100%;\
	}\
	.text3 {\
	position: relative;\
	line-height: 22px;\
	}\
	.input-addonlabel2,\
	.wrapper3 {\
	overflow: hidden;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.wrapper3 {\
	height: 30px;\
	display: flex;\
	}\
	.input-addonlabel2 {\
	align-self: stretch;\
	border-radius: var(--br-11xs) 0 0 var(--br-11xs);\
	background-color: var(--neutral-2);\
	border-top: 1px solid var(--color-gainsboro);\
	border-bottom: 1px solid var(--color-gainsboro);\
	border-left: 1px solid var(--color-gainsboro);\
	display: none;\
	padding: 0 var(--padding-xs);\
	}\
	.input-prefix-icon2 {\
	height: 14px;\
	width: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	display: none;\
	}\
	.input5 {\
	width: 100%;\
	border: 0;\
	outline: 0;\
	font-family: var(--body-regular);\
	font-size: var(--body-regular-size);\
	background-color: transparent;\
	height: 22px;\
	flex: 1;\
	position: relative;\
	line-height: 22px;\
	color: var(--character-title-85);\
	text-align: left;\
	display: flex;\
	align-items: center;\
	min-width: 186px;\
	padding: 0;\
	}\
	.input-suffix-icon2 {\
	height: 14px;\
	width: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	display: none;\
	}\
	.input4 {\
	flex: 1;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	border: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	padding: var(--padding-8xs) var(--padding-2xs);\
	gap: var(--gap-9xs);\
	max-width: 100%;\
	}\
	.wrapper-icon4 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper4,\
	.input-addonicon2 {\
	overflow: hidden;\
	align-items: center;\
	justify-content: center;\
	}\
	.icon-wrapper4 {\
	height: 14px;\
	background-color: var(--hitbox);\
	display: flex;\
	flex-direction: column;\
	}\
	.input-addonicon2 {\
	width: 38px;\
	border-radius: 0 var(--br-11xs) var(--br-11xs) 0;\
	background-color: var(--neutral-2);\
	border-top: 1px solid var(--color-gainsboro);\
	border-right: 1px solid var(--color-gainsboro);\
	border-bottom: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	flex-shrink: 0;\
	display: none;\
	flex-direction: row;\
	padding: var(--padding-4xs) var(--padding-2xs);\
	}\
	.field2,\
	.vertical-form-iteminput2 {\
	display: flex;\
	justify-content: flex-start;\
	box-sizing: border-box;\
	max-width: 100%;\
	}\
	.field2 {\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	border: 1px solid var(--color-gainsboro);\
	overflow: hidden;\
	flex-direction: row;\
	align-items: center;\
	text-align: center;\
	}\
	.vertical-form-iteminput2 {\
	flex: 1;\
	flex-direction: column;\
	align-items: flex-start;\
	padding: 0 0 var(--padding-5xl);\
	min-width: 217px;\
	}\
	.title3 {\
	flex: 1;\
	position: relative;\
	line-height: 22px;\
	display: inline-block;\
	max-width: 100%;\
	}\
	.label3 {\
	align-self: stretch;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	padding: 0 0 var(--padding-5xs);\
	box-sizing: border-box;\
	max-width: 100%;\
	}\
	.text4 {\
	position: relative;\
	line-height: 22px;\
	}\
	.input-addonlabel3,\
	.wrapper4 {\
	overflow: hidden;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.wrapper4 {\
	height: 30px;\
	display: flex;\
	}\
	.input-addonlabel3 {\
	align-self: stretch;\
	border-radius: var(--br-11xs) 0 0 var(--br-11xs);\
	background-color: var(--neutral-2);\
	border-top: 1px solid var(--color-gainsboro);\
	border-bottom: 1px solid var(--color-gainsboro);\
	border-left: 1px solid var(--color-gainsboro);\
	display: none;\
	padding: 0 var(--padding-xs);\
	}\
	.input-prefix-icon3 {\
	height: 14px;\
	width: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	display: none;\
	}\
	.input7 {\
	width: 100%;\
	border: 0;\
	outline: 0;\
	font-family: var(--body-regular);\
	font-size: var(--body-regular-size);\
	background-color: transparent;\
	height: 22px;\
	flex: 1;\
	position: relative;\
	line-height: 22px;\
	color: var(--character-title-85);\
	text-align: left;\
	display: flex;\
	align-items: center;\
	min-width: 186px;\
	padding: 0;\
	}\
	.input-suffix-icon3 {\
	height: 14px;\
	width: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	display: none;\
	}\
	.input6 {\
	flex: 1;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	border: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	padding: var(--padding-8xs) var(--padding-2xs);\
	gap: var(--gap-9xs);\
	max-width: 100%;\
	}\
	.wrapper-icon5 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper5,\
	.input-addonicon3 {\
	overflow: hidden;\
	align-items: center;\
	justify-content: center;\
	}\
	.icon-wrapper5 {\
	height: 14px;\
	background-color: var(--hitbox);\
	display: flex;\
	flex-direction: column;\
	}\
	.input-addonicon3 {\
	width: 38px;\
	border-radius: 0 var(--br-11xs) var(--br-11xs) 0;\
	background-color: var(--neutral-2);\
	border-top: 1px solid var(--color-gainsboro);\
	border-right: 1px solid var(--color-gainsboro);\
	border-bottom: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	flex-shrink: 0;\
	display: none;\
	flex-direction: row;\
	padding: var(--padding-4xs) var(--padding-2xs);\
	}\
	.component-2,\
	.field3,\
	.vertical-form-iteminput3 {\
	display: flex;\
	justify-content: flex-start;\
	max-width: 100%;\
	}\
	.field3 {\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	border: 1px solid var(--color-gainsboro);\
	box-sizing: border-box;\
	overflow: hidden;\
	flex-direction: row;\
	align-items: center;\
	text-align: center;\
	}\
	.component-2,\
	.vertical-form-iteminput3 {\
	align-items: flex-start;\
	}\
	.vertical-form-iteminput3 {\
	flex: 1;\
	flex-direction: column;\
	padding: 0 0 var(--padding-5xl);\
	box-sizing: border-box;\
	min-width: 217px;\
	}\
	.component-2 {\
	width: 100%;\
	margin: 0 !important;\
	position: absolute;\
	top: 102px;\
	left: 0;\
	flex-direction: row;\
	gap: var(--gap-mini-6);\
	text-align: left;\
	color: var(--character-title-85);\
	}\
	.wrapper-icon6 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper6 {\
	height: 14px;\
	background-color: var(--hitbox);\
	overflow: hidden;\
	display: none;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	}\
	.text5 {\
	position: relative;\
	line-height: 22px;\
	}\
	.button2 {\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	overflow: hidden;\
	display: none;\
	flex-direction: row;\
	align-items: center;\
	justify-content: center;\
	padding: var(--padding-9xs) var(--padding-mini);\
	gap: var(--gap-3xs);\
	}\
	.line-icon {\
	height: 12px;\
	width: 1px;\
	position: relative;\
	}\
	.divider {\
	height: 12px;\
	background-color: var(--conditional-divider);\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.layoutblockshorizontal5 {\
	align-self: stretch;\
	display: flex;\
	flex-direction: row;\
	align-items: flex-start;\
	justify-content: center;\
	padding: 0 var(--padding-xl);\
	}\
	.wrapper-icon7 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper7 {\
	height: 14px;\
	background-color: var(--hitbox);\
	overflow: hidden;\
	display: none;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	}\
	.text6 {\
	width: 133px;\
	position: relative;\
	font-size: var(--body-regular-size);\
	line-height: 22px;\
	font-family: var(--body-regular);\
	color: var(--primary-6);\
	text-align: center;\
	display: inline-block;\
	}\
	.button3,\
	.label-divider1 {\
	display: flex;\
	align-items: flex-start;\
	gap: var(--gap-3xs);\
	}\
	.button3 {\
	cursor: pointer;\
	border: 1px solid var(--primary-6);\
	padding: var(--padding-8xs) var(--padding-xl);\
	background-color: var(--neutral-1);\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	box-shadow: var(--drop-shadow-button-secondary);\
	flex-direction: row;\
	justify-content: center;\
	}\
	.label-divider1 {\
	width: 333px;\
	margin: 0 !important;\
	position: absolute;\
	top: 204px;\
	left: 0;\
	flex-direction: column;\
	justify-content: flex-start;\
	}\
	.wrapper-icon8 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper8 {\
	height: 14px;\
	background-color: var(--hitbox);\
	overflow: hidden;\
	display: none;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	}\
	.text7 {\
	position: relative;\
	line-height: 22px;\
	}\
	.button4 {\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	overflow: hidden;\
	display: none;\
	flex-direction: row;\
	align-items: center;\
	justify-content: center;\
	padding: var(--padding-9xs) var(--padding-mini);\
	gap: var(--gap-3xs);\
	}\
	.line-icon1 {\
	height: 12px;\
	width: 1px;\
	position: relative;\
	}\
	.divider1 {\
	height: 12px;\
	background-color: var(--conditional-divider);\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: flex-start;\
	}\
	.layoutblockshorizontal51 {\
	align-self: stretch;\
	display: flex;\
	flex-direction: row;\
	align-items: flex-start;\
	justify-content: center;\
	padding: 0 var(--padding-xl);\
	}\
	.wrapper-icon9 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper9 {\
	height: 14px;\
	background-color: var(--hitbox);\
	overflow: hidden;\
	display: none;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	}\
	.text8 {\
	width: 133px;\
	position: relative;\
	font-size: var(--body-regular-size);\
	line-height: 22px;\
	font-family: var(--body-regular);\
	color: var(--primary-6);\
	text-align: center;\
	display: inline-block;\
	}\
	.button5 {\
	cursor: pointer;\
	border: 1px solid var(--primary-6);\
	padding: var(--padding-8xs) var(--padding-xl);\
	background-color: var(--neutral-1);\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	box-shadow: var(--drop-shadow-button-secondary);\
	display: flex;\
	flex-direction: row;\
	align-items: flex-start;\
	justify-content: center;\
	gap: var(--gap-3xs);\
	}\
	.field-type,\
	.label-divider2 {\
	display: flex;\
	align-items: flex-start;\
	justify-content: flex-start;\
	}\
	.label-divider2 {\
	width: 333px;\
	margin: 0 !important;\
	position: absolute;\
	top: 204px;\
	left: 349px;\
	flex-direction: column;\
	gap: var(--gap-3xs);\
	}\
	.field-type {\
	height: 700px;\
	width: 682px;\
	flex-direction: row;\
	flex-wrap: wrap;\
	position: relative;\
	gap: 16px 14px;\
	min-width: 682px;\
	max-width: 100%;\
	}\
	.title4 {\
	position: relative;\
	line-height: 22px;\
	display: inline-block;\
	min-width: 123px;\
	}\
	.label4 {\
	align-self: stretch;\
	overflow: hidden;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: center;\
	padding: 0 0 var(--padding-5xs);\
	}\
	.aspect-ratio-keeper-addition {\
	align-self: stretch;\
	flex: 1;\
	max-width: 100%;\
	overflow: hidden;\
	max-height: 100%;\
	object-fit: contain;\
  border: 2px solid #d9d9d9\
	}\
	.field4 {\
	align-self: stretch;\
	flex: 1;\
	display: flex;\
	flex-direction: column;\
	align-items: flex-start;\
	justify-content: flex-start;\
	background-size: cover;\
	background-repeat: no-repeat;\
	background-position: top;\
  align-items:center;\
  justify-content: center;\
	}\
	.wrapper-icon10 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper10 {\
	height: 14px;\
	background-color: var(--hitbox);\
	overflow: hidden;\
	display: none;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	}\
	.text9 {\
	position: relative;\
	line-height: 22px;\
	}\
	.button6 {\
	width: 368px;\
	border-radius: var(--br-11xs);\
	overflow: hidden;\
	display: none;\
	flex-direction: row;\
	align-items: center;\
	justify-content: center;\
	padding: var(--padding-9xs) var(--padding-mini);\
	box-sizing: border-box;\
	gap: var(--gap-3xs);\
	max-width: 100%;\
	}\
	.line-icon2 {\
	height: 12px;\
	width: 1px;\
	position: relative;\
	}\
	.divider2,\
	.layoutblockshorizontal52 {\
  display: none;\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	}\
	.divider2 {\
	height: 12px;\
	background-color: var(--conditional-divider);\
	justify-content: flex-start;\
	padding: 0;\
	box-sizing: border-box;\
	}\
	.layoutblockshorizontal52 {\
	align-self: stretch;\
	justify-content: center;\
	padding: 0 var(--padding-xl);\
	}\
	.wrapper-icon11 {\
	width: 14px;\
	height: 14px;\
	position: relative;\
	overflow: hidden;\
	flex-shrink: 0;\
	object-fit: cover;\
	}\
	.icon-wrapper11 {\
	height: 14px;\
	background-color: var(--hitbox);\
	overflow: hidden;\
	display: none;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	}\
	.title-wrapper {\
	width: 109px;\
	position: relative;\
	font-size: var(--body-regular-size);\
	line-height: 22px;\
	font-family: var(--body-regular);\
	color: var(--primary-6);\
	text-align: center;\
	display: inline-block;\
	min-width: 109px;\
	}\
	.button7 {\
  width: 100%;\
	cursor: pointer;\
	border: 1px solid var(--primary-6);\
	padding: var(--padding-8xs) var(--padding-xl);\
	background-color: var(--neutral-1);\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	box-shadow: var(--drop-shadow-button-secondary);\
	display: flex;\
	flex-direction: row;\
	align-items: center;\
	justify-content: center;\
	gap: var(--gap-3xs);\
	}\
	.vertical-form-item-label,\
	.vertical-form-itemselect {\
	display: flex;\
	flex-direction: column;\
	align-items: center;\
	justify-content: center;\
	max-width: 100%;\
	}\
	.vertical-form-item-label {\
	align-self: stretch;\
	gap: var(--gap-3xs);\
	text-align: center;\
	color: var(--primary-6);\
	}\
	.vertical-form-itemselect {\
	height: 500px;\
	flex: 1;\
	padding: 0 0 var(--padding-5xl);\
	box-sizing: border-box;\
	}\
	.field-type1 {\
	width: 646px;\
	min-width: 420px;\
	text-align: left;\
	color: var(--character-title-85);\
	}\
	.card-legacy-header,\
	.card-legacy-header-wrapper,\
	.field-type1 {\
	display: flex;\
	flex-direction: row;\
	align-items: flex-start;\
	justify-content: flex-start;\
	max-width: 100%;\
	}\
	.card-legacy-header {\
	flex: 1;\
	gap: 16px;\
	}\
	.card-legacy-header-wrapper {\
	align-self: stretch;\
	padding: 0 var(--padding-5xl);\
	box-sizing: border-box;\
	text-align: center;\
	font-size: var(--body-regular-size);\
	color: var(--primary-6);\
	font-family: var(--body-regular);\
	}\
	.ignore2,\
	.ignore3 {\
	width: 0;\
	height: 0;\
	position: relative;\
	}\
	.adjust-btn,\
	.adjuster {\
	display: flex;\
	align-items: flex-start;\
	}\
	.adjust-btn {\
	width: 0;\
	flex-direction: column;\
	justify-content: flex-start;\
	padding: var(--padding-5xl) 0 0;\
	box-sizing: border-box;\
	}\
	.adjuster {\
	align-self: stretch;\
	flex-direction: row;\
	justify-content: center;\
	}\
	.cardlegacy,\
	.configurao-do-bin {\
	overflow: hidden;\
	display: flex;\
	flex-direction: column;\
	align-items: flex-start;\
	justify-content: flex-start;\
	}\
	.cardlegacy {\
	align-self: stretch;\
	border-radius: var(--br-11xs);\
	background-color: var(--neutral-1);\
	gap: 24px;\
	max-width: 100%;\
	}\
	.configurao-do-bin {\
	width: 100%;\
	position: relative;\
	background-color: #f5f5f5;\
	padding: 0 var(--padding-5xl) var(--padding-7xs);\
	box-sizing: border-box;\
	gap: var(--gap-xl);\
	letter-spacing: normal;\
	}\
  .form-image {\
	display:flex;\
  width: 100%;\
  justify-content: center;\
  align-items:center;\
  margin-top: 16px;\
	}\
	@media screen and (max-width: 1200px) {\
	.field-type {\
		flex: 1;\
	}\
	.card-legacy-header {\
		flex-wrap: wrap;\
	}\
	}\
	@media screen and (max-width: 1050px) {\
	.field-type {\
		min-width: 100%;\
	}\
	}\
	@media screen and (max-width: 750px) {\
	.component-2,\
	.label-divider {\
		flex-wrap: wrap;\
	}\
	.field-type {\
		height: auto;\
		min-height: 700;\
	}\
	.field-type1 {\
		min-width: 100%;\
	}\
	}\
	</style>\
	<script>\
		function mostrarImagem(event) {\
			var imagem = event.target.files[0];\
			var leitor = new FileReader();\
			leitor.onload = function () {\
				var imgTag = document.getElementById(\'imagemPreview\');\
				imgTag.src = leitor.result;\
			}\
			leitor.readAsDataURL(imagem); 0\
		}\
	</script>\
</head>\
<body>\
    <div class=\"configurao-do-bin\">\
      <header class=\"page-headerlegacy\">\
        <div class=\"breadcrumb\">\
          <div class=\"st-item\">\
            <div class=\"st-item-label\">Gestão de Bins</div>\
          </div>\
          <div class=\"componentsseparator\">\
            <div class=\"separator\">/</div>\
          </div>\
          <div class=\"last-item\">\
            <div class=\"last-item-label\">Ponto de Monitoramento</div>\
          </div>\
        </div>\
        <div class=\"heading\">\
          <div class=\"heading-left\">\
            <div class=\"wrapper\">\
              <h3 class=\"configurao-dispositivo-devic-container\">\
                <p class=\"configurao-dispositivo\">Configuração dispositivo</p>\
                <p class=\"device-id-deviceid\">Device ID: </p>\
              </h3>\
            </div>\
          </div>\
          <div class=\"heading-extra\">\
          </div>\
        </div>\
        <div class=\"content\">\
          <div class=\"advanced-forms-are\">\
            Ao preencher as informações, atenção aos detalhes é fundamental.\
            Certifique-se de inserir dados corretos e atualizados.\
          </div>\
          <div class=\"width-scrubber\">\
            <div class=\"ignore\"></div>\
            <div class=\"ignore1\"></div>\
          </div>\
        </div>\
      </header>\
      <main class=\"cardlegacy\">\
        <span class=\"head\" ></span>\
        <div class=\"cover\">\
          <div class=\"aspect-ratio-keeper-rotated\"></div>\
        </div>\
        <section class=\"card-legacy-header-wrapper\">\
          <div class=\"card-legacy-header\">\
            <div class=\"field-type\">\
              <div class=\"button1\">\
                <div class=\"icon-wrapper1\">\
                  <img\
                    class=\"wrapper-icon1\"\
                    alt=\"\"\
                    src=\"./public/wrapper-1@2x.png\"\
                  />\
                </div>\
                <div class=\"text\">Novo dispositivo</div>\
              </div>\
              <form action=\"/get/save_settings\">\
              <div class=\"label-divider\">\
                <div class=\"vertical-form-iteminput\">\
                  <div class=\"label\">\
                    <div class=\"title\">Intervalo de Monitoramento (segundos)</div>\
                  </div>\
                  <div class=\"field\">\
                    <div class=\"input-addonlabel\">\
                      <div class=\"wrapper1\">\
                        <div class=\"text1\">http://</div>\
                      </div>\
                    </div>\
                    <div class=\"input\">\
                      <img\
                        class=\"input-prefix-icon\"\
                        alt=\"\"\
                        src=\"./public/inputprefix@2x.png\"\
                      />\
                      <input class=\"input1\" name=\"" + WAKE_UP_INTERVAL_FIELD + "\"  placeholder=\"10\" type=\"number\" />\
                      <img\
                        class=\"input-suffix-icon\"\
                        alt=\"\"\
                        src=\"./public/inputsuffix@2x.png\"\
                      />\
                    </div>\
                    <div class=\"input-addonicon\">\
                      <div class=\"icon-wrapper2\">\
                        <img\
                          class=\"wrapper-icon2\"\
                          alt=\"\"\
                          src=\"./public/wrapper-2@2x.png\"\
                        />\
                      </div>\
                    </div>\
                  </div>\
                </div>\
                <div class=\"vertical-form-iteminput1\">\
                  <div class=\"label1\">\
                    <div class=\"title1\">Endereço do servidor</div>\
                  </div>\
                  <div class=\"field1\">\
                    <div class=\"input-addonlabel1\">\
                      <div class=\"wrapper2\">\
                        <div class=\"text2\">http://</div>\
                      </div>\
                    </div>\
                    <div class=\"input2\">\
                      <img\
                        class=\"input-prefix-icon1\"\
                        alt=\"\"\
                        src=\"./public/inputprefix@2x.png\"\
                      />\
                      <input class=\"input3\" name=\"" + SERVER_ADDR_FIELD + "\" placeholder=\"192.168.0.0:8000\" type=\"text\" />\
                      <img\
                        class=\"input-suffix-icon1\"\
                        alt=\"\"\
                        src=\"./public/inputsuffix@2x.png\"\
                      />\
                    </div>\
                    <div class=\"input-addonicon1\">\
                      <div class=\"icon-wrapper3\">\
                        <img\
                          class=\"wrapper-icon3\"\
                          alt=\"\"\
                          src=\"./public/wrapper-3@2x.png\"\
                        />\
                      </div>\
                    </div>\
                  </div>\
                </div>\
              </div>\
              <div class=\"component-2\">\
                <div class=\"vertical-form-iteminput2\">\
                  <div class=\"label2\">\
                    <div class=\"title2\">SSID</div>\
                  </div>\
                  <div class=\"field2\">\
                    <div class=\"input-addonlabel2\">\
                      <div class=\"wrapper3\">\
                        <div class=\"text3\">http://</div>\
                      </div>\
                    </div>\
                    <div class=\"input4\">\
                      <img\
                        class=\"input-prefix-icon2\"\
                        alt=\"\"\
                        src=\"./public/inputprefix@2x.png\"\
                      />\
                      <input class=\"input5\"  name=\"" + SSID_FIELD+ "\"  placeholder=\"Salcomp_Prod\" type=\"text\" />\
                      <img\
                        class=\"input-suffix-icon2\"\
                        alt=\"\"\
                        src=\"./public/inputsuffix@2x.png\"\
                      />\
                    </div>\
                    <div class=\"input-addonicon2\">\
                      <div class=\"icon-wrapper4\">\
                        <img\
                          class=\"wrapper-icon4\"\
                          alt=\"\"\
                          src=\"./public/wrapper-2@2x.png\"\
                        />\
                      </div>\
                    </div>\
                  </div>\
                </div>\
                <div class=\"vertical-form-iteminput3\">\
                  <div class=\"label3\">\
                    <div class=\"title3\">Senha</div>\
                  </div>\
                  <div class=\"field3\">\
                    <div class=\"input-addonlabel3\">\
                      <div class=\"wrapper4\">\
                        <div class=\"text4\">http://</div>\
                      </div>\
                    </div>\
                    <div class=\"input6\">\
                      <img\
                        class=\"input-prefix-icon3\"\
                        alt=\"\"\
                        src=\"./public/inputprefix@2x.png\"\
                      />\
                      <input class=\"input7\"  name=\"" + PASSWORD_FIELD + "\" placeholder=\"Insira Senha\" type=\"password\" />\
                      <img\
                        class=\"input-suffix-icon3\"\
                        alt=\"\"\
                        src=\"./public/inputsuffix@2x.png\"\
                      />\
                    </div>\
                    <div class=\"input-addonicon3\">\
                      <div class=\"icon-wrapper5\">\
                        <img\
                          class=\"wrapper-icon5\"\
                          alt=\"\"\
                          src=\"./public/wrapper-3@2x.png\"\
                        />\
                      </div>\
                    </div>\
                  </div>\
                </div>\
              </div>\
              <div class=\"label-divider1\">\
                <div class=\"layoutblockshorizontal5\">\
                  <div class=\"divider\">\
                    <img\
                      class=\"line-icon\"\
                      loading=\"lazy\"\
                      alt=\"\"\
                      src=\"./public/line.svg\"\
                    />\
                  </div>\
                </div>\
                <button class=\"button3\">\
                  <div class=\"icon-wrapper7\">\
                    <img\
                      class=\"wrapper-icon7\"\
                      alt=\"\"\
                      src=\"./public/wrapper-7@2x.png\"\
                    />\
                  </div>\
                  <div class=\"text6\">Salvar Configurações</div>\
                </button>\
              </div>\
             </form>\
            <form action=\"/get/restart_system\">\
              <div class=\"label-divider2\">\
                <div class=\"layoutblockshorizontal51\">\
                  <div class=\"divider1\">\
                    <img\
                      class=\"line-icon1\"\
                      loading=\"lazy\"\
                      alt=\"\"\
                      src=\"./public/line.svg\"\
                    />\
                  </div>\
                </div>\
                <button type=\"submit\" class=\"button5\">\
                  <div class=\"icon-wrapper9\">\
                    <img\
                      class=\"wrapper-icon9\"\
                      alt=\"\"\
                      src=\"./public/wrapper-7@2x.png\"\
                    />\
                  </div>\
                  <div class=\"text8\">Reiniciar dispositivo</div>\
                </button>\
              </div>\
            </form>\
            </div>\
            <div class=\"field-type1\">\
              <div class=\"vertical-form-itemselect\">\
                <div class=\"label4\">\
                  <div class=\"title4\">Captura de Imagem</div>\
                </div>\
                <div class=\"field4\">\
                  <img\
                    class=\"aspect-ratio-keeper-addition\"\
                    alt=\"Captura de imagem\"\
                   src = \"data:image/jpeg;base64,";



static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) 
	{
        ESP_LOGI(APL_WIFI_SOFTAP_TAG, "WIFI_EVENT_AP_STACONNECTED");
    } 
	else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) 
	{
        ESP_LOGI(APL_WIFI_SOFTAP_TAG, "WIFI_EVENT_AP_STADISCONNECTED");
    }
}

void apl_wifi_init_access_point(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_ap();					

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {};

	strcpy((char *)wifi_config.ap.ssid, APOLO_SERVER_SSID.c_str());
	strcpy((char *)wifi_config.ap.password, APOLO_SERVER_PASS.c_str());
	wifi_config.ap.ssid_len = APOLO_SERVER_SSID.length();
	wifi_config.ap.channel = APOLO_SERVER_CHANNEL;
	wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	wifi_config.ap.max_connection = APOLO_SERVER_MAX_CONNECTIONS_ALLOWED;

    if (APOLO_SERVER_PASS.length() == 0)
	{
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGV(APL_WIFI_SOFTAP_TAG, "apl_wifi_init_softap finalizado. SSID:%s password:%s channel:%d",
             APOLO_SERVER_SSID.c_str(), APOLO_SERVER_PASS.c_str(), APOLO_SERVER_CHANNEL);
}

// FIXME: atualizar funcao para permitir obter o IP configurado para o accesspoint
void apl_get_access_point_ip(void)
{
	esp_netif_ip_info_t ip_info = {};
	esp_netif_t *net_handler = esp_netif_get_default_netif();
	esp_netif_get_ip_info(net_handler, &ip_info);

	char ip_addr_str[20] = "\0";
	esp_ip4addr_ntoa(&ip_info.ip, ip_addr_str, 20);
    ESP_LOGI(APL_WEBSERVER_TAG,"IP do Access Point (ESP32): ((%s))\n", ip_addr_str);
}

esp_err_t landing_page_handler(	httpd_req_t *req)
{   
	httpd_resp_send_chunk(req, initial_string_webserver.c_str(), initial_string_webserver.length());
	httpd_resp_send_chunk(req, boilerplate_img.c_str(), boilerplate_img.length());
	httpd_resp_send_chunk(req, end_string_webserver.c_str(), end_string_webserver.length());

	ESP_LOGD(APL_WEBSERVER_TAG, "Requisição de landing page");
    return ESP_OK;
}

esp_err_t restart_system_handler(httpd_req_t *req)
{
	ESP_LOGI(APL_WEBSERVER_TAG, "Reiniciando sistema!");
	esp_restart();
    return ESP_OK;
}

esp_err_t take_picture_handler(httpd_req_t *req)
{
	ESP_LOGD(APL_WEBSERVER_TAG, "Requisição de captura de imagem");
	// char photo_name[50];
	// static uint64_t picture_counter = 0;

	// sprintf(photo_name, "/sdcard/apl_img_%lld.jpg", picture_counter++);
	// ESP_LOGI(APL_WEBSERVER_TAG, "nome do arquivo da foto: %s", photo_name);

	ApoloCamera::Get_camera_device().take_picture();
	uint8_t *base64_img = ApoloCamera::Get_camera_device().JPEG_to_base64();
	// apl_save_picture(photo_name);

	if(base64_img == NULL)
	{
		ESP_LOGE(APL_WEBSERVER_TAG, "nao eh possivel converter imagem para base64");
		return ESP_FAIL;
	}

	httpd_resp_send_chunk(req, initial_string_webserver.c_str(), initial_string_webserver.length());
	httpd_resp_send_chunk(req, (char *)base64_img, strlen((char *)base64_img));
	httpd_resp_send_chunk(req, end_string_webserver.c_str(), end_string_webserver.length());

	ApoloCamera::Get_camera_device().free_webserver_img(base64_img);

    return ESP_OK;
}

esp_err_t save_settings_handler(httpd_req_t *req)
{
    char *buf;
    size_t buf_len;
	mias_configs_t apolo_configs = {};
	char *strtol_end_ptr = NULL;

    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1)
    {
        buf = (char *)malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK)
        {
            ESP_LOGI(APL_WEBSERVER_TAG, "Host: %s", buf);
        }
        free(buf);
    }

     buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char *)malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) 
		{
            ESP_LOGD(APL_WEBSERVER_TAG, "buffer: %s", buf);
            char param[MAX_STRING_LENGTH_TO_TEXT_ENTRY];
            if (httpd_query_key_value(buf, WAKE_UP_INTERVAL_FIELD.c_str(), param, sizeof(param)) == ESP_OK) 
            {
                ESP_LOGI(APL_WEBSERVER_TAG, "Intervalo de wake up: %s", param);
				apolo_configs.wakeup_interval = (uint32_t)(strtol(param, &strtol_end_ptr, DECIMAL_BASE));
            }
			else
			{
				ESP_LOGE(APL_WEBSERVER_TAG, "Falha na leitura campo de intervalo wake up");
			}
            if (httpd_query_key_value(buf, SERVER_ADDR_FIELD.c_str(), param, sizeof(param)) == ESP_OK) {
				url_decode(param);
                ESP_LOGI(APL_WEBSERVER_TAG, "Endereco do servidor: %s", param);
				strcpy(apolo_configs.server_addr, param);
            }
			else
			{
				ESP_LOGE(APL_WEBSERVER_TAG, "Falha na leitura campo do endereco do servidor");
			}
            if (httpd_query_key_value(buf, SSID_FIELD.c_str(), param, sizeof(param)) == ESP_OK) {
				ssid_decode(param);
                ESP_LOGI(APL_WEBSERVER_TAG, "SSID: %s", param);
				strcpy(apolo_configs.ssid, param);
            }
			else
			{
				ESP_LOGE(APL_WEBSERVER_TAG, "Falha na leitura campo ssid");
			}
            if (httpd_query_key_value(buf, PASSWORD_FIELD.c_str(), param, sizeof(param)) == ESP_OK) {
				
				char *decoded = password_decode(param); // Trata o caractere @
				ESP_LOGI(APL_WEBSERVER_TAG, "Password: %s", decoded);
				strcpy(apolo_configs.password, decoded);
                // ESP_LOGI(APL_WEBSERVER_TAG, "Password: %s", param);
				// strcpy(apolo_configs.password, param);

            }
			else
			{
				ESP_LOGE(APL_WEBSERVER_TAG, "Falha na leitura campo password");
			}
        }
		else
		{
			ESP_LOGE(APL_WEBSERVER_TAG, "Falha no carregamento da landing page");
        	free(buf);
			return ESP_FAIL;
		}
        free(buf);
		apl_write_configs(apolo_configs);
    }
    return ESP_OK;
}

httpd_uri_t uri_get_landing_page = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = landing_page_handler,	
    .user_ctx = NULL};

httpd_uri_t uri_get_save_settings = {
    .uri = "/get/save_settings",
    .method = HTTP_GET,
    .handler = save_settings_handler,
    .user_ctx = NULL};

httpd_uri_t uri_get_restart_system = {
    .uri = "/get/restart_system",
    .method = HTTP_GET,
    .handler = restart_system_handler,
    .user_ctx = NULL};
    
httpd_uri_t uri_get_take_picture = {
    .uri = "/get/take_picture",
    .method = HTTP_GET,
    .handler = take_picture_handler,
    .user_ctx = NULL};


void uart_init()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void apl_cadastra_id(void *pvParameters)
{
    //TODO Adicionar uma verificação do ID para que ele não fique esperando ser colocado toda vez. Essa função não implica com webserver

	//=========================================== INICIO ESTRATÉGIA 3 problema AS RESOLVIDO=============================================================
	uart_init();
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    ESP_LOGI(APL_WEBSERVER_TAG_UART_CONFIG, "abrindo arquivo de device ID");

    FILE *device_id_file = fopen(APL_SPIFFS_DEVICE_ID_PATH, "rb+");
    if(device_id_file == NULL)
    {
        ESP_LOGE(APL_WEBSERVER_TAG_UART_CONFIG, "falha em abrir arquivo device ID");
        return;
    }

    // Lê o conteúdo atual do identificador da flash
    size_t len = fread(data, 1, BUF_SIZE, device_id_file);
    if (len <= 0) {
        // Se não houver conteúdo lido, podemos inicializar o buffer
        data[0] = '\0'; // Garante que o buffer esteja terminado corretamente com um null character
    } else {
        // Se houver dados lidos, garantimos que o buffer termine corretamente com um null character
        data[len] = '\0';
    }

    while (1) {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            // Se recebermos dados da UART, gravamos apenas esses dados na flash
            data[len] = '\0'; // Garante que os dados recebidos estejam terminados corretamente com um null character
            fseek(device_id_file, 0, SEEK_SET); // Move o cursor para o início do arquivo antes de escrever
            fwrite((const char *)data, 1, strlen((const char *)data), device_id_file);
			fflush(device_id_file);
            ESP_LOGI(APL_WEBSERVER_TAG_UART_CONFIG, "identificador (%s) atribuido a este dispositivo", (const char *)data);
			uart_write_bytes(UART_NUM, (const char *) data, len);
            break;
        } else {
            // Se não recebermos dados da UART, mantemos o identificador atual na flash
            vTaskDelay(pdMS_TO_TICKS(100)); // Atraso de 100ms
        }
    }

    free(data);
    fclose(device_id_file);
    vTaskDelete(NULL);

	//=========================================== FIM ESTRATÉGIA 3 problema AS RESOLVIDO =============================================================





    // FILE *device_id_file = fopen(APL_SPIFFS_DEVICE_ID_PATH, "wb");
    // if(device_id_file == NULL)
    // {
    //     ESP_LOGE(APL_WEBSERVER_TAG_TESTE, "falha em abrir arquivo device ID");
    //     return;
    // }
    // while (1) {
    //     int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);
    //     if (len > 0) {
    //         data[len] = 0; // Adiciona um terminador de string
    //         printf("len: %d\n",len);
    //         printf("Mensagem recebida: %s\n", data);
    //         // vTaskDelay(pdMS_TO_TICKS(100)); // Atraso de 1 segundo
    //         break;
    //     }
    //     // printf("heeeellloo\n");
    //     vTaskDelay(pdMS_TO_TICKS(100)); // Atraso de 1 segundo

    // }
    // fwrite((const char *)data, strlen((const char *)data), strlen((const char *)data), device_id_file);
    // ESP_LOGI(APL_WEBSERVER_TAG_TESTE, "identificador (%s) atribuido a este dispositvo", (const char *)data);
    // free(data);
    // fclose(device_id_file);
    // vTaskDelete(NULL);
}

void task_cadastro_id()
{
    xTaskCreate(apl_cadastra_id, "Cadastro ID MIAS", 2048*4, NULL, 1, NULL);

}


httpd_handle_t apl_start_webserver(void)
{
	device_id = apl_read_device_id();

	size_t insertion_pos_device_id = initial_string_webserver.find("Device ID: ") + std::string("Device ID: ").length();
	if (insertion_pos_device_id != std::string::npos)
	{
        initial_string_webserver.insert(insertion_pos_device_id, device_id);
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = WEBSERVER_HTTP_PAGE_STACK_SIZE;
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK)
    {
		apl_blinkled_webserver();
        httpd_register_uri_handler(server, &uri_get_landing_page);
        httpd_register_uri_handler(server, &uri_get_save_settings);
        httpd_register_uri_handler(server, &uri_get_restart_system);
        httpd_register_uri_handler(server, &uri_get_take_picture);
		
		#ifdef MODO_CADASTRO
		task_cadastro_id();
		#endif
    }
    return server;
}
