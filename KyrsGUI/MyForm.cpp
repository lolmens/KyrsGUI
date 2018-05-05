#include "MyForm.h"
#include "headers.h"
using namespace System;
using namespace System::Windows::Forms;
using namespace Kyrs;

const char lvlFile[24] = "ConfigAndData/lvls.list"; //���� ��������
const int MAXSTRIGLEN = 300; //������������ ����� ������ ����

boolean DEBAG = true;

enum Lang {
	RU = 0,
	EN = 1
} lang;

enum variableTypes {
	___INT = 0,
	___STRING = 1
};

struct meta {// ������ ����(�) ���������� � "������������ ����������"
	char *name;// ��� ����������, �������� "����"
	byte key;//���� ��� ������ ���������� ������ ������� 3-�� lvl
	variableTypes variableType;// ��� ������ ����������
	byte idOnType;// ����� � ������� ���������� ������� ����
	boolean vizible = false;// � ������� 3-�� lvl ���� 1 ���� ������� ���������� ������� ���������� � ��, ���� ���� ��������� �� �������������� ����� ����.
};

struct lvl1 { //lvl 1
			  //������ ������� ��������, ������ ��������� �������� (��������, �������, ������, �������� ������, ��������.)
	char *name; //��� ���������, �������� "������"
	int idType; //����������� ID, �� �������� �� ����� �������� ��� ������� ������� 2-�� � 3-�� ������.
	bool haveLvl2; //���� false ����� ������ �� lvl3 ����� lvl2
	int size = 0;//���-�� ������������.
	union {
		struct lvl2 *dataLvl2; //������ �� lvl2 � lvl3 ����  ������� ������ ���� ������ � ����������������� ����.
		struct lvl3 *dataLvl3; //���� �� ������ ������� = NULL;
	};

} *root;

struct lvl2 { //lvl 2
			  //������ ������� ��������, ������ ������������ �������� (��������, ���� ��������� �������, �� �������������� ����� ���� �������� �������, ������������ �������, ����������� ��������.)
	char *name; //��� ������������, �������� "������������ �������"
	int idTypeParrent;//ID �������� (1 ������) 
	int idType; //����������� ID, �� �������� �� ����� �������� ��� ������� ������� 3-�� ������. 
	int size = 0;//���-�� ������������.
	struct lvl3 *data;
};

struct lvl3 {  //lvl 3
			   //������ ������� ��������, ������ ��������������� ������� (PC, �����, ����)
	char *name; //��� �������, �������� "����� ���������"
	int idTypeParrent; //ID �������� (2 ��� 1 ������)
	byte size = 0;
	struct meta *metadata; //������ ������ ������� (���, ��� ����������, ����� � ������� �� ����������).
	int *IntegerMass; //��������� ������ �������� ����������, �� ������ �� ����������!
	char **StringMass; //��������� ������ ��������� ����������, �� ������ �� ����������!
};

