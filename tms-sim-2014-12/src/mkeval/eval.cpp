/**
 * $Id: eval.cpp 796 2014-12-11 11:12:26Z klugeflo $
 * @file eval.cpp
 * @brief Evaluations for comparison of (m,k)-firm schedulers
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 *
 */

/*
  Runtimes:
  T / n
  100 / 100 ; 0.130s
  1000 / 100 ; 1.308s
  100 / 1000 ; 1.112s
  1000 / 1000 ; 10.978s
  1000 / 10000 ; 1m48.879s
  10000 / 10000 ; 18m2.389s
  10000 / 100000 ; 184m32.798s
*/

#include <utils/logger.h>
#define TLOGLEVEL TLL_WARN
#include <utils/tlogger.h>
#include <utils/kvfile.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <fstream>

#include <condition_variable>
#include <mutex>
#include <thread>

#include <generator/mkgenerator.h>

#include <mkeval/mkeval.h>
#include <mkeval/utilisationstatistics.h>

#include <schedulers/schedulers.h>
//#include <schedulers/gdpa.h>
//#include <schedulers/mkuedf.h>

#include <taskmodels/mktask.h>
#include <taskmodels/mkctask.h>
#include <taskmodels/dbptask.h>

#include <xmlio/tasksetwriter.h>


using namespace std;
using namespace tmssim;


/**
 * Results of a full (m,k) evaluation
 */
struct MKResults {
  MKResults()
    : successes(0), activations(0), completions(0), cancellations(0),
      execCancellations(0), misses(0), preemptions(0), usum(0), esum(0),
      mkfails(0), mksuccs(0) {}
  unsigned int successes;
  double activations;
  double completions;
  double cancellations;
  double execCancellations;
  double misses;
  double preemptions;
  double usum;
  double esum;
  unsigned int mkfails;
  unsigned int mksuccs;
};



/// How many simulations are performed for each task set
#define N_EVALS 5
/**
 * Each evaluation needs its own scheduler and task type. These are stored
 * in this array.
 */
MKEvalAllocatorPair allocators[N_EVALS] = {
  MKEvalAllocatorPair("DBP", FPPSchedulerAllocator, DBPTaskAllocator), // Hamdaoui & Ramanathan 1995
  MKEvalAllocatorPair("MKC", FPPSchedulerAllocator, MKCTaskAllocator), // Ramanathan 1999
  MKEvalAllocatorPair("MKC-S", FPPSchedulerAllocator, MKCTaskWithSpinAllocator), // Ramanathan 1999 + Quan & Hu 2000
  MKEvalAllocatorPair("GDPA", GDPASchedulerAllocator, MKTaskAllocator), // Cho et al. 2010
  //MKEvalAllocatorPair("GDPA-S", GDPASSchedulerAllocator, MKTaskAllocator), // Cho et al. 2010
  MKEvalAllocatorPair("MKU", MKUEDFSchedulerAllocator, MKCTaskAllocator), // Kluge et al.
  //MKEvalAllocatorPair("DMU", DBPEDFSchedulerAllocator, MKTaskAllocator) // compare dbp and mku
};

/**
 * @brief check command line parameters and set actual parameters
 */
bool init();

/// Cleanup all global memory
void cleanup();

/**
 * Print the usage message that describes all parameters
 */
void printUsage(char* prog);

/**
 * Parse arguments
 * @param argc
 * @param argv
 * @return wether parsing was successful
 */
bool parseArgs(int argc, char *argv[]);

/**
 * @brief Check wether the config file contains all relevant data
 */
bool checkGCfg(void);

/**
 * This function handles the generation of tasksets
 * Actually, we might also do this in the main thread, if we like?
 */
void generatorThread();

/**
 * This function shall implement picking a taskset (i.e. a MKEval object)
 * from the globally shared queue that is prepared by the main thread
 * and then executing the attached simulations.
 */
void simulatorThread(int id);

