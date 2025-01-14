/**************************************************************************/
/*  blazium_client.h                                                      */
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

#ifndef JWT_H
#define JWT_H

#include "scene/main/node.h"
#include "core/core_bind.h"

class JWT : public Object {
	GDCLASS(JWT, Object);

public:
	static JWT *singleton;
    static JWT *get_singleton() {
        return singleton;
    }
	static void _bind_methods() {

        ClassDB::bind_method(D_METHOD("get_header", "jwt"), &JWT::get_header);
        ClassDB::bind_method(D_METHOD("get_payload", "jwt"), &JWT::get_payload);
    }

    Dictionary get_header(const String &p_jwt) {
        // split first portion
        Vector<String> split = p_jwt.split(".");
        if (split.size() < 2) {
            return {};
        }
        core_bind::Marshalls *singleton = core_bind::Marshalls::get_singleton();
        if (singleton == nullptr) {
            ERR_PRINT("Failed to get Marshalls singleton.");
        }
        return singleton->base64_to_variant(split[0]);
    }
    Dictionary get_payload(const String &p_jwt) {
        // split first portion
        Vector<String> split = p_jwt.split(".");
        if (split.size() < 2) {
            return {};
        }
        core_bind::Marshalls *singleton = core_bind::Marshalls::get_singleton();
        if (singleton == nullptr) {
            ERR_PRINT("Failed to get Marshalls singleton.");
        }
        return singleton->base64_to_variant(split[1]);
    }
    JWT() { singleton = this; }
    ~JWT() { singleton = nullptr; }
};

#endif // JWT_H
