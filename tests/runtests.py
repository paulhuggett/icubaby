#!/usr/bin/env python3
"""Finds and runs executables within a directory tree."""

import argparse
import os
import stat
import subprocess
import sys
import typing

def is_executable(entry:os.DirEntry) -> bool:
    """Returns a boolean indicating whether a directory entry represents an
    executable file.

    :param entry: The directory entry to be tested.
    :returns: True if entry is an executable file, false otherwise.
    """

    if entry.is_file():
        if sys.platform.startswith('win32'):
            return entry.name.endwith('.exe')
        return (entry.stat(follow_symlinks=False).st_mode & stat.S_IXUSR) != 0
    return False

def find_executables(directory:str, excludes:list[str]) -> typing.Generator:
    """
    A generator function which yields executable files within the directory tree
    given by 'path'.

    :param path: A directory to be scanned for executables.
    :param excludes: A list of file or directory names to be excluded from the search.
    """

    with os.scandir(directory) as entries:
        for entry in entries:
            if entry.name not in excludes:
                path = os.path.join(directory, entry.name)
                if entry.is_dir(follow_symlinks=False):
                    yield from find_exe(path, excludes)
                elif is_executable(entry):
                    yield path

def run_executables(directory:str, excludes:list[str]) -> None:
    """
    :param directory: The directory to be scanned for executables.
    :param excludes: A list of file or directory names to be excluded from the search.
    :returns: None
    """

    for path in find_executables(directory, excludes):
        path = os.path.abspath(path)
        print(f'---\nRunning {path}', flush=True)
        subprocess.run([path], check=True, timeout=60)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='runtests',
        description='Finds an runs executables within a directory tree')
    parser.add_argument('path')
    args = parser.parse_args()
    run_executables(args.path, [ 'CMakeFiles' ])
