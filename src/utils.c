#include "malloc.h"

int ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void ft_putstr(char *str)
{
	write(1, str, ft_strlen(str));
}

// A recursive itoa_base function with uppercase letters
void ft_itoa_base(size_t n, char base, char width)
{
	char *base_str;

	base_str = "0123456789ABCDEFGHIJKLMNOPQRSTUIVWXYZ";
	if (n / base)
		ft_itoa_base(n / base, base, width - 1);
	else
	{
		while (--width > 0)
		{
			ft_putstr("0");
		}
	}
	write(1, &base_str[n % base], 1);
}

void ft_memset(void *ptr, int c, size_t size)
{
	char *p = ptr;
	while (size--)
	{
		*p++ = c;
	}
}

void ft_bzero(void *s, size_t n)
{
	unsigned char *ptr = s;
	while (n--)
	{
		*ptr++ = 0;
	}
}

void ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char *d;
	unsigned char *s;

	d = (unsigned char *)dst;
	s = (unsigned char *)src;
	if (d < s)
	{
		while (len--)
			*d++ = *s++;
	}
	else
	{
		d += len;
		s += len;
		while (len--)
			*--d = *--s;
	}
}