import pytest
from data import INSTALL_DATA, DEPENDENCY_DATA

pytest.register_assert_rewrite('common')
from common import SMAM

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_dependency_remove(addon):
    if addon == 'wants_circular':
        return

    with SMAM() as smam:
        smam.exec('install ' + addon)
        smam.check_installed(INSTALL_DATA[addon])

        for dep in DEPENDENCY_DATA[addon]:
            smam.check_installed(INSTALL_DATA[dep])

        smam.exec('remove ' + addon)
        smam.check_not_installed(INSTALL_DATA[addon])

        for dep in DEPENDENCY_DATA[addon]:
            smam.check_not_installed(INSTALL_DATA[dep])

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_dependency_remove_no_deps(addon):
    if addon == 'wants_self' or addon == 'wants_circular':
        return

    with SMAM() as smam:
        smam.exec('install ' + addon)
        smam.check_installed(INSTALL_DATA[addon])

        for dep in DEPENDENCY_DATA[addon]:
            smam.check_installed(INSTALL_DATA[dep])

        smam.exec('remove --no-deps ' + addon)
        smam.check_not_installed(INSTALL_DATA[addon])

        for dep in DEPENDENCY_DATA[addon]:
            smam.check_installed(INSTALL_DATA[dep])
