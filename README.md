# Програми на C, използващи системни извиквания

## 1. Увод:
* синтаксис на С
* как компилираме код на С:
    * използваме дадения **Makefile** 
    * слагаме го в директорията на задачата, където трябва да има main.c файл
    * пишем "make" и получаваме нови два file-а: main.o, main
* как пускаме програма на С:
    * ./main и подаваме след това параметрите, с които искаме да стартираме програмата
	
## 2. Функция main:
* имаме два варианта за типа на функцията main:
    * int main(void) {}
    * int main(int argc, char* argv[]){}
        * **argc** = number of arguments
        * **argv[0]** = pointer to the 0th element
            ( the executable path )
        * **argv[1]** = pointer to the 1st element
        * **argv[2]** = pointer to the 2nd element
        * ...
    * има възможност и за трети аргумент: char* envp[]
        * това е масив от низове ( string array ), завършващ с NULL pointer
        * можеш да го обходиш, за да видиш всички environment променливи

## 3. Exit status:

```c
int main(void) {
    return 42;
} // exit status = 42
```


* exit status-ът на една програма е числото, върнато от функцията main
* върнатото число не е просто произволно; операционната система го интерпретира по конкретен начин
    * exit status = 0 - програмата е свършила работата си успешно
    * exit status != 0 - програмата не е свършила работата си успешно
* върнатото число може да е в интервала от 0 до 255
    * ако върнеш 256 реално системата ще върне 0, защото mod(256) = 0
* с echo $? можем да принтираме exit status-a на последно върнатата команда

## 4. Фиксирани числови типове:
* в рамките на този курс искаме прецизен контрол на подредбата и размера на данните в паметта
* за това ще използваме целочислените типове с фиксиран размер от библиотеката stdint.h

* **uint8_t** = 1 byte ( 8 bits ) =  number between 0 and 255
* **uint16_t** = 2 bytes ( 16 bits ) = numbers to 65K
* **uint32_t** = 4 bytes ( 32 bits ) = numbers to 4 000 000 000
* **uint64_t** = 8 bytes ( 64 bits) 
* **float** = 4 bytes
* **double** = 8 bytes
* **int** = 4 bytes
* **long** = 8 bytes

    ```c
	int main(void) {
		uint16_t foo = 42;
		int32_t bar = -16;
		uint8_t some_bytes[20];
		some_bytes[0] = 0x26;
	}
    ```

## 5. Представяне на текст: В C няма специален тип данни "string", както в по - модерните езици. Текстът се представя като поредица от символи в паметта, като един символ (char) заема точно един байт.
* Низове с терминираща нула:  char name[] = "Иван"
    * в паметта се заделят 5 байта, а не 4
* **strlen(string)** = размера на самия стринг преди терминиращата нула
* **sizeof(string)** = байтовете на самия стрринг, тоест с 1 повече от размера му заради терминиращата нула

* Указател към низ: char* name = "Ivan"
    * вместо да пазиш целия масив, ти пазиш само адреса в паметта, където започва текста

* Буфер и дължина:
    * В Операционните системи често се използва и друг подход: заделяш фиксирано парче памет (буфер) и сам си записваш в отделна променлива колко байта от него са пълни.
        
        ```c
        char name_buf[256];     // Заделяш твърдо 256 байта памет
        uint16_t name_len = 4;  // Изрично пазиш, че в момента ползваш само 4 от тях
        ```

    При този подход не се интересуваш от терминираща нула. Ти знаеш точно откъде докъде са ти данните, благодарение на променливата name_len. Това се ползва постоянно при четене и писане във файлове и мрежови буфери.

## 6. Записи и структури от данни:
* можем да дефинираме променлива от записен тип със struct:

```c
    struct {
        char name[20];
        uint8_t age;
    } ivan;
```
* в C има различни конвенции за дефиниране на записен тип:
    ```c
    // Като отделен типов синоним:
    typedef struct {
        char name[20];
        uint8_t age;
    } person_t;
    
    person_t ivan;
    ```

    ```c
    // Като запис в таблицата от структури:
    struct person {
        char name[20];
        uint8_t age;
    };

    struct person ivan;
    ```

