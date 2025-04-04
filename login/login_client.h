/**************************************************************************/
/*  login_client.h                                                        */
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

#ifndef LOGIN_CLIENT_H
#define LOGIN_CLIENT_H

#include "../blazium_client.h"
#include "../discord/discord_embedded_app_client.h"
#include "core/io/json.h"
#include "modules/websocket/websocket_peer.h"
#include "scene/main/http_request.h"

class LoginClient : public BlaziumClient {
	GDCLASS(LoginClient, BlaziumClient);

protected:
	String override_discord_path = "blazium/login";
	String server_url;
	String websocket_prefix = "wss://";
	String http_prefix = "https://";
	String game_id = "";
	String connect_route = "/connect";
	String access_code_route = "/auth";
	String verify_jwt_route = "/token/verify";
	String refresh_jwt_route = "/token/refresh";
	bool connected = false;

public:
	class LoginConnectResponse : public RefCounted {
		GDCLASS(LoginConnectResponse, RefCounted);

	protected:
		static void _bind_methods() {
			ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "LoginConnectResult")));
		}

	public:
		class LoginConnectResult : public RefCounted {
			GDCLASS(LoginConnectResult, RefCounted);

			String error = "";

		protected:
			static void _bind_methods() {
				ClassDB::bind_method(D_METHOD("has_error"), &LoginConnectResult::has_error);
				ClassDB::bind_method(D_METHOD("get_error"), &LoginConnectResult::get_error);
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
			}

		public:
			void set_error(String p_error) { this->error = p_error; }

			bool has_error() const { return !error.is_empty(); }
			String get_error() const { return error; }
		};
		void signal_finish(String p_error) {
			Ref<LoginConnectResult> result;
			result.instantiate();
			result->set_error(p_error);
			emit_signal("finished", result);
		}
	};

	class LoginURLResponse : public RefCounted {
		GDCLASS(LoginURLResponse, RefCounted);

	protected:
		static void _bind_methods() {
			ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "LoginURLResult")));
		}

	public:
		class LoginURLResult : public RefCounted {
			GDCLASS(LoginURLResult, RefCounted);

			String error = "";
			String login_url = "";
			String login_type = "";

		protected:
			static void _bind_methods() {
				ClassDB::bind_method(D_METHOD("get_login_url"), &LoginURLResult::get_login_url);
				ClassDB::bind_method(D_METHOD("get_login_type"), &LoginURLResult::get_login_type);
				ClassDB::bind_method(D_METHOD("has_error"), &LoginURLResult::has_error);
				ClassDB::bind_method(D_METHOD("get_error"), &LoginURLResult::get_error);
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "login_url"), "", "get_login_url");
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "login_type"), "", "get_login_type");
			}

		public:
			void set_login_type(String p_type) { this->login_type = p_type; }
			void set_login_url(String p_url) { this->login_url = p_url; }
			void set_error(String p_error) { this->error = p_error; }

			bool has_error() const { return !error.is_empty(); }
			String get_error() const { return error; }
			String get_login_url() const { return login_url; }
			String get_login_type() const { return login_type; }
		};
		void signal_finish(String p_error) {
			Ref<LoginURLResult> result;
			result.instantiate();
			result->set_error(p_error);
			emit_signal("finished", result);
		}
	};

	class LoginIDResponse : public RefCounted {
		GDCLASS(LoginIDResponse, RefCounted);

	protected:
		static void _bind_methods() {
			ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "LoginIDResult")));
		}

	public:
		class LoginIDResult : public RefCounted {
			GDCLASS(LoginIDResult, RefCounted);

			String error = "";
			String login_id = "";
			String login_type = "";

		protected:
			static void _bind_methods() {
				ClassDB::bind_method(D_METHOD("get_login_id"), &LoginIDResult::get_login_id);
				ClassDB::bind_method(D_METHOD("get_login_type"), &LoginIDResult::get_login_type);
				ClassDB::bind_method(D_METHOD("has_error"), &LoginIDResult::has_error);
				ClassDB::bind_method(D_METHOD("get_error"), &LoginIDResult::get_error);
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "login_id"), "", "get_login_id");
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "login_type"), "", "get_login_type");
			}

		public:
			void set_login_type(String p_type) { this->login_type = p_type; }
			void set_login_id(String p_id) { this->login_id = p_id; }
			void set_error(String p_error) { this->error = p_error; }

			bool has_error() const { return !error.is_empty(); }
			String get_error() const { return error; }
			String get_login_id() const { return login_id; }
			String get_login_type() const { return login_type; }
		};
		void signal_finish(String p_error) {
			Ref<LoginIDResult> result;
			result.instantiate();
			result->set_error(p_error);
			emit_signal("finished", result);
		}
	};

	class LoginAccessTokenResponse : public RefCounted {
		GDCLASS(LoginAccessTokenResponse, RefCounted);
		HTTPRequest *request;
		LoginClient *client;
		String request_command;

	protected:
		static void _bind_methods() {
			ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "LoginAccessTokenResult")));
		}

	public:
		class LoginAccessTokenResult : public RefCounted {
			GDCLASS(LoginAccessTokenResult, RefCounted);

			String error = "";

		protected:
			static void _bind_methods() {
				ClassDB::bind_method(D_METHOD("has_error"), &LoginAccessTokenResult::has_error);
				ClassDB::bind_method(D_METHOD("get_error"), &LoginAccessTokenResult::get_error);
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
			}

		public:
			void set_error(String p_error) { this->error = p_error; }

			bool has_error() const { return !error.is_empty(); }
			String get_error() const { return error; }
		};
		
		void _on_request_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data) {
			Ref<LoginAccessTokenResult> result;
			result.instantiate();
			String result_str = String::utf8((const char *)p_data.ptr(), p_data.size());
			if (p_code != 200 || result_str == "") {
				result->set_error("Request failed with code: " + String::num(p_code) + " " + result_str);
				client->emit_signal(SNAME("log_updated"), "error", result_str + " " + p_code);
				emit_signal("log_updated", "error", result_str + " " + p_code);
			} else {
				emit_signal("log_updated", "request_access_token", "Success");
			}
			emit_signal(SNAME("finished"), result);
		}
		
		void signal_finish(String p_error) {
			Ref<LoginAccessTokenResult> result;
			result.instantiate();
			result->set_error(p_error);
			emit_signal("finished", result);
		}

		void post_request(String p_url, Dictionary p_data, LoginClient *p_client) {
			client = p_client;
			p_client->add_child(request);
			request->connect("request_completed", callable_mp(this, &LoginAccessTokenResponse::_on_request_completed));
			request->request(p_url, Vector<String>(), HTTPClient::METHOD_POST, JSON::stringify(p_data));
		}
		LoginAccessTokenResponse() {
			request = memnew(HTTPRequest);
		}
		~LoginAccessTokenResponse() {
			request->queue_free();
		}
	};

	class LoginVerifyTokenResponse : public RefCounted {
		GDCLASS(LoginVerifyTokenResponse, RefCounted);
		HTTPRequest *request;
		LoginClient *client;
		String request_command;

	protected:
		static void _bind_methods() {
			ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "LoginVerifyTokenResult")));
		}

	public:
		class LoginVerifyTokenResult : public RefCounted {
			GDCLASS(LoginVerifyTokenResult, RefCounted);

			String error = "";

		protected:
			static void _bind_methods() {
				ClassDB::bind_method(D_METHOD("has_error"), &LoginVerifyTokenResult::has_error);
				ClassDB::bind_method(D_METHOD("get_error"), &LoginVerifyTokenResult::get_error);
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
			}

		public:
			void set_error(String p_error) { this->error = p_error; }

			bool has_error() const { return !error.is_empty(); }
			String get_error() const { return error; }
		};
		
		void _on_request_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data) {
			Ref<LoginVerifyTokenResult> result;
			result.instantiate();
			String result_str = String::utf8((const char *)p_data.ptr(), p_data.size());
			if (p_code != 200 || result_str == "") {
				result->set_error("Request failed with code: " + String::num(p_code) + " " + result_str);
				client->emit_signal(SNAME("log_updated"), "error", result_str + " " + p_code);
			} else {
				if (result_str != "") {
					Dictionary result_dict = JSON::parse_string(result_str);
					// TODO
					print_line(result_dict);
				}
			}
			emit_signal(SNAME("finished"), result);
		}
		
		void signal_finish(String p_error) {
			Ref<LoginVerifyTokenResult> result;
			result.instantiate();
			result->set_error(p_error);
			emit_signal("finished", result);
		}

		void get_request(String p_url, String p_jwt, LoginClient *p_client) {
			client = p_client;
			p_client->add_child(request);
			Vector<String> headers;
			headers.append("JWT_TOKEN: " + p_jwt);
			request->connect("request_completed", callable_mp(this, &LoginVerifyTokenResponse::_on_request_completed));
			request->request(p_url, headers, HTTPClient::METHOD_GET);
		}
		LoginVerifyTokenResponse() {
			request = memnew(HTTPRequest);
		}
		~LoginVerifyTokenResponse() {
			request->queue_free();
		}
	};

	class LoginRefreshTokenResponse : public RefCounted {
		GDCLASS(LoginRefreshTokenResponse, RefCounted);
		HTTPRequest *request;
		LoginClient *client;
		String request_command;

	protected:
		static void _bind_methods() {
			ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "LoginRefreshTokenResult")));
		}

	public:
		class LoginRefreshTokenResult : public RefCounted {
			GDCLASS(LoginRefreshTokenResult, RefCounted);

			String error = "";

		protected:
			static void _bind_methods() {
				ClassDB::bind_method(D_METHOD("has_error"), &LoginRefreshTokenResult::has_error);
				ClassDB::bind_method(D_METHOD("get_error"), &LoginRefreshTokenResult::get_error);
				ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
			}

		public:
			void set_error(String p_error) { this->error = p_error; }

			bool has_error() const { return !error.is_empty(); }
			String get_error() const { return error; }
		};
		
		void _on_request_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data) {
			Ref<LoginRefreshTokenResult> result;
			result.instantiate();
			String result_str = String::utf8((const char *)p_data.ptr(), p_data.size());
			if (p_code != 200 || result_str == "") {
				result->set_error("Request failed with code: " + String::num(p_code) + " " + result_str);
				client->emit_signal(SNAME("log_updated"), "error", result_str + " " + p_code);
			} else {
				if (result_str != "") {
					Dictionary result_dict = JSON::parse_string(result_str);
					print_line(result_dict);
				}
			}
			emit_signal(SNAME("finished"), result);
		}
		
		void signal_finish(String p_error) {
			Ref<LoginRefreshTokenResult> result;
			result.instantiate();
			result->set_error(p_error);
			emit_signal("finished", result);
		}

		void post_request(String p_url, Dictionary p_data, LoginClient *p_client) {
			client = p_client;
			p_client->add_child(request);
			request->connect("request_completed", callable_mp(this, &LoginRefreshTokenResponse::_on_request_completed));
			request->request(p_url, Vector<String>(), HTTPClient::METHOD_POST, JSON::stringify(p_data));
		}
		LoginRefreshTokenResponse() {
			request = memnew(HTTPRequest);
		}
		~LoginRefreshTokenResponse() {
			request->queue_free();
		}
	};

