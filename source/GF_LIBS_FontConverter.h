
//  Created by CzornyCzfanyCzop on 2/28/20.
//  Copyright © 2020 CzornyCzfanyCzop. All rights reserved.
//

#ifndef GF_FONTCONVERTER_PUBLIC
#define GF_FONTCONVERTER_PUBLIC

//#define OS_MACOS 1
//#define OS_CASE 1

//#pragma GCC visibility push(default)

#define LIB_MAXPATH_SIZE 1041 // 260x4 + 1 = 1040 (unicode on mac char is 4 bytes long)

typedef struct FontsPathS {
    char fontPath[LIB_MAXPATH_SIZE];
} FontsPathS;

typedef struct FontsListS {
    int32_t structureSize;
    FontsPathS *pathsTable;
} FontsListS;

#ifdef __cplusplus
extern "C" {
#endif

    int __cdecl copyConvertFont(const char* sourceFile, const char* sourcePath, const char* destinationPath, int index, FontsListS *fontsList);
  
#ifdef __cplusplus
}
#endif

typedef int(__cdecl *copyConvertFont_creator)(const char* sourceFile, const char* sourcePath, const char* destinationPath, int index, FontsListS *fontsList);

//#pragma GCC visibility pop
#endif // GF_FONTCONVERTER_PUBLIC