/**
 * This function aggregates the data over all task sets that was generated
 * by one specific scheduler.
 */
void aggregatorThread();

/**
 * Thread-safely put a taskset into the #genTaskSets list.
 * @param ts
 */
void putTaskSet(MKTaskSet* ts);

/**
 * Thread-safely take a taskset from the #genTaskSets list
 * @return a taskset, or NULL if the list is empty and no more task sets
 * will be generated.
 */
MKTaskSet* fetchTaskSet();

void putSimResult(MKEval* eval);
MKEval* fetchSimResult();


/**
 * Output results
 */
void printResults();

string binString(unsigned int n, unsigned int width);

/// @name Command line parameters
/// @{

/// @brief execution steps parameter (-n)
int parmSteps = -1;
bool haveSteps = false;
/// @brief seed parameter (-s)
unsigned int parmSeed = 0;
bool haveSeed = false;
/// @brief taskset size parameter (-t)
int parmTaskSetSize = -1;
bool haveTaskSetSize = false;
/// @brief number of task sets parameter (-T)
int parmNTaskSets = -1;
bool haveNTaskSets = false;
/**
 * @brief number of simulation threads parameter (-m)
 *
 * The default value of this parameter is determined by the number of
 * hardware thread contexts available on the platform.
 */
unsigned int parmNThreads = 0;
bool haveNThreads = false;
/// @brief target utilisation during taskset generation (-u)
double parmUtilisation = 0.0;
bool haveUtilisation = false;
/// @brief max deviation from target utilisation (-d)
double parmUtilisationDeviation = 0.0;
bool haveUtilisationDeviation = false;
/// @brief generator configuration file parameter (-c)
string parmCfgFile = "";
bool haveCfgFile = false;
/// @brief taskset log prefix (-p)
string parmLogPrefix = "";
bool haveLogPrefix = false;
/// @}


/// @name Actual parameters
/// @{


const int DEFAULT_STEPS = 100;
const int DEFAULT_TASKSET_SIZE = 5;
const int DEFAULT_N_TASKSETS = 100;
const double DEFAULT_UTILISATION = 1.0;
const double DEFAULT_UTILISATION_DEVIATION = 0.1;

/// @brief Seed used for taskset generation
unsigned int theSeed;
/// @brief Size of a taskset
int theTaskSetSize = -1;
/// @brief Execution steps
int theSteps = -1;
/// @brief Number of tasksets
int theNTaskSets = -1;
/// @brief Number of simulator threads
int theNThreads = 0;
/// @brief Task set target utilisation
double theUtilisation = 0.0;
/// @brief Max deviation from target utilisation
double theUDeviation = 0.0;
/// @brief internal representation of generator configuration file
KVFile* gcfg = NULL;
/// @brief logfiles for tasks/scheduler
ofstream** tsLogs;
/// @brief logfile task success maps
ofstream* taskSuccessLog;
/// @}

/// @name Multithreading
/// @{

/// @brief lock for stdout
mutex outLock;
/// @brief Thread for task set generation
thread tGenerator;
/// @brief Threads for simulation
thread* tSimulator = NULL;
/// @brief Thread for aggregation
thread tAggregator;
/// @}

/// @name Taskset generation
/// @{

/// @brief Tasksets generated by tGenerator
list<MKTaskSet*> genTaskSets;
/// @brief Indicator that taskset generation is finished
bool tsGenerationFinished = false;
/// @brief lock for genTaskSets and tsGenerationFinished
mutex gtsLock;
/// @brief simulatorThreads wait at this conditional for new task sets in genTaskSets
condition_variable gtsCond;
/// @}

/// @name Evaluation
/// @{

/// @brief The evaluations containing the results
list<MKEval*> evaluations;
/// @brief Lock for the evaluations list
mutex evaLock;
/// @brief Indicator that simulations are finished
bool simulationsFinished = false;
/// @brief aggregatorThread waits here for new simulation results
condition_variable evaCond;
/// @}

