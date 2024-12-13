def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_classes():
    return [
        "LobbyClient",
        "BlaziumClient",
        "LobbyInfo",
        "LobbyPeer",
        "LobbyResponse",
        "LobbyResult",
        "ListLobbyResponse",
        "ListLobbyResult",
        "ViewLobbyResponse",
        "ViewLobbyResult",
        "AuthoritativeClient",
        "LobbyCallResponse",
        "LobbyCallResult",
        "POGRClient",
        "POGRResult",
        "POGRResponse",
    ]


def get_doc_path():
    return "doc_classes"
