# -*- coding: utf-8 -*-

from checklib import *
from version import *

vars = Variables('.scons.vars', ARGUMENTS)

vars.AddVariables(
    PathVariable("prefix", "Installation prefix", "/usr",
                 PathVariable.PathAccept),
    ('CC', "The C compiler to use"),
    ('CCFLAGS', "Any additional options to pass in to the C compiler"),
    ('CPPFLAGS', "Any additional options to pass in to the C preprocessor"),
    ('LDFLAGS', "Any additional options to pass in to the linker"),
    )


root_env = Environment(tools=['default', 'packaging', 'write_value'],
                       package="libhwm",
                       pkg_version="$VERSION",
                       CCFLAGS="-O2",
                       BINDIR = "$prefix/bin",
                       DOCDIR = "$prefix/share/doc/$package",
                       LIBDIR = "$prefix/lib",
                       INCLUDEDIR = "$prefix/include")

vars.Update(root_env)
vars.Save(".scons.vars", root_env)

root_env.MergeFlags('-g -Wall -Werror')

# An action that can clean up the scons temporary files.

if 'sdist' not in COMMAND_LINE_TARGETS:
    root_env.Clean("distclean",
                   [
                    ".sconf_temp",
                    ".sconsign.dblite",
                    ".scons.vars",
                    ".scons.vars.check",
                    "config.log",
                   ])

# Only run the configuration steps if we're actually going to build
# something.

if not GetOption('clean') and not GetOption('help'):
    conf = root_env.Configure(custom_tests=
                              {
                               'CheckLibInPath': CheckLibInPath,
                              })

    if not conf.CheckCC():
        print "!! Your compiler and/or environment is not properly configured."
        Exit(0)


    if not conf.CheckLibInPath("check",
                               library="check",
                               call="",
                               header="#include <check.h>"):
        print "!! Cannot find the check library."
        Exit(0)


    root_env = conf.Finish()



version = get_git_version(root_env, "#/RELEASE-VERSION")
print "Package version: %s" % version


# Set up a list of source files for the packaging target later on.
# Each SConscript file is responsible for updating this list.

SOURCE_FILES = []
Export('SOURCE_FILES')

# Add the root scons files to the SOURCES_LIST.

build_files = map(File, \
    [
     "SConstruct",
     "RELEASE-VERSION",
     "site_scons/checklib.py",
     "site_scons/version.py",
     "site_scons/site_tools/write_value.py",
    ])

SOURCE_FILES.extend(build_files)

# Include the subdirectory SConscripts

Export('root_env')
SConscript(['include/SConscript',
            'src/SConscript',
            'tests/SConscript',
            'doc/SConscript'])

# Install documentation files

doc_files = map(File, \
    [
     "LICENSE.txt",
    ])

SOURCE_FILES.extend(doc_files)

root_env.Alias("install", root_env.Install("$DOCDIR", doc_files))

# Define packaging targets

license = 'BSD'
summary = 'A C library for high-water mark buffers'
source_url = 'http://github.com/dcreager/libhwm.git'

src = root_env.Package(NAME="$package",
                       VERSION="${pkg_version}",
                       PACKAGETYPE='src_targz',
                       source=SOURCE_FILES)
root_env.Alias("sdist", src)

if GetOption('clean'):
    root_env.Default("sdist")
    root_env.Clean("sdist", "$package-${pkg_version}")
