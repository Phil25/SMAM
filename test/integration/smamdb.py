#!/usr/bin/env python3

import pytest
from pathlib import Path
from pytest_httpserver import HTTPServer
from collections import namedtuple

WebMock = namedtuple("WebMock", ["url", "filename"])

@pytest.fixture(scope="session")
def server(request):
    with HTTPServer(port=2333) as server:
        yield server

@pytest.fixture(scope="function", autouse=True)
def validate_server(server):
    yield

    handlers = server.oneshot_handlers

    try:
        if len(handlers) > 0:
            with server.wait(timeout=0):
                pass

    except AssertionError as e:
        formatted_handlers = "\n".join([f"    {h.matcher}" for h in handlers])
        raise AssertionError(f"Unmached handlers left:\n{formatted_handlers}") from e

    finally:
        server.clear()

def build_smamdb_response(db, ids, include_deps):
    def get_deps(i):
        left = set(db.get(i, {}).get("deps", []))
        rval = set()
        while left:
            subi = left.pop()
            if subi != i:
                rval.add(subi)
                to_add = set(db.get(subi, {}).get("deps", []))
                left.update(to_add.difference(rval))
        return rval

    effective_ids = set(ids)
    if include_deps:
        for i in ids:
            effective_ids.update(get_deps(i))

    resp = {}

    for i in effective_ids:
        assert i in db.keys(), f"ID '{i}' not found in database."
        resp[i] = db[i]

    return resp

@pytest.fixture(scope="session")
def smamdb(server):
    class SMAMDB:
        def expect_ids(self, ids, include_deps=True, thirdparty_call_override=None):
            db_url = f"https://smamdb.net/?ids={','.join(ids)}"
            expected_response = build_smamdb_response(__DATABASE__, ids, include_deps)

            server \
                .expect_oneshot_request("/", query_string={"external": db_url}) \
                .respond_with_json(expected_response)

            for i in expected_response.keys():
                assert i in __THIRDPARTY_WEBSITES__.keys()

                if thirdparty_call_override is not None:
                    if i not in thirdparty_call_override:
                        continue

                test = False
                for url, filename in __THIRDPARTY_WEBSITES__[i]:
                    path = Path(__file__).parent / "websites" / str(filename)
                    server \
                        .expect_oneshot_request("/", query_string={"external": url.replace("&", "-")}) \
                        .respond_with_data(path.read_bytes())

    return SMAMDB()

__DATABASE__ = {
    "advancedinfiniteammo": {
        "id": "advancedinfiniteammo",
        "author": "Tylerst",
        "description": "Infinite usage for just about everything.",
        "url": "https://forums.alliedmods.net/showpost.php?p=1754217",
        "files": ["plugins/;AdvancedInfiniteAmmo.smx"]
    },
    "afk_manager": {
        "id": "afk_manager",
        "author": "Rothgar",
        "description": "AFK manager/kicker.",
        "url": "https://forums.alliedmods.net/showpost.php?p=708265",
        "files": [
            "plugins/;http://afkmanager.dawgclan.net/plugins/afk_manager4.smx",
            "translations/;http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt"
        ]
    },
    "dynamic_motd": {
        "id": "dynamic_motd",
        "author": "psychonic",
        "description": "Allows sending of player/server info via GET vars to the MOTD url for easy customization of MOTD per player/server.",
        "url": "https://forums.alliedmods.net/showpost.php?p=1387386",
        "files": ["plugins/;dynamic_motd.smx"]
    },
    "funcommandsx": {
        "id": "funcommandsx",
        "author": "Spazman0&Arg!",
        "description": "Extension of SourceMod fun commands.",
        "url": "https://forums.alliedmods.net/showpost.php?p=665771",
        "files": ["./;funcommandsX_.*.zip"]
    },
    "tf2attributes": {
        "id": "tf2attributes",
        "author": "FlaminSarge",
        "description": "Team Fortress 2 attribute management library.",
        "url": "https://github.com/FlaminSarge/tf2attributes/",
        "files": [
            "plugins/;tf2attributes.smx",
            "gamedata/;https://raw.githubusercontent.com/FlaminSarge/tf2attributes/master/tf2.attributes.txt"
        ]
    },
    "tf2items": {
        "id": "tf2items",
        "author": "asherkin",
        "description": "Allows the attributes, level and quality of items to be modified per player as they are given by the server.",
        "url": "https://builds.limetech.io/?p=tf2items",
        "files": ["../../;.*"]
    },
    "rtd": {
        "id": "rtd",
        "author": "Phil25",
        "description": "Lets players roll for temporary benefits.",
        "url": "https://github.com/Phil25/RTD/",
        "files": ["./;rtd-.*.zip"],
        "deps": ["tf2attributes"]
    },
    "thriller": {
        "id": "thriller",
        "author": "pheadxdll",
        "description": "Disables thriller taunt on Halloween event",
        "url": "https://forums.alliedmods.net/showpost.php?p=1590169",
        "files": ["plugins/;thriller.smx", "gamedata/;thriller.plugin.txt"]
    },
    "sharedthriller": {
        "id": "sharedthriller",
        "author": "Somedev",
        "description": "I share a file with thriller.",
        "url": "https://forums.alliedmods.net/showpost.php?p=1590169",
        "files": ["gamedata/;thriller.plugin.txt"]
    },
    "updater": {
        "id": "updater",
        "author": "GoD-Tony",
        "description": "Automatically updates SourceMod plugins and files.",
        "url": "https://bitbucket.org/GoD_Tony/updater/downloads/",
        "files": ["plugins/;updater.smx"]
    },
    "wants_rtd": {
        "id": "wants_rtd",
        "author": "Somedev",
        "description": "I want rtd",
        "url": "https://forums.alliedmods.net/showpost.php?p=1387386",
        "files": ["plugins/;dynamic_motd.smx"],
        "deps": ["rtd"]
    },
    "a_wants_b": {
        "id": "a_wants_b",
        "author": "Somedev",
        "description": "I want plugin b.",
        "url": "https://forums.alliedmods.net/showpost.php?p=1590169",
        "files": ["plugins/;thriller.smx"],
        "deps": ["b_wants_a"]
    },
    "b_wants_a": {
        "id": "b_wants_a",
        "author": "Somedev",
        "description": "I want plugin a.",
        "url": "https://forums.alliedmods.net/showpost.php?p=1590169",
        "files": ["gamedata/;thriller.plugin.txt"],
        "deps": ["a_wants_b"]
    },
    "wants_circular": {
        "id": "wants_circular",
        "author": "Somedev",
        "description": "I want a plugin which wants a plugin which wants the first one.",
        "url": "https://forums.alliedmods.net/showpost.php?p=1387386",
        "files": ["plugins/;dynamic_motd.smx"],
        "deps": ["a_wants_b"]
    },
    "wants_multiple": {
        "id": "wants_multiple",
        "author": "SomeDev",
        "description": "I want afk_manager and tf2items",
        "url": "https://forums.alliedmods.net/showpost.php?p=1387386",
        "files": ["plugins/;dynamic_motd.smx"],
        "deps": ["afk_manager", "tf2attributes"]
    },
    "wants_self": {
        "id": "wants_self",
        "author": "SomeDev",
        "description": "I want myself as dependency.",
        "url": "https://forums.alliedmods.net/showpost.php?p=1387386",
        "files": ["plugins/;dynamic_motd.smx"],
        "deps": ["wants_self"]
    },
}

