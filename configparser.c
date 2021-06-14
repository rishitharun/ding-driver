/*

  ding network driver (c) Rishi Tharun
  ------------------------------------

  Source for ding network driver config parser.
  `````` ``` ```` ``````` `````` `````` ``````
  * Compile the source to generate the binary, only during debugging or development environments.
  * Separate compiling or any sort of action is not required during driver ko binary genaration.
  * Everything is taken care in the makefile.
  * If the config parser is good to go, just setup the .cfg file, and run 'make' in the project root

  * Author: Rishi Tharun <vrishitharunj@gmail.com>
  * License: GPLv3

*/

#define DCONFIG "dconfig.cfg"
#define HEADER_FILE_NAME "dingcfgs.h"
#define COMMENT_TOKEN '#'
#define TABSPACE_TOKEN '\t'
#define NULL_TOKEN '\0'
#define NEW_LINE_TOKEN '\n'
#define WHITESPACE_TOKEN ' '
#define EQUAL_TOKEN '='
#define BUFFER_DEFAULT_SIZE 100

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

FILE * header;

typedef struct
{
  char* buffer;
  short size;
} Buffer;

typedef enum { False = 0, True = 1 } boolean;
enum { INET, MASK, DLL, MTU, HW_ADDR, TOTAL_KEYS };
const unsigned char * keys[] = { "inet\0", "mask\0", "dll\0", "mtu\0", "hw_addr\0" };
const unsigned char key_lens [] = { 4,4,3,3,7 };

Buffer * createBuffer()
{
  Buffer * new_buffer = (Buffer *) malloc(sizeof(Buffer));
  new_buffer->size = BUFFER_DEFAULT_SIZE;
  new_buffer->buffer = (char *) malloc(new_buffer->size);
  return new_buffer;
}

void readToBuffer(Buffer * buffer, char data, short index)
{
  if (index+1 >= buffer->size)
  {
    buffer->size += BUFFER_DEFAULT_SIZE;
    buffer->buffer = (char *) realloc(buffer->buffer, buffer->size);
  }
  else {;}
  * ((buffer->buffer)+index) = data;
  * ((buffer->buffer)+index+1) = 0;
}

void freeBuffer(Buffer * buffer)
{
  free(buffer->buffer);
  free(buffer);
}

void writeHeader(unsigned char * value, unsigned char key)
{
  switch(key)
  {
    case INET:
      fprintf(header, "#define INET \"%s\"\n", value);
      break;
    case MASK:
      fprintf(header, "#define MASK \"%s\"\n", value);
      break;
    case DLL:
      fprintf(header, "#define DLL\n");
      fprintf(header, "#define %s\n",value);
      break;
    case MTU:
      fprintf(header, "#define MTU %s\n",value);
      break;
    case HW_ADDR:
      fprintf(header, "#define HW_ADDR \"%s\"\n", value);
      break;
    default:;
  }
}

char* readConfig(off_t* length)
{
  int file_desc;
  off_t file_size;
  char *source;

  file_desc = open(DCONFIG, O_RDONLY);
  if (file_desc < 0) { fprintf(stderr,"dingconfig.cfg file not found !\n"); return NULL; }
  else {;}

  file_size = lseek(file_desc, 0, SEEK_END);
  lseek(file_desc, 0, SEEK_SET);

  source = (char*)malloc((int) file_size);
  read(file_desc, source, file_size);
  close(file_desc);

  *length = file_size;
  return source;
}

boolean checkSubstr(char * str, const char * substr)
{
  int index = 0;
  boolean found = False;
  boolean condchar_flag = False;
  do
  {
    if(*str == EQUAL_TOKEN) condchar_flag = !condchar_flag;
    else {;}
    if(*str == *substr)
    {
      found = True;
      index++;
      if (!*(substr+1) && (!condchar_flag)) return True;
      else if (!*(substr+1) && (condchar_flag)) return False;
      else *(substr++);
    }
    else if (found) {substr-=index; index=0; found = False;}
    else{;}
  }while(*(++str));
  return False;
}

void parseLine(char * line, short line_len)
{
  int i=0;
  for(; i < TOTAL_KEYS; i++)
  {
    if(checkSubstr(line, keys[i]))
    {
      writeHeader(line+key_lens[i]+1,i);
      break;
    }
    else {;}
  }
}

void parse(char* source, int source_length)
{
  int source_index = 0;

  Buffer *line;
  line = createBuffer();

  boolean comment = False;
  short line_index = 0, read_lines = 0, paranthesis=0;

  for(;source_index<source_length;source_index++)
  {
    switch(*(source+source_index))
    {
      case COMMENT_TOKEN:
        comment = True;
        break;

      case TABSPACE_TOKEN:
      case WHITESPACE_TOKEN:
        continue;

      default:;
    }

    if(*(source+source_index) == NEW_LINE_TOKEN || *(source+source_index) == NULL_TOKEN)
    {
      comment = False;
      if(!line_index) goto skip_line;
      else {;}

      parseLine(line->buffer, line_index+1);

      skip_line:;
      line_index = 0;
      *(line->buffer) = 0;
    }
    else if(comment) continue;
    else readToBuffer(line, *(source+source_index), line_index++);
  }
  freeBuffer(line);
}

void main()
{
  off_t length;
  char * contents = readConfig(&length);

  if(contents == NULL) return;
  else {;}

  header = fopen(HEADER_FILE_NAME, "w");
  parse(contents, length);
  fclose(header);
  free(contents);
}
