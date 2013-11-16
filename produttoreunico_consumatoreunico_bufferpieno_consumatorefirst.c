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

/* Test Suite setup and cleanup functions: */

/* funzioni init e clean delle suite suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante e suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante */
int init_suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst(void) { 
  buffer = buffer_init(1);
  msg_t* msg = msg_init_string("messaggio 0");
  msg_0 = put_bloccante(buffer, msg);  //già testata
  return 0;
}

int clean_suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst(void) { 
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

/* test della suite suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante */
void test_iniziale_semafori_bloccante_bufferpieno (void){
  //verifico lo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

void* thread_function_produttore_bloccante (void* arg){
  params_t* p = (params_t*) arg;
  buffer_t* buffer= p->buffer;
  msg_t* msg = p->msg;
  msg_t* msg_1 = put_bloccante(buffer, msg);
  return (void*) msg_1;
}

void* thread_function_consumatore_bloccante (void* arg){
  buffer_t* b = (buffer_t*) arg;
  msg_t* msg = get_bloccante(b);
  return (void*) msg;
}

void test_consumatoreunico_produttoreunico_putbloccante_getbloccante_bufferpieno_consumatorefirst(void) {
  pthread_t mythreadP;
  pthread_t mythreadC;
  msg_t* ret_msg_1;
  msg_t* ret_msg_0;
  msg_t* msg_1 = msg_init_string("messaggio 1");
  CU_ASSERT_STRING_EQUAL (((char *)msg_1->content), "messaggio 1");
  params_t* par = (params_t*)malloc( sizeof(params_t));
  par->buffer = buffer;
  par->msg = msg_1;
  //lancio la get per prima, poi la put
  pthread_create (&mythreadC,NULL,thread_function_consumatore_bloccante, buffer);
  pthread_create (&mythreadP, NULL, thread_function_produttore_bloccante, par);
  pthread_join(mythreadC,(void*) &ret_msg_0);
  pthread_join(mythreadP,(void*) &ret_msg_1);
  //verifico che il messaggio estratto dal buffer sia quello atteso
  CU_ASSERT_STRING_EQUAL (((char *)ret_msg_0->content), ((char *)msg_0->content));  //dovrebbe restituire messaggio 0
  //verifico che il messaggio inserito nel buffer sia quello atteso
  CU_ASSERT_STRING_EQUAL ((char *)((buffer->buf)[0]).content, ((char *)ret_msg_1->content));

}

void test_finale_semafori_bloccante_bufferpieno (void){
  //verifico il nuovo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

/* test della suite suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante */
void test_iniziale_semafori_nonbloccante_buffervuoto (void){
  //verifico lo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

void* thread_function_produttore_nonbloccante (void* arg){
  params_t* p = (params_t*) arg;
  buffer_t* buffer= p->buffer;
  msg_t* msg = p->msg;
  msg_t* msg_1 = put_non_bloccante(buffer, msg);
  return (void*) msg_1;
}

void* thread_function_consumatore_nonbloccante (void* arg){
  buffer_t* b = (buffer_t*) arg;
  msg_t* msg = get_non_bloccante(b);
  return (void*) msg;
}

void test_consumatoreunico_produttoreunico_putnonbloccante_getnonbloccante_bufferpieno_consumatorefirst(void) {
  pthread_t mythreadP;
  pthread_t mythreadC;
  msg_t* ret_msg_1;
  msg_t* ret_msg_0;
  msg_t* msg_1 = msg_init_string("messaggio 1");
  CU_ASSERT_STRING_EQUAL (((char *)msg_1->content), "messaggio 1");
  params_t* par = (params_t*)malloc( sizeof(params_t));
  par->buffer = buffer;
  par->msg = msg_1;
  //lancio la get per prima, poi la put
  pthread_create (&mythreadC,NULL,thread_function_consumatore_nonbloccante, buffer);
  sleep(1);       // devo introdurlo per imporre la specifica del test "prima il consumatore"; in caso contrario segm fault su ret_msg_1
  pthread_create (&mythreadP, NULL, thread_function_produttore_nonbloccante, par);
  pthread_join(mythreadC,(void*) &ret_msg_0);
  pthread_join(mythreadP,(void*) &ret_msg_1);
  //verifico che il messaggio estratto dal buffer sia quello atteso
  CU_ASSERT_STRING_EQUAL (((char *)ret_msg_0->content), ((char *)msg_0->content));  //dovrebbe restituire messaggio 0 
  //verifico che il messaggio inserito nel buffer sia quello atteso
  CU_ASSERT_STRING_EQUAL ((char *)((buffer->buf)[0]).content, ((char *)ret_msg_1->content));
}

void test_finale_semafori_nonbloccante_buffervuoto (void){
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
   CU_pSuite suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante = NULL;
   CU_pSuite suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante = NULL;
   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante to the registry */
   suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante = CU_add_suite( "suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante", init_suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst, clean_suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst );
   if ( NULL == suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante */
   if ( (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante, "test_iniziale_semafori_bloccante_bufferpieno", test_iniziale_semafori_bloccante_bufferpieno)) ||
        (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante, "test_consumatoreunico_produttoreunico_putbloccante_getbloccante_bufferpieno_consumatorefirst", test_consumatoreunico_produttoreunico_putbloccante_getbloccante_bufferpieno_consumatorefirst)) ||
        (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_bloccante, "test_finale_semafori_bloccante_bufferpieno", test_finale_semafori_bloccante_bufferpieno))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante to the registry */
   suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante = CU_add_suite( "suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante", init_suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst, clean_suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst);
   if ( NULL == suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante */
   if ( (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante, "test_iniziale_semafori_nonbloccante_buffervuoto", test_iniziale_semafori_nonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante, "test_consumatoreunico_produttoreunico_putnonbloccante_getnonbloccante_bufferpieno_consumatorefirst", test_consumatoreunico_produttoreunico_putnonbloccante_getnonbloccante_bufferpieno_consumatorefirst)) ||
        (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_consumatorefirst_nonbloccante, "test_finale_semafori_nonbloccante_buffervuoto", test_finale_semafori_nonbloccante_buffervuoto))
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


//SAREBBE UTILE FARE IL CASO GET BLOCCANTE E PUT NON BLOCCANTE
//SAREBBE UTILE FARE IL CASO GET NON BLOCCANTE E PUT BLOCCANTE