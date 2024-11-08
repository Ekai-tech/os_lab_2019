CC=gcc 
CFLAGS=-I.  # Опции компилятора: -I. указывает на текущую директорию для поиска заголовочных файлов

.PHONY: all clean  # Объявление псевдонимов для целей, которые не соответствуют файлам

# Цель по умолчанию, которая собирает все исполняемые файлы
all: sequential_min_max parallel_min_max launcher 

# Правило для сборки исполняемого файла sequential_min_max
sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h 
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS) 

# Правило для сборки исполняемого файла parallel_min_max
parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h 
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS) 

# Правило для сборки исполняемого файла launcher
launcher : launcher.o utils.o find_min_max.o utils.h find_min_max.h 
	$(CC) -o launcher launcher.o utils.o find_min_max.o $(CFLAGS) 

# Правило для компиляции объекта utils.o из utils.c
utils.o : utils.h 
	$(CC) -o utils.o -c utils.c $(CFLAGS) 

# Правило для компиляции объекта find_min_max.o из find_min_max.c
find_min_max.o : utils.h find_min_max.h 
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS) 

# Правило для компиляции объекта launcher.o из launcher.c
launcher.o : launcher.c 
	$(CC) -o launcher.o -c launcher.c $(CFLAGS) 

# Цель для очистки скомпилированных файлов и исполняемых программ
clean : 
	rm -f utils.o find_min_max.o sequential_min_max parallel_min_max launcher launcher.o