lvl1* load_lvl(int maxStringLen, lvl1 *root, int &countlvl1, int &countlvl2, int &countlvl3);
int countlvl1 = 0, countlvl2 = 0, countlvl3 = 0;
[STAThreadAttribute]
void MyForm::start() {
	printf("Loadig...\n");
	setlocale(LC_CTYPE, "Russian");
	printf("Load lvls.list...\n");
	root = NULL;
	root = load_lvl(1000, root, countlvl1, countlvl2, countlvl3);
	printf("� ��� %s �������� � ������������ ������.\n", (_CrtCheckMemory() ? "������������" : "�����������"));
	printf("Load!\n");
}
void MyForm::creatTree(System::Windows::Forms::TreeView^ tree, bool full) {

	char buff[MAXSTRIGLEN];
	snprintf(buff, sizeof(buff), "%s%s", "	|- Flag full:", full ? "true" : "false");
	std::string buffAsStdStr = buff;
	printf("Creat Tree\n%s\n", buff);


	for (int i = 0; i < countlvl1; i++) {//���������� ��������� �� ������ ������ 
		TreeNode^ rootNode = gcnew TreeNode(gcnew String(root[i].name));
		tree->Nodes->Add(rootNode);
		for (int j = 0; j < root[i].size; j++) {//���������� ��������� �� 2-� �������
			if (root[i].haveLvl2) {
				//printf("	|-**- %s \n", root[i].dataLvl2[j].name);
				TreeNode^ lvl2Node = gcnew TreeNode(gcnew String(root[i].dataLvl2[j].name));
				rootNode->Nodes->Add(lvl2Node);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++) {//���������� ��������� �� 3-� �������
					//printf("		|-*- %s\n", root[i].dataLvl2[j].data[h].name);
					TreeNode^ lvl3Node = gcnew TreeNode(gcnew String(root[i].dataLvl2[j].data[h].name));
					lvl2Node->Nodes->Add(lvl3Node);
					for (int r = 0; r < root[i].dataLvl2[j].data[h].size; r++) {
						if (root[i].dataLvl2[j].data[h].metadata[r].vizible || full) {
							if (root[i].dataLvl2[j].data[h].metadata[r].variableType == variableTypes::___INT)
								snprintf(buff, sizeof(buff), "%s:%3d \n", root[i].dataLvl2[j].data[h].metadata[r].name, root[i].dataLvl2[j].data[h].IntegerMass[root[i].dataLvl2[j].data[h].metadata[r].idOnType]);
							else
								snprintf(buff, sizeof(buff), "%s - %s \n", root[i].dataLvl2[j].data[h].metadata[r].name, root[i].dataLvl2[j].data[h].StringMass[root[i].dataLvl2[j].data[h].metadata[r].idOnType]);
							TreeNode^ metaNode = gcnew TreeNode(gcnew String(buff));
							lvl3Node->Nodes->Add(metaNode);
						}
					}
				}

			}
			else {
				//printf("		|-*- %s \n", root[i].dataLvl3[j].name);
				TreeNode^ lvl3Node = gcnew TreeNode(gcnew String(root[i].dataLvl3[j].name));
				rootNode->Nodes->Add(lvl3Node);
				for (int r = 0; r < root[i].dataLvl3[j].size; r++) {
					if (root[i].dataLvl3[j].metadata[r].vizible || full) {
						if (root[i].dataLvl3[j].metadata[r].variableType == variableTypes::___INT)
							snprintf(buff, sizeof(buff), "%s:%3d \n", root[i].dataLvl3[j].metadata[r].name, root[i].dataLvl3[j].IntegerMass[root[i].dataLvl3[j].metadata[r].idOnType]);
						else
							snprintf(buff, sizeof(buff), "%s - %s \n", root[i].dataLvl3[j].metadata[r].name, root[i].dataLvl3[j].StringMass[root[i].dataLvl3[j].metadata[r].idOnType]);
						TreeNode^ metaNode = gcnew TreeNode(gcnew String(buff));
						lvl3Node->Nodes->Add(metaNode);
					}
				}
			}

		}
	}
	printf("Tree created.\n");
}
struct sp {
	char fio[MAXSTRIGLEN];
	struct sp* sled;
	struct sp* pred;
};

void insert(sp**, char* fio);
void vstavka(sp **spi, char* fio);
void  MyForm::creatNameList(System::Windows::Forms::RichTextBox^ textBox, bool back) {
	printf("creatNameList\n");
	textBox->ReadOnly = true;
	//������ ���� - ���������� ���-�� ���������� � ������ ���-�����
	//������ ���� - ��������������� ��������� ������������ ������ ��� �����
	//������ ���� - ���������� ������ �������
	//�������� ���� - ����� ��������������� ������ � ������ ��� �������� �������

	// step one
	int count = 0;
	// structure
	// lvl 1 | -lvl_1*idType*name*;
	//       |
	//       | -lvl_1*15*��������*;
	//       |
	// lvl 2 |	-|  -lvl_2*idType*idTypeParrent*name*;
	//       |
	//       |  -|  -lvl_2*16*15*����������*;
	//       |  -|  -lvl_2*17*15*IT - ����� * ;
	//       |  -|  -lvl_2*18*15*����� ����������*;
	//       |  -|  -lvl_2*19*15*������������ � ��*;
	//       |
	// lvl 3 |		-|  -lvl_3*idTypeParrent*name*{*key*name*:i:123, *key*name*:S:*Text* ...}; #i - integer, S - string, key - byte (���� key=0, ����������� �������������) !! �������� ��� ��� �� ����������� !! 
	//       |
	//       |		-|  -lvl_3*16*��������*{*0*��������*:S:*������� ���� ����������*, *9*���*:S:*�*};
	//       |		-|  -lvl_3*17*������� �����������*{*0*��������*:S:*������� ������ ���������*, *9*���*:S:*�*};
	//       |		-|  -lvl_3*18*�������� �����*{*0*��������*:S:*��������� ��� ��������*, *9*���*:S:*�*};
	//       |		-|  ...
	for (int i = 0; i < countlvl1; i++)
		if (root[i].idType == 15)//��� ��������� ������ � structure
			if (root[i].haveLvl2 == true)//��� ������������
				for (int j = 0; j < root[i].size; j++)
					if (root[i].dataLvl2[j].idTypeParrent == 15 && root[i].dataLvl2[j].idType >= 16 && root[i].dataLvl2[j].idType <= 19)
						for (int f = 0; f < root[i].dataLvl2[j].size; f++)
							if (root[i].dataLvl2[j].data[f].idTypeParrent >= 16 && root[i].dataLvl2[j].data[f].idTypeParrent <= 19)
								count++;
	printf("	|- Count:%d\n", count);

	//step two
	sp *spisok = NULL;

	//step free
	int h;
	for (int i = 0; i < countlvl1; i++) {
		if (root[i].idType == 15)//��� ��������� ������ � structure
			if (root[i].haveLvl2 == true)//��� ������������
				for (int j = 0; j < root[i].size; j++)
					if (root[i].dataLvl2[j].idTypeParrent == 15 & root[i].dataLvl2[j].idType >= 16 & root[i].dataLvl2[j].idType <= 19)
						for (int f = 0; f < root[i].dataLvl2[j].size; f++)
							if (root[i].dataLvl2[j].data[f].idTypeParrent >= 16 & root[i].dataLvl2[j].data[f].idTypeParrent <= 19)
								for (int g = 0; g < root[i].dataLvl2[j].data[f].size; g++)
									if (root[i].dataLvl2[j].data[f].metadata[g].key == 0) {
										insert(&spisok, root[i].dataLvl2[j].data[f].StringMass[(root[i].dataLvl2[j].data[f].metadata[g].idOnType)]);
										h++;
									}
	}

	for (sp* temp = spisok; temp->pred != 0; temp = temp->pred, spisok = temp); //����������� ��������� �� ������ (�������)

	for (sp* temp = spisok; temp != 0; temp = temp->sled, h++)
		printf("	|- %s\n", temp->fio);
	//printf("	| - %s \n	 \\\n	 |- %s \n	 /\n	|- %s\n\n",(temp->pred != NULL ? temp->pred->fio :  "NULL"),  temp->fio, (temp->sled != NULL ? temp->sled->fio : "NULL"));
	printf("	|- Count:%d\n", h);

	//step four
	/*char *buff;
	buff = (char *)malloc(h*MAXSTRIGLEN * sizeof(char));
	snprintf(buff, sizeof(buff), "%s%s", "	|- Flag full:", full ? "true" : "false");*/
	String^ buff;
	if (back)
		for (sp* temp = spisok; temp != 0; temp = temp->sled, h++)
			buff += gcnew String(temp->fio) + "\n";
	else {
		for (sp* temp = spisok; temp->sled != 0; temp = temp->sled, spisok = temp);
		for (sp* temp = spisok; temp != 0; temp = temp->pred, h++)
			buff += gcnew String(temp->fio) + "\n";
	}
	textBox->Text = buff;
	printf("NameList created.\n");
}