__THIRDPARTY_WEBSITES__ = {
    "advancedinfiniteammo": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1754217", "advancedinfiniteammo.html"),
        WebMock("http://www.sourcemod.net/vbcompiler.php?file_id=148649", "AdvancedInfiniteAmmo.smx"),
    ],
    "afk_manager": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=708265", "afk_manager.html"),
        WebMock("http://afkmanager.dawgclan.net/plugins/afk_manager4.smx", "afk_manager4.smx"),
        WebMock("http://afkmanager.dawgclan.net/translations/afk_manager.phrases.txt", "afk_manager.phrases.txt"),
    ],
    "connect": [
        WebMock("https://builds.limetech.io/?p=connect", "connect.html"),
    ],
    "dynamic_motd": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1387386", "dynamic_motd.html"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=160284&d=1485099001", "dynamic_motd.smx"),
    ],
    "funcommandsx": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=665771", "funcommandsx.html"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=dc5547938c38333b03dff89b0ac17723&attachmentid=159900&d=1483676185", "funcommandsX_2.5.zip"),
    ],
    "steamtools": [
        WebMock("https://builds.limetech.io/?p=steamtools", "steamtools.html"),
    ],
    "tf2items": [
        WebMock("https://builds.limetech.io/?p=tf2items", "tf2items.html"),
        WebMock("https://builds.limetech.io/files/tf2items-1.6.4-hg279-linux.zip", "tf2items-1.6.4-hg279-linux.zip"),
    ],
    "tf2attributes": [
        WebMock("https://api.github.com/repos/FlaminSarge/tf2attributes/releases/latest", "tf2attributes.html"),
        WebMock("https://github.com/FlaminSarge/tf2attributes/releases/download/v1.2.1/tf2attributes.smx", "tf2attributes.smx"),
        WebMock("https://raw.githubusercontent.com/FlaminSarge/tf2attributes/master/tf2.attributes.txt", "tf2.attributes.txt"),
    ],
    "rtd": [
        WebMock("https://api.github.com/repos/Phil25/RTD/releases/latest", "rtd.html"),
        WebMock("https://github.com/Phil25/RTD/releases/download/2.3.1/rtd-2.3.1.zip", "rtd-2.3.1.zip"),
    ],
    "thriller": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1590169", "thriller.html"),
        WebMock("http://www.sourcemod.net/vbcompiler.php?file_id=128466", "thriller.smx"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=133555&d=1400274898", "thriller.plugin.txt"),
    ],
    "sharedthriller": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1590169", "thriller.html"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=133555&d=1400274898", "thriller.plugin.txt"),
    ],
    "updater": [
        WebMock("https://bitbucket.org/GoD_Tony/updater/downloads/updater.smx", "updater.smx"),
    ],
    "wants_rtd": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1387386", "dynamic_motd.html"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=160284&d=1485099001", "dynamic_motd.smx"),
    ],
    "a_wants_b": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1590169", "thriller.html"),
        WebMock("http://www.sourcemod.net/vbcompiler.php?file_id=128466", "thriller.smx"),
    ],
    "b_wants_a": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1590169", "thriller.html"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=133555&d=1400274898", "thriller.plugin.txt"),
    ],
    "wants_circular": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1387386", "dynamic_motd.html"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=160284&d=1485099001", "dynamic_motd.smx"),
    ],
    "wants_multiple": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1387386", "dynamic_motd.html"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=160284&d=1485099001", "dynamic_motd.smx"),
    ],
    "wants_self": [
        WebMock("https://forums.alliedmods.net/showpost.php?p=1387386", "dynamic_motd.html"),
        WebMock("https://forums.alliedmods.net/attachment.php?s=a59969c161c4e0ac23ef19c7cbb9ee5e&attachmentid=160284&d=1485099001", "dynamic_motd.smx"),
    ],
}
