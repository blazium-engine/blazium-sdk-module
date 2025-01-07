/**************************************************************************/
/*  lobby_client.h                                                        */
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

#ifndef SCRIPTED_LOBBY_CLIENT_H
#define SCRIPTED_LOBBY_CLIENT_H

#include "scripted_lobby_response.h"
#include "lobby_client.h"

class ScriptedLobbyClient : public BlaziumClient {
	GDCLASS(ScriptedLobbyClient, BlaziumClient);

protected:
	String server_url = "wss://scriptedlobby.blazium.app/connect";
	String reconnection_token = "";
	String game_id = "";
	Dictionary peer_data = Dictionary();
	Ref<LobbyInfo> lobby;
	Ref<LobbyPeer> peer;
	TypedArray<LobbyInfo> lobbies = TypedArray<LobbyInfo>();
	TypedArray<LobbyPeer> peers = TypedArray<LobbyPeer>();

	Ref<WebSocketPeer> _socket;
	int _counter = 0;
	bool connected = false;
	Dictionary _commands;

	void _clear_lobby();
	void _receive_data(const Dictionary &p_data);
	void _send_data(const Dictionary &p_data);
	void _update_peers(Dictionary p_data_dict, TypedArray<LobbyPeer> &peers);
	String _increment_counter();

	enum CommandType {
		LOBBY_REQUEST = 0,
		LOBBY_VIEW,
		LOBBY_CALL
	};

	void _notification(int p_notification);
	static void _bind_methods();

public:
	void set_server_url(const String &p_server_url);
	String get_server_url();
	void set_reconnection_token(const String &p_reconnection_token);
	String get_reconnection_token();
	void set_game_id(const String &p_game_id);
	String get_game_id();
	bool is_host();
	bool get_connected();
	Dictionary get_peer_data();
	void set_lobby(const Ref<LobbyInfo> &p_lobby);
	Ref<LobbyInfo> get_lobby();
	void set_peer(const Ref<LobbyPeer> &p_peer);
	Ref<LobbyPeer> get_peer();
	TypedArray<LobbyPeer> get_peers();

	Ref<LobbyResponse> connect_to_lobby();
	Ref<LobbyResponse> disconnect_from_lobby();
	Ref<ViewLobbyResponse> create_lobby(const String &p_name, bool p_sealed, const Dictionary &p_tags, int p_max_players, const String &p_password);
	Ref<ViewLobbyResponse> join_lobby(const String &p_lobby_id, const String &p_password);
	Ref<LobbyResponse> leave_lobby();
	Ref<LobbyResponse> list_lobby();
	Ref<LobbyResponse> kick_peer(const String &p_peer_id);

	Ref<LobbyResponse> add_user_data(const Dictionary &p_user_data);
	Ref<LobbyResponse> del_user_data(const TypedArray<String> &p_keys);

	Ref<LobbyResponse> set_lobby_tags(const Dictionary &p_tags);
	Ref<LobbyResponse> del_lobby_tags(const TypedArray<String> &p_keys);

	Ref<LobbyResponse> lobby_chat(const String &chat_message);
	Ref<LobbyResponse> lobby_ready(bool p_ready);
	Ref<LobbyResponse> seal_lobby(bool seal);
	Ref<ScriptedLobbyResponse> lobby_call(const String &p_method, const Array &p_args);

	ScriptedLobbyClient();
	~ScriptedLobbyClient();
};

#endif // SCRIPTED_LOBBY_CLIENT_H