void insert(sp **spi, char* fio) {

	sp *spisok = *spi, *pred, *next, *temp, *New = (sp *)malloc(sizeof(sp));
	if (!spisok) {
		New->pred = NULL;
		New->sled = NULL;
		spisok = New;
		(*spi) = New;
		strcpy(New->fio, fio);
		return;
	}
	for (temp = spisok; ;)
	{
		if (strcmp(temp->fio, fio) > 0) //temp.fio ��� ������ ��� fio
			if (temp->sled) //��� ������, ���� ����� ���� ������ ��������� � �����
				temp = temp->sled;
			else {
				temp->sled = New;
				New->pred = temp;
				New->sled = NULL;
				strcpy(New->fio, fio);
				return;
			}
		else if (strcmp(temp->fio, fio) < 0) { //temp.fio ��� ������ ��� fio, ������� ����� temp, ������ ���� ������
			//������ ���� ���������� ������
			New->sled = temp;
			New->pred = temp->pred;
			//��������� temp ������ ������ ����������� �� ���� 
			temp->pred = New;
			//��������� ������� ������������ �������� ������ �� ���� ��� ����� ����� ����, ���� �� ������� ����������
			if (New->pred)
				New->pred->sled = New;
			strcpy(New->fio, fio);
			//(*spi) = New; //�� �������� �������� ������ �� �����
			return;
		}

	}
}

void vstavka(sp **spi, char* fio)
{
	int i;
	struct sp *spisok = *spi, *New, *nt, *z = 0;
	//My code
	if (spisok==NULL) {
		spisok = (struct sp *) malloc(sizeof(struct sp));
		strcpy(spisok->fio, fio);
		spisok->pred = NULL;
		spisok->sled = NULL;
		*spi = spisok;
	}
	//
	for (nt = spisok; nt != 0 && strcmp(nt->fio, fio) < 0;
		z = nt, nt = nt->sled);
	if (nt && strcmp(nt->fio, fio) == 0) return;
	New = (struct sp *) malloc(sizeof(struct sp));
	strcpy(New->fio, fio);
	New->pred = z;
	New->sled = nt;


	if (!z) spisok = New, *spi = spisok;
	else z->sled = New;
	if (nt)  nt->pred = New;
	return;
}

