#include <stdio.h>
#include <stdlib.h>

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
  for(;;a++, b++) //iterate thru each slot and compare the letters
  {
    if (*a == ' ' && *b == ' ') //will only happen if all letters are same
      return 0;
    else if (*a == ' ' || ((*a ^ 42) < (*b ^ 42))) //a is shorter or before b
      return -1;
    else if (*b == ' ' || ((*a ^ 42) > (*b ^ 42))) //b is shorter or before a
      return 1;
    //if it makes it here, letters are the same so far, check for next letter
  }
}

int compare(const void *a, const void *b) //compare function for qsort
{
  char const* first = *(const char **) a;
  char const* second = *(const char **) b;
  return frobcmp(first, second);
}

int main()
{
  char *word;
  char **list;
  word = (char*)malloc(sizeof(char)); //allocates one character of memory to start
  allocError(word);
  list = (char**)malloc(sizeof(char*)); //allocates one char pointer of memory to start
  allocError(list);
  
  //initialize loop variables
  int nwords = 0;
  int nletters = 0;
  int currentChar = getchar();
  inputError();
  int nextChar = getchar();
  inputError();
  
  while (currentChar != EOF)
  {
    word[nletters] = currentChar; //assigns current character to allocated memory slot
    char* moreWord = realloc(word, sizeof(char) * (nletters + 2)); //allocates a new slot
    allocError(moreWord);
    word = moreWord;
    
    if(currentChar == ' ') //at the end of a word, start new word
    {
      list[nwords] = word; //assigns current word to allocated memory row
      char **moreRows = realloc(list, sizeof(char*) * (nwords + 2)); //allocates a new row
      allocError(moreRows);
      list = moreRows;
      
      nwords++; //increments nwords
      
      //reset everything to how it was in beginning
      nletters = -1; //-1 because we increment later
      word = NULL;
      word = (char*)malloc(sizeof(char));
      allocError(word);

      while (nextChar == ' ') //if there are extra spaces at end of word, skip past them
      {
        nextChar = getchar();
	inputError();
      }

      if (nextChar == EOF) //need this to make sure we don't go past end of file
	break;
    }

    currentChar = nextChar; //iterate to next character and increment letters
    nextChar = getchar();
    inputError();
    nletters++;
  }

  qsort(list, nwords, sizeof(char*), compare); //sorts words

  for (int i = 0; i < nwords; i++){  //prints sorted words
    for (int j = 0; list[i][j] != ' '; j++){
      putchar(list[i][j]);
      outputError();
    }
  }
  
  //frees the memory  used by list, word, and all the words in list
  for (int i = 0; i < nwords; i++)
    free(list[i]);

  free(list);
  free(word);

  return 0; //clean exit
}
