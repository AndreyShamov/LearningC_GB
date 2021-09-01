#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define HELP_INSTRUCTION printf("When invoked without any arguments, programm lists the files in the current working directory\n"); \
                         printf("When invoked with argument -f <path>, the programm lists the files in the <path> directory (recursively)\n"); \
                         printf("When invoked with argument -l, the programm also lists a file's type\n");

enum {PATH_LENGTH = 256};


void my_ls(const char *path, const int l_argument);
void file_type(const char file_type);

/* Переделать программу листинга директории на рекурсивную. 
Так чтобы она принимала на вход аргумент -f <путь к директории> 
и выводила содержимое текущей директории и всех вложенных. 
Также программа должна поддерживать ключ -h по которому 
отображается текст подсказка.

Идея - выводим все файлы из директории и анализируем тип каждого файла.
Если находим директорию - вызываем функцию для вывода файлов 
в этой директории рекурсивно. 

UPDATE: для удобства добавил аргумент 'l', который выводит на экран также
тип файла
*/

/* Функция main принимает аргумент от пользователя */
int main (int argc, const char **argv) {

    char path[PATH_LENGTH] = { 0 };
    extern char *optarg;
    extern int optind;
    extern int enterr;
    extern int optout;
    const char *opts = "f:hl"; 
    int ret, l_arg = 0; 
              

/* если пользователь не вводит аргумент, программа берет путь каталога
из которого запущена сама программа, если аргумент введен - программа его
считает */

    if (argc == 1) {                /* записываем путь текущей директории, если программе не переданы аргументы */

        getcwd(path, PATH_LENGTH);

    } else {                        /* читаем переданные аргументы при их наличии */

        while ( (ret = getopt(argc, argv, opts)) != -1)  {

            switch(ret) {
                    case 'f': 
                        strcpy(path, optarg);
                        break;
                    case 'l': 
                        l_arg = 1;
                        break; 
                    case 'h': 
                        HELP_INSTRUCTION
                        return 0;                  
                    case '?':       /* помимо сообщения об ошибке выводим помощь */
                    printf("\nInstructions:\n");
                    HELP_INSTRUCTION
                    return 1;
            }

        }

    }

my_ls (path, l_arg);

putchar('\n');
    
return 0;
    
}

/* выводит имена файлов на стандартный поток ввода,
если это директория - вызывает функцию рекурсивно  */
void my_ls(const char *path, const int l_argument) {
  
    char path_to_dir[PATH_LENGTH] = { 0 };   /* массив для передачи пути к поддиректории при рекурсии */
    struct dirent *entry; 
    DIR *folder = opendir(path);
    DIR *folder_2;

    if (folder == NULL) {                   /* проверка открытия директории */

        printf("Wrong path!\n");
        return;
    }

/* вывод на экран содержимого текущий директории */
    while ( (entry = readdir(folder)) ) {
        
        if( (entry->d_name[0]) == '.')      /* сразу убираем поддиректории */
             continue;
        
        if (l_argument ==  1) {                  /* вывод в зависимости от наличия аргумента l */
            printf("%s\t", entry->d_name); 
        
       } else {
            printf("\n%15s", entry->d_name); 
            file_type(entry->d_type);       /* вызываем функцию, которая выводит на экран тип файла */
        }
    }

folder_2 = opendir(path);                    /* readdir() не хочет по второму разу работать с одной папкой, открываем путь еще раз */

/* рекурсивный вызов функции для вывода содержимого поддиректорий */
    while ( (entry = readdir(folder_2)) ) {
               

        if ( ( (entry->d_type) == 4) && ((entry->d_name[0]) != '.') ) { /* если файл - это директория и его имя не начинается '.'*/

            path_to_dir[0] = '\0';          /* вместо заполнения массива нулями меняем первый элемент на знак конца строки */
            strcat(path_to_dir, path);      /* создаем путь для поддиректории */
            strcat(path_to_dir, "/");              
            strcat(path_to_dir, entry->d_name);

            printf("\n\n%s:\n", path_to_dir);  /* аналогично ls -R выводим путь к поддиректории */
            my_ls(path_to_dir, l_argument); 
        
        }
    }
    
    closedir(folder);
    closedir(folder_2);       

}

/* функция выводит на экран тип файла */
void file_type(const char file_type) {

        switch (file_type) { 

            case DT_BLK:   printf("\t\tA block device");         break;  
            case DT_CHR:   printf("\t\tA character device");     break;  
            case DT_DIR:   printf("\t\tA directory");            break;  
            case DT_FIFO:  printf("\t\tA named pipe, or FIFO");  break;  
            case DT_LNK:   printf("\t\tA symbolic link");        break;  
            case DT_SOCK:  printf("\t\tA local-domain socke");   break;  
            default:       printf("\t\tOther files");            break;
        }

} 