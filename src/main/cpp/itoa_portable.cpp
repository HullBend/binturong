/**
 * itoa_portable.cpp
 *
 * Caution: this is 32 Bit code!
 */

#include "itoa_portable.h"


static void _xtoa(unsigned long val, char* buf, unsigned base, int is_neg)
{
    char* p;                /* pointer to traverse string */
    char* firstdig;         /* pointer to first digit */
    char temp;              /* temp char */
    unsigned digval;        /* value of digit */

    p = buf;

    if (is_neg) {
        /* negative, so output '-' and negate */
        *p++ = '-';
        val = (unsigned long) (-(long) val);
    }

    firstdig = p;           /* save pointer to first digit */

    do {
        digval = (unsigned) (val % base);
        val /= base;       /* get next digit */

        /* convert to ascii and store */
        if (digval > 9) {
            *p++ = (char) (digval - 10 + 'a');  /* a letter */
        } else {
            *p++ = (char) (digval + '0');       /* a digit */
        }
    } while (val > 0);

    /* We now have the digit of the number in the buffer, but in reverse
       order.  Thus we reverse them now. */

    *p-- = '\0';            /* terminate string; p points to last digit */

    do {
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;   /* swap *p and *firstdig */
        --p;
        ++firstdig;         /* advance to next two digits */
    } while (firstdig < p); /* repeat until halfway */
}


/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char* itoa_portable(int val, char* buf, int base) {
    if (base == 10 && val < 0) {
        _xtoa((unsigned long) val, buf, base, 1);
    } else {
        _xtoa((unsigned long) (unsigned int) val, buf, base, 0);
    }
    return buf;
}

char* ltoa_portable(long val, char* buf, int base) {
    _xtoa((unsigned long) val, buf, base, (base == 10 && val < 0));
    return buf;
}

char* ultoa_portable(unsigned long val, char* buf, int base) {
    _xtoa(val, buf, base, 0);
    return buf;
}

char* itoa_portable(int val, char* buf) {
    return itoa_portable(val, buf, 10);
}

char* ltoa_portable(long val, char* buf) {
    return ltoa_portable(val, buf, 10);
}

char* ultoa_portable(unsigned long val, char* buf) {
    return ultoa_portable(val, buf, 10);
}