## 7. Управление на динамична памет:
* алокираме с malloc()
* деалокираме с free()

    ```c
    person_t* people = malloc(num_people * sizeof(person_t));
    people[42].age = 26;
    free(people);
    ```

* динамично алокираната памет е ресурс, който трябва да бъде следен ръчно и освобождаван навреме
* в рамките на курса почти никога няма да се налага да ползваме динамична памет


--- 


# Езикът C и системни извиквания

## 1. Системни извиквания:
* дотук знаем, че операционната система имплементира комуникация между процеси и връзка на процесите с външния свят
* връзката между един процес и (ядрото на) операционната система се извършва чрез операции, наречени системни извиквания (system calls или syscalls)
* в този раздел ще се научим да пишем програми, които директно използват системните извиквания на операционната система
    * това се нарича "системно програмиране"

* от гледна точка на програмиста, системните извиквания са "просто" библиотечни функции, които може да извика
* специалното на тези функции е, че вместо да изпълняват код като част от програмата, казват на ядрото да изпълни съответната операция и чакат резултат
* програмата заспива докато ядрото не стане готово с изпълнението на системното извикване, и когато се събуди, получава резултат
* тъй като тези функции "обвиват" системните извиквания, ще ги наричаме syscall wrappers

## 2. Как работят системните извиквания:
* най - често използваната библиотека, имплементираща **syscall** wrappers е **glibc**
    * **glibc** - стандартна библиотека за **C**, която освен syscall wrappers имплементира и всички стандартни функции от C
    * има и други такива библиотеки
* съответните библиотеки съдържат хедъри със стандартизирани имена, в които се дефинират **syscall wrapper-ите**

## 3. Какво реално прави един syscall wrapper?
* копира аргументите на системното извикване и неговия номер в специфични процесорни регистри
* изпълнява процесорната инструкция, която предизвиква хардуерно прекъсване
* вади резултата от въпросния процесорен регистър и го връща

## 4. Защо C?
* **езикът, на който е написан Linux (или други UNIX-и) е C**
* по тази причина, системните извиквания използват подреждане и формат на данните, съобразени с ABI (Aplication Binary Interface) на C
* затова повечето библиотеки, имплементиращи syscall wrappers, са написани на C
    * езици като Go също ги имат имплементирани
    * други езици (повечето) използват тези на C

## 5. Пример за системни извиквания на С:
* системните извиквания getuid и geteuid връщат реалното и ефективното UID на процеса, изпълняващ програмата

    ```c
    #include <unistd.h> // getuid, geteuid
    #include <sys/types.h> // uid_t
    #include <stdio.h> // printf

	int main(void) {

		uid_t me = getuid();
		uid_t pretending = geteuid();
		printf("uid: %d euid: %d\n", me, pretending);
		return 0;
	
	}
    ```

* man syscall:
    * getuid(), geteuid(), printf
	
## 6. Exit status:
* върнатата стойност от main() функцията

    ```c
    int main(void) {
        return 42; // exit status 42
    }
    ```

* системното извикване _exit() прекратява изпълнението на процеса, независимо от текущата фунцкия:

    ```c
    #include <unistd.h> // _exit
    void foo(void) {
        _exit(42);
    }

    int main(void) {
        foo();
	}
    ```


---


# Обработване на грешки при системни извиквания

## 1. Резултат от системни извиквания:
* по конвенция, повечето системни извиквания връщат резултат от числов тип, който е отрицателно число, ако извикването е било неуспешно
* open() - системно извикване, отварящо файл
* int open(const char* pathname, int flags);
    * първият аргумент е път до файл
    * вторият аргумент е множество от опции, задаващи режима на отваряне на файл
    * резултатът при успех е положително число - номер на файлов дескриптор
    * -1 = резултатът ако отварянето на файл е било неуспешно
	
    ```c
	#include <fcntl.h> // open
	#include <stdlib.h> // exit

	void tell_user(const char* msg) {
		// за сега приемете, че тази функция показва съобщение на потребителя и я имаме наготово
	}

	int main() {

		int result = open("/tmp/some_file", 0_RDONLY);
		if (result < 0) {
			tell_user("open /tmp/some_file failed\n");
			exit(1);
		}

		tell_user("opened /tmp/some_file successfully\n");
	}
    ```

