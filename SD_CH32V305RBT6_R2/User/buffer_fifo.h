/*
 * buffer_fifo.h
 *
 *  Created on: Dec 5, 2024
 *      Author: Aluizio d'Affons�ca Netto
 */

#ifndef USER_BUFFER_FIFO_H_
#define USER_BUFFER_FIFO_H_

//estrutura de controle
typedef struct {
    int in;
    int out;
    int len;
    int bs_len;
    int full;
    int size_v;
    int count;
    unsigned char *ar;
}SBUFF;


//inicializa buffer
void Init_FIFO(SBUFF *b, void *AR, int size_var ,int len);

//retorna n�mero de amotras que entraram no FIFO
int n_sample_in_FIFO(SBUFF *b);

//retorna n�mero de amostras livres
int n_sample_out_FIFO(SBUFF *b);

//adiciona amostra no buffer
void put_sample_FIFO(SBUFF *b, void *s);

//retira amostra do buffer
void get_sample_FIFO(SBUFF *b, void *s);

#endif /* USER_BUFFER_FIFO_H_ */
