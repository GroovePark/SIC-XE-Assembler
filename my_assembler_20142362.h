/*
* my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
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
* instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
* ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
* ���� ���� �ϳ��� instruction�� �����Ѵ�.
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
* ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;

/*
* ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
* operator�� renaming�� ����Ѵ�.
* nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
*/
struct token_unit {
	char *label;
	char *operator;
	char *operand[MAX_OPERAND];
	char *comment;
	char nixbpe; // ���� ������Ʈ���� ���ȴ�.

};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
* �ɺ��� �����ϴ� ����ü�̴�.
* �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
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
* ���ͷ��� �����ϴ� ����ü�̴�.
* ���ͷ� ���̺��� ���ͷ� �̸�, ���ͷ��� ��ġ, 
ó���Ǿ����� Ȯ���ϱ� ���� �÷��� �� �����ȴ�.
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
* �������ڵ��� �����ϴ� ����ü�̴�.
* ���� ���̺��� ��������� �ɺ� �̸�, �����ؾߵǴ� ��ġ�� 
Ȯ��������� ��Ʈ���� �����ȴ�.
*/
struct modify_unit { 
	char REFname[10];
	int addr;
	int number;
};	
typedef struct modify_unit modify;

modify modify_table[3][5];//ModifyRecord �ۼ��Ҷ� ������ ���̺�
char EXTREF[3][4][7]; //EXTREF�� �� ���ͺ��� ���̺�ȭ��Ų��.
int start_loc; //���α׷� ���� �ּ�
int sectnum; //���ͳѹ�
int modify_number; //modify_table �����Ҷ� �ʿ�
int sectsize[3]; //�� ������ ������
char ob_code[MAX_LINES][69]; // object ���α׷��� ����Ǵ� �迭
int code_line, code_num; //object���α׷��� ���� ��
char *machine_code; // �����ڵ带 write�ϱ� ���� ����
char *machine_code2; //�����
int line_change; // ������Ʈ ���α׷� �� �ٲ� �÷���
int end_flag; //csect �������� flag
int base; // base ��
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

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
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