## 2. Errno:
* когато някое системно извикване е неуспешно, ни се иска начин да разберем какво не е било наред
* за това при неуспех системните извиквания записват число (код на грешка) в глобалната променлива errno
    * errno(3) for more info

    ```c
    int main() {
        int result = open("/tmp/some_file", O_RDONLY);
        
        if (result < 0) {
            switch (errno) {
            case 2: tell_user("no such file\n"); break;
            case 13: tell_user("permission denied\n"); break;
            ...
            }
            exit(1);
        }

        tell_user("opened /tmp/some_file successfully\n");
    }
    ```

## 3. Err.h:
* ако просто искаме да изведем съобщение за грешка до потребителя и да прекратим програмата, най - добре е да използваме функцията err() от err(3), която изписва форматирано 
съобщение за грешка (вътрешно гледа променливата errno)
* първият аргумент на err е exit status, с който да прекрати програмата.

    ```c
	#include <fcntl.h> // open
	#inclued <err.h> // err

	int main(void) {
		int result = open("/tmp/some_file", O_RDONLY);

		if (result < 0) {
			err(1, "could not open file");
		}

		tell_user("opened /tmp/some_file successfully\n");
	}
    ```

* всъщност, err.h задава 4 полезни функции, изписващи съобщение на stderr:
    * void err(int eval, const char* fmt, ...)
        * изписва грешката от errno
        * прекратява програмата с подадения статус
    * void errx(int eval, const char* fmt, ...)
        * НЕ изписва грешката от errno
        * прекратява програмата с подадения статус
    * void warn(const char* fmt, ...)
        * изписва грешката от errno
        * НЕ прекратява програмта
    * void warnx(const char* fmt, ...)
        * НЕ изписва грешката от errno
        * НЕ прекратява пгорамата


--- 


# Файлови дескриптори

* за да работим с файл, трябва да го отворим - това става със системното извикване open(2)
* open() връща число - номер на файлов дескриптор
* при отваряне на файл, ядрото създава системна структура, наречена файлов дескриптор, която съдържа:
	* указател към самия файл
	* текуща позиция (индекс на байт) във файла
	* и други
* за всеки процес ядрото алокира таблица от файлови дескриптори: номерът на файлов дескриптор, върнат от open(), е индекс в тази таблица

## 1. Опции на open():
* open() може да приема 2 или 3 аргумента
    * int open (const char* pathname, int flags);
    * int open (const char* pathname, flags, mode_t mode);
    
    * pathname = път до файл
    * flags = множество от опции
    * mode = права за достъп - представени като число, което е най - удобно да подадем в осмична бройна система

* комбинираме опциите с побитово "ИЛИ"
* важни опции:
    * O_WRONLY, O_RDONLY = отваряне за писане или за четене
    * O_RDWR = отваряне за четене и писане едновременно
    * O_CREAT = ако файлът не съществува, го създава преди да го отвори
    * O_TRUNC = ако файлът съществува, зачиства съдържанието му преди да го отвори
    * O_APPEND = ако файлът съществува, началната позиция е в края му вместо в началото
* третият аргумент на open() задава права за достъп, ако го отваряме сега
    * можем да ги зададем директно с число 0644
* когато един процес умре, всички негови файлови дескриптори се затварят автоматично
* можем ръчно да затворим файлов дескриптор, използвайки системното извикване close(2)
* добре е отворените файлове да се затварят веднага, щом сме приключили да работим с тях

    ```c
	#include <fcntl.h> \\ open, close
	#include <err.h> \\ err

	int main(void) {
		const char filename[] = "/tmp/some_file";
		int fd = open(filename, O_RDONLY);

		if (fd < 0) {
			err(1, "could not open file %s", filename);
		}

		// ...

		if (close(fd) < 0) {
			err(1, "could not close file $s", filename);
		}
	}
    ```

## 2. Четене от файл:
* можем да четем от отворен файл със системното извикване read(2):
* аргументи на read са:
    * номер на файлов дескриптор, от който четем
    * указател към памет, в която искаме да се запишат прочетените данни
    * максимална дължина за четене
