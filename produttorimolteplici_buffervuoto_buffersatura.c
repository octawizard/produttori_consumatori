  #include "CUnit/CUnit.h"
  #include "CUnit/Basic.h"
  //#include "CUnit/Automated.h"
  //#include "CUnit/Console.h"
  #include <pthread.h>
  #include <semaphore.h>
  #include "hwc1.c"
  #include <stdio.h>

  buffer_t* buffer;
  msg_t* msg_0;
  msg_t* msg_1;
  int checkpoint_0;
  int checkpoint_1;

  /* Test Suite setup and cleanup functions: */

  /* funzioni init e clean delle suite suite_produttorimolteplici_buffervuoto_bloccante_buffersatura e suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura */
  int init_suite_produttorimolteplici_buffervuoto(void) { 
    buffer = buffer_init(2);
    msg_0 = msg_init_string("messaggio 0");
    msg_1 = msg_init_string("messaggio 1");
    checkpoint_0 = 0;
    checkpoint_1 = 0;
    return 0; 
  }

  int clean_suite_produttorimolteplici_buffervuoto(void) { 
    msg_destroy_string(msg_0);
    msg_destroy_string(msg_1);
    buffer_destroy(buffer);
    return 0; 
  }

  /************* Test case functions ****************/

  /* test della suite suite_produttorimolteplici_buffervuoto_bloccante_buffersatura */
  void test_iniziale_semafori_putbloccante_buffervuoto_buffersatura (void){
    //verifico lo stato del buffer: vuote=2 e piene=0
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (0 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (2 == i);
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

  void test_produttorimolteplici_putbloccante_buffervuoto_buffersatura(void) {
    pthread_t thread0;
    pthread_t thread1;
    msg_t* ret_msg_0;
    msg_t* ret_msg_1;
    //nel buffer non sono presenti messaggi
    CU_ASSERT (NULL == ((buffer->buf)[0]).content);
    //SOLLECITAZIONE: lancio due thread putbloccante per inserire il loro messaggio nel buffer
    pthread_create (&thread0, NULL, thread_function_produttore_bloccante_0, msg_0);
    pthread_create (&thread1,NULL,thread_function_produttore_bloccante_1, msg_1);
    pthread_join(thread0,(void*) &ret_msg_0);
    pthread_join(thread1,(void*) &ret_msg_1);
    //verifico che la somma dei checkpoint valga sempre 2
    CU_ASSERT_EQUAL (checkpoint_0+checkpoint_1, 2);
    //inoltre verifico di aver inserito nel buffer i dei due messaggi
    CU_ASSERT (NULL != ((buffer->buf)[0]).content);
    CU_ASSERT (NULL != ((buffer->buf)[1]).content);
    CU_ASSERT_STRING_NOT_EQUAL ( (char *)((buffer->buf)[0]).content, (char *)((buffer->buf)[1]).content);
    CU_ASSERT_EQUAL (buffer->D, 0); //incrementi circolari
    CU_ASSERT_EQUAL (buffer->T, 0);
  }

  void test_finale_semafori_putbloccante_buffervuoto_buffersatura (void){
    //verifico il nuovo stato del buffer: vuote=0 e piene=2
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (2 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (0 == i);
  }

  /* test della suite suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura */
  void test_iniziale_semafori_putnonbloccante_buffervuoto_buffersatura (void){
    //verifico lo stato del buffer: vuote=2 e piene=0
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (0 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (2 == i);
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

  void test_produttorimolteplici_putnonbloccante_buffervuoto(void) {
    pthread_t thread0;
    pthread_t thread1;
    msg_t* ret_msg_0;
    msg_t* ret_msg_1;
    //nel buffer non sono presenti messaggi
    CU_ASSERT (NULL == ((buffer->buf)[0]).content);
    //SOLLECITAZIONE: lancio due thread putbloccante per inserire il loro messaggio nel buffer
    pthread_create (&thread0, NULL, thread_function_produttore_nonbloccante_0, msg_0);
    pthread_create (&thread1,NULL,thread_function_produttore_nonbloccante_1, msg_1);
    pthread_join(thread0,(void*) &ret_msg_0);
    pthread_join(thread1,(void*) &ret_msg_1);
    //verifico che la somma dei checkpoint valga sempre 2
    CU_ASSERT_EQUAL (checkpoint_0+checkpoint_1, 2);
    //inoltre verifico di aver inserito nel buffer i dei due messaggi
    CU_ASSERT (NULL != ((buffer->buf)[0]).content);
    CU_ASSERT (NULL != ((buffer->buf)[1]).content);
    CU_ASSERT_STRING_NOT_EQUAL ( (char *)((buffer->buf)[0]).content, (char *)((buffer->buf)[1]).content);
    CU_ASSERT_EQUAL (buffer->D, 0);   //incrementi circolari
    CU_ASSERT_EQUAL (buffer->T, 0);
  }

  void test_finale_semafori_putnonbloccante_buffervuoto_buffersatura (void){
    //verifico il nuovo stato del buffer: vuote=1 e piene=2
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (2 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (0 == i);
  }

  /************* Test Runner Code goes here **************/

  int main ( void )
  {
     CU_pSuite suite_produttorimolteplici_buffervuoto_bloccante_buffersatura = NULL;
     CU_pSuite suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura = NULL;
     /* initialize the CUnit test registry */
     if ( CUE_SUCCESS != CU_initialize_registry() )
        return CU_get_error();

     /* add suite_produttorimolteplici_buffervuoto_bloccante_buffersatura to the registry */
     suite_produttorimolteplici_buffervuoto_bloccante_buffersatura = CU_add_suite( "Produzione concorrente di molteplici messaggi in un buffer non unitario vuoto; il buffer si satura in corso - uso di chiamate bloccanti", init_suite_produttorimolteplici_buffervuoto, clean_suite_produttorimolteplici_buffervuoto );
     if ( NULL == suite_produttorimolteplici_buffervuoto_bloccante_buffersatura ) {
        CU_cleanup_registry();
        return CU_get_error();
     }
     /* add the tests to the suite suite_produttorimolteplici_buffervuoto_bloccante_buffersatura */
     if ( (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_bloccante_buffersatura, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_putbloccante_buffervuoto_buffersatura)) ||
          (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_bloccante_buffersatura, "Lancio dei due thread produttori", test_produttorimolteplici_putbloccante_buffervuoto_buffersatura)) ||
          (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_bloccante_buffersatura, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_putbloccante_buffervuoto_buffersatura))
      )
     {
        CU_cleanup_registry();
        return CU_get_error();
     }

      /* add suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura to the registry */
     suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura = CU_add_suite( "Produzione concorrente di molteplici messaggi in un buffer non unitario vuoto; il buffer si satura in corso - uso di chiamate non bloccanti", init_suite_produttorimolteplici_buffervuoto, clean_suite_produttorimolteplici_buffervuoto);
     if ( NULL == suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura ) {
        CU_cleanup_registry();
        return CU_get_error();
     }
     /* add the tests to the suite suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura */
     if ( (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_putnonbloccante_buffervuoto_buffersatura)) ||
          (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura, "Lancio dei due thread produttori", test_produttorimolteplici_putnonbloccante_buffervuoto)) ||
          (NULL == CU_add_test(suite_produttorimolteplici_buffervuoto_nonbloccante_buffersatura, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_putnonbloccante_buffervuoto_buffersatura))
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