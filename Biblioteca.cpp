#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* --------------------------------------------------------------- */
/* Estrutura do Livro */
typedef struct {
    char titulo[60];
    char autor[40];
    char isbn[20];
} Livro;

/* --------------------------------------------------------------- */
void limpa_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* --------------------------------------------------------------- */
void ler_string(char *s, int tam) {
    fgets(s, tam, stdin);
    s[strcspn(s, "\n")] = '\0';
}

/* --------------------------------------------------------------- */
int tamanho(FILE *arq) {
    long pos = ftell(arq);
    fseek(arq, 0, SEEK_END);
    long fim = ftell(arq);
    fseek(arq, pos, SEEK_SET);
    return (int)(fim / sizeof(Livro));
}

/* ----------------------Cadastrar-------------------------------- */
void cadastrar(FILE *arq) {
    Livro L;
    char confirma;

    printf("\n=== Cadastrar Livro ===\n");
    printf("Registro numero: %d\n", tamanho(arq) + 1);

    printf("Titulo: ");
    ler_string(L.titulo, sizeof(L.titulo));

    printf("Autor: ");
    ler_string(L.autor, sizeof(L.autor));

    printf("ISBN: ");
    ler_string(L.isbn, sizeof(L.isbn));

    printf("Confirmar (s/n)? ");
    if (scanf("%c", &confirma) != 1) { limpa_buffer(); return; }
    limpa_buffer();

    if (toupper(confirma) == 'S') {
        fseek(arq, 0, SEEK_END);
        fwrite(&L, sizeof(Livro), 1, arq);
        fflush(arq);
        printf("Livro cadastrado.\n");
    } else {
        printf("Cancelado.\n");
    }
}

/* -----------------------Consultar-------------------------------- */
void consultar(FILE *arq) {
    int nr;
    Livro L;

    printf("\nCodigo do livro: ");
    if (scanf("%d", &nr) != 1) { limpa_buffer(); return; }
    limpa_buffer();

    int total = tamanho(arq);

    if (nr <= 0 || nr > total) {
        printf("Codigo invalido.\n");
        return;
    }

    long pos = (long)(nr - 1) * sizeof(Livro);
    fseek(arq, pos, SEEK_SET);
    fread(&L, sizeof(Livro), 1, arq);

    printf("\n=== Livro %d ===\n", nr);
    printf("Titulo: %s\n", L.titulo);
    printf("Autor:  %s\n", L.autor);
    printf("ISBN:   %s\n", L.isbn);
}

/* -----------------Gerar Arquivo Texto---------------------------- */
void gerar_arquivo_texto(FILE *arq) {
    FILE *txt = fopen("C:\\ling_c\\Registro.txt", "w");
    Livro L;
    int i, total = tamanho(arq);

    if (!txt) {
        printf("Erro ao criar arquivo texto.\n");
        return;
    }

    fprintf(txt, "Relatorio de Livros\n\n");
    fprintf(txt, "COD  %-30s %-20s %-15s\n",
            "TITULO", "AUTOR", "ISBN");
    fprintf(txt, "---------------------------------------------------------------\n");

    for (i = 0; i < total; i++) {
        fseek(arq, i * sizeof(Livro), SEEK_SET);
        fread(&L, sizeof(Livro), 1, arq);

        fprintf(txt, "%03d %-30s %-20s %-15s\n",
                i + 1, L.titulo, L.autor, L.isbn);
    }

    fclose(txt);

    printf("Arquivo texto gerado com sucesso.\n");
}

/* --------------------Excluir------------------------------------- */
/* Exclusao real: reescreve todo o arquivo sem o registro excluido */
void excluir(FILE **arq) {
    int nr, i, total;
    Livro L;

    printf("\nCodigo do livro a excluir: ");
    if (scanf("%d", &nr) != 1) { limpa_buffer(); return; }
    limpa_buffer();

    total = tamanho(*arq);

    if (nr <= 0 || nr > total) {
        printf("Codigo invalido.\n");
        return;
    }

    FILE *temp = fopen("C:\\ling_c\\temp.dat", "w+b");
    if (!temp) {
        printf("Erro ao criar arquivo temporario.\n");
        return;
    }

    fseek(*arq, 0, SEEK_SET);

    for (i = 0; i < total; i++) {
        fread(&L, sizeof(Livro), 1, *arq);
        if (i != nr - 1) {
            fwrite(&L, sizeof(Livro), 1, temp);
        }
    }

    fclose(*arq);
    fclose(temp);

    remove("C:\\ling_c\\Registro.dat");
    rename("C:\\ling_c\\temp.dat", "C:\\ling_c\\Registro.dat");

    *arq = fopen("C:\\ling_c\\Registro.dat", "r+b");

    printf("Livro excluido com sucesso.\n");
}

/* ---------------------------MAIN-------------------------------- */
int main(void) {
    FILE *arq = fopen("C:\\ling_c\\Registro.dat", "r+b");
    if (!arq) arq = fopen("C:\\ling_c\\Registro.dat", "w+b");
    if (!arq) {
        printf("Erro ao abrir ou criar arquivo.\n");
        return 1;
    }

    int op;
    do {
        printf("\n========= Biblioteca =========\n");
        printf("1. Cadastrar livro\n");
        printf("2. Consultar livro\n");
        printf("3. Gerar arquivo texto\n");
        printf("4. Excluir registro\n");
        printf("5. Sair\n");
        printf("------------------------------\n");
        printf("Total de livros: %d\n", tamanho(arq));
        printf("Opcao: ");

        if (scanf("%d", &op) != 1) { limpa_buffer(); continue; }
        limpa_buffer();

        switch (op) {
            case 1: cadastrar(arq); break;
            case 2: consultar(arq); break;
            case 3: gerar_arquivo_texto(arq); break;
            case 4: excluir(&arq); break;
            case 5: printf("Saindo...\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 5);

    fclose(arq);

    return 0;
}
