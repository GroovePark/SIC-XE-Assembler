/*
* my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
*
*/
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

#define INDIRECT 32
#define IMMEDIATE 16
#define INDEX 8
#define BASE 4
#define PC 2
#define EXTENT 1

/*
* instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
* 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
* 라인 별로 하나의 instruction을 저장한다.
*/
struct inst_unit {
	/* add your code here */
	char mnemonic[7];
	char format[5];
	char opcode[5];
	char operandnum[2];
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
* 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;

/*
* 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
* operator는 renaming을 허용한다.
* nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
*/
struct token_unit {
	char *label;
	char *operator;
	char *operand[MAX_OPERAND];
	char *comment;
	char nixbpe; // 추후 프로젝트에서 사용된다.

};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
* 심볼을 관리하는 구조체이다.
* 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
*/
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
int symnum;
static int locctr;

/*
* 리터럴을 관리하는 구조체이다.
* 리터럴 테이블은 리터럴 이름, 리터럴의 위치, 
처리되었는지 확인하기 위한 플래그 로 구성된다.
*/
struct literal_unit {
	char lvalue[10];
	int addr;
	int flag;
};
typedef struct literal_unit literalt;
literalt literal_table[MAX_LINES] = { {0},0,-1 };
int litnum;

/*
* 수정레코드을 관리하는 구조체이다.
* 수정 테이블은 더해줘야할 심볼 이름, 수정해야되는 위치와 
확인해줘야할 비트수로 구성된다.
*/
struct modify_unit { 
	char REFname[10];
	int addr;
	int number;
};	
typedef struct modify_unit modify;

modify modify_table[3][5];//ModifyRecord 작성할때 참고할 테이블
char EXTREF[3][4][7]; //EXTREF를 각 섹터별로 테이블화시킨다.
int start_loc; //프로그램 시작 주소
int sectnum; //섹터넘버
int modify_number; //modify_table 생성할때 필요
int sectsize[3]; //각 섹터의 사이즈
char ob_code[MAX_LINES][69]; // object 프로그램이 저장되는 배열
int code_line, code_num; //object프로그램의 라인 수
char *machine_code; // 기계어코드를 write하기 위한 버퍼
char *machine_code2; //예비용
int line_change; // 오브젝트 프로그램 줄 바꿈 플래그
int end_flag; //csect 만났을때 flag
int base; // base 값
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_opcode_output(char *file_name);

/* 추후 프로젝트에서 사용하게 되는 함수*/
void make_symtab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
void format2(int line_num, char *machine_code);
void format3(int line_num, int location, char *machine_code);
void format4(int line_num, int location, char *machine_code);
int RegisterToDecnum(char *strReg);
int CharToHexnum(char *opcode);
void make_objectcode(char *machine_code);
void insert_length(char *object_code, char *length);