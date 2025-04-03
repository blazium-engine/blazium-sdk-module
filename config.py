def can_build(env, platform):
	return True


def configure(env):
	pass


def get_doc_classes():
	return [
		"CSV",
		"ResourceImporterCSV",
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
		"LoginURLResponse",
		"LoginURLResult",
		"LoginConnectResponse",
		"LoginConnectResult",
		"LoginVerifyTokenResponse",
		"LoginVerifyTokenResult",
		"LoginRefreshTokenResponse",
		"LoginRefreshTokenResult",
		"LoginIDResponse",
		"LoginIDResult",
		"LoginAccessTokenResponse",
		"LoginAccessTokenResult",
		"ThirdPartyClient",
		"DiscordEmbeddedAppClient",
		"DiscordEmbeddedAppResponse",
		"DiscordEmbeddedAppResult",
		"YoutubePlayablesClient",
		"YoutubePlayablesResponse",
		"YoutubePlayablesResult",
	]


def get_doc_path():
	return "doc_classes"
