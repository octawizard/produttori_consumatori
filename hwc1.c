#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
//#include "hwc1.h"
#define BUFFER_ERROR (msg_t *) NULL

/* STRUTTURE DATI DEFINITE */
typedef struct msg {
	void* content;			// generico contenuto del messaggio
	struct msg * (*msg_init)(void*);	// creazione msg
	void (*msg_destroy)(struct msg *);	// deallocazione msg
	struct msg * (*msg_copy)(struct msg *); // creazione/copia msg
} msg_t;

typedef struct buffer {
	msg_t* buf;	// il buffer contenente i messaggi; un puntatore ad una struttura dati che deve essere allocata all'atto dell'inizializzazione del buffer
	unsigned int maxsize;
	int D;	// indice di inserimento
	int T;	// indice di estrazione
} buffer_t;

//struttura dati per passare entrambi i parametri al thread
typedef struct params {
	buffer_t* buffer;
	msg_t* msg;
} params_t;

/* FIRME DELLE FUNZIONI UTILIZZATE */
msg_t* msg_copy_string(msg_t* msg);
msg_t* msg_init_string(void* content);
void msg_destroy_string(msg_t* msg);

/* allocazione / deallocazione buffer */
// creazione di un buffer vuoto di dim. max nota
buffer_t* buffer_init(unsigned int maxsize);

// deallocazione di un buffer
void buffer_destroy(buffer_t* buffer);

/* operazioni sul buffer */
// inserimento bloccante: sospende se pieno, quindi effettua l’inserimento non appena si libera dello spazio restituisce il messaggio inserito; N.B.: msg!=null
msg_t* put_bloccante(buffer_t* buffer, msg_t* msg);

// inserimento non bloccante: restituisce BUFFER_ERROR se pieno, altrimenti effettua l’inserimento e restituisce il messaggio inserito; N.B.: msg!=null
msg_t* put_non_bloccante(buffer_t* buffer, msg_t* msg);

// estrazione bloccante: sospende se vuoto, quindi restituisce il valore estratto non appena disponibile
msg_t* get_bloccante(buffer_t* buffer);

// estrazione non bloccante: restituisce BUFFER_ERROR se vuoto ed il valore estratto in caso contrario
msg_t* get_non_bloccante(buffer_t* buffer);

/* VARIABILI GLOBALI */
/*semafori binari per accedere a D e T*/
pthread_mutex_t usoD = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t usoT = PTHREAD_MUTEX_INITIALIZER;
/*piene e vuote indicano quante celle sono piene e quante vuote */
sem_t vuote;	//dovra poi essere inizializzato
sem_t piene;	//dovra poi essere inizializzato

/* IMPLEMENTAZIONE FUNZIONI */
void msg_destroy_string(msg_t* msg) {
	free(msg->content);	// free copia privata
	free(msg);			// free struct
}

msg_t* msg_init_string(void* content) {
	msg_t* new_msg = (msg_t*)malloc( sizeof(msg_t) );	//viene creata una copia "privata" della stringa
	char* string = (char*)content;
	char* new_content = (char*)malloc(strlen(string)+1); // +1 per \0 finale
	strcpy(new_content, string);
	new_msg->content = new_content;
	new_msg->msg_init = msg_init_string;
	new_msg->msg_destroy = msg_destroy_string;
	new_msg->msg_copy = msg_copy_string;
	return new_msg;
}

msg_t* msg_copy_string(msg_t* msg) {
	return msg_init_string( msg->content );
}

buffer_t* buffer_init(unsigned int maxsize){
	buffer_t* buffer = (buffer_t*)malloc( sizeof(buffer_t) );	//alloco il buffer
	msg_t* array = (msg_t *)malloc( sizeof(msg_t)*maxsize);		//alloco l'array di messaggi contenuto nel buffer
	buffer->buf = array;
	buffer->maxsize = maxsize;
	buffer->T = 0;
	buffer->D = 0;
	sem_init(&vuote, 0, maxsize);
	sem_init(&piene, 0, 0);
	return buffer;
}

// deallocazione di un buffer
void buffer_destroy(buffer_t* buffer){
	int i = 0;
	free(buffer->buf);		//dealloco l'array di msg
	free(buffer);			//dopodichè dealloco il buffer
	sem_destroy(&vuote);	//distruggi i semafori
	sem_destroy(&piene);
}