struct division {
	char nameOfDivision[MAXSTRIGLEN];
	int man = 0, wooman = 0;
};
void MyForm::fillChart(System::Windows::Forms::DataVisualization::Charting::Chart^ chart, System::Windows::Forms::Label^ labelAll, System::Windows::Forms::Label^ labelMan, System::Windows::Forms::Label^ labelWooman) {

	//chart title  
	chart->Titles->Add("����������� ����������� � ������ �������");

	//������ ���� - ���������� ���-�� ���������� � ������ ���-�����
	//	|-��������� �������
	//������ ���� - ������������ ������� ��� ���������
	//������ ���� - ������� � ������ ��������� ������ � ������
	//�������� ���� - ������������ ��������

	// structure
	// lvl 1 | -lvl_1*idType*name*;
	//       |
	//       | -lvl_1*15*��������*;
	//       |
	// lvl 2 |	-|  -lvl_2*idType*idTypeParrent*name*;
	//       |
	//       |  -|  -lvl_2*16*15*����������*;
	//       |  -|  -lvl_2*17*15*IT - ����� * ;
	//       |  -|  -lvl_2*18*15*����� ����������*;
	//       |  -|  -lvl_2*19*15*������������ � ��*;
	//       |
	// lvl 3 |		-|  -lvl_3*idTypeParrent*name*{*key*name*:i:123, *key*name*:S:*Text* ...}; #i - integer, S - string, key - byte (���� key=0, ����������� �������������) !! �������� ��� ��� �� ����������� !! 
	//       |
	//       |		-|  -lvl_3*16*��������*{*0*��������*:S:*������� ���� ����������*, *9*���*:S:*�*};
	//       |		-|  -lvl_3*17*������� �����������*{*0*��������*:S:*������� ������ ���������*, *9*���*:S:*�*};
	//       |		-|  -lvl_3*18*�������� �����*{*0*��������*:S:*��������� ��� ��������*, *9*���*:S:*�*};
	//       |		-|  ...

	//step one
	int count = 0; //temp counter
	int man = 0, wooman = 0;
	for (int f = 0; f < countlvl1; f++)
		for (int i = 0; i < root[f].size; i++) {
			if (root[f].haveLvl2)
				if (root[f].dataLvl2[i].idTypeParrent == 15)
					count++;
		}


	//step two
	division *divisions;
	divisions = (division*)malloc(count * sizeof(division));
	for (int i = 0; i < count; i++) 
		divisions[i].man = 0, divisions[i].wooman = 0;
	
	//step free
	count = 0;
	for (int f = 0; f < countlvl1; f++)
		for (int i = 0; i < root[f].size; i++) {
			if (root[f].haveLvl2)
				if (root[f].dataLvl2[i].idTypeParrent == 15) {
					strcpy(divisions[count].nameOfDivision, root[f].dataLvl2[i].name);
					for (int g = 0; g < root[f].dataLvl2[i].size; g++)
						for (int j = 0; j < root[f].dataLvl2[i].data[g].size; j++)
							if (root[f].dataLvl2[i].data[g].metadata[j].key == 9) {
								if (strcmp(root[f].dataLvl2[i].data[g].StringMass[root[f].dataLvl2[i].data[g].metadata[j].idOnType], "�") == 0)
									divisions[count].wooman++, wooman++;
								
								else
									divisions[count].man++, man++;
							}
					count++;
				}
		}
	//step four
	for (int i = 0; i < count; i++) {
		chart->Series["�������"]->Points->AddXY(gcnew String(divisions[i].nameOfDivision), divisions[i].man);
		chart->Series["�������"]->Points->AddXY(gcnew String(divisions[i].nameOfDivision), divisions[i].wooman);
	}
	labelAll->Text = gcnew String("����� �����������: ") + (man+wooman);
	labelMan->Text = gcnew String("������: ") + man;
	labelWooman->Text = gcnew String("������: ") + wooman;
}

