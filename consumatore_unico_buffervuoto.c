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
int checkpoint = 0;

/* Test Suite setup and cleanup functions: */

/* funzioni init e clean delle suite suite_consumatoreunico_buffervuoto_bloccante e suite_consumatoreunico_buffervuoto_nonbloccante */
int init_suite_consumatoreunico_buffervuoto(void) { 
  buffer = buffer_init(1);
  return 0;
}

int clean_suite_consumatoreunico_buffervuoto(void) { 
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

/* test della suite suite_consumatoreunico_buffervuoto_bloccante */
void test_iniziale_semafori_getbloccante_buffervuoto (void){
  //verifico lo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

void* thread_function_consumatore_bloccante (void* arg){
  msg_t* msg = get_bloccante(buffer);
  checkpoint = 1;
  return (void*) msg;
}

//ci sono due livelli di indirezione: il produttore richiede l'esecuzione di putbloccante che a sua volta richiama un thread. dentro la put c'è una join che fa stallare
//CAUSO STALLO: forse devo lanciare due thread, prima quello del produttore, e poi del consumatore per vedere se stalla
void test_consumatoreunico_getbloccante_buffervuoto(void) {
  pthread_t thread_consumatore;
  //nel buffer non sono presenti messaggi
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
  //SOLLECITAZIONE: lancio il thread per inserire un messaggio dentro un buffer pieno
  pthread_create (&thread_consumatore,NULL,thread_function_consumatore_bloccante, buffer);
  sleep(1);
  //pthread_join(thread_consumatore,(void*)&ret_msg_0);  <-- se faccio la join va in stallo
  //verifico che il flusso è in wait controllando il valore checkpoint; se rimane zero, vuol dire che il flusso è ancora bloccato sulla put
  CU_ASSERT_EQUAL(checkpoint, 0);
  //inoltre verifico che il buffer sia ancora vuoto
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
}

void test_finale_semafori_getbloccante_buffervuoto (void){
  //verifico il nuovo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

/* test della suite suite_consumatoreunico_buffervuoto_nonbloccante */
void test_iniziale_semafori_getnonbloccante_buffervuoto (void){
  //verifico lo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

void* thread_function_consumatore_nonbloccante (void* arg){
  msg_t* msg = get_non_bloccante(buffer);
  checkpoint = 1;
  CU_ASSERT_EQUAL(checkpoint, 1);
  return (void*) msg;
}

void test_consumatoreunico_getnonbloccante_buffervuoto(void) {
  pthread_t thread_consumatore;
  msg_t* ret_msg_0;
  //nel buffer non sono presenti messaggi
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
  //SOLLECITAZIONE: lancio il thread per inserire un messaggio dentro un buffer pieno
  pthread_create (&thread_consumatore,NULL,thread_function_consumatore_nonbloccante, buffer);
  pthread_join(thread_consumatore,(void*)&ret_msg_0);  //<--- anche facendo la join il flusso non va in blocco perchè la funz non è bloccante
  //verifico che il flusso NON è in wait controllando il valore checkpoint; se è uguale a 1, vuol dire che il flusso non è in wait
  CU_ASSERT_EQUAL(checkpoint, 1);
  //dovrebbe restituire null
  CU_ASSERT_EQUAL (BUFFER_ERROR , ret_msg_0); //VERIFICARE SE È CORRETTO
  //inoltre verifico che il buffer sia ancora vuoto
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
}

void test_finale_semafori_getnonbloccante_buffervuoto (void){
  //verifico il nuovo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

/************* Test Runner Code goes here **************/

int main ( void )
{
   CU_pSuite suite_consumatoreunico_buffervuoto_bloccante = NULL;
   CU_pSuite suite_consumatoreunico_buffervuoto_nonbloccante = NULL;
   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add suite_consumatoreunico_buffervuoto_bloccante to the registry */
   suite_consumatoreunico_buffervuoto_bloccante = CU_add_suite( "suite_consumatoreunico_buffervuoto_bloccante", init_suite_consumatoreunico_buffervuoto, clean_suite_consumatoreunico_buffervuoto );
   if ( NULL == suite_consumatoreunico_buffervuoto_bloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_consumatoreunico_buffervuoto_bloccante */
   if ( (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_bloccante, "test_iniziale_semafori_getbloccante_buffervuoto", test_iniziale_semafori_getbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_bloccante, "test_consumatoreunico_getbloccante_buffervuoto", test_consumatoreunico_getbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_bloccante, "test_finale_semafori_getbloccante_buffervuoto", test_finale_semafori_getbloccante_buffervuoto))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add suite_consumatoreunico_buffervuoto_nonbloccante to the registry */
   suite_consumatoreunico_buffervuoto_nonbloccante = CU_add_suite( "suite_consumatoreunico_buffervuoto_nonbloccante", init_suite_consumatoreunico_buffervuoto, clean_suite_consumatoreunico_buffervuoto);
   if ( NULL == suite_consumatoreunico_buffervuoto_nonbloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_consumatoreunico_buffervuoto_nonbloccante */
  if ( (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_nonbloccante, "test_iniziale_semafori_getnonbloccante_buffervuoto", test_iniziale_semafori_getnonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_nonbloccante, "test_consumatoreunico_getnonbloccante_buffervuoto", test_consumatoreunico_getnonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_nonbloccante, "test_finale_semafori_getnonbloccante_buffervuoto", test_finale_semafori_getnonbloccante_buffervuoto))
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