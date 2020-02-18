import pytest
from data import INSTALL_DATA, DEPENDENCY_DATA, BAD_INSTALL_DATA, BAD_DEPENDENCY_DATA

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_install(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(addon)

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_install_depencencies(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_installed(dep)

@pytest.mark.parametrize('addon', BAD_INSTALL_DATA)
def test_install_fail(smam, addon):
    smam.exec('install ' + addon)
    smam.check_not_installed(addon)

@pytest.mark.parametrize('addon', BAD_DEPENDENCY_DATA)
def test_install_dependencies_fail(smam, addon):
    smam.exec('install ' + addon)
    smam.check_not_installed(addon)

    for dep in BAD_DEPENDENCY_DATA[addon]:
        smam.check_not_installed(dep)

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_install_no_depencencies(smam, addon):
    if addon == 'wants_self':
        pytest.skip()

    smam.exec('install --no-deps ' + addon)
    smam.check_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_not_installed(dep)
