#include <stdio.h>
#include <string.h>

#define TAM 128
int table[100]
         [TAM];     // armazena indice da producao que sera aplicada na variavel
                    // i-esima se a entrada for j-esima for nao terminal
char terminal[TAM]; // armazena lista de terminais
char nao_terminal[26]; // armazena lista de nao terminais

struct producao { // armazenamento de cada producao, sendo str[] onde a producao
                  // sera armazenada e len o tamanho da producao
  char str[100];
  int len;
} prod[20];

int no_prod;
char first[26][TAM];
char follow[26][TAM];

char first_rhs[100][TAM];

int verificaNaoTerminal(char c) { return c >= 'A' && c <= 'Z'; }

void lerArquivo() {
  FILE *fptr;
  fptr = fopen("test3.txt", "r");
  char buffer[255];
  int i;
  int j;

  while (fgets(buffer, sizeof(buffer), fptr)) {
    printf("%s", buffer);
    j = 0;
    nao_terminal[buffer[0] - 'A'] = 1;
    for (i = 0; i < strlen(buffer) - 1; ++i) {
      if (buffer[i] == '|') {
        ++no_prod;
        prod[no_prod - 1].str[j] = '\0';
        prod[no_prod - 1].len = j;
        prod[no_prod].str[0] = prod[no_prod - 1].str[0];
        prod[no_prod].str[1] = prod[no_prod - 1].str[1];
        prod[no_prod].str[2] = prod[no_prod - 1].str[2];
        j = 3;
      } else {
        prod[no_prod].str[j] = buffer[i];
        ++j;
        if (!verificaNaoTerminal(buffer[i]) && buffer[i] != '-' &&
            buffer[i] != '>') {
          terminal[buffer[i]] = 1;
        }
      }
    }
    prod[no_prod].len = j;
    ++no_prod;
  }
}

void add_FIRST_A_to_FOLLOW_B(char A, char B) {
  int i;

  for (i = 0; i < TAM; ++i) {
    if (i != '^')
      follow[B - 'A'][i] = follow[B - 'A'][i] || first[A - 'A'][i];
  }
}

void add_FOLLOW_A_to_FOLLOW_B(char A, char B) {
  int i;

  for (i = 0; i < TAM; ++i) {
    if (i != '^')
      follow[B - 'A'][i] = follow[B - 'A'][i] || follow[A - 'A'][i];
  }
}

void FOLLOW() {
  int t = 0;
  int i, j, k, x;

  while (t++ < no_prod) {
    for (k = 0; k < 26; ++k) {
      if (!nao_terminal[k])
        continue;

      char nt = k + 'A';

      for (i = 0; i < no_prod; ++i) {
        for (j = 3; j < prod[i].len; ++j) {
          if (nt == prod[i].str[j]) {
            for (x = j + 1; x < prod[i].len; ++x) {
              char sc = prod[i].str[x];

              if (verificaNaoTerminal(sc)) {
                add_FIRST_A_to_FOLLOW_B(sc, nt);
                if (first[sc - 'A']['^'])
                  continue;
              } else {
                follow[nt - 'A'][sc] = 1;
              }

              break;
            }

            if (x == prod[i].len)
              add_FOLLOW_A_to_FOLLOW_B(prod[i].str[0], nt);
          }
        }
      }
    }
  }
}
void add_FIRST_A_to_FIRST_B(char A, char B) {
  int i;

  for (i = 0; i < TAM; ++i) {
    if (i != '^') {
      first[B - 'A'][i] = first[A - 'A'][i] || first[B - 'A'][i];
    }
  }
}

void FIRST() {
  int i, j;
  int t = 0;

  while (t < no_prod) {
    for (i = 0; i < no_prod; ++i) {
      for (j = 3; j < prod[i].len; ++j) {
        char sc = prod[i].str[j];
        if (verificaNaoTerminal(sc)) {
          add_FIRST_A_to_FIRST_B(sc, prod[i].str[0]);

          if (first[sc - 'A']['^'])
            continue;
        } else {
          first[prod[i].str[0] - 'A'][sc] = 1;
        }

        break;
      }

      if (j == prod[i].len)
        first[prod[i].str[0] - 'A']['^'] = 1;
    }

    ++t;
  }
}

