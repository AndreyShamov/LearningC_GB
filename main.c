#include <stdio.h>
#include <stdlib.h>

/* 
Написать программу, которая копирует файл, имя которого введено пользователем в файл,
имя которого введено пользователем после.
После копирования вывести количество скопированных байт.
В случае ошибки открытия файла, вывести сообщение о невозможности открытия исходного файла.
Режим работы в файлами - двоичный( "rb", "wb" ) и так далее

например:

Enter source and destination files:
file1 file2
Сopy done. Transfered 100500 bytes

Или

Enter source and destination files:
file1 file2
Error. file1 not exists
*/

/* Создаем файлы, заполняем первый файл массивом со случайными символами */
int init_file(const char *file1_name, const char *file2_name) {

	char random_char[30];
	FILE *file;
    FILE *file2;

	random_char[29] = '\0';                         /* чтобы массив воспринимался как строка, добавляем символ конца строки */   

		for(int i = 0; i < 29; i++) {               /* заполняем массив (кроме последнего символа) случайными символами ascii с кодом от 0 до 100 */
			random_char[i] = rand() % 100;	
		}

	file = fopen(file1_name, "w+");
		
		if (file == NULL) {
			printf("Cannot create the file %s", file1_name);
            return 1;                               /* выход из функции в случае ошибки */
		}

	fwrite(random_char, sizeof(char), 30, file);

	fclose(file);

    file2 = fopen(file2_name, "w+");                /* создаем второй файл */
		if (file == NULL) {
			printf("Cannot create the file %s", file2_name);
            return 1;                               /* выход из функции в случае ошибки */
		}        
    fclose(file2);
    return 0;

}

/* функция копирует содержимое первого файла во второй */
int copy_file(const char *init_file_name, const char *copy_file_name) {
	
	int num;
	char tmp[1000];
	
	FILE *init_file = fopen(init_file_name, "rd");
	FILE *copy_file = fopen(copy_file_name, "wd");

	if ( (init_file != NULL) && (copy_file != NULL) ) {
		
		num = fread(tmp, sizeof(char), 1000, init_file);
		fwrite(tmp, sizeof(char), num, copy_file);

	    } else if (init_file == NULL) {
                printf("Cannot open the file %s", init_file_name);
                return 1;

        } else if (copy_file == NULL) {
                printf("Cannot open the file %s", copy_file_name);
                return 1;
    }

	fclose(init_file);
	fclose(copy_file);

    printf("Sucessfully copied %lu bytes from file %s to file %s\n", (num * sizeof(char)), init_file_name, copy_file_name);
    return 0;

}	

/* функция main запрашивает у пользователя, что он хочет сделать: 
можно копировать информацию из существующего файла в другой существующий файл
 или создать два новых файла, заполнить один из них массивом символов и скопировать содержимое 
 этого файла в другой файл */
int main () {

	char name1[10];
	char name2[10];
    char c;
    int i;

    printf("This program copies an initial file to an another file.\n");
    printf("Print 'Y' if you want to create two new files.\nPrint 'N' if you want to copy from/to alrady existed files \n");
       
        c = getchar();

        if ( (c == 'Y') || (c == 'y') ) {                   /*если пользователь хочет посмотреть корректность работы программы без создания файлов*/
            	printf("The programm will create two new files.\nEnter the first and the second files' names:\n");
	            scanf("%s %s", name1, name2);
                i = init_file(name1, name2);
                       
                        i == 0 ? copy_file(name1, name2) : 1;
                         

        } else if ( (c == 'N') || (c == 'n') ) {            
                printf("Enter the first and the second existed files' names:\n");
                scanf("%s %s", name1, name2);
                i = copy_file(name1, name2);


        } else {                                            /* еще раз запрашиваем пользователя, что он хочет сделать*/
                printf("Plese input only 'Y' or 'N'\n");
                main();
        }
}
