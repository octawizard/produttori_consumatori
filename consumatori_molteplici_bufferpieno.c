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
  msg_t* msg;
  int checkpoint_0 = 0;
  int checkpoint_1 = 0;

  /* Test Suite setup and cleanup functions: */

  /* funzioni init e clean delle suite suite_consumatorimolteplici_bufferpieno_bloccante e suite_consumatorimolteplici_bufferpieno_nonbloccante */
  int init_suite_consumatorimolteplici_bufferpieno(void) { 
    buffer = buffer_init(1);
    msg = msg_init_string("messaggio 0");
    msg = put_bloccante(buffer, msg);
    return 0; 
  }

  int clean_suite_consumatorimolteplici_bufferpieno(void) {
    msg_destroy_string(msg);
    buffer_destroy(buffer);
    return 0; 
  }

  /************* Test case functions ****************/

  /* test della suite suite_consumatorimolteplici_bufferpieno_bloccante */
  void test_iniziale_semafori_getbloccante_bufferpieno (void){
    //verifico lo stato del buffer: vuote=0 e piene=1
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (1 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (0 == i);
  }

  void* thread_function_consumatore_bloccante_0 (void* arg){
    msg_t* ret_msg_0 = get_bloccante(buffer);
    checkpoint_0 = 1;
    return (void*) ret_msg_0;
  }

  void* thread_function_consumatore_bloccante_1 (void* arg){
    msg_t* ret_msg_1 = get_bloccante(buffer);
    checkpoint_1 = 1;
    return (void*) ret_msg_1;
  }

  void test_consumatorimolteplici_getbloccante_bufferpieno(void) {
    pthread_t thread0;
    pthread_t thread1;
    msg_t* ret_msg;
    //nel buffer sono presenti messaggi
    CU_ASSERT (NULL != ((buffer->buf)[0]).content);
    //SOLLECITAZIONE: lancio due thread concorrenti getbloccante per leggere dal buffer
    pthread_create (&thread0, NULL, thread_function_consumatore_bloccante_0, NULL);
    pthread_create (&thread1, NULL, thread_function_consumatore_bloccante_1, NULL);
    //verifico che la somma dei checkpoint valga sempre 1; ciò vuol dire che uno dei due flussi è ancora in wait
    sleep(SUSPENSIONTIME); //do tempo ai due flussi di procedere, ma uno dei due rimarrà in blocco
    CU_ASSERT_EQUAL (checkpoint_0+checkpoint_1, 1);
    //inoltre verifico che uno dei due flussi abbia estratto dal buffer il messaggio
    if (checkpoint_0 == 1){
      pthread_join(thread0, (void*)&ret_msg);
      CU_ASSERT_STRING_EQUAL (((char *)ret_msg->content), ((char *)msg->content));
    }
    if (checkpoint_1 == 1){
      pthread_join(thread1, (void*)&ret_msg);
      CU_ASSERT_STRING_EQUAL (((char *)ret_msg->content), ((char *)msg->content));
    }
  }

  void test_finale_semafori_getbloccante_bufferpieno (void){
    //verifico il nuovo stato del buffer: vuote=1 e piene=0
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (0 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (1 == i);
  }

  /* test della suite suite_consumatorimolteplici_bufferpieno_nonbloccante */
  void test_iniziale_semafori_getnonbloccante_bufferpieno (void){
    //verifico lo stato del buffer: vuote=0 e piene=1
    int i;
    sem_getvalue(&piene, &i);
    CU_ASSERT (1 == i);
    sem_getvalue(&vuote, &i);
    CU_ASSERT (0 == i);
  }

  void* thread_function_consumatore_nonbloccante_0 (void* arg){
    msg_t* ret_msg = get_non_bloccante(buffer);
    return (void*) ret_msg;
  }

  void* thread_function_consumatore_nonbloccante_1 (void* arg){
    msg_t* ret_msg = get_non_bloccante(buffer);
    return (void*) ret_msg;
  }

  void test_produttorimolteplici_getnonbloccante_bufferpieno(void) {
    pthread_t thread0;
    pthread_t thread1;
    msg_t* ret_msg_0;
    msg_t* ret_msg_1;
    //nel buffer sono presenti messaggi
    CU_ASSERT (NULL != ((buffer->buf)[0]).content);
    //SOLLECITAZIONE: lancio due thread concorrenti getbloccante per leggere dal buffer
    pthread_create (&thread0, NULL, thread_function_consumatore_nonbloccante_0, NULL);
    pthread_create (&thread1, NULL, thread_function_consumatore_nonbloccante_1, NULL);
    pthread_join(thread0,(void*) &ret_msg_0);
    pthread_join(thread1,(void*) &ret_msg_1);
    //verifico che uno dei due flussi abbia estratto dal buffer il messaggio
    if (ret_msg_0 == NULL)
      CU_ASSERT_STRING_EQUAL (((char *)ret_msg_1->content), ((char *)msg->content));
    if (ret_msg_1 == NULL)
      CU_ASSERT_STRING_EQUAL (((char *)ret_msg_0->content), ((char *)msg->content));
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
     CU_pSuite suite_consumatorimolteplici_bufferpieno_bloccante = NULL;
     CU_pSuite suite_consumatorimolteplici_bufferpieno_nonbloccante = NULL;
     /* initialize the CUnit test registry */
     if ( CUE_SUCCESS != CU_initialize_registry() )
        return CU_get_error();

     /* add suite_consumatorimolteplici_bufferpieno_bloccante to the registry */
     suite_consumatorimolteplici_bufferpieno_bloccante = CU_add_suite( "Consumazione concorrente di molteplici messaggi da un buffer unitario pieno: uso di chiamate bloccanti", init_suite_consumatorimolteplici_bufferpieno, clean_suite_consumatorimolteplici_bufferpieno );
     if ( NULL == suite_consumatorimolteplici_bufferpieno_bloccante ) {
        CU_cleanup_registry();
        return CU_get_error();
     }
     /* add the tests to the suite suite_consumatorimolteplici_bufferpieno_bloccante */
     if ( (NULL == CU_add_test(suite_consumatorimolteplici_bufferpieno_bloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_getbloccante_bufferpieno)) ||
          (NULL == CU_add_test(suite_consumatorimolteplici_bufferpieno_bloccante, "Lancio di consumazioni concorrenti", test_consumatorimolteplici_getbloccante_bufferpieno)) ||
          (NULL == CU_add_test(suite_consumatorimolteplici_bufferpieno_bloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_getbloccante_bufferpieno))
      )
     {
        CU_cleanup_registry();
        return CU_get_error();
     }

      /* add suite_consumatorimolteplici_bufferpieno_nonbloccante to the registry */
     suite_consumatorimolteplici_bufferpieno_nonbloccante = CU_add_suite( "Consumazione concorrente di molteplici messaggi da un buffer unitario pieno: uso di chiamate non bloccanti", init_suite_consumatorimolteplici_bufferpieno, clean_suite_consumatorimolteplici_bufferpieno);
     if ( NULL == suite_consumatorimolteplici_bufferpieno_nonbloccante ) {
        CU_cleanup_registry();
        return CU_get_error();
     }
     /* add the tests to the suite suite_consumatorimolteplici_bufferpieno_nonbloccante */
     if ( (NULL == CU_add_test(suite_consumatorimolteplici_bufferpieno_nonbloccante, "Stato del buffer: Valutazione iniziale dei semafori", test_iniziale_semafori_getnonbloccante_bufferpieno)) ||
          (NULL == CU_add_test(suite_consumatorimolteplici_bufferpieno_nonbloccante, "Lancio di consumazioni concorrenti", test_produttorimolteplici_getnonbloccante_bufferpieno)) ||
          (NULL == CU_add_test(suite_consumatorimolteplici_bufferpieno_nonbloccante, "Stato del buffer: Valutazione finale dei semafori", test_finale_semafori_getnonbloccante_bufferpieno))
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