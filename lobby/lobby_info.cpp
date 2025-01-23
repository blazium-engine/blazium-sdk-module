/**************************************************************************/
/*  lobby_info.cpp                                                        */
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

#include "lobby_info.h"

void LobbyInfo::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_host"), &LobbyInfo::get_host);
	ClassDB::bind_method(D_METHOD("get_max_players"), &LobbyInfo::get_max_players);
	ClassDB::bind_method(D_METHOD("is_sealed"), &LobbyInfo::is_sealed);
	ClassDB::bind_method(D_METHOD("is_password_protected"), &LobbyInfo::is_password_protected);
	ClassDB::bind_method(D_METHOD("get_id"), &LobbyInfo::get_id);
	ClassDB::bind_method(D_METHOD("get_lobby_name"), &LobbyInfo::get_lobby_name);
	ClassDB::bind_method(D_METHOD("get_players"), &LobbyInfo::get_players);
	ClassDB::bind_method(D_METHOD("get_tags"), &LobbyInfo::get_tags);
	ClassDB::bind_method(D_METHOD("get_data"), &LobbyInfo::get_data);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "id"), "", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "tags"), "", "get_tags");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data"), "", "get_data");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "lobby_name"), "", "get_lobby_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "players"), "", "get_players");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "host"), "", "get_host");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_players"), "", "get_max_players");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "sealed"), "", "is_sealed");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "password_protected"), "", "is_password_protected");
}

void LobbyInfo::set_id(const String &p_id) { id = p_id; }
void LobbyInfo::set_lobby_name(const String &p_lobby_name) { lobby_name = p_lobby_name; }
void LobbyInfo::set_host(const String &p_host) { host = p_host; }
void LobbyInfo::set_max_players(int p_max_players) { max_players = p_max_players; }
void LobbyInfo::set_players(int p_players) { players = p_players; }
void LobbyInfo::set_sealed(bool p_sealed) { sealed = p_sealed; }
void LobbyInfo::set_password_protected(bool p_password_protected) { password_protected = p_password_protected; }
void LobbyInfo::set_tags(const Dictionary &p_tags) { tags = p_tags; }
void LobbyInfo::set_data(const Dictionary &p_data) { data = p_data; }

void LobbyInfo::set_dict(const Dictionary &p_dict) {
	set_host(p_dict.get("host", ""));
	set_max_players(p_dict.get("max_players", 0));
	set_sealed(p_dict.get("sealed", false));
	set_players(p_dict.get("players", 0));
	set_id(p_dict.get("id", ""));
	set_lobby_name(p_dict.get("name", ""));
	set_password_protected(p_dict.get("has_password", false));
	set_tags(p_dict.get("tags", Dictionary()));
	set_data(p_dict.get("public_data", Dictionary()));
}
Dictionary LobbyInfo::get_dict() const {
	Dictionary dict;
	dict["host"] = get_host();
	dict["max_players"] = get_max_players();
	dict["sealed"] = is_sealed();
	dict["players"] = get_players();
	dict["id"] = get_id();
	dict["name"] = get_lobby_name();
	dict["has_password"] = is_password_protected();
	dict["tags"] = get_tags();
	dict["public_data"] = get_data();
	return dict;
}

Dictionary LobbyInfo::get_data() const { return data; }
Dictionary LobbyInfo::get_tags() const { return tags; }
String LobbyInfo::get_id() const { return id; }
String LobbyInfo::get_lobby_name() const { return lobby_name; }
String LobbyInfo::get_host() const { return host; }
int LobbyInfo::get_max_players() const { return max_players; }
int LobbyInfo::get_players() const { return players; }
bool LobbyInfo::is_sealed() const { return sealed; }
bool LobbyInfo::is_password_protected() const { return password_protected; }