* резултатът от read е броят реално прочетени байтове
* при успешно изпълнение на read(), текущата позиция на файловия дескриптор се предвижва напред с броя успешни пречетени байтове
    * това означава, че всяко следващо викане на read() чете нови данни от файла
* ако read() прочете 0 байта, това означава, че сме стигнали края на файла
	
    ```c
	char buff[20];
	int num_bytes = read(fd, buff, 20);
	if (num_bytes < 0) {
		err(1, "could not read data");
	}
    ```

## 3. Писане във файл:
* писането във файл е аналогично на четенето - чрез системното извикване write(2)
* аргументите на write() са:
    * номер на файлов дескриптор, в който пишем
    * указател към памет, от която да се прочетат данните
    * брой байтове (размер на данните), които искаме да запишем
* резултатът от write() е броят реално записани байтове

    ```c
	char buf[] = "Hello world!\n";
	int num_bytes = write(fd, buf, strlen(buf));
	if (num_bytes < 0) {
		err(1, "could not write the data");
	}
	if (num_bites != strlen(buf)) {
		errx(1, "could not write data all at once");
	}
    ```

* при записване на низ в текстов файл трябва да внимаваме да не запишем терминиращата нула във файла
    * функцията strlen() е полезна: връща размера на низа, без да включва терминиращата нула

## 4. Файлови дескриптори на стандартни потоци:
* стандартните потоци stdin, stdout, stderr по подразбиране съществуват при създаване на процес:
    * stdin = 0
    * stdout = 1
    * stderr = 2
	
    ```c
	char name_buf[512];
	const char prompt[] = "What's your name? ";
	const char hello[] = "Hello, ";
	const char end[] = "!\n";

	int write_result = write(1, prompt, strlen(prompt));
	if (write_result < 0) {
		err(1, "could not write prompt");
	}

	int name_len = read(0, name_buf, sizeof(name_buf));
	if (name_len < 0) {
		err(1, "could not read the name");
	}

	write_result = write(1, hello, strlen(hello));
	if (write_result < 0) {
		err(1, "could not write hello");
	}

	write_result = write(1, name_buf, name_len);
	if (write_result < 0) {
		err(1, "could not write name");
	}

	write_result = write(1, end, strlen(end));
	if (write_result < 0) {
		err(1, "could not write end");
	}
    ```

## 5. Преместване на текущата позиция във файл:
* със системното извикване lseek(2) можем да преместим текущата позиция на произволно място във файла:
* аргументи на lseek() са:
    * файлов дескриптор
    * отместване
    * интерпретация на отместването
* резултатът от lseek() е новата абсолютна позиция
* възможни интерпретации на отместването са:
    * SEEK_SET = абсолютно отместване
    * SEEK_CUR = относително отместване спрямо текущата позиция
    * SEEK_END = относително отместване спрямо края на файла

	* off_t lseek(int fd, off_t offset, int whence);

    ```c
    // jump in the beginning of the file:
    int new_pos = lseek(fd, 0, SEEK_SET);
    if (new_pos < 0) {
        err(1, "could not go to the start of the file");
    }
    ```

    ```c
    // jump position 42:
    int new_pos = lseek(fd, 42, SEEK_SET);
    if (new_pos < 0) {
        err(1, "could not go byte 42");
    }
    ```

    ```c
    // jump 5 bytes backwards:
    int new_pos = lseek(fd, -5, SEEK_CUR);
    if (new_pos < 0) {
        err(1, "could not jump 5 bytes backwards");
    }
    ```


---


# Четене и писане на двоични данни от паметта във файлове

## 1. Форматиран и неформатиран вход/изход
* мислено можем да разделим подходите за вход/изход на две категории:
    * форматиран вход/изход
    * неформатиран вход/изход
	
