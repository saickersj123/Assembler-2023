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
	int ops; 	//	0:-		1:M		2:R		3:N		4:RR	5:RN
	int format; // 0:not SIC/XE code	1:format 1	2:format 2		3:format 3/4
	uchar op;
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

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
	// ���������� ���� ����
	// uchar nixbpe;
};
//input.txt ����

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
};
//symbol table ���� �� ����
typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

static int locctr; //symtable �� loc�� 
//--------------

static uchar *input_file; //fopen(input.txt)
static uchar *output_file;//fopen(output.txt object file)
int init_my_assembler(void); // input inst.dat init �ʱ�ȭ �Լ� ȣ��
int init_inst_file(uchar *inst_file); //inst.dat
int init_input_file(uchar *input_file); //intput.txt
int token_parsing(uchar *str); //input.txt �� ��ū(��) ������
int search_opcode(uchar *str); //���ε� inst.data�� input.txt�� op�ڵ���� �˻�
static int assem_pass1(void); //�� �������� ȣ��
void make_opcode_output(uchar *file_name); //optable���� ���ε� inst.data

void make_symtab_output(uchar *file_name); // ���̺� + LOC ������ �ɺ����̺� ����
static int assem_pass2(void); // FROM PASS 1 (OPTABLE SYMBLE �Ѵ� ����) ���
void make_objectcode_output(uchar *file_name); // ���� OUTPUT ������Ʈ���� ����
