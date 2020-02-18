INSTALL_DATA = {
    'advancedinfiniteammo': [
        ['plugins/AdvancedInfiniteAmmo.smx', 23290]
    ],
    'afk_manager': [
        ['plugins/afk_manager4.smx', 24919],
        ['translations/afk_manager.phrases.txt', 14663]
    ],
    'dynamic_motd': [
        ['plugins/dynamic_motd.smx', 16171]
    ],
    'funcommandsx': [
        ['plugins/funcommandsX.smx', 38290],
        ['configs/happy_phrases.ini', 364],
        ['translations/funcommandsX.phrases.txt', 2618],
        ['translations/ru/funcommandsX.phrases.txt', 3392],
        ['translations/tr/funcommandsX.phrases.txt', 3000]
    ],
    'tf2attributes': [
        ['plugins/tf2attributes.smx', 12701],
        ['gamedata/tf2.attributes.txt', 3287]
    ],
    'tf2items': [
        ['scripting/tf2items_manager.sp', 15646],
        ['scripting/include/tf2items.inc', 9463],
        ['extensions/tf2items.ext.2.ep2v.so', 172477]
    ],
    'thriller': [
        ['plugins/thriller.smx', 3691],
        ['gamedata/thriller.plugin.txt', 656]
    ],
    'sharedthriller': [
        ['gamedata/thriller.plugin.txt', 656]
    ],
    'rtd': [
        ['configs/rtd2_perks.default.cfg', 30829],
        ['plugins/rtd.smx', 124057],
        ['scripting/rtd.sp', 51622],
        ['translations/rtd2.phrases.txt', 11692],
        ['translations/rtd2_perks.phrases.txt', 15264]
    ],
    'updater': [
        ['plugins/updater.smx', 22011]
    ],
    'wants_rtd': [
        ['plugins/dynamic_motd.smx', 16171]
    ],
    'wants_circular': [
        ['plugins/dynamic_motd.smx', 16171]
    ],
    'wants_multiple': [
        ['plugins/dynamic_motd.smx', 16171]
    ],
    'wants_self': [
        ['plugins/dynamic_motd.smx', 16171]
    ],
    'a_wants_b': [
        ['plugins/thriller.smx', 3691]
    ],
    'b_wants_a': [
        ['gamedata/thriller.plugin.txt', 656]
    ]
}

BAD_INSTALL_DATA = {
    'badaddon': [
        ['../../../baddir/thriller.smx', 0],
        ['../../../mod2/addons/sourcemod/data/thriller.plugin.txt', 0]
    ],
    'badthriller': [
        ['plugins/thriller.smx', 0],
        ['gamedata/invalid.plugin.txt', 0]
    ],
    'badaddon_no_id': [
        ['plugins/thriller.smx', 0],
        ['gamedata/thriller.plugin.txt', 0]
    ],
    'badaddon_no_author': [
        ['plugins/thriller.smx', 0],
        ['gamedata/thriller.plugin.txt', 0]
    ],
    'badaddon_no_description': [
        ['plugins/thriller.smx', 0],
        ['gamedata/thriller.plugin.txt', 0]
    ],
    'wants_badaddon': [
        ['gamedata/thriller.plugin.txt', 0]
    ]
}

DEPENDENCY_DATA = {
    'wants_rtd': ['rtd', 'tf2attributes'],
    'rtd': ['tf2attributes'],
    'wants_circular': ['a_wants_b'],
    'wants_multiple': ['afk_manager', 'tf2items'],
    'wants_self': ['wants_self'],
    'a_wants_b': ['b_wants_a'],
    'b_wants_a': ['a_wants_b']
}

BAD_DEPENDENCY_DATA = {
    'wants_badaddon': ['badaddon'],
}