/// @name Results
/// @{

/// @brief Accumulated results of all task sets
MKResults results[N_EVALS];
/// @brief Accumulated results of all successful task sets
MKResults successResults[N_EVALS];
/// @brief Discarded Task sets
unsigned int discardedTaskSets = 0;
/// @brief How many task sets passed the sufficient schedulability test (Jia et al.)
unsigned int schedTestPass = 0;
/**
 * @brief Count specific success combinations.
 *
 * Each index bit stands for one scheduler evaluation using the respective
 * scheduler from the #allocators.
 */
unsigned int resultBucket[1 << N_EVALS];
/**
 * @brief Count specific success combinations including schedulability test
 *
 * Highest index bit stands for schedulability test.
 * @see #resultBucket
 */
unsigned int resultSchedBucket[1 << (N_EVALS + 1)];
/// @brief Collect statistics about the task sets' utilisation
UtilisationStatistics uStats;

/// @}

int main(int argc, char* argv[]) {
  // INIT
  logger::setLevel(-1);
  //parmNThreads = thread::hardware_concurrency();
  //cout << "Available hardware threads: " << thread::hardware_concurrency() << endl;
  /// @todo check return values more accurately
  int parseArgsStatus = parseArgs(argc, argv);
  if (parseArgsStatus > 0) { // -- help was called
    return 0;
  }
  else if (parseArgsStatus < 0) { // an error occured during parsing parameters
    tError() << "Failed parsing arguments, exiting";
    return -1;
  }

  if (!init()) {
    tError() << "Initialisation failed, see error messages above. Exiting.";
    return -1;
  }

  cout << "==INFO== Seed: " << theSeed << endl;
  cout << "==INFO== Taskset size: " << theTaskSetSize << endl;
  cout << "==INFO== Utilisation: " << theUtilisation << endl;
  cout << "==INFO== UDeviation: " << theUDeviation << endl;
  cout << "==INFO== Execution steps: " << theSteps << endl;
  cout << "==INFO== CfgFile: " << parmCfgFile << endl;
  cout << "==INFO== N Tasksets: " << theNTaskSets << endl;
  cout << "==INFO== N Threads: " << theNThreads << endl;
  if (haveLogPrefix) {
    cout << "==INFO== Log prefix: " << parmLogPrefix << endl;
  }

  // RUN
  tGenerator = thread(generatorThread);
  tAggregator = thread(aggregatorThread);
  tSimulator = new thread[theNThreads];
  for (int i = 0; i < theNThreads; ++i) {
    tSimulator[i] = thread(simulatorThread, i);
  }
  
  // Wait for simulations and aggreation to finish
  tGenerator.join();
  for (int i = 0; i < theNThreads; ++i) {
    tSimulator[i].join();
  }
  evaLock.lock();
  simulationsFinished = true;
  evaCond.notify_all();
  evaLock.unlock();
  tAggregator.join();

  cout << endl;

  // FINISH
  // now create final data and output results
  uStats.evaluate();
  printResults();

  cleanup();
  return 0;
}


#define INIT_FAIL success = false;		\
  goto init_end;

