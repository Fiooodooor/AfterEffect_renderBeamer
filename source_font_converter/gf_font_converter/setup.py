from distutils.core import setup, Extension

mainmodule = Extension('garagefarm_font', sources = ['garagefarm_fontmodule.c'])

setup (name = 'garagefarm_font_exporter',
       version = '0.9',
       description = 'Python module for wrapping macOs font convert functions.',
       author = 'Milosz Linkiewicz',
       author_email = 'milosz.lin@gmail.com',
       url = 'https://www.garageFarm.net',
       ext_modules = [mainmodule])
