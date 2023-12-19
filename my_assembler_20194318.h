#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <ctype.h>

/* 
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

typedef unsigned char uchar;

/* 
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit
{
	uchar str[10];
	int ops; //	0:-		1:M		2:R		3:N		4:RR	5:RN
	int format; // 0:not SIC/XE code	1:format 1	2:format 2		3:format 3/4
	uchar op;
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
static int inst_index;
static int inst_count;

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 fopen input.txt
 */
uchar *input_data[MAX_LINES];
static int line_num;

/* 
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 */
struct token_unit
{
	uchar *label;
	uchar *operator;
	uchar operand[MAX_OPERAND][20];
	uchar comment[100];
	int addr;
	int ta;
};


typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * ���� ������ ��� ����
 */
struct symbol_unit
{
	uchar symbol[10];
	int addr;
	int sec; // section
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int sym_index;

static int locctr; // loc�� 
static int starting_address;
static int program_length;
static int sec;

// Define the structure for the program section
typedef struct {
    int sec;             // Section number
	//int locctr;          // Location counter
    int program_length;  // Program length
} csect;
#define MAX_CSECT 10
csect csect_table[MAX_CSECT];
//--------------
static int is_first_write = 1;
static int first_write = 1;
static uchar *input_file; // �Է� ���� input.txt
static uchar *output_file;// ��� ���� output.txt

int init_my_assembler(void); // ����� �ʱ�ȭ ���� init_inst_file()�� init_input_file()�� ����
int init_inst_file(uchar *inst_file); //inst.data�� �а� inst_table[]�� ����
int init_input_file(uchar *input_file); //intput.txt�� �а� input_data[]�� ����
int token_parsing(uchar *str); //input_data[]�� ����� ��ɾ ��ūȭ�Ͽ� token_table[]�� ����
int search_opcode(uchar *str); //input_data[]�� ����� ��ɾ��� opcode�� �˻� inst_table[]�� ����
static int assem_pass1(void); //SIC/XE Pass1 make_opcode_output�� make_symtab_output�� ����
void make_opcode_output(uchar *file_name); 
void make_symtab_output(uchar *file_name); 
static int assem_pass2(void); // SIC/XE Pass2 make_objectcode_output�� ����
void make_objectcode_output(uchar *file_name, uchar *list_name); // ���� output.txt�� �Էµ� ��ɾ��� ������Ʈ �ڵ� ����

//Extra functions, variables
void write_intermediate_file(uchar *str, int locctr);
void add_to_symtab(const uchar *label, int loc, int is_equ, int secton);
int search_symtab(uchar *symbol, int section);
int tok_search_opcode(uchar *str);
int init_token_table(void);
int write_listing_line(int format);
void write_text_record(void);
int evaluate_expression(uchar *expr);
int search_literal(uchar *operand);
int search_literaladdr(uchar *operand);
int calculate_byte_length(uchar *operand);
int search_extRtab(uchar *symbol, int section);
int search_extR_index(uchar *symbol, int section);
int search_extDtab(uchar *symbol);
void handle_extdef(uchar *symbol);
void handle_extref(uchar *symbol, int section);
void handle_equ_directive(uchar *label, uchar *operand);
void handle_ltorg_directive(void);
int getREGnum(uchar *register_name);
int generate_object_code(int format);
int hexstr2dec(char H);
int write_literal(void);
int handle_pass2(void);
void write_text_record(void);
int write_listing_line(int format);

typedef struct {
    uchar name[20];
    int leng;
	int value;
    int addr;// -1 if not assigned an address yet
} LT;

// Maximum number of literals (adjust as needed)
#define MAX_LITERALS 100
LT LTtab[MAX_LITERALS];
static int LT_num;
int current_pool;

#define MAX_EXTDEF 10 
#define MAX_EXTREF 10

symbol extDef[MAX_EXTDEF];
symbol extRef[MAX_EXTDEF];
static int extDefCount;
static int extRefCount;

// Define a counter for object code records
FILE *object_code_file;
FILE *listing_file;

// Define an array to store object code
int object_code[20];
uchar text_record[70];
int text_record_start;
int text_record_ctr;

// Define a counter for modification records
int mod_record_count;
int mod_last;
uchar mod_record[30][20];
int is_lt;
int csect_start_address;
int csect_length;
int BASEADDR;
int FEI;
uchar HEXTAB[]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
uchar *texp[10];