#ifndef HWC1_H_INCLUDED
#define HWC1_H_INCLUDED

#define BUFFER_ERROR (msg_t *) NULL;

pthread_mutex_t usoD;
pthread_mutex_t usoT;

sem_t vuote;
sem_t piene;

typedef struct msg
{
    void* content;
    struct msg* (*msg_init)(void*);
    void (*msg_destroy)(struct msg *);
    struct msg* (*msg_copy)(struct msg *);
} msg_t;

typedef struct buffer {
	msg_t* buf;
	unsigned int maxsize;
	int D;
	int T;
} buffer_t;

//struttura dati per passare entrambi i parametri al thread
typedef struct params {
	buffer_t* buffer;
	msg_t* msg;
} params_t;

void msg_destroy_string(msg_t* msg);
msg_t* msg_copy_string(msg_t* msg);
msg_t* msg_init_string(void* content);

buffer_t* buffer_init(unsigned int maxsize);
void buffer_destroy(buffer_t* buffer);

msg_t* put_bloccante(buffer_t* buffer, msg_t* msg);
msg_t* put_non_bloccante(buffer_t* buffer, msg_t* msg);
msg_t* get_bloccante(buffer_t* buffer);
msg_t* get_non_bloccante(buffer_t* buffer);

#endif // HWC1_H_INSSCLUDED