## 2. Неформатиран вход/изход:
* когато говорим за неформатиран вход/изход, имаме предвид, че програмата чете и пише данни във формат, който не може да се интерпретира като текст
* числата най - често ги представяме по същия начин, както са представени в паметта
* системните извиквания read() и write() могат да се използват за неформатиран вход/изход на данни в паметта, без промяна на тяхната структура

    ```c
    void write_number(int fd, uint16_t num) {
        int n = write(fd, &num, sizeof(num));

        if (n < 0) {
            err(1, "could not write the number");
        }
        if (n != sizeof(num)) {
            errx(1, "could not write the number at once");
        }
    }
    ```
    
    ```c
	uint16_t read_number(int fd) {
		uint16_t num;

		int n = read(fd, &num, sizeof(num));

		if (n < 0) {
			err(1, "could not read the number");
		}
		if (n != sizeof(num)) {
			errx(1, "could not read the number at once");
		}

	}
    ```

    ```c
	typedef struct {
		char name[16];
		uint8_t age;
	} person_t;

	void write_person(int fd, person_t* person) {
		int n = write(fd, person, sizeof(person_t));
		if (n < 0) {
			err(1, "could not write the person");
		}
		if (n != sizeof(person)) {
			errx(1, "could not write the person at once");
		}
	}

	void read_person(int fd, person_t* person) {
		int n = read(fd, person, sizeof(person_t));
		if (n < 0) {
			err(1, "could not read the person");
		}
		if (n != sizeof(person)) {
			errx(1, "could not read the person at once");
		}
	}
    ```

## 3. Форматиран вход/изход:
* когато говорим за форматиран вход/изход, имаме предвид, че програмата чете и пише текст, предназначен за четене от хора
* нищо не пречи текстът да е машинно четим
* числата са форматирани като последователност от цифри(текст)
* можем да използваме вградената функция snprintf(3), за да форматираме числа като текст:

    ```c
	void print_number(int fd, uint16_t num) {
		char num_text[6];
		snprintf(num_text, sizeof(num_text), "%d", num);
		write(fd, num_text, strlen(num_text));
	}
    ```


---


# Информация за файлове чрез stat
* системното извикване stat(2) дава достъп до метаданните на файла (командата stat(1) използва това системно извикване)
* първият аргумент е път до файл, а вторият е указател към структура от тип struct stat, която е дефинирана в стандартната библиотека

    ```c
    struct stat{
        dev_t st_dev; // ID of device containing file
        ino_t st_ino; // Inode number
        mode_t st_mode; // File type and mode
        nlink_t st_nlink; // Number of hard links
        uid_t st_uid; // User ID of owner
        gid_t st_gid; // Group ID of owner
        dev_t st_rdev; // Device ID (if special file)
        off_t st_size; // Total size in bytes
        blksize_t st_blksize; // Block size for filesystem I/O
        blkcnt_t st_blocks; // Number of 512B blocks allocated

        struct timespec st_atim; // Time of last access
        struct timespec st_mtim; // Time of last modification
        struct timespec st_ctim; // Time of last status
    }
    ```

* алтернативният вариант fstat() използва файлов дескриптор като първи аргумент вместо път
* може да се използва при вече отворени процеси


---


# Изпълняване на програми с exec
* фамилията от системни извиквания exec(3) се използва, за да изпълним външна програма в текущия процес
	* резлични варианти на извиквания - execl(), execlp(), execvp(), execle(), execve()
* при успешно изпълнение на exec(), програмата на текущия процес се заменя с дадената

    ```c
    int main(void) {
        int result = execl(
            "/usr/bin/cat",       // executable
            "cat", "/etc/issue",  // arguments
            (char*) NULL          // sentinel
            );
    }
    ```

## 1. exec*p - търсене в $PATH
* вариантите exec*p използват environment променлива PATH, за да търсят изпълнимия файл

## 2. execv* - масив от аргументи
* вариантите execv* приемат масив от аргументи


--- 


# Създаване на процеси

* в UNIX света създаването на процеси става чрез системното извикване **fork(2)**
* при извикване на fork(), текущия процес се клонира на родител и дете
    * семантично, цялата памет на процеса се **копира**
    * реално копието се извършва чрез copy-on-write(CoW)
* родителят и детето използват **отделни** региони във физическата памет и **не могат** да достъпват паметта помежду си
* изпълнението на програмата в процеса - дете продължава от мястото, където е извикан **fork()**
* отделните процеси работят конкурентно и не се изчакват

