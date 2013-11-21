#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"
#include <pthread.h>
#include <semaphore.h>
#include "hwc1.c"
#include <stdio.h>

#define SUSPENSIONTIME 3

buffer_t* buffer;
msg_t* msg_0;
int checkpoint;

/* Test Suite setup and cleanup functions: */

/* funzioni init e clean delle suite suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante e suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante */
int init_suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst(void) { 
  buffer = buffer_init(1);
  msg_t* msg = msg_init_string("messaggio 0");
  msg_0 = put_bloccante(buffer, msg);  //già testata
  checkpoint = 0;
  return 0;
}

int clean_suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst(void) { 
  buffer_destroy(buffer);
  return 0; 
}

/************* Test case functions ****************/

/* test della suite suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante */
void test_iniziale_semafori_bloccante_bufferpieno (void){
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

void* thread_function_consumatore_bloccante (void* arg){
  msg_t* msg = get_bloccante(buffer);
  return (void*) msg;
}

//cosa vuol dire 
void test_consumatoreunico_produttoreunico_putbloccante_getbloccante_bufferpieno_produttorefirst(void) {
  pthread_t mythreadP;
  pthread_t mythreadC;
  msg_t* ret_msg_1;
  msg_t* ret_msg_0;
  msg_t* msg_1 = msg_init_string("messaggio 1");
  CU_ASSERT_STRING_EQUAL (((char *)msg_1->content), "messaggio 1");
  //lancio la PUT per prima, poi la get
  pthread_create (&mythreadP, NULL, thread_function_produttore_bloccante, msg_1);
  sleep(SUSPENSIONTIME);
  //verifico che la put è in blocco
  CU_ASSERT_EQUAL (checkpoint, 0);
  pthread_create (&mythreadC,NULL,thread_function_consumatore_bloccante, NULL);
  pthread_join(mythreadP,(void*) &ret_msg_1);
  pthread_join(mythreadC,(void*) &ret_msg_0);
  //verifico che il messaggio estratto dal buffer sia quello atteso
  CU_ASSERT_STRING_EQUAL (((char *)ret_msg_0->content), ((char *)msg_0->content));  //dovrebbe restituire messaggio 0
  //verifico che il messaggio inserito nel buffer sia quello atteso; la put va a buon fine solo dopo la get che lo sblocca
  CU_ASSERT_STRING_EQUAL ((char *)((buffer->buf)[0]).content, ((char *)msg_1->content));
  CU_ASSERT_STRING_EQUAL (((char *)ret_msg_1->content), ((char *)msg_1->content));
}

void test_finale_semafori_bloccante_bufferpieno (void){
  //verifico il nuovo stato del buffer: vuote=0 e piene=1
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (1 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (0 == i);
}

/* test della suite suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante */
void test_iniziale_semafori_nonbloccante_buffervuoto (void){
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
    return (void*) msg_1;
}

void* thread_function_consumatore_nonbloccante (void* arg){
    msg_t* msg = get_non_bloccante(buffer);
    return (void*) msg;
}

void test_consumatoreunico_produttoreunico_putnonbloccante_getnonbloccante_bufferpieno_produttorefirst(void) {
  pthread_t mythreadP;
  pthread_t mythreadC;
  msg_t* ret_msg_1;
  msg_t* ret_msg_0;
  msg_t* msg_1 = msg_init_string("messaggio 1");
  CU_ASSERT_STRING_EQUAL (((char *)msg_1->content), "messaggio 1");
  //lancio la get per prima, poi la put
  pthread_create (&mythreadP, NULL, thread_function_produttore_nonbloccante, msg_1);
  sleep(SUSPENSIONTIME);       // devo introdurlo per imporre la specifica del test "prima il produttore"
  pthread_create (&mythreadC,NULL,thread_function_consumatore_nonbloccante, NULL);
  pthread_join(mythreadP,(void*) &ret_msg_1);
  pthread_join(mythreadC,(void*) &ret_msg_0);
  //verifico che il messaggio estratto dal buffer sia quello atteso
  CU_ASSERT_STRING_EQUAL (((char *)ret_msg_0->content), ((char *)msg_0->content));  //dovrebbe restituire messaggio 0
  //verifico che l'esecuzione della put sia fallita
  CU_ASSERT_EQUAL (ret_msg_1, NULL);
}

void test_finale_semafori_nonbloccante_buffervuoto (void){
  //verifico il nuovo stato del buffer: vuote=1 e piene=0 perchè la put fallisce
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

/************* Test Runner Code goes here **************/

int main ( void )
{
   CU_pSuite suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante = NULL;
   CU_pSuite suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante = NULL;
   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante to the registry */
   suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante = CU_add_suite( "Consumazione e produzione concorrente di un messaggio da un buffer unitario; prima il produttore - uso di chiamate bloccanti", init_suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst, clean_suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst );
   if ( NULL == suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante */
   if ( (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_bloccante_bufferpieno)) ||
        (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante, "Prima produzione di un messaggio poi estrazione di un messaggio", test_consumatoreunico_produttoreunico_putbloccante_getbloccante_bufferpieno_produttorefirst)) ||
        (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_bloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_bloccante_bufferpieno))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

    /* add suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante to the registry */
   suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante = CU_add_suite( "Consumazione e produzione concorrente di un messaggio da un buffer unitario; prima il produttore - uso di chiamate non bloccanti", init_suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst, clean_suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst);
   if ( NULL == suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante */
   if ( (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_nonbloccante_buffervuoto)) ||
        (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante, "Prima produzione di un messaggio poi estrazione di un messaggio", test_consumatoreunico_produttoreunico_putnonbloccante_getnonbloccante_bufferpieno_produttorefirst)) ||
        (NULL == CU_add_test(suite_produttoreunico_consumatoreunico_bufferpieno_produttorefirst_nonbloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_nonbloccante_buffervuoto))
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