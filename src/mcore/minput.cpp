

#if defined(_MSC_VER)
#include <Windows.h>
#include "basic_types.h"
#include "print_util.h"
#include "merror.h"
#include "minput.h"

static int minput_mouse_send(struct minput_event *evt);
static int minput_keyboard_send(struct minput_event *evet);
static int minput_keyboard_send_uchar(wchar_t);

static int get_screen_width(void)
{
    return GetSystemMetrics(SM_CXSCREEN);
}

static int get_screen_height(void)
{
    return GetSystemMetrics(SM_CYSCREEN);
}

int		minput_send(struct minput_event *evt)
{
	if (evt->type & MINPUT_MOUSE_MASK) {
		return minput_mouse_send(evt);
	}
	else if ((evt->type >= MINPUT_KEYBOARD) && (evt->type <= MINPUT_KEYBOARD_WSTRING)) {
		return minput_keyboard_send(evt);
	}

	return -1;
}

static int minput_mouse_send(struct minput_event *evt)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "minput_mouse_send(evt{x:%d, y:%d, interval:%d}) "
#define func_format()	evt->mou.x, evt->mou.y, evt->mou.interval
	INPUT input = { 0 };
	int rnd_x, rnd_y, mov_times = evt->mou.pre_mov;

	mov_times++;

	if (!evt->mou.interval) {
		print_warn("mouse event interval is zero, that may be can't work in some application.");
	}

	while (mov_times--) {
		if (evt->type & MINPUT_MOUSE_MOV) {
			input.type = INPUT_MOUSE;

			rnd_x = rnd_y = 0;
			if (evt->mou.rnd) {
				rnd_x = rand() % evt->mou.rnd;
				rnd_y = rand() % evt->mou.rnd;
			}

			input.mi.dx = (evt->mou.x + rnd_x) * 0xffff / get_screen_width();
			input.mi.dy = (evt->mou.y + rnd_y) * 0xffff / get_screen_height();
			input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);
			if (SendInput(1, &input, sizeof(input)) != 1) {
				print_info("failed with (%s)SendInput.", merr_msg1());
				return -1;
			}
			Sleep(evt->mou.interval);
		}
	}

	if (evt->type & MINPUT_MOUSE_DOWN) {
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		if (SendInput(1, &input, sizeof(input)) != 1) {
			print_info("failed with (%s)SendInput().", merr_msg1());
			return -1;
		}
		Sleep(evt->mou.interval);
	}

	if (evt->type & MINPUT_MOUSE_UP) {
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		if (SendInput(1, &input, sizeof(input)) != 1) {
			print_info("failed with (%s)SendInput().", merr_msg1());
			return -1;
		}
		Sleep(evt->mou.interval);
	}

	return 0;
}

static int minput_keyboard_send(struct minput_event *evt)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "minput_keyboard_send(evt{wstr:%ls}) "
#define func_format()	evt->key.wstr
	int i, len =wcslen(evt->key.wstr);

	for (i = 0; i < len; i++) {
		if (minput_keyboard_send_uchar(evt->key.wstr[i])) {
			print_err("failed with (%s)minput_keyboard_send_uchar().", merr_msg1());
			return -1;
		}
		Sleep(evt->key.interval);
	}

	return 0;
}

static int minput_keyboard_send_uchar(wchar_t u)
{
	INPUT input;

	input.type = INPUT_KEYBOARD;
	input.ki.wVk = 0;
	input.ki.wScan = u;
	input.ki.time = 0;
	input.ki.dwFlags = KEYEVENTF_UNICODE;
	input.ki.dwExtraInfo = 0;

	if (SendInput(1, &input, sizeof(input)) != 1) {
		return -1;
	}

	return 0;
}

int		minput_mov(int x, int y, int repeat, int rnd)
{
	struct minput_event evt = { 0 };

	if (!repeat)
		repeat = 1;

	evt.type = MINPUT_MOUSE_ABSOLUTE | MINPUT_MOUSE_MOV;
	evt.mou.x = x;
	evt.mou.y = y;
	evt.mou.rnd = rnd;
	evt.mou.interval = 50;

	while (repeat--) {
		minput_send(&evt);
	}

	return 0;
}

int		minput_click(int interval)
{
	struct minput_event evt = { 0 };

	evt.type = MINPUT_MOUSE_DOWN | MINPUT_MOUSE_UP;
	evt.mou.interval = interval;

	return minput_send(&evt);
}

int		minput_dbclick(int interval)
{
	struct minput_event evt = { 0 };

	evt.type = MINPUT_MOUSE_DOWN | MINPUT_MOUSE_UP;
	evt.mou.interval = interval;

	minput_send(&evt);
	Sleep(50);
	minput_send(&evt);

	return 0;
}

int		minput_wstring(const wchar_t *wstr, int interval)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "minput_wstring(wstr:%ls, interval:%d) "
#define func_format()	wstr, interval
	int i, len =wcslen(wstr);

	if (!interval) {
		print_warn("meet 0 interval, that is you need?.");
	}

	for (i = 0; i < len; i++) {
		if (minput_keyboard_send_uchar(wstr[i])) {
			print_err("failed with (%s)minput_keyboard_send_uchar().", merr_msg1());
			return -1;
		}
		Sleep(interval);
	}

	return 0;
}

#endif
