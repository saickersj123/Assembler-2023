/*
  ����: SIC/XE Assembler, Example of a SIC/XE program
  �ۼ���: �迵�� (http://www.howto.pe.kr)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define MAXIDLEN 20 // ��Ī(identifier�� ǥ�� ����)

struct TAB { // Symbol, Literal Table ����ü
  char name[MAXIDLEN];
  int  value;
  int  sec; // ���ǹ�ȣ
};

struct OPT { // Operation Table ����ü
  char name[MAXIDLEN];
  int  op;
  int  oplength;
  char ext; // Ȯ�� ��ɾ�(SIC/XE) �̸� 1
};

char REGTAB[]= {'A','X','L','B','S','T','F'};
char HEXTAB[]= {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

// SIC/XE ��� ���̺�(Operation Table)
struct OPT OPTAB[] =
{
  // ����� ��ɾ�
  {"ADD",0x18,3,0},   {"ADDF",0x58,3,1},  {"ADDR",0x90,2,1},  {"AND",0x40,3,0},
  {"CLEAR",0xB4,2,1}, {"COMP",0x28,3,0},  {"COMPF",0x88,3,1}, {"COMPR",0xA0,2,1},
  {"DIV",0x24,3,0},   {"DIVF",0x64,3,1},  {"DIVR",0x9C,2,1},  {"FIX",0xC4,1,1},
  {"FLOAT",0xC0,1,1}, {"HIO",0xF4,1,1},   {"J",0x3C,3,0},     {"JEQ",0x30,3,0},
  {"JGT",0x34,3,0},   {"JLT",0x38,3,0},   {"JSUB",0x48,3,0},  {"LDA",0x00,3,0},
  {"LDB",0x68,3,1},   {"LDCH",0x50,3,0},  {"LDF",0x70,3,1},   {"LDL",0x08,3,0},
  {"LDS",0x6C,3,1},   {"LDT",0x74,3,1},   {"LDX",0x04,3,0},   {"LPS",0xD0,3,1},
  {"MUL",0x20,3,0},   {"MULF",0x60,3,1},  {"MULR",0x98,2,1},  {"NORM",0xC8,1,1},
  {"OR",0x44,3,0},    {"RD",0xD8,3,0},    {"RMO",0xAC,2,1},   {"RSUB",0x4C,3,0},
  {"SHIFTL",0xA4,2,1},{"SHIFTR",0xA8,2,1},{"SIO",0xF0,1,1},   {"SSK",0xEC,3,1},
  {"STA",0x0C,3,0},   {"STB",0x78,3,1},   {"STCH",0x54,3,0},  {"STF",0x80,3,1}, 
  {"STI",0xD4,3,1},   {"STL",0x14,3,0},   {"STS",0x7C,3,1},   {"STSW",0xE8,3,0},
  {"STT",0x84,3,1},   {"STX",0x10,3,0},   {"SUB",0x1C,3,0},   {"SUBF",0x5C,3,1},
  {"SUBR",0x94,2,1},  {"SVC",0xB0,2,1},   {"TD",0xE0,3,0},    {"TIO",0xF8,1,1}, 
  {"TIX",0x2C,3,0},   {"TIXR",0xB8,2,1},  {"WD",0xDC,3,0 },
  // ����� ������
  {"BASE",0,0,0},  {"NOBASE",0,0,0}, {"BYTE",1,0,0}, {"END",2,0,0},   {"EQU",7,0,0},
  {"LTORG",8,0,0}, {"RESB",3,0,0},   {"RESW",4,0,0}, {"START",5,0,0}, {"WORD",6,0,0},
  {"USE",9,0,0},   {"CSECT",10,0,0}, {"EXTREF",11,0,0}, {"EXTDEF",12,0,0}
};

// �ҽ����� �ڵ鷯
FILE *fSrc;
// ���� �ڵ尡 ÷���� �ҽ����� �ڵ鷯
FILE *fLst;
// ���� �ڵ� ���� �ڵ鷯
FILE *fObj;
// �ҽ� ������ label, operation code, operand 1/2
char label[MAXIDLEN], opcode[MAXIDLEN], op1[MAXIDLEN], op2[MAXIDLEN];
// �ҽ� ������ �о� ����
char buf[255];
// buf ���ڿ����� �ϳ��� ��ū(token) ���ڿ��� tmp�� ����
char tmp[MAXIDLEN];
// ��ġ �����
int LOCCTR;
// ���α׷� ���� �ּ�
int STARTADDR;
// ���̽� �ּ�
int BASEADDR;
// Symbol Table�� ��ȣ ����
int SYMTABp;
// Literal Table�� ��ȣ ����
int LTTABp;
// Literal Table�� �ֱ� ���� ��ġ
int LThandlep;
// Literal Table(pool) ��ȣ
int LTctr, LTp;
// �ҽ� ������ ���ڼ�
int length;
// �ҽ� ������ ó�� ��ġ
int index1, index2, index3;
// ���
int LineNum;
// ���� �����(control section) ��ȣ
int secctr;
// object code
int obj[MAXIDLEN];
// ���� ó������ pass ��ȣ
int PASS;
// ���� ������ ���α׷�(����) ���� ���
int Plength[20];
// ���� ó���Ǵ� ��ɾ��� �ּ�
int L2;
// 16������ ��Ÿ�� �ؽ�Ʈ ���ڵ�(Text record)�� ���� �ڵ�
char TR[70];
// ����Ʈ�� ��Ÿ�� �ؽ�Ʈ ���ڵ��� ����
int TRctr;
// �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�
int TS;
// Operation Table�� opcode
int opc;
// �ܺ� ������ ��ȣ ����
int EXTTABp;
// ���� ���ڵ�(Modification record)�� ����
int MTABp;
// ���� ���ڵ�(Modification record)�� �ֱ� ���� ��ġ
int MThandlep;
// ���ʷ� ������ ��ɾ��� �ּ�(first execution instruction)
int FEI;
// ��ȣ(symbol), ���ͷ�(literal), �ܺ����� ���̺�
struct TAB SYMTAB[200], LTTAB[100], EXTTAB[100];
// ����Ǹ�(control section name)
char SN[20][MAXIDLEN];
// ���� ���ڵ�(Modification record) ���̺�
char MTAB[30][MAXIDLEN];
// �Լ��� prototype
int writeLST(int format);

/*
  Operation table�� TBL[]���� obj[]�� ã�´�
*/
int searchop(char obj[])
{
  int i;
  for(i = 0; i < (sizeof(OPTAB) / sizeof(struct OPT)); i++) { // opcode ������ŭ
    if (obj[0] == '+') { // Ȯ�� ���� ��ɾ�(4����)
      if (strcmp(OPTAB[i].name, obj+1) == 0) {
        opc = OPTAB[i].op; // �����ڵ�(operation code)�� �����ص�
        return (OPTAB[i].oplength+1); // +1 �� 4byte ���̸� ��Ÿ��
      }
    }
    if (strcmp(OPTAB[i].name, obj) == 0) {
      opc = OPTAB[i].op;
      return (OPTAB[i].oplength);
    }
  }
  return (-1);
}

