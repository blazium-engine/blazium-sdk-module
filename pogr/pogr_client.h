/**************************************************************************/
/*  pogr_client.h                                                         */
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

#ifndef POGR_CLIENT_H
#define POGR_CLIENT_H

#include "../blazium_client.h"
#include "core/io/json.h"
#include "core/templates/vector.h"
#include "core/version.h"
#include "main/performance.h"
#include "scene/main/http_request.h"

class POGRClient : public BlaziumClient {
	GDCLASS(POGRClient, BlaziumClient);

private:
	String session_id;
	String POGR_URL = "https://api.pogr.io/v1/intake";
	String pogr_client_id;
	String pogr_build;
	Array valid_tags;

	Vector<String> get_init_headers() {
		Vector<String> headers;
		headers.append("POGR_CLIENT: " + pogr_client_id);
		headers.append("POGR_BUILD: " + pogr_build);
		return headers;
	}

	Vector<String> get_session_headers() {
		Vector<String> headers;
		headers.append("INTAKE_SESSION_ID: " + session_id);
		return headers;
	}

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("init", "association_id"), &POGRClient::init);
		ClassDB::bind_method(D_METHOD("end"), &POGRClient::end);
		ClassDB::bind_method(D_METHOD("data", "tags", "data"), &POGRClient::data);
		ClassDB::bind_method(D_METHOD("event", "event_name", "sub_event", "event_key", "flag", "type", "tags", "data"), &POGRClient::event, DEFVAL("user-event"), DEFVAL(Dictionary()), DEFVAL(Dictionary()));
		ClassDB::bind_method(D_METHOD("logs", "log", "severity", "environment", "service", "type", "tags", "data"), &POGRClient::logs, DEFVAL("info"), DEFVAL("dev"), DEFVAL("gameclient"), DEFVAL("user-event"), DEFVAL(Dictionary()), DEFVAL(Dictionary()));
		ClassDB::bind_method(D_METHOD("metrics", "metrics", "environment", "service", "tags"), &POGRClient::metrics, DEFVAL("dev"), DEFVAL("gameclient"), DEFVAL(Dictionary()));
		ClassDB::bind_method(D_METHOD("monitor", "settings"), &POGRClient::monitor);

		ClassDB::bind_method(D_METHOD("get_client_id"), &POGRClient::get_client_id);
		ClassDB::bind_method(D_METHOD("set_client_id", "client_id"), &POGRClient::set_client_id);
		ClassDB::bind_method(D_METHOD("get_build_id"), &POGRClient::get_build_id);
		ClassDB::bind_method(D_METHOD("set_build_id", "build_id"), &POGRClient::set_build_id);
		ClassDB::bind_method(D_METHOD("get_pogr_url"), &POGRClient::get_pogr_url);
		ClassDB::bind_method(D_METHOD("get_session_id"), &POGRClient::get_session_id);

		ADD_PROPERTY(PropertyInfo(Variant::STRING, "client_id"), "set_client_id", "get_client_id");
		ADD_PROPERTY(PropertyInfo(Variant::STRING, "build_id"), "set_build_id", "get_build_id");

		ADD_SIGNAL(MethodInfo("log_updated", PropertyInfo(Variant::STRING, "command"), PropertyInfo(Variant::STRING, "logs")));
	}

