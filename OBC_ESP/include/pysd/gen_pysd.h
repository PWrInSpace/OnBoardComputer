#ifndef PYSD_ZYGOCHUJ_H_
#define PYSD_ZYGOCHUJ_H_


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "/home/kuba/git/OnBoardComputer/OBC_ESP/lib/data_struct/pysd_data_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t pysd_get_header_size(bool new_line_ending);
size_t pysd_get_sd_frame_size(pysdmain_DataFrame pysd_main, bool new_line_ending);
size_t pysd_create_header(char*buffer, size_t size, bool new_line_ending);
size_t pysd_create_sd_frame(char *buffer, size_t size, pysdmain_DataFrame pysd_main, bool new_line_ending);

#ifdef __cplusplus
}
#endif

#endif
