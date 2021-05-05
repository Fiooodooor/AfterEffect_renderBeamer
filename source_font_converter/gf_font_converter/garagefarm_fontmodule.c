//
//  garagefarm_fontmodule.c
//  gf_font_converter
//
//  Created by Milosz Linkiewicz 2021
//  CzornyCzfanyCzop on 5/4/21.
//
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <dlfcn.h>

#include "garagefarm_fontmodule.h"
#include "gf_font_converter.hpp"

typedef struct {
    PyObject_HEAD
    LIB_FONTPATH
} FontsPathP;

typedef struct {
    PyObject_HEAD
    int32_t structureSize;
    FontsPathP *pathsTable;
} FontsListP;

static PyTypeObject FontsPathType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "garagefarm_font.FontPath",
    .tp_doc = "FontPath unicode maxpath",
    .tp_basicsize = sizeof(FontsPathP),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
};

static PyTypeObject FontsPathListType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "garagefarm_font.FontsPathList",
    .tp_doc = "FontsPathList with structureSize holding the number of unicode paths",
    .tp_basicsize = sizeof(FontsListP),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
};

static PyObject* garagefarm_font_copyconvert(PyObject *self, PyObject *args)
{
//    FontsListS *fontsList;
    int index = 0;
    const char *libFullPathC, *sourceFileC, *sourcePathC, *destinationPathC;
    char *libFullPath, *sourceFile, *sourcePath, *destinationPath;
    Py_ssize_t libFullPathSize, sourceFileSize, sourcePathSize, destinationPathSize;
    PyObject *libFullPathO, *sourceFileO, *sourcePathO, *destinationPathO;
    PyObject *libFullPathU, *sourceFileU, *sourcePathU, *destinationPathU;
    


    if (!PyArg_ParseTuple(args, "O|O|O|O|l", &libFullPathO, &sourceFileO, &sourcePathO, &destinationPathO, &index))
        return NULL;
    if (!PyUnicode_FSDecoder(libFullPathO, &libFullPathU) ||
        !PyUnicode_FSDecoder(sourceFileO, &sourceFileU) ||
        !PyUnicode_FSDecoder(sourcePathO, &sourcePathU) ||
        !PyUnicode_FSDecoder(destinationPathO, &destinationPathU))
    {
        return NULL;
    }
    
    libFullPathC = PyUnicode_AsUTF8AndSize(libFullPathU, &libFullPathSize);
    if(libFullPathC && libFullPathSize < LIB_MAXPATH_SIZE)
        libFullPath = strdup(libFullPathC);
    else
        return NULL;
    
    sourceFileC = PyUnicode_AsUTF8AndSize(sourceFileU, &sourceFileSize);
    if(sourceFileC && sourceFileSize < LIB_MAXPATH_SIZE)
        sourceFile = strdup(sourceFileC);
    else
        return NULL;
    
    sourcePathC = PyUnicode_AsUTF8AndSize(sourcePathU, &sourcePathSize);
    if(sourcePathC && sourcePathSize < LIB_MAXPATH_SIZE)
        sourcePath = strdup(sourcePathC);
    else
        return NULL;
    
    destinationPathC = PyUnicode_AsUTF8AndSize(destinationPathU, &destinationPathSize);
    if(destinationPathC && destinationPathSize < LIB_MAXPATH_SIZE)
        destinationPath = strdup(destinationPathC);
    else
        return NULL;
    
    printf("Decoded sourcePath: %s  and sourceFile: %s \n", sourcePath, sourceFile);
    printf("Decoded destinationPath: %s \n", destinationPath);
    printf("Decoded library path: %s \n", libFullPath);
    
    Py_XDECREF(libFullPathU);
    Py_XDECREF(sourceFileU);
    Py_XDECREF(sourcePathU);
    Py_XDECREF(destinationPathU);
    
    PyObject* python_list = NULL;
    FontsListS *fntList = (FontsListS*)malloc(sizeof( FontsListS) );
    if(!fntList) {
        printf("Malloc for fonts list failed! Returning.\n");
        return NULL;
    }
    void* libraryInstance = dlopen(libFullPath ,RTLD_LOCAL|RTLD_LAZY);
    if(libraryInstance)
    {
        copyConvertFont_creator libraryConverterF;
        *(void **)(&libraryConverterF) = dlsym(libraryInstance, "copyConvertFont");
        if(libraryConverterF)
        {
            fntList->pathsTable = NULL;
            fntList->structureSize = 0;
            int converted = libraryConverterF(sourceFile, sourcePath, destinationPath, 1, fntList);
            printf("Fonts converted: %d \n", converted);
            if(converted > 0 && fntList && fntList->pathsTable)
            {
                printf("Fonts in pathsTable: %d \n", fntList->structureSize);
                int i = 0;
                Py_ssize_t path_len = 0;
                python_list = PyList_New(fntList->structureSize);
                while(i < fntList->structureSize)
                {
                    printf("Font converted: %s \n", fntList->pathsTable[i].fontPath);
                    path_len = strlen(fntList->pathsTable[i].fontPath);
                    if(path_len > 0) {
                        printf("Starting string to python convertion, font path len: %ld \n", path_len);
                        PyObject* fontPath_py = PyUnicode_DecodeFSDefaultAndSize(fntList->pathsTable[i].fontPath, path_len);
                        PyList_SetItem(python_list, i, fontPath_py);
                    }
                    else {
                        printf("Font path len = [%ld] invalid! No conversion to pyString at [%d]\n", path_len, i);
                        PyList_SetItem(python_list, i, NULL);
                    }
                    ++i;
                }
            }
            else
            {
                printf("No fonts converted or data error occured. \n");
            }
        }
        else
        {
            printf("Font library function instance not loaded! ERROR!\n");
            char* err = dlerror();
            if(err) {
                printf("Error string: %s \n", err);
            }
        }
        dlclose((void*)libraryInstance);
    }
    else
    {
        printf("Font library instance not loaded! ERROR!\n");
        char* err = dlerror();
        if(err) {
            printf("Error string: %s \n", err);
        }
        return NULL;
    }

    return python_list;
}

static PyMethodDef garagefarm_fontMethods[] = {
    {"copyconvert",  garagefarm_font_copyconvert, METH_VARARGS,
     "Function to convert font and save it to destination."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static PyModuleDef garagefarm_fontmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "garagefarm_font",
    .m_doc = "GarageFarn.net module for fonts exporting under macOs.",
    .m_size = -1,
    .m_methods = garagefarm_fontMethods
};

PyMODINIT_FUNC PyInit_garagefarm_font(void)
{
    PyObject *module_pt;
    if (PyType_Ready(&FontsPathType) < 0)
        return NULL;
    
    if (PyType_Ready(&FontsPathListType) < 0)
        return NULL;
    
    module_pt = PyModule_Create(&garagefarm_fontmodule);
    if (module_pt == NULL)
        return NULL;

    Py_INCREF(&FontsPathType);
    if (PyModule_AddObject(module_pt, "FontPath", (PyObject *) &FontsPathType) < 0) {
        Py_DECREF(&FontsPathType);
        Py_DECREF(module_pt);
        return NULL;
    }
    Py_INCREF(&FontsPathListType);
    if (PyModule_AddObject(module_pt, "FontsPathList", (PyObject *) &FontsPathListType) < 0) {
        Py_DECREF(&FontsPathListType);
        Py_DECREF(module_pt);
        return NULL;
    }

    return module_pt;
}
//int __cdecl copyConvertFont(const char* sourceFile, const char* sourcePath, const char* destinationPath, int index, FontsListS *fontsList);
