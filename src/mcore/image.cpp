
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "image.h"
#include "print_util.h"
#include "mtime_ex.h"
#include "fftw3.h"

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")

#define img_valid(_img)			((_img) && (_img)->width && (_img)->height && (_img)->data)

static int16_t* boxaverage(int16_t*input, int sx, int sy, int wx, int wy)
{
	int16_t *horizontalmean = (int16_t*)malloc(sizeof(int16_t)*sx*sy);
	int wx2 = wx / 2;
	int wy2 = wy / 2;
	int16_t* from = input + (sy - 1) * sx;
	int16_t* to = horizontalmean + (sy - 1) * sx;
	int initcount = wx - wx2;
	if (sx < initcount) initcount = sx;
	int xli = -wx2;
	int xri = wx - wx2;
	for (; from >= input; from -= sx, to -= sx)
	{
		int64_t sum = 0;
		int count = initcount;
		for (int c = 0; c < count; c++)
			sum += (int64_t)from[c];
		to[0] = (int16_t)(sum / count);
		int xl = xli, x = 1, xr = xri;
		/**
		 * The area where the window is slightly outside the
		 * left boundary. Beware: the right bnoundary could be
		 * outside on the other side already
		 */
		for (; x < sx; x++, xl++, xr++)
		{
			if (xl >= 0) break;
			if (xr < sx)
			{
				sum += (int64_t)from[xr];
				count++;
			}
			to[x] = (int16_t)(sum / count);
		}
		/**
		 * both bounds of the sliding window
		 * are fully inside the images
		 */
		for (; xr < sx; x++, xl++, xr++)
		{
			sum -= (int64_t)from[xl];
			sum += (int64_t)from[xr];
			to[x] = (int16_t)(sum / count);
		}
		/**
		 * the right bound is falling of the page
		 */
		for (; x < sx; x++, xl++)
		{
			sum -= (int64_t)from[xl];
			count--;
			to[x] = (int16_t)(sum / count);
		}
	}

	/**
	 * The same process as aboe but for the vertical dimension now
	 */
	int ssy = (sy - 1)*sx + 1;
	from = horizontalmean + sx - 1;
	int16_t *verticalmean = (int16_t*)malloc(sizeof(int16_t)*sx*sy);
	to = verticalmean + sx - 1;
	initcount = wy - wy2;
	if (sy < initcount) initcount = sy;
	int initstopat = initcount * sx;
	int yli = -wy2 * sx;
	int yri = (wy - wy2)*sx;
	for (; from >= horizontalmean; from--, to--)
	{
		int64_t sum = 0;
		int count = initcount;
		for (int d = 0; d < initstopat; d += sx)
			sum += (int64_t)from[d];
		to[0] = (int16_t)(sum / count);
		int yl = yli, y = 1, yr = yri;
		for (; y < ssy; y += sx, yl += sx, yr += sx)
		{
			if (yl >= 0) break;
			if (yr < ssy)
			{
				sum += (int64_t)from[yr];
				count++;
			}
			to[y] = (int16_t)(sum / count);
		}
		for (; yr < ssy; y += sx, yl += sx, yr += sx)
		{
			sum -= (int64_t)from[yl];
			sum += (int64_t)from[yr];
			to[y] = (int16_t)sum / count;
		}
		for (; y < ssy; y += sx, yl += sx)
		{
			sum -= (int64_t)from[yl];
			count--;
			to[y] = (int16_t)(sum / count);
		}
	}
	free(horizontalmean);
	return verticalmean;
}

static void window(int16_t * img, int sx, int sy, int wx, int wy)
{
	int wx2 = wx / 2;
	int sxsy = sx * sy;
	int sx1 = sx - 1;
	for (int x = 0; x < wx2; x++)
		for (int y = 0; y < sxsy; y += sx)
		{
			img[x + y] = img[x + y] * x / wx2;
			img[sx1 - x + y] = img[sx1 - x + y] * x / wx2;
		}

	int wy2 = wy / 2;
	int syb = (sy - 1)*sx;
	int syt = 0;
	for (int y = 0; y < wy2; y++)
	{
		for (int x = 0; x < sx; x++)
		{
			/**
			 * here we need to recalculate the stuff (*y/wy2)
			 * to preserve the discrete nature of integers.
			 */
			img[x + syt] = img[x + syt] * y / wy2;
			img[x + syb] = img[x + syb] * y / wy2;
		}
		/**
		 * The next row for the top rows
		 * The previous row for the bottom rows
		 */
		syt += sx;
		syb -= sx;
	}
}

