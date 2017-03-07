#ifndef _lacuna_0_defined_
#define _lacuna_0_defined_

#if defined(__cplusplus)
extern "C" {
#endif

extern const unsigned char
	embed_1[];

typedef struct {
	const char *image;
	const unsigned char *pixels;
	int size, width, height, channels;
	} EmbedImageItem;

const EmbedImageItem embed_lacuna_0[] = {
	{"lacuna_0.tga", embed_1, 4194304, 1024, 1024, 4},
	{NULL, NULL, 0, 0, 0, 0}
};
const int embed_lacuna_0_count = 1;

#if defined(__cplusplus)
}
#endif
#endif // of lacuna_0
