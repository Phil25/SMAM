import os
import pytest
import shutil
from subprocess import Popen, PIPE
from data import INSTALL_DATA, BAD_INSTALL_DATA

BINARY = './bin/smam'
FILES = {**INSTALL_DATA, **BAD_INSTALL_DATA}

class SMAM:

    def __init__(self, binary = BINARY, destination = './mod/'):
        self.binary = binary
        self.destination = destination
        self.smroot = destination + 'addons/sourcemod/'

    def exec(self, cmd):
        cmds = [self.binary] + cmd.split() + ['-d', self.destination]
        columns = shutil.get_terminal_size().columns
        print('\n\033[95m\033[1m' + ' '.join(cmds).center(columns, '-') + '\033[0m')
        process = Popen(cmds)
        process.communicate() # wait for finish
        return process.returncode

    def exists(self, f):
        return os.path.exists(self.smroot + f)

    def isfile(self, f):
        return os.path.isfile(self.smroot + f)

    def isdir(self, f):
        return os.path.isdir(self.smroot + f)

    def getsize(self, f):
        return os.path.getsize(self.smroot + f)

    def check_installed(self, addon):
        for (filename, size) in FILES[addon]:
            assert self.exists(filename)
            assert self.isfile(filename)
            assert size == self.getsize(filename)

    def check_not_installed(self, addon):
        if not FILES[addon]:
            return

        for (filename, size) in FILES[addon]:
            assert not self.exists(filename)

@pytest.fixture(scope="function")
def smam():
    smam = SMAM()

    os.makedirs(smam.smroot + 'configs/')
    os.makedirs(smam.smroot + 'data/')
    os.makedirs(smam.smroot + 'gamedata/')
    os.makedirs(smam.smroot + 'plugins/')
    os.makedirs(smam.smroot + 'scripting/')
    os.makedirs(smam.smroot + 'translations/')

    columns = shutil.get_terminal_size().columns
    print('\n\033[95m\033[1m' + ''.center(columns, '=') + '\033[0m')

    yield smam

    shutil.rmtree(smam.destination)
