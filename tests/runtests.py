#!/usr/bin/env python3
"""Finds and runs executables within a directory tree."""

import argparse
import os
import pathlib
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
            return entry.name.endswith('.exe')
        return (entry.stat(follow_symlinks=False).st_mode & stat.S_IXUSR) != 0
    return False

def find_executables(directory:pathlib.Path, excludes:list[str]) -> typing.Generator:
    """
    A generator function which yields executable files within the directory tree
    given by 'path'.

    :param directory: A directory to be scanned for executables.
    :param excludes: A list of file or directory names to be excluded from the search.
    """

    with os.scandir(directory) as entries:
        for entry in entries:
            if entry.name not in excludes:
                path = entry.path
                if entry.is_dir(follow_symlinks=False):
                    yield from find_executables(pathlib.Path(path), excludes)
                elif is_executable(entry):
                    yield path

def run_executables(directory:pathlib.Path, timeout:int, excludes:list[str]) -> None:
    """
    :param directory: The directory to be scanned for executables.
    :param timeout: The timeout for executable in seconds.
    :param excludes: A list of file or directory names to be excluded from the search.
    :returns: None
    """

    for path in find_executables(directory, excludes):
        path = os.path.abspath(path)
        print(f'---\nRunning {path}', flush=True)
        subprocess.run([path], check=True, timeout=timeout)

def main() -> int:
    parser = argparse.ArgumentParser(
        prog='runtests',
        description='Finds and runs executables within a directory tree')
    parser.add_argument('path', type=pathlib.Path, help='The path to be searched for executables')
    parser.add_argument('-t', '--timeout', type=int, default=60, help='The timeout for each executable in seconds')
    args = parser.parse_args()
    run_executables(args.path, args.timeout, [ 'CMakeFiles', os.path.split(__file__)[1]])
    return 0

if __name__ == '__main__':
    sys.exit (main())
