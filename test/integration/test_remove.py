import pytest
from data import INSTALL_DATA, BAD_INSTALL_DATA, DEPENDENCY_DATA

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_remove(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(INSTALL_DATA[addon])
    smam.exec('remove ' + addon)
    smam.check_not_installed(INSTALL_DATA[addon])

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_remove_dependencies(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(INSTALL_DATA[addon])

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_installed(INSTALL_DATA[dep])

    smam.exec('remove ' + addon)
    smam.check_not_installed(INSTALL_DATA[addon])

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_not_installed(INSTALL_DATA[dep])

def test_remove_shared(smam):
    smam.exec('install thriller sharedthriller')
    smam.check_installed(INSTALL_DATA['thriller'])
    smam.check_installed(INSTALL_DATA['sharedthriller'])

    smam.exec('remove sharedthriller')
    smam.check_installed(INSTALL_DATA['thriller']) # should be fully installed

    smam.exec('remove thriller')
    smam.check_not_installed(INSTALL_DATA['thriller'])

def test_not_remove_explicit_dependency_during_install(smam):
    smam.exec('install rtd tf2attributes')
    smam.check_installed(INSTALL_DATA['rtd'])
    smam.check_installed(INSTALL_DATA['tf2attributes'])

    smam.exec('remove rtd')
    smam.check_not_installed(INSTALL_DATA['rtd'])
    smam.check_installed(INSTALL_DATA['tf2attributes'])

def test_not_remove_explicit_dependency_after_install(smam):
    smam.exec('install rtd')
    smam.check_installed(INSTALL_DATA['rtd'])
    smam.check_installed(INSTALL_DATA['tf2attributes'])

    smam.exec('install tf2attributes')
    smam.exec('remove rtd')
    smam.check_not_installed(INSTALL_DATA['rtd'])
    smam.check_installed(INSTALL_DATA['tf2attributes'])
