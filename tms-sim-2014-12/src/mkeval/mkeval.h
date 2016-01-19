/**
 * $Id: mkeval.h 737 2014-09-29 11:37:16Z klugeflo $
 * @file mkeval.h
 * @brief Store for (m,k) evaluations
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef MKEVAL_MKEVAL_H
#define MKEVAL_MKEVAL_H 1

#include <core/scheduler.h>
#include <core/simulation.h>
#include <core/task.h>
#include <generator/mkgenerator.h>
#include <taskmodels/mktask.h>

namespace tmssim {

  /**
   * How would the eval/successmap look like if all evals are successfull
   * @param n total number of evals
   */
#define EVAL_MAP_FULL(n) ( ( 1 << (n) ) - 1 )

  /**
   * One bit for the eval/successmap
   * @param n number of the evaluation
   */
#define EVAL_MAP_BIT(n) ( 1 << (n) )


  typedef Scheduler* (*SchedulerAllocator)();
  typedef MKTask* (*TaskAllocator)(MKTask*);

  /**
   * @brief Convenience Store
   */
  struct MKEvalAllocatorPair {
  MKEvalAllocatorPair(const string _id, SchedulerAllocator _sa, TaskAllocator _ta)
  : id(_id), schedAlloc(_sa), taskAlloc(_ta) {}
    
    const string id;
    SchedulerAllocator schedAlloc;
    TaskAllocator taskAlloc;
  };


  struct MKSimulationResults : public SimulationResults {
    MKSimulationResults();
    MKSimulationResults(const SimulationResults& simStats);
    MKSimulationResults(const MKSimulationResults& rhs);
    bool mkfail;

    friend std::ostream& operator<< (std::ostream& out, const MKSimulationResults& stat);
  };

  std::ostream& operator<< (std::ostream& out, const MKSimulationResults& stat);

  /**
   * @brief Storage space for the evaluation of one task set with several
   * schedulers.
   *
   */
  class MKEval {
  public:
    /**
     * @brief C'tor.
     *
     * @param _taskset Class takes ownership of object
     * @param _nSchedulers
     * @param _allocators Array with _nSchedulers elements
     * @param _steps
     */
    MKEval(MKTaskSet* _taskset, unsigned int _nSchedulers,
	   const MKEvalAllocatorPair *_allocators, int _steps);

    /// @brief D'tor
    ~MKEval();

    /**
     * @brief Perform the actual evaluation.
     */
    void run();

    const MKSimulationResults* getResults() const;

    /**
     * @brief Get the successMap
     * @return the successMap of this evaluation
     */
    unsigned int getSuccessMap() const;

    const MKTaskSet* getTaskSet() const;

  private:
    /**
     * @brief Prepare the simulation object for one scheduler execution
     * @param num in 0...nSchedulers-1
     */
    void prepareEval(unsigned int num);

    /**
     * @brief Perform one simulation (with one scheduler).
     * @param num in 0...nSchedulers-1
     */
    void runEval(unsigned int num);
    

    MKTaskSet* taskset;
    unsigned int nSchedulers;
    const MKEvalAllocatorPair* allocators;
    int steps;

    Simulation** simulations;

    //NullStreambuf* nsb;
    //std::ostream** nos;
    //Logger** nolog;

    /**
     * Store task sets also as MKTasks (need this to check for fulfillment
     * of (m,k)-condition
     */
    std::vector<MKTask*>** mkts;


    MKSimulationResults* results;
    /**
     * The lower nSchedulers bits stand each for one simulation run.
     * A 1 indicates that the simulation was successful.
     */
    unsigned int successMap;

  };

} // NS tmssim

#endif // !MKEVAL_MKEVAL_H
