
from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c') + Glob('*.cpp')
path    = [cwd]

if GetDepend(['PKG_USING_X9555_EXAMPLE']):
    src += Glob('example/*.c') + Glob('example/*.cpp')
    path += [cwd + '/example']

group = DefineGroup('x9555', src, depend = ['PKG_USING_X9555'], CPPPATH = path)

Return('group')
