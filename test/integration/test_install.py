import pytest
from data import INSTALL_DATA, BAD_INSTALL_DATA

pytest.register_assert_rewrite('common')
from common import SMAM

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_install(addon):
    with SMAM() as smam:
        smam.exec('install ' + addon)
        smam.check_installed(INSTALL_DATA[addon])

@pytest.mark.parametrize('addon', BAD_INSTALL_DATA)
def test_install_fail(addon):
    with SMAM() as smam:
        smam.exec('install ' + addon)
        smam.check_not_installed(BAD_INSTALL_DATA[addon])
