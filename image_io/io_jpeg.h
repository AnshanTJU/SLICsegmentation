#ifdef __cplusplus
extern "C" {
#endif

#define IO_JPEG_VERSION 0.20121130

/* io_jpeg.c */
unsigned char *read_jpeg_u8(const char *fname, int *width, int *height, 
				  int *bytes_per_pixel, int *color_space);

int write_jpeg_u8(const char *fname, unsigned char *data, int width, int height, 
				  int bytes_per_pixel, int color_space);

#ifdef __cplusplus
}
#endif