bool init() {
  bool success = true;
  unsigned int hwThreads = thread::hardware_concurrency();

  // config file:
    gcfg = new KVFile(parmCfgFile);
    if (!checkGCfg()) {
      tError() << "Invalid configuration file!";
      INIT_FAIL;
    }
    else {
      tInfo() << "Configuration file read successfully!";
    }

  if (haveSeed) {
    theSeed = parmSeed;
  }
  else {
    theSeed = time(NULL);
  }

  if (haveTaskSetSize) {
    if (parmTaskSetSize <= 0) {
      tError() << "Invalid taskset size: " << parmTaskSetSize;
      INIT_FAIL;
    }
    else {
      theTaskSetSize = parmTaskSetSize;
    }
  }
  else {
    theTaskSetSize = DEFAULT_TASKSET_SIZE;
  }

  if (haveNTaskSets) {
    if (parmNTaskSets <= 0) {
      tError() << "Invalid number of execution steps: " << parmNTaskSets;
      INIT_FAIL;
    }
    else {
      theNTaskSets = parmNTaskSets;
    }
  }
  else {
    theNTaskSets = DEFAULT_N_TASKSETS;
  }

  if (haveSteps) {
    if (parmSteps <= 0) {
      tError() << "Invalid number of execution steps: " << parmSteps;
      INIT_FAIL;
    }
    else {
      theSteps = parmSteps;
    }
  }
  else {
    theSteps = DEFAULT_STEPS;
  }

  if (haveNThreads) {
    if (parmNThreads > hwThreads) {
      tWarn() << "You specified more threads than are available in hardware -"
	      << " falling back to nHWThreads = " << hwThreads;
      theNThreads = hwThreads;
    }
    else {
      theNThreads = parmNThreads;
    }
  }
  else {
    theNThreads = hwThreads;
  }

  if (haveUtilisation) {
    if (parmUtilisation <= 0) {
      tError() << "Invalid utilisation: " << parmUtilisation;
      INIT_FAIL;
    }
    else {
      theUtilisation = parmUtilisation;
    }
  }
  else {
    theUtilisation = DEFAULT_UTILISATION;
  }

  if (haveUtilisationDeviation) {
    if (parmUtilisationDeviation <= 0 | parmUtilisationDeviation > parmUtilisation) {
      tError() << "Invalid utilisation deviation: " << parmUtilisationDeviation;
      INIT_FAIL;
    }
    else {
      theUDeviation = parmUtilisationDeviation;
    }
  }
  else {
    theUDeviation = DEFAULT_UTILISATION_DEVIATION;
  }


  if (haveLogPrefix) {
    tsLogs = new ofstream*[N_EVALS];
    for (int i = 0; i < N_EVALS; ++i) {
      ostringstream oss;
      oss << parmLogPrefix;
      oss << "-" << allocators[i].id << ".txt";
      tsLogs[i] = new ofstream(oss.str(), ios::out | ios::trunc);
      *tsLogs[i] << "tsid ; succ ; act ; compl ; canc ; ecanc ; miss ; preempt ; usum ; esum ; mksuccs" << endl;
    }
    ostringstream oss;
    oss << parmLogPrefix << "-success.txt";
    taskSuccessLog = new ofstream(oss.str(), ios::out | ios::trunc);
    *taskSuccessLog << "TSID ; STEST";
    for (int i = N_EVALS - 1; i >= 0; --i) {
      *taskSuccessLog << " | " << allocators[i].id;
    }
    *taskSuccessLog << endl;
  }

  // just to be sure
  for (int i = 0; i < N_EVALS; ++i) {
    resultBucket[i] = 0;
    resultSchedBucket[i] = 0;
  }
 init_end:
  return success;
}


void cleanup() {
  // clean up
  //delete[] tAggregator;
  delete[] tSimulator;
  delete gcfg;

  for (list<MKEval*>::iterator it = evaluations.begin();
       it != evaluations.end(); ++it) {
    delete *it;
  }
  evaluations.clear();

  if (haveLogPrefix) { // flush and close log files
    for (int i= 0; i < N_EVALS; ++i) {
      tsLogs[i]->close();
      delete tsLogs[i];
    }
    delete[] tsLogs;
    taskSuccessLog->close();
    delete taskSuccessLog;
  }

  //delete[] results;
}


void printUsage(char* prog) {
  //cout << "Job: " << sizeof(Job) << endl;

  cout << "Usage: " << prog << " [arguments] \n\
  Arguments:\n\
  -c CFGFILE KVFile with settings for the task set generator (mandatory)\n\
  -s SEED [default=time(NULL)]\n\
  -t TASKSETSIZE [default=5]\n\
  -n SIMULATIONSTEPS [default=100]\n\
  -m SIMULATIONTHREADS [default=" << thread::hardware_concurrency() << "]\n\
  -T NTASKSETS [default=100]\n\
  -u UTILISATION [default=1.0]\n\
  -d U. DEVIATION [default=0.1]\n\
  -p LOG_PREFIX\n\
" << endl;
}


