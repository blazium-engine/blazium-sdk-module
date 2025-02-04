/**************************************************************************/
/*  resource_csv.h                                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
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

#ifndef CSV_RESOURCE_H
#define CSV_RESOURCE_H

#include "scene/resources/text_file.h"
#include "core/variant/typed_array.h"
#include "core/variant/dictionary.h"

class CSV : public TextFile {
    GDCLASS(CSV, TextFile);
    TypedArray<Dictionary> rows;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_rows", "rows"), &CSV::set_rows);
        ClassDB::bind_method(D_METHOD("get_rows"), &CSV::get_rows);

        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "rows", PROPERTY_HINT_ARRAY_TYPE, "Dictionary"), "set_rows", "get_rows");
    }

protected:
	virtual bool editor_can_reload_from_file() override { return true; }

public:
    virtual void reload_from_file() override {
        load_text(get_path());
	    Ref<FileAccess> f = FileAccess::open(get_path(), FileAccess::READ);
        Vector<String> header = f->get_csv_line();
        TypedArray<Dictionary> rows;
        do {
            Vector<String> line = f->get_csv_line();
            Dictionary row;
            for (int i = 0; i < line.size(); i++) {
                row[header[i]] = line[i];
            }
            rows.push_back(row);
        } while (!f->eof_reached());
        set_rows(rows);
    }
    void set_rows(const TypedArray<Dictionary> &p_rows) { rows = p_rows; }
    TypedArray<Dictionary> get_rows() const { return rows; }
};
#endif // CSV_RESOURCE_H
