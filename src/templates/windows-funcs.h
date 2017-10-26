#ifndef WINDOWS_FUNCS_H
#define WINDOWS_FUNCS_H
#ifdef WIN32

#define WIN_FFI_HIDE 0x1
#define WIN_FFI_DIR  0x2
#define WIN_FFI_NORMAL 0x4

typedef struct _win_find_file_info_t {
	unsigned int handle;
	unsigned int file_attrs;
	time_t       last_access_time;
	unsigned int file_size;
	int          access_mode;
	char         file_name[1024];
	void*        private_data;
}win_find_file_info;

int win_find_first_file(const char* path, win_find_file_info* win_info);
int win_find_next_file(win_find_file_info* win_info);
void win_end_find_file(win_find_file_info* win_info);



#endif
#endif