boolean haveEnd(char *string);
void loadlvl1(char* str, lvl1 &lvl);
void loadlvl2(char* str, lvl2 &lvl);
void loadlvl3(char* str, lvl3 &lvl, bool needMeta);
lvl1* load_lvl(int maxStringLen, lvl1 *root, int &countlvl1, int &countlvl2, int &countlvl3) {
	/*
	*��� ����������� ������� �������� ���������� �������� ���������
	*�� ������ ����� �������� �������� ��� ��������� ������ �� ������ �� ������� �������������, ������ ��������� �����, ��������
	*
	*/
	FILE *file_lvls;
	if ((file_lvls = fopen(lvlFile, "r")) == NULL)
	{
		printf("\nFile %s not open!", lvlFile);
		system("pause"); exit(1);
	}
	int stringsCount = 0;//���������� ��� �������� ���-�� ����� � �������, ����� ��������� ��� ��������� ������.
	char *readString = (char *)malloc(maxStringLen * sizeof(char));//������ ��� ��������� ������ �� �����
	while (fgets(readString, maxStringLen, file_lvls) != NULL) {
		//if (DEBAG) printf("\nDEBAG function load_lvl String \"%s\" len:%d", readString, strlen(readString));
		switch (readString[0])
		{
		case '#': continue;//������� ������������
		case '-'://�������� ���-�� �������� 
			stringsCount++;
			continue;
		}
	}
	rewind(file_lvls);//� ������ �����, ������� ���-�� �������� ������ ��������.

					  //if (DEBAG) printf("\nDEBAG function load_lvl Total strings:%d\n", stringsCount);

	char **stringsWithData = (char**)malloc(stringsCount * sizeof(char*));//������ ���������� �� ������ � ��� ��������������� �������.

	stringsCount = 0;
	char *tempString = (char *)malloc(maxStringLen * sizeof(char));//����� ��� ������������ ��������� ������ �����.
	char *copyTempString = tempString;
	while (fgets(readString, maxStringLen, file_lvls) != NULL) {
		//if (DEBAG) printf("\nDEBAG function load_lvl Read String \"%s\" len:%d", readString, strlen(readString));
		switch (readString[0])
		{
		case '#': continue;//������� ������������
		case '-':
			while (!haveEnd(readString)) {//���� � ������ ��� ";" ������ � ���� ������ ��������� �� ����������� � ��������� ��������� ������.
										  //if (DEBAG) printf("\nDEBAG function load_lvl String \"%s\" len:%d", readString, strlen(readString));
				if (fgets(tempString, maxStringLen, file_lvls) != NULL) {
					strcat(readString, tempString); //��������� ������ ������ � ������ ��� ������������ ������.
				}
				else {
					//FIXME: ������, � ���� ������ ���� ������ ����� ���������� ��������.
				}
			}
			break;
		default:
			continue;
		}
		//if (DEBAG) printf("\nDEBAG function load_lvl Full String \"%s\" len:%d", readString, strlen(readString));
		stringsWithData[stringsCount] = (char*)malloc(strlen(readString) * sizeof(char)); //��������� ������������ ���-�� ������ ��� "������ ������"
		strcpy(stringsWithData[stringsCount], readString);//���������� ���������� �����.
														  //if (DEBAG) printf("\nDEBAG function load_lvl Full String \"%s\" len:%d ", stringsWithData[stringsCount], strlen(stringsWithData[stringsCount]));
		stringsCount++;
		//printf("%d", stringsCount);
	}
	for (int i = 0; i < stringsCount; i++) {//��������� 5-� ������, ������ ���������� �� ����� ������
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
				   //��������� 1 ������� � ������� ���-�� ��������� �� 2-� ������
	for (int i = 0; i < stringsCount; i++) {//��������� 5-� ������, ������ ���������� �� ����� ������
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 1:
			loadlvl1(&stringsWithData[i][6], root[countlvl1]);
			//if (DEBAG) printf("DEBAG function loadlvl Id:%d Name:%s\n", root[lv1].idType, root[lv1].name);
			countlvl1++;
			break;
		case 2: //������� ��������� ��������� lvl 2 � ������ lvl 1
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

	//�������� ������ ��� ��������� 2-�� lvl
	for (int i = 0; i < countlvl1; i++) {
		if (root[i].haveLvl2 == true) {
			root[i].dataLvl2 = (struct lvl2*)malloc(root[i].size * sizeof(struct lvl2));
			root[i].size = 0; //����� �������������� ��� �������
		}
	}

	//��������� 2 ������� 
	for (int i = 0; i < stringsCount; i++) {//��������� 5-� ������, ������ ���������� �� ����� ������
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
					loadlvl2(&stringsWithData[i][6], root[j].dataLvl2[root[j].size]); //root[j].size - ������������ � �������� ��������.
					root[j].dataLvl2[root[j].size].size = 0;
					root[j].size++;
					break;
				}
			}
			break;
		}
	}

	//��� ���������� 3-�� ������ ����� ���-�� �������������� ������� ��� �������� ��� ���������, �������������� ��������� �� �������� �� ������ � ���������� � 0.
	for (int i = 0; i < stringsCount; i++) {//��������� 5-� ������, ������ ���������� �� ����� ������
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString)) {
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int i = 0; i < countlvl1; i++) {//������ �� 1-�� ������.
				if (root[i].haveLvl2 == true) {
					for (int j = 0; j < root[i].size; j++) {//������ �� 2-�� ������ ���� ������� ����.
						if (root[i].dataLvl2[j].idType == temp.idTypeParrent) {
							root[i].dataLvl2[j].size = 0;
							goto contini1;
						}
					}
				}
				else //������ �� 3-�� ������ ���� ��� ��������� � ���� 2-��.
					if (root[i].idType == temp.idTypeParrent) {
						root[i].size = 0;
						goto contini1;
					}
			}
		}
	contini1:
		;
	}

	//������� ���-�� ��������� �� 3-� ������
	for (int i = 0; i < stringsCount; i++) {//��������� 5-� ������, ������ ���������� �� ����� ������
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString)) {
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//������ �� 1-�� ������.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//������ �� 2-�� ������ ���� ������� ����.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].size++;
							goto contini2;
						}
				}
				else { //������ �� 3-�� ������ ���� ��� ��������� � ���� 2-��.
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

	//printf("����� ���������� ������\n");
	if (false)	for (int i = 0; i < countlvl1; i++) {//���������� ��������� �� ������ ������ 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//���������� ��������� �� 2-� �������
			if (root[i].haveLvl2) {
				printf("	|-lvl2- IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++)//���������� ��������� �� 3-� �������
					;// printf("		|-lvl3- IdTypeParrent:%d Name:%s\n", root[i].dataLvl3[h].idTypeParrent, root[i].dataLvl3[h].name);
					 //root[i].dataLvl2[j]
			}
			else
				;//printf("	|-lvl3- IdTypeParrent:%d Name:%s\n", (*root[i].dataLvl3).idTypeParrent, (*root[i].dataLvl3).name);
		}
	}

	//�������� ������ ��� �������� 3-�� ������
	for (int i = 0; i < stringsCount; i++) {//��������� 5-� ������, ������ ���������� �� ����� ������
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//������ �� 1-�� ������.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//������ �� 2-�� ������ ���� ������� ����.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].data = (struct lvl3*)malloc(root[h].dataLvl2[j].size * sizeof(lvl3));
							goto contini3;
						}
				}
				else //������ �� 3-�� ������ ���� ��� ��������� � ���� 2-��.
					if (root[h].idType == temp.idTypeParrent) {
						root[h].dataLvl3 = (struct lvl3*)malloc(root[h].size * sizeof(lvl3));
						goto contini3;
					}
			}
		}
	contini3:
		;
	}

	//��������� ������� �������� 3-�� ������ ����� ������������ �� ��� ��������.
	for (int i = 0; i < stringsCount; i++) {//��������� 5-� ������, ������ ���������� �� ����� ������
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//������ �� 1-�� ������.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//������ �� 2-�� ������ ���� ������� ����.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							root[h].dataLvl2[j].size = 0;
							goto contini4;
						}
				}
				else //������ �� 3-�� ������ ���� ��� ��������� � ���� 2-��.
					if (root[h].idType == temp.idTypeParrent) {
						root[h].size = 0;
						goto contini4;
					}
			}
		}
	contini4:
		;
	}
	//printf("����� �����������\n");
	if (false)	for (int i = 0; i < countlvl1; i++) {//���������� ��������� �� ������ ������ 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//���������� ��������� �� 2-� �������
			if (root[i].haveLvl2) {
				printf("	|-lvl2- IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++)//���������� ��������� �� 3-� �������
					;// printf("		|-lvl3- IdTypeParrent:%d Name:%s\n", root[i].dataLvl3[h].idTypeParrent, root[i].dataLvl3[h].name);
					 //root[i].dataLvl2[j]
			}
			else
				;// printf("	|-lvl3- IdTypeParrent:%d Name:%s\n", (*root[i].dataLvl3).idTypeParrent, (*root[i].dataLvl3).name);
		}
	}

	//��������� ���������� 3-�� ������
	for (int i = 0; i < stringsCount; i++) {//��������� 5-� ������, ������ ���������� �� ����� ������
		tempString[0] = stringsWithData[i][5];
		tempString[1] = '\0';
		switch (atoi(tempString))
		{
		case 3:
			lvl3 temp;
			loadlvl3(&stringsWithData[i][6], temp, false);
			for (int h = 0; h < countlvl1; h++) {//������ �� 1-�� ������.
				if (root[h].haveLvl2 == true) {
					for (int j = 0; j < root[h].size; j++)//������ �� 2-�� ������ ���� ������� ����.
						if (root[h].dataLvl2[j].idType == temp.idTypeParrent) {
							loadlvl3(&stringsWithData[i][6], root[h].dataLvl2[j].data[root[h].dataLvl2[j].size], true);//���������, ����� ������� ����..��� ���������� � i,h & j //FIXME: ��� � ������� ����� �����-�� �����!!
																													   //printf("Debag:temp  |-lvl3- IdTypeParrent:%d Name:%s\n", temp.idTypeParrent, temp.name);
							root[h].dataLvl2[j].size++;
							goto contini5;
						}

				}
				else //������ �� 3-�� ������ ���� ��� ��������� � ���� 2-��.
					if (root[h].idType == temp.idTypeParrent) { //FIXME: ��� � ������� ����� �����-�� �����!!
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

	printf("��������� �����\n");
	if (DEBAG)	for (int i = 0; i < countlvl1; i++) {//���������� ��������� �� ������ ������ 
		printf("|-lvl1- IdType:%d Name:%s Size:%d Type child %s\n", root[i].idType, root[i].name, root[i].size, (root[i].haveLvl2 ? "2" : "3"));
		for (int j = 0; j < root[i].size; j++) {//���������� ��������� �� 2-� �������
			if (root[i].haveLvl2) {
				printf("	|-lvl2- J:%d IdType:%d IdTypeParrent:%d Size:%d Name:%s\n", j, root[i].dataLvl2[j].idType, root[i].dataLvl2[j].idTypeParrent, root[i].dataLvl2[j].size, root[i].dataLvl2[j].name);
				for (int h = 0; h < root[i].dataLvl2[j].size; h++) {//���������� ��������� �� 3-� �������
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
	return root;
	fclose(file_lvls);
	//free(tempString);//������ ���� �� �������� �����������, ����� ������..! ��� �� ����..
}

boolean haveEnd(char *string) {//������� ������ �������� ����� � Tab (��� ��� ���������).
	int i = 0;
	int len = strlen(string);//������� �������
	for (i = 0; i < len; i++)
		switch (string[i])
		{
		case ';'://���� ; ������, � ���� ������ ��������� ���������.
			if (i + 1 < len&&string[i + 1] == '\n')
				string[i] = '\0';
			return true;
		case '	'://������ Tab �� ������
			string[i] = ' ';
		case '\n'://������ Enter �� ������
			string[i] = ' ';
		}
	return false;
}
void loadlvl1(char* str, lvl1 &lvl) {//FIXME:���������� ��� ����� * � �� ����������� ���������
									 //str =  *idType*name* 
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//���������� ������� ������� * � �������� ���
	*(strchr(tempString, c)) = '\0';//���������� ������� ������� * � ���������� ����� ������ ����.
	lvl.idType = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//���������� ����� � ����������� �� ���� ������ ������ (� �� ������ ����� �� �����??)
	*(strchr(tempString, c)) = '\0';//���������� ������� * � ���������� ����� ������ ����.
	lvl.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(lvl.name, tempString);
	//free(tempString);//�� ������ ����������
	//if (DEBAG) printf("DEBAG function loadlvl1 Id:%d Name:%s\n", lvl.idType, lvl.name);
}

void loadlvl2(char* str, lvl2 &lvl) {
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//���������� ������� ������� * � �������� ���
	*(strchr(tempString, c)) = '\0';//���������� ������� ������� * � ���������� ����� ������ ����.
	lvl.idType = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//���������� ����� � ����������� �� ���� ������ ������ (� �� ������ ����� �� �����??)
	*(strchr(tempString, c)) = '\0';//���������� ������� * � ���������� ����� ������ ����.
	lvl.idTypeParrent = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//���������� ����� � ����������� �� ���� ������ ������
	*(strchr(tempString, c)) = '\0';//���������� ������� * � ���������� ����� ������ ����.
	lvl.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(lvl.name, tempString);
	//if (DEBAG) printf("DEBAG function loadlvl2 IdType:%d IdTypeParrent:%d Name:%s\n", lvl.idType, lvl.idTypeParrent, lvl.name);
}

void loadlvlMeta(char* str, meta &met, char **text, int &integer);
void loadlvl3(char* str, lvl3 &lvl, bool needMeta) {
	//printf("� ��� %s ��������.\n", (_CrtCheckMemory()? "������������" : "�����������"));
	//if (DEBAG) printf("DBAG function loadlvl3 String:%s\n", str);
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	char *tempMetaString = (char*)malloc(strlen(str) * sizeof(char));
	//printf("sizeof tempString %d\n", strlen(tempString));
	strcpy(tempString, str);
	tempString = strchr(tempString, c), tempString++;//���������� ������� ������� * � �������� ���
	*(strchr(tempString, c)) = '\0';//���������� ������� ������� * � ���������� ����� ������ ����.
	lvl.idTypeParrent = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//���������� ����� � ����������� �� ���� ������ ������
	*(strchr(tempString, c)) = '\0';//���������� ������� * � ���������� ����� ������ ����.
	lvl.name = (char *)malloc((strlen(tempString) + 1) * sizeof(char));
	strcpy(lvl.name, tempString);
	tempString = strchr(tempString, '\0'), tempString++;//���������� ����� � ����������� �� ���� ������ ������ ����� ���� ����������� ������ ������ 
														//if (DEBAG) printf("DEBAG function loadlvl3 String:%s\n", tempString);
	tempString = strchr(tempString, '{'), tempString++;//���������� ������� ������� { � �������� ���
	*(strchr(tempString, '}')) = '\0';//���������� ������� ������� } � ���������� ����� ������ ����.
									  //if (DEBAG) printf("DEBAG function loadlvl3 IdTypeParrent:%d Name:%s meta:%s\n", lvl.idTypeParrent, lvl.name, tempString);
									  //printf("� ��� %s ��������.\n", (_CrtCheckMemory() ? "������������" : "�����������"));
	if (!needMeta)
		return;
	char* metString = 0;
	int countIntMeta = 0, countStringMeta = 0, metInt = 0;
	meta met;
	met.idOnType = 0, met.name = "", met.variableType = variableTypes::___INT, met.vizible = false;
	char *copyTempMetaString = tempMetaString;//������ ����� ������ �� ������ ������
	strcpy(tempMetaString, tempString);
	while (true) {
		bool needBreak = false;
		if (strchr(tempMetaString, ',') == NULL)//���� true ������ �� ����� ��������� ��������
			needBreak = true;
		else
			*(strchr(tempMetaString, ',')) = '\0';//���������� ������� ',' � ���������� ����� ������ ����.
		tempMetaString = strchr(tempMetaString, c);//���������� ������� ������� * � ����������� ����
		loadlvlMeta(tempMetaString, met, &metString, metInt);
		if (met.variableType == variableTypes::___INT)
			countIntMeta++;
		else
			countStringMeta++;
		tempMetaString = strchr(tempMetaString, '\0'), tempMetaString++; //���������� ����� � ����������� �� ���� ������ ������
		if (needBreak)
			break;
	}
	//printf("countIntMeta: %d countStringMeta: %d\n", countIntMeta, countStringMeta);
	lvl.metadata = (meta*)malloc((countIntMeta + countStringMeta) * sizeof(meta));
	lvl.IntegerMass = (int*)malloc(countIntMeta * sizeof(int));
	lvl.StringMass = (char**)malloc(countStringMeta * sizeof(char*));
	lvl.size = countIntMeta + countStringMeta;
	countIntMeta = 0, countStringMeta = 0;
	tempMetaString = copyTempMetaString;//���������� ��������� �� ������ ����������� ����� ������ � ��� ���� �������� ���-����
	strcpy(tempMetaString, tempString);

	while (true) {
		bool needBreak = false;
		if (strchr(tempMetaString, ',') == NULL)//���� true ������ �� ����� ��������� ��������
			needBreak = true;
		else
			*(strchr(tempMetaString, ',')) = '\0';//���������� ������� ',' � ���������� ����� ������ ����.
		tempMetaString = strchr(tempMetaString, c);//���������� ������� ������� * � ����������� ����
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
		tempMetaString = strchr(tempMetaString, '\0'), tempMetaString++; //���������� ����� � ����������� �� ���� ������ ������
																		 //printf("Lvl:%20s Metdata: idOnType =%2d, key=%2d, variableType =%2d, vizible =%2d, met.name = %30s, metInt = %3d, metString = %s\n", lvl.name, lvl.metadata[(countIntMeta + countStringMeta)-1].idOnType, lvl.metadata[(countIntMeta + countStringMeta) - 1].key, lvl.metadata[(countIntMeta + countStringMeta)-1].variableType, lvl.metadata[(countIntMeta + countStringMeta)-1].vizible, lvl.metadata[(countIntMeta + countStringMeta) - 1].name, metInt, metString);
		if (needBreak)
			break;
	}
	//printf("� ��� %s �������� � ������������ ������.\n", (_CrtCheckMemory() ? "������������" : "�����������"));
	//printf("metdata: met.idOnType = %d, met.idOnType = %d, met.name = %s, met.variableType = %d, met.vizible = %d, metInt = %d, metString = %s\n", met.idOnType, met.idOnType, met.name, met.variableType, met.vizible, metInt, metString);
}

void loadlvlMeta(char* str, meta &met, char **text, int &integer) {
	char c = '*';
	char *tempString = (char*)malloc(strlen(str) * sizeof(char));
	strcpy(tempString, str);
	//printf("//*key*name*:i:123 OR *key*name*:S:*Text* \n %s \n", tempString);
	tempString = strchr(tempString, c), tempString++;//���������� ������� ������� * � �������� ���
													 //key*name*:i:123 OR key*name*:S:*Text*
													 //printf("//key*name*:i:123 OR key*name*:S:*Text* \n %s \n", tempString);
	*(strchr(tempString, c)) = '\0';//���������� ������� ������� * � ���������� ����� ������ ����.
									//key'\0'name*:i:123 OR key'\0'name*:S:*Text*
	met.key = atoi(tempString);
	tempString = strchr(tempString, '\0'), tempString++;//���������� ����� � ����������� �� ���� ������ ������
														//name*:i:123 OR name*:S:*Text*
	*(strchr(tempString, c)) = '\0';//���������� ������� * � ���������� ����� ������ ����.
									//name'\0':i:123 OR name'\0':S:*Text*
	met.name = (char *)malloc(strlen(tempString) * sizeof(char));
	strcpy(met.name, tempString);
	tempString = strchr(tempString, '\0'), tempString++;//���������� ����� � ����������� �� ���� ������ ������
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

