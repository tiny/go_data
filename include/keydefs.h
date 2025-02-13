/*
  keydefs.h
  key definitions

  programmer  rmcinnis
  date        january 28, 1996
*/
#ifndef KEYDEFS_H
#define KEYDEFS_H

#define  XF_BASE        0x1000
#define  XF_EXTEND      0x2000
#define  XF_TIMEOUT     0x4000

#define  XF_ANYKEY      (XF_BASE + 0x00FF)
#define  XF_ESCAPE      (XF_BASE + 0x001B)
#define  XF_CR          (XF_BASE + 0x000A)
#define  XF_RETURN      (XF_BASE + 0x000D)
#define  XF_SPACE       (XF_BASE + 0x0020)
#define  XF_BACKSPACE   (XF_BASE + 0x0008)
#define  XF_TAB         (XF_BASE + 0x0009)
#define  XF_SHIFTTAB    (XF_EXTEND + 0x000F)
#define  XF_CTRLTAB     (XF_EXTEND + 0x0094)

#define  XF_F1          (XF_EXTEND + 0x3B)
#define  XF_F2          (XF_EXTEND + 0x3C)
#define  XF_F3          (XF_EXTEND + 0x3D)
#define  XF_F4          (XF_EXTEND + 0x3E)
#define  XF_F5          (XF_EXTEND + 0x3F)
#define  XF_F6          (XF_EXTEND + 0x40)
#define  XF_F7          (XF_EXTEND + 0x41)
#define  XF_F8          (XF_EXTEND + 0x42)
#define  XF_F9          (XF_EXTEND + 0x43)
#define  XF_F10         (XF_EXTEND + 0x44)
#define  XF_F11         (XF_EXTEND + 0x85)
#define  XF_F12         (XF_EXTEND + 0x86)

#define  XF_ALTF1       (XF_EXTEND + 0x68)
#define  XF_ALTF2       (XF_EXTEND + 0x69)
#define  XF_ALTF3       (XF_EXTEND + 0x6A)
#define  XF_ALTF4       (XF_EXTEND + 0x6B)
#define  XF_ALTF5       (XF_EXTEND + 0x6C)
#define  XF_ALTF6       (XF_EXTEND + 0x6D)
#define  XF_ALTF7       (XF_EXTEND + 0x6E)
#define  XF_ALTF8       (XF_EXTEND + 0x6F)
#define  XF_ALTF9       (XF_EXTEND + 0x70)
#define  XF_ALTF10      (XF_EXTEND + 0x71)
#define  XF_ALTF11      (XF_EXTEND + 0x8B)
#define  XF_ALTF12      (XF_EXTEND + 0x8C)

#define  XF_CTLF1       (XF_EXTEND + 0x5E)
#define  XF_CTLF2       (XF_EXTEND + 0x5F)
#define  XF_CTLF3       (XF_EXTEND + 0x60)
#define  XF_CTLF4       (XF_EXTEND + 0x61)
#define  XF_CTLF5       (XF_EXTEND + 0x62)
#define  XF_CTLF6       (XF_EXTEND + 0x63)
#define  XF_CTLF7       (XF_EXTEND + 0x64)
#define  XF_CTLF8       (XF_EXTEND + 0x65)
#define  XF_CTLF9       (XF_EXTEND + 0x66)
#define  XF_CTLF10      (XF_EXTEND + 0x67)
#define  XF_CTLF11      (XF_EXTEND + 0x89)
#define  XF_CTLF12      (XF_EXTEND + 0x8A)

#define  XF_END         (XF_EXTEND + 0x4F)
#define  XF_DNARR       (XF_EXTEND + 0x50)
#define  XF_PGDN        (XF_EXTEND + 0x51)
#define  XF_LEFT        (XF_EXTEND + 0x4B)
#define  XF_NUMPAD5     (XF_EXTEND + 0x4C)
#define  XF_RIGHT       (XF_EXTEND + 0x4D)
#define  XF_HOME        (XF_EXTEND + 0x47)
#define  XF_UPARR       (XF_EXTEND + 0x48)
#define  XF_PGUP        (XF_EXTEND + 0x49)

#define  XF_ALT_A       (XF_EXTEND + 0x1E)
#define  XF_ALT_B       (XF_EXTEND + 0x30)
#define  XF_ALT_C       (XF_EXTEND + 0x2E)
#define  XF_ALT_D       (XF_EXTEND + 0x20)
#define  XF_ALT_E       (XF_EXTEND + 0x12)
#define  XF_ALT_F       (XF_EXTEND + 0x21)
#define  XF_ALT_G       (XF_EXTEND + 0x22)
#define  XF_ALT_H       (XF_EXTEND + 0x23)
#define  XF_ALT_I       (XF_EXTEND + 0x17)
#define  XF_ALT_J       (XF_EXTEND + 0x24)
#define  XF_ALT_K       (XF_EXTEND + 0x25)
#define  XF_ALT_L       (XF_EXTEND + 0x26)
#define  XF_ALT_M       (XF_EXTEND + 0x32)
#define  XF_ALT_N       (XF_EXTEND + 0x31)
#define  XF_ALT_O       (XF_EXTEND + 0x18)
#define  XF_ALT_P       (XF_EXTEND + 0x19)
#define  XF_ALT_Q       (XF_EXTEND + 0x10)
#define  XF_ALT_R       (XF_EXTEND + 0x13)
#define  XF_ALT_S       (XF_EXTEND + 0x1F)
#define  XF_ALT_T       (XF_EXTEND + 0x14)
#define  XF_ALT_U       (XF_EXTEND + 0x16)
#define  XF_ALT_V       (XF_EXTEND + 0x2F)
#define  XF_ALT_W       (XF_EXTEND + 0x11)
#define  XF_ALT_X       (XF_EXTEND + 0x2D)
#define  XF_ALT_Y       (XF_EXTEND + 0x15)
#define  XF_ALT_Z       (XF_EXTEND + 0x2C)

#endif


