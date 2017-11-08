#include <stdio.h>
#include <dos.h>


int main()
{
	poke(0x40,0x1A,30);
	return 0;
}