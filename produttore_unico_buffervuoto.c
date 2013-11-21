#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"
#include <pthread.h>
#include <semaphore.h>
#include "hwc1.c"
#include <stdio.h>

buffer_t* buffer;

/* Test Suite setup and cleanup functions: */

/* funzioni init e clean delle suite suite_produttoreunico_buffervuoto_bloccante e suite_produttoreunico_buffervuoto_nonbloccante */
int init_suite_produttoreunico_buffervuoto(void) { 
  buffer = buffer_init(1);
  return 0; 
}

int clean_suite_produttoreunico_buffervuoto(void) { 
  buffer_destroy(buffer);
  return 0; 
}

/************* Test case functions ****************/

/* test della suite suite_produttoreunico_buffervuoto_bloccante */
void test_iniziale_semafori_putbloccante_buffervuoto (void){
  //verifico lo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

void* thread_function_produttore_bloccante (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg = put_bloccante(buffer, msg);
  return (void*) msg;
}

void test_produttoreunico_putbloccante_buffervuoto(void) {
  pthread_t thread;
  msg_t* ret_msg;
  //nel buffer non sono presenti messaggi
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
  //creo il messaggio da inserire nel buffer
  msg_t* msg = msg_init_string("messaggio 0");
  CU_ASSERT_STRING_EQUAL (((char *)msg->content), "messaggio 0");
  //SOLLECITAZIONE: creo un messaggio da inserire nel buffer e lo inserisco
  pthread_create(&thread, NULL, thread_function_produttore_bloccante, msg);
  pthread_join(thread, (void*)&ret_msg);
  //verifico di aver inserito nel buffer lo stesso messaggio che avevo inzializzato e verifico che sia contenuto nel buffer
  CU_ASSERT_STRING_EQUAL (((char *)ret_msg->content), ((char *)msg->content));
  CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg->content));
}

void test_finale_semafori_putbloccante_buffervuoto (void){
  //verifico il nuovo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

/* test della suite suite_produttoreunico_buffervuoto_nonbloccante */
void test_iniziale_semafori_putnonbloccante_buffervuoto (void){
  //verifico lo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}


void* thread_function_produttore_nonbloccante (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg = put_non_bloccante(buffer, msg);
  return (void*) msg;
}

void test_produttoreunico_putnonbloccante_buffervuoto(void) {
  pthread_t thread;
  msg_t* ret_msg;
  //nel buffer non sono presenti messaggi
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
  //creo il messaggio da inserire nel buffer
  msg_t* msg = msg_init_string("messaggio 0");
  CU_ASSERT_STRING_EQUAL (((char *)msg->content), "messaggio 0");
  //SOLLECITAZIONE: creo un messaggio da inserire nel buffer e lo inserisco
  pthread_create(&thread, NULL, thread_function_produttore_nonbloccante, msg);
  pthread_join(thread, (void*)&ret_msg);
  //verifico di aver inserito nel buffer lo stesso messaggio che avevo inzializzato e verifico che sia contenuto nel buffer
  CU_ASSERT_STRING_EQUAL (((char *)ret_msg->content), ((char *)msg->content));
  CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg->content));
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
   CU_pSuite suite_produttoreunico_buffervuoto_bloccante = NULL;
   CU_pSuite suite_produttoreunico_buffervuoto_nonbloccante = NULL;
   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add suite_produttoreunico_buffervuoto_bloccante to the registry */
   suite_produttoreunico_buffervuoto_bloccante = CU_add_suite( "Produzione di un messaggio in un buffer unitario vuoto: uso di chiamate bloccanti", init_suite_produttoreunico_buffervuoto, clean_suite_produttoreunico_buffervuoto );
   if ( NULL == suite_produttoreunico_buffervuoto_bloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttoreunico_buffervuoto_bloccante */
   if ( (NULL == CU_add_test(suite_produttoreunico_buffervuoto_bloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_putbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttoreunico_buffervuoto_bloccante, "Produzione di un messaggio", test_produttoreunico_putbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttoreunico_buffervuoto_bloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_putbloccante_buffervuoto))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add suite_produttoreunico_buffervuoto_nonbloccante to the registry */
   suite_produttoreunico_buffervuoto_nonbloccante = CU_add_suite( "Produzione di un messaggio in un buffer unitario vuoto: uso di chiamate non bloccanti", init_suite_produttoreunico_buffervuoto, clean_suite_produttoreunico_buffervuoto);
   if ( NULL == suite_produttoreunico_buffervuoto_nonbloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttoreunico_buffervuoto_nonbloccante */
   if ( (NULL == CU_add_test(suite_produttoreunico_buffervuoto_nonbloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_putnonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttoreunico_buffervuoto_nonbloccante, "Produzione di un messaggio", test_produttoreunico_putnonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttoreunico_buffervuoto_nonbloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_putnonbloccante_buffervuoto))
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