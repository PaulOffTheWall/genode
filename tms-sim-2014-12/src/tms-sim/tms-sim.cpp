/**
 * $Id: tms-sim.cpp 695 2014-09-15 16:05:03Z klugeflo $
 * @file tms-sim.cpp
 * @brief Timing Models Scheduling SIMulator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 *
 * @todo check de-/allocation of simulation objects
 */

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>
using namespace std;

#include <utils/logger.h>
#include <utils/tlogger.h>
#include <core/scobjects.h>
#include <core/simulation.h>
//#include <core/stat.h>
#include <xmlio/tasksetreader.h>
#include <xmlio/tasksetwriter.h>
#include <xmlio/xmlutils.h>

#include <schedulers/schedulers.h>

#include <taskmodels/taskmodels.h>

#include <utility/aggregators.h>
#include <utility/calculators.h>

using namespace tmssim;

vector<Scheduler*> schedulers;
int iterations = 100;
bool verbose = false;
int verboseLevel = 0;

void printLegend();
bool parseArgs(int argc, char *argv[],vector<Task*>& taskset);
void calcStat(const vector<int>& v);
void EvaluateStatistics(vector<SimulationResults>&);

int main(int argc, char *argv[]) {
  //Logger::setLogLevel(Logger::NONE);
  LOG(0) << "Hello from tms-sim";

  //TLogger::init(TLogger::DEBUG);
  printLegend();
  
  int rv = 0;
  vector<vector<Task*>* > tasksetsForScheduler;
  vector<Simulation*> simulations;
  vector<SimulationResults> statistics;
  
  vector<Task*>* taskset = new vector<Task*>();
  //Logger* logger = NULL;
  
  if (!parseArgs(argc, argv, *taskset)) {
    rv = -1;
    goto cleanup;
  }
  
  logger::setLevel(verboseLevel);
  //logger = new Logger(verboseLevel);
  LOG(0) << "Level 0";
  LOG(1) << "Level 1";
  LOG(2) << "Level 2";
  LOG(3) << "Level 3";
  LOG(4) << "Level 4";
  LOG(5) << "Level 5";
  
  /***************************************************************************
   * Preparing the simulation
   ***************************************************************************/
  
  // For every specified scheduler
  for (size_t j=0; j<schedulers.size(); j++) {
    //tasksetsForScheduler.push_back(new vector<Task*>());
    vector<Task*>* simTaskset = new vector<Task*>;//(taskset->size());
    // Create Copy of the Tasks for each simulation
    for(size_t i=0; i<taskset->size(); i++) {
      //tasksetsForScheduler[j]->push_back(taskset->at(i)->clone());
      simTaskset->push_back(taskset->at(i)->clone());
    }
    //simulations.push_back(new Simulation(tasksetsForScheduler[j],schedulers[j],iterations,verbose));
    simulations.push_back(new Simulation(simTaskset, schedulers[j],
					 iterations));//, logger));
  }
  
  /***************************************************************************
   * Running the simulations
   ***************************************************************************/
  
  
  for (size_t j=0; j<simulations.size(); j++) {
    statistics.push_back(simulations[j]->run());
  }
  
  /***************************************************************************
   * Compare the statistics of each simulation
   ***************************************************************************/
  
  EvaluateStatistics(statistics);
  
  /***************************************************************************
   * Cleaning Up
   ***************************************************************************/
  
  // Delete simulations
  for(size_t i=0; i<simulations.size(); i++) {
    delete simulations[i];
  }
  
  // Delete statistics
  /*
  for(size_t i=0; i<statistics.size(); i++) {
    delete statistics[i];
  }
  */
  // Delete taskset-Copies
  /*
  // Already done in Simulation
  for (size_t j=0; j<schedulers.size(); j++) {
    for(size_t i=0; i<tasksetsForScheduler[j]->size(); i++) {
      delete (*(tasksetsForScheduler[j]))[i];
    }
    delete tasksetsForScheduler[j];
  }
  */
  // Delete schedulers
  /*
  // Already done in Simulation
  for(size_t i=0; i<schedulers.size(); i++) {
    delete schedulers[i];
  }
  */
 cleanup:
  // Delete original taskset
  for(size_t i=0; i<taskset->size(); i++) {
    delete (*taskset)[i];
  }
  delete taskset;
  
  TasksetReader::getInstance()->clear();
  TasksetWriter::getInstance()->clear();
  //Logger::getInstance()->clear();
  
  cout << "finished" << endl;

  //TLogger::clear();

  //delete logger;
  
  return rv;
}


void printLegend() {
  cout << "Job: J[taskref],[jobnum] ([remET]/[ET]-[ct] P [priority] S [st])\n";
}

void printUsage(const string& prog) {
  //cout << __FUNCTION__ << " not implemented yet" << endl;

  cout << "Usage: " << prog << " [arguments] \n\
  Arguments:\n\
  - [i]nput <path-to-tasksetX.xml> (mandatory)\n\
  - [n]umber of iterations (default=100)\n\
  - [s]cheduler (select at least one)\n\
    - [b]eedf\n\
    - [d]vdedf\n\
    - [e]df\n\
    - [f]pp\n\
    - [g]dpa\n\
    - gdpa[s]\n\
    - [h]cedf\n\
    - [m]kuedf\n\
    - [p]hcedf\n\
  - [v]erbose" << endl;
}


