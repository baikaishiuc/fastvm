
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __minput_h__
#define __minput_h__

#define MINPUT_MOUSE_MASK (MINPUT_MOUSE_ABSOLUTE | MINPUT_MOUSE_MOV | MINPUT_MOUSE_DOWN | MINPUT_MOUSE_UP)

#define MINPUT_MOUSE_ABSOLUTE			0x0001		
#define MINPUT_MOUSE_MOV				0x0002
#define MINPUT_MOUSE_DOWN				0x0004
#define MINPUT_MOUSE_UP					0x0008

#define MINPUT_KEYBOARD					0x0100
#define MINPUT_KEYBOARD_WSTRING			0x0200

#define MINPUT_MODE_NORMAL				0
#define MINPUT_MODE_ENHANCE				1

	struct minput_event
	{
		int type;
		/*
		工作模式
		普通模式：只是调用系统api，比如SendInput
		增强模式：需要驱动配合，每有实现
		*/
		int mode;
		union
		{
			/*
			鼠标事件
			@x			x坐标
			@y			y坐标
			@rnd		加入随机偏移，对x, y同时生效
			@interval	拆解开鼠标事件后，每次事件之间得间隔，这是个同步函数，意味着里面会有Sleep
						eg. 一次点击事件，可以拆解开3个事件，移动，按起，松开
			@pre_mov	过某些保护时有用，有些输入控件会检测在移动进控件时，是否移动过，有移动过
						才会接受信息，算时某种行为识别
			*/
			struct {
				int		x;
				int		y;
				int		rnd;
				int		interval;
				int		pre_mov;
			} mou;

			/*
			按键事件
			@wstr		假如事件为输入字符串
			*/
			struct {
				const wchar_t	*wstr;
				int				interval;
			} key;
		};
	};

	int		minput_send(struct minput_event *evt);
	int		minput_mov(int x, int y, int repeat, int rnd);
	int		minput_click(int interval);
	int		minput_dbclick(int interval);
	int		minput_wstring(const wchar_t *wstr, int interval);


#endif

#ifdef __cplusplus
}
#endif
