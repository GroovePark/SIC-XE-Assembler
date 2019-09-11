/*
* 화일명 : my_assembler_00000000.c
* 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
* 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
* 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
*/

/*
*
* 프로그램의 헤더를 정의한다.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

// 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20142362.h"

/* ----------------------------------------------------------------------------------
* 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
* 매계 : 실행 파일, 어셈블리 파일
* 반환 : 성공 = 0, 실패 = < 0
* 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
*         또한 중간파일을 생성하지 않는다.
* ----------------------------------------------------------------------------------
*/
int main(int args, char *argv[])
{
	int i;
	if (init_my_assembler()< 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0) {
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}

	//make_opcode_output(argv[1]);
	make_symtab_output("symtab_20142362.txt");

	if (assem_pass2() < 0) {
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
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
* 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
* 매계 : 없음
* 반환 : 정상종료 = 0 , 에러 발생 = -1
* 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
*         위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
*         구현하였다.
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
* 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
*        생성하는 함수이다.
* 매계 : 기계어 목록 파일
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
*
*   ===============================================================================
*         | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
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

		/******************instruction set 입력받고 inst.dat에 이진저장***********
		scanf("%s %s %s %s", inst_table[i]->mnemonic, inst_table[i]->format,
		inst_table[i]->opcode,inst_table[i]->operandnum);
		fwrite(inst_table[i]->mnemonic, 7, 1, file);
		fwrite(inst_table[i]->format, 5, 1, file);
		fwrite(inst_table[i]->opcode, 5, 1, file);
		fwrite(inst_table[i]->operandnum, 2, 1, file);
		*/
	}


	for (i = 0; i < inst_index; i++) {
		//inst.dat에 있는 데이터를 읽어서 inst_table에 저장
		fread(inst_table[i]->mnemonic, 7, 1, file);
		fread(inst_table[i]->format, 5, 1, file);
		fread(inst_table[i]->opcode, 5, 1, file);
		fread(inst_table[i]->operandnum, 2, 1, file);

		/**** inst_table이 잘 저장됬는지 확인하위한 출력 코드******
		printf("inst_table : %s %s %s %s\n", inst_table[i]->mnemonic,
		inst_table[i]->format,
		inst_table[i]->opcode,
		inst_table[i]->operandnum);
		//*/
	}

	return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다.
* 매계 : 어셈블리할 소스파일명
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : 라인단위로 저장한다.
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

		//소스코드테이블(input_data)가 잘 저장됬는지 확인하기 위한 출력코드
		//printf("%s", input_data[i]); 

	}
	return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
