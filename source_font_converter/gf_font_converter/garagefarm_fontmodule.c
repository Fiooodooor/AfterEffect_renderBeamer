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

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

#include <dlfcn.h>
//#include <string>

//#include "garagefarm_fontmodule.h"
//#include "gf_font_converter.hpp"

#define LIB_MAXPATH_SIZE 1041 // 260x4 + 1 = 1040 (unicode on mac char is 4 bytes long)
#define LIB_FONTPATH char fontPath[LIB_MAXPATH_SIZE];

typedef struct FontsPathS {
    char fontPath[LIB_MAXPATH_SIZE];
} FontsPathS;

typedef struct FontsListS {
    int32_t structureSize;
    FontsPathS *pathsTable;
} FontsListS;

typedef int(__cdecl *copyConvertFont_creator)(const char* sourceFile, const char* sourcePath, const char* destinationPath, int index, FontsListS *fontsList);


static char* StringFromEncoded(PyObject *encodedO)
{
    PyObject *s;
    char *val = malloc(sizeof(char)*255);
    PyObject *encodedU = PyUnicode_FromEncodedObject(encodedO, Py_FileSystemDefaultEncoding, NULL);
    if(encodedU == NULL)
        return val;
    
    if( PyUnicode_Check(encodedU) ) {  // python3 has unicode, but we convert to bytes
        s = PyUnicode_AsUTF8String(encodedU);
    } else if( PyBytes_Check(encodedU) ) {  // python2 has bytes already
        s = PyObject_Bytes(encodedU);
    } else {
        return val;
    }
        
    if(s) {
        strcpy(val , PyBytes_AsString(s) );
        Py_XDECREF(s);
    }
    return val;
}
static PyObject* garagefarm_font_copyconvert(PyObject *self, PyObject *args)
{
//    FontsListS *fontsList;
    int index = 0;
    const char *libFullPath, *sourceFile, *sourcePath, *destinationPath;
    //PyObject *libFullPathO, *sourceFileO, *sourcePathO, *destinationPathO;
    

    if (!PyArg_ParseTuple(args, "s|s|s|s|l", &libFullPath, &sourceFile, &sourcePath, &destinationPath, &index))
        return NULL;
   
    //libFullPath = StringFromEncoded(libFullPathO);
    //sourceFile = StringFromEncoded(sourceFileO);
    //sourcePath = StringFromEncoded(sourcePathO);
    //destinationPath = StringFromEncoded(destinationPathO);
    
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
    {"copyconvert", garagefarm_font_copyconvert, METH_VARARGS, "Exec copyconvert"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

/*static PyModuleDef garagefarm_fontmodule = {
    //PyModuleDef_HEAD_INIT,
    .m_name = "garagefarm_font",
    .m_doc = "GarageFarn.net module for fonts exporting under macOs.",
    .m_size = -1,
    .m_methods = garagefarm_fontMethods
};*/

//PyMODINIT_FUNC PyInit_garagefarm_font(void)
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
//int __cdecl copyConvertFont(const char* sourceFile, const char* sourcePath, const char* destinationPath, int index, FontsListS *fontsList);
