#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int checkDups(const char* str)
{
  for(; *str != '\0'; str++) {
    for (const char* ahead = str + 1; *ahead != '\0'; ahead++) {
      if (*str == *ahead)
	return 1;
    }
  }
  return 0;
}

int main(int argc, const char* argv[])
{
  //assigns to and from args
  const char* from = argv[1];
  const char* to = argv[2];
  
  //check the two error cases
  if (strlen(from) != strlen(to)) //unequal arg lengths
    {
      fprintf(stderr, "The lengths of from and to must match.");
      exit(1);
    }
  if (checkDups(from)) //duplicated chars in from arg
    {
      fprintf(stderr, "The argument 'from' must not contain duplicate chars");
      exit(1);
    }

  //transliterate
  char buffer[] = " ";
  ssize_t byte;
  
  while (1) //will iterate until EOF
    {
      byte = read(0, buffer, 1); //read 1 byte from stdin into the buffer array

      if (byte == 0) //checks that not EOF
	break;

      if (byte < 0) //means there was a read error
	{
	  fprintf(stderr, "Could not read from standard input");
	  exit(1);
	}

      //transpose
      for (int i = 0; i < strlen(from); i++)
	{
	  if (buffer[0] == from[i]) //if current char is in from, change it
	    {
	      buffer[0] = to[i];
	      break;
	    }
	}

      //write to stdout
      byte = write(1, buffer, 1);
      if (byte < 0)
	{
	  fprintf(stderr, "Could not write to stdout");
	  exit(1);
	}
    }
}