* стойността, върната от fork(), е различна при родителя и детето:
    * в детето, fork() връща 0
    * в родителя, fork() връща цяло число, по - голямо от 0: PID-a на детето

    ```c
    pid_t pid = fork();
    if (pid < 0) {
        err(1, "could not fork");
    }

    if (pid > 0) {
        const char msg[] = "I am your father\n";
        write(1, msg, strlen(msg));
    } else {
        const char msg[] = "Nooooooooooo!\n";
        write(1, msg, strlen(msg));
    }
    ```

    ```c
    pid_t pid = fork();
    if (pid < 0) {
        err(1, "could not fork");
    }

    if (pid > 0) {
        char msg[128] = "I am the parent\n";
        write(1, msg, strlen(msg));

        snprintf(msg, sizeof(msg), "The child's pid is %d\n", pid);
        write(1, msg, strlen(msg));
    } else {
        const char msg[] = "I am the child\n";
        write(1, msg, strlen(msg));
    }

    const char msg[] = "I am both\n";
    write(1, msg, strlen(msg));
    ```

## 1. Pids:
* както видяхме, **PID-ът** на процеса-дете се връща от **fork()**
* **getpid(2)** и **getppid(2)** връщат **PID-а** на текущия процес и на неговия родител

    ```c
    pid_t my_pid = getpid();
    pid_t parent_pid = getppid();

    char msg[128];
    snprintf(
        msg, sizeof(msg),
        "My pid is %d and my parent's pid is %d",
        my_pid, parent_pid
        );
    write(1, msg, strlen(msg));
    ```

## 2. Изчакване с wait():
* системното извикване **wait(2)** блокира, докато някое дете на текущия процес не умре
    * аргументът му е указател, сочещ към променливата, в която wait() ще запише статуса на завършилото дете
    * стойността, върната от wait() е PID-а на детето
* всъщност, статусът, който wait() записва в аргумента си, кодира малко повече информация освен exit-статуса на процеса-дете
* например, можем да разберем дали процесът е бил убит или е завършил нормално
    * макрото **WIFEXITED(status)** проверява дали статусът е такъв на нормално-завършил процес
* можем и да извлечем истинския exit status на процеса
    * макрото **WEXITSTATUS(status)** извлича exit status-а
* за по-подробна информация - wait(2)

    ```c
    pid_t child_pid = fork();
    if (child_pid < 0) {
        err(1, "could not fork");
    }

    if (child_pid == 0) {
        do_task();
        exit(0);
    }

    int status;
    child_pid = wait(&status);
    if (child_pid < 0) {
        err(1, "could not wait for child");
    }
    if (!WIFEXITED(status)) {
        errx(1, "task failed: child was killed!");        
    } else if (WEXITSTATUS(status) != 0) {
        errx(1, "task failed: exit status != 0");
    }

    const char msg[] = "task completed successfully\n";
    write(1, msg, strlen(msg));
    ```

    ```c
    for (int i = 0; i < num_tasks; i++) {
        pid_t child_pid = fork();
        if (child_pid < 0) {
            err(1, "could not fork");
        }
        if (child_pid == 0) {
            do_task(i);
            exit(0); // The child does its work and exits
        }
    }

    for (int i = 0; i < num_tasks; i++) {
        int status;
        pid_t child_pid = wait(&status);
        if (child_pid < 0) {
            err(1, "could not wait for child");
        }
        if (!WIFEXITED(status)) {
            warnx(1, "a task failed: child was killed");
        } else if (WEXITSTATUS(status) != 0) {
            warnx(1, "a task failed: exit status != 0");
        }
    }

    const char msg[] = "all tasks completed successfully";
    write(1, msg, strlen(msg));
    ```

* със системното извикване **waitpid(2)** можем да изчакаме завършването на процес с **конкретно PID**
* има и малко повече възможности от **wait()**
    * може да провери дали процес е завършил, без да блокира
    * може да чака за цяла група процеси
* за информация как да го ползвате - man page

