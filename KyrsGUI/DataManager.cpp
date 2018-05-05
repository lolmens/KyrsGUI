#include "DataManager.h"

enum variableTypes {
	___INT = 0,
	___STRING = 1
};
struct meta {// Хранит Мета(о) информацию о "динамической переменной"
	char *name;// Имя переменной, например "Цена"
	byte key;//Ключ для поиска переменной внутри объекта 3-го lvl
	variableTypes variableType;// Тип Данной переменной
	byte idOnType;// Место в массиве переменных данного типа
	boolean vizible = false;// У объекта 3-го lvl есть 1 поле которое показывает краткую информацию о нём, этот флаг указывает на принадлежность этому полю.
};

struct lvl1 { //lvl 1
			  //Первый уровень объектов, хранит категории оъбектов (Например, Техника, Мебель, Предметы утвари, Животные.)
	char *name; //Имя категории, например "Мебель"
	int idType; //Собственный ID, по которому во время загрузки его находят объекты 2-го и 3-го уровня.
	bool haveLvl2; //Если false зачит ссылка на lvl3 иначе lvl2
	int size = 0;//Кол-во подэлементов.
	union {
		struct lvl2 *dataLvl2; //Ссылка на lvl2 и lvl3 одна  Массивы хранят сами данные в структурированном виде.
		struct lvl3 *dataLvl3; //Один из данных масивов = NULL;
	};

};

struct lvl2 { //lvl 2
			  //Второй уровень объектов, хранит подкатегории оъбектов (Например, если категория техника, то подкатегориями могут быть кухонная техника, компьютерная техника, портативные средства.)
	char *name; //Имя подкатегории, например "компьютерная техника"
	int idTypeParrent;//ID родителя (1 уровня) 
	int idType; //Собственный ID, по которому во время загрузки его находят объекты 3-го уровня. 
	int size = 0;//Кол-во подэлементов.
	struct lvl3 *data;
};

struct lvl3 {  //lvl 3
			   //Третий уровень объектов, хранит непосредственно объекты (PC, тумба, стул)
	char *name; //Имя объекта, например "Чашка Бугалтера"
	int idTypeParrent; //ID родителя (2 или 1 уровня)
	byte size = 0;
	struct meta *metadata; //Хранит данные объекта (тип, имя переменной, место в массиве со значениями).
	int *IntegerMass; //Физически хранит числовые переменные, НА ПРЯМУЮ НЕ ОБРАЩАТЬСЯ!
	char **StringMass; //Физически хранит текстовые переменные, НА ПРЯМУЮ НЕ ОБРАЩАТЬСЯ!
};
lvl1 *rootLvl = NULL;
int countlvl1 = 0, countlvl2 = 0, countlvl3 = 0;

void load(char* lvlFile) {
	rootLvl = load_lvl(lvlFile, 1000, rootLvl, countlvl1, countlvl2, countlvl3);
}