*        패스 1로 부터 호출된다.
* 매계 : 파싱을 원하는 문자열
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
* ----------------------------------------------------------------------------------
*/
int token_parsing(char *str)
{

	/* add your code here */
	int check;
	//..............subrountin jump 하는 구간..................
	if (str[0] == ' ') {
		return -1; // tab으로 구분하였기 때문에 소스파일 처음에 공백이 있을 시 인식하지 못한다.
	}
	if (str[0] == '.') {
		//2줄위의 token_table label 값이 RDREC면 TO READ RECORD 출력//
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
		//2줄위의 token_table label 값이 RDREC면 TO WRITE RECORD 출력//
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

	//Label값이 비어있는 구간
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
		//마지막 토큰의 개행문자를 없애주고 비어있는 토큰에는 공백을 넣는다.
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
		/*****************디버깅 하기 위한 중간 출력 코드***************************
		printf("token table label[%d] = %s\n", token_line, token_table[token_line]->label);
		printf("token table operator[%d] = %s\n", token_line, token_table[token_line]->operator);
		printf("token table operand[%d] = %s\n", token_line, token_table[token_line]->operand[0]);
		printf("token table comment[%d] = %s\n", token_line, token_table[token_line]->comment);
		printf("-------------------------------------------------\n");
		//*/
	}

	//Label 값 존재 
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

			//마지막 토큰의 개행문자를 없애주고 비어있는 토큰에는 공백을 넣는다.
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

		/*****************디버깅 하기 위한 중간 출력 코드***************************
		printf("token table label[%d] = %s\n", token_line, token_table[token_line]->label);
		printf("token table operator[%d] = %s\n", token_line, token_table[token_line]->operator);
		printf("token table operand[%d] = %s\n", token_line, token_table[token_line]->operand[0]);
		printf("token table comment[%d] = %s\n", token_line, token_table[token_line]->comment);
		printf("--------------------------------------------------\n");
		//*/

	}
	// operand 배열에 할당  0,1,2,3 이렇게 4개의 배열을 할당해줌
	token_table[token_line]->operand[1] = malloc(1);
	strcpy(token_table[token_line]->operand[1], "");
	token_table[token_line]->operand[2] = malloc(1);
	strcpy(token_table[token_line]->operand[2], "");
	token_table[token_line]->operand[3] = malloc(1);
	strcpy(token_table[token_line]->operand[3], "");

	//operand[0]을 분할 해서 operand배열에 넣어줌
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
* 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 문자열
* 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
* 주의 :
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
	if (strcmp(str, "1") < 0) { //4형식인 값을 입력받았을때 inst_table에 '+'를 추가해서 비교한다.
		for (i = 0; i < inst_index; i++) {
			char e_1[8] = "+";
			strcat(e_1, inst_table[i]->mnemonic);
			if (strcmp(str, e_1) == 0) {

				//기계어코드가 있는 라인 중 4형식이 어디어디인지 알려주는 출력코드이다.
				//printf("line %d = %s\n", token_line, str);

				strcpy(token_table[token_line]->comment, inst_table[i]->opcode);
				check = 1; //입력문자열이 기계어코드임을 의미
			}
		}
	}

	for (i = 0; i < inst_index; i++) {

		if (strcmp(str, inst_table[i]->mnemonic) == 0) {
			//기계어코드가 있는 라인이 어디인지 알려주는 출력코드이다.
			//printf("line %d = %s\n", token_line, str); //
			strcpy(token_table[token_line]->comment, inst_table[i]->opcode);
			check = 1;
		}
	}
	if (check == 0)//입력문자열이 기계어코드가 아닐때 comment에 공백넣어줌
		strcpy(token_table[token_line]->comment, "");
	return 0;
}



