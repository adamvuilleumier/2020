#include <stdio.h>
#include <stdlib.h>
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
  //assign to and from
  const char* from = argv[1];
  const char* to = argv[2];
  
  //check the two error cases
  if (strlen(from) != strlen(to)) //unequal arg lengths
    {
      fprintf(stderr, "The lengths of from and to must match.");
      exit(1);
    }
  if (0)//checkDups(from)) //duplicated chars in from arg
    {
      fprintf(stderr, "The argument 'from' must not contain duplicate chars");
      exit(1);
    }

  //transliterate
  char curr;
  while (1) //while there is standard input
    {
      curr = getchar();
      if (feof(stdin))
	break;
      
      for (int i = 0; i < strlen(from); i++)
	{
	  if (curr == from[i]) //if current char is in from, change it
	    {
	      curr = to[i];
	      break;
	    }
	}
      putchar(curr); //write current char to stdout
    }
}
