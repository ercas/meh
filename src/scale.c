
#include <stdint.h>
#include <sys/time.h>
#include "meh.h"

#define GETVAL(x, c) (( \
			( \
				ibuf[(x) * 3 + (c)] * (ur) + \
				ibuf[(x) * 3 + 3+(c)] * (u) \
			) * (vr) + \
			( \
				(ibufn[(x) * 3 + (c)]) * (ur) + \
				(ibufn[(x) * 3 + 3+(c)]) * (u)\
			) * (v)) >> 20)


void scale(struct image *img, XImage *ximg){
	int x, y;
	unsigned char * __restrict__ ibuf;
	unsigned char * __restrict__ ibufn;
	char* __restrict__ newBuf = ximg->data;
	unsigned int jdy = ximg->bytes_per_line / 4 - ximg->width;

//	struct timeval t0;
//	struct timeval t1;
//	gettimeofday(&t0, NULL);

	unsigned int bufxs[ximg->width];
	unsigned int us[ximg->width];
	{
		unsigned int dx = (img->bufwidth << 10) / ximg->width;
		unsigned int bufx = img->bufwidth / ximg->width;
		for(x = 0; x < ximg->width; x++){
			bufxs[x] = bufx >> 10;
			us[x] = (bufx & 1023);
			bufx += dx;
		}
	}

	for(y = 0; y < ximg->height; y++){
		unsigned int bufy = (y << 10) * img->bufheight / ximg->height;
		unsigned int v = (bufy & 1023);
		unsigned int vr = 1023^(bufy & 1023);
		ibuf = &img->buf[y * img->bufheight / ximg->height * img->bufwidth * 3];
		ibufn = &img->buf[(y * img->bufheight / ximg->height + 1) * img->bufwidth * 3];
		for(x = 0; x < ximg->width; x++){
			unsigned int bufx = bufxs[x];
			unsigned int u = us[x];
			unsigned int ur = 1023^u;

			*newBuf++ = GETVAL(bufx, 2);
			*newBuf++ = GETVAL(bufx, 1);
			*newBuf++ = GETVAL(bufx, 0);
			newBuf++;
		}
		newBuf += jdy;
	}

//	gettimeofday(&t1, NULL);
//	printf("%li ms\n", ((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec) / 1000);
}

void linearscale(struct image *img, XImage *ximg){
	int x, y;
	unsigned char * __restrict__ ibuf;
	char* __restrict__ newBuf = ximg->data;
	unsigned int jdy = ximg->bytes_per_line / 4 - ximg->width;
	unsigned int dx = (img->bufwidth << 10) / ximg->width;

//	struct timeval t0;
//	struct timeval t1;
//	gettimeofday(&t0, NULL);

	for(y = 0; y < ximg->height; y++){
		unsigned int bufx = img->bufwidth / ximg->width;
		ibuf = &img->buf[y * img->bufheight / ximg->height * img->bufwidth * 3];

		for(x = 0; x < ximg->width; x++){
			*newBuf++ = (ibuf[(bufx >> 10)*3+2]);
			*newBuf++ = (ibuf[(bufx >> 10)*3+1]);
			*newBuf++ = (ibuf[(bufx >> 10)*3+0]);
			newBuf++;
			bufx += dx;
		}
		newBuf += jdy;
	}

//	gettimeofday(&t1, NULL);
//	printf("%i\n", ((t1.tv_sec - t0.tv_sec) * 1000000 + t1.tv_usec - t0.tv_usec) / 10);
}


