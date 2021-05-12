# Garagefarm python v2/v3 font converter

Library is dedicated for MacOs 10.13+ operating system. Only dependency 'libgf_font_converter.dylib' can be present as a bundle, for example in renderbeamer install directory. The full filepath must be specified in python using 'lib_converter_path' in examples bellow. The only difference is that python2 function call is named 'copyconvert_old' due to encoding differences in implementation. Python2 send strings without any modifications but Python3 needs to encode the strings using native filesystem encoding (function call to encode form os.fspath).

## Python3 and building the module:

```bash
$ cd gf_font_converter
$ sudo python3 setup_3.py install
```
result will be build and installed for python3 as module. Run python3 interpreter and test code.

## Python3 test code:

*(script for converting lohit.ttf in directory unitests and copy-conver all to ccc downloads dir with prefix of 1 in name)*

*(inside python interpreter or from script)*

```python
import os
import garagefarm_font

font_name = os.fspath('lohit.ttf').encode()
font_path = os.fspath('/Users/ccc/Downloads/fontforge-master/pycontrib/FontCompare/unittests').encode()
font_new_path = os.fspath('/Users/ccc/Downloads').encode()
lib_converter_path = os.fspath('/Users/ccc/Downloads/AfterEffects/Release/libgf_font_converter.dylib').encode()

index_prefix = 1;

res_list = garagefarm_font.copyconvert( lib_converter_path, font_name, font_path, font_new_path, index_prefix )
```

**Function definition:**

result = garagefarm_font.copyconvert( <lib_path_encoded>, <font_name_encoded>, <font_path_encoded>, <font_destination_encoded>, <fnt_filename_prefix> )

if everything ok, result will be a list object with converted fonts and full path to every converted file.

if partialy not ok, some list fields may be empty or NULL.

if criticall error, then result will be NULL.


## Python2 and building the module:

```bash
$ cd gf_font_converter
$ sudo python2 setup_2.py install
```
result will be build and installed for python2 as module. Run python2 interpreter and test code.

## Pthon2 test code:
(script for converting lohit.ttf in directory unitests and copy-conver all to ccc downloads dir with prefix of 1 in name)

(inside python interpreter or from script):


```python
import garagefarm_font

font_name = 'lohit.ttf'
font_path = '/Users/ccc/Downloads/fontforge-master/pycontrib/FontCompare/unittests'
font_new_path = '/Users/ccc/Downloads'
lib_converter_path = '/Users/ccc/Downloads/AfterEffects/PyModule_and_dylib/Release/libgf_font_converter.dylib'

index_prefix = 1;
res_list = garagefarm_font.copyconvert_old( lib_converter_path, font_name, font_path, font_new_path, index_prefix )
```