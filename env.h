/**************************************************************************/
/*  env.h                                                                 */
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

#ifndef ENV_H
#define ENV_H

#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/io/file_access.h"
#include "core/variant/typed_array.h"

class ENV : public Object {
	GDCLASS(ENV, Object);
	static ENV *env_singleton;
    bool debug = false;
    Dictionary env_vars = Dictionary();
    TypedArray<String> env_files;
public:
	static ENV *get_singleton();
	static void _bind_methods();

	Dictionary config(const String &p_file, bool override = false);
	Dictionary parse(const String &p_data);
	Dictionary populate(const Dictionary &p_env, bool override = false);
	Dictionary refresh(bool override = false);
	void clear();
    Variant get_env(const String &p_key);
    void set_env(const String &p_key, const Variant &p_value);
	bool has_env(const String &p_key);

    bool get_debug() const { return debug; }
    void set_debug(bool p_debug) { debug = p_debug; }

	void print_debug(String text);

	ENV();
	~ENV();
};

#endif // ENV_H