static void normalize(int16_t * img, int sx, int sy, int wx, int wy)
{
	/**
	 * Calculate the mean background. We will subtract this
	 * from img to make sure that it has a mean of zero
	 * over a window size of wx x wy. Afterwards we calculate
	 * the square of the difference, which will then be used
	 * to normalize the local variance of img.
	 */
	int16_t * mean = boxaverage(img, sx, sy, wx, wy);
	int16_t * sqr = (int16_t*)malloc(sizeof(int16_t)*sx*sy);
	for (int j = 0; j < sx * sy; j++)
	{
		img[j] -= mean[j];
		int16_t v = img[j];
		sqr[j] = v * v;
	}
	int16_t * var = boxaverage(sqr, sx, sy, wx, wy);
	/**
	 * The normalization process. Currenlty still
	 * calculated as doubles. Could probably be fixed
	 * to integers too. The only problem is the sqrt
	 */
	for (int j = 0; j < sx * sy; j++)
	{
		double v = sqrt(fabs(var[j]));
		if (v < 0.0001) v = 0.0001;
		img[j] = (int16_t)(img[j] * 32 / v);
		if (img[j] > 127) img[j] = 127;
		if (img[j] < -127) img[j] = -127;
	}
	/**
	 * Mean was allocated in the boxaverage function
	 * Sqr was allocated in thuis function
	 * Var was allocated through boxaveragering
	 */
	free(mean);
	free(sqr);
	free(var);

	/**
	 * As a last step in the normalization we
	 * window the sub image around the borders
	 * to become 0
	 */
	window(img, sx, sy, wx, wy);
}

static int imgsub_find_fft(struct image *img, struct image *subimg, struct image_result *ret)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "imgsub_find_fft(img:%p) "
#define func_format()	img
	uint8_t* img_sad = (uint8_t *)malloc(sizeof(uint8_t) * img->width * img->height);
	uint8_t* subimg_sad = (uint8_t *)malloc(sizeof(uint8_t) * subimg->width * subimg->height);

	if (!img_sad || !subimg_sad) {
		print_err("failed with malloc().");
		return -1;
	}

	for (int i = 0; i < subimg->width*subimg->height; i++)
	{
		subimg_sad[i] = (uint8_t)subimg->data[i];
		img_sad[i] = (uint8_t)img->data[i];
	}
	for (int i = subimg->width * subimg->height; i < (img->width * img->height); i++)
		img_sad[i] = (uint8_t)img->data[i];

	/**
	 * Normalization and windowing of the images.
	 *
	 * The window size (wx,wy) is half the size of the smaller subimage. This
	 * is useful to have as much good information from the subimg.
	 */
	int wx = subimg->width / 2;
	int wy = subimg->height / 2;
	normalize(subimg->data, subimg->width, subimg->height, wx, wy);
	normalize(img->data, img->width, img->height, wx, wy);

	int Asx2 = img->width / 2 + 1;

	/**
	 * Preparation of the fourier transforms.
	 * img_fft_in is the amplitude of image A. Af is the frequence of image A
	 * Similar for B. crosscors will hold the crosscorrelations.
	 */
	double * img_fft_in = (double*)fftw_malloc(sizeof(double) * img->width * img->height);
	fftw_complex * img_fft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Asx2 * img->height);
	double * subimg_fft_in = (double*)fftw_malloc(sizeof(double) * img->width * img->height);
	fftw_complex * subimg_fft_out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*Asx2 * img->height);

	/**
	 * The forward transform of A goes from img_fft_in to img_fft_out
	 * The forward tansform of B goes from subimg_fft_in to subimg_fft_out 
	 * In Bf we will also calculate the inproduct of img_fft_out and Bf
	 * The backward transform then goes from Bf to img_fft_in again. That
	 * variable is aliased as crosscors;
	 */
	fftw_plan forwardA = fftw_plan_dft_r2c_2d(img->height, img->width, img_fft_in, img_fft_out, FFTW_ESTIMATE);
	fftw_plan forwardB = fftw_plan_dft_r2c_2d(img->height, img->width, subimg_fft_in, subimg_fft_out, FFTW_ESTIMATE);
	double * crosscorrs = img_fft_in;
	fftw_plan backward = fftw_plan_dft_c2r_2d(img->height, img->width, subimg_fft_out, crosscorrs, FFTW_ESTIMATE);

	if (!forwardA || !forwardB || !backward) {
		printf("fftw_plan_dft_r2c_2d == NULL\n");
		return -1;
	}

	/**
	 * The two forward transforms of A and B. Before we do so we copy the normalized
	 * data into the double array. For B we also pad the data with 0
	 */
	for (int row = 0; row < img->height; row++)
		for (int col = 0; col < img->width; col++)
			img_fft_in[col + img->width * row] = img->data[col + img->width * row];
	fftw_execute(forwardA);

	for (int j = 0; j < img->height * img->width; j++)
		subimg_fft_in[j] = 0;
	for (int row = 0; row < subimg->height; row++)
		for (int col = 0; col < subimg->width; col++)
			subimg_fft_in[col + img->width * row] = subimg->data[col + subimg->width * row];
	fftw_execute(forwardB);

	/**
	 * The inproduct of the two frequency domains and calculation
	 * of the crosscorrelations
	 */
	double norm = img->width * img->height;
	for (int j = 0; j < Asx2 * img->height; j++)
	{
		double a = img_fft_out[j][0];
		double b = img_fft_out[j][1];
		double c = subimg_fft_out[j][0];
		double d = -subimg_fft_out[j][1];
		double e = a * c - b * d;
		double f = a * d + b * c;
		subimg_fft_out[j][0] = e / norm;
		subimg_fft_out[j][1] = f / norm;
	}
	fftw_execute(backward);

	/**
	 * We now have a correlation map. We can spent one more pass
	 * over the entire image to actually find the best matching images
	 * as defined by the SAD.
	 * We calculate this by gridding the entire image according to the
	 * size of the subimage. In each cel we want to know what the best
	 * match is.
	 */
	int sa = 1 + img->width / subimg->width;
	int sb = 1 + img->height / subimg->height;
	int sadx = 0;
	int sady = 0;
	int64_t minsad = subimg->width * subimg->height * 256L;
	//minsad = 25000;
	for (int a = 0; a < sa; a++)
	{
		int xl = a * subimg->width;
		int xr = xl + subimg->width;
		if (xr > img->width) continue;
		for (int b = 0; b < sb; b++)
		{
			int yl = b * subimg->height;
			int yr = yl + subimg->height;
			if (yr > img->height) continue;

			// find the maximum correlation in this cell
			int cormxat = xl + yl * img->width;
			double cormx = crosscorrs[cormxat];
			for (int x = xl; x < xr; x++)
				for (int y = yl; y < yr; y++)
				{
					int j = x + y * img->width;
					if (crosscorrs[j] > cormx)
						cormx = crosscorrs[cormxat = j];
				}
			int corx = cormxat % img->width;
			int cory = cormxat / img->width;

			// We dont want subimages that fall of the larger image
			if ((corx + subimg->width) > img->width) continue;
			if ((cory + subimg->height) > img->height) continue;

			int64_t sad = 0;
			for (int x = 0; (sad < minsad) && (x < subimg->width); x++)
				for (int y = 0; y < subimg->height; y++)
				{
					int j = (x + corx) + (y + cory) * img->width;
					int i = x + y * subimg->width;
					sad += llabs((int)subimg_sad[i] - (int)img_sad[j]);
				}

			if (sad < minsad)
			{
				minsad = sad;
				sadx = corx;
				sady = cory;
			}
		}
	}

	ret->sad = (int)minsad;
	ret->x = sadx;
	ret->y = sady;

	/**
	 * img_fft_in, subimg_fft_in, img_fft_out and subimg_fft_out were allocated in this function
	 * crosscorrs was an alias for img_fft_in and does not require deletion.
	 */
	fftw_free(img_fft_in);
	fftw_free(subimg_fft_in);
	fftw_free(img_fft_out);
	fftw_free(subimg_fft_out);
	fftw_destroy_plan(forwardA);
	fftw_destroy_plan(forwardB);
	fftw_destroy_plan(backward);
	return 0;
}

