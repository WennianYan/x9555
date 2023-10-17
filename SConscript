
from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c') + Glob('*.cpp')
path    = [cwd]

group = DefineGroup('x9555', src, depend = ['PKG_USING_X9555'], CPPPATH = path)

Return('group')