/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*         패스1에서는..
*         1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*         테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*     따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */
	/* input_data의 문자열을 한줄씩 입력 받아서
	* token_parsing()을 호출하여 token_unit에 저장
	*/
	int i, j, k, start, flag, abcheck;
	symnum = 0; // symbol table 크기
	litnum = 0; // literal table 크기
	token_line = 0;

	//토컨 파싱 및 opcode 매칭
	for (i = 0; i < line_num; i++)
		token_table[i] = malloc(sizeof(token));
	for (i = 0; i < line_num; i++)
	{
		if (input_data[i] == NULL)
			return -1;
		if (token_parsing(input_data[i]) == -1) {
			printf("source code 시작 부분 공백확인\n");
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


	//symbol table 구성 및 literal 선언 등//
	for (i = 0; i < line_num; i++)
	{	
		

		abcheck = 0; // absolute expression 시 symbol table에 이중 저장 방지 위함
		flag = 0; // literal table에 값이 이중 저장되는 것을 방지하기 위함

		//operator "START" 일때
		if (strcmp(token_table[i]->operator, "START") == 0)
		{
			start = atoi(token_table[i]->operand[0]);
			locctr = start;
		}
		//operator "RESW" 일때
		else if (strcmp(token_table[i - 1]->operator, "RESW") == 0)	
			locctr = locctr + 3 * atoi(token_table[i - 1]->operand[0]);
		//operator "RESB" 일때
		else if (strcmp((token_table[i - 1]->operator), "RESB") == 0)
			locctr = locctr + atoi(token_table[i - 1]->operand[0]);
		//operator "4형식" 일때
		else if (token_table[i - 1]->operator[0] == '+')
			locctr = locctr + 4;
		//operator "LTORG" 일때
		else if (strcmp(token_table[i - 1]->operator, "LTORG") == 0)
		{
			// LTORG 전에 '='값이 있는 것들을 찾아서 literal_table에 넣어준다.
			for (j = 0, flag = 0; j < i - 1; j++)
				if (token_table[j]->operand[0][0] == '=')
				{
					flag = 0;
					//literal table을 검사해서 값이 중복되지 않도록 한다
					for (k = 0; k < litnum; k++)
						if (strcmp(token_table[j]->operand[0], literal_table[k].lvalue))
							flag = 1;
					if (flag == 0)
					{	
						// 캐릭터 값인 경우
						if (token_table[j]->operand[0][1] == 'C')
						{
							strcpy(literal_table[litnum].lvalue, token_table[j]->operand[0]);
							literal_table[litnum].addr = locctr;
							//-4를 해주는 이유는 =,C,',' 이렇게 4개의 문자 제외하기 때문
							locctr = locctr + strlen(token_table[j]->operand[0]) - 4;
							litnum++;
						}
						// 16진수 값인 경우 
						else if (token_table[j]->operand[0][1] == 'X')
						{
							strcpy(literal_table[litnum].lvalue, token_table[j]->operand[0]);
							literal_table[litnum].addr = locctr;
							//마찬가지로 4빼주고 16진수는 2문자당 1바이트이므로 2로 나눠줌
							locctr = locctr + (strlen(token_table[j]->operand[0]) - 4) / 2;
							litnum++;
						}
					}
				}
		}
		//operator "BYTE" 일때
		else if (strcmp(token_table[i - 1]->operator,"BYTE") == 0)
			locctr = locctr + 1;
		//operator "WORD" 일때
		else if (strcmp(token_table[i - 1]->operator,"WORD") == 0)
			locctr = locctr + 3;

		// 그외의 2형식과 3/4형식인데 4형식은 위에서 처리했으므로 3/4는 곧 3형식이다
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
		//operator "CSECT" 일때 
		if (strcmp(token_table[i]->operator,"CSECT") == 0)
		{
			//각각의 섹터크기 저장
			sectsize[sectnum] = locctr;
			locctr = 0;
			sectnum++;
			//sector가 바뀌었으므로 공백을 한칸주기 위함
			strcpy(sym_table[symnum].symbol, "");
			symnum++;
		}

		//EQU인 경우 '-'기준으로 오퍼랜드를 나누어서 각자의 오퍼랜드 값을 계산 빼준다
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
			// absolute expression 한 값을 symbol table에 넣어줌 
			// 이중으로 저장되는 것을 막기 위해 abcheck값 1로 세팅
			strcpy(sym_table[symnum].symbol, token_table[i]->label);
			sym_table[symnum].addr = num1 - num2;
			symnum++;
			abcheck = 1;
		}

		/*END인 경우 처음부터 루프를 돌면서 '='인 것들을 찾아주고 이것들중
		literal_table에 없는 것들을 테이블에 넣어준다.
		(literal_table에 있는 것은 LTORG에서 이미 처리*/
		else if (strcmp(token_table[i]->operator,"END") == 0)
		{
			for (j = 0; j < i - 1; j++)
				if (token_table[j]->operand[0][0] == '=')
				{
					flag = 0;
					//literal table을 검사 있는 것들은 flag 값 1로 바꿔줌
					for (k = 0; k < litnum; k++)
						if (strcmp(token_table[j]->operand[0], literal_table[k].lvalue) == 0)
							flag = 1;
					if (flag == 0) // flag값이 1이 아닌 것들만 처리
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

		// location 증가 후 label 검사 후 값이 있으면 symtab에 저장
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
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	/* add your code here */
	int i;
	FILE *write = NULL;
	if (file_name == NULL) {
		printf("-----------file 입력을 안했으므로 표준출력합니다.-----------------\n");
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
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/

void make_symtab_output(char *file_name)
{
	
	int i;
	FILE *write = NULL;
	if (file_name == NULL) {
		printf("-----------file 입력을 안했으므로 표준출력합니다.-----------------\n");
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
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*         패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*         다음과 같은 작업이 수행되어 진다.
*         1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
	int i, j, k, check4, opnum;
	sectnum = 0; //몇번째 섹터인지 가르쳐줌
	line_change = 0; //레코드 라인을 바꿔줄때 쓰는 flag
	code_line, code_num = 0;//레코드라인 및 몇번째 column인지 알려줌
	modify_number = 0; //modify에 기록해야 할 데이터 수

	//저장하는데 있어서 토큰하는데 필요한 문자열 버퍼들을 선언
	char *token;
	char *l_buffer;
	char *l_buffer2;
	char *l_buffer3;

	//어셈블리 코드를 라인별로 순차적으로 분석한다.
	//라인별로 if문을 통해 경우를 설정 각 경우에 알맞은 머신코드를 생성한다.
	//그리고 make_objectprogram 함수를 통해 프로그램배열에 기록한다.
	for (i = 0; i < line_num; i++)
	{	

		// EXTREF 참조한 값의 위치와 위치로부터 몇개의 비트를 검사해야하는지 
		// operand와 extref을 비교하여 modify_table 작성(sect별로)
		// 'WORD'인 경우는 지금 처리가 안되므로 나중에 조건문 안에서 처리할 예정
		for (j = 0; j < 4; j++)
		{	
		
			//서로 비어있을때 같다고 인식하면 안되므로 비어있는 경우 예외 처리
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

		/*과제4에서 token->comment에 opcode를 넣어놨으므로 0이 아니라는 것은 
		2,3,4형식을 통해 머신코드를 만든다는 뜻이다.*/
		if (strcmp(token_table[i]->comment, "") != 0) 
		{
			if (token_table[i]->operator[0] == '+') //4형식일시 일단 + 제거해둔다
			{
				check4 = 1; //그리고 나중에 봤을때 4형식이었음을 알수있도록 플래그 설정
				token_table[i]->operator = strtok(token_table[i]->operator,"+");
			}
			else
			{
				check4 = 0;
			}

			for (j = 0; j < inst_index; j++) //inst table과 비교하면서 형식 찾아줌
			{
				if (strcmp(token_table[i]->operator,inst_table[j]->mnemonic) == 0)
				{
					if (strcmp(inst_table[j]->format, "2") == 0) // 2형식 일때
					{
						machine_code = malloc(4);
						format2(i, machine_code); //2형식일때 머신코드를 생성하는 함수이다
						locctr = locctr + 2;
					}
					else if (strcmp(inst_table[j]->format, "3/4") == 0 && check4 == 0) // 3형식 일때
					{
						machine_code = malloc(6);
						format3(i, locctr, machine_code);//3형식일때 머신코드를 생성하는 함수이다
						locctr = locctr + 3;
					}
					else if (strcmp(inst_table[j]->format, "3/4") == 0 && check4 == 1) // 4형식 일때
					{
						machine_code = malloc(8);
						format4(i, locctr, machine_code);//4형식일때 머신코드를 생성하는 함수이다
						locctr = locctr + 4;
					}
				}
			}
make_objectcode(machine_code);
		}
		//start 일때 H,루틴이름,시작주소,사이즈 순차적으로 이어준다
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

		//EXTDEF 일때 symtab을 참고해서 위치구해준다.
		if (strcmp(token_table[i]->operator, "EXTDEF") == 0)
		{
			machine_code = malloc(6);
			sprintf(machine_code, "D");
			make_objectcode(machine_code);
			opnum = 0;
			while (1) //operand 값이 NULL 값이 나올때까지 반복문을 돌면서 extdef 값 찾음
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
		//위에 경우와 마찬가지로 구해줌
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
					//프로그램 알고리즘 상 T레코드의 길이는 나중에 넣어줘야하므로
					//column상으로는 있는 셈으로 친다. 그러므로 2 설정
					code_num = 2;
					sprintf(machine_code, "T%06X", locctr);
					make_objectcode(machine_code);
					break;
				}
			}

		}
		//RESW일때 로케이션 증가시켜줌
		if (strcmp(token_table[i]->operator, "RESW") == 0)
		{
			locctr = locctr + 3 * atoi(token_table[i]->operand[0]);
			line_change = 1;

		}
		//RESB일때 로케이션 증가시켜줌
		if (strcmp(token_table[i]->operator, "RESB") == 0)
		{

			locctr = locctr + atoi(token_table[i]->operand[0]);
			line_change = 1;
		}

		//BASE일때 int base에 주소값 할당
		if (strcmp(token_table[i]->operator, "BASE") == 0)
		{
			for (j = 0; j < symnum; j++)
				if (strcmp(token_table[i]->operand[0], sym_table[j].symbol) == 0)
					base = sym_table[j].addr;
		}
		/*LTORG일때 literal table상에서 찾아줘서 LTORG전에 있는 =값들에 대해서는
		location을 증가시켜줘야 되는데 캐릭터와 16진수로 표현된 경우를 나누어서 처리*/
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

					/*캐릭터형인 경우 C,=,' 을 없애주고 나머지 문자를 
					16진수 아스키형을 바꿔서 오브젝트프로그램에 써주는 과정*/
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

					//16진수 형은 X,=,' 을 없애주고 그대로 오브젝트프로그램에 써주면 됨
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

		// BYTE선언의 오브젝트 코드를 생성하는 부분
		// 캐릭터 타입은 아스키 코드값으로 출력해야하고
		// 16진수 타입은 그냥 출력 하면 된다. (위의 LTORG와 같은 기법)
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
		
		/*WORD인 경우 오퍼랜드 값을 '-'기준으로 나눠주고 
		앞에서 말한대로 처리되지 않은 modify_table을 처리해줌
		각자값이 EXREF에 선언되어있으면 modify 테이블에 써준다.*/
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
			//값이 참조되었으므로 일단 000000으로 써준다
			strcpy(machine_code, "000000");
			locctr += 3;
			make_objectcode(machine_code);
		}

		//END인 경우 LOTRG에서 처리되지 않은 리터럴 처리 및 modify레코드 생성,END레코드 생성
		if (strcmp(token_table[i]->operator, "END") == 0)
		{	


			/* literal를 처리해주는데 앞에서부터 검사를 시작해서 나오는 =''값을 검사해서
			 flag를 1로 바꿔준다. 그리고 LTORG를 만났을 시 다시 1값을 -1로 초기화 시킨다.
			 결과적으로 마지막에는 LTORG 후에 나왔던 literal 값들만 프로그램 끝에 써준다.*/
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
			
			//modify 및 end레코드 생성
			end_flag = 1;
			make_objectcode(machine_code);
		}

		//CSECT인 경우 modify와 end레코드를 생성해준다
		if (strcmp(token_table[i]->operator, "CSECT") == 0)
		{
			end_flag = 1;
			make_objectcode(machine_code);
			sectnum++;
			modify_number = 0;
			locctr = 0;

			//다음 섹터 objectprogram의 HeadRecord 입력 부분
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
// foramt2 함수
// 매계 : 라인 넘버, 머신코드포인터
// 기능 : operator가 2형식일때 라인넘버에서 생성되는 머신코드 
//        값을 머신코드 포인터에 저장한다.
/////////////////////////////////////////////////////////////////////////
void format2(int line_num, char *machine_code)
{
	int i, operandnum;
	operandnum = 0;
	for (i = 0; i < 4; i++) // operand 갯수 구하는 과정
	{

		if (strcmp(token_table[line_num]->operand[i], "") == 0)
		{
			operandnum = i;
			i = 4;
		}
		if (operandnum == 1)// operand 갯수가 1개 일때 기계어 코드 구하는 과정
		{
			sprintf(machine_code, "%s%X0",
				token_table[line_num]->comment, RegisterToDecnum(token_table[line_num]->operand[0]));
		}
		else if (operandnum == 2)// operand 갯수가 2개 일때 기계어 코드 구하는 과정
		{
			sprintf(machine_code, "%s%X%X",
				token_table[line_num]->comment, RegisterToDecnum(token_table[line_num]->operand[0]), RegisterToDecnum(token_table[line_num]->operand[1]));
		}
	}
	return;
}
///////////////////////////////////////////////////////////////////////////
// foramt3 함수
// 매계 : 라인 넘버, 라인의 주소값 ,머신코드포인터
// 기능 : operator가 3형식일때 라인넘버에서 생성되는 머신코드 
//        값을 머신코드 포인터에 저장한다.
/////////////////////////////////////////////////////////////////////////
void format3(int line_num, int location, char *machine_code)
{
	int nDisp, i,a,b;
	char *strDisp;
	token_table[line_num]->nixbpe = malloc(sizeof(char));
	token_table[line_num]->nixbpe = 0;
	if (token_table[line_num]->operand[1] == 'X') //index 값이 있을 때
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
			// nDisp가 음수일경우... nDisp 16진수 값을 문자열로 바꾼뒤
			// 앞에 5자리가 불필요하게 'F'로 채워져 나오므로 제거하고
			// 앞에 제거하고 남은 3자리만 출력하게 해줌
			strDisp = (char *)malloc(sizeof(char) * 9);
			sprintf(strDisp, "%X", nDisp);
			strcpy(strDisp, &strDisp[5]);
			nDisp = CharToHexnum(strDisp);

			sprintf(machine_code, "%02X%X%s",
				CharToHexnum(token_table[line_num]->comment) + a, b, strDisp);
			free(strDisp);
		}
		//Base relative 를 사용해야할때
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

	else // index값이 없을 때
	{
		if (token_table[line_num]->operand[0][0] == '#') // immediate 처리
		{
			a = (token_table[line_num]->nixbpe | IMMEDIATE) >> 4;
			b = (token_table[line_num]->nixbpe | IMMEDIATE) & 15;

			token_table[line_num]->operand[0] = strtok(token_table[line_num]->operand[0], "#");
			nDisp = atoi(token_table[line_num]->operand[0]);
			sprintf(machine_code, "%02X%X%03X",
				CharToHexnum(token_table[line_num]->comment) +a, b, nDisp);
		}
		else if (token_table[line_num]->operand[0][0] == '=') // literal 처리
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
		else if (token_table[line_num]->operand[0][0] == '@') // indirect 처리
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
				// nDisp가 음수일경우... nDisp 16진수 값을 문자열로 바꾼뒤
				// 앞에 5자리가 불필요하게 'F'로 채워져 나오므로 제거하고
				// 앞에 제거하고 남은 3자리만 출력하게 해줌
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
		else if (strcmp(token_table[line_num]->operand[0], " ") == 0) //operand 값이 없을 때 처리
		{
			a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE) >> 4;
			b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE) & 15;

			sprintf(machine_code, "%02X%X000",
				CharToHexnum(token_table[line_num]->comment) + a, b);
		}
		else // operand 값이 있는 simple adressing 처리
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
				// nDisp가 음수일경우... nDisp 16진수 값을 문자열로 바꾼뒤
				// 앞에 5자리가 불필요하게 'F'로 채워져 나오므로 제거하고
				// 앞에 제거하고 남은 3자리만 출력하게 해줌
				strDisp = (char *)malloc(sizeof(char) * 9);
				sprintf(strDisp, "%X", nDisp);
				strcpy(strDisp, &strDisp[5]);
				nDisp = CharToHexnum(strDisp);

				sprintf(machine_code, "%02X%X%s",
					CharToHexnum(token_table[line_num]->comment) + a,b, strDisp);
				free(strDisp);
			}
			//Base relative 를 사용해야할때
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
// foramt4 함수
// 매계 : 라인 넘버,라인의 주소값, 머신코드포인터
// 기능 : operator가 4형식일때 라인넘버에서 생성되는 머신코드 
//        값을 머신코드 포인터에 저장한다.
/////////////////////////////////////////////////////////////////////////
void format4(int line_num, int location, char *machine_code)
{
	int nDisp, i,a,b;
	token_table[line_num]->nixbpe = malloc(sizeof(char));
	token_table[line_num]->nixbpe = 0;
	//IMMEDIATE를 사용한 4형식인 경우
	if (token_table[line_num]->operand[0][0] == '#')
	{
		
		a = (token_table[line_num]->nixbpe | IMMEDIATE + EXTENT) >> 4;
		b = (token_table[line_num]->nixbpe | IMMEDIATE + EXTENT) & 15;

		token_table[line_num]->operand[0] = strtok(token_table[line_num]->operand[0], "#");
		nDisp = atoi(token_table[line_num]->operand[0]);
		sprintf(machine_code, "%02X%01X%05X",
			CharToHexnum(token_table[line_num]->comment) + a, b, nDisp);
	}
	//operand를 검사해서 INDEX 값이 존재하는 경우 
	if (strcmp(token_table[line_num]->operand[1], "X") == 0)
	{
		a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + EXTENT) >> 4;
		b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + INDEX + EXTENT) & 15;

		sprintf(machine_code, "%02X%01X00000",
			CharToHexnum(token_table[line_num]->comment) + a, b);
	}
	// INDEX 값이 없는 경우
	else
	{
		a = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + EXTENT) >> 4;
		b = (token_table[line_num]->nixbpe | INDIRECT + IMMEDIATE + EXTENT) & 15;
		sprintf(machine_code, "%02X%01X00000",
			CharToHexnum(token_table[line_num]->comment) + a, b);
	}
}


///////////////////////////////////////////////////////////////////////////
// CharToHexnum 함수
// 매계 : 문자열(16진수) 을 받아와서
// 기능 : 정수로 변환 후 리턴한다.
/////////////////////////////////////////////////////////////////////////
int CharToHexnum(char *opcode)
{
	int nSum = 0, loop, len;
	double x = 16;
	len = (int)strlen(opcode) - 1;
	// 자리수 만큼 루프를 돈다
	for (loop = 0; loop < len + 1; loop++)
	{
		if (('0' <= opcode[loop]) && (opcode[loop] <= '9'))
		{
			// 만약 16진수가 0~9 사이의 숫자이면

			nSum += (int)(pow(x, (len - loop)) * (opcode[loop] - '0'));
		}
		else if (('A' <= opcode[loop]) && (opcode[loop] <= 'F'))
		{
			// 만약 A~F사이의 문자이면

			nSum += (int)(pow(x, (len - loop)) * (opcode[loop] - 'A' + 10));
		}

	}

	// 10진수 결과값 반환
	return nSum;
}
//////////////////////////////////////////////////////////////////////////
// RegisterToDecnum 함수
// cReg : Register 캐릭터 문자를 받아오는 부분
// 기능 : Register 캐릭터 문자를 받아와서 대응하는 숫자로 리턴해주는 함수
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
		// 여기에 적절치 못한 Register가 올경우 에러 처리해줘야함
		printf("error : Register Not found!!!\n");
		return -1;
	}
}



