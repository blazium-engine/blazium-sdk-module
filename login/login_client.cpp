/**************************************************************************/
/*  login_client.cpp                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                            BLAZIUM ENGINE                              */
/*                        https://blazium.app                             */
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

#include "login_client.h"

void LoginClient::_bind_methods() {
	ClassDB::bind_method(D_METHOD("request_login_info", "login_type"), &LoginClient::request_login_info);
	ClassDB::bind_method(D_METHOD("request_auth_id", "login_type"), &LoginClient::request_auth_id);
	ClassDB::bind_method(D_METHOD("request_access_token", "login_type", "auth_id", "code"), &LoginClient::request_access_token);
	ClassDB::bind_method(D_METHOD("refresh_jwt_token", "refresh_token"), &LoginClient::refresh_jwt_token);
	ClassDB::bind_method(D_METHOD("verify_jwt_token", "jwt_token"), &LoginClient::verify_jwt_token);

	ClassDB::bind_method(D_METHOD("set_server_url", "server_url"), &LoginClient::set_server_url);
	ClassDB::bind_method(D_METHOD("get_server_url"), &LoginClient::get_server_url);
	ClassDB::bind_method(D_METHOD("set_game_id", "game_id"), &LoginClient::set_game_id);
	ClassDB::bind_method(D_METHOD("get_game_id"), &LoginClient::get_game_id);

	ClassDB::bind_method(D_METHOD("set_websocket_prefix", "websocket_prefix"), &LoginClient::set_websocket_prefix);
	ClassDB::bind_method(D_METHOD("get_websocket_prefix"), &LoginClient::get_websocket_prefix);
	ClassDB::bind_method(D_METHOD("set_http_prefix", "http_prefix"), &LoginClient::set_http_prefix);
	ClassDB::bind_method(D_METHOD("get_http_prefix"), &LoginClient::get_http_prefix);

	ClassDB::bind_method(D_METHOD("get_connected"), &LoginClient::get_connected);
	ClassDB::bind_method(D_METHOD("set_override_discord_path", "override_discord_path"), &LoginClient::set_override_discord_path);
	ClassDB::bind_method(D_METHOD("get_override_discord_path"), &LoginClient::get_override_discord_path);

	ClassDB::bind_method(D_METHOD("connect_to_server"), &LoginClient::connect_to_server);
	ClassDB::bind_method(D_METHOD("disconnect_from_server"), &LoginClient::disconnect_from_server);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "server_url", PROPERTY_HINT_NONE, ""), "set_server_url", "get_server_url");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "game_id", PROPERTY_HINT_NONE, ""), "set_game_id", "get_game_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "http_prefix", PROPERTY_HINT_NONE, ""), "set_http_prefix", "get_http_prefix");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "websocket_prefix", PROPERTY_HINT_NONE, ""), "set_websocket_prefix", "get_websocket_prefix");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "connected"), "", "get_connected");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "discord_embedded_app/path"), "set_override_discord_path", "get_override_discord_path");

	ADD_SIGNAL(MethodInfo("log_updated", PropertyInfo(Variant::STRING, "command"), PropertyInfo(Variant::STRING, "logs")));
	ADD_SIGNAL(MethodInfo("disconnected_from_server", PropertyInfo(Variant::STRING, "reason")));
	ADD_SIGNAL(MethodInfo("connected_to_server"));
	ADD_SIGNAL(MethodInfo("received_jwt", PropertyInfo(Variant::STRING, "jwt"), PropertyInfo(Variant::STRING, "type"), PropertyInfo(Variant::STRING, "access_token")));
}

Ref<LoginClient::LoginConnectResponse> LoginClient::connect_to_server() {
	if (connected) {
		Ref<LoginConnectResponse> response = Ref<LoginConnectResponse>();
		response.instantiate();
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LoginConnectResponse::signal_finish);
		callable.call_deferred("Already connected to the server.");
		return response;
	}
	String url = get_server_url();
	PackedStringArray protocols;
	protocols.push_back("blazium");
	protocols.push_back(game_id);
	_socket->set_supported_protocols(protocols);
	String connect_url = websocket_prefix + url + connect_route;
	Error err = _socket->connect_to_url(connect_url);
	if (err != OK) {
		set_process_internal(false);
		emit_signal("log_updated", "error", "Unable to connect to server at: " + connect_url);
		connected = false;
		return Ref<LoginConnectResponse>();
	}
	set_process_internal(true);
	emit_signal("log_updated", "connect_to_server", "Connecting to: " + connect_url);
	connect_response = Ref<LoginConnectResponse>();
	connect_response.instantiate();
	return connect_response;
}

void LoginClient::disconnect_from_server() {
	if (connected) {
		_socket->close(1000, "Normal Closure");
		connected = false;
		set_process_internal(false);
		emit_signal("disconnected_from_server", _socket->get_close_reason());
		emit_signal("log_updated", "disconnected_from_server", "Disconnected from: " + get_server_url());
	}
}