/*
  -c CFGFILE
  -s SEED
  -t TASKSETSIZE
  -n SIMULATIONSTEPS
  -m THREADS
  -T N_TASKSETS
 */

/**
 * @todo return more different error codes
 */
bool parseArgs(int argc, char *argv[]) {
  
  int rv = 0;
  char *str;
  char sw;
  int p = 1;
  int ts = -1;
  
  while (p < argc) {
    if (argv[p][0] == '-') {
      sw = argv[p][1];
      if (sw == '\0') {
	// empty parameter?
	tError() << "Received stray parameter: -\n";
	goto failure;
      }
      
      if (argv[p][2] != '\0') { // -sstring
	str = &argv[p][2];
      } else {
	if (++p < argc) { // -s string
	  str = argv[p];
	} else {
	  // ???? problem!!!
	  str = NULL;
	  // TODO: this can lead to SIGSEGV below!
	}
      }
      
      switch (sw) {
	
      case '-': // this is the --PARM section
	if (strcmp(str, "help") == 0) {
	  printUsage(argv[0]);
	  rv = 1;
	  goto failure;
	}
	break;
	
	// the -PARM parameters follow

      case 'c': // config file
	if (str == NULL) {
	  tError() << "You need to specify the config file!";
	  goto failure;
	} else {
	  parmCfgFile = str;
	  haveCfgFile = true;
	}
	break;

      case 'd': // Utilisation deviation
	if (str == NULL) {
	  tError() << "You need to specify a valid utilisation deviation!";
	  goto failure;
	}
	else {
	  parmUtilisationDeviation = strtod(str, NULL);
	  haveUtilisationDeviation = true;
	}
	break;

      case 'm': // max. number of threads
	if (str == NULL) {
	  tError() << "You need to specify the number of threads!";
	  goto failure;
	} else {
	  parmNThreads = atoi(str);
	  haveNThreads = true;
	}
	break;

      case 'n': // Number of iterations
	if (str == NULL) {
	  tError() << "You need to specify the number of iterations!";
	  goto failure;
	} else {
	  parmSteps = atoi(str);
	  haveSteps = true;
	}
	break;

      case 'p': // task statistics 
	if (str == NULL) {
	  tError() << "You must specify a logfiles prefix!";
	  goto failure;
	}
	else {
	  parmLogPrefix = str;
	  haveLogPrefix = true;
	}
	break;

      case 's': // Seed
	if (str == NULL) {
	  tError() << "You need to specify a seed!";
	  goto failure;
	} else {
	  parmSeed = atoi(str);
	  haveSeed = true;
	}
	break;

      case 't': // Task set size
	if (str == NULL) {
	  tError() << "You need to specify a valid task set size!";
	  goto failure;
	} else {
	  parmTaskSetSize = atoi(str);
	  haveTaskSetSize = true;
	}
	break;

      case 'T': // Number of task sets
	if (str == NULL) {
	  tError() << "You need to specify a valid number of task sets!";
	  goto failure;
	} else {
	  parmNTaskSets = atoi(str);
	  haveNTaskSets = true;
	}
	break;

      case 'u': // Utilisation
	if (str == NULL) {
	  tError() << "You need to specify a valid utilisation!";
	  goto failure;
	}
	else {
	  parmUtilisation = strtod(str, NULL);
	  haveUtilisation = true;
	}
	break;
	
      default:
	tError() << "Unknown option -" << sw << "!";
	goto failure;
      }
      ++p;
    } else {
      // failure
      tError() << "Invalid paramter format";
      goto failure;
    }
  } // end while
  
    // check parameters
  if (!haveCfgFile) {
    tError() <<  "You must specify a configuration file for the task set generator!";
    rv = -1;
  }
 failure:
  return rv;
}


