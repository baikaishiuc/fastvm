
#ifndef __image_h__
#define __image_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */


#ifndef __def_vertex2d__
#define __def_vertex2d__
	struct image_result
	{
		int		x;
		int		y;
		int		sad;
	};
#endif

	/* 子图搜索用的数据结构
	@width		图像宽度，必须偶数
	@height		图像高度，必须偶数
	@data		灰度图数据*/
	struct image
	{
		int				width;
		int				height;
		short			*data;
	};

	// 速度较慢，但是更加稳定
	// https://www.ics.uci.edu/~eppstein/161/960227.html
#define IMG_KMP		0

	// 速度更快，效果更好，但是在屏幕截图时，不稳定
	// 我在屏幕截图时，似乎无法正确得检测到屏幕里得内容
// http://werner.yellowcouch.org/Papers/subimg/index.html#eq:StandardC
#define IMG_FFT		1

/*
@img		搜索的原图
@sub_img	子图
@ratio		相似比例，指的是匹配时 主图 和 子图的色彩相差比例
@return		0			sucess
			<0			error code
*/
	int		image_subimage_find(int algorithm, struct image *img, struct image *sub_img, float ratio, struct image_result *ret);


#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif