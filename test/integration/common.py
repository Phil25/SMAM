from subprocess import Popen, PIPE
import os
import shutil

BINARY = './bin/smam'

class SMAM:

    def __init__(self, binary = BINARY, destination = './mod/'):
        self.binary = binary
        self.destination = destination
        self.smroot = destination + 'addons/sourcemod/'

    def __enter__(self):
        os.makedirs(self.smroot + 'configs/')
        os.makedirs(self.smroot + 'data/')
        os.makedirs(self.smroot + 'gamedata/')
        os.makedirs(self.smroot + 'plugins/')
        os.makedirs(self.smroot + 'scripting/')
        os.makedirs(self.smroot + 'translations/')

        columns = shutil.get_terminal_size().columns
        print('\n\033[95m\033[1m' + ''.center(columns, '=') + '\033[0m')

        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        shutil.rmtree(self.destination)

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

    def check_installed(self, files):
        for (filename, size) in files:
            assert self.exists(filename)
            assert self.isfile(filename)
            assert size == self.getsize(filename)

    def check_not_installed(self, files):
        if not files:
            return

        if isinstance(files[0], list):
            for (filename, size) in files:
                assert not self.exists(filename)
        else:
            for filename in files:
                assert not self.exists(filename)
