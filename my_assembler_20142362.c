/*
* ȭ�ϸ� : my_assembler_00000000.c
* ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
* �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
* ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
*/

/*
*
* ���α׷��� ����� �����Ѵ�.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

// ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20142362.h"

/* ----------------------------------------------------------------------------------
* ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
* �Ű� : ���� ����, ����� ����
* ��ȯ : ���� = 0, ���� = < 0
* ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
*         ���� �߰������� �������� �ʴ´�.
* ----------------------------------------------------------------------------------
*/
int main(int args, char *argv[])
{
	int i;
	if (init_my_assembler()< 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0) {
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	//make_opcode_output(argv[1]);
	make_symtab_output("symtab_20142362.txt");

	if (assem_pass2() < 0) {
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	make_objectcode_output("output_20142362.txt");
	
	for (i = 0; i < inst_index; i++)
		free(inst_table[i]);
	for (i = 0; i < line_num; i++) {
		free(input_data[i]);
		free(token_table[i]);
	}

	return 0;
}

/* ----------------------------------------------------------------------------------
* ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
* �Ű� : ����
* ��ȯ : �������� = 0 , ���� �߻� = -1
* ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
*         ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
*         �����Ͽ���.
* ----------------------------------------------------------------------------------
*/
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.dat")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
* ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
*        �����ϴ� �Լ��̴�.
* �Ű� : ���� ��� ����
* ��ȯ : �������� = 0 , ���� < 0
* ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
*
*   ===============================================================================
*         | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
*   ===============================================================================
*
* ----------------------------------------------------------------------------------
*/
int init_inst_file(char *inst_file)
{
	FILE * file;
	int errno;

	/* add your code here */
	inst st;
	int i;
	if ((file = fopen(inst_file, "rb")) == NULL)
		return -1;
	fseek(file, 0, SEEK_END);
	inst_index = ftell(file) / (sizeof(inst));
	fseek(file, 0, SEEK_SET);
	for (i = 0; i < inst_index; i++) {
		inst_table[i] = malloc(sizeof(inst));

		/******************instruction set �Է¹ް� inst.dat�� ��������***********
		scanf("%s %s %s %s", inst_table[i]->mnemonic, inst_table[i]->format,
		inst_table[i]->opcode,inst_table[i]->operandnum);
		fwrite(inst_table[i]->mnemonic, 7, 1, file);
		fwrite(inst_table[i]->format, 5, 1, file);
		fwrite(inst_table[i]->opcode, 5, 1, file);
		fwrite(inst_table[i]->operandnum, 2, 1, file);
		*/
	}


	for (i = 0; i < inst_index; i++) {
		//inst.dat�� �ִ� �����͸� �о inst_table�� ����
		fread(inst_table[i]->mnemonic, 7, 1, file);
		fread(inst_table[i]->format, 5, 1, file);
		fread(inst_table[i]->opcode, 5, 1, file);
		fread(inst_table[i]->operandnum, 2, 1, file);

		/**** inst_table�� �� �������� Ȯ�������� ��� �ڵ�******
		printf("inst_table : %s %s %s %s\n", inst_table[i]->mnemonic,
		inst_table[i]->format,
		inst_table[i]->opcode,
		inst_table[i]->operandnum);
		//*/
	}

	return errno;
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�.
* �Ű� : ������� �ҽ����ϸ�
* ��ȯ : �������� = 0 , ���� < 0
* ���� : ���δ����� �����Ѵ�.
*
* ----------------------------------------------------------------------------------
*/
int init_input_file(char *input_file)
{
	FILE * file;
	int errno;

	/* add your code here */
	int i;
	line_num = 0;
	char buff[50];
	char c;
	if ((file = fopen(input_file, "r")) == NULL) {
		return -1;
	}
	while ((c = fgetc(file)) != EOF)
		if (c == '\n') line_num++;
	fseek(file, 0, SEEK_SET);
	for (i = 0; i < line_num; i++) {
		input_data[i] = malloc(sizeof(buff));
		if (fgets(buff, sizeof(buff), file) == NULL)
			return -1;
		strcpy(input_data[i], buff);

		//�ҽ��ڵ����̺�(input_data)�� �� �������� Ȯ���ϱ� ���� ����ڵ�
		//printf("%s", input_data[i]); 

	}
	return errno;
}

/* ----------------------------------------------------------------------------------
* ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
*        �н� 1�� ���� ȣ��ȴ�.
* �Ű� : �Ľ��� ���ϴ� ���ڿ�
* ��ȯ : �������� = 0 , ���� < 0
* ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
* ----------------------------------------------------------------------------------
*/
int token_parsing(char *str)
{

	/* add your code here */
	int check;
	//..............subrountin jump �ϴ� ����..................
	if (str[0] == ' ') {
		return -1; // tab���� �����Ͽ��� ������ �ҽ����� ó���� ������ ���� �� �ν����� ���Ѵ�.
	}
	if (str[0] == '.') {
		//2������ token_table label ���� RDREC�� TO READ RECORD ���//
		if (strcmp(token_table[token_line - 2]->label, "RDREC") == 0)
		{
			token_table[token_line]->label = malloc(3);
			strcpy(token_table[token_line]->label, ".");
			token_table[token_line]->operator = malloc(30);
			strcpy(token_table[token_line]->operator, "SUBROUTINE TO READ RECORD INTO BUFFER");
			token_table[token_line]->operand[0] = malloc(3);
			strcpy(token_table[token_line]->operand[0], "");
			token_table[token_line]->comment = malloc(3);
			strcpy(token_table[token_line]->comment, "");
		}
		//2������ token_table label ���� RDREC�� TO WRITE RECORD ���//
		else if (strcmp(token_table[token_line - 2]->label, "WRREC") == 0)
		{
			token_table[token_line]->label = malloc(3);
			strcpy(token_table[token_line]->label, ".");
			token_table[token_line]->operator = malloc(30);
			strcpy(token_table[token_line]->operator, "SUBROUTINETO WRITE RECORD INTO BUFFER");
			token_table[token_line]->operand[0] = malloc(3);
			strcpy(token_table[token_line]->operand[0], "");
			token_table[token_line]->comment = malloc(3);
			strcpy(token_table[token_line]->comment, "");
		}
		else {
			token_table[token_line]->label = malloc(3);
			strcpy(token_table[token_line]->label, ".");
			token_table[token_line]->operator = malloc(3);
			strcpy(token_table[token_line]->operator, "");
			token_table[token_line]->operand[0] = malloc(3);
			strcpy(token_table[token_line]->operand[0], "");
			token_table[token_line]->comment = malloc(3);
			strcpy(token_table[token_line]->comment, "");
		}
	}

	//Label���� ����ִ� ����
	if (str[0] == '\t' || str[0] == ' ') {
		check = 0;
		token_table[token_line]->label = malloc(3);
		strcpy(token_table[token_line]->label, "");

		char *ptr = strtok(str, "\t");
		token_table[token_line]->operator = malloc(strlen(ptr) * sizeof(char));
		strcpy(token_table[token_line]->operator, ptr);
		while (1) {

			ptr = strtok(NULL, "\t");
			if (ptr != NULL)
				check++;
			if (ptr == NULL)
				break;
			if (check == 1) {
				token_table[token_line]->operand[0] = malloc(strlen(ptr) * sizeof(char) + 10);
				strcpy(token_table[token_line]->operand[0], ptr);
			}
			if (check == 2) {
				token_table[token_line]->comment = malloc(strlen(ptr) * sizeof(char) + 10);
				strcpy(token_table[token_line]->comment, ptr);
			}
		}
		//������ ��ū�� ���๮�ڸ� �����ְ� ����ִ� ��ū���� ������ �ִ´�.
		if (check == 0) {
			char *ptr2 = strtok(token_table[token_line]->operator,"\n");
			strcpy(token_table[token_line]->operator,ptr2);
			token_table[token_line]->operand[0] = malloc(strlen(ptr2) * sizeof(char));
			strcpy(token_table[token_line]->operand[0], "");
			token_table[token_line]->comment = malloc(strlen(ptr2) * sizeof(char));
			strcpy(token_table[token_line]->comment, "");
		}
		if (check == 1) {
			char *ptr2 = strtok(token_table[token_line]->operand[0], "\n");
			strcpy(token_table[token_line]->operand[0], ptr2);
			token_table[token_line]->comment = malloc(strlen(ptr2) * sizeof(char));
			strcpy(token_table[token_line]->comment, "");
		}
		/*****************����� �ϱ� ���� �߰� ��� �ڵ�***************************
		printf("token table label[%d] = %s\n", token_line, token_table[token_line]->label);
		printf("token table operator[%d] = %s\n", token_line, token_table[token_line]->operator);
		printf("token table operand[%d] = %s\n", token_line, token_table[token_line]->operand[0]);
		printf("token table comment[%d] = %s\n", token_line, token_table[token_line]->comment);
		printf("-------------------------------------------------\n");
		//*/
	}

	//Label �� ���� 
	if (str[0] != '\t' && str[0] != '.' &&str[0] != ' ')
	{
		check = 0;
		char *ptr = strtok(str, "\t");
		token_table[token_line]->label = malloc(strlen(ptr) * sizeof(char));
		strcpy(token_table[token_line]->label, ptr);
		while (1) {

			ptr = strtok(NULL, "\t");
			if (ptr != NULL)
				check++;
			if (ptr == NULL)
				break;
			if (check == 1)
			{
				token_table[token_line]->operator = malloc(strlen(ptr) * sizeof(char));
				strcpy(token_table[token_line]->operator, ptr);
			}
			else if (check == 2)
			{
				token_table[token_line]->operand[0] = malloc(strlen(ptr) * sizeof(char));
				strcpy(token_table[token_line]->operand[0], ptr);
			}

			else if (check == 3)
			{
				token_table[token_line]->comment = malloc(strlen(ptr) * sizeof(char));
				strcpy(token_table[token_line]->comment, ptr);
			}
		}
		if (check == 1) {

			//������ ��ū�� ���๮�ڸ� �����ְ� ����ִ� ��ū���� ������ �ִ´�.
			char *ptr2 = strtok(token_table[token_line]->operator,"\n");
			strcpy(token_table[token_line]->operator,ptr2);
			token_table[token_line]->operand[0] = malloc(strlen(ptr2) * sizeof(char));
			strcpy(token_table[token_line]->operand[0], "");
			token_table[token_line]->comment = malloc(strlen(ptr2) * sizeof(char));
			strcpy(token_table[token_line]->comment, "");
		}
		if (check == 2) {
			char *ptr2 = strtok(token_table[token_line]->operand[0], "\n");
			strcpy(token_table[token_line]->operand[0], ptr2);
			token_table[token_line]->comment = malloc(strlen(ptr2) * sizeof(char));
			strcpy(token_table[token_line]->comment, "");
		}

		/*****************����� �ϱ� ���� �߰� ��� �ڵ�***************************
		printf("token table label[%d] = %s\n", token_line, token_table[token_line]->label);
		printf("token table operator[%d] = %s\n", token_line, token_table[token_line]->operator);
		printf("token table operand[%d] = %s\n", token_line, token_table[token_line]->operand[0]);
		printf("token table comment[%d] = %s\n", token_line, token_table[token_line]->comment);
		printf("--------------------------------------------------\n");
		//*/

	}
	// operand �迭�� �Ҵ�  0,1,2,3 �̷��� 4���� �迭�� �Ҵ�����
	token_table[token_line]->operand[1] = malloc(1);
	strcpy(token_table[token_line]->operand[1], "");
	token_table[token_line]->operand[2] = malloc(1);
	strcpy(token_table[token_line]->operand[2], "");
	token_table[token_line]->operand[3] = malloc(1);
	strcpy(token_table[token_line]->operand[3], "");

	//operand[0]�� ���� �ؼ� operand�迭�� �־���
	int i = 0;
	char *ptr3 = strtok(token_table[token_line]->operand[i], ",");
	while (1) {
		ptr3 = strtok(NULL, ",");
		if (ptr3 == NULL)
			break;
		i++;
		token_table[token_line]->operand[i] = malloc(strlen(ptr3) * sizeof(char));
		strcpy(token_table[token_line]->operand[i], ptr3);
	}
	return 0;
}


/* ----------------------------------------------------------------------------------
* ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
* �Ű� : ��ū ������ ���е� ���ڿ�
* ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
* ���� :
*
* ----------------------------------------------------------------------------------
*/
int search_opcode(char *str)
{
	/* add your code here */
	int i;
	int check = 0;
	if (str == NULL)
		return -1;
	if (strcmp(str, "1") < 0) { //4������ ���� �Է¹޾����� inst_table�� '+'�� �߰��ؼ� ���Ѵ�.
		for (i = 0; i < inst_index; i++) {
			char e_1[8] = "+";
			strcat(e_1, inst_table[i]->mnemonic);
			if (strcmp(str, e_1) == 0) {

				//�����ڵ尡 �ִ� ���� �� 4������ ��������� �˷��ִ� ����ڵ��̴�.
				//printf("line %d = %s\n", token_line, str);

				strcpy(token_table[token_line]->comment, inst_table[i]->opcode);
				check = 1; //�Է¹��ڿ��� �����ڵ����� �ǹ�
			}
		}
	}

	for (i = 0; i < inst_index; i++) {

		if (strcmp(str, inst_table[i]->mnemonic) == 0) {
			//�����ڵ尡 �ִ� ������ ������� �˷��ִ� ����ڵ��̴�.
			//printf("line %d = %s\n", token_line, str); //
			strcpy(token_table[token_line]->comment, inst_table[i]->opcode);
			check = 1;
		}
	}
	if (check == 0)//�Է¹��ڿ��� �����ڵ尡 �ƴҶ� comment�� ����־���
		strcpy(token_table[token_line]->comment, "");
	return 0;
}



/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*         �н�1������..
*         1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*         ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*     ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */
	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ�
	* token_parsing()�� ȣ���Ͽ� token_unit�� ����
	*/
	int i, j, k, start, flag, abcheck;
	symnum = 0; // symbol table ũ��
	litnum = 0; // literal table ũ��
	token_line = 0;

	//���� �Ľ� �� opcode ��Ī
	for (i = 0; i < line_num; i++)
		token_table[i] = malloc(sizeof(token));
	for (i = 0; i < line_num; i++)
	{
		if (input_data[i] == NULL)
			return -1;
		if (token_parsing(input_data[i]) == -1) {
			printf("source code ���� �κ� ����Ȯ��\n");
			return -1;
		}
		token_line++;
	}

	token_line = 0;
	for (i = 0; i < line_num; i++)
	{
		search_opcode(token_table[i]->operator);
		if (search_opcode(token_table[i]->operator) == -1)
			return -1;
		token_line++;
	}

	sectnum = 0;


	//symbol table ���� �� literal ���� ��//
	for (i = 0; i < line_num; i++)
	{	
		

		abcheck = 0; // absolute expression �� symbol table�� ���� ���� ���� ����
		flag = 0; // literal table�� ���� ���� ����Ǵ� ���� �����ϱ� ����

		//operator "START" �϶�
		if (strcmp(token_table[i]->operator, "START") == 0)
		{
			start = atoi(token_table[i]->operand[0]);
			locctr = start;
		}
		//operator "RESW" �϶�
		else if (strcmp(token_table[i - 1]->operator, "RESW") == 0)	
			locctr = locctr + 3 * atoi(token_table[i - 1]->operand[0]);
		//operator "RESB" �϶�
		else if (strcmp((token_table[i - 1]->operator), "RESB") == 0)
			locctr = locctr + atoi(token_table[i - 1]->operand[0]);
		//operator "4����" �϶�
		else if (token_table[i - 1]->operator[0] == '+')
			locctr = locctr + 4;
		//operator "LTORG" �϶�
		else if (strcmp(token_table[i - 1]->operator, "LTORG") == 0)
		{
			// LTORG ���� '='���� �ִ� �͵��� ã�Ƽ� literal_table�� �־��ش�.
			for (j = 0, flag = 0; j < i - 1; j++)
				if (token_table[j]->operand[0][0] == '=')
				{
					flag = 0;
					//literal table�� �˻��ؼ� ���� �ߺ����� �ʵ��� �Ѵ�
					for (k = 0; k < litnum; k++)
						if (strcmp(token_table[j]->operand[0], literal_table[k].lvalue))
							flag = 1;
					if (flag == 0)
					{	
						// ĳ���� ���� ���
						if (token_table[j]->operand[0][1] == 'C')
						{
							strcpy(literal_table[litnum].lvalue, token_table[j]->operand[0]);
							literal_table[litnum].addr = locctr;
							//-4�� ���ִ� ������ =,C,',' �̷��� 4���� ���� �����ϱ� ����
							locctr = locctr + strlen(token_table[j]->operand[0]) - 4;
							litnum++;
						}
						// 16���� ���� ��� 
						else if (token_table[j]->operand[0][1] == 'X')
						{
							strcpy(literal_table[litnum].lvalue, token_table[j]->operand[0]);
							literal_table[litnum].addr = locctr;
							//���������� 4���ְ� 16������ 2���ڴ� 1����Ʈ�̹Ƿ� 2�� ������
							locctr = locctr + (strlen(token_table[j]->operand[0]) - 4) / 2;
							litnum++;
						}
					}
				}
		}
		//operator "BYTE" �϶�
		else if (strcmp(token_table[i - 1]->operator,"BYTE") == 0)
			locctr = locctr + 1;
		//operator "WORD" �϶�
		else if (strcmp(token_table[i - 1]->operator,"WORD") == 0)
			locctr = locctr + 3;

		// �׿��� 2���İ� 3/4�����ε� 4������ ������ ó�������Ƿ� 3/4�� �� 3�����̴�
		else
		{
			for (j = 0; j < inst_index; j++)
			{
				if (strcmp(token_table[i - 1]->operator,inst_table[j]->mnemonic) == 0)
				{
					if (strcmp(inst_table[j]->format, "3/4") == 0)
						locctr = locctr + 3;

					else if (strcmp(inst_table[j]->format, "2") == 0)
						locctr = locctr + 2;

				}
			}
		}
		//operator "CSECT" �϶� 
		if (strcmp(token_table[i]->operator,"CSECT") == 0)
		{
			//������ ����ũ�� ����
			sectsize[sectnum] = locctr;
			locctr = 0;
			sectnum++;
			//sector�� �ٲ�����Ƿ� ������ ��ĭ�ֱ� ����
			strcpy(sym_table[symnum].symbol, "");
			symnum++;
		}

		//EQU�� ��� '-'�������� ���۷��带 ����� ������ ���۷��� ���� ��� ���ش�
		else if (strcmp(token_table[i]->operator,"EQU") == 0 && token_table[i]->operand[0][0] != '*')
		{
			int num1, num2;
			char *ptr = strtok(token_table[i]->operand[0], "-");
			for (j = 0; j < symnum; j++)
			{
				if (strcmp(ptr, sym_table[j].symbol) == 0)
					num1 = sym_table[j].addr;
			}
			while (1) {
				ptr = strtok(NULL, "-");
				if (ptr == NULL)
					break;
				for (j = 0; j < symnum; j++)
				{
					if (strcmp(ptr, sym_table[j].symbol) == 0)
						num2 = sym_table[j].addr;
				}
			}
			// absolute expression �� ���� symbol table�� �־��� 
			// �������� ����Ǵ� ���� ���� ���� abcheck�� 1�� ����
			strcpy(sym_table[symnum].symbol, token_table[i]->label);
			sym_table[symnum].addr = num1 - num2;
			symnum++;
			abcheck = 1;
		}

		/*END�� ��� ó������ ������ ���鼭 '='�� �͵��� ã���ְ� �̰͵���
		literal_table�� ���� �͵��� ���̺� �־��ش�.
		(literal_table�� �ִ� ���� LTORG���� �̹� ó��*/
		else if (strcmp(token_table[i]->operator,"END") == 0)
		{
			for (j = 0; j < i - 1; j++)
				if (token_table[j]->operand[0][0] == '=')
				{
					flag = 0;
					//literal table�� �˻� �ִ� �͵��� flag �� 1�� �ٲ���
					for (k = 0; k < litnum; k++)
						if (strcmp(token_table[j]->operand[0], literal_table[k].lvalue) == 0)
							flag = 1;
					if (flag == 0) // flag���� 1�� �ƴ� �͵鸸 ó��
					{
						if (token_table[j]->operand[0][1] == 'C')
						{
							strcpy(literal_table[litnum].lvalue, token_table[j]->operand[0]);
							literal_table[litnum].addr = locctr;
							locctr = locctr + strlen(token_table[j]->operand[0]) - 4;
							litnum++;
						}
						else if (token_table[j]->operand[0][1] == 'X')
						{
							strcpy(literal_table[litnum].lvalue, token_table[j]->operand[0]);
							literal_table[litnum].addr = locctr;
							locctr = locctr + (strlen(token_table[j]->operand[0]) - 4) / 2;
							litnum++;
						}
					}
				}

			sectsize[sectnum] = locctr;
		}

		// location ���� �� label �˻� �� ���� ������ symtab�� ����
		if (strcmp(token_table[i]->label, "") != 0 && token_table[i]->label[0] != '.' && abcheck == 0)
		{
			strcpy(sym_table[symnum].symbol, token_table[i]->label);
			sym_table[symnum].addr = locctr;
			symnum++;
		}

	}

	return 0;
}


/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	/* add your code here */
	int i;
	FILE *write = NULL;
	if (file_name == NULL) {
		printf("-----------file �Է��� �������Ƿ� ǥ������մϴ�.-----------------\n");
		for (i = 0; i < token_line; i++)
			fprintf(stderr, "%s\t %s\t %s\t %s\n", token_table[i]->label
				, token_table[i]->operator
				, token_table[i]->operand[0]
				, token_table[i]->comment);
	}

	else {
		write = fopen(file_name, "w+");
		for (i = 0; i < token_line; i++) {
			fprintf(write, "%s\t %s\t %s\t %s\n", token_table[i]->label
				, token_table[i]->operator
				, token_table[i]->operand[0]
				, token_table[i]->comment);
		}
		fclose(write);
	}

}



/* --------------------------------------------------------------------------------*
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/

void make_symtab_output(char *file_name)
{
	
	int i;
	FILE *write = NULL;
	if (file_name == NULL) {
		printf("-----------file �Է��� �������Ƿ� ǥ������մϴ�.-----------------\n");
		for (i = 0; i < symnum; i++)
		{
			if (strcmp(sym_table[i].symbol, "") == 0)
				printf("\n");
			else
				printf("%6s            %X\n", sym_table[i].symbol, sym_table[i].addr);
		}
	}
	else {
		write = fopen(file_name, "w+");
		for (i = 0; i <symnum; i++)
		{
			if (strcmp(sym_table[i].symbol, "") == 0)
				fprintf(write,"\n");
			else
			fprintf(write, "%6s            %X\n", sym_table[i].symbol, sym_table[i].addr);
		}
		fclose(write);
	}
}


/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*         �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*         ������ ���� �۾��� ����Ǿ� ����.
*         1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
	int i, j, k, check4, opnum;
	sectnum = 0; //���° �������� ��������
	line_change = 0; //���ڵ� ������ �ٲ��ٶ� ���� flag
	code_line, code_num = 0;//���ڵ���� �� ���° column���� �˷���
	modify_number = 0; //modify�� ����ؾ� �� ������ ��

	//�����ϴµ� �־ ��ū�ϴµ� �ʿ��� ���ڿ� ���۵��� ����
	char *token;
	char *l_buffer;
	char *l_buffer2;
	char *l_buffer3;

	//����� �ڵ带 ���κ��� ���������� �м��Ѵ�.
	//���κ��� if���� ���� ��츦 ���� �� ��쿡 �˸��� �ӽ��ڵ带 �����Ѵ�.
	//�׸��� make_objectprogram �Լ��� ���� ���α׷��迭�� ����Ѵ�.
	for (i = 0; i < line_num; i++)
	{	

		// EXTREF ������ ���� ��ġ�� ��ġ�κ��� ��� ��Ʈ�� �˻��ؾ��ϴ��� 
		// operand�� extref�� ���Ͽ� modify_table �ۼ�(sect����)
		// 'WORD'�� ���� ���� ó���� �ȵǹǷ� ���߿� ���ǹ� �ȿ��� ó���� ����
		for (j = 0; j < 4; j++)
		{	
		
			//���� ��������� ���ٰ� �ν��ϸ� �ȵǹǷ� ����ִ� ��� ���� ó��
			if( strcmp(EXTREF[sectnum][j],"") != 0)
			{
				if (strcmp(token_table[i]->operand[0], EXTREF[sectnum][j]) == 0 && strcmp(token_table[i]->operator,"WORD") != 0)
				{	
				
					strcpy(modify_table[sectnum][modify_number].REFname, token_table[i]->operand[0]);
					modify_table[sectnum][modify_number].addr = locctr + 1;
					modify_table[sectnum][modify_number].number = 5;
					modify_number++;
				}
		
			}
		}

		/*����4���� token->comment�� opcode�� �־�����Ƿ� 0�� �ƴ϶�� ���� 
		2,3,4������ ���� �ӽ��ڵ带 ����ٴ� ���̴�.*/
		if (strcmp(token_table[i]->comment, "") != 0) 
		{
			if (token_table[i]->operator[0] == '+') //4�����Ͻ� �ϴ� + �����صд�
			{
				check4 = 1; //�׸��� ���߿� ������ 4�����̾����� �˼��ֵ��� �÷��� ����
				token_table[i]->operator = strtok(token_table[i]->operator,"+");
			}
			else
			{
				check4 = 0;
			}

			for (j = 0; j < inst_index; j++) //inst table�� ���ϸ鼭 ���� ã����
			{
				if (strcmp(token_table[i]->operator,inst_table[j]->mnemonic) == 0)
				{
					if (strcmp(inst_table[j]->format, "2") == 0) // 2���� �϶�
					{
						machine_code = malloc(4);
						format2(i, machine_code); //2�����϶� �ӽ��ڵ带 �����ϴ� �Լ��̴�
						locctr = locctr + 2;
					}
					else if (strcmp(inst_table[j]->format, "3/4") == 0 && check4 == 0) // 3���� �϶�
					{
						machine_code = malloc(6);
						format3(i, locctr, machine_code);//3�����϶� �ӽ��ڵ带 �����ϴ� �Լ��̴�
						locctr = locctr + 3;
					}
					else if (strcmp(inst_table[j]->format, "3/4") == 0 && check4 == 1) // 4���� �϶�
					{
						machine_code = malloc(8);
						format4(i, locctr, machine_code);//4�����϶� �ӽ��ڵ带 �����ϴ� �Լ��̴�
						locctr = locctr + 4;
					}
				}
			}
make_objectcode(machine_code);
		}
		//start �϶� H,��ƾ�̸�,�����ּ�,������ ���������� �̾��ش�
		if (strcmp(token_table[i]->operator,"START") == 0)
		{

			machine_code = malloc(6);
			locctr = atoi(token_table[i]->operand[0]);
			sprintf(machine_code, "H");
			make_objectcode(machine_code);
			sprintf(machine_code, "%-6s", token_table[i]->label);
			make_objectcode(machine_code);
			sprintf(machine_code, "%06X", locctr);
			start_loc = locctr;
			make_objectcode(machine_code);
			sprintf(machine_code, "%06X", sectsize[sectnum]);
			make_objectcode(machine_code);
			code_line++; code_num = 0;
		}

		//EXTDEF �϶� symtab�� �����ؼ� ��ġ�����ش�.
		if (strcmp(token_table[i]->operator, "EXTDEF") == 0)
		{
			machine_code = malloc(6);
			sprintf(machine_code, "D");
			make_objectcode(machine_code);
			opnum = 0;
			while (1) //operand ���� NULL ���� ���ö����� �ݺ����� ���鼭 extdef �� ã��
			{
				if (strcmp(token_table[i]->operand[opnum], "") != 0)
				{

					sprintf(machine_code, "%6s", token_table[i]->operand[opnum]);
					make_objectcode(machine_code);
					for (j = 0; j < symnum; j++)
					{
						if (strcmp(token_table[i]->operand[opnum], sym_table[j].symbol) == 0)
						{
							sprintf(machine_code, "%06X", sym_table[j].addr);
							make_objectcode(machine_code);
						}
					}
					opnum++;
				}
				else
				{
					code_line++;
					code_num = 0;
					break;
				}
			}

		}
		//���� ���� ���������� ������
		if (strcmp(token_table[i]->operator, "EXTREF") == 0)
		{
			modify_number = 0;
			machine_code = malloc(6);
			sprintf(machine_code, "R");
			make_objectcode(machine_code);
			opnum = 0;

			while (1)
			{
				if (strcmp(token_table[i]->operand[opnum], "") != 0)
				{
					strcpy(EXTREF[sectnum][opnum], token_table[i]->operand[opnum]);
					sprintf(machine_code, "%-6s", token_table[i]->operand[opnum]);
					make_objectcode(machine_code);
					opnum++;
				}
				else
				{
					code_line++;
					//���α׷� �˰��� �� T���ڵ��� ���̴� ���߿� �־�����ϹǷ�
					//column�����δ� �ִ� ������ ģ��. �׷��Ƿ� 2 ����
					code_num = 2;
					sprintf(machine_code, "T%06X", locctr);
					make_objectcode(machine_code);
					break;
				}
			}

		}
		//RESW�϶� �����̼� ����������
		if (strcmp(token_table[i]->operator, "RESW") == 0)
		{
			locctr = locctr + 3 * atoi(token_table[i]->operand[0]);
			line_change = 1;

		}
		//RESB�϶� �����̼� ����������
		if (strcmp(token_table[i]->operator, "RESB") == 0)
		{

			locctr = locctr + atoi(token_table[i]->operand[0]);
			line_change = 1;
		}

		//BASE�϶� int base�� �ּҰ� �Ҵ�
		if (strcmp(token_table[i]->operator, "BASE") == 0)
		{
			for (j = 0; j < symnum; j++)
				if (strcmp(token_table[i]->operand[0], sym_table[j].symbol) == 0)
					base = sym_table[j].addr;
		}
		/*LTORG�϶� literal table�󿡼� ã���༭ LTORG���� �ִ� =���鿡 ���ؼ���
		location�� ����������� �Ǵµ� ĳ���Ϳ� 16������ ǥ���� ��츦 ����� ó��*/
		if (strcmp(token_table[i]->operator, "LTORG") == 0)
		{	
			for (j = 0; j < i; j++)
				for (k = 0; k < litnum; k++)
					if (strcmp(token_table[j]->operand[0], literal_table[k].lvalue) == 0)
						literal_table[k].flag = 1;
			

			for (j = 0; j < litnum; j++)
				if (literal_table[j].flag == 1)
				{	
					l_buffer = malloc(10);
					l_buffer2 = malloc(10);
					l_buffer3 = malloc(10);
					strcpy(l_buffer, literal_table[j].lvalue);

					/*ĳ�������� ��� C,=,' �� �����ְ� ������ ���ڸ� 
					16���� �ƽ�Ű���� �ٲ㼭 ������Ʈ���α׷��� ���ִ� ����*/
					if (l_buffer[1] == 'C')
					{
						l_buffer = strtok(l_buffer, "=");
						l_buffer = strtok(l_buffer, "C");
						l_buffer = strtok(l_buffer, "'");
						for (k = 0; k < strlen(literal_table[j].lvalue) - 4; k++)
						{	
							sprintf(l_buffer2, "%X", l_buffer[k]);
							if(k==0)
								strcpy(l_buffer3, l_buffer2);
							else
								strcat(l_buffer3, l_buffer2);
						}
							
					}

					//16���� ���� X,=,' �� �����ְ� �״�� ������Ʈ���α׷��� ���ָ� ��
					else if (l_buffer[1] == 'X')
					{
						l_buffer = strtok(l_buffer, "=");
						l_buffer = strtok(l_buffer, "X");
						l_buffer = strtok(l_buffer, "'");
						strncpy(l_buffer3, l_buffer, strlen(l_buffer) - 1);
					}
				}
			
			sprintf(machine_code, l_buffer3);
			locctr = locctr + strlen(machine_code) / 2;
			make_objectcode(machine_code);
			
		}
		if (strcmp(token_table[i]->operator, "EQU") == 0);

		// BYTE������ ������Ʈ �ڵ带 �����ϴ� �κ�
		// ĳ���� Ÿ���� �ƽ�Ű �ڵ尪���� ����ؾ��ϰ�
		// 16���� Ÿ���� �׳� ��� �ϸ� �ȴ�. (���� LTORG�� ���� ���)
		if (strcmp(token_table[i]->operator, "BYTE") == 0)
		{	
			l_buffer = malloc(10);
			l_buffer2 = malloc(10);
			l_buffer3 = malloc(10);
			strcpy(l_buffer, token_table[i]->operand[0]);
			if (l_buffer[0] == 'C')
			{
				l_buffer = strtok(l_buffer, "=");
				l_buffer = strtok(l_buffer, "C");
				l_buffer = strtok(l_buffer, "'");
				for (k = 0; k < strlen(token_table[i]->operand[0]) - 3; k++)
				{
					sprintf(l_buffer2, "%X", l_buffer[k]);
					if (k == 0)
						strcpy(l_buffer3, l_buffer2);
					else
						strcat(l_buffer3, l_buffer2);
				}

			}
			else if (l_buffer[0] == 'X')
			{
				l_buffer = strtok(l_buffer, "X");
				l_buffer = strtok(l_buffer, "'");
				sprintf(l_buffer3,"%s", l_buffer);
			}
			sprintf(machine_code, l_buffer3);
			locctr = locctr + 1;
			make_objectcode(machine_code);
		}
		
		/*WORD�� ��� ���۷��� ���� '-'�������� �����ְ� 
		�տ��� ���Ѵ�� ó������ ���� modify_table�� ó������
		���ڰ��� EXREF�� ����Ǿ������� modify ���̺� ���ش�.*/
		if (strcmp(token_table[i]->operator, "WORD") == 0)
		{	
			char *ptr = strtok(token_table[i]->operand[0], "-");
			ptr = strtok(NULL, "-");
			strcat(token_table[i]->operand[1], "-");
			strcat(token_table[i]->operand[1], ptr);
			for (j = 0; j < 4; j++)
			{
				if (strcmp(token_table[i]->operand[0], EXTREF[sectnum][j]) == 0)
				{

					strcpy(modify_table[sectnum][modify_number].REFname, token_table[i]->operand[0]);
					modify_table[sectnum][modify_number].addr = locctr;
					modify_table[sectnum][modify_number].number = 6;
					modify_number++;
				}
			}
			for (j = 0; j < 4; j++)
			{
				if (strcmp(ptr, EXTREF[sectnum][j]) == 0)
				{	
					strcpy(modify_table[sectnum][modify_number].REFname, token_table[i]->operand[1]);
					modify_table[sectnum][modify_number].addr = locctr;
					modify_table[sectnum][modify_number].number = 6;
					modify_number++;
				}
			}
			//���� �����Ǿ����Ƿ� �ϴ� 000000���� ���ش�
			strcpy(machine_code, "000000");
			locctr += 3;
			make_objectcode(machine_code);
		}

		//END�� ��� LOTRG���� ó������ ���� ���ͷ� ó�� �� modify���ڵ� ����,END���ڵ� ����
		if (strcmp(token_table[i]->operator, "END") == 0)
		{	


			/* literal�� ó�����ִµ� �տ������� �˻縦 �����ؼ� ������ =''���� �˻��ؼ�
			 flag�� 1�� �ٲ��ش�. �׸��� LTORG�� ������ �� �ٽ� 1���� -1�� �ʱ�ȭ ��Ų��.
			 ��������� ���������� LTORG �Ŀ� ���Դ� literal ���鸸 ���α׷� ���� ���ش�.*/
			for (j = 0; j < i; j++)
			{
				for (k = 0; k < litnum; k++)
				{
					if (strcmp(token_table[j]->operand[0], literal_table[k].lvalue) == 0)
						literal_table[k].flag = 1;
				}
				if (strcmp(token_table[j]->operator, "LTORG") == 0)
					for (k = 0; k < litnum; k++)
						literal_table[k].flag = -1;
			}
			
			for (j = 0; j < litnum; j++)
				if (literal_table[j].flag == 1)
				{
					l_buffer = malloc(10);
					l_buffer2 = malloc(10);
					l_buffer3 = malloc(10);
					strcpy(l_buffer, literal_table[j].lvalue);
					if (l_buffer[1] == 'C')
					{
						l_buffer = strtok(l_buffer, "=");
						l_buffer = strtok(l_buffer, "C");
						l_buffer = strtok(l_buffer, "'");
						for (k = 0; k < strlen(literal_table[j].lvalue) - 4; k++)
						{
							sprintf(l_buffer2, "%X", l_buffer[k]);
							if (k == 0)
								strcpy(l_buffer3, l_buffer2);
							else
								strcat(l_buffer3, l_buffer2);
						}

					}
					else if (l_buffer[1] == 'X')
					{
						l_buffer = strtok(l_buffer, "=");
						l_buffer = strtok(l_buffer, "X");
						l_buffer = strtok(l_buffer, "'");
						strcpy(l_buffer3, l_buffer);
					}
				}
			sprintf(machine_code, l_buffer3);
			locctr = locctr + strlen(machine_code) / 2;
			make_objectcode(machine_code);
			
			//modify �� end���ڵ� ����
			end_flag = 1;
			make_objectcode(machine_code);
		}

		//CSECT�� ��� modify�� end���ڵ带 �������ش�
		if (strcmp(token_table[i]->operator, "CSECT") == 0)
		{
			end_flag = 1;
			make_objectcode(machine_code);
			sectnum++;
			modify_number = 0;
			locctr = 0;

			//���� ���� objectprogram�� HeadRecord �Է� �κ�
			sprintf(machine_code, "H");
			strcat(ob_code[code_line], machine_code);
			sprintf(machine_code, "%-6s", token_table[i]->label);
			strcat(ob_code[code_line], machine_code);
			sprintf(machine_code, "%06X", locctr);
			strcat(ob_code[code_line], machine_code);
			sprintf(machine_code, "%06X", sectsize[sectnum]);
			strcat(ob_code[code_line], machine_code);
			code_line++;
		}

	}
	/* add your code here */

}

