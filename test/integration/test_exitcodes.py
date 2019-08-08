import pytest

pytest.register_assert_rewrite('common')
from common import SMAM

EXITCODE_DATA = [
    # 0 success
    # 1 run as root
    # 2 no permissions
    # 3 write error
    (4, ''), # no command
    (5, 'badcmd tf2items'), # unknown command
    (6, 'install'), # no addons
    (7, 'install tf2items -d .') # no SourceMod root
    # 8 corrupted cache
]

@pytest.mark.parametrize('code,cmd', EXITCODE_DATA)
def test_exitcode(code, cmd):
    with SMAM() as smam:
        assert code == smam.exec(cmd)
