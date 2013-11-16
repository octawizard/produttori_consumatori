#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"
#include <pthread.h>
#include <semaphore.h>
#include "hwc1.c"
#include <stdio.h>  // for printf

buffer_t* buffer;
msg_t* msg_0;
msg_t* msg_1;
int checkpoint_0 = 0;
int checkpoint_1 = 0;

/* Test Suite setup and cleanup functions: */

/* funzioni init e clean delle suite suite_produttorimolteplici_buffervuoto_bloccante e suite_produttorimolteplici_buffervuoto_nonbloccante */
int init_suite_produttorimolteplici_buffervuoto(void) { 
  buffer = buffer_init(1);
  msg_0 = msg_init_string("messaggio 0");
  msg_1 = msg_init_string("messaggio 1");
  return 0; 
}

int clean_suite_produttorimolteplici_buffervuoto(void) { 
  buffer_destroy(buffer);
  return 0; 
}

/************* Test case functions ****************/

/*void test_case_sample(void)
{
   CU_ASSERT(CU_TRUE);
   CU_ASSERT_NOT_EQUAL(2, -1);
   CU_ASSERT_STRING_EQUAL("string #1", "string #1");
   CU_ASSERT_STRING_NOT_EQUAL("string #1", "string #2");

   CU_ASSERT(CU_FALSE);
   CU_ASSERT_EQUAL(2, 3);
   CU_ASSERT_STRING_NOT_EQUAL("string #1", "string #1");
   CU_ASSERT_STRING_EQUAL("string #1", "string #2");
}*/

/* test della suite suite_produttorimolteplici_buffervuoto_bloccante */
void test_iniziale_semafori_putbloccante_buffervuoto (void){
  //verifico lo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

void* thread_function_produttore_bloccante_0 (void* arg){
  params_t* p = (params_t*) arg;
  buffer_t* buffer= p->buffer;
  msg_t* msg = p->msg;
  msg_t* ret_msg_0 = put_bloccante(buffer, msg);
  checkpoint_0 = 1;
  return (void*) ret_msg_0;
}

void* thread_function_produttore_bloccante_1 (void* arg){
  params_t* p = (params_t*) arg;
  buffer_t* buffer= p->buffer;
  msg_t* msg = p->msg;
  msg_t* ret_msg_1 = put_bloccante(buffer, msg);
  checkpoint_1 = 1;
  return (void*) ret_msg_1;
}

void test_produttorimolteplici_putbloccante_buffervuoto(void) {
  pthread_t thread0;
  pthread_t thread1;
  //nel buffer non sono presenti messaggi
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
  //SOLLECITAZIONE: lancio due thread putbloccante per inserire il loro messaggio nel buffer
  params_t* par_0 = (params_t*)malloc( sizeof(params_t));
  par_0->buffer = buffer;
  par_0->msg = msg_0;
  params_t* par_1 = (params_t*)malloc( sizeof(params_t));
  par_1->buffer = buffer;
  par_1->msg = msg_1;
  pthread_create (&thread0, NULL, thread_function_produttore_bloccante_0, par_0);
  pthread_create (&thread1,NULL,thread_function_produttore_bloccante_1, par_1);
  //verifico che la somma dei checkpoint valga sempre 1; ciò vuol dire che uno dei due flussi è ancora in wait
  sleep(1); //do tempo ai due flussi di procedere, ma uno dei due rimarrà in blocco
  //printf("valore dei checkpoint_0 = %d; checkpoint_1=%d \n", checkpoint_0, checkpoint_1);
  CU_ASSERT_EQUAL (checkpoint_0+checkpoint_1, 1);
  //inoltre verifico di aver inserito nel buffer almeno uno dei due messaggi
  CU_ASSERT (NULL != ((buffer->buf)[0]).content);
  if (checkpoint_0 == 1)
    CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg_0->content));
  if (checkpoint_1 == 1)
    CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg_1->content));
}

