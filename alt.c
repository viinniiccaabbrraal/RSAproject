#include <stdio.h>
#include <gmp.h>
#include <string.h>


void pre_codificar(char my_string[], mpz_t codigo[], int tam)
{
    for (int i = 0; i < tam; i++)
    {
        if (my_string[i] == ' ')
        {
            mpz_set_ui(codigo[i], 28);
        }
        else
        {
            unsigned long int aux = my_string[i]; 
            mpz_set_ui(codigo[i], aux);
            mpz_sub_ui(codigo[i], codigo[i], 63);
        }
    }
    return;
}

void codificar(mpz_t codigo[], int tam, mpz_t e, mpz_t n)
{
    for (int i = 0; i < tam; i++)
    {
        mpz_powm(codigo[i], codigo[i], e, n);
    }
    return;
}

void decodificar(mpz_t codigo[], int tam, mpz_t d, mpz_t n)
{
    for (int i = 0; i < tam; i++)
    {
        mpz_powm(codigo[i], codigo[i], d, n);   
    }
    return;
}

int achando_inverso(mpz_t fi_de_n, mpz_t e, mpz_t d, mpz_t R[], mpz_t Q[], mpz_t m[])
{
    mpz_t a, b, aux;
    mpz_inits(a, b, aux, NULL);
    mpz_set(a, fi_de_n);
    mpz_set(b, e);

    int aconteceu = 0;
    if (mpz_cmp(a, b) < 0)
    {
        mpz_set(aux, a);
        mpz_set(a, b);
        mpz_set(b, aux);

        aconteceu = 1;
    }

    mpz_set(R[0], a); mpz_set(R[1], b);

    mpz_t zero, r;
    mpz_inits(zero, r, NULL);
    mpz_tdiv_r(r, a, b);
    
    int i;
    for (i = 2; mpz_cmp(r, zero); i++)
    {
        mpz_tdiv_q(Q[i], a, b);
        
        mpz_set(aux, b);
        mpz_tdiv_r(b, a, b);
        mpz_set(a, aux);
        mpz_set(R[i], b);

        mpz_tdiv_r(r, a, b);
    }
    mpz_tdiv_q(Q[i], a, b);

    if (aconteceu)
    {
        mpz_set_ui(m[0], 1);
        mpz_set_ui(m[1], 0);
        
        mpz_t prod;
        mpz_init(prod);
        for (int j = 2; j < i; j++)
        {
            mpz_mul(prod, Q[j], m[j - 1]);
            mpz_sub(m[j], m[j - 2], prod);    
        }
        mpz_clear(prod);
    }
    else
    {
        mpz_set_ui(m[0], 0);
        mpz_set_ui(m[1], 1);

        mpz_t prod;
        mpz_init(prod);
        for (int j = 2; j < i; j++)
        {
            mpz_mul(prod, Q[j], m[j - 1]);
            mpz_sub(m[j], m[j - 2], prod);
            
        }
        mpz_clear(prod);
    }
    mpz_clears(a, b, aux, zero, r, NULL);
    return i;
}

signed main()
{
    printf("VOCE TEM TRES OPCOES\n");
    printf("1 - GERAR CHAVE PUBLICA\n");
    printf("2 - ENCRIPTAR\n");
    printf("3 - DESENCRIPTAR\n");

    int option;
    scanf("%d", &option);
    getchar();

    if (option == 1)
    {
        printf("Digite p q e e\n");
        
        mpz_t p, q, e;
        mpz_inits(p, q, e, NULL);
        gmp_scanf("%Zd %Zd %Zd", &p, &q, &e);
        
        mpz_t n;
        mpz_init(n);
        mpz_mul(n, p, q);


        FILE * arq;
        arq = fopen("public-key.txt", "wt");
        gmp_fprintf(arq, "%Zd %Zd", n, e);
        fclose(arq);
        
        mpz_clears(p, q, e, n, NULL);   
    }   

    if (option == 2)
    {
        printf("Digite a mensagem\n");
        char my_string[5000];
        fgets(my_string, 5000, stdin);
        
        int k = strlen(my_string) - 1;
        my_string[k] = '\0';

        printf("Digite a chave publica n e e\n");
        mpz_t n, e;
        mpz_inits(n, e, NULL);

        gmp_scanf("%Zd %Zd", &n, &e);

        mpz_t codigo[10000];
        for (int i = 0; i < k; i++)
        {
            mpz_init(codigo[i]);
        }

        pre_codificar(my_string, codigo, k);

        codificar(codigo, k, e, n);

        FILE * arq;
        arq = fopen("msgEncrip.txt", "wt");
        for (int i = 0; i < k; i++)
        {
            if (i == k - 1)
            {
                gmp_fprintf(arq, "%Zd", codigo[i]);
                break;
            }
            gmp_fprintf(arq, "%Zd ", codigo[i]);
        }    
        fclose(arq);

        mpz_clears(n, e, NULL);
        for (int i = 0; i < k; i++)
        {
            mpz_clear(codigo[i]);
        }
    }

    if (option == 3)
    {
        printf("Digite p q e\n");
        
        mpz_t p, q, e, n, fi_de_n, d;
        
        mpz_inits(p, q, e, n, fi_de_n, d, NULL);
        
        gmp_scanf("%Zd %Zd %Zd", &p, &q, &e);
        
        mpz_mul(n, p, q);
        mpz_sub_ui(p, p, 1);
        mpz_sub_ui(q, q, 1);
        mpz_mul(fi_de_n, p, q);
        
        mpz_t R[100000], Q[100000], m[100000];
        for (int i = 0; i < 100000; i++)
        {
            mpz_inits(R[i], Q[i], m[i], NULL);
        }

        int i = achando_inverso(fi_de_n, e, d, R, Q, m);
        mpz_set(d, m[i - 1]);

        mpz_mod(d, d, fi_de_n);

        mpz_t codigo[100000];
        for (int i = 0; i < 100000; i++)
        {
            mpz_init(codigo[i]);
        }

        FILE * arq = fopen("msgEncrip.txt", "rt");
        int tam = -1;
        do
        {
            tam++;
        } while (gmp_fscanf(arq, "%Zd", &codigo[tam]) != EOF);
        fclose(arq);

        decodificar(codigo, tam, d, n);

        char myString[5000];
        for(int i = 0; i < tam; i++)
        {
            if (mpz_cmp_ui(codigo[i], 28) == 0)
            {
                myString[i] = ' ';
            }
            else
            {
                myString[i] = mpz_get_ui(codigo[i]) + 63;
            }
        }

        arq = fopen("msgDes.txt", "wt");
        fputs(myString, arq);
        fclose(arq);
        

        for (int i = 0; i < 100000; i++)
        {
            mpz_clears(R[i], Q[i], m[i], codigo[i], NULL);
        }

        mpz_clears(p, q, e, n, fi_de_n, d, NULL);
    }
    return 0;
}