#include <string>

static std::string initial_string_webserver = "<!DOCTYPE html>\
<html>\
<head>\
	<meta charset=\"UTF-8\">\
	<title>CONFIGURAÇÃO DE DISPOSITIVO</title>\
	<style type=\"text/css\">\
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
	}\
	.field4 {\
	align-self: stretch;\
	flex: 1;\
	display: flex;\
	flex-direction: column;\
	align-items: flex-start;\
	justify-content: flex-start;\
	background: #f3f3f3;\
	background-size: cover;\
	background-repeat: no-repeat;\
	background-position: top;\
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
	align-items: flex-start;\
	justify-content: flex-start;\
	max-width: 100%;\
	}\
	.vertical-form-item-label {\
	align-self: stretch;\
	gap: var(--gap-3xs);\
	text-align: center;\
	color: var(--primary-6);\
	}\
	.vertical-form-itemselect {\
	height: 754px;\
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
                <p class=\"device-id-deviceid\">Device ID: {deviceID}</p>\
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
                <button class=\"button5\">\
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
            </div>\
            <div class=\"field-type1\">\
              <div class=\"vertical-form-itemselect\">\
                <div class=\"label4\">\
                  <div class=\"title4\">Captura de Imagem</div>\
                </div>\
                <div class=\"field4\">\
                  <img\
                    class=\"aspect-ratio-keeper-addition\"\
                    loading=\"lazy\"\
                    alt=\"Captura de imagem\"\
                    src=\"./public/alt_capture.jpg\"\
                  />\
                </div>\
                <div class=\"vertical-form-item-label\">\
                  <div class=\"layoutblockshorizontal52\">\
                    <div class=\"divider2\">\
                      <img\
                        class=\"line-icon2\"\
                        loading=\"lazy\"\
                        alt=\"\"\
                        src=\"./public/line.svg\"\
                      />\
                    </div>\
                  </div>\
                  <button class=\"button7\">\
                    <div class=\"icon-wrapper11\">\
                      <img\
                        class=\"wrapper-icon11\"\
                        alt=\"\"\
                        src=\"./public/wrapper-11@2x.png\"\
                      />\
                    </div>\
                    <div class=\"title-wrapper\">Capturar Imagem</div>\
                  </button>\
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
	<div class=\"quadrado\">\
		<div class=\"quadrado-acima\">\
			<img src = \"data:image/jpeg;base64,";