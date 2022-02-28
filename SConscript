from building import *

cwd     = GetCurrentDir()
src     = []
path    = [cwd + '/inc']

src += ['src/infrared.c']

if GetDepend(['MULTI_INFRARED_NEC_DECODER']):
    src += ['src/nec_decoder.c']

if GetDepend(['PKG_USING_DRV_MULTI_INFRARED']):
    src += ['src/drv_infrared.c']

group = DefineGroup('multi_Infrared_frame', src, depend = ['PKG_USING_MULTI_INFRARED'], CPPPATH = path)

Return('group')
