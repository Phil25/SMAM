import pytest
from data import INSTALL_DATA, DEPENDENCY_DATA

pytest.register_assert_rewrite('common')
from common import SMAM

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_dependency_install(addon):
    with SMAM() as smam:
        smam.exec('install ' + addon)
        smam.check_installed(INSTALL_DATA[addon])

        for dep in DEPENDENCY_DATA[addon]:
            smam.check_installed(INSTALL_DATA[dep])

@pytest.mark.parametrize('addon', DEPENDENCY_DATA)
def test_dependency_install_no_deps(addon):
    if addon == 'wants_self':
        return

    with SMAM() as smam:
        smam.exec('install --no-deps ' + addon)
        smam.check_installed(INSTALL_DATA[addon])

        for dep in DEPENDENCY_DATA[addon]:
            smam.check_not_installed(INSTALL_DATA[dep])

def test_dependency_bad_addon():
    with SMAM() as smam:
        smam.exec('install wants_badaddon')
        assert not smam.isfile('plugins/dynamic_motd.smx')
        assert not smam.isfile('../../../baddir/thriller.smx')

def test_dependency_bad_wants_good():
    with SMAM() as smam:
        smam.exec('install bad_and_wants_rtd')
        assert not smam.isfile('plugins/dynamic_motd.smx')
        assert not smam.isfile('../../../baddir/thriller.smx')

        # TODO: rollback dependent addon installation
        # test for: single (this), circular, hierarchical
        #for (filename, _) in INSTALL_DATA['rtd']:
            #assert not smam.isfile(filename)