///////////////////////////////////////////////////////////////
// make_objcetcode 함수
// 기능 : machine_code를 인자로 받아서 경우에 따라 
//        object프로그램에 써준다.
///////////////////////////////////////////////////////////////
void make_objectcode(char *machine_code)
{
	int i, j;
	code_num += strlen(machine_code);


	//CSECT나 END를 만났을때 ModifyRecord, EndRecord obejctcodef를 써준다 
	if (end_flag == 1)
	{	
		line_change = 0;
		machine_code2 = malloc(7);
		code_line++;

		sprintf(machine_code2, "%02X", (code_num - 9 - strlen(machine_code)) / 2);
		insert_length(ob_code[code_line - 1], machine_code2);

		
		for (i = 0; i < modify_number; i++)
		{	
			// modify_table을 참조해서 써줌 RXTREF값을 빼줘야하는경우
			if (modify_table[sectnum][i].REFname[0] == '-')
			{
				sprintf(machine_code, "M%06X%02d%s", modify_table[sectnum][i].addr,
					modify_table[sectnum][i].number,
					modify_table[sectnum][i].REFname);
				strcat(ob_code[code_line], machine_code);
				code_line++;
			}
			// RXTREF값을 더해주는 경우
			else{
				sprintf(machine_code, "M%06X%02d+%s", modify_table[sectnum][i].addr,
					modify_table[sectnum][i].number,
					modify_table[sectnum][i].REFname);
				strcat(ob_code[code_line], machine_code);
				code_line++;
			}

		}
		//첫번째 섹터의 END레코드에는 시작주소를 써준다.
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

	// 연속하지 않은 location으로 인해 TextRecord줄을 바꿔주는 경우
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

	//column이 가득 차서 TextRecord줄을 바꿔주는 경우
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
// insert_length 함수
// 기능 : TextRecord가 가득 차거나 끝나서 넘어가야 할때 레코드의 길이를 읽어서 레코드 
//        앞쪽에 있는 시작위치 바로 뒤에 써준다 
///////////////////////////////////////////////////////////////
void insert_length(char *object_code, char *length)
{
	int i;
	char buffer1[61];
	char buffer2[8];
	char buffer3[3];

	//버퍼2에는 앞에 7자리 레코드 이름과 위치 저장
	strncpy(buffer2, object_code, 7);
	buffer2[sizeof(buffer2) - 1] = '\0';

	//버퍼3에는 길이를 저장
	strcpy(buffer3, length);

	//버퍼1에는 내용 저장
	strncpy(buffer1, &object_code[7], 60);

	//레코드 이름, 위치, 길이. 내용 순으로 이어준다.
	strcpy(object_code, buffer2);
	strcat(object_code, buffer3);
	strcat(object_code, buffer1);

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */
	int i;
	FILE *write = NULL;
	if (file_name == NULL) {
		printf("-----------file 입력을 안했으므로 표준출력합니다.-----------------\n");
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