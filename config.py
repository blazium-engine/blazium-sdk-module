def can_build(env, platform):
	return True


def configure(env):
	pass


def get_doc_classes():
	return [
		"JWT",
		"ENV",
		"LobbyClient",
		"BlaziumClient",
		"LobbyInfo",
		"LobbyPeer",
		"LobbyResponse",
		"LobbyResult",
		"ViewLobbyResponse",
		"ViewLobbyResult",
		"ScriptedLobbyClient",
		"ScriptedLobbyResponse",
		"ScriptedLobbyResult",
		"POGRClient",
		"POGRResult",
		"POGRResponse",
		"MasterServerClient",
		"MasterServerResult",
		"MasterServerResponse",
		"MasterServerListResult",
		"MasterServerListResponse",
		"GameServerInfo",
		"LoginClient",
		"LoginResponse",
		"LoginResult",
		"ThirdPartyClient",
		"DiscordEmbeddedAppClient",
		"DiscordEmbeddedAppResponse",
		"DiscordEmbeddedAppResult",
	]


def get_doc_path():
	return "doc_classes"