bool checkGCfg(void) {
  return gcfg->containsKey("minPeriod")
    && gcfg->containsKey("maxPeriod")
    && gcfg->containsKey("minK")
    && gcfg->containsKey("maxK")
    && gcfg->containsKey("maxWC");
}


void putTaskSet(MKTaskSet* ts) {
  unique_lock<mutex> lck(gtsLock);
  genTaskSets.push_back(ts);
  gtsCond.notify_one();
}


MKTaskSet* fetchTaskSet() {
  unique_lock<mutex> lck(gtsLock);
  while (!tsGenerationFinished && genTaskSets.size() == 0) {
    gtsCond.wait(lck);
  }
  MKTaskSet* ts = NULL;
  if (genTaskSets.size() > 0) {
    ts = genTaskSets.front();
    genTaskSets.pop_front();
  }
  else if (tsGenerationFinished) {
    ts = NULL;
  }
  else {
    // should not happen
    abort();
  }
  return ts;
}


void putSimResult(MKEval* eval) {
  unique_lock<mutex> lck(evaLock);
  evaluations.push_back(eval);
  evaCond.notify_all();
}


MKEval* fetchSimResult() {
  unique_lock<mutex> lck(evaLock);
  while (!simulationsFinished && evaluations.size() == 0) {
    /*outLock.lock();
    cout << "_";
    outLock.unlock();*/
    evaCond.wait(lck);
  }
  /*outLock.lock();
  cout << "*";
  outLock.unlock();*/
  MKEval* eval = NULL;
  if (evaluations.size() > 0) {
    eval = evaluations.front();
    evaluations.pop_front();
  }
  else if (simulationsFinished) {
    eval = NULL;
  }
  else {
    // should not happen
    abort();
  }
  return eval;
}


void generatorThread() {
  outLock.lock();
  cout << "==INFO== Using generation parameters:" << endl;
  cout << "==INFO== \tminPeriod: " << gcfg->getUInt32("minPeriod") << endl;
  cout << "==INFO== \tmaxPeriod: " << gcfg->getUInt32("maxPeriod") << endl;
  cout << "==INFO== \tminK: " << gcfg->getUInt32("minK") << endl;
  cout << "==INFO== \tmaxK: " << gcfg->getUInt32("maxK") << endl;
  cout << "==INFO== \tmaxWC: " << gcfg->getUInt32("maxWC") << endl << endl;
  outLock.unlock();

  MKGenerator generator(theSeed, theTaskSetSize,
			gcfg->getUInt32("minPeriod"), gcfg->getUInt32("maxPeriod"),
			gcfg->getUInt32("minK"), gcfg->getUInt32("maxK"),
			theUtilisation, theUDeviation, gcfg->getUInt32("maxWC"));
  for (int i = 0; i < theNTaskSets; ++i) {
    MKTaskSet* ts = generator.nextTaskSet();
    for (vector<MKTask *>::iterator it = ts->tasks.begin();
	 it != ts->tasks.end(); ++it) {
      assert(*it != NULL);
    }
    /*
      if (i == 13) {
      vector<Task*> tts;
      for (vector<MKTask*>::iterator it = ts->tasks.begin();
      it != ts->tasks.end(); ++it) {
      tts.push_back(*it);
      }
      TasksetWriter* writerPtr = TasksetWriter::getInstance();
      if (!writerPtr->write("error.xml", tts)) {
      tError() << "Error while writing tasks to file";
      }
      }
    */
    putTaskSet(ts);
    /*outLock.lock();
    (cout << "+").flush();
    outLock.unlock();*/
  }
  gtsLock.lock();
  tsGenerationFinished = true;
  gtsCond.notify_all();
  gtsLock.unlock();
}