/*
  1. Symbol Table �� TBL[]���� sc ������ obj[]�� ã�´�
  2. Literal Table(pool) �� TBL[]���� sc Ǯ�� obj[]�� ã�´�
*/
int search(struct TAB TBL[], char obj[], int sc)
{
  int i;
  for(i = 0; i < 100; i++) {
    if ((strcmp(TBL[i].name, obj) == 0) && (TBL[i].sec == sc))
      return (TBL[i].value);
  }
  return (-1);
}

/*
  buf ���ڿ��� index1���� �����Ͽ� �ϳ��� ��ū(token) ���ڿ��� tmp�� �����Ѵ�
*/
int gettoken()
{
  index2 = 0;
  for(;(buf[index1] == ' ') || (buf[index1] == '\t'); index1++); //���鹮��(white space) ����
  if ((buf[index1] == ',') || (buf[index1] == '-') || (buf[index1] == '+')){
    tmp[index2] = buf[index1];
    index2++;
    index1++;
  }
  while ((buf[index1] != '\n') && (buf[index1] != '\r') && // ���� ��
         (buf[index1] != ' ') && (buf[index1] != '\t') && (buf[index1] != ',') &&
         (buf[index1] != '+') && (buf[index1] != '-') && (index1 < length)) {
    tmp[index2] = buf[index1];
    index1++;
    index2++;
  }
  tmp[index2] = '\0';
}

/*
  �ҽ��� �� ���� �о� label, opcode, op1, op2 �� ���Ѵ�
  return code:
    0: ����
    1: syntax error
    2: eof
*/
int readoneline()
{
  int n;

  if (fgets(buf, 255, fSrc) < 0)
    return (2); // eof

  while (buf[0] == '.' || buf[0] == '\n') { // �ּ�(comment)�� �������(empty line)�� ����
    if (PASS == 2) {
      LineNum += 5;
      fprintf(fLst, "%4d          %s", LineNum, buf); // �ּ��� ������ Lst ���Ͽ� Loc���� �׳� ���
    }
    if (fgets(buf, 255, fSrc) < 0)
      return (2); // eof
  }
  index1 = 0;
  length = strlen(buf);

  gettoken();
  if (searchop(tmp) >= 0) { // ù��° ��ū�� Operation table�� �ִ�
    label[0] = '\0'; // �̶��� label�� ����
    strcpy(opcode, tmp); // ������ label�� �ڸ������� Operation table�� �����Ƿ� opcode�� ���ֵ�
  } else {
    strcpy(label, tmp); // ù��° ��ū�� label�� �����Ѵ�
    gettoken();
    strcpy(opcode, tmp); // opcode(operation code)
    if (searchop(tmp) == -1) { // Operation table�� ����
      gettoken();
      strcpy(op1, tmp);
      gettoken();
      strcpy(op2, tmp);
      index3 = index1;
      if (PASS == 1)
  		  printf("illegal operation code: %s\n", buf);
      return (1); // syntax error ó��
    }
  }

  n = index1;
  gettoken();
  // op1�� �ְ� op1�� opcode�� �Ÿ��� 20 �̻��̸� op1�� �ּ����� ����
  if ((tmp[0] != '\0') && ((index1 - n) > 20)) {
    op1[0] = '\0';
    op2[0] = '\0';
  } else {
    strcpy(op1, tmp); // ù��° operand
    gettoken();
    strcpy(op2, tmp); // �ι�° operand
  }
  index3 = index1; // index3�� op2 ������ġ�� ����Ŵ
  // printf("label->%s opcode->%s op1->%s op2->%s\n", label, opcode, op1, op2);
  return (0); // ����ó��
}

/*
  �ҽ����� ù���� START������ ã�� �����ּҿ� �⺻ ���Ǹ��� ���Ѵ�
  return code:
    0: ����
    1: syntax error �Ǵ� eof
*/
int findstart()
{
  LOCCTR = 0; // ��ġ�����(location counter)
  secctr = 0; // ���� �����(control section) ��ȣ

  if (readoneline() == 0) {
    if (strcmp(opcode, "START") == 0) {
      sscanf(op1, "%d", &STARTADDR); // ���α׷� ���� �ּ�
      strcpy(SN[secctr], label); // �⺻ ����Ǹ�(START���� ��Ÿ�� label��)

		LOCCTR = STARTADDR; // ��ġ �����(LOCCTR)�� START���� ��Ÿ�� ���� �ּҷ� �ʱ�ȭ�Ѵ�

		if (PASS == 2) {
        fprintf(fLst, " ��     Loc                     ���ù�                     �����ڵ�\n");
        fprintf(fLst, "----    ----  -------------------------------------------- --------\n");
        writeLST(0); // Lst ������ ��� ����
        // Obj ������ ��� ����(Header)
        fprintf(fObj, "H%-6s%06X%06X\n", label, STARTADDR, Plength[secctr]);
      }
      // printf("section name->%s value->%d section->%d\n", SN[secctr], STARTADDR, secctr);

      if (readoneline() != 0) { // START�� �������� �о� �д�
        return(1);
      }
    } else // START���� �����ּҰ� ����
      printf("The start location is set to 0 as default.\n");
  } else {
    return(1);
  }
  return(0);
}