#define img_pos(img, x, y)			(img->width * (y) + (x))

static int imgsub_find_kmp(struct image *img, struct image *subimg, struct image_result *img_ret)
{
	int i, j, m, n;

	img_ret->sad = 0;
	img_ret->x = 0;
	img_ret->y = 0;

	for (j = 0; j < (img->height - subimg->height); j++) {
		for (i = 0; i < (img->width - subimg->width); i++) {
			for (n = 0; n < subimg->height; n+=2) {
				for (m = 0; m < subimg->width; m+=2) {
					if (abs(img->data[img_pos(img, i + m, j + n)] - subimg->data[img_pos(subimg, m, n)]) > 40) {
						break;
					}
				}

				if (m < subimg->width)
					break;
			}

			if (((m + 1) >= subimg->width) && ((n + 1) >= subimg->height)) {
				img_ret->x = i;
				img_ret->y = j;
				img_ret->sad = 1;
				return 0;
			}
		}
	}

	return 0;
}

int		image_subimage_find(int algorithm, struct image *img, struct image *sub_img, float ratio, struct image_result *img_ret)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "image_subimage_find(algorithm:%d, img:%p, sub_img:%p, ratio:%f) "
#define func_format()	 algorithm, img, sub_img, ratio

	int ret = -1;

	struct image img1 = { 0 }, subimg1 = { 0 };

	if (!img_valid(img)|| !img_valid(sub_img) || !img_ret) {
		print_err("failed with invalid param.");
		return -1;
	}

	img1 = *img;
	subimg1 = *sub_img;

	img1.data = (short *)malloc(img->width * img->height * sizeof (img->data[0]));
	subimg1.data = (short *)malloc(sub_img->width * sub_img->height * sizeof(sub_img->data[0]));

	if (!img1.data || !subimg1.data) {
		print_err("failed with  malloc().");
		goto exit_label;
	}

	memcpy(img1.data, img->data, sizeof (img->data[0]) * img->width * img->height);
	memcpy(subimg1.data, sub_img->data, sizeof (sub_img->data[0]) * sub_img->width * sub_img->height);

	if (algorithm == IMG_FFT) {
		ret = imgsub_find_fft(&img1, &subimg1, img_ret);
	}
	else {
		ret = imgsub_find_kmp(&img1, &subimg1, img_ret);
	}

exit_label:
	if (img1.data)		free(img1.data);
	if (subimg1.data)	free(subimg1.data);

	return ret;
}
#endif
