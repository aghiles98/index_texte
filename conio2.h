
#define KEY_OK 13
#define KEY_UP 72
#define KEY_DOWN 80

#include <conio.h>

void gettextinfo (struct text_info * info);

void inittextinfo (void);

void clreol (void);

void clrscr (void);

void delline (void);

void insline (void);

void _conio_gettext (int left, int top, int right, int bottom,
                     struct char_info * buf);

void puttext (int left, int top, int right, int bottom, struct char_info * buf);

void movetext (int left, int top, int right, int bottom, int destleft,
              int desttop);

void gotoxy(int x, int y);

void cputsxy (int x, int y, char * str);

void putchxy (int x, int y, char ch);

void _setcursortype (int type);

void textattr (int _attr);

void normvideo (void);

void textbackground (int color);

void textcolor (int color);

int wherex (void);

int wherey (void);

char * getpass (const char * prompt, char * str);

void highvideo (void);

void lowvideo (void);


