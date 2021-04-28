#pragma once
#ifndef GF_AEGP_STRINGS_H
#define GF_AEGP_STRINGS_H

#include "GF_GlobalTypes.h"
namespace RenderBeamer {

A_char *GetStringPtr(int strNum);
A_char *GetBeamerMaskA(int strNum);
const wchar_t *GetBeamerMaskW(int strNum);

} // namespace RenderBeamer
#endif
