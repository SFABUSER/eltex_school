#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <dirent.h>

void write_message() {
  char *message = "String from file";
  char *filename = "output.txt";
  FILE *fp = fopen(filename, "w");
  if (fp) {
    fputs(message, fp);
    fclose(fp);
  } else {
    printf("Ошибка открытия файла для записи\n");
  }
}

void read_message() {
  char *filename = "output.txt";
  char buffer[100];
  FILE *fp = fopen(filename, "r");

  if (fp) {
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
      int length = strlen(buffer);
      if (buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
        length--;
      }
      for (int i = length - 1; i >= 0; i--) {
        putchar(buffer[i]);
      }
      putchar('\n');
    }
    fclose(fp);
  } else {
    printf("Ошибка открытия файла для чтения\n");
  }
}

int main() {
  write_message();
  read_message();
  return 0;
}