/*
  pass 2���� ����ϱ� ���� ��� label�� ������ �ּ� ������ �����Ͽ�
  ��ȣ ���̺�(Symbol Table)�� �����ϰ� EQU ����� �����ڸ� ó���Ѵ�
*/
int handlelabel1()
{
  int value;
  if (label[0] != '\0') {
    // EQU ����� �����ڴ� ��ȣ�� �����ϰ�(��, SYMTAB�� ����) �� ��ȣ�� ������
    // ���� �����Ѵ�. ��ȣ�� ���� ��� �Ǵ� ����� ������ ���ǵ� ��ȣ�� �����Ǵ�
    // ������ �� �� �ִ�
    if (strcmp(opcode, "EQU") == 0) { // EQU ����� ������ ó��
      if (op1[0] != '\0') {
        strcpy(SYMTAB[SYMTABp].name, label);
        if (op1[0] == '*') { // location counter���� ��ȣ�� ������ ���
          SYMTAB[SYMTABp].value = LOCCTR;
        } else {
          // ��ȣ�� ���� ��ȣ ���̺� �ִٸ� ��ȣ���� ����
          SYMTAB[SYMTABp].value = search(SYMTAB, op1, secctr);
          if (SYMTAB[SYMTABp].value == -1) // ��ȣ ���̺� ���ٸ� ��ȣ���� ����� ���� ó��
            SYMTAB[SYMTABp].value = atoi(op1);
        }
        SYMTAB[SYMTABp].sec = secctr;// ������ ���� ��ȣ(��ȣ�� ����ִ� ���� ��ȣ)

        // �Ʒ� ������ �������� ��ȣ(MAXLEN EQU BUFEND-BUFFER-10 ���� ����)�� ó���ϱ� ����...
        if ((op2[0] == '-') || (op2[0] == '+')) { // ������ ������ +, - �� ������
          strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
          while ((tmp[0] == '-') || (tmp[0] == '+')) {
            if (tmp[0] == '-') {
              value = search(SYMTAB, tmp+1, secctr);
              if (value == -1) // ��ȣ ���̺� ���ٸ� ��ȣ���� ����� ���� ó��
                SYMTAB[SYMTABp].value -= atoi(tmp+1);
              else
                SYMTAB[SYMTABp].value -= value;
            }
            if (tmp[0] == '+') {
              value = search(SYMTAB, tmp+1, secctr);
              if (value == -1)
                SYMTAB[SYMTABp].value += atoi(tmp+1);
              else
                SYMTAB[SYMTABp].value += value;
            }
            gettoken(); // ���� ��ū�� tmp�� �д´�
          } // while
        }

        SYMTABp++;
        // printf("symbol name->%s value->%d section->%d\n",
        //        SYMTAB[SYMTABp-1].name, SYMTAB[SYMTABp-1].value, SYMTAB[SYMTABp-1].sec);
      }
    } else {
      if (search(SYMTAB, label, secctr) >= 0) {
        printf("duplicate symbol\n");
        printf("%s \n", buf);
      } else {
        strcpy(SYMTAB[SYMTABp].name, label);
        SYMTAB[SYMTABp].value = LOCCTR; // location counter���� ��ȣ�� ������ ���
        SYMTAB[SYMTABp].sec = secctr; // ������ ���� ��ȣ(��ȣ�� ����ִ� ���� ��ȣ)
        SYMTABp++; // Symbol Table�� ��ȣ ����
        // printf("symbol name->%s value->%d section->%d\n",
        //        SYMTAB[SYMTABp-1].name, SYMTAB[SYMTABp-1].value, SYMTAB[SYMTABp-1].sec);
      }
    }
  }
}

/*
  Literal Table(pool) �� ���ͷ� ���(��������� computeLT()���� ��)
*/
int handleLT1()
{
  if (op1[0] == '=') {
    if (search(LTTAB, op1, LTctr) == -1) { // Literal Table(pool) ���� �ߺ� �˻�
      strcpy(LTTAB[LTTABp].name, op1);
      // ������ Literal Table ��ȣ(LTORG ���� ���� ���� �ٸ� ���̺��� �������)
      LTTAB[LTTABp].sec = LTctr;
      LTTABp++; // Literal Table�� ���ͷ� ����
      // printf("literal name->%s pool->%d\n", LTTAB[LTTABp-1].name, LTTAB[LTTABp-1].sec);
    }
  }
}

/*
  16���� ���� H�� 10������ �����Ѵ�
*/
int hexstr2dec(char H)
{
  int i;
  for (i = 0; i <= 15; i++)
    if (HEXTAB[i] == H)
      return (i);
  return (-1);
}

/*
  ���α׷�(�����)�� ���̰ų� LTORG ����� �����ڸ� �������� ȣ��Ǹ�
  Literal Table(pool) �� ���ͷ��� ������� ��(LTORG �ؿ� �����Ǿ� �ϹǷ�)
*/
int computeLT()
{
  int n;
  for (; LThandlep < LTTABp; LThandlep++) {
    if (LTTAB[LThandlep].name[1] == 'C') { // ������ ���ڷ� �ʱ�ȭ�Ǵ� ����Ʈ ��
      n = 0;
      LTTAB[LThandlep].value = LOCCTR;
      while (LTTAB[LThandlep].name[n+3] != '\'')
        n++;
      LOCCTR += n;
      // printf("literal name->%s value->%d pool->%d size->%d\n",
      //        LTTAB[LThandlep].name, LTTAB[LThandlep].value, LTTAB[LThandlep].sec, n);
    } else if (LTTAB[LThandlep].name[1] == 'X') { // ������ 16������ �ʱ�ȭ�Ǵ� ����Ʈ ��
      n = 0;
      LTTAB[LThandlep].value = LOCCTR;
      while (LTTAB[LThandlep].name[n+3] != '\'')
        n++;
      LOCCTR += n / 2; // byte �� �� ȯ���ϹǷ� 2�� ����
      // printf("literal name->%s value->%d pool->%d size->%d\n",
      //        LTTAB[LThandlep].name, LTTAB[LThandlep].value, LTTAB[LThandlep].sec, n/2);
    } else {
      LTTAB[LThandlep].value = LOCCTR;
      LOCCTR++;
    }
  }
}