protected:
	Ref<WebSocketPeer> _socket;
	Ref<LoginURLResponse> login_url_response;
	Ref<LoginIDResponse> login_id_response;
	Ref<LoginConnectResponse> connect_response;

	void _receive_data(const Dictionary &p_data) {
		String action = p_data.get("action", "error");
		if (action == "login_url") {
			String url = p_data.get("url", "");
			String type = p_data.get("type", "");
			Ref<LoginURLResponse::LoginURLResult> login_url_result;
			login_url_result.instantiate();
			login_url_result->set_login_url(url);
			login_url_result->set_login_type(type);
			login_url_response->emit_signal("finished", login_url_result);
			emit_signal("log_updated", "request_login_url", "Success");
		}
		if (action == "conn_id") {
			String id = p_data.get("id", "");
			String type = p_data.get("type", "");
			Ref<LoginIDResponse::LoginIDResult> login_id_result;
			login_id_result.instantiate();
			login_id_result->set_login_id(id);
			login_id_result->set_login_type(type);
			login_id_response->emit_signal("finished", login_id_result);
			emit_signal("log_updated", "request_auth_id", "Success");
		}
		if (action == "jwt") {
			String jwt = p_data.get("url", "");
			String type = p_data.get("type", "");
			String access_token = p_data.get("access_token", "");
			if (p_data.has("url")) {
				emit_signal("received_jwt", jwt, type, access_token);
			}
			emit_signal("log_updated", "received_jwt", "Success");
		}
		if (action == "error") {
			String error = p_data.get("error", "");
			if (login_url_response.is_valid()) {
				Ref<LoginURLResponse::LoginURLResult> login_url_result;
				login_url_result.instantiate();
				login_url_result->set_error(error);
				login_url_response->emit_signal("finished", login_url_result);
			}
			if (login_id_response.is_valid()) {
				Ref<LoginIDResponse::LoginIDResult> login_id_result;
				login_id_result.instantiate();
				login_id_result->set_error(error);
				login_id_response->emit_signal("finished", login_id_result);
			}
			emit_signal("log_updated", "error", error);
		}
	}

	void _notification(int p_notification) {
		switch (p_notification) {
			case NOTIFICATION_INTERNAL_PROCESS: {
				_socket->poll();

				WebSocketPeer::State state = _socket->get_ready_state();
				if (state == WebSocketPeer::STATE_OPEN) {
					if (!connected) {
						connected = true;
						if (connect_response.is_valid()) {
							Ref<LoginConnectResponse::LoginConnectResult> connected_result;
							connected_result.instantiate();
							connect_response->emit_signal("finished", connected_result);
						}
						String connect_url = websocket_prefix + server_url + "/connect";
						emit_signal("log_updated", "connect_to_server", "Connected to: " + connect_url);
						emit_signal("connected_to_server");
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
					emit_signal("disconnected_from_server", _socket->get_close_reason());
					set_process_internal(false);
					connected = false;
				}
			} break;
		}
	}
	static void _bind_methods();

	void _send_data(const Dictionary &p_data_dict) {
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

public:
	void set_server_url(const String &p_server_url) { this->server_url = p_server_url; }
	String get_server_url() { return server_url; }
	void set_game_id(const String &p_game_id) { this->game_id = p_game_id; }
	String get_game_id() { return game_id; }

	void set_http_prefix(const String &p_http_prefix) { this->http_prefix = p_http_prefix; }
	String get_http_prefix() { return http_prefix; }

	void set_websocket_prefix(const String &p_websocket_prefix) { this->websocket_prefix = p_websocket_prefix; }
	String get_websocket_prefix() { return websocket_prefix; }

	bool get_connected() { return connected; }

	Ref<LoginConnectResponse> connect_to_server();
	void disconnect_from_server();
	void set_override_discord_path(String p_path) {
		override_discord_path = p_path;
		if (DiscordEmbeddedAppClient::static_is_discord_environment()) {
			server_url = DiscordEmbeddedAppClient::static_find_client_id() + ".discordsays.com/.proxy/" + override_discord_path;
		}
	}
	String get_override_discord_path() const { return override_discord_path; }

	Ref<LoginURLResponse> request_login_info(String p_type) {
		if (!connected) {
			Ref<LoginURLResponse> response = Ref<LoginURLResponse>();
			response.instantiate();
			// signal the finish deferred
			Callable callable = callable_mp(*response, &LoginURLResponse::signal_finish);
			callable.call_deferred("Not connected to login server.");
			return response;
		}
		Dictionary command;
		command["action"] = "getLogin";
		command["type"] = p_type;
		_send_data(command);
		return login_url_response;
	}

	Ref<LoginIDResponse> request_auth_id(String p_type) {
		if (!connected) {
			Ref<LoginIDResponse> response = Ref<LoginIDResponse>();
			response.instantiate();
			// signal the finish deferred
			Callable callable = callable_mp(*response, &LoginIDResponse::signal_finish);
			callable.call_deferred("Not connected to login server.");
			return response;
		}
		Dictionary command;
		command["action"] = "getID";
		command["type"] = p_type;
		_send_data(command);
		return login_id_response;
	}

	Ref<LoginAccessTokenResponse> request_access_token(String p_type, String p_auth_id, String p_code) {
		if (!connected) {
			Ref<LoginAccessTokenResponse> response = Ref<LoginAccessTokenResponse>();
			response.instantiate();
			// signal the finish deferred
			Callable callable = callable_mp(*response, &LoginAccessTokenResponse::signal_finish);
			callable.call_deferred("Not connected to login server.");
			return response;
		}
		Dictionary body_data;
		body_data["code"] = p_code;
		Ref<LoginAccessTokenResponse> response;
		response.instantiate();
		String access_code_route_with_path = access_code_route + "/" + p_type + "/" + p_auth_id;
		response->post_request(http_prefix + server_url + access_code_route_with_path, body_data, this);
		return response;
	}

	Ref<LoginVerifyTokenResponse> verify_jwt_token(String p_jwt) {
		if (!connected) {
			Ref<LoginVerifyTokenResponse> response = Ref<LoginVerifyTokenResponse>();
			response.instantiate();
			// signal the finish deferred
			Callable callable = callable_mp(*response, &LoginVerifyTokenResponse::signal_finish);
			callable.call_deferred("Not connected to login server.");
			return response;
		}
		Ref<LoginVerifyTokenResponse> response;
		response.instantiate();
		response->get_request(http_prefix + server_url + verify_jwt_route, p_jwt, this);
		return response;
	}

	Ref<LoginRefreshTokenResponse> refresh_jwt_token(String p_refresh_token) {
		if (!connected) {
			Ref<LoginRefreshTokenResponse> response = Ref<LoginRefreshTokenResponse>();
			response.instantiate();
			// signal the finish deferred
			Callable callable = callable_mp(*response, &LoginRefreshTokenResponse::signal_finish);
			callable.call_deferred("Not connected to login server.");
			return response;
		}
		Dictionary body_data;
		body_data["refresh_token"] = p_refresh_token;
		Ref<LoginRefreshTokenResponse> response;
		response.instantiate();
		response->post_request(http_prefix + server_url + refresh_jwt_route, body_data, this);
		return response;
	}

	LoginClient() {
		if (DiscordEmbeddedAppClient::static_is_discord_environment()) {
			server_url = DiscordEmbeddedAppClient::static_find_client_id() + ".discordsays.com/.proxy/" + override_discord_path;
		} else {
			server_url = "login.blazium.app";
		}
		_socket = Ref<WebSocketPeer>(WebSocketPeer::create());
		set_process_internal(false);
		login_id_response.instantiate();
		login_url_response.instantiate();
	}

	~LoginClient() {
		_socket->close();
		set_process_internal(false);
	}
};

#endif // LOGIN_CLIENT_H
