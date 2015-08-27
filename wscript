#! /usr/bin/env python
# encoding: utf-8

VERSION = '0.1'
APPNAME = 'kugaru'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c compiler_cxx')

    opt.add_option('--debug', action='store_true', default=False, help='pass debug options to compiler')

def configure(cnf):
    cnf.load('compiler_c compiler_cxx')

    cnf.env['INSTALL_DATA_DIR'] = '{0}/share/{1}'.format(cnf.env['PREFIX'], APPNAME)

    cnf.check_cfg(package='glib-2.0', uselib_store='GLIB', args='--cflags --libs', mandatory=True)
    cnf.check_cfg(package='gl', args='--cflags --libs', mandatory=True)
    cnf.check_cfg(package='glu', args='--cflags --libs', mandatory=True)
    cnf.check_cfg(package='openal', args='--cflags --libs', mandatory=True)
    cnf.check_cfg(package='sdl2', args='--cflags --libs', madatory=True)
    cnf.check_cfg(package='libpng', args='--cflags --libs', mandatory=True)
    cnf.check_cfg(package='zlib', args='--cflags --libs', mandatory=True)
    cnf.check_cfg(package='vorbis', args='--cflags --libs', mandatory=True)
    cnf.check_cfg(package='vorbisfile', args='--cflags --libs', mandatory=True)

    cnf.check_cc(type_name='uint32_t', header='stdint.h', mandatory=True)
    cnf.check_cc(type_name='uint64_t', header='stdint.h', mandatory=True)
    cnf.check_cc(type_name='int32_t', header='stdint.h', mandatory=True)
    cnf.check_cc(type_name='int64_t', header='stdint.h', mandatory=True)

    cnf.recurse('src')

def build(bld):
    bld.recurse('data src')
