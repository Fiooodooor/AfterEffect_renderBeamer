//
//  garagefarm_fontmodule.c
//  gf_font_converter
//
//  Created by Milosz Linkiewicz 2021
//  CzornyCzfanyCzop on 5/4/21.
//

#include <Python.h>
struct module_state {
    PyObject *error;
};

#define GETSTATE(m) (&_state)
static struct module_state _state;

#include <dlfcn.h>
#include "gf_font_converter.hpp"

static PyObject* garagefarm_font_copyconvert_old(PyObject *self, PyObject *args)
{
    int index = 0;
    const char *libFullPath, *sourceFile, *sourcePath, *destinationPath;

    if (!PyArg_ParseTuple(args, "s|s|s|s|l", &libFullPath, &sourceFile, &sourcePath, &destinationPath, &index))
        return NULL;
     
    printf("Decoded sourcePath: %s  and sourceFile: %s \n", sourcePath, sourceFile);
    printf("Decoded destinationPath: %s \n", destinationPath);
    printf("Decoded library path: %s \n", libFullPath);
       
    PyObject* python_list = NULL;
    FontsListS *fntList = (FontsListS*)malloc(sizeof( FontsListS) );
    if(!fntList) {
        printf("Malloc for fonts list failed! Returning.\n");
        return NULL;
    }
    void* libraryInstance = dlopen(libFullPath, RTLD_LOCAL|RTLD_LAZY);
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
                        PyObject* fontPath_py = PyUnicode_DecodeUTF8(fntList->pathsTable[i].fontPath, path_len, NULL);
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

static PyMethodDef garagefarm_font_methods[] = {
    {"copyconvert", garagefarm_font_copyconvert_old, METH_VARARGS, "Exec copyconvert"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

void initgaragefarm_font(void)
{
    PyObject *module = Py_InitModule("garagefarm_font", garagefarm_font_methods);
    char ext_error[] = "garagefarm_font.Error";
    if(module == NULL)
        return;
    
    struct module_state *st = GETSTATE(module);
    st->error = PyErr_NewException(&ext_error[0], NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        return;
    }
}
