import pytest
from data import INSTALL_DATA, BAD_INSTALL_DATA

pytest.register_assert_rewrite('common')
from common import SMAM

@pytest.mark.parametrize('addon', INSTALL_DATA)
def test_remove(addon):
    with SMAM() as smam:
        smam.exec('install ' + addon)
        smam.check_installed(INSTALL_DATA[addon])
        smam.exec('remove ' + addon)
        smam.check_not_installed(INSTALL_DATA[addon])

def test_remove_shared():
    with SMAM() as smam:
        smam.exec('install thriller sharedthriller')
        smam.check_installed(INSTALL_DATA['thriller'])
        smam.check_installed(INSTALL_DATA['sharedthriller'])

        smam.exec('remove sharedthriller')
        smam.check_installed(INSTALL_DATA['thriller']) # should be fully installed

        smam.exec('remove thriller')
        smam.check_not_installed(INSTALL_DATA['thriller'])
