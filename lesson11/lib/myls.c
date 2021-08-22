#include "myls.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

/* функция возвращает массив с именами файлов, находящихся в директории path */
void my_ls(const char *path, char *files_name) {

    struct dirent *entry;  
    DIR *folder = opendir(path);

    if (folder == NULL) {                   /* проверка открытия директории */

        printf("Wrong path!\n");
        return;
    }

    while ( (entry = readdir(folder)) ) {
        
        if( (entry->d_name[0]) == '.')      /* убираем поддиректории */
            continue;

        strcat(files_name, entry->d_name);        /* записываем имя файла в массив */
        strcat(files_name, "\n");                 /* добавляем знак переноса строки */

        }

    closedir(folder);

}