public:
	class POGRResult : public RefCounted {
		GDCLASS(POGRResult, RefCounted);
		String error;
		String result;

	protected:
		static void _bind_methods() {
			ClassDB::bind_method(D_METHOD("has_error"), &POGRResult::has_error);
			ClassDB::bind_method(D_METHOD("get_error"), &POGRResult::get_error);
			ClassDB::bind_method(D_METHOD("get_result"), &POGRResult::get_result);
			ADD_PROPERTY(PropertyInfo(Variant::STRING, "error"), "", "get_error");
			ADD_PROPERTY(PropertyInfo(Variant::STRING, "result"), "", "get_result");
		}

	public:
		void set_error(String p_error) { error = p_error; }
		void set_result(String p_result) { result = p_result; }

		bool has_error() const { return error != ""; }
		String get_error() const { return error; }
		String get_result() const { return result; }
	};
	class POGRResponse : public RefCounted {
		GDCLASS(POGRResponse, RefCounted);
		HTTPRequest *request;
		POGRClient *client;
		String request_command;
	protected:
		static void _bind_methods() {
			ADD_SIGNAL(MethodInfo("finished", PropertyInfo(Variant::OBJECT, "result", PROPERTY_HINT_RESOURCE_TYPE, "POGRResult")));
		}

	public:
		void _on_request_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data) {
			Ref<POGRResult> result;
			result.instantiate();
			String result_str = String::utf8((const char *)p_data.ptr(), p_data.size());
			// remove \n character
			if (result_str.size() > 0 && result_str[result_str.length() - 1] == '\n') {
				result_str = result_str.substr(0, result_str.length() - 1);
			}
			if (p_code != 200 || result_str == "") {
				result->set_error("Request failed with code: " + String::num(p_code) + " " + result_str);
				client->emit_signal(SNAME("log_updated"), "error", result_str + " " + p_code);
			} else {
				if (result_str != "") {
					Dictionary result_dict = JSON::parse_string(result_str);
					if (!result_dict.get("success", false)) {
						String error = result_dict.get("error", itos(p_code));
						result->set_error(error);
						client->emit_signal(SNAME("log_updated"), "error", result_dict);
					} else {
						result->set_result(result_str);
						client->emit_signal(SNAME("log_updated"), request_command, JSON::stringify(result_dict.get("payload", "")));
						if (request_command == "end") {
							client->session_id = "";
						}
					}
				}
			}
			emit_signal(SNAME("finished"), result);
		}
		void post_request(String p_pogr_url, String p_command, Vector<String> p_headers, Dictionary p_data, POGRClient *p_client) {
			client = p_client;
			p_client->add_child(request);
			request_command = p_command;
			request->connect("request_completed", callable_mp(this, &POGRResponse::_on_request_completed));
			request->request(p_pogr_url + "/" +p_command, p_headers, HTTPClient::METHOD_POST, JSON::stringify(p_data));
		}
		void signal_finish(String p_error) {
			Ref<POGRResult> result;
			result.instantiate();
			result->set_error(p_error);
			emit_signal("finished", result);
		}
		POGRResponse() {
			request = memnew(HTTPRequest);
		}
		~POGRResponse() {
			request->queue_free();
		}
	};

	bool _validate_tags(Dictionary p_tags) {
		Array keys = p_tags.keys();
		for (int i = 0; i < p_tags.size(); i++) {
			String key = keys[i];
			if (valid_tags.find(key) == -1) {
				return false;
			}
		}
		return true;
	}

	Ref<POGRResponse> init(String p_association_id) {
		Ref<POGRResponse> response;
		response.instantiate();
		response->connect("finished", callable_mp(this, &POGRClient::init_finished));
		Dictionary dict_data;
		if (p_association_id != "") {
			dict_data["association_id"] = p_association_id;
		}
		response->post_request(POGR_URL, "init", get_init_headers(), dict_data, this);
		return response;
	}

	void init_finished(Ref<POGRResult> result) {
		String result_str = result->get_result();
		if (result.is_valid() && result_str != "") {
			Dictionary result_dict = JSON::parse_string(result_str);
			Dictionary payload = result_dict.get("payload", Dictionary());
			set_session_id(payload.get("session_id", ""));
		}
	}

	Ref<POGRResponse> data(Dictionary tags_data, Dictionary p_data) {
		Ref<POGRResponse> response;
		response.instantiate();
		if (session_id == "") {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Session id is invalid. Call init first.");
			return response;
		}
		if (!_validate_tags(tags_data)) {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Invalid tags.");
			return response;
		}
		Dictionary data;
		data["data"] = p_data;
		data["tags"] = tags_data;
		response->post_request(POGR_URL, "data", get_session_headers(), data, this);
		return response;
	}

	Ref<POGRResponse> end() {
		Ref<POGRResponse> response;
		response.instantiate();
		if (session_id == "") {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Session id is invalid. Call init first.");
			return response;
		}
		response->post_request(POGR_URL, "end", get_session_headers(), Dictionary(), this);
		return response;
	}

	Ref<POGRResponse> event(String event_name, String sub_event, String event_key, String event_flag, String event_type, Dictionary p_tags, Dictionary event_data) {
		Ref<POGRResponse> response;
		response.instantiate();
		if (session_id == "") {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Session id is invalid. Call init first.");
			return response;
		}
		if (!_validate_tags(p_tags)) {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Invalid tags.");
			return response;
		}
		Dictionary data;
		data["event"] = event_name;
		data["event_data"] = event_data;
		data["event_flag"] = event_flag;
		data["event_key"] = event_key;
		data["event_type"] = event_type;
		data["sub_event"] = sub_event;
		data["tags"] = p_tags;
		response->post_request(POGR_URL, "event", get_session_headers(), data, this);
		return response;
	}

	Ref<POGRResponse> logs(String p_log, String p_severity, String p_environment, String p_service, String p_type, Dictionary p_tags, Dictionary p_data) {
		Ref<POGRResponse> response;
		response.instantiate();
		if (session_id == "") {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Session id is invalid. Call init first.");
			return response;
		}
		if (!_validate_tags(p_tags)) {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Invalid tags.");
			return response;
		}
		Dictionary data;
		data["tags"] = p_tags;
		data["data"] = p_data;
		data["environment"] = p_environment;
		data["log"] = p_log;
		data["service"] = p_service;
		data["severity"] = p_severity;
		data["type"] = p_type;
		response->post_request(POGR_URL, "logs", get_session_headers(), data, this);
		return response;
	}

	Ref<POGRResponse> metrics(Dictionary p_metrics, String p_environment, String p_service, Dictionary p_tags) {
		Ref<POGRResponse> response;
		response.instantiate();
		if (session_id == "") {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Session id is invalid. Call init first.");
			return response;
		}
		if (!_validate_tags(p_tags)) {
			// signal the finish deferred
			Callable callable = callable_mp(*response, &POGRResponse::signal_finish);
			callable.call_deferred("Invalid tags.");
			return response;
		}
		Dictionary data;
		data["tags"] = p_tags;
		data["environment"] = p_environment;
		data["metrics"] = p_metrics;
		data["service"] = p_service;
		response->post_request(POGR_URL, "metrics", get_session_headers(), data, this);
		return response;
	}

	Ref<POGRResponse> monitor(Dictionary p_settings) {
		Ref<POGRResponse> response;
		response.instantiate();
		Dictionary data;
		data["settings"] = p_settings;
		data["cpu_usage"] = Performance::get_singleton()->get_monitor(Performance::Monitor::TIME_FPS);
		data["dlls_loaded"] = Array();
		data["memory_usage"] = OS::get_singleton()->get_static_memory_usage();
		response->post_request(POGR_URL, "monitor", get_session_headers(), data, this);
		return response;
	}

	String get_client_id() const { return pogr_client_id; }
	String get_build_id() const { return pogr_build; }
	void set_client_id(String p_client_id) { pogr_client_id = p_client_id; }
	void set_build_id(String p_build_id) { pogr_build = p_build_id; }
	String get_pogr_url() const { return POGR_URL; }
	String get_session_id() {
		return session_id;
	}
	void set_session_id(String p_session_id) {
		if (p_session_id == "") {
			return;
		}
		session_id = p_session_id;
	}

	POGRClient() {
		valid_tags.append("steam_id");
		valid_tags.append("twitch_id");
		valid_tags.append("association_id");
		valid_tags.append("pogr_game_session");
		valid_tags.append("xbox_id");
		valid_tags.append("battlenet_id");
		valid_tags.append("twitter_id");
		valid_tags.append("linkedin_id");
		valid_tags.append("pogr_player_id");
		valid_tags.append("discord_id");
		valid_tags.append("override_timestamp");
	}
};

#endif // POGR_CLIENT_H