void simulatorThread(int id) {
  bool finished = false;
  while (!finished) {
    MKTaskSet* ts = NULL;
    ts = fetchTaskSet();
    if (ts == NULL) { // no more task sets
      finished = true;
    }
    else {
      // now run simulation
      MKEval* eval = new MKEval(ts, N_EVALS, allocators, theSteps);
      eval->run();

      putSimResult(eval);

      /*outLock.lock();
      (cout << "~").flush();
      outLock.unlock();*/

      std::this_thread::yield();
    }
  }
}


/*
  unsigned int successes;
  double activations;
  double completions;
  double cancellations;
  double execCancellations;
  double misses;
  double preemptions;
  double usum;
  double esum;
  unsigned int mkfails;
*/

void aggregatorThread() {
  bool finished = false;
  while (!finished) {
    MKEval* eval = fetchSimResult();
    if (eval == NULL) {
      finished = true;
    }
    else {
      if (eval->getSuccessMap() == EVAL_MAP_FULL(N_EVALS) ) {
	// all schedulers successful
	unsigned int rb = 0;
	unsigned int rsb = 0;
	for (int i = 0; i < N_EVALS; ++i) {
	  const MKSimulationResults& sres = eval->getResults()[i];
	  results[i].successes += sres.success ? 1 : 0;
	  results[i].activations += sres.activations;
	  results[i].completions += sres.completions;
	  results[i].cancellations += sres.cancellations;
	  results[i].execCancellations += sres.execCancellations;
	  results[i].misses += sres.misses;
	  results[i].preemptions += sres.preemptions;
	  results[i].usum += sres.usum;
	  results[i].esum += sres.esum;
	  if (sres.mkfail) {
	    results[i].mkfails += 1;
	  }
	  else {
	    results[i].mksuccs += 1;
	    rb |= 1 << i;
	  }
	  if (haveLogPrefix) {
	    *tsLogs[i] << eval->getTaskSet()->seed << " ; "
		       << sres.success << " ; "
		       << sres.activations << " ; "
		       << sres.completions << " ; "
		       << sres.cancellations << " ; "
		       << sres.execCancellations << " ; "
		       << sres.misses << " ; "
		       << sres.preemptions << " ; "
		       << sres.usum << " ; "
		       << sres.esum << " ; "
		       << (sres.mkfail ? 0 : 1) << endl;
	    tsLogs[i]->flush();
	  }
	}
	// aggregate fully successfull tasksets
	if (rb == EVAL_MAP_FULL(N_EVALS)) {
	  for (int i = 0; i < N_EVALS; ++i) {
	    const MKSimulationResults& sres = eval->getResults()[i];
	    successResults[i].successes += sres.success ? 1 : 0;
	    successResults[i].activations += sres.activations;
	    successResults[i].completions += sres.completions;
	    successResults[i].cancellations += sres.cancellations;
	    successResults[i].execCancellations += sres.execCancellations;
	    successResults[i].misses += sres.misses;
	    successResults[i].preemptions += sres.preemptions;
	    successResults[i].usum += sres.usum;
	    successResults[i].esum += sres.esum;
	    if (sres.mkfail) {
	      successResults[i].mkfails += 1;
	    }
	    else {
	      successResults[i].mksuccs += 1;
	      rb |= 1 << i;
	    }
	  }
	}

	if (eval->getTaskSet()->suffMKSched) {
	  ++schedTestPass;
	  rsb = 1 << N_EVALS;
	}
	uStats.addUtilisation(eval->getTaskSet()->utilisation);
	rsb |= rb;
	++resultBucket[rb];
	++resultSchedBucket[rsb];

	if (haveLogPrefix) {
	  *taskSuccessLog << eval->getTaskSet()->seed << " ; "
			  << binString(rsb, N_EVALS + 1) << endl;
	}
      }
      else {
	// This should not happen, any scheduler should run until the end!
	tError() << "Aggregator detected unfinished taskset!";
	++discardedTaskSets;
      }
      delete eval;
      /*outLock.lock();
      (cout << ".").flush();
      outLock.unlock();*/
    }
  }
  uStats.evaluate();
}


