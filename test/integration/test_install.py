import pytest
from data import INSTALL_DATA, DEPENDENCY_DATA, BAD_INSTALL_DATA, BAD_DEPENDENCY_DATA

@pytest.mark.parametrize("addon", INSTALL_DATA)
def test_install(smam, smamdb, addon):
    smamdb.expect_ids([addon])
    smam.exec("install " + addon)
    smam.check_installed(addon)

    if addon in DEPENDENCY_DATA.keys():
        for dep in DEPENDENCY_DATA[addon]:
            smam.check_installed(dep)

@pytest.mark.parametrize("addon", DEPENDENCY_DATA)
def test_install_no_depencencies(smam, smamdb, addon):
    if addon == "wants_self":
        pytest.skip()

    smamdb.expect_ids([addon], include_deps=False)
    smam.exec("install --no-deps " + addon)
    smam.check_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_not_installed(dep)

@pytest.mark.parametrize("addon", BAD_INSTALL_DATA)
def test_install_fail(smam, addon):
    smam.exec("install " + addon)
    smam.check_not_installed(addon)

@pytest.mark.parametrize("addon", BAD_DEPENDENCY_DATA)
def test_install_dependencies_fail(smam, addon):
    smam.exec("install " + addon)
    smam.check_not_installed(addon)

    for dep in BAD_DEPENDENCY_DATA[addon]:
        smam.check_not_installed(dep)
