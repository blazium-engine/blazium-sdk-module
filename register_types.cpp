/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "core/io/resource_importer.h"
#include "register_types.h"
#include "blazium_client.h"
#include "lobby/scripted_lobby_client.h"
#include "lobby/scripted_lobby_response.h"
#include "lobby/lobby_client.h"
#include "lobby/lobby_info.h"
#include "lobby/lobby_peer.h"
#include "lobby/lobby_response.h"
#include "login/login_client.h"
#include "master_server/master_server_client.h"
#include "pogr/pogr_client.h"
#include "third_party_client.h"
#include "discord/discord_embedded_app_client.h"
#include "discord/discord_embedded_app_response.h"
#include "jwt.h"
#include "env.h"
#include "csv/resource_csv.h"
#include "csv/resource_importer_csv.h"

static JWT *jwt_singleton_global = nullptr;
static ENV *env_singleton_global = nullptr;
static Ref<ResourceImporterCSV> csv_importer;

void initialize_blazium_sdk_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		// JWT singleton
		jwt_singleton_global = memnew(JWT);
		GDREGISTER_CLASS(JWT);
		Engine::get_singleton()->add_singleton(Engine::Singleton("JWT", JWT::get_singleton()));
		// ENV singleton
		env_singleton_global = memnew(ENV);
		GDREGISTER_CLASS(ENV);
		Engine::get_singleton()->add_singleton(Engine::Singleton("ENV", ENV::get_singleton()));
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
		GDREGISTER_CLASS(CSV);
		GDREGISTER_CLASS(ResourceImporterCSV);
		csv_importer.instantiate();
		ResourceFormatImporter::get_singleton()->add_importer(csv_importer);
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		// Blazium clients
		GDREGISTER_ABSTRACT_CLASS(BlaziumClient);
		GDREGISTER_CLASS(LobbyInfo);
		GDREGISTER_CLASS(LobbyPeer);
		GDREGISTER_CLASS(LobbyClient);
		GDREGISTER_CLASS(LobbyResponse::LobbyResult);
		GDREGISTER_CLASS(LobbyResponse);
		GDREGISTER_CLASS(ViewLobbyResponse::ViewLobbyResult);
		GDREGISTER_CLASS(ViewLobbyResponse);
		GDREGISTER_CLASS(ScriptedLobbyClient);
		GDREGISTER_CLASS(ScriptedLobbyResponse);
		GDREGISTER_CLASS(ScriptedLobbyResponse::ScriptedLobbyResult);
		GDREGISTER_CLASS(POGRClient);
		GDREGISTER_CLASS(POGRClient::POGRResponse);
		GDREGISTER_CLASS(POGRClient::POGRResult);
		GDREGISTER_CLASS(GameServerInfo);
		GDREGISTER_CLASS(MasterServerClient);
		GDREGISTER_CLASS(MasterServerClient::MasterServerResponse);
		GDREGISTER_CLASS(MasterServerClient::MasterServerResult);
		GDREGISTER_CLASS(MasterServerClient::MasterServerListResponse);
		GDREGISTER_CLASS(MasterServerClient::MasterServerListResult);
		GDREGISTER_CLASS(LoginClient);
		GDREGISTER_CLASS(LoginClient::LoginResponse);
		GDREGISTER_CLASS(LoginClient::LoginResponse::LoginResult);
		// Third party clients
		GDREGISTER_ABSTRACT_CLASS(ThirdPartyClient);
		GDREGISTER_CLASS(DiscordEmbeddedAppClient);
		GDREGISTER_CLASS(DiscordEmbeddedAppResponse);
		GDREGISTER_CLASS(DiscordEmbeddedAppResponse::DiscordEmbeddedAppResult);
	}
}

void uninitialize_blazium_sdk_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		Engine::get_singleton()->remove_singleton("JWT");
		Engine::get_singleton()->remove_singleton("ENV");
		memdelete(jwt_singleton_global);
		memdelete(env_singleton_global);
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {
		if (csv_importer != nullptr) {
			ResourceFormatImporter::get_singleton()->remove_importer(csv_importer);
			csv_importer.unref();
		}
	}
}