void test_finale_semafori_putbloccante_buffervuoto (void){
  //verifico il nuovo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

/* test della suite suite_produttorimolteplici_buffervuoto_nonbloccante */
void test_iniziale_semafori_putnonbloccante_buffervuoto (void){
  //verifico lo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

void* thread_function_produttore_nonbloccante_0 (void* arg){
  params_t* p = (params_t*) arg;
  buffer_t* buffer= p->buffer;
  msg_t* msg = p->msg;
  msg_t* ret_msg_0 = put_non_bloccante(buffer, msg);
  return (void*) ret_msg_0;
}

void* thread_function_produttore_nonbloccante_1 (void* arg){
  params_t* p = (params_t*) arg;
  buffer_t* buffer= p->buffer;
  msg_t* msg = p->msg;
  msg_t* ret_msg_1 = put_non_bloccante(buffer, msg);
  return (void*) ret_msg_1;
}

void test_produttorimolteplici_putnonbloccante_buffervuoto(void) {
  pthread_t thread0;
  pthread_t thread1;
  msg_t* ret_msg_0;
  msg_t* ret_msg_1;
  //nel buffer non sono presenti messaggi
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
  //SOLLECITAZIONE: lancio due thread putnonbloccante per inserire il loro messaggio nel buffer
  params_t* par_0 = (params_t*)malloc( sizeof(params_t));
  par_0->buffer = buffer;
  par_0->msg = msg_0;
  params_t* par_1 = (params_t*)malloc( sizeof(params_t));
  par_1->buffer = buffer;
  par_1->msg = msg_1;
  pthread_create (&thread0, NULL, thread_function_produttore_nonbloccante_0, par_0);
  pthread_create (&thread1,NULL,thread_function_produttore_nonbloccante_1, par_1);
  pthread_join(thread1,(void*) &ret_msg_1);
  pthread_join(thread0,(void*) &ret_msg_0);
  //verifico di aver inserito nel buffer uno dei due messaggi; uno dei due flussi ha restituito NULL
  CU_ASSERT (NULL != ((buffer->buf)[0]).content);
  if (ret_msg_0 == NULL)
    CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg_1->content));
  if (ret_msg_1 == NULL)
    CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg_0->content));
}

void test_finale_semafori_putnonbloccante_buffervuoto (void){
  //verifico il nuovo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

/************* Test Runner Code goes here **************/

int main ( void )
{
   CU_pSuite suite_produttorimolteplici_buffervuoto_bloccante = NULL;
   CU_pSuite suite_produttorimolteplici_buffervuoto_nonbloccante = NULL;
   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add suite_produttorimolteplici_buffervuoto_bloccante to the registry */
   suite_produttorimolteplici_buffervuoto_bloccante = CU_add_suite( "suite_produttorimolteplici_buffervuoto_bloccante", init_suite_produttorimolteplici_buffervuoto, clean_suite_produttorimolteplici_buffervuoto );
   if ( NULL == suite_produttorimolteplici_buffervuoto_bloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttorimolteplici_buffervuoto_bloccante */
   if ( (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_bloccante, "test_iniziale_semafori_putbloccante_buffervuoto", test_iniziale_semafori_putbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_bloccante, "test_produttorimolteplici_putbloccante_buffervuoto", test_produttorimolteplici_putbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_bloccante, "test_finale_semafori_putbloccante_buffervuoto", test_finale_semafori_putbloccante_buffervuoto))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add suite_produttorimolteplici_buffervuoto_nonbloccante to the registry */
   suite_produttorimolteplici_buffervuoto_nonbloccante = CU_add_suite( "suite_produttorimolteplici_buffervuoto_nonbloccante", init_suite_produttorimolteplici_buffervuoto, clean_suite_produttorimolteplici_buffervuoto);
   if ( NULL == suite_produttorimolteplici_buffervuoto_nonbloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttorimolteplici_buffervuoto_nonbloccante */
   if ( (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_nonbloccante, "test_iniziale_semafori_putnonbloccante_buffervuoto", test_iniziale_semafori_putnonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_nonbloccante, "test_produttorimolteplici_putnonbloccante_buffervuoto", test_produttorimolteplici_putnonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_nonbloccante, "test_finale_semafori_putnonbloccante_buffervuoto", test_finale_semafori_putnonbloccante_buffervuoto))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");
/*
   // Run all tests using the automated interface
   CU_automated_run_tests();
   CU_list_tests_to_file();

   // Run all tests using the console interface
   CU_console_run_tests();
*/
   /* Clean up registry and return */
   CU_cleanup_registry();
   return CU_get_error();
}


//entrambe le suite hanno la stessa init e la stessa clean