# -*- coding: utf-8 -*-

# Calculates the current version number.  If possible, this is the
# output of “git describe”, modified to conform to the versioning
# scheme that setuptools uses.  If “git describe” returns an error
# (most likely because we're in an unpacked copy of a release tarball,
# rather than in a git working copy), then we fall back on reading the
# contents of the RELEASE-VERSION file.
#
# This will automatically update the RELEASE-VERSION file, if
# necessary.  Note that the RELEASE-VERSION file should *not* be
# checked into git; please add it to your top-level .gitignore file.
#
# You'll probably want to distribute the RELEASE-VERSION file in your
# sdist tarballs; to do this, just create a MANIFEST.in file that
# contains the following line:
#
#   include RELEASE-VERSION

__all__ = (
    "ABBREV",
    "get_git_version",
)

from subprocess import Popen, PIPE

from SCons.Script import *


ABBREV = 4


def call_git_describe():
    try:
        p = Popen(['git', 'describe', '--abbrev=%d' % ABBREV],
                  stdout=PIPE, stderr=PIPE)
        p.stderr.close()
        line = p.stdout.readlines()[0]
        return line.strip()

    except:
        return None


def read_release_version(filename):
    try:
        f = open(filename, "r")

        try:
            version = f.readlines()[0]
            return version.strip()

        finally:
            f.close()

    except:
        return None


def get_git_version(env, filename):
    # First try to get the current version using “git describe”.

    version = call_git_describe()

    # Create a file node for the filename.  (Mainly, to resolve any
    # variables in the filename).

    file_node = File(filename)

    # If we got something from git-describe, write the version to the
    # output file.

    if version is not None:
        env.WriteValue(file_node, env.Value(version))
        env.Default(file_node)

    # Otherwise, fall back on the value that's in RELEASE-VERSION.

    else:
        version = read_release_version(str(file_node))

        # If we still don't have anything, that's an error.

        if version is None:
            raise ValueError("Cannot find the version number!")

    # Regardless of where we got the version from, set the version in
    # the environment.

    env['VERSION'] = version

    return version
