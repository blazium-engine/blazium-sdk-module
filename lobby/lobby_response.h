/**************************************************************************/
/*  lobby_response.h                                                      */
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

#pragma once

#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"
#include "lobby_info.h"
#include "lobby_peer.h"

class LobbyResponse : public RefCounted {
	GDCLASS(LobbyResponse, RefCounted);

protected:
	static void _bind_methods() {
		ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "LobbyResult")));
	}

public:
	class LobbyResult : public RefCounted {
		GDCLASS(LobbyResult, RefCounted);

		String error = "";

	protected:
		static void _bind_methods() {
			ClassDB::bind_method(D_METHOD("has_error"), &LobbyResult::has_error);
			ClassDB::bind_method(D_METHOD("get_error"), &LobbyResult::get_error);
			ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
		}

	public:
		void set_error(String p_error) { this->error = p_error; }

		bool has_error() const { return !error.is_empty(); }
		String get_error() const { return error; }
	};
	void signal_finish(String p_error) {
		Ref<LobbyResult> result;
		result.instantiate();
		result->set_error(p_error);
		emit_signal("finished", result);
	}
};

class ViewLobbyResponse : public RefCounted {
	GDCLASS(ViewLobbyResponse, RefCounted);

protected:
	static void _bind_methods() {
		ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "ViewLobbyResult")));
	}

public:
	class ViewLobbyResult : public RefCounted {
		GDCLASS(ViewLobbyResult, RefCounted);
		String error = "";
		TypedArray<LobbyPeer> peers_info = TypedArray<LobbyPeer>();
		Ref<LobbyInfo> lobby_info;

	protected:
		static void _bind_methods() {
			ClassDB::bind_method(D_METHOD("has_error"), &ViewLobbyResult::has_error);
			ClassDB::bind_method(D_METHOD("get_error"), &ViewLobbyResult::get_error);
			ClassDB::bind_method(D_METHOD("get_peers"), &ViewLobbyResult::get_peers);
			ClassDB::bind_method(D_METHOD("get_lobby"), &ViewLobbyResult::get_lobby);
			ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "peers", PROPERTY_HINT_ARRAY_TYPE, "LobbyPeer"), "", "get_peers");
			ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "lobby", PROPERTY_HINT_RESOURCE_TYPE, "LobbyInfo"), "", "get_lobby");
			ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
			ADD_PROPERTY_DEFAULT("lobby", Ref<LobbyInfo>());
		}

	public:
		void set_error(const String &p_error) { this->error = p_error; }
		void set_peers(const TypedArray<LobbyPeer> &p_peers) { this->peers_info = p_peers; }
		void set_lobby(const Ref<LobbyInfo> &p_lobby_info) { this->lobby_info = p_lobby_info; }

		bool has_error() const { return !error.is_empty(); }
		String get_error() const { return error; }
		TypedArray<LobbyPeer> get_peers() const { return peers_info; }
		Ref<LobbyInfo> get_lobby() const { return lobby_info; }
		ViewLobbyResult() {
			lobby_info.instantiate();
		}
		~ViewLobbyResult() {
		}
	};
	void signal_finish(String p_error) {
		Ref<ViewLobbyResult> result;
		result.instantiate();
		result->set_error(p_error);
		emit_signal("finished", result);
	}
};
