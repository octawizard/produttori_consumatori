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
msg_t* msg_2;
msg_t* msg_3;
int checkpoint_0;
int checkpoint_1;
int c;

/* Test Suite setup and cleanup functions: */

/* funzioni init e clean delle suite suite_produttorimolteplici_bufferpieno_bloccante_buffersatura e suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura */
int init_suite_produttorimolteplici_bufferpieno(void) {
  buffer = buffer_init(2);
  msg_0 = msg_init_string("messaggio 0");
  msg_1 = msg_init_string("messaggio 1");
  msg_2 = msg_init_string("messaggio 2");
  msg_3 = msg_init_string("messaggio 3");
  checkpoint_0 = 0;
  checkpoint_1 = 0;
  put_bloccante(buffer, msg_0);
  put_bloccante(buffer, msg_1);
  return 0; 
}

int clean_suite_produttorimolteplici_bufferpieno(void) {
  msg_destroy_string(msg_0);
  msg_destroy_string(msg_1);
  msg_destroy_string(msg_2);
  msg_destroy_string(msg_3);
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

/* test della suite suite_produttorimolteplici_bufferpieno_bloccante_buffersatura */
void test_iniziale_semafori_putbloccante_bufferpieno_buffersatura (void){
  //verifico lo stato del buffer: vuote=0 e piene=2
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (2 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

void* thread_function_produttore_bloccante_0 (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* ret_msg_0 = put_bloccante(buffer, msg);
  checkpoint_0 = 1;
  return (void*) ret_msg_0;
}

void* thread_function_produttore_bloccante_1 (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* ret_msg_1 = put_bloccante(buffer, msg);
  checkpoint_1 = 1;
  return (void*) ret_msg_1;
}

void test_produttorimolteplici_putbloccante_bufferpieno_buffersatura(void) {
  pthread_t thread0;
  pthread_t thread1;
  //SOLLECITAZIONE: lancio due thread putbloccante per inserire il loro messaggio nel buffer
  pthread_create (&thread0, NULL, thread_function_produttore_bloccante_0, msg_2);
  pthread_create (&thread1,NULL,thread_function_produttore_bloccante_1, msg_3);
  //verifico che la somma dei checkpoint valga sempre 0; i flussi sono in wait
  CU_ASSERT_EQUAL (checkpoint_0+checkpoint_1, 0);
  //inoltre verifico che il buffer sia intatto
  CU_ASSERT_STRING_EQUAL ( (char *)((buffer->buf)[0]).content, (char *)msg_0->content);
  CU_ASSERT_STRING_EQUAL ( (char *)((buffer->buf)[1]).content, (char *)msg_1->content);
  pthread_cancel(thread0);  //se non uccido queasti thread, va in stallo sul buffer init della suitcase successiva, al momento della put
  pthread_cancel(thread1);
}

void test_finale_semafori_putbloccante_bufferpieno_buffersatura (void){
  //verifico il nuovo stato del buffer: vuote=0 e piene=2
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (2 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

/* test della suite suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura */
void test_iniziale_semafori_putnonbloccante_bufferpieno_buffersatura (void){
  //verifico lo stato del buffer: vuote=0 e piene=2
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (2 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

void* thread_function_produttore_nonbloccante_0 (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* ret_msg_0 = put_non_bloccante(buffer, msg);
  if (ret_msg_0 != NULL)
    checkpoint_0 = 1;
  return (void*) ret_msg_0;
}

void* thread_function_produttore_nonbloccante_1 (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* ret_msg_1 = put_non_bloccante(buffer, msg);
  if (ret_msg_1 != NULL)
    checkpoint_1 = 1;
  return (void*) ret_msg_1;
}

void test_produttorimolteplici_putnonbloccante_bufferpieno(void) {
  pthread_t thread0;
  pthread_t thread1;
  msg_t* ret_msg_2;
  msg_t* ret_msg_3;
  //SOLLECITAZIONE: lancio due thread putbloccante per inserire il loro messaggio nel buffer
  pthread_create (&thread0, NULL, thread_function_produttore_nonbloccante_0, msg_2);
  pthread_create (&thread1,NULL,thread_function_produttore_nonbloccante_1, msg_3);
  pthread_join(thread0,(void*) &ret_msg_2);
  pthread_join(thread1,(void*) &ret_msg_3);
  //verifico che la somma dei checkpoint valga sempre 0; i due flussi hanno ritornato BUFFER_ERROR
  CU_ASSERT_EQUAL (checkpoint_0+checkpoint_1, 0);
  //inoltre verifico che il buffer sia intatto
  CU_ASSERT_STRING_EQUAL ( (char *)((buffer->buf)[0]).content, (char *)msg_0->content);
  CU_ASSERT_STRING_EQUAL ( (char *)((buffer->buf)[1]).content, (char *)msg_1->content);
}

void test_finale_semafori_putnonbloccante_bufferpieno_buffersatura (void){
  //verifico il nuovo stato del buffer: vuote=0 e piene=2
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (2 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

/************* Test Runner Code goes here **************/

int main ( void )
{
   CU_pSuite suite_produttorimolteplici_bufferpieno_bloccante_buffersatura = NULL;
   CU_pSuite suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura = NULL;
   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add suite_produttorimolteplici_bufferpieno_bloccante_buffersatura to the registry */
   suite_produttorimolteplici_bufferpieno_bloccante_buffersatura = CU_add_suite( "suite_produttorimolteplici_bufferpieno_bloccante_buffersatura", init_suite_produttorimolteplici_bufferpieno, clean_suite_produttorimolteplici_bufferpieno );
   if ( NULL == suite_produttorimolteplici_bufferpieno_bloccante_buffersatura ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttorimolteplici_bufferpieno_bloccante_buffersatura */
   if ( (NULL == CU_add_test(suite_produttorimolteplici_bufferpieno_bloccante_buffersatura, "test_iniziale_semafori_putbloccante_bufferpieno_buffersatura", test_iniziale_semafori_putbloccante_bufferpieno_buffersatura)) ||
        (NULL == CU_add_test(suite_produttorimolteplici_bufferpieno_bloccante_buffersatura, "test_produttorimolteplici_putbloccante_bufferpieno_buffersatura", test_produttorimolteplici_putbloccante_bufferpieno_buffersatura)) ||
        (NULL == CU_add_test(suite_produttorimolteplici_bufferpieno_bloccante_buffersatura, "test_finale_semafori_putbloccante_bufferpieno_buffersatura", test_finale_semafori_putbloccante_bufferpieno_buffersatura))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura to the registry */
   suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura = CU_add_suite( "suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura", init_suite_produttorimolteplici_bufferpieno, clean_suite_produttorimolteplici_bufferpieno);
   if ( NULL == suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura */
   if ( (NULL == CU_add_test(suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura, "test_iniziale_semafori_putnonbloccante_bufferpieno_buffersatura", test_iniziale_semafori_putnonbloccante_bufferpieno_buffersatura)) ||
        (NULL == CU_add_test(suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura, "test_produttorimolteplici_putnonbloccante_bufferpieno", test_produttorimolteplici_putnonbloccante_bufferpieno)) ||
        (NULL == CU_add_test(suite_produttorimolteplici_bufferpieno_nonbloccante_buffersatura, "test_finale_semafori_putnonbloccante_bufferpieno_buffersatura", test_finale_semafori_putnonbloccante_bufferpieno_buffersatura))
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