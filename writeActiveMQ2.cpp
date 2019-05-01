/*  File    : writeActiveMQ2.cpp
 *
 *  Copyright 2019 Ulysses Bernardet CHL
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include "mex.h"
#define WIN32_LEAN_AND_MEAN //-> http://activemq.2283324.n4.nabble.com/ACTIVEMQ-CPP-Error-compiling-with-windows-h-header-included-in-the-project-td3235898.html
#include <Windows.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <activemq/util/Config.h>
#include <decaf/lang/System.h>
#include <decaf/lang/Runnable.h>
#include <decaf/lang/Integer.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/Destination.h>
#include <cms/MessageProducer.h>
#include <cms/TextMessage.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

using namespace cms;
using namespace activemq;
using namespace activemq::core;
using namespace decaf;
using namespace decaf::lang;

using namespace std;

#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME  writeActiveMQ2

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"

/* FindMatlab defines these as EXPORTS; not sure why...
   void mexCreateMexFunction() {}
   void mexDestroyMexFunction() {}
   void mexFunctionAdapter() {}
*/


static const std::string prefix = "vrSpeak";


std::string int_array_to_string(int int_array[], int size_of_array) {
  std::ostringstream oss("");
  for (int temp = 0; temp < size_of_array; temp++) {
    oss << (char)int_array[temp];
  }
  std::string str = oss.str();
  std::size_t found = str.find('\0');
  str = str.substr(0, found);
  return str;
}




std::string double_array_to_string(InputRealPtrsType double_array, int size_of_array) {
  std::ostringstream oss("");
  for (int temp = 0; temp < size_of_array; temp++) {
    oss << (char)*double_array[temp];
  }
  std::string str = oss.str();
  std::size_t found = str.find('\0');
  str = str.substr(0, found);
  return str;
}


#define IS_PARAM_DOUBLE(pVal) (mxIsNumeric(pVal) && !mxIsLogical(pVal) && !mxIsEmpty(pVal) && !mxIsSparse(pVal) && !mxIsComplex(pVal) && mxIsDouble(pVal))
#define IS_PARAM_CARRAY(pVal) (mxIsChar(pVal) && !mxIsLogical(pVal) && !mxIsEmpty(pVal) && !mxIsSparse(pVal) && !mxIsComplex(pVal) && !mxIsDouble(pVal))

/*====================*
 * S-function methods *
 *====================*/

/*
 * Check to make sure that each parameter is 1-d and positive
 */
static void mdlCheckParameters(SimStruct *S) {
  const mxArray *pVal0 = ssGetSFcnParam(S, 0);

  if (!IS_PARAM_CARRAY(pVal0)) {
    ssSetErrorStatus(S, "Parameter to S-function must be a text");
    return;
  }
}


/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S) {

  ssSetNumSFcnParams(S, 3);  /* Number of expected parameters */

  // Parameter mismatch will be reported by Simulink
  if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
    mdlCheckParameters(S);
    if (ssGetErrorStatus(S) != NULL) {
      return;
    }
  }
  else {
    return; /* Parameter mismatch will be reported by Simulink */
  }

  ssSetSFcnParamTunable(S, 0, 0);

  ssSetNumContStates(S, 0);
  ssSetNumDiscStates(S, 0);

  if (!ssSetNumInputPorts(S, 2)) return;
  ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
  ssSetInputPortWidth(S, 1, DYNAMICALLY_SIZED);
  ssSetInputPortDirectFeedThrough(S, 0, 1);
  ssSetInputPortDirectFeedThrough(S, 0, 2);

  if (!ssSetNumOutputPorts(S, 0)) return;
  //not needed?    ssSetOutputPortWidth(S, 0, 1);

  ssSetNumSampleTimes(S, 1);
  //--  ssSetNumRWork(S, 0);
  //--  ssSetNumIWork(S, 0);

  ssSetNumPWork(S, 5); // reserve elements in the pointer vector ssGetPWork
  ssSetNumModes(S, 0); // to store a C++ object
  ssSetNumNonsampledZCs(S, 0);

  ssSetSimStateCompliance(S, USE_CUSTOM_SIM_STATE);

  ssSetOptions(S, 0);
}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S) {

  // this was 20160217
#define SAMPLE_TIME_0        -1
  ssSetSampleTime(S, 0, SAMPLE_TIME_0);

  //try:
  // ssSetSampleTime(S, 0, mxGetScalar(ssGetSFcnParam(S, 0)));

  ssSetOffsetTime(S, 0, 0.0);
  ssSetModelReferenceSampleTimeDefaultInheritance(S);
}



/* Function: mdlStart =======================================================
 * Abstract:
 *    This function is called once at start of model execution. If you
 *    have states that should be initialized once, this is the place
 *    to do it.
 */
