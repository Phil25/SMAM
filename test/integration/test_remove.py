import pytest
from data import INSTALL_DATA, DEPENDENCY_DATA

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_remove(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(addon)

    smam.exec('remove ' + addon)
    smam.check_not_installed(addon)

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_remove_dependencies(smam, addon):
    smam.exec('install ' + addon)
    smam.check_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_installed(dep)

    smam.exec('remove ' + addon)
    smam.check_not_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_not_installed(dep)

def test_remove_shared(smam):
    smam.exec('install thriller sharedthriller')
    smam.check_installed('thriller')
    smam.check_installed('sharedthriller')

    smam.exec('remove sharedthriller')
    smam.check_installed('thriller') # should be fully installed

    smam.exec('remove thriller')
    smam.check_not_installed('thriller')

def test_not_remove_explicit_dependency_during_install(smam):
    smam.exec('install rtd tf2attributes')
    smam.check_installed('rtd')
    smam.check_installed('tf2attributes')

    smam.exec('remove rtd')
    smam.check_not_installed('rtd')
    smam.check_installed('tf2attributes')

def test_not_remove_explicit_dependency_after_install(smam):
    smam.exec('install rtd')
    smam.check_installed('rtd')
    smam.check_installed('tf2attributes')

    smam.exec('install tf2attributes')
    smam.exec('remove rtd')
    smam.check_not_installed('rtd')
    smam.check_installed('tf2attributes')

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_remove_no_dependencies(smam, addon):
    if addon == 'wants_self':
        pytest.skip()

    smam.exec('install ' + addon)
    smam.check_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_installed(dep)

    smam.exec('remove --no-deps ' + addon)
    smam.check_not_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_installed(dep)
