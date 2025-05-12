/**************************************************************************/
/*  lobby_peer.h                                                          */
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

#ifndef LOBBY_PEER_H
#define LOBBY_PEER_H

#include "core/io/resource.h"

class LobbyPeer : public Resource {
	GDCLASS(LobbyPeer, Resource);
	String id = "";
	int order_id = -1;
	bool disconnected = false;
	bool ready = false;
	String platform = "";
	Dictionary user_data = Dictionary();
	Dictionary data = Dictionary();

protected:
	static void _bind_methods();

public:
	void set_order_id(int p_order_id);
	void set_id(const String &p_id);
	void set_ready(bool p_ready);
	void set_platform(const String &p_platform);
	void set_disconnected(bool p_disconnected);
	void set_data(const Dictionary &p_data);
	void set_dict(const Dictionary &p_dict);
	void set_user_data(const Dictionary &p_data);

	Dictionary get_dict() const;
	Dictionary get_data() const;
	String get_id() const;
	String get_platform() const;
	bool is_disconnected() const;
	bool is_ready() const;
	int get_order_id() const;
	Dictionary get_user_data() const;
};

#endif // LOBBY_PEER_H
