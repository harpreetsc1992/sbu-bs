#include <sys/kb.h>

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,					/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/* Keyboard mapping on SHIFT press */
unsigned char kbdshift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* Curly Brackets */
    '(', ')', '_', '+', '\b', /* Backspace */
    '\t',           /* Tab */
    'Q', 'W', 'E', 'R',   /* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',   /* Enter key */
    0,          /* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
    '\"', '~',   0,      /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',         /* 49 */
    'M', '<', '>', '?',   0,              /* Right shift */
    '*',
    0,    /* Alt */
    ' ',  /* Space bar */
    0,    /* Caps lock */
    0,    /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,    /* < ... F10 */
    0,    /* 69 - Num Lock*/
    0,    /* Scroll Lock */
    0,    /* Home key */
    0,    /* Up Arrow */
    0,    /* Page Up */
    '-',
    0,    /* Left Arrow */
    0,
    0,    /* Right Arrow */
    '+',
    0,    /* 79 - End key*/
    0,    /* Down Arrow */
    0,    /* Page Down */
    0,    /* Insert Key */
    0,    /* Delete Key */
    0,   0,   0,
    0,    /* F11 Key */
    0,    /* F12 Key */
    0,    /* All other keys are undefined */
}; 

unsigned char kbdcaps[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',         /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',     /* Enter key */
    0,          /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', /* 39 */
 '\'', '`',   0,        /* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 49 */
  'M', ',', '.', '/',   0,                  /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

char buf[1024];
int flag;
uint16_t counter;
uint64_t* gets_at;
last_key_pressed lkp;

char*
gets(
	 char *s
	)
{
	char *user_buf = s;

	flag = 1;
	kmemcpy((void *)user_buf, (void *)buf, counter);
	counter = 0;
	flag = 0;
	return s;
}

void
init_kbd(
		)
{
	lkp = NONE;
	gets_at = NULL;
	counter = 0;
}

/* Handles the keyboard interrupt */
void
keyboard_handler(
				 regs_t r
				)
{
    unsigned char scancode, value;

	value = 0;
    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
		if ((scancode == 157) || (scancode == 170) || (scancode == 184))
		{
			 lkp = NONE;
		}
		else
		{
			if (scancode == 29)
			{
				lkp = CTRL;
			}
			else if ((scancode == 42) || (scancode == 54))
			{
				lkp = SHIFT;
			}
			else if (scancode == 56)
			{
				lkp = ALT;
			}
			else if (scancode == 58)
			{
				lkp = CAPS;
			}
			else
			{
				switch (lkp) 
				{
					case NONE:
						value = kbdus[scancode];
					break;
					case CTRL:
						value = kbdus[scancode];
					break;
					case SHIFT:
						value = kbdshift[scancode];
					break;
					case ALT:
						value = kbdus[scancode];
					break;
					case CAPS:
						value = kbdcaps[scancode];
					break;
					default:
						value = 0;
					break;
				}
			}
			if (flag == 1) 
			{
				if (value == '\n') 
				{
					buf[counter++] = '\0';
					putch(value);
					flag = 0;
                }
				else if (value == '\b') 
				{
					putch(value);
					counter--;
                }
				else
				{
					buf[counter++] = value;
					putch(value);
                }
			}
		}
    }
}
