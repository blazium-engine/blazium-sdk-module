/**************************************************************************/
/*  discord_embedded_app_client.cpp                                       */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2024-present Blazium Engine contributors.                */
/* Copyright (c) 2024 Dragos Daian, Randolph William Aarseth II.          */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "discord_embedded_app_client.h"
#include "platform/web/api/javascript_bridge_singleton.h"
#include "core/io/json.h"

void DiscordEmbeddedAppClient::_bind_methods() {
	ClassDB::bind_method(D_METHOD("close", "code", "message"), &DiscordEmbeddedAppClient::close);
	ClassDB::bind_method(D_METHOD("authenticate", "access_token"), &DiscordEmbeddedAppClient::authenticate);
	ClassDB::bind_method(D_METHOD("authorize", "client_id", "response_type", "state", "prompt", "scope"), &DiscordEmbeddedAppClient::authorize);
	ClassDB::bind_method(D_METHOD("capture_log", "level", "message"), &DiscordEmbeddedAppClient::capture_log);
	ClassDB::bind_method(D_METHOD("encourage_hardware_acceleration"), &DiscordEmbeddedAppClient::encourage_hardware_acceleration);
	ClassDB::bind_method(D_METHOD("get_channel", "channel_id"), &DiscordEmbeddedAppClient::get_channel);
	ClassDB::bind_method(D_METHOD("get_channel_permissions"), &DiscordEmbeddedAppClient::get_channel_permissions);
	ClassDB::bind_method(D_METHOD("get_entitlements"), &DiscordEmbeddedAppClient::get_entitlements);
	ClassDB::bind_method(D_METHOD("get_instance_connected_participants"), &DiscordEmbeddedAppClient::get_instance_connected_participants);
	ClassDB::bind_method(D_METHOD("get_platform_behaviours"), &DiscordEmbeddedAppClient::get_platform_behaviours);
	ClassDB::bind_method(D_METHOD("get_skus"), &DiscordEmbeddedAppClient::get_skus);
	ClassDB::bind_method(D_METHOD("initiate_image_upload"), &DiscordEmbeddedAppClient::initiate_image_upload);
	ClassDB::bind_method(D_METHOD("open_external_link", "url"), &DiscordEmbeddedAppClient::open_external_link);
	ClassDB::bind_method(D_METHOD("open_invite_dialog"), &DiscordEmbeddedAppClient::open_invite_dialog);
	ClassDB::bind_method(D_METHOD("open_share_moment_dialog", "media_url"), &DiscordEmbeddedAppClient::open_share_moment_dialog);
	ClassDB::bind_method(D_METHOD("set_activity", "activity"), &DiscordEmbeddedAppClient::set_activity);
	ClassDB::bind_method(D_METHOD("set_config", "use_interactive_pip"), &DiscordEmbeddedAppClient::set_config);
	ClassDB::bind_method(D_METHOD("set_orientation_lock_state", "lock_state", "picture_in_picture_lock_state", "grid_lock_state"), &DiscordEmbeddedAppClient::set_orientation_lock_state);
	ClassDB::bind_method(D_METHOD("start_purchase", "sku_id", "pid"), &DiscordEmbeddedAppClient::start_purchase);
	ClassDB::bind_method(D_METHOD("user_settings_get_locale"), &DiscordEmbeddedAppClient::user_settings_get_locale);

	ClassDB::bind_method(D_METHOD("is_discord_environment"), &DiscordEmbeddedAppClient::is_discord_environment);
	ClassDB::bind_method(D_METHOD("get_user_id"), &DiscordEmbeddedAppClient::get_user_id);
	ClassDB::bind_method(D_METHOD("get_client_id"), &DiscordEmbeddedAppClient::get_client_id);
	ClassDB::bind_method(D_METHOD("set_client_id", "client_id"), &DiscordEmbeddedAppClient::set_client_id);
	ClassDB::bind_method(D_METHOD("get_user_instance_id"), &DiscordEmbeddedAppClient::get_user_instance_id);
	ClassDB::bind_method(D_METHOD("get_custom_id"), &DiscordEmbeddedAppClient::get_custom_id);
	ClassDB::bind_method(D_METHOD("get_referrer_id"), &DiscordEmbeddedAppClient::get_referrer_id);
	ClassDB::bind_method(D_METHOD("get_platform"), &DiscordEmbeddedAppClient::get_platform);
	ClassDB::bind_method(D_METHOD("get_guild_id"), &DiscordEmbeddedAppClient::get_guild_id);
	ClassDB::bind_method(D_METHOD("get_channel_id"), &DiscordEmbeddedAppClient::get_channel_id);
	ClassDB::bind_method(D_METHOD("get_location_id"), &DiscordEmbeddedAppClient::get_location_id);
	ClassDB::bind_method(D_METHOD("get_sdk_version"), &DiscordEmbeddedAppClient::get_sdk_version);
	ClassDB::bind_method(D_METHOD("get_mobile_app_version"), &DiscordEmbeddedAppClient::get_mobile_app_version);
	ClassDB::bind_method(D_METHOD("get_frame_id"), &DiscordEmbeddedAppClient::get_frame_id);
    
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "user_id"), "", "get_user_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "client_d"), "set_client_id", "get_client_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "user_instance_id"), "", "get_user_instance_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "custom_id"), "", "get_custom_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "referrer_id"), "", "get_referrer_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "platform"), "", "get_platform");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "guild_id"), "", "get_guild_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "channel_id"), "", "get_channel_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "location_id"), "", "get_location_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "sdk_version"), "", "get_sdk_version");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "mobile_app_version"), "", "get_mobile_app_version");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "frame_id"), "", "get_frame_id");

    ADD_SIGNAL(MethodInfo("is_ready", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("error", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("voice_state_update", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("speaking_start", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("speaking_stop", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("activity_layout_mode_update", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("orientation_update", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("thermal_state_update", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("activity_instance_participants_update", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("entitlement_create", PropertyInfo(Variant::DICTIONARY, "data")));
    ADD_SIGNAL(MethodInfo("current_guild_member_update", PropertyInfo(Variant::DICTIONARY, "data")));
	ADD_SIGNAL(MethodInfo("current_user_update", PropertyInfo(Variant::DICTIONARY, "data")));

	BIND_ENUM_CONSTANT(DISCORD_EMBEDDED_APP_ORIENTATION_LOCK_STATE_UNHANDLED);
	BIND_ENUM_CONSTANT(DISCORD_EMBEDDED_APP_ORIENTATION_LOCK_STATE_UNLOCKED);
	BIND_ENUM_CONSTANT(DISCORD_EMBEDDED_APP_ORIENTATION_LOCK_STATE_PORTRAIT);
	BIND_ENUM_CONSTANT(DISCORD_EMBEDDED_APP_ORIENTATION_LOCK_STATE_LANDSCAPE);
}


void DiscordEmbeddedAppClient::_handle_message(Variant p_event) {
    // event is a tuple
    // https://github.com/discord/embedded-app-sdk/blob/main/src/Discord.ts#L281
    Array event_array = p_event;
    if (event_array.size() != 1) {
        ERR_PRINT("Event is invalid");
        return;
    }
    Dictionary event_dict = event_array[0];
    if (!event_dict.has("data")) {
        ERR_PRINT("Event data is invalid");
        return;
    }
	Array data_tuple = event_dict["data"];
	if (data_tuple.size() != 2) {
        ERR_PRINT("Event data tuple is invalid");
        return;
	}
    JavaScriptBridge *singleton = JavaScriptBridge::get_singleton();
    if (!singleton) {
        ERR_PRINT("JavaScriptBridge singleton is invalid");
        return;
    }
    Ref<JavaScriptObject> json = singleton->get_interface("JSON");
	if (!json.is_valid()) {
        ERR_PRINT("JavaScriptBridge JSON is invalid");
        return;
	}
	String stringified_data = json->call("stringify", data_tuple[1]);
	Dictionary data_dict = JSON::parse_string(stringified_data);
	int opcode = data_tuple[0];
	if (opcode == DiscordEmbeddedAppClient::Opcode::OP_FRAME) {
		if (data_dict["cmd"] == "DISPATCH") {
			_handle_dispatch(data_dict);
		} else if (data_dict.has("nonce")) {
			String nonce = data_dict["nonce"];
			if (_commands.has(nonce)) {
				Ref<DiscordEmbeddedAppResponse> response = _commands[nonce];
				_commands.erase(nonce);
				if (response.is_valid()) {
					Ref<DiscordEmbeddedAppResponse::DiscordEmbeddedAppResult> result;
					result.instantiate();
					result->set_data(data_dict["data"]);
					response->emit_signal("finished", result);
				}
			}
		} else {
			ERR_PRINT("Unkown packet received.");
		}
	}
}
void DiscordEmbeddedAppClient::_handle_dispatch(Dictionary p_data) {
	String event = p_data["evt"];
	if (event == "READY") {
		// once ready, subscribe to all events
		_subscribe_to_events();
		emit_signal("is_ready", p_data["data"]);
	} else if (event == "ERROR") {
		emit_signal("error", p_data["data"]);
	} else if (event == "VOICE_STATE_UPDATE") {
		emit_signal("voice_state_update", p_data["data"]);
	} else if (event == "SPEAKING_START") {
		emit_signal("speaking_start", p_data["data"]);
	} else if (event == "SPEAKING_STOP") {
		emit_signal("speaking_stop", p_data["data"]);
	} else if (event == "ACTIVITY_LAYOUT_MODE_UPDATE") {
		emit_signal("activity_layout_mode_update", p_data["data"]);
	} else if (event == "ORIENTATION_UPDATE") {
		emit_signal("orientation_update", p_data["data"]);
	} else if (event == "CURRENT_USER_UPDATE") {
		Dictionary data_dict = p_data["data"];
		user_id = data_dict["id"];
		emit_signal("current_user_update", p_data["data"]);
	} else if (event == "THERMAL_STATE_UPDATE") {
		emit_signal("thermal_state_update", p_data["data"]);
	} else if (event == "ACTIVITY_INSTANCE_PARTICIPANTS_UPDATE") {
		emit_signal("activity_instance_participants_update", p_data["data"]);
	} else if (event == "ENTITLEMENT_CREATE") {
		emit_signal("entitlement_create", p_data["data"]);
	} else if (event == "CURRENT_GUILD_MEMBER_UPDATE") {
		emit_signal("current_guild_member_update", p_data["data"]);
	} else {
		ERR_PRINT("Unkown event: " + event);
	}
}
void DiscordEmbeddedAppClient::_send_command(String p_command, Dictionary p_args, String p_nonce) {
	Dictionary body;
	body["cmd"] = p_command;
	body["args"] = p_args;
	body["nonce"] = p_nonce;
	_send_message(DiscordEmbeddedAppClient::Opcode::OP_FRAME, body);
}
void DiscordEmbeddedAppClient::_send_message(int opcode, Dictionary body) {
	Array data_message;
	data_message.push_back(opcode);
	data_message.push_back(body);

	String js_command = String("window.source.postMessage(") + JSON::stringify(data_message) + ", '*')";
	JavaScriptBridge *singleton = JavaScriptBridge::get_singleton();
	if (!singleton) {
		ERR_PRINT("JavaScriptBridge not available.");
		return;
	}
	singleton->eval(js_command, false);
}

DiscordEmbeddedAppClient::DiscordEmbeddedAppClient() {
	JavaScriptBridge *singleton = JavaScriptBridge::get_singleton();
	if (!singleton) {
		ERR_PRINT("JavaScriptBridge not available.");
		return;
	}
	Ref<JavaScriptObject> window = singleton->get_interface("window");
	if (!window.is_valid()) {
		// Don't error here as we are on desktop most likely.
		return;
	}
	// this reference must be kept to keep it alive
	
	callback = singleton->create_callback(callable_mp(this, &DiscordEmbeddedAppClient::_handle_message));
	if (!callback.is_valid()) {
		ERR_PRINT("Callback is invalid");
		return;
	}
	window->call("addEventListener", "message", callback);
	// update params
	String query_parts_string = singleton->eval("window.location.search");
	Vector<String> query_parts = query_parts_string.trim_prefix("?").split("&", false);
	Dictionary query_map = {};
	for (int i = 0; i < query_parts.size(); i++) {
		String part = query_parts[i];
		PackedStringArray parts = part.split("=");
		if (parts.size() == 2) {
			query_map[parts[0]] = parts[1];
		} else {
			ERR_PRINT("Invalid query part: " + part);
		}
	}

	user_instance_id = query_map.get("instance_id", "");
	custom_id = query_map.get("custom_id", "");
	referrer_id = query_map.get("referrer_id", "");
	platform = query_map.get("platform", "");
	guild_id = query_map.get("guild_id", "");
	channel_id = query_map.get("channel_id", "");
	location_id = query_map.get("location_id", "");
	mobile_app_version = query_map.get("mobile_app_version", "");

	frame_id = query_map.get("frame_id", "");
	
	singleton->eval("window.source = window.parent.opener ?? window.parent", true);
	
	_handshake();
}

String _generate_nonce() {
	String chars = "0123456789abcdef";
	String nonce;
	for (int i = 0; i < 32; i++) {
		nonce += chars[rand() % chars.length()];
		if (i == 7 || i == 11 || i == 15 || i == 19) {
			nonce += "-";
		}
	}
	return nonce;
}

void DiscordEmbeddedAppClient::_subscribe_to_events() {
    Array events;
    events.push_back("READY");
    events.push_back("ERROR");
    events.push_back("VOICE_STATE_UPDATE");
    events.push_back("SPEAKING_START");
    events.push_back("SPEAKING_STOP");
    events.push_back("ACTIVITY_LAYOUT_MODE_UPDATE");
    events.push_back("ORIENTATION_UPDATE");
    events.push_back("CURRENT_USER_UPDATE");
    events.push_back("THERMAL_STATE_UPDATE");
    events.push_back("ACTIVITY_INSTANCE_PARTICIPANTS_UPDATE");
    events.push_back("ENTITLEMENT_CREATE");
    events.push_back("CURRENT_GUILD_MEMBER_UPDATE");
	for (int i = 0; i < events.size(); i++) {
		String event = events[i];
		Dictionary args;
		args["cmd"] = "SUBSCRIBE";
		args["evt"] = event;
		Dictionary args_dict;
		args_dict["channel_id"] = channel_id;
		args_dict["guild_id"] = guild_id;
		args["args"] = args_dict;
		args["nonce"] = _generate_nonce();

		_send_message(DiscordEmbeddedAppClient::Opcode::OP_FRAME, args);
	}
}

void DiscordEmbeddedAppClient::_handshake() {
	Dictionary body;
	body["v"] = 1;
	body["encoding"] = "json";
	body["client_id"] = client_id;
	body["frame_id"] = frame_id;

	_send_message(DiscordEmbeddedAppClient::Opcode::OP_HANDSHAKE, body);
}

bool DiscordEmbeddedAppClient::is_discord_environment() {
    JavaScriptBridge *singleton = JavaScriptBridge::get_singleton();
    if (!singleton) {
        ERR_PRINT("JavaScriptBridge singleton is invalid");
        return false;
    }

	bool is_discord = singleton->eval("window.location.hostname.includes('discord')", true);
	return is_discord;
}

void DiscordEmbeddedAppClient::close(int p_code, String p_message) {
	Dictionary body;
	body["code"] = p_code;
	body["message"] = p_message;
	body["nonce"] = _generate_nonce();
	_send_message(DiscordEmbeddedAppClient::Opcode::OP_CLOSE, body);
}

Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::authenticate(String p_access_token) {
	Dictionary body;
	body["access_token"] = p_access_token;

	String nonce = _generate_nonce();
	_send_command("AUTHENTICATE", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::authorize(String p_client_id, String p_response_type, String p_state, String p_prompt, Array p_scope) {
	Dictionary body;
	body["client_id"] = client_id;
	body["response_type"] = p_response_type;
	body["state"] = p_state;
	body["prompt"] = p_prompt;
	body["scope"] = p_scope;

	String nonce = _generate_nonce();
	_send_command("AUTHORIZE", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}

Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::capture_log(String p_level, String p_message) {
	Dictionary body;
	body["level"] = p_level;
	body["message"] = p_message;

	String nonce = _generate_nonce();
	_send_command("CAPTURE_LOG", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::encourage_hardware_acceleration() {
	Dictionary body;
	String nonce = _generate_nonce();
	_send_command("ENCOURAGE_HW_ACCELERATION", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::get_channel(String p_channel_id) {
	Dictionary body;
	body["channel_id"] = p_channel_id;
	String nonce = _generate_nonce();
	_send_command("GET_CHANNEL", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::get_channel_permissions() {
	String nonce = _generate_nonce();
	_send_command("GET_CHANNEL_PERMISSIONS", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::get_entitlements() {
	String nonce = _generate_nonce();
	_send_command("GET_ENTITLEMENTS_EMBEDDED", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::get_instance_connected_participants() {
	String nonce = _generate_nonce();
	_send_command("GET_ACTIVITY_INSTANCE_CONNECTED_PARTICIPANTS", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::get_platform_behaviours() {
	String nonce = _generate_nonce();
	_send_command("GET_PLATFORM_BEHAVIORS", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::get_skus() {
	String nonce = _generate_nonce();
	_send_command("GET_SKUS_EMBEDDED", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::initiate_image_upload() {
	String nonce = _generate_nonce();
	_send_command("INITIATE_IMAGE_UPLOAD", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::open_external_link(String p_url) {
	String nonce = _generate_nonce();
	_send_command("OPEN_EXTERNAL_LINK", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::open_invite_dialog() {
	String nonce = _generate_nonce();
	_send_command("OPEN_INVITE_DIALOG", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::open_share_moment_dialog(String p_media_url) {
	Dictionary body;
	body["mediaUrl"] = p_media_url;

	String nonce = _generate_nonce();
	_send_command("OPEN_SHARE_MOMENT_DIALOG", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::set_activity(Dictionary p_activity) {
	Dictionary body;
	body["activity"] = p_activity;

	String nonce = _generate_nonce();
	_send_command("SET_ACTIVITY", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::set_config(bool p_use_interactive_pip) {
	Dictionary body;
	body["use_interactive_pip"] = p_use_interactive_pip;

	String nonce = _generate_nonce();
	_send_command("SET_CONFIG", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::set_orientation_lock_state(DiscordEmbeddedAppOrientationLockState p_lock_state, DiscordEmbeddedAppOrientationLockState p_picture_in_picture_lock_state, DiscordEmbeddedAppOrientationLockState p_grid_lock_state) {
	Dictionary body;
	body["lock_state"] = p_lock_state;
	body["pip_lock_state"] = p_picture_in_picture_lock_state;
	body["grid_lock_state"] = p_grid_lock_state;

	String nonce = _generate_nonce();
	_send_command("SET_ORIENTATION_LOCK_STATE", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::start_purchase(String p_sku_id, String p_pid) {
	Dictionary body;
	body["sku_id"] = p_sku_id;
	body["pid"] = p_pid;

	String nonce = _generate_nonce();
	_send_command("START_PURCHASE", body, nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
Ref<DiscordEmbeddedAppResponse> DiscordEmbeddedAppClient::user_settings_get_locale() {
	String nonce = _generate_nonce();
	_send_command("USER_SETTINGS_GET_LOCALE", Dictionary(), nonce);
	Ref<DiscordEmbeddedAppResponse> response;
	response.instantiate();
	_commands[nonce] = response;
	return response;
}