///////////////////////////////////////////////////////////////////////////
// foramt2 �Լ�
// �Ű� : ���� �ѹ�, �ӽ��ڵ�������
// ��� : operator�� 2�����϶� ���γѹ����� �����Ǵ� �ӽ��ڵ� 
//        ���� �ӽ��ڵ� �����Ϳ� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////
void format2(int line_num, char *machine_code)
{
	int i, operandnum;
	operandnum = 0;
	for (i = 0; i < 4; i++) // operand ���� ���ϴ� ����
	{

		if (strcmp(token_table[line_num]->operand[i], "") == 0)
		{
			operandnum = i;
			i = 4;
		}
		if (operandnum == 1)// operand ������ 1�� �϶� ���� �ڵ� ���ϴ� ����
		{
			sprintf(machine_code, "%s%X0",
				token_table[line_num]->comment, RegisterToDecnum(token_table[line_num]->operand[0]));
		}
		else if (operandnum == 2)// operand ������ 2�� �϶� ���� �ڵ� ���ϴ� ����
		{
			sprintf(machine_code, "%s%X%X",
				token_table[line_num]->comment, RegisterToDecnum(token_table[line_num]->operand[0]), RegisterToDecnum(token_table[line_num]->operand[1]));
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////
// foramt3 �Լ�
// �Ű� : ���� �ѹ�, ������ �ּҰ� ,�ӽ��ڵ�������
// ��� : operator�� 3�����϶� ���γѹ����� �����Ǵ� �ӽ��ڵ� 
//        ���� �ӽ��ڵ� �����Ϳ� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////
void format3(int line_num, int location, char *machine_code)
{
	int nDisp, i,a,b;
	char *strDisp;
	token_table[line_num]->nixbpe = malloc(sizeof(char));
	token_table[line_num]->nixbpe = 0;
	if (token_table[line_num]->operand[1] == 'X') //index ���� ���� ��
	{
		a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + PC) >> 4;
		b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + PC) & 15;

		for (i = 0; i < symnum; i++)
		{
			if (strcmp(token_table[line_num]->operand[0], sym_table[i].symbol) == 0)
				nDisp = sym_table[i].addr - (locctr + 3);
		}

		if (nDisp < 0)
		{
			// nDisp�� �����ϰ��... nDisp 16���� ���� ���ڿ��� �ٲ۵�
			// �տ� 5�ڸ��� ���ʿ��ϰ� 'F'�� ä���� �����Ƿ� �����ϰ�
			// �տ� �����ϰ� ���� 3�ڸ��� ����ϰ� ����
			strDisp = (char *)malloc(sizeof(char) * 9);
			sprintf(strDisp, "%X", nDisp);
			strcpy(strDisp, &strDisp[5]);
			nDisp = CharToHexnum(strDisp);

			sprintf(machine_code, "%02X%X%s",
				CharToHexnum(token_table[line_num]->comment) + a, b, strDisp);
			free(strDisp);
		}
		//Base relative �� ����ؾ��Ҷ�
		else if (nDisp < -2048 && nDisp > 2048)
		{
			nDisp = sym_table[i].addr - base;
			a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + BASE) >> 4;
			b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + BASE) & 15;
			sprintf(machine_code, "%02X%X%03X",
				CharToHexnum(token_table[line_num]->comment) + a, b, nDisp);
		}
		else {
			sprintf(machine_code, "%02X%X%03X",
				CharToHexnum(token_table[line_num]->comment) + a, b, nDisp);
		}
	}

	else // index���� ���� ��
	{
		if (token_table[line_num]->operand[0][0] == '#') // immediate ó��
		{
			a = (token_table[line_num]->nixbpe | IMMEDIATE) >> 4;
			b = (token_table[line_num]->nixbpe | IMMEDIATE) & 15;

			token_table[line_num]->operand[0] = strtok(token_table[line_num]->operand[0], "#");
			nDisp = atoi(token_table[line_num]->operand[0]);
			sprintf(machine_code, "%02X%X%03X",
				CharToHexnum(token_table[line_num]->comment) +a, b, nDisp);
		}
		else if (token_table[line_num]->operand[0][0] == '=') // literal ó��
		{
			a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + PC) >> 4;
			b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + PC) & 15;

			token_table[line_num]->operand[0] = strtok(token_table[line_num]->operand[0], "@");
			for (i = 0; i < litnum; i++)
			{
				if (strcmp(token_table[line_num]->operand[0], literal_table[i].lvalue) == 0)
					nDisp = literal_table[i].addr - (locctr + 3);
			}
			sprintf(machine_code, "%02X%X%03X",
				CharToHexnum(token_table[line_num]->comment) + a, b, nDisp);
		}
		else if (token_table[line_num]->operand[0][0] == '@') // indirect ó��
		{
			a = (token_table[line_num]->nixbpe | INDIRECT + PC) >> 4;
			b = (token_table[line_num]->nixbpe | INDIRECT + PC) & 15;

			token_table[line_num]->operand[0] = strtok(token_table[line_num]->operand[0], "@");
			for (i = 0; i < symnum; i++)
			{
				if (strcmp(token_table[line_num]->operand[0], sym_table[i].symbol) == 0)
					nDisp = sym_table[i].addr - (locctr + 3);
			}

			if (nDisp < 0)
			{
				// nDisp�� �����ϰ��... nDisp 16���� ���� ���ڿ��� �ٲ۵�
				// �տ� 5�ڸ��� ���ʿ��ϰ� 'F'�� ä���� �����Ƿ� �����ϰ�
				// �տ� �����ϰ� ���� 3�ڸ��� ����ϰ� ����
				strDisp = (char *)malloc(sizeof(char) * 9);
				sprintf(strDisp, "%X", nDisp);
				strcpy(strDisp, &strDisp[5]);
				nDisp = CharToHexnum(strDisp);

				sprintf(machine_code, "%02X%X%s",
					CharToHexnum(token_table[line_num]->comment) + a, b, strDisp);
				free(strDisp);
			}
			else {
				sprintf(machine_code, "%02X%X%03X",
					CharToHexnum(token_table[line_num]->comment) + a, b, nDisp);
			}
		}
		else if (strcmp(token_table[line_num]->operand[0], " ") == 0) //operand ���� ���� �� ó��
		{
			a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE) >> 4;
			b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE) & 15;

			sprintf(machine_code, "%02X%X000",
				CharToHexnum(token_table[line_num]->comment) + a, b);
		}
		else // operand ���� �ִ� simple adressing ó��
		{
			a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + PC) >> 4;
			b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + PC) & 15;

			for (i = 0; i < symnum; i++)
			{
				if (strcmp(token_table[line_num]->operand[0], sym_table[i].symbol) == 0)
					nDisp = sym_table[i].addr - (locctr + 3);
			}

			if (nDisp < 0)
			{
				// nDisp�� �����ϰ��... nDisp 16���� ���� ���ڿ��� �ٲ۵�
				// �տ� 5�ڸ��� ���ʿ��ϰ� 'F'�� ä���� �����Ƿ� �����ϰ�
				// �տ� �����ϰ� ���� 3�ڸ��� ����ϰ� ����
				strDisp = (char *)malloc(sizeof(char) * 9);
				sprintf(strDisp, "%X", nDisp);
				strcpy(strDisp, &strDisp[5]);
				nDisp = CharToHexnum(strDisp);

				sprintf(machine_code, "%02X%X%s",
					CharToHexnum(token_table[line_num]->comment) + a,b, strDisp);
				free(strDisp);
			}
			//Base relative �� ����ؾ��Ҷ�
			else if (nDisp < -2048 && nDisp > 2048)
			{
				nDisp = sym_table[i].addr - base;
				a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + BASE) >> 4;
				b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + BASE) & 15;
				sprintf(machine_code, "%02X%X%03X",
					CharToHexnum(token_table[line_num]->comment) + a, b, nDisp);
			}
			else {
				sprintf(machine_code, "%02X%X%03X",
					CharToHexnum(token_table[line_num]->comment) + a, b, nDisp);
			}

		}
	}

}