/*
  1. ���α׷����� ��� ���� �ּҸ� �����Ѵ�
  2. ����� �����ڵ鿡 ���õ� ó���� �κ������� ���Ѵ�(BYTE, RESW � ���Ͽ� ���ǵǴ�
     ����Ÿ ������ ���� ������ ���� �ּ� ������ ������ �ִ� ó���� �����Ѵ�
*/
int handleopcode1()
{
  int n = 0;
  int v = searchop(opcode); // ��ɾ� ���̸� ����(�������� ���̴� 0)

  if (v > 0)
    LOCCTR = LOCCTR + v;
  else if (v == 0) { // ����� ������
    if (strcmp(opcode, "WORD") == 0) {
      LOCCTR = LOCCTR + 3;
      // printf("%s %s\n", opcode, op1);
    } else if (strcmp(opcode, "RESW") == 0) {
      sscanf(op1, "%d", &n); // n�� ����Ǵ� ����(3 byte)�� ����
      LOCCTR = LOCCTR + (n * 3);
      // printf("%s %s\n", opcode, op1);
    } else if (strcmp(opcode, "RESB") == 0) {
      sscanf(op1, "%d", &n); // n�� ����Ǵ� byte�� ����
      LOCCTR = LOCCTR + n;
      // printf("%s %s\n", opcode, op1);
    } else if (strcmp(opcode, "BYTE") == 0) {
      if (op1[0] == 'C') { // ������ ���ڷ� �ʱ�ȭ�Ǵ� ����Ʈ ��
        n = 0;
        while (op1[n+2] != '\'') {
          LOCCTR++;
          n++;
        }
        // printf("%s %s -> %d\n", opcode, op1, n);
      } else if (op1[0] == 'X') { // ������ 16������ �ʱ�ȭ�Ǵ� ����Ʈ ��
        n = 0;
        while (op1[n+2] != '\'')
          n++;
        LOCCTR += n / 2; // byte �� �� ȯ���ϹǷ� 2�� ����
        // printf("%s %s -> %d\n", opcode, op1, n/2);
      } else
        LOCCTR++;
    } else if (strcmp(opcode, "LTORG") == 0) {
      computeLT(); // Literal Table(pool) �� ���ͷ��� ������� ��
      LTctr++; // ���ο� Literal Pool �̹Ƿ� Literal Table(pool) ��ȣ�� �����Ѵ�
    } else if (strcmp(opcode, "CSECT") == 0) { // ���ο� ���� ������ �߰�
      Plength[secctr] = LOCCTR-STARTADDR; // ���� ���� ������ ���α׷�(����) ���� ���
      
      secctr++; // �����(control section) ��ȣ ����
      LOCCTR = 0; // ��ġ�����(location counter) �ʱ�ȭ
      STARTADDR = 0; // ���� �ּ� �ʱ�ȭ

      strcpy(SN[secctr], label); // ����Ǹ�(CSECT���� ��Ÿ�� label��)
      // ����Ǹ��� �̹� ��ȣ ���̺� �� �����Ƿ�(ó�� ������ �׷��� ��) �ʱ�ȭ
      SYMTAB[SYMTABp-1].value = 0;
      SYMTAB[SYMTABp-1].sec = secctr;
      // printf("section name->%s value->%d section->%d\n",
      //        SN[secctr], SYMTAB[SYMTABp-1].value, SYMTAB[SYMTABp-1].sec);
    } else if (strcmp(opcode, "EXTREF") == 0) { // �ܺ� ����(external reference)
      if (op1[0] != '\0') {
        strcpy(EXTTAB[EXTTABp].name, op1);
        EXTTAB[EXTTABp].value = 0; // �ּҰ��� ���� �������� ����
        EXTTAB[EXTTABp].sec = secctr; // ������ ���� ��ȣ
        EXTTABp++; // �ܺ� ������ ��ȣ ����
      }
      // printf("external reference->%s value->%d section->%d\n",
      //        EXTTAB[EXTTABp-1].name, EXTTAB[EXTTABp-1].value, EXTTAB[EXTTABp-1].sec);

      strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
      while (tmp[0] == ',') {
        strcpy(EXTTAB[EXTTABp].name, tmp+1);
        EXTTAB[EXTTABp].value = 0;
        EXTTAB[EXTTABp].sec = secctr;
        EXTTABp++;
        // printf("external reference->%s value->%d section->%d\n",
        //        EXTTAB[EXTTABp-1].name, EXTTAB[EXTTABp-1].value, EXTTAB[EXTTABp-1].sec);
        gettoken(); // ���� ��ū�� tmp�� �д´�
      }
    }
  }
}

/*
  �������� ���� REG�� ��ȣ�� �����Ѵ�
*/
char getREGnum(char REG)
{
  int i;
  for (i = 0; i <= 6; i++)
    if (REGTAB[i] == REG)
      return (i);
  printf("Error! REGISTER NAME expected: %s\n", buf);
  return (-1);
}

