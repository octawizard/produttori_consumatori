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

  /* funzioni init e clean delle suite suite_consumatoreunico_bufferpieno_bloccante e suite_consumatoreunico_bufferpieno_nonbloccante */
  int init_suite_consumatoreunico_bufferpieno(void) { 
    buffer = buffer_init(1);
    msg_t* msg = msg_init_string("messaggio 0");
    msg_0 = put_bloccante(buffer, msg);  //già testata
    return 0; 
  }

  int clean_suite_consumatoreunico_bufferpieno(void) { 
    msg_destroy_string(msg_0);
    buffer_destroy(buffer);
    return 0; 
  }

  /************* Test case functions ****************/

  /* test della suite suite_consumatoreunico_bufferpieno_bloccante */
  void test_iniziale_semafori_getbloccante_bufferpieno (void){
    //verifico che il buffer sia pieno; vuote=0 e piene=1
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (1 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (0 == i);
  }

  void test_consumatoreunico_getbloccante_bufferpieno(void) {
    //verifico che nella cella sia presente un messaggio
    CU_ASSERT (NULL != ((buffer->buf)[0]).content);
    //SOLLECITAZIONE: estraggo un messaggio dal buffer
    msg_t* msg = get_bloccante(buffer);
    //verifico di aver estratto esattamente lo stesso messaggio che avevo inserito nel buffer
    CU_ASSERT_STRING_EQUAL (((char *)msg->content), ((char *)msg_0->content));
  }

  void test_finale_semafori_getbloccante_bufferpieno (void){
    //verifico il nuovo stato del buffer: vuote=1 e piene=0
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (0 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (1 == i);
  }

  /* test della suite suite_consumatoreunico_bufferpieno_nonbloccante */
  void test_iniziale_semafori_getnonbloccante_bufferpieno (void){
    //verifico che il buffer sia pieno; vuote=0 e piene=1
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (1 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (0 == i);
  }

  void test_consumatoreunico_getnonbloccante_bufferpieno(void) {
    //verifico che nella cella sia presente un messaggio
    CU_ASSERT (NULL != ((buffer->buf)[0]).content);
    //SOLLECITAZIONE: estraggo un messaggio dal buffer
    msg_t* msg = get_non_bloccante(buffer);
    //verifico di aver estratto esattamente lo stesso messaggio che avevo inserito nel buffer
    CU_ASSERT_STRING_EQUAL (((char *)msg->content), ((char *)msg_0->content));
  }

  void test_finale_semafori_getnonbloccante_bufferpieno (void){
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
     CU_pSuite suite_consumatoreunico_bufferpieno_bloccante = NULL;
     CU_pSuite suite_consumatoreunico_bufferpieno_nonbloccante = NULL;
     /* initialize the CUnit test registry */
     if ( CUE_SUCCESS != CU_initialize_registry() )
        return CU_get_error();

     /* add suite_consumatoreunico_bufferpieno_bloccante to the registry */
     suite_consumatoreunico_bufferpieno_bloccante = CU_add_suite( "Consumazione di un solo messaggio da un buffer unitario pieno: uso di chiamate bloccanti", init_suite_consumatoreunico_bufferpieno, clean_suite_consumatoreunico_bufferpieno );
     if ( NULL == suite_consumatoreunico_bufferpieno_bloccante ) {
        CU_cleanup_registry();
        return CU_get_error();
     }
     /* add the tests to the suite suite_consumatoreunico_bufferpieno_bloccante */
     if ( (NULL == CU_add_test(suite_consumatoreunico_bufferpieno_bloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_getbloccante_bufferpieno)) ||
          (NULL == CU_add_test(suite_consumatoreunico_bufferpieno_bloccante, "Estrazione del messaggio dal buffer unitario pieno", test_consumatoreunico_getbloccante_bufferpieno)) ||
          (NULL == CU_add_test(suite_consumatoreunico_bufferpieno_bloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_getbloccante_bufferpieno))
      )
     {
        CU_cleanup_registry();
        return CU_get_error();
     }

    /* add suite_consumatoreunico_bufferpieno_nonbloccante to the registry */
    suite_consumatoreunico_bufferpieno_nonbloccante = CU_add_suite( "Consumazione di un solo messaggio da un buffer unitario pieno: uso di chiamate non bloccanti", init_suite_consumatoreunico_bufferpieno, clean_suite_consumatoreunico_bufferpieno );
     if ( NULL == suite_consumatoreunico_bufferpieno_nonbloccante ) {
        CU_cleanup_registry();
        return CU_get_error();
     }
     /* add the tests to the suite suite_consumatoreunico_bufferpieno_nonbloccante */
     if ( (NULL == CU_add_test(suite_consumatoreunico_bufferpieno_nonbloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_getnonbloccante_bufferpieno)) ||
          (NULL == CU_add_test(suite_consumatoreunico_bufferpieno_nonbloccante, "Estrazione del messaggio dal buffer unitario pieno", test_consumatoreunico_getnonbloccante_bufferpieno)) ||
          (NULL == CU_add_test(suite_consumatoreunico_bufferpieno_nonbloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_getnonbloccante_bufferpieno))
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