#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"
#include <pthread.h>
#include <semaphore.h>
#include "hwc1.c"
#include <stdio.h>
#define SUSPENSIONTIME 5

buffer_t* buffer;
msg_t* msg_0;
int checkpoint = 0;

/* Test Suite setup and cleanup functions: */

/* funzioni init e clean delle suite suite_produttoreunico_bufferpieno_bloccante e suite_produttoreunico_bufferpieno_nonbloccante */
int init_suite_produttoreunico_bufferpieno(void) { 
  buffer = buffer_init(1);
  msg_t* msg = msg_init_string("messaggio 0");
  msg_0 = put_bloccante(buffer, msg);  //già testata
  return 0;
}

int clean_suite_produttoreunico_bufferpieno(void) {
  msg_destroy_string(msg_0);
  buffer_destroy(buffer);
  return 0; 
}

/************* Test case functions ****************/

/* test della suite suite_produttoreunico_bufferpieno_bloccante */
void test_iniziale_semafori_putbloccante_bufferpieno (void){
  //verifico lo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

void* thread_function_produttore_bloccante (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* msg_1 = put_bloccante(buffer, msg);
  checkpoint = 1;
  return (void*) msg_1;
}

//CAUSO STALLO: forse devo lanciare due thread, prima quello del produttore, e poi del consumatore per vedere se stalla
void test_produttoreunico_putbloccante_bufferpieno(void) {
  pthread_t thread_produttore;
  msg_t* msg = msg_init_string("messaggio 1");
  CU_ASSERT_STRING_EQUAL (((char *)msg->content), "messaggio 1");
  //nel buffer sono presenti messaggi
  CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg_0->content));
  //SOLLECITAZIONE: lancio il thread per inserire un messaggio dentro un buffer pieno
  pthread_create (&thread_produttore,NULL,thread_function_produttore_bloccante, msg);
  //verifico che il flusso è in wait controllando il valore checkpoint; se rimane zero, vuol dire che il flusso è ancora bloccato sulla put
  sleep(SUSPENSIONTIME);
  CU_ASSERT_EQUAL(checkpoint, 0);
  //inoltre verifico che il nuovo inserimento nel buffer non sia stato ancora eseguito; per via del flusso in wait
  CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg_0->content));  //qui nel buffer c'è "messaggio 0"
}

void test_finale_semafori_putbloccante_bufferpieno (void){
  //verifico il nuovo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

/* test della suite suite_produttoreunico_bufferpieno_nonbloccante */
void test_iniziale_semafori_putnonbloccante_buffervuoto (void){
  //verifico lo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

void* thread_function_produttore_nonbloccante (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* msg_1 = put_non_bloccante(buffer, msg);
  checkpoint = 1;
  CU_ASSERT_EQUAL(checkpoint, 1);
  return (void*) msg_1;
}

void test_produttoreunico_putnonbloccante_buffervuoto(void) {
  pthread_t thread_produttore;
  msg_t* ret_msg_0;
  msg_t* msg = msg_init_string("messaggio 1");
  CU_ASSERT_STRING_EQUAL (((char *)msg->content), "messaggio 1");
  //nel buffer sono presenti messaggi
  CU_ASSERT_STRING_EQUAL ((char *)(buffer->buf[0].content), ((char *)msg_0->content));
  //SOLLECITAZIONE: lancio il thread per inserire un messaggio dentro un buffer pieno
  pthread_create (&thread_produttore,NULL,thread_function_produttore_nonbloccante, msg);
  pthread_join(thread_produttore,(void*)&ret_msg_0);  //<--- anche facendo la join il flusso non va in blocco perchè la funz non è bloccante
  //verifico che il flusso NON è in wait controllando il valore checkpoint; se è uguale a 1, vuol dire che il flusso non è in wait
  sleep(SUSPENSIONTIME);
  CU_ASSERT_EQUAL(checkpoint, 1);
  //il thread deve restituire null
  CU_ASSERT (NULL == ret_msg_0);
  //inoltre verifico che il nuovo inserimento nel buffer non sia stato ancora eseguito; per via del flusso in wait
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
   CU_pSuite suite_produttoreunico_bufferpieno_bloccante = NULL;
   CU_pSuite suite_produttoreunico_bufferpieno_nonbloccante = NULL;
   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add suite_produttoreunico_bufferpieno_bloccante to the registry */
   suite_produttoreunico_bufferpieno_bloccante = CU_add_suite( "Produzione di un messaggio in un buffer unitario pieno: uso di chiamate bloccanti", init_suite_produttoreunico_bufferpieno, clean_suite_produttoreunico_bufferpieno );
   if ( NULL == suite_produttoreunico_bufferpieno_bloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttoreunico_bufferpieno_bloccante */
   if ( (NULL == CU_add_test(suite_produttoreunico_bufferpieno_bloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_putbloccante_bufferpieno)) ||
        (NULL == CU_add_test(suite_produttoreunico_bufferpieno_bloccante, "Produzione di un messaggio", test_produttoreunico_putbloccante_bufferpieno)) ||
        (NULL == CU_add_test(suite_produttoreunico_bufferpieno_bloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_putbloccante_bufferpieno))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add suite_produttoreunico_bufferpieno_nonbloccante to the registry */
   suite_produttoreunico_bufferpieno_nonbloccante = CU_add_suite( "Produzione di un messaggio in un buffer unitario pieno: uso di chiamate non bloccanti", init_suite_produttoreunico_bufferpieno, clean_suite_produttoreunico_bufferpieno);
   if ( NULL == suite_produttoreunico_bufferpieno_nonbloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttoreunico_bufferpieno_nonbloccante */
   if ( (NULL == CU_add_test(suite_produttoreunico_bufferpieno_nonbloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_putnonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttoreunico_bufferpieno_nonbloccante, "Produzione di un messaggio", test_produttoreunico_putnonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttoreunico_bufferpieno_nonbloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_putnonbloccante_buffervuoto))
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