/*
  �����ڵ�(operation code)�� object �ڵ� ����
*/
int makeobjcode(int format)
{
  int dxx;
  int disp;
  int loc;
  int loc2;

  // opc�� �����ڵ�(operation code)�� searchop()������ �̸� �����ص�
  switch(format) {
    case 1: // 1���� ��ɾ�(FIX, FLOAT, HIO, NORM, SIO, TIO)
      obj[0] = opc;
      obj[1] = '\0';
      break;

    case 2: // 2���� ��ɾ�(operand ������ r1 �Ǵ� r1,r2 �Ǵ� n �Ǵ� r1,n)
      obj[0] = opc;
      if (op1[0] == '\0')
        printf("Error! Operand expected: %s\n", buf);

      if (op1[0] < 'A') { // op1�� �������Ͱ� �ƴ� �����(n)
        sscanf(op1, "%d", &dxx);
        obj[1] = dxx << 4; // n
      } else {
        obj[1] = getREGnum(op1[0]) << 4; // r1
      }
      if (op2[0] == ',') {
        if (op2[1] < 'A') { // op2�� �������Ͱ� �ƴ� �����(n)
          sscanf(op2+1, "%d", &dxx);
          obj[1] = obj[1] | dxx; // n
        } else {
          obj[1] = obj[1] | getREGnum(op2[1]); // r2
        }
      }
      break;

    case 3: // 3���� ��ɾ�
      switch (op1[0]) {
        case '#': // ��� �ּ� ����(Immediate addressing)
          obj[0] = opc + 1; // n=0,i=1 �̹Ƿ� +1 ��
          if (op1[1] >= 'A') { // PC ����ּ� + ��� �ּ� ������ ���(��, #LENGTH)
            dxx = search(SYMTAB, op1+1, secctr);
            if (dxx == -1)
              printf("Error: Undefind symbol: %s\n", op1+1);
            else
              dxx -= LOCCTR; // op1�� ���� ��ɾ ����Ű�� �ִ� LOCCTR ���� ����
            obj[1] = (dxx >> 8) & 15; // TA ���� 1 �Ϻ�
            obj[1] = obj[1] | 32; // p=1 (PC ����ּ�)
            obj[2] = dxx & 255; // TA ���� 2 �Ϻ�
          } else { // �����
             sscanf(op1+1, "%d", &dxx);
             obj[1] = (dxx >> 8) & 15; // TA ���� 1 �Ϻ�
             obj[2] = dxx & 255; // TA ���� 2 �Ϻ�
          }
          break;
        case '\0': // operand�� ����(RSUB)
           obj[0] = opc + 3; // n=1,i=1 �̹Ƿ� +3 ��
           obj[1] = 0;
           obj[2] = 0;
           break;
        default:
          if (op1[0] == '@') { // ���� �ּ� ����(Indirect addressing)
            obj[0] = opc + 2; // n=1,i=0 �̹Ƿ� +2 ��
            loc = search(SYMTAB, op1+1, secctr);
            disp =loc - LOCCTR; // op1�� ���� ��ɾ ����Ű�� �ִ� LOCCTR ���� ����
          }
          else if (op1[0] == '=') { // ���ͷ�
            obj[0] = opc + 3; // n=1,i=1 �̹Ƿ� +3 ��
            loc = search(LTTAB, op1, LTctr); // Literal Table(pool) ���� �˻�
            disp = loc - LOCCTR; // op1�� ���� ��ɾ ����Ű�� �ִ� LOCCTR ���� ����
          }
          else {
            obj[0] = opc + 3; // n=1,i=1 �̹Ƿ� +3 ��
            loc = search(SYMTAB, op1, secctr);
            disp = loc - LOCCTR; // op1�� ���� ��ɾ ����Ű�� �ִ� LOCCTR ���� ����
          }

          if (loc < 0) {
            printf("Error: Undefinded symbol: %s\n", op1);
            loc = 0;
          }

          if ((abs(disp) >= 4096) && (loc >= 0)) { // base ����ּҸ� ����ؾ��ϴ� ���
            disp = abs(BASEADDR - loc);
            obj[1] = (disp >> 8) & 15; // TA ���� 1 �Ϻ�
            obj[1] = obj[1] | 64; // b=1 (base ����ּ�)
            obj[2] = disp & 255; // TA ���� 2 �Ϻ�
            // printf("opcode->%s base->%X loc->%X disp->%d\n", opcode, BASEADDR, loc, disp);
          }
			 else if ((disp < 4096) && (loc >= 0)) {
            obj[1] = (disp >> 8) & 15; // TA ���� 1 �Ϻ�
            obj[1] = obj[1] | 32; // p=1 (PC ����ּ�)
            obj[2] = disp & 255; // TA ���� 2 �Ϻ�
          }
          else {
            int ddd;
            if (op1[0] == '@') // Indirect addressing
              sscanf(op1+1, "%d", &ddd);
            else
              sscanf(op1, "%d", &ddd);
            obj[1] = (ddd >> 8);
            obj[2] = ddd & 255;
          }

          if ((op2[1] == 'X') && (op2[0] == ',')) // �ε��� �ּ� ����
            obj[1] = obj[1] + 128; // x=1
      }
      // printf("<3����> opcode->%s obj->%02X%02X%02X\n", opcode, obj[0], obj[1], obj[2]);
      break;

    case 4: // 4����
      switch (op1[0]) {
        case '#': // ��� �ּ� ����(Immediate addressing)
          obj[0] = opc + 1; // n=0,i=1 �̹Ƿ� +1 ��
          if (op1[1] >= 'A') { // PC ����ּ� + ��� �ּ� ������ ���(��, #LENGTH)
            dxx = search(SYMTAB, op1+1, secctr);
            if (dxx== -1)
              printf("Error: Undefind symbol: %s\n", op1+1);
            else
              dxx -= LOCCTR; // op1�� ���� ��ɾ ����Ű�� �ִ� LOCCTR ���� ����
          } else  // �����
            sscanf(op1+1, "%d", &dxx);
          obj[1] = 16; // e=1
          obj[2] = (dxx >> 8) & 255; // TA ���� 1 ����Ʈ
          obj[3] = dxx & 255; // TA ���� 1 ����Ʈ
          break;
        default:
          if (op1[0] == '@') { // ���� �ּ� ����(Indirect addressing)
            obj[0] = opc + 2; // n=1,i=0 �̹Ƿ� +2 ��
            loc2 = search(SYMTAB, op1+1, secctr);
          }
          else {
            obj[0] = opc + 3; // n=1,i=1 �̹Ƿ� +3 ��
            loc2 = search(SYMTAB, op1, secctr);
            if (loc2 < 0) { // ���� ���ǿ��� ã�Ҵµ� ����
              loc2 = search(EXTTAB, op1, secctr); // �ܺ� �������� �˻�
              if (loc2 >= 0) {
                // ���� ���ڵ�(Modification record)�� �߰�
                // L2-STARTADDR+1 -> �� ������� ó������ ��������� ǥ��� ������ �ʵ��� �ּ�
                // 05 -> ���� ����Ʈ�� ��Ÿ�� ������ ���ڵ��� ����(���⼭�� 05�� fix��Ŵ)
                // + -> ���� �÷���(���⼭�� +�� fix��Ŵ)
                sprintf(MTAB[MTABp], "M%06X05+%-6s\n", L2-STARTADDR+1, op1);
                MTABp++;
              }
            }
            if (loc2 < 0) {
               printf("Error: Undefinded symbol: %s\n", op1);
               loc2=0;
            }
          }
          if (loc2 >= 0) {
            obj[1] = 16; // e=1
            obj[2] = (loc2 >> 8); // TA ���� 1 ����Ʈ
            obj[3] = loc2 & 255;  // TA ���� 1 ����Ʈ
          }
          else {
            int ddd;
            if (op1[0] == '@') // Indirect addressing
              sscanf(op1+1, "%d", &ddd);
            else
              sscanf(op1, "%d", &ddd);
            obj[1] = 16; // e=1
            obj[2] = (ddd >> 8); // TA ���� 1 ����Ʈ
            obj[3] = ddd & 255;  // TA ���� 1 ����Ʈ
          }
          if (op2[1] == 'X') // �ε��� �ּ� ����
            obj[1] = obj[1] + 128; // x=1
      }
      // printf("<4����> opcode->%s obj->%02X%02X%02X%02X\n", opcode, obj[0], obj[1], obj[2], obj[3]);
      break;
  }
}

/*
  Literal Table(pool) ���
*/
int writeLT()
{
  int n, len=0;

  for (; (LTp < LTTABp) && (LTTAB[LTp].sec == LTctr); LTp++) {
    L2 = LOCCTR;
    len = 0;
    label[0] = '*'; // *
    label[1] = '\0';
    strcpy(opcode, LTTAB[LTp].name);
    op1[0] = '\0';
    if (LTTAB[LTp].name[1] == 'C') { // ������ ���ڷ� �ʱ�ȭ�Ǵ� ����Ʈ ��
      n = 0;
      while (LTTAB[LTp].name[n+3] != '\'') {
        obj[len] = LTTAB[LTp].name[n+3]; // character�� byte�� ȯ��
        n++;
        len++;
      }
      obj[len] = '\0';
      LOCCTR += len;
    }
    else if (LTTAB[LTp].name[1] == 'X') { // ������ 16������ �ʱ�ȭ�Ǵ� ����Ʈ ��
      n = 0;
      while (LTTAB[LTp].name[n+3] != '\'') {
        // 16���� �� ���ڸ� 1 byte�� ȯ��
        obj[len] = hexstr2dec(LTTAB[LTp].name[n+3])*16 + hexstr2dec(LTTAB[LTp].name[n+4]);
        n += 2;
        len++;
      }
      obj[len] = '\0';
      LOCCTR += len;
    }
    else {
      sscanf(op1+1, "%d", obj);
      len++;
      LOCCTR++;
    }

    if (opcode[0] != '=')
      writeLST(0);
    else
      writeLST(len);

  } // for
  return(len);
}

