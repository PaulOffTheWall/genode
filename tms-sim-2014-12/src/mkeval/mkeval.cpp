/**
 * $Id: mkeval.cpp 737 2014-09-29 11:37:16Z klugeflo $
 * @file mkeval.cpp
 * @brief Store for (m,k) evaluations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <mkeval/mkeval.h>
//#include <core/stat.h>

#include <utils/tlogger.h>
#include <xmlio/tasksetwriter.h>

#include <cassert>
#include <sstream>
using namespace std;

namespace tmssim {

  MKSimulationResults::MKSimulationResults()
    : SimulationResults(), mkfail(false) {
  }


  MKSimulationResults::MKSimulationResults(const SimulationResults& simStats)
    : SimulationResults(simStats), mkfail(false) {
  }


  MKSimulationResults::MKSimulationResults(const MKSimulationResults& rhs)
    : SimulationResults(rhs), mkfail(rhs.mkfail) {
  }


  std::ostream& operator<< (std::ostream& out, const MKSimulationResults& stat) {
    out << (SimulationResults) stat;
    out << "(m,k)-fail: " << stat.mkfail << endl;
    }


  MKEval::MKEval(MKTaskSet* _taskset, unsigned int _nSchedulers,
		 const MKEvalAllocatorPair *_allocators, int _steps)
    : taskset(_taskset), nSchedulers(_nSchedulers), allocators(_allocators),
      steps(_steps) {
    simulations = new Simulation*[nSchedulers];
    mkts = new std::vector<MKTask*>*[nSchedulers];
    results = new MKSimulationResults[nSchedulers];
    for (int i = 0; i < nSchedulers; ++i) {
      simulations[i] = NULL;
    }

    //successMap = ~((unsigned int)(1 << _nSchedulers) - 1);
    successMap = 0;
    //cout << "Map inited to " << hex << successMap << dec << endl;
    /*
    nsb = new NullStreambuf[nSchedulers];
    nos = new ostream*[nSchedulers];
    nolog = new Logger*[nSchedulers];
    for (int i = 0; i < nSchedulers; ++i) {
      nos[i] = new ostream(&nsb[i]);
      nolog[i] = new Logger(-1, *nos[i]);
    }
    */
  }


  MKEval::~MKEval() {
    // Dealloc taskset
    delete taskset;
    for (int i = 0; i < nSchedulers; ++i) {
      if (simulations[i] != NULL)
	delete simulations[i];
      if (mkts[i] != NULL)
	delete mkts[i];
      //delete nolog[i];
      //delete nos[i];
    }
    delete[] simulations;
    delete[] mkts;
    delete[] results;
    /*
    delete[] nolog;
    delete[] nos;
    delete[] nsb;
    */
  }


  void MKEval::run() {
    for (int i = 0; i < nSchedulers; ++i) {
      prepareEval(i);
      runEval(i);
    }
  }


  const MKSimulationResults* MKEval::getResults() const {
    return results;
  }


  unsigned int MKEval::getSuccessMap() const {
    return successMap;
  }

  const MKTaskSet* MKEval::getTaskSet() const {
    return taskset;
  }

  /*
  static NullStreambuf nsb;
  ostream nullStream(&nsb);
  Logger noLog(-1, nullStream);

   */

  void MKEval::prepareEval(unsigned int num) {
    assert(num < nSchedulers);
    // copy taskset
    vector<Task*>* ts = new vector<Task*>;
    mkts[num] = new vector<MKTask*>;
    for (vector<MKTask*>::iterator it = taskset->tasks.begin();
	 it != taskset->tasks.end(); ++it) {
      if (*it == NULL) {
	cerr << "No task found!\n";
	abort();
      }
      MKTask* task = allocators[num].taskAlloc(*it);
      //cout << "\tt@" << task << endl;
      ts->push_back(task);
      mkts[num]->push_back(task);
    }
    simulations[num] = new Simulation(ts, allocators[num].schedAlloc(), steps);//, nolog[num] /*, new Logger()*/);
  }


  void MKEval::runEval(unsigned int num) {
    assert(num < nSchedulers);
    results[num] = simulations[num]->run();
    if (results[num].simulatedTime < steps || !results[num].success) {
      //results[num].success = false;

      //string filename = "results/tsfail-" + taskset->seed + ".xml";
      ostringstream oss;
      oss << "results/tsfail-" << taskset->seed << "-" << allocators[num].id << ".xml";
      string filename = oss.str();
      //LOG(1) << "Writing fail taskset to " << filename;
      TasksetWriter* writerPtr = TasksetWriter::getInstance();
      if (!writerPtr->write(filename, *results[num].taskset)) {
	tError() << "Error while writing tasks to file: " << filename << "!";
      }
      
    }
    else {
      //results[num].success = true;
      //cout << "Eval successful, giving to map: " << ((unsigned int)1<<num) << endl;
      successMap |= EVAL_MAP_BIT(num);
    }
    for (vector<MKTask*>::iterator it = mkts[num]->begin();
	 it != mkts[num]->end(); ++it) {
      if ((*it)->getMonitor().getViolations() > 0) {
	results[num].mkfail = true;
	break;
      }
    }
  }


} // NS tmssim
