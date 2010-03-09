# -*- coding: utf-8 -*-

from SCons.Script import *


def write_value(target, source, env):
    # Writes the value of the first source into the first target file.

    f = open(str(target[0]), 'w')
    f.write("%s\n" % source[0].get_contents())


def generate(env, **kwargs):
    write_action = Action(write_value, "Writing $SOURCE to $TARGET")
    env['BUILDERS']['WriteValue'] = Builder(action=write_action)


def exists(env):
    return 1
