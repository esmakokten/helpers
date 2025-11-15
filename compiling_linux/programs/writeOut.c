#include <stdint.h>
#include <stdio.h>
#include <sys/io.h>
#include <string.h>

void writeChar(char c)
{
    // The 'volatile' keyword prevents the compiler from optimizing away the instruction
    // if it thinks the output is unused.
	__asm__ __volatile__ (
	"outb %b0, %w1"           // Assembly instruction: outb source, destination
	:                       // No output operands
	: "a"(c), "Nd"(0xe9) // Input operands: %0 gets 'data' in 'a' register, %1 gets 0xe9 in 'd' register
    	: "memory"
   );
}
void writeString(char* s){
	for(int i=0;i<strlen(s);i++){
		writeChar(s[i]);
	}
}

int main(void)
{
	iopl(3);
	int character;
	writeChar('\n');
	while ((character = fgetc(stdin)) != EOF) {
		writeChar(character); 
	}

	return 0;
}
