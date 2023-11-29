#include "my_assembler_20194318.c"
int main(void){
    // �ʱ�ȭ
    if (init_my_assembler() != 0) {
        fprintf(stderr, "Assembler initialization failed.\n");
        return 1;
    }

    //Pass 1 ����
    if (assem_pass1() != 0) {
        fprintf(stderr, "Pass 1 failed.\n");
        return 1;
    }
    // Pass 2 ����
    if (assem_pass2() != 0) {
        fprintf(stderr, "Pass 2 failed.\n");
        return 1;
    }

    return 0;
}