## 3. Изпращане на сигнали с kill():
* изпращането на сигнал до процес става с **kill(2)**
* **int kill(pid_t pid, int sig)**
* имената на сигнали са дефинирани в **<signal.h>**
* можем да изпратим сигнал и до група процеси (man page kill(2))

    ```c
    pid_t child_pid = fork();
    if (child_pid < 0) {
        err(1, "could not fork");
    }
    if (child_pid == 0) {
        sleep(10);
        exit(0);
    }

    int kill_result = kill(child_pid, SIGTERM);
    if (kill_result < 0) {
        err(1, "could not kill child");
    }

    int status;
    child_pid = wait(&status);
    if (child_pid < 0) {
        err(1, "could not wait for child");
    }
    if (!WIFEXITED(status)) {
        errx(1, "task failed: child was killed!");        
    } else if (WEXITSTATUS(status) != 0) {
        errx(1, "task failed: exit status != 0");
    }
    ```

## 4. Обработване на сигнали:
* всеки процес може да избере да бъде уведомен за получаване на сигнал вместо да бъде прекратен
* това става със **sigaction(3p)**
* **в рамките на курса няма да се използва**

## 5. Наследяване на средата при fork():
* процесът-дете наследява цялата среда на родителя си:
    * потребител(EUID, UID)
    * права
    * environment променливи
    * отворени файлове дескриптори

    ```c
    int fd = open(
        "/tmp/test.txt",
        O_WRONLY|O_CREAT|O_TRUNC
        0666
        );
    if (fd < 0) {
        err(1, "could not open file");
    }

    pid_t pid = fork();
    if (pid < 0) {
        err(1, "could not fork");
    }

    for (int i = 0; i < 1000; i++) {
        if (pid == 0) {
            write(fd, "foo\n", 4);
        } else {
            write(fd, "bar\n", 4);
        }
    }

    close(fd);
    ```
* в предния пример получихме файл, в който имаме 1000 реда "foo" и 1000 реда "bar", в произволен ред
* двата процеса имат достъп до един и същ файлов дескриптор
* процесите се създават, кой от тях да запише своя текст и да премести указателя на файловия дескриптор напред


---


# Тръби и водопроводчици

## 1. Pipe:
* системното извикване **pipe(2)** създава тръба
    * тръбата е структура в ядрото, имплементиращо **FIFO опашка**
* взаимодействие с тръбата през два файлови дескриптора:
    * **pipe()** приема като аргумент масив от 2 елемента, в който да запише номера на двата файлови дескриптора
    * дескриптор за четене (индекс 0)
    * дескриптор за писане (индекс 1)

    ```c
    int pfd[2];
    if (pipe(pfd) < 0) {
        err(1, "could not create pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        err(1, "could not fork");
    }
    if (pid == 0) {
        close(pfd[0]);

        write(pfd[1], "foo\n", 4);

        close(pfd[1]);
        exit(0);
    } else {
        close(pfd[1]);
        char buf[20];
        read(pfd[0], buf, 20);
    }
    ```
* тръбите са удобен метод за комуникация между процеси
* при четене от тръба, текущият процес **блокира** докато някой друг не запише данни в тръбата
* когато всички крайща за писане се затворят, краищата за четене получават "край на файл" (EOF) и четенето от тях **не блокира**
* нужно е всеки процес да затваря краищата на тръбата, които не използва
    * в противен случай може да се получи **deadlock**
    * например може процесът, който пише данни, да е приключил, но процесът, който чете данни, да чака блокиран до безкрай, защото не е затворил своя край за писане

## 2. Копиране на файлови дескриптори:
* можем да копираме файлови дескриптори със системните извиквания **dup()** и **dup2()**
* **int dup(int oldfd)** = копира подадения файлов дескриптор с номер **oldfd** на първия свободен номер, и връща новия номер
* **int dup2(int oldfd, int newfd)** = копира подадения файлов дескриптор с номер **oldfd** като нов файлов дескриптор с номер **newfd** и връща **newfd**
    * ако файлов дескриптор с номер **newfd** е съществувал, **dup2()** го затваря преди да направи копието
* тези системни извиквания са много полезни, ако искаме да имплементираме пренасочване на стандартните потоци
