/**************************************************************************/
/*  resource_csv.cpp                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             BLAZIUM ENGINE                             */
/*                        https://http://blazium.app                      */
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

#ifndef CSV_RESOURCE_H
#define CSV_RESOURCE_H

#include "core/io/resource.h"
#include "core/variant/typed_array.h"
#include "core/variant/dictionary.h"

class CSV : public Resource {
    GDCLASS(CSV, Resource);
    TypedArray<Dictionary> rows;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_rows", "rows"), &CSV::set_rows);
        ClassDB::bind_method(D_METHOD("get_rows"), &CSV::get_rows);

        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "rows", PROPERTY_HINT_ARRAY_TYPE, "Dictionary"), "set_rows", "get_rows");
    }

public:
    bool headers = false;
    String delimiter = ",";
    Variant convert_to_variant(String text) {
        if (text.is_valid_float()) {
            return text.to_float();
        }
        if (text.is_valid_int()) {
            return text.to_int();
        }
        if (text.casecmp_to("true") == 0) {
            return true;
        }
        if (text.casecmp_to("false") == 0) {
            return false;
        }
        return text;
    }
    Error load_file(String p_path) {
        Error err;
	    Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ, &err);
        if (err != OK) {
            return err;
        }
        Vector<String> header = f->get_csv_line(delimiter);
        if (!headers) {
            Dictionary row;
            for (int i = 0; i < header.size(); i++) {
                row[i] = convert_to_variant(header[i]);
            }
            rows.push_back(row);
        }
        do {
            Vector<String> line = f->get_csv_line(delimiter);
            Dictionary row;
            for (int i = 0; i < line.size(); i++) {
                if (!headers) {
                    row[i] = convert_to_variant(line[i]);
                } else {
                    row[convert_to_variant(header[i])] = convert_to_variant(line[i]);
                }
            }
            if (!line.is_empty()) {
                rows.push_back(row);
            }
        } while (!f->eof_reached());
        f->close();
        return OK;
    }
    void set_rows(const TypedArray<Dictionary> &p_rows) { rows = p_rows; }
    TypedArray<Dictionary> get_rows() const { return rows; }
};
#endif // CSV_RESOURCE_H