/**
 * Arguments:
 * - [i]nput <path-to-tasksetX.xml>
 * - [n]umber of iterations
 * - [s]cheduler
 *   + [b]eedf
 *   + [d]vdedf
 *   + [e]df
 *   + [f]pp
 *   + [g]dpa
 *   + [h]cedf
 *   + [m]kuedf
 *   + [p]hcedf
 * - [v]erbose
 */
bool parseArgs(int argc, char *argv[],vector<Task*>& taskset) {
  
  bool rv = true;
  char *str;
  char sw;
  int p = 1;
  int ts = -1;
  
  while (p < argc) {
    if (argv[p][0] == '-') {
      sw = argv[p][1];
      if (sw == '\0') {
	// empty parameter?
	cerr << "Received stray parameter: -\n";
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
	  rv = false;
	  goto failure;
	}
	break;
	
	// the -PARM parameters follow
      case 'i': // Input selection
	{
	  TasksetReader* readerPtr = TasksetReader::getInstance();
	  // task and ntasks called by reference
	  if (!readerPtr->read(str, taskset)) {
	    cerr << "Error while retrieving taskset from " << str << "!" << endl;
	    goto failure;
	  }
	  cout << "Parsed Tasks #:" << taskset.size() << endl;
	  break;
	}

      case 'n': // Number of iterations
	if (str == NULL) {
	  cerr << "You need to specify the number of iterations!" << endl;
	  goto failure;
	} else {
	  iterations = atoi(str);
	}
	break;
	/*
      case 'o': // Ouput selection
	{
	  TasksetWriter* writerPtr = TasksetWriter::getInstance();
	  if (!writerPtr->write(str, taskset)) {
	    cerr << "Error while writing tasks to file: " << str << "!" << endl;
	    goto failure;
	  }
	  cout << "Wrote Tasks #" << taskset.size() << " to file: " << str << endl;
	  exit(0); // TODO: End here because until now it is not clear what should happen after writing
	  break;
	}
	*/
      case 's': // Scheduler selection
	if (str != NULL)
	  while (*str != '\0') {
	    switch (*str) {
	    case 'b':
	      schedulers.push_back(new BEEDFScheduler());
	      break;
	    case 'd':
	      schedulers.push_back(new DVDEDFScheduler());
	      break;
	    case 'e':
	      schedulers.push_back(new EDFScheduler());
	      break;
	    case 'f':
	      schedulers.push_back(new FPPScheduler());
	      break;
	    case 'g':
	      schedulers.push_back(new GDPAScheduler());
	      break;
	    case 'h':
	      schedulers.push_back(new HCEDFScheduler());
	      break;
	    case 'm':
	      schedulers.push_back(new MKUEDFScheduler());
	      break;
	    case 'p':
	      schedulers.push_back(new PHCEDFScheduler());
	      break;
	    case 's':
	      schedulers.push_back(new GDPASScheduler());
	      break;
	    }
	    str++;
	  }
	else {
	  cerr << "You need to specify at least one scheduler!" << endl;
	  goto failure;
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
	cerr << "Unknown option -" << sw << "!" << endl;
	goto failure;
      }
      ++p;
    } else {
      // failure
      cerr << "Invalid paramter format" << endl;
      goto failure;
    }
  } // end while
  
    // check parameters
  if (schedulers.size() <= 0) {
    cerr << "You need to specify at least one scheduler!" << endl;
    rv = false;
  }
  if (ts == -1 && taskset.size() < 1) {
    cerr << "You need to specify a task set!" << endl;
    rv = false;
  }
  if (rv)
    return rv;
  
 failure:
  return false;
}

void calcStat(const vector<int>& v) {
  double sum = 0;
  int n = 0;
  int sd = 1; // successive delays
  size_t max = 0;
  for (vector<int>::const_iterator it = v.begin(); it != v.end(); ++it, ++sd) {
    if (*it != 0) {
      sum += *it * sd;
      n += *it; //sd;
      max = sd;
    }
  }
  double mu = sum / n;
  
  sum = 0;
  sd = 1;
  for (vector<int>::const_iterator it = v.begin(); it != v.end(); ++it, ++sd) {
    if (*it != 0) {
      double tmp = *it * sd - mu;
      sum += tmp * tmp;
      n += *it; //sd;
    }
  }
  
  double var = sum / n;
  
  //cout << "MU: " << mu << " VAR: " << var << " MAX: " << max;
  cout << "MU: " << mu << " SIG: " << sqrt(var) << " MAX: " << max;
}

/**
 * Evaluates the retrieved statistics
 * @param statistics The statistics to be compared to each other
 */
void EvaluateStatistics(vector<SimulationResults>& statistics) {
  cout << endl;
  cout << "########################################" << endl;
  cout << "Evaluating Statistics of all Simulations" << endl;
  cout << "########################################" << endl;
  cout << endl;
  for(size_t i=0; i<statistics.size(); i++) {
    cout << "Did Simulation with Scheduler: " << statistics[i].schedulerId << endl;
    cout << "   Here come the statistics:" << endl;
    cout << statistics[i] << endl;
  }
  cout << "Leaving out further comparisons..." << endl;
  cout << "Finished evaluation" << endl;
}
