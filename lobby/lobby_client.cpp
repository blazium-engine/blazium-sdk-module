/**************************************************************************/
/*  lobby_client.cpp                                                      */
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

#include "./lobby_client.h"
#include "lobby_info.h"
#include "scene/main/node.h"
#include "../discord/discord_embedded_app_client.h"

LobbyClient::LobbyClient() {
	if (DiscordEmbeddedAppClient::static_is_discord_environment()) {
		server_url = "wss://" + DiscordEmbeddedAppClient::static_find_client_id() + ".discordsays.com/.proxy/" + override_discord_path;
	} else {
		server_url = "wss://lobby.blazium.app/connect";
	}
	lobby.instantiate();
	peer.instantiate();
	_socket = Ref<WebSocketPeer>(WebSocketPeer::create());
	set_process_internal(false);
}

LobbyClient::~LobbyClient() {
	_socket->close();
	set_process_internal(false);
}

void LobbyClient::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_server_url", "server_url"), &LobbyClient::set_server_url);
	ClassDB::bind_method(D_METHOD("get_server_url"), &LobbyClient::get_server_url);
	ClassDB::bind_method(D_METHOD("set_reconnection_token", "reconnection_token"), &LobbyClient::set_reconnection_token);
	ClassDB::bind_method(D_METHOD("get_reconnection_token"), &LobbyClient::get_reconnection_token);
	ClassDB::bind_method(D_METHOD("set_game_id", "game_id"), &LobbyClient::set_game_id);
	ClassDB::bind_method(D_METHOD("get_game_id"), &LobbyClient::get_game_id);

	ClassDB::bind_method(D_METHOD("is_host"), &LobbyClient::is_host);
	ClassDB::bind_method(D_METHOD("get_connected"), &LobbyClient::get_connected);
	ClassDB::bind_method(D_METHOD("get_lobby"), &LobbyClient::get_lobby);
	ClassDB::bind_method(D_METHOD("get_peer"), &LobbyClient::get_peer);
	ClassDB::bind_method(D_METHOD("get_peers"), &LobbyClient::get_peers);
	ClassDB::bind_method(D_METHOD("get_host_data"), &LobbyClient::get_host_data);
	ClassDB::bind_method(D_METHOD("get_peer_data"), &LobbyClient::get_peer_data);

	ClassDB::bind_method(D_METHOD("set_override_discord_path", "override_discord_path"), &LobbyClient::set_override_discord_path);
	ClassDB::bind_method(D_METHOD("get_override_discord_path"), &LobbyClient::get_override_discord_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "discord_embedded_app/path"), "set_override_discord_path", "get_override_discord_path");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "server_url", PROPERTY_HINT_NONE, ""), "set_server_url", "get_server_url");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "reconnection_token", PROPERTY_HINT_NONE, ""), "set_reconnection_token", "get_reconnection_token");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "game_id", PROPERTY_HINT_NONE, ""), "set_game_id", "get_game_id");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "connected"), "", "get_connected");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "lobby", PROPERTY_HINT_RESOURCE_TYPE, "LobbyInfo"), "", "get_lobby");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer"), "", "get_peer");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "peers", PROPERTY_HINT_ARRAY_TYPE, "LobbyPeer"), "", "get_peers");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "host_data"), "", "get_host_data");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "peer_data"), "", "get_peer_data");
	ADD_PROPERTY_DEFAULT("peers", TypedArray<LobbyPeer>());
	ADD_PROPERTY_DEFAULT("peer", Ref<LobbyPeer>());
	ADD_PROPERTY_DEFAULT("lobby", Ref<LobbyInfo>());
	// Register methods
	ClassDB::bind_method(D_METHOD("connect_to_server"), &LobbyClient::connect_to_server);
	ClassDB::bind_method(D_METHOD("disconnect_from_server"), &LobbyClient::disconnect_from_server);
	ClassDB::bind_method(D_METHOD("create_lobby", "title", "sealed", "tags", "max_players", "password"), &LobbyClient::create_lobby, DEFVAL(Dictionary()), DEFVAL(4), DEFVAL(""));
	ClassDB::bind_method(D_METHOD("join_lobby", "lobby_id", "password"), &LobbyClient::join_lobby, DEFVAL(""));
	ClassDB::bind_method(D_METHOD("leave_lobby"), &LobbyClient::leave_lobby);
	ClassDB::bind_method(D_METHOD("list_lobbies"), &LobbyClient::list_lobby);
	ClassDB::bind_method(D_METHOD("kick_peer", "peer_id"), &LobbyClient::kick_peer);
	ClassDB::bind_method(D_METHOD("send_chat_message", "chat_message"), &LobbyClient::lobby_chat);
	ClassDB::bind_method(D_METHOD("set_lobby_ready", "ready"), &LobbyClient::lobby_ready);
	ClassDB::bind_method(D_METHOD("add_lobby_tags", "tags"), &LobbyClient::set_lobby_tags);
	ClassDB::bind_method(D_METHOD("del_lobby_tags", "keys"), &LobbyClient::del_lobby_tags);
	ClassDB::bind_method(D_METHOD("set_lobby_sealed", "seal"), &LobbyClient::seal_lobby);
	ClassDB::bind_method(D_METHOD("notify_lobby", "data"), &LobbyClient::lobby_notify);
	ClassDB::bind_method(D_METHOD("notify_peer", "data", "target_peer"), &LobbyClient::peer_notify);

	ClassDB::bind_method(D_METHOD("add_peer_user_data", "data"), &LobbyClient::add_user_data);
	ClassDB::bind_method(D_METHOD("del_peer_user_data", "keys"), &LobbyClient::del_user_data);

	ClassDB::bind_method(D_METHOD("add_lobby_data", "data", "is_private"), &LobbyClient::lobby_data, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("del_lobby_data", "keys", "is_private"), &LobbyClient::del_lobby_data, DEFVAL(false));

	ClassDB::bind_method(D_METHOD("add_peer_data", "data", "target_peer", "is_private"), &LobbyClient::set_peer_data, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("del_peer_data", "keys", "target_peer", "is_private"), &LobbyClient::del_peer_data, DEFVAL(false));

	ClassDB::bind_method(D_METHOD("add_peers_data", "data", "is_private"), &LobbyClient::set_peers_data, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("del_peers_data", "keys", "is_private"), &LobbyClient::del_peers_data, DEFVAL(false));

	// Register signals
	ADD_SIGNAL(MethodInfo("connected_to_server", PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer"), PropertyInfo(Variant::STRING, "reconnection_token")));
	ADD_SIGNAL(MethodInfo("disconnected_from_server", PropertyInfo(Variant::STRING, "reason")));
	ADD_SIGNAL(MethodInfo("received_peer_user_data", PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer"), PropertyInfo(Variant::OBJECT, "data")));
	ADD_SIGNAL(MethodInfo("lobby_notified", PropertyInfo(Variant::OBJECT, "data"), PropertyInfo(Variant::OBJECT, "from_peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer")));
	ADD_SIGNAL(MethodInfo("received_peer_data", PropertyInfo(Variant::OBJECT, "data"), PropertyInfo(Variant::OBJECT, "to_peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer"), PropertyInfo(Variant::BOOL, "is_private")));
	ADD_SIGNAL(MethodInfo("received_lobby_data", PropertyInfo(Variant::OBJECT, "data"), PropertyInfo(Variant::BOOL, "is_private")));
	ADD_SIGNAL(MethodInfo("lobby_created", PropertyInfo(Variant::OBJECT, "lobby", PROPERTY_HINT_RESOURCE_TYPE, "LobbyInfo"), PropertyInfo(Variant::ARRAY, "peers", PROPERTY_HINT_ARRAY_TYPE, "LobbyPeer")));
	ADD_SIGNAL(MethodInfo("lobby_joined", PropertyInfo(Variant::OBJECT, "lobby", PROPERTY_HINT_RESOURCE_TYPE, "LobbyInfo"), PropertyInfo(Variant::ARRAY, "peers", PROPERTY_HINT_ARRAY_TYPE, "LobbyPeer")));
	ADD_SIGNAL(MethodInfo("lobby_left", PropertyInfo(Variant::BOOL, "kicked")));
	ADD_SIGNAL(MethodInfo("lobby_sealed", PropertyInfo(Variant::BOOL, "sealed")));
	ADD_SIGNAL(MethodInfo("lobby_tagged", PropertyInfo(Variant::DICTIONARY, "tags")));
	ADD_SIGNAL(MethodInfo("lobbies_listed", PropertyInfo(Variant::ARRAY, "lobbies", PROPERTY_HINT_ARRAY_TYPE, "LobbyInfo")));
	ADD_SIGNAL(MethodInfo("peer_joined", PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer")));
	ADD_SIGNAL(MethodInfo("peer_reconnected", PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer")));
	ADD_SIGNAL(MethodInfo("peer_left", PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer"), PropertyInfo(Variant::BOOL, "kicked")));
	ADD_SIGNAL(MethodInfo("peer_disconnected", PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer")));
	ADD_SIGNAL(MethodInfo("peer_messaged", PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer"), PropertyInfo(Variant::STRING, "chat_message")));
	ADD_SIGNAL(MethodInfo("peer_ready", PropertyInfo(Variant::OBJECT, "peer", PROPERTY_HINT_RESOURCE_TYPE, "LobbyPeer"), PropertyInfo(Variant::BOOL, "is_ready")));
	ADD_SIGNAL(MethodInfo("log_updated", PropertyInfo(Variant::STRING, "command"), PropertyInfo(Variant::STRING, "logs")));
}

void LobbyClient::set_server_url(const String &p_server_url) { this->server_url = p_server_url; }
String LobbyClient::get_server_url() { return server_url; }
void LobbyClient::set_reconnection_token(const String &p_reconnection_token) { this->reconnection_token = p_reconnection_token; }
String LobbyClient::get_reconnection_token() { return reconnection_token; }
void LobbyClient::set_game_id(const String &p_game_id) { this->game_id = p_game_id; }
String LobbyClient::get_game_id() { return game_id; }
bool LobbyClient::is_host() { return lobby->get_host() == peer->get_id(); }
bool LobbyClient::get_connected() { return connected; }
Dictionary LobbyClient::get_host_data() { return host_data; }
Dictionary LobbyClient::get_peer_data() { return peer_data; }
void LobbyClient::set_lobby(const Ref<LobbyInfo> &p_lobby) { this->lobby = p_lobby; }
Ref<LobbyInfo> LobbyClient::get_lobby() { return lobby; }
void LobbyClient::set_peer(const Ref<LobbyPeer> &p_peer) { this->peer = p_peer; }
Ref<LobbyPeer> LobbyClient::get_peer() { return peer; }
TypedArray<LobbyPeer> LobbyClient::get_peers() { return peers; }

Ref<LobbyResponse> LobbyClient::connect_to_server() {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Already connected to the server.");
		return response;
	}
	// if there is another command connect, finish that with error
	if (_commands.has("connect")) {
		Ref<LobbyResponse> old_response = _commands.get("connect", Ref<LobbyResponse>());
		Ref<LobbyResponse::LobbyResult> result;
		result.instantiate();
		result->set_error("Trying to connect twice");
		old_response->emit_signal("finished", result);
	}
	_commands["connect"] = response;
	String lobby_url = get_server_url();
	String url = lobby_url;
	PackedStringArray protocols;
	protocols.push_back("blazium");
	protocols.push_back(game_id);
	if (reconnection_token != "") {
		protocols.push_back(reconnection_token);
	}
	_socket->set_supported_protocols(protocols);
	Error err = _socket->connect_to_url(url);
	set_process_internal(true);
	if (err != OK) {
		emit_signal("log_updated", "error", "Unable to connect to lobby server at: " + url);
		return response;
	}
	emit_signal("log_updated", "connect_to_server", "Connecting to: " + url);
	return response;
}

Ref<LobbyResponse> LobbyClient::disconnect_from_server() {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	// if there is another disconnect connect, finish that with error
	if (_commands.has("disconnect")) {
		Ref<LobbyResponse> old_response = _commands.get("disconnect", Ref<LobbyResponse>());
		Ref<LobbyResponse::LobbyResult> result;
		result.instantiate();
		result->set_error("Trying to disconnect twice");
		old_response->emit_signal("finished", result);
	}
	_commands["disconnect"] = response;
	set_process_internal(true);
	_socket->close(1000, "Normal Closure");
	connected = false;
	host_data = Dictionary();
	peer_data = Dictionary();
	peer->set_data(Dictionary());
	lobbies.clear();
	peers.clear();
	lobby->set_dict(Dictionary());
	emit_signal("log_updated", "disconnect_from_server", "Disconnecting from: " + get_server_url());
	return response;
}

String LobbyClient::_increment_counter() {
	return String::num(_counter++);
}

Ref<ViewLobbyResponse> LobbyClient::create_lobby(const String &p_name, bool p_sealed, const Dictionary &p_tags, int p_max_players, const String &p_password) {
	Ref<ViewLobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &ViewLobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "create_lobby";
	Dictionary data_dict;
	command["data"] = data_dict;
	data_dict["name"] = p_name;
	data_dict["max_players"] = p_max_players;
	data_dict["password"] = p_password;
	data_dict["tags"] = p_tags;
	data_dict["sealed"] = p_sealed;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_VIEW);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<ViewLobbyResponse> LobbyClient::join_lobby(const String &p_lobby_id, const String &p_password) {
	Ref<ViewLobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &ViewLobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "join_lobby";
	Dictionary data_dict;
	command["data"] = data_dict;
	data_dict["lobby_id"] = p_lobby_id;
	data_dict["password"] = p_password;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_VIEW);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::leave_lobby() {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "leave_lobby";
	Dictionary data_dict;
	command["data"] = data_dict;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::list_lobby() {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "list_lobby";
	Dictionary data_dict;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::kick_peer(const String &p_peer_id) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "kick_peer";
	Dictionary data_dict;
	command["data"] = data_dict;
	data_dict["peer_id"] = p_peer_id;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::set_lobby_tags(const Dictionary &p_tags) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "lobby_tags";
	Dictionary data_dict;
	command["data"] = data_dict;
	data_dict["tags"] = p_tags;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::del_lobby_tags(const TypedArray<String> &p_keys) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "lobby_tags";
	Dictionary data_dict;
	command["data"] = data_dict;
	Dictionary data_object_dict;
	// set null value
	for (int i = 0; i < p_keys.size(); i++) {
		data_object_dict[p_keys[i]] = Variant();
	}
	data_dict["tags"] = data_object_dict;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::lobby_chat(const String &p_chat_message) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "chat_lobby";
	Dictionary data_dict;
	command["data"] = data_dict;
	data_dict["chat"] = p_chat_message;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::lobby_ready(bool p_ready) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	if (p_ready) {
		command["command"] = "lobby_ready";
	} else {
		command["command"] = "lobby_unready";
	}
	Dictionary data_dict;
	command["data"] = data_dict;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::seal_lobby(bool seal) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	if (seal) {
		command["command"] = "seal_lobby";
	} else {
		command["command"] = "unseal_lobby";
	}
	Dictionary data_dict;
	command["data"] = data_dict;
	data_dict["id"] = id;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::lobby_notify(const Variant &p_peer_data) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "lobby_notify";
	Dictionary data_dict;
	data_dict["peer_data"] = p_peer_data;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::peer_notify(const Variant &p_peer_data, const String &p_target_peer) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "notify_to";
	Dictionary data_dict;
	data_dict["peer_data"] = p_peer_data;
	data_dict["target_peer"] = p_target_peer;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::add_user_data(const Dictionary &p_user_data) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "user_data";
	Dictionary data_dict;
	data_dict["user_data"] = p_user_data;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::del_user_data(const TypedArray<String> &p_keys) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "user_data";
	Dictionary data_dict;
	Dictionary data_object_dict;
	data_dict["user_data"] = data_object_dict;
	// set null value
	for (int i = 0; i < p_keys.size(); i++) {
		data_object_dict[p_keys[i]] = Variant();
	}
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::lobby_data(const Dictionary &p_lobby_data, bool p_is_private) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "lobby_data";
	Dictionary data_dict;
	data_dict["lobby_data"] = p_lobby_data;
	data_dict["is_private"] = p_is_private;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::del_lobby_data(const TypedArray<String> &p_keys, bool p_is_private) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "lobby_data";
	Dictionary data_dict;
	Dictionary data_object_dict;
	data_dict["lobby_data"] = data_object_dict;
	// set null value
	for (int i = 0; i < p_keys.size(); i++) {
		data_object_dict[p_keys[i]] = Variant();
	}
	data_dict["is_private"] = p_is_private;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::set_peer_data(const Dictionary &p_peer_data, const String &p_target_peer, bool p_is_private) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "data_to";
	Dictionary data_dict;
	data_dict["peer_data"] = p_peer_data;
	data_dict["target_peer"] = p_target_peer;
	data_dict["is_private"] = p_is_private;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::del_peer_data(const TypedArray<String> &p_keys, const String &p_target_peer, bool p_is_private) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "data_to";
	Dictionary data_dict;
	Dictionary data_object_dict;
	data_dict["peer_data"] = data_object_dict;
	// set null value
	for (int i = 0; i < p_keys.size(); i++) {
		data_object_dict[p_keys[i]] = Variant();
	}
	data_dict["target_peer"] = p_target_peer;
	data_dict["is_private"] = p_is_private;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::set_peers_data(const Dictionary &p_peer_data, bool p_is_private) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "data_to_all";
	Dictionary data_dict;
	data_dict["peer_data"] = p_peer_data;
	data_dict["is_private"] = p_is_private;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

Ref<LobbyResponse> LobbyClient::del_peers_data(const TypedArray<String> &p_keys, bool p_is_private) {
	Ref<LobbyResponse> response;
	response.instantiate();
	if (!connected) {
		// signal the finish deferred
		Callable callable = callable_mp(*response, &LobbyResponse::signal_finish);
		callable.call_deferred("Not conneceted to the server.");
		return response;
	}
	String id = _increment_counter();
	Dictionary command;
	command["command"] = "data_to_all";
	Dictionary data_dict;
	Dictionary data_object_dict;
	data_dict["peer_data"] = data_object_dict;
	// set null value
	for (int i = 0; i < p_keys.size(); i++) {
		data_object_dict[p_keys[i]] = Variant();
	}
	data_dict["is_private"] = p_is_private;
	data_dict["id"] = id;
	command["data"] = data_dict;
	Array command_array;
	command_array.push_back(LOBBY_REQUEST);
	command_array.push_back(response);
	_commands[id] = command_array;
	_send_data(command);
	return response;
}

void LobbyClient::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			_socket->poll();

			WebSocketPeer::State state = _socket->get_ready_state();
			if (state == WebSocketPeer::STATE_OPEN) {
				if (_commands.has("connect")) {
					Ref<LobbyResponse> response = _commands["connect"];
					Ref<LobbyResponse::LobbyResult> result;
					result.instantiate();
					response->emit_signal("finished", result);
					_commands.erase("connect");
				}
				if (!connected) {
					connected = true;
					emit_signal("log_updated", "connect_to_server", "Connected to: " + server_url);
				}
				while (_socket->get_available_packet_count() > 0) {
					Vector<uint8_t> packet_buffer;
					Error err = _socket->get_packet_buffer(packet_buffer);
					if (err != OK) {
						emit_signal("log_updated", "error", "Unable to get packet.");
						return;
					}
					String packet_string = String::utf8((const char *)packet_buffer.ptr(), packet_buffer.size());
					_receive_data(JSON::parse_string(packet_string));
				}
			} else if (state == WebSocketPeer::STATE_CLOSED) {
				if (_commands.has("disconnect")) {
					Ref<LobbyResponse> response = _commands["disconnect"];
					Ref<LobbyResponse::LobbyResult> result;
					result.instantiate();
					response->emit_signal("finished", result);
					_commands.erase("disconnect");
				}
				_clear_lobby();
				emit_signal("log_updated", "error", _socket->get_close_reason());
				emit_signal("disconnected_from_server", _socket->get_close_reason());
				set_process_internal(false);
				connected = false;
			}
		} break;
	}
}

void LobbyClient::_send_data(const Dictionary &p_data_dict) {
	if (_socket->get_ready_state() != WebSocketPeer::STATE_OPEN) {
		emit_signal("log_updated", "error", "Socket is not ready.");
		return;
	}
	Error err = _socket->send_text(JSON::stringify(p_data_dict));
	if (err != OK) {
		emit_signal("log_updated", "error", "No longer connected.");
		_socket->close(1000, "Disconnected");
	}
}

void LobbyClient::_update_peers(Dictionary p_data_dict, TypedArray<LobbyPeer> &p_peers) {
	Array peers_array = p_data_dict.get("peers", Array());
	TypedArray<LobbyPeer> peers_info;
	p_peers.clear();
	for (int i = 0; i < peers_array.size(); ++i) {
		Ref<LobbyPeer> peer_info = Ref<LobbyPeer>(memnew(LobbyPeer));
		Dictionary peer_dict = peers_array[i];
		peer_info->set_dict(peer_dict);
		if (peer_dict.has("private_data")) {
			peer_data = peer_dict.get("private_data", Dictionary());
		}
		// update self peer
		if (peer_info->get_id() == peer->get_id()) {
			peer->set_dict(peer_info->get_dict());
		}
		p_peers.push_back(peer_info);
	}
}

// TODO optimize this
void sort_peers_by_id(TypedArray<LobbyPeer> &peers) {
	for (int i = 0; i < peers.size(); ++i) {
		for (int j = i + 1; j < peers.size(); ++j) {
			Ref<LobbyPeer> peer_i = peers[i];
			Ref<LobbyPeer> peer_j = peers[j];
			if (peer_i->get_order_id() > peer_j->get_order_id()) {
				Ref<LobbyPeer> temp = peers[i];
				peers[i] = peers[j];
				peers[j] = temp;
			}
		}
	}
}

void LobbyClient::_clear_lobby() {
	lobby->set_dict(Dictionary());
	peers.clear();
	lobbies.clear();
	host_data = Dictionary();
	peer_data = Dictionary();
	peer->set_data(Dictionary());
	peer->set_ready(false);
}

void LobbyClient::_receive_data(const Dictionary &p_dict) {
	String command = p_dict.get("command", "error");
	String message = p_dict.get("message", "");
	Dictionary data_dict = p_dict.get("data", Dictionary());
	String message_id = data_dict.get("id", "");
	Array command_array = _commands.get(message_id, Array());
	_commands.erase(message_id);
	// update lobby and peers
	{
		TypedArray<LobbyPeer> peers_info;
		if (data_dict.has("peers")) {
			// Iterate through peers and populate arrays
			_update_peers(data_dict, peers_info);
			sort_peers_by_id(peers_info);
		}
		if (data_dict.has("lobby")) {
			Dictionary lobby_dict = data_dict.get("lobby", Dictionary());
			Ref<LobbyInfo> lobby_info = Ref<LobbyInfo>(memnew(LobbyInfo));
			lobby_info->set_dict(lobby_dict);
			if (lobby_info->get_id() == lobby->get_id() || command == "lobby_created" || command == "joined_lobby") {
				// Update lobby info because we viewed our own lobby
				lobby->set_dict(lobby_info->get_dict());
				if (lobby_dict.has("private_data")) {
					host_data = lobby_dict.get("private_data", Dictionary());
				}
				peers = peers_info;
			}
		}
	}
	if (command == "peer_state") {
		Dictionary peer_dict = data_dict.get("peer", Dictionary());
		peer->set_dict(peer_dict);
		lobby->set_id(peer_dict.get("lobby_id", ""));
		reconnection_token = peer_dict.get("reconnection_token", "");
		emit_signal("connected_to_server", peer, reconnection_token);
	} else if (command == "lobby_created") {
		lobbies.clear();
		emit_signal("lobby_created", lobby, peers);
	} else if (command == "joined_lobby") {
		lobbies.clear();
		emit_signal("lobby_joined", lobby, peers);
	} else if (command == "lobby_left") {
		_clear_lobby();
		emit_signal("lobby_left", false);
	} else if (command == "lobby_kicked") {
		_clear_lobby();
		emit_signal("lobby_left", true);
	} else if (command == "lobby_sealed") {
		Dictionary lobby_dict = data_dict.get("lobby", Dictionary());
		lobby->set_sealed(true);
		emit_signal("lobby_sealed", true);
	} else if (command == "lobby_unsealed") {
		lobby->set_sealed(false);
		emit_signal("lobby_sealed", false);
	} else if (command == "lobby_tags") {
		lobby->set_tags(data_dict.get("tags", Dictionary()));
		emit_signal("lobby_tagged", lobby->get_tags());
	} else if (command == "lobby_list") {
		Array arr = data_dict.get("lobbies", Array());
		TypedArray<Dictionary> lobbies_input = arr;
		for (int i = 0; i < lobbies_input.size(); ++i) {
			Dictionary lobby_dict = lobbies_input[i];
			if (!lobby_dict.has("name")) {
				// lobby got removed
				// go through every lobby and remove the one with id
				for (int j = 0; j < lobbies.size(); ++j) {
					String lobby_id = lobby_dict.get("id", "");
					Ref<LobbyInfo> lobby_j = lobbies[j];
					if (lobby_j->get_id() == lobby_id) {
						lobbies.remove_at(j);
						break;
					}
				}
				continue;
			}
			bool updated = false;
			// go and see if there already is a lobby in lobbies
			for (int j = 0; j < lobbies.size(); ++j) {
				String lobby_id = lobby_dict.get("id", "");
				Ref<LobbyInfo> lobby_j = lobbies[j];
				if (lobby_j->get_id() == lobby_id) {
					lobby_j->set_dict(lobby_dict);
					updated = true;
					break;
				}
			}
			if (updated) {
				continue;
			}
			// if not, add a new one
			Ref<LobbyInfo> lobby_info;
			lobby_info.instantiate();
			lobby_info->set_dict(lobby_dict);
			lobbies.push_front(lobby_info);
		}
		emit_signal("lobbies_listed", lobbies);
	} else if (command == "peer_chat") {
		String peer_id = data_dict.get("from_peer", "");
		String chat_data = data_dict.get("chat_data", "");
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> found_peer = peers[i];
			if (found_peer->get_id() == peer_id) {
				emit_signal("peer_messaged", found_peer, chat_data);
				break;
			}
		}
	} else if (command == "peer_user_data") {
		String peer_id = data_dict.get("peer_id", "");
		Dictionary peer_user_data = data_dict.get("user_data", "");
		bool notified = false;
		if (peer->get_id() == peer_id) {
			peer->set_user_data(peer_user_data);
			// notify self
			emit_signal("received_peer_user_data", peer, peer_user_data);
			notified = true;
		}
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> updated_peer = peers[i];
			if (updated_peer->get_id() == peer_id) {
				updated_peer->set_user_data(peer_user_data);
				if (!notified) {
					emit_signal("received_peer_user_data", updated_peer, peer_user_data);
				}
				break;
			}
		}
	} else if (command == "peer_ready") {
		String peer_id = data_dict.get("peer_id", "");
		if (peer->get_id() == peer_id) {
			peer->set_ready(true);
		}
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> updated_peer = peers[i];
			if (updated_peer->get_id() == String(peer_id)) {
				updated_peer->set_ready(true);
				emit_signal("peer_ready", updated_peer, true);
				break;
			}
		}
	} else if (command == "peer_unready") {
		String peer_id = data_dict.get("peer_id", "");
		if (peer->get_id() == peer_id) {
			peer->set_ready(false);
		}
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> updated_peer = peers[i];
			if (updated_peer->get_id() == String(data_dict.get("peer_id", ""))) {
				updated_peer->set_ready(false);
				emit_signal("peer_ready", updated_peer, false);
				break;
			}
		}
	} else if (command == "peer_joined") {
		Ref<LobbyPeer> joining_peer = Ref<LobbyPeer>(memnew(LobbyPeer));
		Dictionary peer_dict = data_dict.get("peer", Dictionary());
		joining_peer->set_id(peer_dict.get("id", ""));
		joining_peer->set_user_data(peer_dict.get("user_data", ""));
		peers.append(joining_peer);
		sort_peers_by_id(peers);
		lobby->set_players(peers.size());
		emit_signal("peer_joined", joining_peer);
	} else if (command == "peer_reconnected") {
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> updated_peer = peers[i];
			if (updated_peer->get_id() == String(data_dict.get("peer_id", ""))) {
				peer->set_disconnected(false);
				emit_signal("peer_reconnected", updated_peer);
				break;
			}
		}
	} else if (command == "peer_left") {
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> leaving_peer = peers[i];
			if (leaving_peer->get_id() == String(data_dict.get("peer_id", ""))) {
				peers.remove_at(i);
				lobby->set_players(peers.size());
				emit_signal("peer_left", leaving_peer, data_dict.get("kicked", false));
				break;
			}
		}
		sort_peers_by_id(peers);
	} else if (command == "peer_disconnected") {
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> leaving_peer = peers[i];
			if (leaving_peer->get_id() == String(data_dict.get("peer_id", ""))) {
				peer->set_disconnected(true);
				emit_signal("peer_disconnected", leaving_peer);
				break;
			}
		}
		sort_peers_by_id(peers);
	} else if (command == "peer_notify") {
		String from_peer_id = data_dict.get("from_peer", "");
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> from_peer = peers[i];
			if (from_peer->get_id() == from_peer_id) {
				emit_signal("lobby_notified", data_dict.get("peer_data", Variant()), from_peer);
				break;
			}
		}
	} else if (command == "lobby_data") {
		Dictionary lobby_data = data_dict.get("lobby_data", Dictionary());
		bool is_private = data_dict.get("is_private", false);
		if (is_private) {
			host_data = lobby_data;
		} else {
			lobby->set_data(lobby_data);
		}
		emit_signal("received_lobby_data", lobby_data, is_private);
		// nothing for now
	} else if (command == "data_to") {
		String target_peer_id = data_dict.get("target_peer", "");
		bool is_private = data_dict.get("is_private", false);
		Dictionary peer_data_variant = data_dict.get("peer_data", Dictionary());
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> updated_peer = peers[i];
			if (updated_peer->get_id() == target_peer_id) {
				// got peer data, update it
				if (is_private && target_peer_id == peer->get_id()) {
					// private data, update self
					peer_data = peer_data_variant;
				} else {
					// public peer data
					updated_peer->set_data(peer_data_variant);
				}
				emit_signal("received_peer_data", peer_data_variant, updated_peer, is_private);
				break;
			}
		}
	} else if (command == "notified_to") {
		String from_peer_id = data_dict.get("from_peer", "");
		for (int i = 0; i < peers.size(); ++i) {
			Ref<LobbyPeer> from_peer = peers[i];
			if (from_peer->get_id() == from_peer_id) {
				emit_signal("lobby_notified", data_dict.get("peer_data", Variant()), from_peer);
				break;
			}
		}
	} else if (command == "data_to_sent") {
		// nothing for now
	} else if (command == "lobby_notify_sent") {
		// nothing for now
	} else if (command == "notify_to_sent") {
		// nothing for now
	} else if (command == "error") {
		if (command_array.size() == 2) {
			int command_type = command_array[0];
			switch (command_type) {
				case LOBBY_REQUEST: {
					Ref<LobbyResponse> lobby_response = command_array[1];
					if (lobby_response.is_valid()) {
						Ref<LobbyResponse::LobbyResult> result;
						result.instantiate();
						result->set_error(message);
						lobby_response->emit_signal("finished", result);
					}
				} break;
				case LOBBY_VIEW: {
					Ref<ViewLobbyResponse> view_response = command_array[1];
					if (view_response.is_valid()) {
						Ref<ViewLobbyResponse::ViewLobbyResult> result;
						result.instantiate();
						result->set_error(message);
						view_response->emit_signal("finished", result);
					}
				} break;
			}
		}
	} else {
		emit_signal("log_updated", "error", "Unknown command received.");
	}
	emit_signal("log_updated", command, message);
	if (command_array.size() == 2 && command != "error") {
		int command_type = command_array[0];
		switch (command_type) {
			case LOBBY_REQUEST: {
				Ref<LobbyResponse> response = command_array[1];
				if (response.is_valid()) {
					Ref<LobbyResponse::LobbyResult> result = Ref<LobbyResponse::LobbyResult>(memnew(LobbyResponse::LobbyResult));
					response->emit_signal("finished", result);
				}
			} break;
			case LOBBY_VIEW: {
				Dictionary lobby_dict = data_dict.get("lobby", Dictionary());

				// Iterate through peers and populate arrays
				TypedArray<LobbyPeer> peers_info;
				_update_peers(data_dict, peers_info);
				sort_peers_by_id(peers_info);
				Ref<LobbyInfo> lobby_info = Ref<LobbyInfo>(memnew(LobbyInfo));
				lobby_info->set_dict(lobby_dict);
				// notify
				Ref<ViewLobbyResponse> response = command_array[1];
				if (response.is_valid()) {
					Ref<ViewLobbyResponse::ViewLobbyResult> result;
					result.instantiate();
					result->set_peers(peers_info);
					result->set_lobby(lobby_info);
					response->emit_signal("finished", result);
				}
			} break;
		}
	}
}