void add_FIRST_A_to_FIRST_RHS__B(char A, int B) {
  int i;

  for (i = 0; i < TAM; ++i) {
    if (i != '^')
      first_rhs[B][i] = first[A - 'A'][i] || first_rhs[B][i];
  }
}

void FIRST_RHS() {
  int i, j;
  int t = 0;

  while (t < no_prod) {
    for (i = 0; i < no_prod; ++i) {
      for (j = 3; j < prod[i].len; ++j) {
        char sc = prod[i].str[j];

        if (verificaNaoTerminal(sc)) {
          add_FIRST_A_to_FIRST_RHS__B(sc, i);
          if (first[sc - 'A']['^'])
            continue;
        } else {
          first_rhs[i][sc] = 1;
        }

        break;
      }

      if (j == prod[i].len)
        first_rhs[i]['^'] = 1;
    }

    ++t;
  }
}

int main() {
  lerArquivo();
  follow[prod[0].str[0] - 'A']['$'] = 1;
  FIRST();
  FOLLOW();
  FIRST_RHS();
  int i, j, k;

  printf("\n");
  for (i = 0; i < no_prod; ++i) {
    if (i == 0 || (prod[i - 1].str[0] != prod[i].str[0])) {
      char c = prod[i].str[0];

      printf("FIRST DE %c: ", c);
      for (j = 0; j < TAM; ++j) {
        if (first[c - 'A'][j]) {
          printf("%c ", j);
        }
      }
      printf("\n");
    }
  }
  printf("\n");
  for (i = 0; i < no_prod; ++i) {
    if (i == 0 || (prod[i - 1].str[0] != prod[i].str[0])) {
      char c = prod[i].str[0];

      printf("FOLLOW DE %c: ", c);

      for (j = 0; j < TAM; ++j) {
        if (follow[c - 'A'][j]) {
          printf("%c ", j);
        }
      }

      printf("\n");
    }
  }

  printf("\n");

  for (i = 0; i < no_prod; ++i) {
    printf("FIRST DE %s: ", prod[i].str);

    for (j = 0; j < TAM; ++j) {
      if (first_rhs[i][j]) {
        printf("%c ", j);
      }
    }

    printf("\n");
  }

  terminal['$'] = 1;
  terminal['^'] = 0;

  printf("\n");
  printf("\n\t----------------- TABELA PARSER LL(1) ---------------\n");
  printf("\t--------------------------------------------------------\n");
  printf("%-10s", "");

  for (i = 0; i < TAM; ++i) {
    if (terminal[i])
      printf("%-10c", i);
  }

  printf("\n");

  int p = 0;

  for (i = 0; i < no_prod; ++i) {
    if (i != 0 && (prod[i].str[0] != prod[i - 1].str[0]))
      p = p + 1;

    for (j = 0; j < TAM; ++j) {
      if (first_rhs[i][j] && j != '^') {
        table[p][j] = i + 1;
      } else if (first_rhs[i]['^']) {
        for (k = 0; k < TAM; ++k) {
          if (follow[prod[i].str[0] - 'A'][k]) {
            table[p][k] = i + 1;
          }
        }
      }
    }
  }

  k = 0;
  for (i = 0; i < no_prod; ++i) {
    if (i == 0 || (prod[i - 1].str[0] != prod[i].str[0])) {
      printf("%-10c", prod[i].str[0]);
      for (j = 0; j < TAM; ++j) {
        if (table[k][j]) {
          printf("%-10s", prod[table[k][j] - 1].str);
        } else if (terminal[j]) {
          printf("%-10s", "");
        }
      }

      ++k;

      printf("\n");
    }
  }
}