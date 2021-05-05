**Python3 and building the module:**

$ cd gf_font_converter

$ python3 setup.py build

result will be in build folder. While beeing in the same folder as module binary, run python3 interpreter and run&test.

**Python3 test code:**

*(script for converting lohit.ttf in directory unitests and copy-conver all to ccc downloads dir with prefix of 1 in name)*

*(inside python interpreter or from script)*

import os

import garagefarm_font

font_name = 'lohit.ttf'

font_path = '/Users/ccc/Downloads/fontforge-master/pycontrib/FontCompare/unittests'

font_new_path = '/Users/ccc/Downloads'

lib_converter_path = '/Users/ccc/Downloads/AfterEffects/Release/libgf_font_converter.dylib'

font_namefs = os.fspath(font_name)

font_pathfs = os.fspath(font_path)

font_new_pathfs = os.fspath(font_new_path)

lib_converter_pathfs = os.fspath(lib_converter_path)

index_prefix = 1;

res_list = garagefarm_font.copyconvert( lib_converter_pathfs.encode(), font_namefs.encode(), font_pathfs.encode(), font_new_pathfs.encode(), index_prefix )


**Function definition:**

result = garagefarm_font.copyconvert( <lib_path_encoded>, <font_name_encoded>, <font_path_encoded>, <font_destination_encoded>, <fnt_filename_prefix> )

if everything ok, result will be a list object with converted fonts and full path to every converted file.

if partialy not ok, some list fields may be empty or NULL.

if criticall error, then result will be NULL.