/*
  �ؽ�Ʈ ���ڵ�(Text record)�� ���
*/
int writeTEXT()
{
  if (TRctr > 0) {
    // �ؽ�Ʈ ���ڵ�(Text record)
    // TS -> �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�
    // TRctr-> ����Ʈ�� ��Ÿ�� �ؽ�Ʈ ���ڵ��� ����(TR�� 16���� ���ڷ� �� �����Ƿ� ������ ��������)
    // TR -> 16������ ��Ÿ�� ���� �ڵ�
    fprintf(fObj, "T%06X%02X%s\n", TS, TRctr/2, TR);
    TRctr = 0;
  }
}

/*
  Pass 2 ����Ʈ, �����ڵ� ���
*/
int handlepass2()
{
  int n = 0;
  int format = searchop(opcode); // ��ɾ� ���̸� ����(�������� ���̴� 0)
  // * LOCCTR�� ���� ó���Ǵ� ��ɾ��� ���� ��ɾ ����Ű�� �ǹǷ� L2�� ���� �ּҷ� ����Ͻÿ�!
  L2 = LOCCTR;
  if (format > 0) { // ����� �����ڰ� �ƴ�
    LOCCTR = LOCCTR + format; // ��ġ�����(location counter)�� ���� ��ɾ ����Ű�Ե�
    makeobjcode(format);
    writeLST(format);
  }
  else if (format == 0) { // ����� ������
    if (strcmp(opcode, "BASE") == 0) { // ���̽� ���������� ���� ��������� �˷��ش�
      writeLST(format);
      BASEADDR = search(SYMTAB, op1, secctr); // ��ȣ ���̺��� �˻�
      if (BASEADDR < 0)
        BASEADDR = 0;
    }
    else if (strcmp(opcode, "NOBASE") == 0) { // ���̽� ���������� ���� �����Ѵ�
      writeLST(format);
      BASEADDR = 0;
    }
    else if (strcmp(opcode, "WORD") == 0) {
      LOCCTR = LOCCTR + 3;
      if (op1[0] > 'A') { // ��ȣ ���
        int W3, W4=0;
        W3 = search(SYMTAB, op1, secctr); // ��ȣ ���̺��� �˻�
        if (W3 < 0) {
          W3 = search(EXTTAB, op1, secctr); // �ܺ� �������� �˻�
          if (W3 < 0) {
            printf("Error: Undefinded symbol: %s\n", op1);
            obj[0] = obj[1] = obj[2] = 0;
            return(3);
          }
          // ���� ���ڵ�(Modification record)�� �߰�
          // L2-STARTADDR+1 -> �� ������� ó������ ��������� ǥ��� ������ �ʵ��� �ּ�
          // 05 -> ���� ����Ʈ�� ��Ÿ�� ������ ���ڵ��� ����(���⼭�� 05�� fix��Ŵ)
          // + -> ���� �÷���(���⼭�� +�� fix��Ŵ)
          sprintf(MTAB[MTABp], "M%06X06+%-6s\n", L2-STARTADDR, op1);
          MTABp++;
        }

        // �Ʒ� ������ �������� ��ȣ(MAXLEN WORD BUFEND-BUFFER ���� ����)�� ó���ϱ� ����...
        if ((op2[0] == '-') || (op2[0] == '+')) { // ������ ������ +, - �� ������
          strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
          while ((tmp[0] == '-') || (tmp[0] == '+')) {
            W4 = search(SYMTAB, tmp+1, secctr);
            if (W4 < 0) {
              W4 = search(EXTTAB, tmp+1, secctr);
              if (W4 < 0) {
                printf("Error: Undefinded symbol: %s\n", tmp+1);
                obj[0] = obj[1] = obj[2] = 0;
                return(3);
              }
              sprintf(MTAB[MTABp], "M%06X06%c%-6s\n", L2-STARTADDR, tmp[0], tmp+1);
              MTABp++;
            }
            if (op2[0]=='+')
              W3 = W3 + W4;
            else
              W3 = W3 - W4;

            gettoken(); // ���� ��ū�� tmp�� �д´�
          } // while
        }

        obj[0] = (W3 >> 16) & 255;
        obj[1] = (W3 >> 8) & 255;
        obj[2] = W3 & 255;
      }
      else {
        sscanf(op1, "%d", &n);
        obj[0] = 0;
        obj[1] = (n >> 8) & 255;
        obj[2] = n & 255;
      }
      writeLST(3); // 3������ �ƴ϶� ���� �μ⸦ ����
      return (3);
    }
    else if (strcmp(opcode, "RESW") == 0) {
      writeLST(format);
      sscanf(op1, "%d", &n); // n�� ����Ǵ� ����(3 byte)�� ����
      LOCCTR = LOCCTR + (n * 3);
      writeTEXT(); // ���� ������ �ؽ�Ʈ ���ڵ�(Text record)�� ���
      // TS -> �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�
      // LOCCTR�� �����ϴ°��� ���� RESW�� �Ҵ�޴� ������ �Ѿ �ּҰ� ���۵Ǿ�� �ϹǷ�...
      TS = LOCCTR;
    }
    else if (strcmp(opcode, "RESB") == 0) {
      writeLST(format);
      sscanf(op1, "%d", &n); // n�� ����Ǵ� byte�� ����
      LOCCTR = LOCCTR + n;
      writeTEXT(); // ���� ������ �ؽ�Ʈ ���ڵ�(Text record)�� ���
      // TS -> �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�
      // LOCCTR�� �����ϴ°��� ���� RESW�� �Ҵ�޴� ������ �Ѿ �ּҰ� ���۵Ǿ�� �ϹǷ�...
      TS = LOCCTR;
    }
    else if (strcmp(opcode, "BYTE")==0)
    {
      if (op1[0] == 'C') { // ������ ���ڷ� �ʱ�ȭ�Ǵ� ����Ʈ ��
        n = 0;
        while (op1[n+2] != '\'') {
          obj[n] = op1[n+2]; // character�� byte�� ȯ��
          n++;
        }
        obj[n] = '\0';
        LOCCTR += n;
        writeLST(n);
        return(n);
      }
      else if (op1[0] == 'X') { // ������ 16������ �ʱ�ȭ�Ǵ� ����Ʈ ��
        int len=0;
        n = 0;
        while (op1[n+2] != '\'') {
          obj[len] = hexstr2dec(op1[n+2])*16 + hexstr2dec(op1[n+3]); // 16���� �� ���ڸ� 1 byte�� ȯ��
          n += 2;
          len++;
        }
        obj[len] = '\0';
        LOCCTR += len;
        writeLST(len);
        return(len);
      }
      else {
        sscanf(op1, "%d", &n);
        obj[0] = n;
        LOCCTR++;
        writeLST(n);
        return(1);
      }
    }
    else if (strcmp(opcode, "LTORG") == 0) {
      writeLST(format);
      n = writeLT(); // Literal Table(pool) ���
      LTctr++; // ���ο� Literal Pool �̹Ƿ� Literal Table(pool) ��ȣ�� �����Ѵ�
      return(n);
    }
    else if (strcmp(opcode, "CSECT")==0)
    {
      LOCCTR = 0; // ��ġ�����(location counter) �ʱ�ȭ
      STARTADDR = 0; // ���� �ּ� �ʱ�ȭ
      L2 = 0;
      TS = 0; // TS -> �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�

      fprintf(fLst, "\n");
      writeLST(format);

      secctr++; // �����(control section) ��ȣ ����
      writeTEXT(); // ���� ������ �ؽ�Ʈ ���ڵ�(Text record)�� ���
      while (MThandlep < MTABp) { // ���� ���ڵ�(Modification record)�� ����Ѵ�
        fprintf(fObj, "%s", MTAB[MThandlep]);
        MThandlep++;
      }
      if (secctr == 1) // �⺻ ����(default section)
         fprintf(fObj, "E%06X\n\n", FEI); // ���� ���ڵ�(End record)
      else
         fprintf(fObj, "E\n\n");
      fprintf(fObj, "H%-6s%06X%06X\n", label, 0, Plength[secctr]);
    }
    else if (strcmp(opcode, "EQU") == 0) {
      L2 = search(SYMTAB, label, secctr);
      LineNum += 5;
      fprintf(fLst, "%4d    %04X  %-8s %-10s %s", LineNum, L2, label, opcode, op1); // �� �տ� Loc ���
      strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
      while ((tmp[0] == '-') || (tmp[0] == '+')) {
        fprintf(fLst, "%s", tmp);
        gettoken(); // ���� ��ū�� tmp�� �д´�
      }
      fprintf(fLst, "\n");
    }
    else if (strcmp(opcode, "EXTDEF") == 0) // �ܺ� ����(external definition)
    {
      fprintf(fObj, "D"); // ���� ���ڵ�(define record)
      if (op1[0] != '\0') {
        int a1 = search(SYMTAB, op1, secctr);
        if (a1 < 0)
          printf("Error in EXTDEF: No such symbol: %s\n", op1);
        fprintf(fObj, "%-6s%06X", op1, a1); // �� ���� ���ǿ��� ���ǵ� �ܺα�ȣ�̸�,����ּ�
        LineNum += 5;
        fprintf(fLst, "%4d          %-8s %-10s %s", LineNum, label, opcode, op1);
      }

      strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
      while (tmp[0] == ',') {
        int a2 = search(SYMTAB, tmp+1, secctr);
        if (a2 < 0)
          printf("Error in EXTDEF: No such symbol: %s\n", tmp+1);
        fprintf(fObj,"%-6s%06X", tmp+1, a2);
        fprintf(fLst, "%s", tmp);
        gettoken(); // ���� ��ū�� tmp�� �д´�
      }
      fprintf(fObj, "\n");
      fprintf(fLst, "\n");
    }
    else if (strcmp(opcode, "EXTREF") == 0) // �ܺ� ����(external reference)
    {
      fprintf(fObj, "R"); // ���� ���ڵ�(Refer record)
      if (op1[0] != '\0') {
        fprintf(fObj, "%-6s", op1);
        LineNum +=  5;
        fprintf(fLst, "%4d          %-8s %-10s %s", LineNum, label, opcode, op1);
      }

      strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
      while (tmp[0] == ',') {
        fprintf(fObj, "%-6s", tmp+1);
        fprintf(fLst, "%s", tmp);
        gettoken(); // ���� ��ū�� tmp�� �д´�
      }
      fprintf(fObj, "\n");
      fprintf(fLst, "\n");
    }
  }
  return (format);
}