#define MDL_START
static void mdlStart(SimStruct *S) {

  ssGetPWork(S)[4] = (void *) new std::string; /* we store the actor name in this persistend variable so we don't have to query it at very update cycle*/
  std::string *strActor = (std::string *) ssGetPWork(S)[4];


  /* definition of the parameters:
   * - update activeMQ.slx to change mask etc
   * - parameter:
   *    1. host
   *   2. scope (should be pull-down)
   *   3. character name
   */

#define LENGTH 100
  char_T buf01[LENGTH];
  mxGetString(ssGetSFcnParam(S, 0), buf01, LENGTH);
  std::string strActiveMQServer(buf01);

  char_T buf02[LENGTH];
  mxGetString(ssGetSFcnParam(S, 1), buf02, LENGTH);
  std::string port(buf02);
  
  char_T buf03[LENGTH];
  mxGetString(ssGetSFcnParam(S, 2), buf03, LENGTH);
  strActor->assign(buf03);

  mexPrintf("Talking to server: %s on port: %s\n", strActiveMQServer.c_str(), port.c_str());


  std::string brokerURI = "tcp://" + strActiveMQServer + ":" + port;

  try {
    activemq::library::ActiveMQCPP::initializeLibrary();

    activemq::core::ActiveMQConnectionFactory factory;
    factory.setBrokerURI(brokerURI);
    cms::Connection * connection = factory.createConnection();
    ssGetPWork(S)[0] = (void *)connection;
    connection->start();

    cms::Session * session = connection->createSession();
    ssGetPWork(S)[1] = (void *)session;

    Destination * destBML = session->createTopic("UNITY_BML");
    cms::MessageProducer * producerBML = session->createProducer(destBML);
    ssGetPWork(S)[2] = (void *)producerBML;
    producerBML->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
    
    Destination * destJSON = session->createTopic("UNITY_JSON");
    cms::MessageProducer * producerJSON = session->createProducer(destJSON);
    ssGetPWork(S)[3] = (void *)producerJSON;
    producerJSON->setDeliveryMode(DeliveryMode::NON_PERSISTENT);


    /*
      std::auto_ptr<TextMessage> message(session->createTextMessage("Blabla"));
      message->setStringProperty("MESSAGE_PREFIX", "blabla");
      producerBML->send(message.get());
    */

    //   cerr << "activeMQ connection setup successful" << endl;
    mexPrintf("activeMQ connection setup successful\n");
  }
  catch (cms::CMSException & e) {
    cerr << "activeMQ connection setup FAIL:" << e.getMessage() << endl;
    mexPrintf("activeMQ connection setup FAIL: %s\n", e.getMessage().c_str());
    std::string strError;
    strError = "activeMQ connection setup FAIL: " + e.getMessage();
    ssSetErrorStatus(S,strError.c_str());
    // >= 2019a ssSetLocalErrorStatus(S,strError.c_str());

  }


  //--	Sleep(1000);
}

/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block.
 */
static void mdlOutputs(SimStruct *S, int_T tid) {

}


#define MDL_UPDATE
static void mdlUpdate(SimStruct *S, int_T tid) {

  UNUSED_ARG(tid); /* not used in single tasking mode */

  InputRealPtrsType  uPtrs = ssGetInputPortRealSignalPtrs(S, 0);
  std::string strMsgBML = double_array_to_string(uPtrs, 2048);

  uPtrs = ssGetInputPortRealSignalPtrs(S, 1);
  std::string strMsgJSON = double_array_to_string(uPtrs, 2048);

  cms::Session *session = (cms::Session*)ssGetPWork(S)[1];
  
  if(strMsgBML.length()>0){
    cms::MessageProducer * producerBML = (cms::MessageProducer*)ssGetPWork(S)[2];
  

    time_t tNow;
    tNow = time(NULL);
    int iSecs = (int)(tNow - floor((double)(tNow / 1000.)) * 1000); //get the last 1000 seconds
    stringstream ss;
    ss << setfill('0');
    ss << setw(3) << iSecs;
    ss << setw(5) << rand();
    string strID = /*"bml_" + */ ss.str();
    string *strActor = (std::string*) ssGetPWork(S)[4];

#ifdef BML_SMARTBODY
    string strBML = *strActor + " ALL " + strID + " " + strMsgBML;
#else
    string strBML = "<xml><bml id=\"" + strID + "\" characterId=\"" + *strActor + "\">" + strMsgBML + "</bml></xml>";
#endif

    std::auto_ptr<TextMessage> message(session->createTextMessage(strBML));
    message->setStringProperty("MESSAGE_PREFIX", prefix);
    producerBML->send(message.get()); /* .get() -> get pointer to auto_ptr object */


  }

  if(strMsgJSON.length()>0){
    cms::MessageProducer * producerJSON = (cms::MessageProducer*)ssGetPWork(S)[3];

    std::auto_ptr<TextMessage> message(session->createTextMessage(strMsgJSON));
    message->setStringProperty("MESSAGE_PREFIX", prefix);
    producerJSON->send(message.get()); /* .get() -> get pointer to auto_ptr object */
  }
    
 
  Sleep(1000 / 100);
}
  

/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S) {
  cerr << "trying to shutdown" << endl;
  mexPrintf("trying to shutdown\n");
  //--  Sleep(1000);

  try {
    cms::Session * session = (cms::Session *)ssGetPWork(S)[1];
    session->close();
  }
  catch (cms::CMSException & e) {
    cerr << "activeMQ terminate FAIL:" << e.getMessage() << endl;
    std::string strError;
    strError = "activeMQ terminate FAIL: " + e.getMessage();
    ssSetErrorStatus(S, strError.c_str());
  }
  mexPrintf("activeMQ connection closed successful\n");
}

/*======================================================*
 * See sfuntmpl.doc for the optional S-function methods *
 *======================================================*/

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
