import pytest
from data import INSTALL_DATA, BAD_INSTALL_DATA

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_install(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(INSTALL_DATA[addon])

@pytest.mark.parametrize('addon', BAD_INSTALL_DATA)
def test_install_fail(smam, addon):
    smam.exec('install ' + addon)
    smam.check_not_installed(BAD_INSTALL_DATA[addon])
