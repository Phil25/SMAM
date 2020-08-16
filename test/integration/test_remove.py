import pytest
from data import INSTALL_DATA, DEPENDENCY_DATA

@pytest.mark.parametrize("addon", INSTALL_DATA)
def test_remove(smam, smamdb, addon):
    smamdb.expect_ids([addon])
    smam.exec("install " + addon)
    smam.check_installed(addon)

    if addon in DEPENDENCY_DATA.keys():
        for dep in DEPENDENCY_DATA[addon]:
            smam.check_installed(dep)

    smam.exec("remove " + addon)
    smam.check_not_installed(addon)

    if addon in DEPENDENCY_DATA.keys():
        for dep in DEPENDENCY_DATA[addon]:
            smam.check_not_installed(dep)

def test_remove_shared(smam, smamdb):
    smamdb.expect_ids(["thriller", "sharedthriller"])
    smam.exec("install thriller sharedthriller")
    smam.check_installed("thriller")
    smam.check_installed("sharedthriller")

    smam.exec("remove sharedthriller")
    smam.check_installed("thriller") # should be fully installed

    smam.exec("remove thriller")
    smam.check_not_installed("thriller")

def test_not_remove_explicit_dependency_during_install(smam, smamdb):
    smamdb.expect_ids(["rtd", "tf2attributes"])
    smam.exec("install rtd tf2attributes")
    smam.check_installed("rtd")
    smam.check_installed("tf2attributes")

    smam.exec("remove rtd")
    smam.check_not_installed("rtd")
    smam.check_installed("tf2attributes")

def test_not_remove_explicit_dependency_after_install(smam, smamdb):
    smamdb.expect_ids(["rtd"])
    smam.exec("install rtd")
    smam.check_installed("rtd")
    smam.check_installed("tf2attributes")

    smamdb.expect_ids(["tf2attributes"], thirdparty_call_override=[])
    smam.exec("install tf2attributes")
    smam.exec("remove rtd")
    smam.check_not_installed("rtd")
    smam.check_installed("tf2attributes")

def test_not_remove_shared_dependencies(smam, smamdb):
    smamdb.expect_ids(["wants_multiple"])
    smam.exec("install wants_multiple")
    smam.check_installed("wants_multiple")

    smam.check_installed("afk_manager") # wants_multiple"s deps
    smam.check_installed("tf2attributes")

    # shouldn"t call to tf2attribute"s thirdparties, bc it"s installed
    smamdb.expect_ids(["rtd"], thirdparty_call_override=["rtd"])
    smam.exec("install rtd")
    smam.check_installed("rtd")

    smam.check_installed("afk_manager") # still installed
    smam.check_installed("tf2attributes")

    smam.exec("remove rtd")
    smam.check_not_installed("rtd")

    smam.check_installed("afk_manager") # still installed
    smam.check_installed("tf2attributes")

@pytest.mark.parametrize("addon", DEPENDENCY_DATA)
def test_remove_no_dependencies(smam, smamdb, addon):
    if addon == "wants_self":
        pytest.skip()

    smamdb.expect_ids([addon])
    smam.exec("install " + addon)
    smam.check_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_installed(dep)

    smam.exec("remove --no-deps " + addon)
    smam.check_not_installed(addon)

    for dep in DEPENDENCY_DATA[addon]:
        smam.check_installed(dep)
