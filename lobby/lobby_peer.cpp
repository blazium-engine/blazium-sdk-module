#include "lobby_peer.h"

void LobbyPeer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_id"), &LobbyPeer::get_id);
	ClassDB::bind_method(D_METHOD("get_user_data"), &LobbyPeer::get_user_data);
	ClassDB::bind_method(D_METHOD("is_ready"), &LobbyPeer::is_ready);
	ClassDB::bind_method(D_METHOD("is_disconnected"), &LobbyPeer::is_disconnected);
	ClassDB::bind_method(D_METHOD("get_data"), &LobbyPeer::get_data);
	ClassDB::bind_method(D_METHOD("get_order_id"), &LobbyPeer::get_order_id);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "id"), "", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "user_data"), "", "get_user_data");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "disconnected"), "", "is_disconnected");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "ready"), "", "is_ready");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data"), "", "get_data");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "order_id"), "", "get_order_id");
}

void LobbyPeer::set_id(const String &p_id) { id = p_id; }
void LobbyPeer::set_order_id(int p_order_id) { order_id = p_order_id; }
void LobbyPeer::set_user_data(const Dictionary &p_user_data) { user_data = p_user_data; }
void LobbyPeer::set_ready(bool p_ready) { ready = p_ready; }
void LobbyPeer::set_disconnected(bool p_disconnected) { disconnected = p_disconnected; }
void LobbyPeer::set_data(const Dictionary &p_data) { data = p_data; }
void LobbyPeer::set_dict(const Dictionary &p_dict) {
	if (p_dict.has("id")) {
		set_id(p_dict.get("id", ""));
	}
	if (p_dict.has("order_id")) {
		set_order_id(p_dict.get("order_id", -1));
	}
	if (p_dict.has("ready")) {
		set_ready(p_dict.get("ready", false));
	}
	if (p_dict.has("user_data")) {
		set_user_data(p_dict.get("user_data", Dictionary()));
	}
	if (p_dict.has("public_data")) {
		set_data(p_dict.get("public_data", Dictionary()));
	}
	if (p_dict.has("is_disconnected")) {
		set_disconnected(p_dict.get("is_disconnected", false));
	}
}
Dictionary LobbyPeer::get_dict() const {
	Dictionary dict;
	dict["order_id"] = get_order_id();
	dict["id"] = get_id();
	dict["user_data"] = get_user_data();
	dict["ready"] = is_ready();
	dict["public_data"] = get_data();
	dict["is_disconnected"] = is_disconnected();
	return dict;
}

Dictionary LobbyPeer::get_data() const { return data; }
String LobbyPeer::get_id() const { return id; }
int LobbyPeer::get_order_id() const { return order_id; }
Dictionary LobbyPeer::get_user_data() const { return user_data; }
bool LobbyPeer::is_ready() const { return ready; }
bool LobbyPeer::is_disconnected() const { return disconnected; }
