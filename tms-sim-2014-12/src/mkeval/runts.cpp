/**
 * $Id: runts.cpp 800 2014-12-11 16:27:29Z klugeflo $
 * @file runts.cpp
 * @brief Run a single generated task set
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

// build/src/mkeval/runts -s 168799635 -c mkeval/short.mkg -t3 -vvv -n100

#include <utils/logger.h>
#include <utils/tlogger.h>
#include <utils/kvfile.h>

#include <generator/mkgenerator.h>

#include <mkeval/mkeval.h>

#include <schedulers/schedulers.h>

#include <taskmodels/mktask.h>
#include <taskmodels/mkctask.h>
#include <taskmodels/dbptask.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;
using namespace tmssim;

/// How many simulations are performed for each task set
#define N_EVALS 2
/**
 * Each evaluation needs its own scheduler and task type. These are stored
 * in this array.
 */
MKEvalAllocatorPair allocators[N_EVALS] = {
  MKEvalAllocatorPair("DBP", FPPSchedulerAllocator, DBPTaskAllocator), // Hamdaoui & Ramanathan 1995
  //MKEvalAllocatorPair("MKC", FPPSchedulerAllocator, MKCTaskAllocator), // Ramanathan 1999
  //MKEvalAllocatorPair("MKC-S", FPPSchedulerAllocator, MKCTaskWithSpinAllocator), // Ramanathan 1999 + Quan & Hu 2000
  // MKEvalAllocatorPair("GDPA", GDPASchedulerAllocator, MKTaskAllocator), // Cho et al. 2010
  //MKEvalAllocatorPair("GDPA-S", GDPASSchedulerAllocator, MKTaskAllocator), // Cho et al. 2010
  MKEvalAllocatorPair("MKU", MKUEDFSchedulerAllocator, MKCTaskAllocator)//, // Kluge et al.
  //MKEvalAllocatorPair("DMU", DBPEDFSchedulerAllocator, MKTaskAllocator) // compare dbp and mku
};

const int DEFAULT_STEPS = 100;
const int DEFAULT_TASKSET_SIZE = 5;
const double DEFAULT_UTILISATION = 1.0;
const double DEFAULT_UTILISATION_DEVIATION = 0.1;


bool parseArgs(int argc, char *argv[]);
bool init();
bool checkGCfg(void);
void performSimulation();
void printLegend();

/// @brief execution steps parameter (-n)
int parmSteps = -1;
bool haveSteps = false;
/// @brief seed parameter (-s)
unsigned int parmSeed = 0;
bool haveSeed = false;
/// @brief target utilisation during taskset generation (-u)
double parmUtilisation = 0.0;
bool haveUtilisation = false;
/// @brief max deviation from target utilisation (-d)
double parmUtilisationDeviation = 0.0;
bool haveUtilisationDeviation = false;
/// @brief generator configuration file parameter (-c)
string parmCfgFile = "";
bool haveCfgFile = false;
/// @brief taskset size parameter (-t)
int parmTaskSetSize = -1;
bool haveTaskSetSize = false;

bool verbose = false;
int verboseLevel = 0;

/// @brief Seed used for taskset generation
unsigned int theSeed;
/// @brief Size of a taskset
int theTaskSetSize = -1;
/// @brief Execution steps
int theSteps = -1;
/// @brief Task set target utilisation
double theUtilisation = 0.0;
/// @brief Max deviation from target utilisation
double theUDeviation = 0.0;
/// @brief internal representation of generator configuration file
KVFile* gcfg = NULL;


int main(int argc, char* argv[]) {
  logger::setLevel(-1);
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

  printLegend();
  performSimulation();

}


#define INIT_FAIL success = false;		\
  goto init_end;

bool init() {
  bool success = true;
  logger::setLevel(verboseLevel);

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

 init_end:
  return success;
}


void printUsage(const string& prog) {
  //cout << __FUNCTION__ << " not implemented yet" << endl;

  cout << "Usage: " << prog << " [arguments] \n\
  Arguments:\n\
  -c CFGFILE KVFile with settings for the task set generator (mandatory)\n\
  -u UTILISATION [default=1.0]\n\
  -d U. DEVIATION [default=0.1]\n\
  -s SEED [default=time(NULL)]\n\
  -t TASKSETSIZE [default=5]\n\
  -n SIMULATIONSTEPS [default=100]\n\
  -p LOG_PREFIX\n\
  -v[v...] verbosity\n\
" << endl;


}


/**
 * Parameters:
 * -s seed
 * -n steps
 * -v[vv] verbosity
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

      case 'n': // Number of iterations
	if (str == NULL) {
	  tError() << "You need to specify the number of iterations!";
	  goto failure;
	} else {
	  parmSteps = atoi(str);
	  haveSteps = true;
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

      case 'v':
	++verboseLevel;
	if (str != NULL)
	  while (*str == 'v') {
	    ++verboseLevel;
	    ++str;
	  }
	else
	  p--;
	verbose = true;
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


void performSimulation() {
  MKGenerator generator(theSeed, theTaskSetSize,
			gcfg->getUInt32("minPeriod"), gcfg->getUInt32("maxPeriod"),
			gcfg->getUInt32("minK"), gcfg->getUInt32("maxK"),
			theUtilisation, theUDeviation, gcfg->getUInt32("maxWC"));

  MKTaskSet* mkts = generator.nextTaskSet(theSeed);
  //for (int i = 0; i < N_EVALS; ++i) {
  //}
  MKEval* eval = new MKEval(mkts, N_EVALS, allocators, theSteps);
  eval->run();
  const MKSimulationResults* results = eval->getResults();
  for (int i = 0; i < N_EVALS; ++i) {
    cout << results[i];
  }

  for (vector<MKTask*>::iterator it = mkts->tasks.begin();
       it != mkts->tasks.end(); ++it) {
    cout << *(*it) << endl;
  }

}


void printLegend() {
  cout << "Job: J[taskref],[jobnum] ([remET]/[ET]-[ct] P [priority] S [st])\n";
}

