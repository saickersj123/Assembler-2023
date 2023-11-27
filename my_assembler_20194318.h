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
	//uchar nixbpe;
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
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int sym_index;

static int locctr; // loc�� 
static int starting_address;
static int program_length;
//--------------

static uchar *input_file; // �Է� ���� input.txt
static uchar *output_file;// ��� ���� output.txt
int init_my_assembler(void); // ����� �ʱ�ȭ ���� init_inst_file()�� init_input_file()�� ����
int init_inst_file(uchar *inst_file); //inst.data�� �а� inst_table[]�� ����
int init_input_file(uchar *input_file); //intput.txt�� �а� input_data[]�� ����
int token_parsing(uchar *str); //input_data[]�� ����� ��ɾ ��ūȭ�Ͽ� token_table[]�� ����
int search_opcode(uchar *str); //input_data[]�� ����� ��ɾ��� opcode�� �˻� inst_table[]�� ����
static int assem_pass1(void); //SIC/XE Pass1 make_opcode_output�� make_symtab_output�� ����
void make_opcode_output(uchar *file_name); /*Contain the mnemonic operation code & its machine language 
equivalent.
- May also contain information about instruction format and length
- Used to look up & validate mnemonic operation codes (Pass 1)
and translate them to machine language (Pass 2) 
- In SIC, both processes could be done together
- In SIC/XE, search OPTAB to find the instruction length (Pass 1), 
determine instruction format to use in assembling the instruction (Pass 2)
- Usually organized as a hash table & a static table
- Mnemonic operation code as a key
- Provide fast retrieval with a minimum searching
- A static table in most cases
- Entries are not normally added to or deleted from it
*/
void make_symtab_output(uchar *file_name); /*- Used to store values (addresses) assigned to labels
- Contain the name and value (addresses) for each label, 
together with flags for error conditions, 
also information about the data area or instruction labeled
- for example, its the type or length
- Pass 1 : labels are entered with their assigned (from LOCCTR)
- Pass 2 : symbols are used to look up in SYMTAB to obtain the 
addresses to be inserted
- Usually organized as a hash table
- For efficiency of insertion & retrieval
- Heavily used throughout the assembly*/
static int assem_pass2(void); // SIC/XE Pass2 make_objectcode_output�� ����
void make_objectcode_output(uchar *file_name); // ���� output.txt�� �Էµ� ��ɾ��� ������Ʈ �ڵ� ����
void write_intermediate_file(uchar *str, int locctr);
void add_to_symtab(const uchar *label, int loc, int is_equ);
int search_symtab(uchar *symbol);
int tok_search_opcode(uchar *str);
struct literal_unit {
    uchar name[20];
    int length;
    int address; // -1 if not assigned an address yet
};

// Maximum number of literals (adjust as needed)
#define MAX_LITERALS 100
struct literal_unit literals[MAX_LITERALS];
int num_literals = 0;

// Function to evaluate an arithmetic expression
static int evaluate_expression(uchar *expr);

#define MAX_EXTDEF 10 
#define MAX_EXTREF 10

symbol extDef[MAX_EXTDEF];
symbol extRef[MAX_EXTDEF];
int extDefCount = 0;
int extRefCount = 0;