#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>

//sets global variable to recognize -f
int f_flag = 0;

void inputError()
{
  if(ferror(stdin))
    {
      fprintf(stderr, "Error reading from stdin");
      exit(1);
    }
}

void outputError()
{
  if (ferror(stdout))
    {
      fprintf(stderr, "Error writing to stdout");
      exit(1);
    }
}

void allocError(void *ptr)
{
  if (ptr == NULL)
    {
      fprintf(stderr, "Error: Cannot allocate space");
      exit(1);
    }
}

int frobcmp(char const *a, char const *b)
{
  if (f_flag) //when the -f argument is active
    {
      for (;;a++, b++)
	{
	  if (*a == ' ' && *b == ' ')
	    return 0;
	  else if (*a == ' ' || (toupper(*a ^ 42) < toupper(*b ^ 42)))
	    return -1;
	  else if (*b == ' ' || (toupper(*a ^ 42) > toupper(*b ^ 42)))
	    return 1;
	}
    }
  else
  {
    for (;;a++, b++) //iterate thru each slot and compare the letters
      {
	if (*a == ' ' && *b == ' ') //will only happen if all letters are same
	  return 0;
	else if (*a == ' ' || ((*a ^ 42) < (*b ^ 42))) //a is shorter/ before b
	  return -1;
	else if (*b == ' ' || ((*a ^ 42) > (*b ^ 42))) //b is shorter/ before a
	  return 1;
    //if it makes it here, letters are the same so far, check for next letter
    }
  }
}

int compare(const void *a, const void *b) //compare function for qsort
{
  char const* first = *(const char **) a;
  char const* second = *(const char **) b;
  return frobcmp(first, second);
}

void strWrite(const char* s) //writes a one word string to stdout
{
  ssize_t a;
  for (;; s++)
    { 
      a = write(1, s, 1); //write one byte to stdout
      outputError();

      if (*s == ' ') //all words end with a space
	return;
    }
}

int main(int argc, char *argv[])
{
  if (argc > 2) //check for correct number of args (including location 1st arg)
    {
      fprintf(stderr, "Error: Too many arguments");
      exit(1);
    }
  else if (argc == 2)
    {
      if (argv[1][0] == '-' && argv[1][1] == 'f')
	f_flag = 1;
      else
	{
	  fprintf(stderr, "Error: Invalid argument");
	  exit(1);
	}
    }

  struct stat f; //makes struct for fstat
  
  if (fstat(0, &f) < 0)
    {
      fprintf(stderr, "Error reading file");
      exit(1);
    }

  char *wordBuff; //single word buffer
  char curr; //currently viewed character
  char ioBuff[] = " "; //buffer to pass to write()

  //counters and info
  int nwords = 0;
  int nletters = 0;
  int bufferSize = 0;
  bool isEOF = false;

  int maxBufferSize;
  if (S_ISREG (f.st_mode))
    maxBufferSize = f.st_size + 1; //allocates a 1 byte larger than needed buff
  else
    maxBufferSize = 8192; //allocates an 8 kB buffer

  wordBuff = malloc(sizeof(char) * maxBufferSize);
  allocError(wordBuff);
  
  while (isEOF == false)
  {
    ssize_t retVal = read(0, ioBuff, 1); //read 1 char into ioBuff
    inputError();
    isEOF = !retVal; //retVal will be 0 iff read has reached end of file
    curr = ioBuff[0];

    if (nletters == 0 && curr == ' ') //ignore repeated spaces
      continue;

    if (bufferSize > (maxBufferSize / 2)) //if buffer is > half filled, double its capacity
      {
	maxBufferSize *= 2;
	char* doubleBuff = realloc(wordBuff, sizeof(char) * maxBufferSize);
	allocError(doubleBuff);
	wordBuff = doubleBuff;
      }

    if (isEOF == 0) //if the current char isn't EOF
      {
	wordBuff[bufferSize] = curr;
	bufferSize++;
	nletters++;
	if (curr != ' ') //if the word hasn't ended, don't execute below code
	  continue;
      }
    else
      {
        if (wordBuff[bufferSize - 1] != ' ') //if the last letter isn't a space...
	  {
	    wordBuff[bufferSize] = ' '; //add a space to the end
	    bufferSize++;
	  }
      }
    nletters = 0;
    nwords++;
  }

  //at this point in the code, wordBuff has all input words separated by spaces
  
  char** list = malloc(sizeof(char*) * nwords);
  allocError(list);

  char* buffPtr = wordBuff; //set buffPtr to the beginning of the wordBuff
  for (int i = 0, wordIndex = 0; i < bufferSize; i++)
    {
      if (wordBuff[i] == ' ') //separate words by space, add to list
	{
	  list[wordIndex] = buffPtr;
	  wordIndex++;
	  buffPtr = wordBuff + i + 1; //sets ptr to start of next word (after space)
	}
    }

  qsort(list, nwords, sizeof(char*), compare); //sorts words

  for (int i = 0; i < nwords; i++)  //prints sorted words
    strWrite(list[i]);
  
  //frees the memory
  free(list);
  free(wordBuff);

  return 0; //clean exit
}
