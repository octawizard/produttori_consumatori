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
int checkpoint_p0;
int checkpoint_p1;
int checkpoint_c0;
int checkpoint_c1;

/* Test Suite setup and cleanup functions: */

/* funzioni init e clean delle suite suite_consumatorimolteplici_produttorimolteplici_bufferunitario_bloccante e suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante */
int init_suite_consumatorimolteplici_produttorimolteplici_bufferunitario(void) { 
  buffer = buffer_init(1);
  msg_0 = msg_init_string("messaggio 0");
  msg_1 = msg_init_string("messaggio 1");
  checkpoint_p0 = 0;
  checkpoint_p1 = 0;
  checkpoint_c0 = 0;
  checkpoint_c1 = 0;
  return 0; 
}

int clean_suite_consumatorimolteplici_produttorimolteplici_bufferunitario(void) { 
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

/* test della suite suite_consumatorimolteplici_bufferpieno_bloccante */
void test_iniziale_semafori_bloccante_bufferunitario (void){
  //verifico lo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

void* thread_function_consumatore_bloccante_0 (void* arg){
  msg_t* ret_msg_0 = get_bloccante(buffer);
  checkpoint_c0 = 1;
  return (void*) ret_msg_0;
}

void* thread_function_consumatore_bloccante_1 (void* arg){
  msg_t* ret_msg_1 = get_bloccante(buffer);
  checkpoint_c1 = 1;
  return (void*) ret_msg_1;
}

void* thread_function_produttore_bloccante_0 (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* ret_msg_0 = put_bloccante(buffer, msg);
  checkpoint_p0 = 1;
  return (void*) ret_msg_0;
}

void* thread_function_produttore_bloccante_1 (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* ret_msg_1 = put_bloccante(buffer, msg);
  checkpoint_p1 = 1;
  return (void*) ret_msg_1;
}

void test_consumatorimolteplici_produttorimolteplici_bloccante_bufferunitario(void) {
  pthread_t threadC0;
  pthread_t threadC1;
  pthread_t threadP0;
  pthread_t threadP1;
  msg_t* ret_msg_0;
  msg_t* ret_msg_1;
  //nel buffer non sono presenti messaggi
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
  //SOLLECITAZIONE: lancio due thread getbloccante per leggere dal buffer e due thread putbloccante per scrivere nel buffer
  pthread_create (&threadC0, NULL, thread_function_consumatore_bloccante_0, NULL);
  pthread_create (&threadC1, NULL, thread_function_consumatore_bloccante_1, NULL);
  pthread_create (&threadP0, NULL, thread_function_produttore_bloccante_0, msg_0);
  pthread_create (&threadP1, NULL, thread_function_produttore_bloccante_1, msg_1);
  pthread_join(threadC0, (void*)&ret_msg_0);
  pthread_join(threadC1, (void*)&ret_msg_1);
  //verifico che la somma dei checkpoint valga sempre 4; ovvero tutti i flussi sono terminati in tempo finito
  CU_ASSERT_EQUAL (checkpoint_c0+checkpoint_c1+checkpoint_p0+checkpoint_p1, 4);
  //controllo che le get abbiano letto ognuna un certo messaggio dal buffer
  if ((char *)ret_msg_0->content == "messaggio 0")
    CU_ASSERT_STRING_EQUAL((char *)ret_msg_1->content, ((char *)msg_1->content));
  if ((char *)ret_msg_0->content == "messaggio 1")
    CU_ASSERT_STRING_EQUAL((char *)ret_msg_1->content, ((char *)msg_0->content));
}

void test_finale_semafori_bloccante_bufferunitario (void){
  //verifico il nuovo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

/* test della suite suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante */
//PER PREVEDERE IL COMPORTAMENTO VENGONO FORZATE LE SEQ DI INTERLEAVING CHE PREVEDONO PRIMA LE DUE PUT INSIEME, POI LE DUE GET INSIEME.
//problemi: il test perde un po' di generalità, ma è sufficiente in relazione al fatto che si basa sull'affidabilità dei casi base che lo costituiscono, già testati
void test_iniziale_semafori_nonbloccante_bufferunitario (void){
  //verifico lo stato del buffer: vuote=1 e piene=0
  int i;
  sem_getvalue(&piene, &i);
  CU_ASSERT (0 == i);
  sem_getvalue(&vuote, &i);
  CU_ASSERT (1 == i);
}

void* thread_function_consumatore_nonbloccante_0 (void* arg){
  msg_t* ret_msg_0 = get_bloccante(buffer);
  if (ret_msg_0 == NULL)
    checkpoint_c0 = 1;
  return (void*) ret_msg_0;
}

void* thread_function_consumatore_nonbloccante_1 (void* arg){
  msg_t* ret_msg_1 = get_non_bloccante(buffer);
  if (ret_msg_1 == NULL)
    checkpoint_c1 = 1;
  return (void*) ret_msg_1;
}

void* thread_function_produttore_nonbloccante_0 (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* ret_msg_0 = put_non_bloccante(buffer, msg);
  if (ret_msg_0 == NULL)
    checkpoint_p0 = 1;
  return (void*) ret_msg_0;
}

void* thread_function_produttore_nonbloccante_1 (void* arg){
  msg_t* msg = (msg_t*) arg;
  msg_t* ret_msg_1 = put_non_bloccante(buffer, msg);
  if (ret_msg_1 == NULL)
    checkpoint_p1 = 1;
  return (void*) ret_msg_1;
}

void test_consumatorimolteplici_produttorimolteplici_nonbloccante_bufferunitario(void) {
  pthread_t threadC0;
  pthread_t threadC1;
  pthread_t threadP0;
  pthread_t threadP1;
  msg_t* ret_msg_c0;
  msg_t* ret_msg_c1;
  msg_t* ret_msg_p0;
  msg_t* ret_msg_p1;
  //nel buffer non sono presenti messaggi
  CU_ASSERT (NULL == ((buffer->buf)[0]).content);
  //SOLLECITAZIONE: lancio due thread putnonbloccante per scrivere nel buffer e due thread getnonbloccante per leggere dal buffer
  //l'obiettivo è fare in modo che solo una delle due put e una delle due get vadano regolarmente a buon fine
  pthread_create (&threadP0, NULL, thread_function_produttore_nonbloccante_0, msg_0);
  pthread_create (&threadP1, NULL, thread_function_produttore_nonbloccante_1, msg_1);
  sleep(2);   //forza le seq di interleaving
  pthread_create (&threadC0, NULL, thread_function_consumatore_nonbloccante_0, NULL);
  pthread_create (&threadC1, NULL, thread_function_consumatore_nonbloccante_1, NULL);
  pthread_join(threadC0,(void*) &ret_msg_c0);
  pthread_join(threadC1,(void*) &ret_msg_c1);
  pthread_join(threadP0,(void*) &ret_msg_p0);
  pthread_join(threadP1,(void*) &ret_msg_p1);
  //verifico che al massimo un produttore e al massimo un consumatore abbiano portato a termine le operazioni
  CU_ASSERT_EQUAL (checkpoint_c0+checkpoint_c1, 1); //il checkpoint vale uno se la funzione corrispondente non è andata a buon fine
  CU_ASSERT_EQUAL (checkpoint_p0+checkpoint_p1, 1);
  if (checkpoint_p0 == 0){  //flusso p0 andato a termine regolarmente dopo l'inserimento nel buffer
    CU_ASSERT_STRING_EQUAL (((char *)ret_msg_p0->content), ((char *)msg_0->content));
    if (checkpoint_c0 == 0)
      CU_ASSERT_STRING_EQUAL (((char *)ret_msg_c0->content), ((char *)msg_0->content));
    if (checkpoint_c1 == 0)
      CU_ASSERT_STRING_EQUAL (((char *)ret_msg_c1->content), ((char *)msg_0->content));
  }
  if (checkpoint_p1 == 0){
    CU_ASSERT_STRING_EQUAL (((char *)ret_msg_p1->content), ((char *)msg_1->content));
    if (checkpoint_c0 == 0)
      CU_ASSERT_STRING_EQUAL (((char *)ret_msg_c0->content), ((char *)msg_1->content));      
    if (checkpoint_c1 == 0)
      CU_ASSERT_STRING_EQUAL (((char *)ret_msg_c1->content), ((char *)msg_1->content));
  }
}

void test_finale_semafori_nonbloccante_bufferunitario (void){
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
   CU_pSuite suite_consumatorimolteplici_produttorimolteplici_bufferunitario_bloccante = NULL;
   CU_pSuite suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante = NULL;
   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add suite_consumatorimolteplici_bufferpieno_bloccante to the registry */
   suite_consumatorimolteplici_produttorimolteplici_bufferunitario_bloccante = CU_add_suite( "suite_consumatorimolteplici_produttorimolteplici_bufferunitario_bloccante", init_suite_consumatorimolteplici_produttorimolteplici_bufferunitario, clean_suite_consumatorimolteplici_produttorimolteplici_bufferunitario );
   if ( NULL == suite_consumatorimolteplici_produttorimolteplici_bufferunitario_bloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_consumatorimolteplici_bufferpieno_bloccante */
   if ( (NULL == CU_add_test(suite_consumatorimolteplici_produttorimolteplici_bufferunitario_bloccante, "test_iniziale_semafori_bloccante_bufferunitario", test_iniziale_semafori_bloccante_bufferunitario)) ||
        (NULL == CU_add_test(suite_consumatorimolteplici_produttorimolteplici_bufferunitario_bloccante, "test_consumatorimolteplici_produttorimolteplici_bloccante_bufferunitario", test_consumatorimolteplici_produttorimolteplici_bloccante_bufferunitario)) ||
        (NULL == CU_add_test(suite_consumatorimolteplici_produttorimolteplici_bufferunitario_bloccante, "test_finale_semafori_bloccante_bufferunitario", test_finale_semafori_bloccante_bufferunitario))
    )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante to the registry */
   suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante = CU_add_suite( "suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante", init_suite_consumatorimolteplici_produttorimolteplici_bufferunitario, clean_suite_consumatorimolteplici_produttorimolteplici_bufferunitario);
   if ( NULL == suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante ) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante */
   if ( (NULL == CU_add_test(suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante, "test_iniziale_semafori_nonbloccante_bufferunitario", test_iniziale_semafori_nonbloccante_bufferunitario)) ||
        (NULL == CU_add_test(suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante, "test_consumatorimolteplici_produttorimolteplici_nonbloccante_bufferunitario", test_consumatorimolteplici_produttorimolteplici_nonbloccante_bufferunitario)) ||
        (NULL == CU_add_test(suite_consumatorimolteplici_produttorimolteplici_bufferunitario_nonbloccante, "test_finale_semafori_nonbloccante_bufferunitario", test_finale_semafori_nonbloccante_bufferunitario))
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