int writeLST(int format)
{
  LineNum += 5; // ���ȣ

  switch (format) {
    case 0: // ������ ������
      if (label[0] == '\0')
        fprintf(fLst, "%4d          %-8s %-10s %s\n", LineNum, label, opcode, op1);
      else
        fprintf(fLst, "%4d    %04X  %-8s %-10s %s\n", LineNum, L2, label, opcode, op1);
      break;

    case 1:
      if ((TRctr + 2) > 60) {
        writeTEXT();
        TS = L2;
      }
      sprintf(TR+TRctr, "%02X", obj[0]);
      TRctr += 2;
      fprintf(fLst, "%4d    %04X  %-8s %-10s %-25s%02X\n", LineNum, L2, label, opcode, op1, obj[0]);
      break;

    case 2: // 2����
      if ((TRctr + 4) > 60) {
        writeTEXT(); // ���� ������ �ؽ�Ʈ ���ڵ�(Text record)�� ���
        // TS -> �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�
        // L2�� �����ϴ°��� ���� ��ɾ ���� �ؽ�Ʈ ���ڵ��� �����ּ��̹Ƿ�...
        TS = L2;
      }
      sprintf(TR+TRctr, "%02X%02X", obj[0], obj[1]);
      TRctr += 4;

      fprintf(fLst,"%4d    %04X  %-8s %-10s", LineNum, L2, label, opcode);
      strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
      while ((tmp[0] == ',') || (tmp[0] == '-') || (tmp[0] == '+')) {
        strcat(op1, tmp);
        gettoken(); // ���� ��ū�� tmp�� �д´�
      }
      fprintf(fLst," %-25s%02X%02X\n", op1, obj[0], obj[1]);
      break;

    case 3: // 3����
      if ((TRctr + 6) > 60) {
        writeTEXT(); // ���� ������ �ؽ�Ʈ ���ڵ�(Text record)�� ���
        // TS -> �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�
        // L2�� �����ϴ°��� ���� ��ɾ ���� �ؽ�Ʈ ���ڵ��� �����ּ��̹Ƿ�...
        TS = L2;
      }
      sprintf(TR+TRctr, "%02X%02X%02X", obj[0], obj[1], obj[2]);
      TRctr += 6;
      fprintf(fLst,"%4d    %04X  %-8s %-10s", LineNum, L2, label, opcode);
      strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
      while ((tmp[0] == ',') || (tmp[0] == '-') || (tmp[0] == '+')) {
        strcat(op1, tmp);
        gettoken(); // ���� ��ū�� tmp�� �д´�
      }
      fprintf(fLst," %-25s%02X%02X%02X\n", op1, obj[0], obj[1], obj[2]);
      break;

    case 4: // 4����
      if ((TRctr + 8) > 60) {
        writeTEXT(); // ���� ������ �ؽ�Ʈ ���ڵ�(Text record)�� ���
        // TS -> �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�
        // L2�� �����ϴ°��� ���� ��ɾ ���� �ؽ�Ʈ ���ڵ��� �����ּ��̹Ƿ�...
        TS = L2;
      }
      sprintf(TR+TRctr, "%02X%02X%02X%02X", obj[0], obj[1], obj[2], obj[3]);
      TRctr += 8;
      fprintf(fLst,"%4d    %04X  %-8s %-10s", LineNum, L2, label, opcode);
      strcpy(tmp, op2); // ���� ��ū�� �б����� �̹� �о��� op2�� ���� ó��
      while ((tmp[0] == ',') || (tmp[0] == '-') || (tmp[0] == '+')) {
        strcat(op1, tmp);
        gettoken(); // ���� ��ū�� tmp�� �д´�
      }
      fprintf(fLst," %-25s%02X%02X%02X%02X\n", op1, obj[0], obj[1], obj[2], obj[3]);
      break;
  }
}

