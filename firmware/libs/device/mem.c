

typedef unsigned int size_t;

void *memset(void *dest, int value, size_t len) 
{
    unsigned char *ptr = (unsigned char *)dest;
    while (len--) {
        *ptr++ = (unsigned char)value;
    }
    return dest;
}
