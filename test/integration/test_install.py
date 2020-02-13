import pytest
from data import INSTALL_DATA, DEPENDENCY_DATA, BAD_INSTALL_DATA, BAD_DEPENDENCY_DATA

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_install(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(INSTALL_DATA[addon])

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_install_depencencies(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(INSTALL_DATA[addon])

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_installed(INSTALL_DATA[dep])

@pytest.mark.parametrize('addon', BAD_INSTALL_DATA)
def test_install_fail(smam, addon):
    smam.exec('install ' + addon)
    smam.check_not_installed(BAD_INSTALL_DATA[addon])

@pytest.mark.parametrize('addon', BAD_DEPENDENCY_DATA)
def test_install_dependencies_fail(smam, addon):
    smam.exec('install ' + addon)
    smam.check_not_installed(BAD_DEPENDENCY_DATA[addon])

    for dep in BAD_DEPENDENCY_DATA[addon]:
        smam.check_not_installed(BAD_INSTALL_DATA[dep])