/*
  filename�� Ȯ���ڸ� ext�� �ٲپ ofilename���� �����Ѵ�
*/
char * makefilename(char filename[], char ofilename[], char ext[])
{
  int i;
  char *dot;
  ofilename[0] = '\0';
  if (strchr(filename, '.') == NULL) { // Ȯ���� ����
    strcpy(ofilename, filename);
    strcat(ofilename, ext);
  } else {
    for (i = 0; filename[i]; i++)
      if (filename[i] == '.')
        break;
      else
        ofilename[i] = filename[i];
      ofilename[i] = '\0';
    strcat(ofilename, ext);
  }
  return ofilename;
}

int pass1(char filename[])
{
  int i;
  char ofilename[30];
  FILE *fSbl;

  PASS      = 1;
  SYMTABp   = 0;
  LTTABp    = 0;
  LThandlep = 0;
  LTctr     = 0;
  EXTTABp   = 0;
  if (findstart() != 0)
    return (1);

  while ((strcmp(opcode, "END") != 0)) {
    /*
      Pass 1 (��ȣ ����)
        1. ���α׷����� ��� ���� �ּҸ� �����Ѵ�
        2. Pass 2 ���� ����ϱ� ���� ��� label�� ������ �ּ� ������ �����Ѵ�
        3. ����� �����ڵ鿡 ���õ� ó���� �κ������� ���Ѵ�(BYTE, RESW � ���Ͽ� ���ǵǴ�
           ����Ÿ ������ ���� ������ ���� �ּ� ������ ������ �ִ� ó���� �����Ѵ�
    */
    handlelabel1();
    handleopcode1();
    handleLT1();
    // printf("label->[%s] opcode->[%s] op1->[%s] op2->[%s]\n", label, opcode, op1, op2);
    if (readoneline() != 0) // syntax error �Ǵ� eof
      break;
  }

  if (op1[0] != '\0')
    FEI = search(SYMTAB, op1, 0); // ���ʷ� ������ ��ɾ��� �ּ�
  else
    FEI = 0;

  computeLT(); // Literal Table(pool) �� ���ͷ��� ������� ��
  Plength[secctr] = LOCCTR-STARTADDR; // ������ ���� ������ ���α׷�(����) ���� ���

  // Pass 1�� ����� ��ȣ ���̺�(Symbol Table), ���ͷ� ���̺�(Literal Table) ���
  fSbl = fopen(makefilename(filename,ofilename,".sbl"), "w");
  fprintf(fSbl, "1. Symbol Table for %s\n\n", filename);
  fprintf(fSbl, "    Symbol Name           Loc          Section Name\n");
  fprintf(fSbl, "--------------------      ----      --------------------\n");
  for (i = 0; i < SYMTABp; i++)
    fprintf(fSbl, "%20s      %04X      %20s\n", SYMTAB[i].name, SYMTAB[i].value, SN[SYMTAB[i].sec]);
  if (LTTABp > 0) {
    fprintf(fSbl, "\n\n2. Literal Table for %s\n\n", filename);
    fprintf(fSbl, "       Literal            Loc        Pool Number\n");
    fprintf(fSbl, "--------------------      ----      -------------\n");
    for (i = 0; i < LTTABp; i++)
      fprintf(fSbl, "%20s      %04X          %3d\n", LTTAB[i].name, LTTAB[i].value, LTTAB[i].sec);
  }
  fclose(fSbl);
  return (0);
}

int pass2(char filename[])
{
  int i;
  char ofilename[30];

  PASS      = 2;
  LTctr     = 0;

  // ���� �ڵ尡 ÷���� �ҽ����� �ڵ鷯 fLst
  fLst = fopen(makefilename(filename,ofilename,".lst"), "w"); // Code listing with object code

  // ���� �ڵ� ���� �ڵ鷯 fObj
  fObj = fopen(makefilename(filename,ofilename,".obj"), "w"); // Object code

  if (findstart() != 0)
    return (1);

  while (1) {
    // ����Ʈ, �����ڵ� ���
	 handlepass2();
    if (readoneline() != 0) // syntax error �Ǵ� eof
      break;

    if (strcmp(opcode, "END") == 0) {
      writeLST(0);
      writeTEXT(); // ���� ������ �ؽ�Ʈ ���ڵ�(Text record)�� ���

      TS = LOCCTR; // TS -> �ؽ�Ʈ ���ڵ忡 ���Ե� ���� �ڵ� �����ּ�
      i = writeLT(); // Literal Table(pool) ���
      writeTEXT(); // Literal Table(pool) �� ���� �ؽ�Ʈ ���ڵ�(Text record)�� ���

      while (MThandlep < MTABp) { // ���� ���ڵ�(Modification record)�� ����Ѵ�
        fprintf(fObj, "%s", MTAB[MThandlep]);
        MThandlep++;
      }
      if (secctr == 0) // �⺻ ����(default section)
        fprintf(fObj, "E%06X\n\n", FEI); // ���� ���ڵ�(End record)
      else
        fprintf(fObj, "E\n\n");
      break;
    }
  }

  fclose(fLst);
  fclose(fObj);
  return (0);
}

void main(int argc, char *argv[])
{
  char filename[30] = "input_visible.txt";

  if (argc > 1)
    strcpy(filename, argv[1]); // ������� �ҽ����ϸ�
  else {
    printf("SIC/XE assembler: No input files\n");
    exit(1);
  }

  // �ҽ����� �ڵ鷯 fSrc
  fSrc = fopen(filename, "r");
  while (fSrc == NULL) {
    printf("Can't find File: %s\n", filename);
    exit(1);
  }

  // Pass 1 ����
  if (pass1(filename) != 0)
    exit(1);

  rewind(fSrc); // �ҽ������� �����͸� �� ������ ������

  // Pass 2 ����
  if (pass2(filename) != 0)
    exit(1);

  fclose(fSrc);
}
