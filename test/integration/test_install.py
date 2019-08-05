from common import SMAM
import pytest

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
    'rtd': [
        ['configs/rtd2_perks.default.cfg', 30829],
        ['plugins/rtd.smx', 124057],
        ['scripting/rtd.sp', 51622],
        ['translations/rtd2.phrases.txt', 11692],
        ['translations/rtd2_perks.phrases.txt', 15264]
    ],
    'updater': [
        ['plugins/updater.smx', 22011]
    ]
}

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_install(addon):
    with SMAM() as smam:
        smam.exec('install ' + addon)

        for (filename, size) in INSTALL_DATA[addon]:
            assert smam.exists(filename)
            assert smam.isfile(filename)
            assert size == smam.getsize(filename)

def test_badaddon():
    with SMAM() as smam:
        smam.exec('install badaddon')
        assert not smam.isfile('../../../baddir/thriller.smx')
        assert not smam.isfile('../../../mod2/addons/sourcemod/data/thriller.plugin.txt')

def test_badthriller():
    with SMAM() as smam:
        smam.exec('install badthriller')
        assert not smam.isfile('plugins/thriller.smx')
        assert not smam.isfile('gamedata/invalid.plugin.txt')
