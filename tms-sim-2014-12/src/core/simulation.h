/**
 * $Id: simulation.h 741 2014-09-30 19:49:25Z klugeflo $
 * @file simulation.h
 * @brief Encapsulation of simulations
 * @author Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 *
 * @todo
 * - revise logging
 */

#ifndef CORE_SIMULATION_H
#define CORE_SIMULATION_H 1

#include <core/task.h>
#include <core/scheduler.h>
#include <utils/logger.h>

#include <vector>
#include <sstream>


// Cancel Statistics:
// how often got $i tasks cancelled in one time step
// CANCEL_SLOTS sets the size of the different time steps
// A Value of 10 would result that even jobs are reported that are cancelled 10 times in a row
/**
 * @todo This seems to be BS, are we really interested in these data?
 * Maybe it might be better to use a map as cancelCtrl?
 */
//#define CANCEL_SLOTS 1000

namespace tmssim {

  // Forward declaration
  class Stat;


  struct SimulationResults {
    SimulationResults(Taskset* ts = NULL);
    SimulationResults(const SimulationResults& rhs);

    unsigned int simulatedTime;
    bool success; ///< simulation has run until the end
    unsigned int activations;
    unsigned int completions;
    unsigned int cancellations;
    unsigned int execCancellations;
    unsigned int misses;
    unsigned int preemptions;
    unsigned int usum;
    unsigned int esum;
    unsigned int cancelSteps;
    unsigned int idleSteps;
    Taskset* taskset;
    std::string schedulerId;

    friend std::ostream& operator<< (std::ostream& out, const SimulationResults& stat);
  };

  std::ostream& operator<< (std::ostream& out, const SimulationResults& stat);
  
  class Simulation {
  public:
    
    /**
     * @brief C'tor
     * @param _taskset The taskset on which the simulations operates (takes
     * ownership)
     * @param _scheduler The scheduler that should be used for this simulation
     * (takes ownership)
     * @param _steps The maximum number of time-steps that simulations
     * should run
     */
    Simulation(Taskset* _taskset, Scheduler* _scheduler,
	       int _steps);

    
    /**
     * @brief D'tor
     */
    ~Simulation();

    
    /**
     * Starts the simulation
     * @return Accumulated simulation result
     */
    const SimulationResults& run();


    /**
     * @brief Get the taskset, e.g. to read task statistics
     * @return Pointer to the simulation's taskset - is only valid as long
     * as the Simulation object exists
     */
    const Taskset* getTaskset() const;


  private:   
    
    /**
     * Methods
     */
    
    /**
     * Activate pending jobs of a taskset
     * @param time The time-step at which you want to activate
     * @param sched Pointer to the scheduler that provides the
     * scheduling algorithm
     * @param log Outputstream to store log information in
     * @return true, if there are jobs to be executed, false otherwise
     */
    bool doActivations(int time, Scheduler *sched/*, std::ostream& log*/);
    
    /**
     * Applies the scheduling algorithm to the pending jobs and tries to
     * complete them
     * @param time The current time-step
     * @param sched Pointer to the scheduler that provides the
     * scheduling algorithm
     * @param log Outputstream to store log information in
     * @param statPtr Pointer to a statistics object that should store
     * the cancelStepList
     * @return true, if a job is dispatched, false otherwise
     */
    bool doExecutions(int time, Scheduler *sched/*, std::ostream& log,*/
		      /*Stat* statPtr*/);
    
    /**
     * Prints the executions status of all tasks in the simulations
     * taskset to the console
     * @param head custom header of the output
     */
    void printExecStats(const std::string& head);
    
    /**
     * Inits the delay counters variables. Should be called before using them.
     */
    void initCounters(void);
    
    /**
     * Prints information about the delay counters to the console.
     * Delay counters mean how many times a job gets delayed (and not
     * dispatched) in row
     */
    void printDelayCounters(void);
    
    /**
     * stores information about this simulation into a statistics object
     * that can be used independently of this simulation
     * @param statistics Pointer to a statistics object where to
     * information gets stored into
     */
    void storeStatistics(Stat* statistics);
    

    /**
     * @brief Calculate execution statistics
     */
    void calculateStatistics();

    
    /**
     * @brief The taskset on which the simulation will run
     */
    Taskset* taskset;
    
    /**
     * @brief The scheduler which provides the scheduling algorithm
     */
    Scheduler* scheduler;
    
    /**
     * @brief The amount of time-ticks to simulate
     */
    int steps;

    
    /**
     * @brief Logging/tracing of execution
     */
    //Logger* logger;

    /**
     * cancelCtr[i] = k means there were k time steps in which exactly i
     * jobs were cancelled
     */
    //int cancelCtr[CANCEL_SLOTS];

    /**
     * count in how many time steps jobs were cancelled
     */
    int cancelSteps;

    /**
     * count in how many time steps no job was executed
     */
    int idleSteps;

    /**
     * Numbers of the steps in which cancellations happened
     */
    std::list<int> cancelStepList;


    SimulationResults stats;
  };
  
} // NS tmssim

#endif /* CORE_SIMULATION_H */