void* thread_function_produttore(void* arg){
	params_t* p = (params_t*) arg;
	buffer_t* buffer = p->buffer;
	msg_t* msg = p->msg;
	sem_wait(&vuote);
	pthread_mutex_lock(&usoD);
		(buffer->buf)[buffer->D] = *msg;
		buffer->D = ((buffer->D)+1) % buffer->maxsize;
	pthread_mutex_unlock(&usoD);
	sem_post(&piene);
	return msg_copy_string(msg);
}

msg_t* put_bloccante(buffer_t* buffer, msg_t* msg){
	//creo una struttura dati per passare entrambi i parametri al thread
	params_t* par = (params_t*)malloc( sizeof(params_t));
	par->buffer = buffer;
	par->msg = msg;
	msg_t* ret_msg;
	pthread_t mythread;
	pthread_create(&mythread,NULL,thread_function_produttore, par);
	pthread_join(mythread,(void*) &ret_msg);
	return ret_msg;
}

void* thread_function_produttore_nb(void* arg){
	params_t* p = (params_t*) arg;
	buffer_t* buffer= p->buffer;
	msg_t* msg = p->msg;
	if (sem_trywait(&vuote) == 0){
		pthread_mutex_lock(&usoD);
		(buffer->buf)[buffer->D] = *msg;
		buffer->D = ((buffer->D)+1) % buffer->maxsize;
		pthread_mutex_unlock(&usoD);
		sem_post(&piene);
		return msg_copy_string(msg);
	}
	else
		return BUFFER_ERROR;
}

msg_t* put_non_bloccante(buffer_t* buffer, msg_t* msg){	//scelta tra sem posix e var condizione; Forse conviene usare i semafori posix che implementano facilmente il concetto di attesa non bloccante
	//creo una struttura dati per passare entrambi i parametri al thread
	params_t* par = (params_t*)malloc( sizeof(params_t));
	par->buffer = buffer;
	par->msg = msg;
	msg_t* ret_msg;
	pthread_t mythread;
	pthread_create(&mythread,NULL,thread_function_produttore_nb, par);
	pthread_join(mythread,(void*)&ret_msg);
	return ret_msg;
}

void* thread_function_consumatore(void* arg){
	msg_t* msg;
	buffer_t* buffer = (buffer_t*) arg;
	sem_wait(&piene);
	pthread_mutex_lock(&usoT);
		msg = msg_copy_string(&(buffer->buf)[buffer->T]);
		//msg = &(buffer->buf)[buffer->T];
		buffer->T = ((buffer->T)+1) % buffer->maxsize;
	pthread_mutex_unlock(&usoT);
	sem_post(&vuote);
	return msg;
}

msg_t* get_bloccante(buffer_t* buffer){
	msg_t* ret_msg;
	pthread_t mythread;
	pthread_create(&mythread,NULL,thread_function_consumatore, buffer);
	pthread_join(mythread,(void*)&ret_msg);
	return ret_msg;
}

void* thread_function_consumatore_nb(void* arg){
	msg_t* msg;
	buffer_t* buffer = (buffer_t*) arg;
	if (sem_trywait(&piene) == 0){
		pthread_mutex_lock(&usoT);
			msg = msg_copy_string(&(buffer->buf)[buffer->T]);
			buffer->T = ((buffer->T)+1) % buffer->maxsize;
		pthread_mutex_unlock(&usoT);
		sem_post(&vuote);
		return msg;
	}
	else
		return BUFFER_ERROR;
}

msg_t* get_non_bloccante(buffer_t* buffer){
	msg_t* ret_msg;
	pthread_t mythread;
	pthread_create(&mythread,NULL,thread_function_consumatore_nb, buffer);
	pthread_join(mythread,(void*)&ret_msg);
	return ret_msg;
}


//COSE STRANE
/*
1) buffer_destroy, devo anche cancellare ogni singolo messaggio? mi dava seg fault facendolo, subito dopo che cancello il primo msg
2) per richiamare direttamente l'header, basta definire le strutture solo nell'header, poi compilare insieme i test insieme a hwc1.
*/