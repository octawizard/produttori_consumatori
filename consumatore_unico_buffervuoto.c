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

  void test_consumatoreunico_getbloccante_buffervuoto(void) {
    pthread_t thread_consumatore;
    //nel buffer non sono presenti messaggi
    CU_ASSERT (NULL == ((buffer->buf)[0]).content);
    //SOLLECITAZIONE: lancio il thread per leggere un messaggio da un buffer vuoto
    pthread_create (&thread_consumatore,NULL,thread_function_consumatore_bloccante, buffer);
    sleep(SUSPENSIONTIME);   //aspetto un tempo ragionevole per assumere che il flusso o abbia terminato o sia in attesa
    //verifico che il flusso è in wait controllando il valore checkpoint; se rimane zero, vuol dire che il flusso è ancora bloccato sulla get
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
    //SOLLECITAZIONE: lancio il thread per leggere un messaggio da un buffer vuoto
    pthread_create (&thread_consumatore,NULL,thread_function_consumatore_nonbloccante, buffer);
    pthread_join(thread_consumatore,(void*)&ret_msg_0);  //<--- anche facendo la join il flusso non va in blocco perchè la funz non è bloccante
    //verifico che il flusso NON è in wait controllando il valore checkpoint; se è uguale a 1, vuol dire che il flusso non è in wait
    CU_ASSERT_EQUAL(checkpoint, 1);
    //dovrebbe restituire null
    CU_ASSERT (NULL == ret_msg_0);
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
     suite_consumatoreunico_buffervuoto_bloccante = CU_add_suite( "Consumazione di un solo messaggio da un buffer unitario vuoto: uso di chiamate bloccanti", init_suite_consumatoreunico_buffervuoto, clean_suite_consumatoreunico_buffervuoto );
     if ( NULL == suite_consumatoreunico_buffervuoto_bloccante ) {
        CU_cleanup_registry();
        return CU_get_error();
     }
     /* add the tests to the suite suite_consumatoreunico_buffervuoto_bloccante */
     if ( (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_bloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_getbloccante_buffervuoto)) ||
          (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_bloccante, "Estrazione del messaggio dal buffer unitario vuoto", test_consumatoreunico_getbloccante_buffervuoto)) ||
          (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_bloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_getbloccante_buffervuoto))
      )
     {
        CU_cleanup_registry();
        return CU_get_error();
     }

      /* add suite_consumatoreunico_buffervuoto_nonbloccante to the registry */
     suite_consumatoreunico_buffervuoto_nonbloccante = CU_add_suite( "Consumazione di un solo messaggio da un buffer unitario vuoto: uso di chiamate non bloccanti", init_suite_consumatoreunico_buffervuoto, clean_suite_consumatoreunico_buffervuoto);
     if ( NULL == suite_consumatoreunico_buffervuoto_nonbloccante ) {
        CU_cleanup_registry();
        return CU_get_error();
     }
     /* add the tests to the suite suite_consumatoreunico_buffervuoto_nonbloccante */
    if ( (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_nonbloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_getnonbloccante_buffervuoto)) ||
          (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_nonbloccante, "Estrazione del messaggio dal buffer unitario vuoto", test_consumatoreunico_getnonbloccante_buffervuoto)) ||
          (NULL == CU_add_test(suite_consumatoreunico_buffervuoto_nonbloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_getnonbloccante_buffervuoto))
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