lvl1* load_lvl(char* lvlFile, int maxStringLen, lvl1 *root, int &countlvl1, int &countlvl2, int &countlvl3) {
	/*
	*Тут потребуется краткое описание дальнейших действий программы
	*Во первых Нужно отделить полезные для программы данные от мусора на подобии комментрариев, лишних переносов строк, отступов
	*
	*/
	FILE *file_lvls;
	if ((file_lvls = fopen(lvlFile, "r")) == NULL)
	{
		printf("\nFile %s not open!", lvlFile);
		system("pause"); exit(1);
	}
	int stringsCount = 0;//Переменная для подсчёта кол-ва строк с данными, далее требуется для выделении памяти.
	char *readString = (char *)malloc(max  9StringLen * sizeof(char));//Память под считанную строку из файла
	while (fgets(readString, maxStringLen, file_lvls) != NULL) {
		//if (DEBAG) printf("\nDEBAG function load_lvl String \"%s\" len:%d", readString, strlen(readString));
		switch (readString[0])
		{
		case '#': continue;//Пропуск комментариев
		case '-'://Выбираем что-то полезное 
			stringsCount++;
			continue;
		}
	}
	rewind(file_lvls);//В начало файла, подсчёт кол-ва полезных данных закончен.

					  //if (DEBAG) printf("\nDEBAG function load_lvl Total strings:%d\n", stringsCount);

	char **stringsWithData = (char**)malloc(stringsCount * sizeof(char*));//Массив указателей на строки с уже подготовленными данными.

	stringsCount = 0;
	char *tempString = (char *)malloc(maxStringLen * sizeof(char));//Место под динамическую подгрузку частей строк.
	char *copyTempString = tempString;
	while (fgets(readString, maxStringLen, file_lvls) != NULL) {
		//if (DEBAG) printf("\nDEBAG function load_lvl Read String \"%s\" len:%d", readString, strlen(readString));
		switch (readString[0])
		{
		case '#': continue;//Пропуск комментариев
		case '-':
			while (!haveEnd(readString)) {//Если в строке нет ";" значит в этой строке структура не закончилась и требуется дополнить строку.
										  //if (DEBAG) printf("\nDEBAG function load_lvl String \"%s\" len:%d", readString, strlen(readString));
				if (fgets(tempString, maxStringLen, file_lvls) != NULL) {
					strcat(readString, tempString); //Склеиваем начало строки с только что подгруженной частью.
				}
				else {
					//FIXME: Короче, в этом случае файл скорее всего неожиданно кончился.
				}
			}
			break;
		default:
			continue;
		}
		//if (DEBAG) printf("\nDEBAG function load_lvl Full String \"%s\" len:%d", readString, strlen(readString));
		stringsWithData[stringsCount] = (char*)malloc(strlen(readString) * sizeof(char)); //Выделение достаточного кол-ва памяти под "чистую строку"
		strcpy(stringsWithData[stringsCount], readString);//Сохранение полученных даных.
														  //if (DEBAG) printf("\nDEBAG function load_lvl Full String \"%s\" len:%d ", stringsWithData[stringsCount], strlen(stringsWithData[stringsCount]));
		stringsCount++;
		//printf("%d", stringsCount);
	}
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 1:
			countlvl1++;
			break;
		case 2:
			countlvl2++;
			break;
		case 3:
			countlvl3++;
			break;
		}
	}
	//if (DEBAG) printf("DEBAG function loadlvl.\n Lvl 1 count cells: %d\n Lvl 2 count cells: %d\n Lvl 3 count cells: %d\n", countlvl1, countlvl2, countlvl3);
	root = (struct lvl1*)malloc(countlvl1 * sizeof(struct lvl1));
	for (int i = 0; i < countlvl1; i++) {
		root[i].size = 0;
		root[i].haveLvl2 = false;
	}
	countlvl1 = 0; //lv2 = 0, lv3 = 0;
				   //Заполняем 1 уровень и считаем кол-во элементов на 2-м уровне
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 1:
			loadlvl1(&stringsWithData[i][6], root[countlvl1]);
			//if (DEBAG) printf("DEBAG function loadlvl Id:%d Name:%s\n", root[lv1].idType, root[lv1].name);
			countlvl1++;
			break;
		case 2: //Подсчет количесва элементов lvl 2 в каждом lvl 1
			lvl2 temp;
			loadlvl2(&stringsWithData[i][6], temp);
			//if (DEBAG) printf("DEBAG function loadlvl IdType:%d IdTypeParrent:%d Name:%s\n", temp.idType, temp.idTypeParrent, temp.name);
			for (int i = 0; i < countlvl1; i++) {
				if (root[i].idType == temp.idTypeParrent) {
					//printf("DEBAG function loadlvl Id:%d Name:%s Size:%d\n", root[i].idType, root[i].name, root[i].size);
					root[i].haveLvl2 = true;
					root[i].size++;
					break;
				}
			}
			break;
		}
	}

	//Выделяем память под структуры 2-го lvl
	for (int i = 0; i < countlvl1; i++) {
		if (root[i].haveLvl2 == true) {
			root[i].dataLvl2 = (struct lvl2*)malloc(root[i].size * sizeof(struct lvl2));
			root[i].size = 0; //Будет использоваться как счётчик
		}
	}

	//Заполняем 2 уровень 
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 2:
			lvl2 temp;
			loadlvl2(&stringsWithData[i][6], temp);
			//if (DEBAG) printf("DEBAG function loadlvl IdType:%d IdTypeParrent:%d Name:%s\n", temp.idType, temp.idTypeParrent, temp.name);
			for (int j = 0; j < countlvl1; j++) {
				if (root[j].idType == temp.idTypeParrent) {
					loadlvl2(&stringsWithData[i][6], root[j].dataLvl2[root[j].size]); //root[j].size - Используется в качестве счётчика.
					root[j].dataLvl2[root[j].size].size = 0;
					root[j].size++;
					break;
				}
			}
			break;
		}
	}

	//При заполнении 3-го уровня будут так-же использоваться размеры как счётчики для элементов, предварительно требуется из очистить от мусора и прировнять к 0.
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString)) {
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int i = 0; i < countlvl1; i++) {//Проход по 1-му уровню.
				if (root[i].haveLvl2 == true) {
					for (int j = 0; j < root[i].size; j++) {//Проход по 2-му уровню если таковой есть.
						if (root[i].dataLvl2[j].idType == temp.idTypeParrent) {
							root[i].dataLvl2[j].size = 0;
							goto contini1;
						}
					}
				}
				else //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[i].idType == temp.idTypeParrent) {
						root[i].size = 0;
						goto contini1;
					}
			}
		}
	contini1:
		;
	}

	//Считаем кол-во элементов на 3-м уровне
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString)) {
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//Проход по 1-му уровню.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//Проход по 2-му уровню если таковой есть.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].size++;
							goto contini2;
						}
				}
				else { //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[h].idType == temp.idTypeParrent) {
						root[h].size++;
						goto contini2;
					}

				}

			}
		}
	contini2:
		;
	}

	//printf("Перед выделением памяти\n");
	if (false)	for (int i = 0; i < countlvl1; i++) {//Количество элементов на первом уровне 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//Количество элементов на 2-х уровнях
			if (root[i].haveLvl2) {
				printf("	|-lvl2- IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++)//Количество элементов на 3-х уровнях
					;// printf("		|-lvl3- IdTypeParrent:%d Name:%s\n", root[i].dataLvl3[h].idTypeParrent, root[i].dataLvl3[h].name);
					 //root[i].dataLvl2[j]
			}
			else
				;//printf("	|-lvl3- IdTypeParrent:%d Name:%s\n", (*root[i].dataLvl3).idTypeParrent, (*root[i].dataLvl3).name);
		}
	}

	//Выделяем память под элементы 3-го уровня
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//Проход по 1-му уровню.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//Проход по 2-му уровню если таковой есть.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].data = (struct lvl3*)malloc(root[h].dataLvl2[j].size * sizeof(lvl3));
							goto contini3;
						}
				}
				else //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[h].idType == temp.idTypeParrent) {
						root[h].dataLvl3 = (struct lvl3*)malloc(root[h].size * sizeof(lvl3));
						goto contini3;
					}
			}
		}
	contini3:
		;
	}

	//Скидываем размеры массивов 3-го уровня чтобы использовать их как счётчики.
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//Проход по 1-му уровню.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//Проход по 2-му уровню если таковой есть.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].size = 0;
							goto contini4;
						}
				}
				else //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[h].idType == temp.idTypeParrent) {
						root[h].size = 0;
						goto contini4;
					}
			}
		}
	contini4:
		;
	}
	//printf("Перед заполнением\n");
	if (false)	for (int i = 0; i < countlvl1; i++) {//Количество элементов на первом уровне 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//Количество элементов на 2-х уровнях
			if (root[i].haveLvl2) {
				printf("	|-lvl2- IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++)//Количество элементов на 3-х уровнях
					;// printf("		|-lvl3- IdTypeParrent:%d Name:%s\n", root[i].dataLvl3[h].idTypeParrent, root[i].dataLvl3[h].name);
					 //root[i].dataLvl2[j]
			}
			else
				;// printf("	|-lvl3- IdTypeParrent:%d Name:%s\n", (*root[i].dataLvl3).idTypeParrent, (*root[i].dataLvl3).name);
		}
	}

	//Заполняем элементами 3-го уровня
	for (int i = 0; i < stringsCount; i++) {//считываем 5-й символ, символ отвечающий за номер уровня
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//Проход по 1-му уровню.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//Проход по 2-му уровню если таковой есть.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							loadlvl3(&stringsWithData[i][6], root[h].dataLvl2[j].data[root[h].dataLvl2[j].size], true);//Осторожно, можно сломать мозг..Или запутаться в i,h & j //FIXME: Тут с памятью точно какой-то косяк!!
																													   //printf("Debag:temp  |-lvl3- IdTypeParrent:%d Name:%s\n", temp.idTypeParrent, temp.name);
							root[h].dataLvl2[j].size++;
							goto contini5;
						}

				}
				else //Проход по 3-му уровню если нет прослойки в виде 2-го.
					if (root[h].idType == temp.idTypeParrent) { //FIXME: Тут с памятью точно какой-то косяк!!
						loadlvl3(&stringsWithData[i][6], root[h].dataLvl3[root[h].size], true);
						//printf("Debag:root  |-lvl3- root[h].size:%d IdTypeParrent:%d Name:%s\n", root[h].size, root[h].dataLvl3[root[h].size].idTypeParrent, root[h].dataLvl3[root[h].size].name);
						root[h].size++;
						goto contini5;
					}
			}
		}
	contini5:
		;
	}
	return root;
	fclose(file_lvls);
	//free(tempString);//Память надо не забывать освобождать, иначе утечка..! Или не надо..
}

