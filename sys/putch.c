#include <sys/putcharacter.h>

void
putch(
	  unsigned char c
	 )
{
	if (x == HOR_BITS)
    {
        x = 0;
        y++;
    }
    if (y == VERT_BITS)
    {
        y = 0;
        x = 0;
    }

    if ((char) c == '\n')
    {
        x = 0;
        y++;
    }
    if ((char) c == '\t')
    {
        switch ((x + TABSPACE) - HOR_BITS)
        {
            case 0:
                x = 4;
                y++;
            break;
            case 1:
                x = 3;
                y++;
            break;
            case 2:
                x = 2;
                y++;
            break;
            case 3:
                x = 1;
                y++;
            break;
            case 4:
                x = 0;
                y++;
            break;
            default:
                x += TABSPACE;
            break;
        }
    }
    if ((char) c == '\b')
    {
        if ((x == 0) && (y != 0))
        {
            placechar(' ', x, y, 0x07);
            y--;
            x = HOR_BITS;
        }
        else
        {
            placechar(' ', x, y, 0x07);
            x--;
        }
    }
    else
    {
        placechar(c, x, y, 0x07);
        x++;
    }
}