///////////////////////////////////////////////////////////////////////////
// foramt4 �Լ�
// �Ű� : ���� �ѹ�,������ �ּҰ�, �ӽ��ڵ�������
// ��� : operator�� 4�����϶� ���γѹ����� �����Ǵ� �ӽ��ڵ� 
//        ���� �ӽ��ڵ� �����Ϳ� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////
void format4(int line_num, int location, char *machine_code)
{
	int nDisp, i,a,b;
	token_table[line_num]->nixbpe = malloc(sizeof(char));
	token_table[line_num]->nixbpe = 0;
	//IMMEDIATE�� ����� 4������ ���
	if (token_table[line_num]->operand[0][0] == '#')
	{
		
		a = (token_table[line_num]->nixbpe | IMMEDIATE + EXTENT) >> 4;
		b = (token_table[line_num]->nixbpe | IMMEDIATE + EXTENT) & 15;

		token_table[line_num]->operand[0] = strtok(token_table[line_num]->operand[0], "#");
		nDisp = atoi(token_table[line_num]->operand[0]);
		sprintf(machine_code, "%02X%01X%05X",
			CharToHexnum(token_table[line_num]->comment) + a, b, nDisp);
	}
	//operand�� �˻��ؼ� INDEX ���� �����ϴ� ��� 
	if (strcmp(token_table[line_num]->operand[1], "X") == 0)
	{
		a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + EXTENT) >> 4;
		b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + EXTENT) & 15;

		sprintf(machine_code, "%02X%01X00000",
			CharToHexnum(token_table[line_num]->comment) + a, b);
	}
	// INDEX ���� ���� ���
	else
	{
		a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + EXTENT) >> 4;
		b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + EXTENT) & 15;
		sprintf(machine_code, "%02X%01X00000",
			CharToHexnum(token_table[line_num]->comment) + a, b);
	}
}