boolean haveEnd(char *string) {//Убирает лишние переносы строк и Tab (все что попадутся).
	int i = 0;
	int len = strlen(string);//Убираем тормаза
	for (i = 0; i < len; i++)
		switch (string[i])
		{
		case ';'://Если ; значит, в этой строке закрылась структура.
			if (i + 1 < len&&string[i + 1] == '\n')
				string[i] = '\0';
			return true;
		case '	'://Замена Tab на Пробел
			string[i] = ' ';
		case '\n'://Замена Enter на Пробел
			string[i] = ' ';
		}
	return false;
}
void loadlvl1(char* str, lvl1 &lvl) {//FIXME:Переписать под поиск * а не статическое полежение
									 //str =  *idType*name* 
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//Нахождение первого символа * и удаление его
	*(strchr(tempString, c)) = '\0';//Нахождение второго символа * и постановка конца строки туда.
	lvl.idType = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше (А ты можешь выйти за рамки??)
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	lvl.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(lvl.name, tempString);
	//free(tempString);//Хз почему неработает
	//if (DEBAG) printf("DEBAG function loadlvl1 Id:%d Name:%s\n", lvl.idType, lvl.name);
}

void loadlvl2(char* str, lvl2 &lvl) {
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//Нахождение первого символа * и удаление его
	*(strchr(tempString, c)) = '\0';//Нахождение второго символа * и постановка конца строки туда.
	lvl.idType = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше (А ты можешь выйти за рамки??)
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	lvl.idTypeParrent = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	lvl.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(lvl.name, tempString);
	//if (DEBAG) printf("DEBAG function loadlvl2 IdType:%d IdTypeParrent:%d Name:%s\n", lvl.idType, lvl.idTypeParrent, lvl.name);
}


