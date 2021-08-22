#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <myls.h>

enum {PATH_LENGTH=256};
extern void my_ls(const char *path, char *names);

/*Функция main может принять аргумент от пользователя*/
int main (int argc, const char **argv) {

    char path[PATH_LENGTH] = { 0 };
    char files_name[200] = { 0 };                /* строка для записи имен файлов */

    if (argc > 1) {                         /* Проверяем предоставлены ли доп. аргументы*/
        
        strcpy(path, argv[1]);              /* Записываем полученный аргумент в качестве пути */

    } else {        

        getcwd(path, PATH_LENGTH);          /* Берем путь текущего, если аргумент не предоставлен */
    }

my_ls(path, files_name);
printf("%s", files_name);  

    
}