import pytest
from data import INSTALL_DATA

pytest.register_assert_rewrite('common')
from common import SMAM

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_install(addon):
    with SMAM() as smam:
        smam.exec('install ' + addon)
        smam.check_installed(addon, INSTALL_DATA[addon])

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