///////////////////////////////////////////////////////////////////////////
// CharToHexnum �Լ�
// �Ű� : ���ڿ�(16����) �� �޾ƿͼ�
// ��� : ������ ��ȯ �� �����Ѵ�.
/////////////////////////////////////////////////////////////////////////
int CharToHexnum(char *opcode)
{
	int nSum = 0, loop, len;
	double x = 16;
	len = (int)strlen(opcode) - 1;
	// �ڸ��� ��ŭ ������ ����
	for (loop = 0; loop < len + 1; loop++)
	{
		if (('0' <= opcode[loop]) && (opcode[loop] <= '9'))
		{
			// ���� 16������ 0~9 ������ �����̸�

			nSum += (int)(pow(x, (len - loop)) * (opcode[loop] - '0'));
		}
		else if (('A' <= opcode[loop]) && (opcode[loop] <= 'F'))
		{
			// ���� A~F������ �����̸�

			nSum += (int)(pow(x, (len - loop)) * (opcode[loop] - 'A' + 10));
		}

	}

	// 10���� ����� ��ȯ
	return nSum;
}
//////////////////////////////////////////////////////////////////////////
// RegisterToDecnum �Լ�
// cReg : Register ĳ���� ���ڸ� �޾ƿ��� �κ�
// ��� : Register ĳ���� ���ڸ� �޾ƿͼ� �����ϴ� ���ڷ� �������ִ� �Լ�
////////////////////////////////////////////////////////////////////////
int RegisterToDecnum(char *strReg)
{
	if (strcmp(strReg, "A") == 0) return 0;
	else if (strcmp(strReg, "X") == 0) return 1;
	else if (strcmp(strReg, "L") == 0) return 2;
	else if (strcmp(strReg, "B") == 0) return 3;	
	else if (strcmp(strReg, "S") == 0) return 4;
	else if (strcmp(strReg, "T") == 0) return 5;
	else if (strcmp(strReg, "F") == 0) return 6;
	else if (strcmp(strReg, "PC") == 0) return 8;
	else if (strcmp(strReg, "SW") == 0) return 9;
	else
	{
		// ���⿡ ����ġ ���� Register�� �ð�� ���� ó���������
		printf("error : Register Not found!!!\n");
		return -1;
	}
}



