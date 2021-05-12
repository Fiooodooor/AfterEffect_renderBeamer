from distutils.core import setup, Extension

mainmodule = Extension('garagefarm_font', sources = ['garagefarm_fontmodule_v3_9.c'])

setup (name = 'garagefarm_font_exporter',
       version = '1.0',
       description = 'Python module for wrapping macOs font convert functions.',
       author = 'Milosz Linkiewicz',
       author_email = 'milosz.lin@gmail.com',
       url = 'https://www.garageFarm.net',
       ext_modules = [mainmodule])