void loadlvl3(char* str, lvl3 &lvl, bool needMeta) {
	//printf("У нас %s проблемы.\n", (_CrtCheckMemory()? "присутствуют" : "отсутствуют"));
	//if (DEBAG) printf("DBAG function loadlvl3 String:%s\n", str);
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	char *tempMetaString = (char*)malloc(strlen(str) * sizeof(char));
	//printf("sizeof tempString %d\n", strlen(tempString));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//Нахождение первого символа * и удаление его
	*(strchr(tempString, c)) = '\0';//Нахождение второго символа * и постановка конца строки туда.
	lvl.idTypeParrent = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
	lvl.name = (char *)malloc((strlen(tempString) + 1) * sizeof(char));
	strcpy(lvl.name, tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше чтобы была возможность искать дальше 
														//if (DEBAG) printf("DEBAG function loadlvl3 String:%s\n", tempString);
	tempString = strchr(tempString, '{'), tempString++;//Нахождение первого символа { и удаление его
	*(strchr(tempString, '}')) = '\0';//Нахождение второго символа } и постановка конца строки туда.
									  //if (DEBAG) printf("DEBAG function loadlvl3 IdTypeParrent:%d Name:%s meta:%s\n", lvl.idTypeParrent, lvl.name, tempString);
									  //printf("У нас %s проблемы.\n", (_CrtCheckMemory() ? "присутствуют" : "отсутствуют"));
	if (!needMeta)
		return;
	char* metString = 0;
	int countIntMeta = 0, countStringMeta = 0, metInt = 0;
	meta met;
	met.idOnType = 0, met.name = "", met.variableType = variableTypes::___INT, met.vizible = false;
	char *copyTempMetaString = tempMetaString;//Делаем копию ссылки на начало строки
	strcpy(tempMetaString, tempString);
	while (true) {
		bool needBreak = false;
		if (strchr(tempMetaString, ',') == NULL)//Если true значит мы нашли последнее значение
			needBreak = true;
		else
			*(strchr(tempMetaString, ',')) = '\0';//Нахождение символа ',' и постановка конца строки туда.
		tempMetaString = strchr(tempMetaString, c);//Нахождение первого символа * и перемещение туда
		loadlvlMeta(tempMetaString, met, &metString, metInt);
		if (met.variableType == variableTypes::___INT)
			countIntMeta++;
		else
			countStringMeta++;
		tempMetaString = strchr(tempMetaString, '\0'), tempMetaString++; //Нахождение конца и перемецение на один символ дальше
		if (needBreak)
			break;
	}
	//printf("countIntMeta: %d countStringMeta: %d\n", countIntMeta, countStringMeta);
	lvl.metadata = (meta*)malloc((countIntMeta + countStringMeta) * sizeof(meta));
	lvl.IntegerMass = (int*)malloc(countIntMeta * sizeof(int));
	lvl.StringMass = (char**)malloc(countStringMeta * sizeof(char*));
	lvl.size = countIntMeta + countStringMeta;
	countIntMeta = 0, countStringMeta = 0;
	tempMetaString = copyTempMetaString;//Возвращаем указатель на начато выделенного куска памяти и уже туда копируем что-либо
	strcpy(tempMetaString, tempString);

	while (true) {
		bool needBreak = false;
		if (strchr(tempMetaString, ',') == NULL)//Если true значит мы нашли последнее значение
			needBreak = true;
		else
			*(strchr(tempMetaString, ',')) = '\0';//Нахождение символа ',' и постановка конца строки туда.
		tempMetaString = strchr(tempMetaString, c);//Нахождение первого символа * и перемещение туда
		loadlvlMeta(tempMetaString, lvl.metadata[(countIntMeta + countStringMeta)], &metString, metInt);
		if (!(countIntMeta + countStringMeta))
			lvl.metadata[(countIntMeta + countStringMeta)].vizible = true;
		else
			lvl.metadata[(countIntMeta + countStringMeta)].vizible = false;
		if (lvl.metadata[(countIntMeta + countStringMeta)].variableType == variableTypes::___INT) {
			lvl.IntegerMass[countIntMeta] = metInt;
			lvl.metadata[(countIntMeta + countStringMeta)].idOnType = countIntMeta;
			//printf("Lvl:%20s Metdata: idOnType =%2d, variableType = INT, vizible =%2d, met.name = %30s, metInt = %3d\n", lvl.name, lvl.metadata[(countIntMeta + countStringMeta)].idOnType, lvl.metadata[(countIntMeta + countStringMeta)].vizible, lvl.metadata[(countIntMeta + countStringMeta)].name, metInt);
			countIntMeta++;
		}
		else {
			lvl.StringMass[countStringMeta] = (char*)malloc(strlen(metString) * sizeof(char));
			strcpy(lvl.StringMass[countStringMeta], metString);
			lvl.metadata[(countIntMeta + countStringMeta)].idOnType = countStringMeta;
			//printf("Lvl:%20s Metdata: idOnType =%2d, variableType = STR, vizible =%2d, met.name = %30s, metString = %s\n", lvl.name, lvl.metadata[(countIntMeta + countStringMeta)].idOnType, lvl.metadata[(countIntMeta + countStringMeta)].vizible, lvl.metadata[(countIntMeta + countStringMeta)].name, metString);
			countStringMeta++;
		}
		tempMetaString = strchr(tempMetaString, '\0'), tempMetaString++; //Нахождение конца и перемецение на один символ дальше
																		 //printf("Lvl:%20s Metdata: idOnType =%2d, key=%2d, variableType =%2d, vizible =%2d, met.name = %30s, metInt = %3d, metString = %s\n", lvl.name, lvl.metadata[(countIntMeta + countStringMeta)-1].idOnType, lvl.metadata[(countIntMeta + countStringMeta) - 1].key, lvl.metadata[(countIntMeta + countStringMeta)-1].variableType, lvl.metadata[(countIntMeta + countStringMeta)-1].vizible, lvl.metadata[(countIntMeta + countStringMeta) - 1].name, metInt, metString);
		if (needBreak)
			break;
	}
	//printf("У нас %s проблемы с повреждением памяти.\n", (_CrtCheckMemory() ? "присутствуют" : "отсутствуют"));
	//printf("metdata: met.idOnType = %d, met.idOnType = %d, met.name = %s, met.variableType = %d, met.vizible = %d, metInt = %d, metString = %s\n", met.idOnType, met.idOnType, met.name, met.variableType, met.vizible, metInt, metString);
}

void loadlvlMeta(char* str, meta &met, char **text, int &integer) {
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	//printf("//*key*name*:i:123 OR *key*name*:S:*Text* \n %s \n", tempString);
	tempString = strchr(tempString, c), tempString++;//Нахождение первого символа * и удаление его
													 //key*name*:i:123 OR key*name*:S:*Text*
													 //printf("//key*name*:i:123 OR key*name*:S:*Text* \n %s \n", tempString);
	*(strchr(tempString, c)) = '\0';//Нахождение второго символа * и постановка конца строки туда.
									//key'\0'name*:i:123 OR key'\0'name*:S:*Text*
	met.key = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше
														//name*:i:123 OR name*:S:*Text*
	*(strchr(tempString, c)) = '\0';//Нахождение символа * и постановка конца строки туда.
									//name'\0':i:123 OR name'\0':S:*Text*
	met.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(met.name, tempString);
	tempString = strchr(tempString, '\0'), tempString++;//Нахождение конца и перемецение на один символ дальше
														//:i:123 OR :S:*Text*
	*(strchr(tempString, ':')) = '\0';
	//'\0'i:123 OR '\0'S:*Text*
	tempString = strchr(tempString, '\0'), tempString++;
	//i:123 OR S:*Text*
	*(strchr(tempString, ':')) = '\0';
	//i'\0'123 OR S'\0'*Text*
	char type = tempString[0];
	tempString = strchr(tempString, '\0'), tempString++;
	//123 OR *Text*
	switch (type)
	{
	case 'i'://integer
		met.variableType = variableTypes::___INT;
		integer = atoi(tempString);
		break;
	case 'S'://Stiring
			 //*Text*
		met.variableType = variableTypes::___STRING;
		tempString = strchr(tempString, c), tempString++;
		//Text*
		//printf("%s, strlen: %d \n", tempString, strlen(tempString));
		*(strchr(tempString, c)) = '\0';
		//Text
		*text = ((char*)malloc(strlen(tempString) * sizeof(char)));
		strcpy(*text, tempString);
		break;
	}
	//if (DEBAG) printf("DEBAG function loadlvlMeta key:%d Name:%s Type:%s, data string:%s data integer:%d \n", met.key, met.name, (met.variableType == variableTypes::___INT ? "INT" : "STRING"), text, integer);
}

void print(lvl1 *root, int countlvl1) {
	printf("Финальный вывод\n");
	for (int i = 0; i < countlvl1; i++) {//Количество элементов на первом уровне 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//Количество элементов на 2-х уровнях
			if (root[i].haveLvl2) {
				printf("	|-lvl2- J:%d IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", j, root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++) {//Количество элементов на 3-х уровнях
					printf("		|-lvl3- H:%d IdTypeParrent:%d, Size:%d, Name:%s\n", h, root[i].dataLvl2[j].data[h].idTypeParrent, root[i].dataLvl2[j].data[h].size, root[i].dataLvl2[j].data[h].name);
					for (int r = 0; r < root[i].dataLvl2[j].data[h].size; r++) {
						if (/*root[i].dataLvl2[j].data[h].metadata[r].vizible */true) {
							if (root[i].dataLvl2[j].data[h].metadata[r].variableType == variableTypes::___INT)
								printf("			|-%s:%d\n", root[i].dataLvl2[j].data[h].metadata[r].name, root[i].dataLvl2[j].data[h].IntegerMass[root[i].dataLvl2[j].data[h].metadata[r].idOnType]);
							else
								printf("			|-%s - %s\n", root[i].dataLvl2[j].data[h].metadata[r].name, root[i].dataLvl2[j].data[h].StringMass[root[i].dataLvl2[j].data[h].metadata[r].idOnType]);
							//break;
						}
						else
							continue;
					}
				}

			}
			else {
				printf("		|-lvl3- J:%d IdTypeParrent:%d, Size:%d, Name:%s\n", j, root[i].dataLvl3[j].idTypeParrent, root[i].dataLvl3[j].size, root[i].dataLvl3[j].name);
				for (int r = 0; r < root[i].dataLvl3[j].size; r++) {
					if (/*root[i].dataLvl3[j].metadata[r].vizible*/true) {
						if (root[i].dataLvl3[j].metadata[r].variableType == variableTypes::___INT)
							printf("			|-%s:%d\n", root[i].dataLvl3[j].metadata[r].name, root[i].dataLvl3[j].IntegerMass[root[i].dataLvl3[j].metadata[r].idOnType]);
						else
							printf("			|-%s - %s\n", root[i].dataLvl3[j].metadata[r].name, root[i].dataLvl3[j].StringMass[root[i].dataLvl3[j].metadata[r].idOnType]);
						//break;
					}
					else
						continue;
				}
			}

		}
	}
}