///////////////////////////////////////////////////////////////
// make_objcetcode �Լ�
// ��� : machine_code�� ���ڷ� �޾Ƽ� ��쿡 ���� 
//        object���α׷��� ���ش�.
///////////////////////////////////////////////////////////////
void make_objectcode(char *machine_code)
{
	int i, j;
	code_num += strlen(machine_code);


	//CSECT�� END�� �������� ModifyRecord, EndRecord obejctcodef�� ���ش� 
	if (end_flag == 1)
	{	
		line_change = 0;
		machine_code2 = malloc(7);
		code_line++;

		sprintf(machine_code2, "%02X", (code_num - 9 - strlen(machine_code)) / 2);
		insert_length(ob_code[code_line - 1], machine_code2);

		
		for (i = 0; i < modify_number; i++)
		{	
			// modify_table�� �����ؼ� ���� RXTREF���� ������ϴ°��
			if (modify_table[sectnum][i].REFname[0] == '-')
			{
				sprintf(machine_code, "M%06X%02d%s", modify_table[sectnum][i].addr,
					modify_table[sectnum][i].number,
					modify_table[sectnum][i].REFname);
				strcat(ob_code[code_line], machine_code);
				code_line++;
			}
			// RXTREF���� �����ִ� ���
			else{
				sprintf(machine_code, "M%06X%02d+%s", modify_table[sectnum][i].addr,
					modify_table[sectnum][i].number,
					modify_table[sectnum][i].REFname);
				strcat(ob_code[code_line], machine_code);
				code_line++;
			}

		}
		//ù��° ������ END���ڵ忡�� �����ּҸ� ���ش�.
		if (sectnum == 0) {
			sprintf(machine_code, "%06X", start_loc);
			strcat(ob_code[code_line], "E");
			strcat(ob_code[code_line], machine_code);
		}
		else
			strcat(ob_code[code_line], "E");
		code_line += 2;
		end_flag = 0;
		return;
	}

	// �������� ���� location���� ���� TextRecord���� �ٲ��ִ� ���
	if (line_change == 1)
	{
		machine_code2 = malloc(7);
		code_line++;

		sprintf(machine_code2, "T%06X", locctr - strlen(machine_code) / 2);
		strcat(ob_code[code_line], machine_code2);

		sprintf(machine_code2, "%02X", (code_num - 9 - strlen(machine_code)) / 2);
		insert_length(ob_code[code_line - 1], machine_code2);
		code_num = 9 + strlen(machine_code);
		line_change = 0;
	}

	//column�� ���� ���� TextRecord���� �ٲ��ִ� ���
	if (code_num > 69)
	{
		machine_code2 = malloc(7);
		code_line++;
		sprintf(machine_code2, "T%06X", locctr - strlen(machine_code)/2);
		strcat(ob_code[code_line], machine_code2);


		sprintf(machine_code2, "%02X", (code_num - 9 - strlen(machine_code)) / 2);
		insert_length(ob_code[code_line - 1], machine_code2);
		code_num = 9 + strlen(machine_code);
	}
	strcat(ob_code[code_line], machine_code);

}

