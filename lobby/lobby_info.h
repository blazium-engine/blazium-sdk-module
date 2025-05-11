/**************************************************************************/
/*  lobby_info.h                                                          */
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

#include "core/io/resource.h"

class LobbyInfo : public Resource {
	GDCLASS(LobbyInfo, Resource);
	String id = "";
	String lobby_name = "";
	String host = "";
	Dictionary tags = Dictionary();
	Dictionary data = Dictionary();
	int max_players = 0;
	int players = 0;
	bool sealed = false;
	bool password_protected = false;

protected:
	static void _bind_methods();

public:
	void set_id(const String &p_id);
	void set_lobby_name(const String &p_lobby_name);
	void set_host(const String &p_host);
	void set_max_players(int p_max_players);
	void set_players(int p_players);
	void set_sealed(bool p_sealed);
	void set_password_protected(bool p_password_protected);
	void set_tags(const Dictionary &p_tags);
	void set_data(const Dictionary &p_data);

	void set_dict(const Dictionary &p_dict);
	Dictionary get_dict() const;

	Dictionary get_data() const;
	Dictionary get_tags() const;
	String get_id() const;
	String get_lobby_name() const;
	String get_host() const;
	int get_max_players() const;
	int get_players() const;
	bool is_sealed() const;
	bool is_password_protected() const;
};