void printResults() {
  /*
  cout << "RB:" << endl;
  for (int i = 0; i < (1 << N_EVALS); ++i) {
    //cout << " " << resultBucket[i];
    cout << binString(i, N_EVALS) << ": " << resultBucket[i] << endl;
  }
  cout << endl;
  */
  //cout << "RSB:" << endl;
  cout << "==RSBI==; schedTestPass";
  for (int i = N_EVALS - 1; i >= 0; --i) {
    cout << " ; " << allocators[i].id;
  }
  cout << endl;
  for (int i = 0; i < (1 << (N_EVALS + 1)); ++i) {
    //cout << " " << resultSchedBucket[i];
    cout << "==RSB==; " << i << " ; " << binString(i, N_EVALS + 1) << " ; " << resultSchedBucket[i] << endl;
  }
  cout << endl;

  cout << "==STAT==; Sched ; succ ; act ; compl ; canc ; ecanc ; miss ; preempt ; usum ; esum ; mksuccs ; succ%" << endl;
  for (int i = 0; i < N_EVALS; ++i) {
    cout << "==STAT==; " << allocators[i].id
	 << " ; " << round(results[i].successes / theNTaskSets)
	 << " ; " << round(results[i].activations / theNTaskSets)
	 << " ; " << round(results[i].completions / theNTaskSets)
	 << " ; " << round(results[i].cancellations / theNTaskSets)
	 << " ;" << round(results[i].execCancellations / theNTaskSets)
	 << " ; " << round(results[i].misses / theNTaskSets)
	 << " ; " << round(results[i].preemptions / theNTaskSets)
	 << " ; " << round(results[i].usum / theNTaskSets)
	 << " ; " << round(results[i].esum / theNTaskSets)
	 << " ; " << results[i].mksuccs
	 << " ; " << (double)results[i].mksuccs / theNTaskSets << endl;
  }
  cout << "==STA2==; schedTestPass ; " << schedTestPass 
       << " ; " << (double)schedTestPass / theNTaskSets << endl;
  //cout << "Task sets that passed the sufficient schedulability test: " << schedTestPass << endl;
  cout << "==STA2==; discarded ; " << discardedTaskSets << endl;
  //cout << "Discarded " << (discardedTaskSets/4) << " tasksets from results" << endl;

  for (int i = 0; i < N_EVALS; ++i) {
    cout << "==SUCC==; " << allocators[i].id
	 << " ; " << round(successResults[i].successes / theNTaskSets)
	 << " ; " << round(successResults[i].activations / theNTaskSets)
	 << " ; " << round(successResults[i].completions / theNTaskSets)
	 << " ; " << round(successResults[i].cancellations / theNTaskSets)
	 << " ;" << round(successResults[i].execCancellations / theNTaskSets)
	 << " ; " << round(successResults[i].misses / theNTaskSets)
	 << " ; " << round(successResults[i].preemptions / theNTaskSets)
	 << " ; " << round(successResults[i].usum / theNTaskSets)
	 << " ; " << round(successResults[i].esum / theNTaskSets)
	 << " ; " << successResults[i].mksuccs
	 << " ; " << (double)successResults[i].mksuccs / theNTaskSets << endl;
  }


  cout << "==UTIL== "
       << "[" << uStats.getMin() << ";" << uStats.getMax() << "] "
       << uStats.getMean()
       << " +/- " << uStats.getSigma()
       << " m=" << uStats.getMedian()
       << " 50% [" << uStats.getMedianIntervalLower(50)
       << ";" << uStats.getMedianIntervalUpper(50) << "]" << endl;

}


string binString(unsigned int n, unsigned int width) {
  assert(width <= sizeof(unsigned int) * 8);
  ostringstream oss;
  for (int i = width - 1; i >= 0; --i) {
    if (n & (1 << i))
      oss << 1;
    else
      oss << 0;
  }
  return oss.str();
}