///////////////////////////////////////////////////////////////
// insert_length �Լ�
// ��� : TextRecord�� ���� ���ų� ������ �Ѿ�� �Ҷ� ���ڵ��� ���̸� �о ���ڵ� 
//        ���ʿ� �ִ� ������ġ �ٷ� �ڿ� ���ش� 
///////////////////////////////////////////////////////////////
void insert_length(char *object_code, char *length)
{
	int i;
	char buffer1[61];
	char buffer2[8];
	char buffer3[3];

	//����2���� �տ� 7�ڸ� ���ڵ� �̸��� ��ġ ����
	strncpy(buffer2, object_code, 7);
	buffer2[sizeof(buffer2) - 1] = '\0';

	//����3���� ���̸� ����
	strcpy(buffer3, length);

	//����1���� ���� ����
	strncpy(buffer1, &object_code[7], 60);

	//���ڵ� �̸�, ��ġ, ����. ���� ������ �̾��ش�.
	strcpy(object_code, buffer2);
	strcat(object_code, buffer3);
	strcat(object_code, buffer1);

}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */
	int i;
	FILE *write = NULL;
	if (file_name == NULL) {
		printf("-----------file �Է��� �������Ƿ� ǥ������մϴ�.-----------------\n");
		for (i = 0; i < code_line + 1; i++)
		{
			fprintf(stderr,"%s\n", ob_code[i]);	
		}
	}

	else {
		write = fopen(file_name, "w+");
		for (i = 0; i < code_line + 1; i++)
		{
			fprintf(write,"%s\n", ob_code[i]);
		}
		fclose(write);
	}
}