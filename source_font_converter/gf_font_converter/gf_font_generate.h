
#ifndef GF_FONT_GENERATE_H
#define GF_FONT_GENERATE_H
#ifndef LIB_MAXPATH_SIZE
#define LIB_MAXPATH_SIZE 1041 // 260x4 + 1 = 1040 (unicode on mac char is 4 bytes long)
#endif
int convert_font(const char* src_file, const char* output_path, const char* output_file, int font_nr, char* destFilePath);

#endif // GF_FONT_GENERATE_H

