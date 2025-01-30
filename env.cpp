/**************************************************************************/
/*  env.cpp                                                               */
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

#include "env.h"
#include "core/io/json.h"
#include "core/core_bind.h"
#include "core/os/os.h"

ENV *ENV::env_singleton = nullptr;

ENV *ENV::get_singleton() {
	return env_singleton;
}
void ENV::_bind_methods() {
    ClassDB::bind_method(D_METHOD("config", "file", "override"), &ENV::config, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("parse", "data"), &ENV::parse);
    ClassDB::bind_method(D_METHOD("populate", "env", "override"), &ENV::populate, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("refresh", "override"), &ENV::refresh, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("clear"), &ENV::clear);
    ClassDB::bind_method(D_METHOD("get_env", "key"), &ENV::get_env);
    ClassDB::bind_method(D_METHOD("set_env", "key", "value"), &ENV::set_env);
    ClassDB::bind_method(D_METHOD("has_env", "key"), &ENV::has_env);

    ClassDB::bind_method(D_METHOD("get_debug"), &ENV::get_debug);
    ClassDB::bind_method(D_METHOD("set_debug", "debug"), &ENV::set_debug);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug"), "set_debug", "get_debug");

    ADD_SIGNAL(MethodInfo("file_loaded", PropertyInfo(Variant::STRING, "file"), PropertyInfo(Variant::DICTIONARY, "env")));
    ADD_SIGNAL(MethodInfo("cleared"));
    ADD_SIGNAL(MethodInfo("refreshed", PropertyInfo(Variant::DICTIONARY, "env")));
    ADD_SIGNAL(MethodInfo("updated", PropertyInfo(Variant::STRING, "key"), PropertyInfo(Variant::STRING, "value")));
}

void ENV::clear() {
    env_vars.clear();
    emit_signal("cleared");
}

Variant ENV::get_env(const String &p_key) { 
    if (env_vars.has(p_key)) {
        return env_vars[p_key];
    }
    return OS::get_singleton()->get_environment(p_key);
}
void ENV::set_env(const String &p_key, const Variant &p_value) {
    env_vars[p_key] = p_value;
    emit_signal("updated", p_key, p_value);    
}

bool ENV::has_env(const String &p_key) {
    return env_vars.has(p_key);
}

void ENV::print_debug(String debug_text) {
    if (!debug) {
        return;
    }
    print_line("[ENV] " + debug_text);
}

Dictionary ENV::config(const String &p_file, bool override) {
    print_debug("Loading .env file: " + p_file);
    Ref<FileAccess> file = FileAccess::open(p_file, FileAccess::READ);
    
    if (!file.is_valid()) {
        print_debug("Failed to open .env file: " + p_file);
        return Dictionary();
    }

    Dictionary env = populate(parse(file->get_as_text()), override);
    emit_signal("file_loaded", p_file, env);
    print_debug("Loaded environment variables from: " + p_file);
    
    return env;
}

Dictionary ENV::parse(const String &p_data) {
    Dictionary env;
    Vector<String> lines = p_data.split("\n");
    
    String current_key;
    String current_value;
    bool multi_line = false;

    for (int i = 0; i < lines.size(); i++) {
        String line = lines[i].strip_edges();

        // Skip empty lines and comments
        if (line.is_empty() || line.begins_with("#")) {
            continue;
        }

        // Detect multi-line values
        if (multi_line) {
            if (line.ends_with("\"") || line.ends_with("'")) {
                current_value += "\n" + line.substr(0, line.length() - 1);
                env[current_key] = current_value;
                multi_line = false;
                print_debug("Multi-line env [" + current_key + "] = " + current_value);
            } else {
                current_value += "\n" + line;
            }
            continue;
        }

        // Find the first `=` sign to split key/value
        int eq_pos = line.find("=");
        if (eq_pos == -1) {
            continue; // Invalid line (no `=`)
        }

        String key = line.substr(0, eq_pos).strip_edges();
        String value = line.substr(eq_pos + 1).strip_edges();

        // Remove inline comments (# after a value)
        int comment_pos = value.find(" #");
        if (comment_pos != -1) {
            value = value.substr(0, comment_pos).strip_edges();
        }

        // Handle quoted values
        if ((value.begins_with("\"") && !value.ends_with("\"")) ||
            (value.begins_with("'") && !value.ends_with("'")) ||
            (value.begins_with("`") && !value.ends_with("`"))) {
            // Start multi-line mode
            current_key = key;
            current_value = value.substr(1); // Remove starting quote
            multi_line = true;
            continue;
        }

        if ((value.begins_with("\"") && value.ends_with("\"")) ||
            (value.begins_with("'") && value.ends_with("'")) ||
            (value.begins_with("`") && value.ends_with("`"))) {
            value = value.substr(1, value.length() - 2);
        }

        // Replace escaped characters
        value = value.replace("\\n", "\n")
                     .replace("\\t", "\t")
                     .replace("\\r", "\r");

        // Store the key-value pair
        env[key] = value;
        print_debug("Parsed env [" + key + "] = " + value);
    }

    return env;
}

Dictionary ENV::populate(const Dictionary &p_env, bool override) {
    Dictionary env;
    for (int i = 0; i < p_env.keys().size(); i++) {
        String key = p_env.keys()[i];
        Variant value = p_env[p_env.keys()[i]];

        if (env_vars.has(key)) {
            if (override) {
                env_vars[key] = value;
                print_debug("Updated env [" + key + "] = " + value.to_json_string());
                emit_signal("updated", key, value);
            }
        } else {
            env_vars[key] = value;
            print_debug("Added new env [" + key + "] = " + value.to_json_string());
            emit_signal("updated", key, value);
        }
    }
    return env;
}

Dictionary ENV::refresh(bool override) {
    for (int i = 0; i < env_files.size(); i++) {
        config(env_files[i], override);
    }
    emit_signal("refreshed", env_vars);
    return env_vars;
}
ENV::ENV() { env_singleton = this; }
ENV::~ENV() { env_singleton = nullptr; }
