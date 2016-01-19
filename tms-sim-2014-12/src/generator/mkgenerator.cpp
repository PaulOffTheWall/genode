/**
 * $Id: mkgenerator.cpp 807 2014-12-12 11:16:35Z klugeflo $
 * @file mkgenerator.cpp
 * @brief Generation of (m,k)-tasksets
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <generator/mkgenerator.h>

#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <list>

#include <limits.h>

#include <taskmodels/mktask.h>
#include <utility/ucfirm.h>
#include <utility/uawmk.h>
//#define TLOGLEVEL TLL_DEBUG
#include <utils/tlogger.h>

using namespace std;

namespace tmssim {

  UtilityCalculator* DefaultUCMKAllocator() {
    return new UCFirmRT;
  }


  UtilityAggregator* DefaultUAMKAllocator(unsigned int m, unsigned int k) {
    return new UAWMK(m, k);
  }


  MKTaskSet::MKTaskSet() : utilisation(0.0), suffMKSched(false) {}

  MKTaskSet::~MKTaskSet() {
    for (vector<MKTask*>::iterator it = tasks.begin();
	 it != tasks.end(); ++it) {
      delete *it;
    }
    tasks.clear();
  }


  MKGenerator::MKGenerator(unsigned int _seed, unsigned int _size,
			   unsigned int _minPeriod, unsigned int _maxPeriod,
			   unsigned int _minK, unsigned int _maxK,
			   float _utilisation, float _utilisationDeviation,
			   unsigned int _maxWC,
			   UCMKAllocator _ucAlloc, UAMKAllocator _uaAlloc)
    : baseSeed(_seed), size(_size), minPeriod(_minPeriod), maxPeriod(_maxPeriod),
      minK(_minK), maxK(_maxK), utilisation(_utilisation),
      utilisationDeviation(_utilisationDeviation), maxWC(_maxWC),
      ucAlloc(_ucAlloc), uaAlloc(_uaAlloc)
  {
  }
  

  MKGenerator::~MKGenerator() {
  }


  //std::vector<MKTask*>* MKGenerator::nextTaskSet() {
  MKTaskSet* MKGenerator::nextTaskSet() {
    tDebug() << "Generating next taskset using seed " << baseSeed;
    unsigned int seed = newSeed();
    return nextTaskSet(seed);
  }
  
  MKTaskSet* MKGenerator::nextTaskSet(unsigned int tsSeed) {
    unsigned int sumWC = 0;
    float sumU = 0.0;
    MKTaskSet *ts = NULL;// = new MKTaskSet();
    int ctr = 0;
    float lbu = utilisation - utilisationDeviation;
    float ubu = utilisation + utilisationDeviation;
    //cout << "TSGEN utilisation: " << utilisation << " utilisationDeviation: "
    // << utilisationDeviation << endl;
    //cout << "TSGEN lbu: " << lbu << " ubu: " << ubu << " U: " << sumU << endl;
    while ( sumU < (utilisation - utilisationDeviation)
    	    || sumU > (utilisation + utilisationDeviation) ) {
      //cout << "TSGEN Attempt " << ctr++ << " (last U=" << sumU << ")" << endl;
      list<TempTask *> tmpTS;
      sumU = 0;
      sumWC = 0;
      if (ts != NULL)
	delete ts;
      ts = new MKTaskSet();
      ts->seed = tsSeed;
      // generate basic parameters
      for (int i = 0; i < size; ++i) {
	TempTask * tt = new TempTask;
	tt->period = produceInt(&tsSeed, minPeriod, maxPeriod);
	tt->k = produceInt(&tsSeed, minK, maxK);
	tt->m = produceInt(&tsSeed, 1, tt->k);
	tt->wc = produceInt(&tsSeed, 1, maxWC);
	sumWC += tt->wc;
	tmpTS.push_back(tt);
	//cout << "TT: wc=" << tt->wc << " p=" << tt->period
	//   << " m=" << tt->m << " k=" << tt->k << endl;
      }
      
      // sort
      
      
      // Perform rate-monotonic priority assignment
      list<TempTask *> tmpPri;
      for (list<TempTask *>::iterator it = tmpTS.begin();
	   it != tmpTS.end(); ++it) {
	list<TempTask *>::iterator ins = tmpPri.begin();
	while (ins != tmpPri.end() && (*ins)->period < (*it)->period)
	  ++ins;
	tmpPri.insert(ins, *it);
      }
      int i = 0;
      for (list<TempTask *>::iterator it = tmpPri.begin();
	   it != tmpPri.end(); ++it, ++i) {
	(*it)->priority = i;
      }
      
      
      //vector<MKTask*>* ts = new vector<MKTask*>(size);
      float uw = utilisation / sumWC; // $\frac{U}{W}$ to speed up calculation
      i = 0;
      //float sumFU = 0;
      
      //cout << "sumWC=" << sumWC << " uw=" << uw << endl;
      for (list<TempTask *>::iterator it = tmpPri.begin();
	   it != tmpPri.end(); ++it, ++i) {
	TempTask *tt = *it; // only for convenience
	float fci = uw * tt->period * tt->wc;
	//sumFU += fci/tt->period;
	//cout << "\t" << i << ": fci=" << fci << endl;
	tt->c = lround(fci);
	if (tt->c == 0)
	  tt->c = 1;
	sumU += (float)tt->c / (float)tt->period;
	MKTask* task = new MKTask(i, tt->period, tt->c, tt->period, ucAlloc(),
				  uaAlloc(tt->m, tt->k), tt->priority, tt->m, tt->k);
	ts->tasks.push_back(task);
	delete tt;
      }
      tmpPri.clear();
      tmpTS.clear();
    }
    ts->utilisation = sumU;
    ts->suffMKSched = testSufficientSchedulability(ts->tasks);
    if (ts->suffMKSched)
      MKGenerator::calcRotationValues(ts->tasks);
    
    //printf("Actual taskset utilisation: %f sched: %d\n", sumU, ts->suffMKSched);
    return ts;    
  }


  bool MKGenerator::testSufficientSchedulability(vector<MKTask*>& tasks) {
    unsigned int min = UINT_MAX;

    for (int i = 0; i < tasks.size(); ++i) {
      int sum = 0;
      for (int j = 0; j < i - 1; ++j) {
	sum += tasks[j]->getExecutionTime() * calcNij(tasks[i], tasks[j]);
      }
      sum += tasks[i]->getExecutionTime();
      if (sum > tasks[i]->getPeriod())
	return false;
    }
    return true;
  }
  

  int MKGenerator::calcNij(MKTask* ti, MKTask* tj) const {
    double mj = tj->getM();
    double kj = tj->getK();
    double pi = ti->getPeriod();
    double pj = tj->getPeriod();

    double nij = ceil( mj / kj * ceil(pi / pj) );
    return round(nij);
  }


  unsigned int MKGenerator::newSeed() {
    return rand_r(&baseSeed);
  }

  unsigned int MKGenerator::produceInt(unsigned int* seed, unsigned int min, unsigned int max) {
    assert(min < max);
    unsigned int len = max - min + 1; // due to modulo arithmetics
    unsigned long long int interval = RAND_MAX / len;
    unsigned long long int rnd = rand_r(seed);
    unsigned int number = (uint32_t) ((uint64_t)rnd / interval);
    return number + min;
  }


  int MKGenerator::calcRotationValues(vector<MKTask*>& tasks) {
    tDebug() << "Calculating rotation values...";
    list<MKTask*> psi;
    list<MKTask*> ts; // \mathcal{T} in original algorithm
    for (vector<MKTask*>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
      ts.push_back(*it);
    }
    while (ts.size() > 0) {
      list<MKTask*>::iterator min = ts.begin();
      // find t_i = task in T with smallest k_i
      for (list<MKTask*>::iterator it = ++ts.begin(); it != ts.end(); ++it) {
	if ( (*it)->getK() < (*min)->getK() )
	  min = it;
      }
      MKTask* ti = *min;
      tDebug() << "... for task " << *ti;
      if (psi.size() > 0) {
	list<MKTask*> omega = psi;
	MKTask* tj = NULL;
	  unsigned int g = 0;
	while (omega.size() > 0) {
	  unsigned int maxval = 0;
	  list<MKTask*>::iterator max = omega.begin();
	  for (list<MKTask*>::iterator it = omega.begin();
	       it != omega.end(); ++it) {
	    unsigned int val;
	    if (val = calcExecutionInterference(MKCTask(ti), MKCTask(*it)) > maxval) {
	      max = it;
	      maxval = val;
	    }	    
	  }
	  tj = *max;
	  g = calcGCD(ti->getK() * ti->getPeriod(),
				   (*max)->getK() * (*max)->getPeriod());
	  if (g == 1)
	    omega.erase(max);
	  else
	    break;
	}
	tDebug() << "Found tj = " << *tj;
	// O_j' = O_j + s_j Tj
	int osj = tj->getOffset() + tj->getSpin() * tj->getPeriod();
	// s_i = l such that 0 <= l <= k_i
	// and |l T_i + 0_i - O_j'| is nearest to one of
	// (2q+1)g/2, q\in Z
	int lOpt;
	float distance = FLT_MAX;
	for (int l = 0; l < ti->getK(); ++l) {
	  tDebug() << "l=" << l << " Ti=" << ti->getPeriod()
		   << " Oi=" << ti->getOffset() << " Oj'=" << osj;
	  int val = abs(l * (int)ti->getPeriod() + (int)ti->getOffset() - osj);
	  int q = floor((float)val / g - 0.5);
	  float lb = ((float)q * 2 + 1) * g / 2;
	  float ub = ((float)(q+1) * 2 + 1) * g / 2;
	  tDebug() << "\tval: " << val << " lb: " << lb << " ub: " << ub;
	  assert( lb <= val);
	  assert( ub >= val);
	  float dl = val - lb;
	  float du = ub - val;
	  if (dl < distance) {
	    lOpt = l;
	    distance = dl;
	  }
	  if (du < distance) {
	    lOpt = l;
	    distance = du;
	  }
	}
	ti->setSpin(lOpt);
      }
      else {
	(*min)->setSpin(0);
      }
      psi.push_back(*min);
      ts.erase(min);
    }
  }


  int MKGenerator::calcExecutionInterference(const MKCTask& ti, const MKCTask& th) {
    unsigned int fhi = 0;
    unsigned int g = calcGCD(ti.getK() * ti.getPeriod(),
			     th.getK() * th.getPeriod());
    for (int j = 1; j <= ti.getK(); ++j) {
      // CAUTION: jobs are counted from 0, not 1:
      if (ti.isJobMandatory(j-1)) {
	unsigned int x = (ti.getOffset() + j * ti.getPeriod() - th.getOffset()) % g;
	while (x < th.getK() * th.getPeriod()) {
	  // FIXME: CHECK usage of j and j-1
	  // l^h_{ij} number of mandatory jobs of \tau_h that fall
	  // entirely in the interval
	  // [(j-1)T_i+O_i, jT_i+O_i]
	  unsigned int lIntervalLow = (j - 1) * ti.getPeriod() + ti.getOffset();
	  unsigned int lIntervalHigh = j * ti.getPeriod() + ti.getOffset();
	  //unsigned int lhij = calcLhij(th, lIntervalLow, lIntervalHigh);
	  // we need to calculate this here to reuse the first/last values
	  // for s and t
	  unsigned int first = floor( ( (double) lIntervalLow
					- (double) th.getOffset() )
				      / (double) th.getPeriod() ) + 1;
	  unsigned int last = floor ( ( (double) lIntervalHigh
					- (double) th.getOffset() )
				      / (double) th.getPeriod() );
	  unsigned int lhij = 0;
	  for (int i = first; i <= last; ++i) {
	    if (th.isJobMandatory(i))
	      ++lhij;
	  }
	  // e_s = \pi_{hs} min\{C_h + r_{hs} - r_{i(j-1)}, 0\}
	  assert(first > 0);
	  unsigned int s = first - 1;
	  unsigned int es = 0;
	  if (th.isJobMandatory(s)) {
	    unsigned int rhs = th.getOffset() + s * th.getPeriod();
	    unsigned int rij1 = ti.getOffset() + (j-1) * ti.getPeriod();
	    es = min(th.getExecutionTime() + rhs - rij1, 0U);
	  }

	  // e_t = \pi_{ht} min\{C_h, r_{ij} - r_{ht}\}
	  unsigned int t = last + 1;
	  unsigned int et = 0;
	  if (th.isJobMandatory(t)) {
	    int rij = ti.getOffset() + j * ti.getPeriod();
	    int rht = th.getOffset() + t * th.getPeriod();
	    et = min(th.getExecutionTime(), rij - rht);
	  }


	  // F^h_{ij} = e_s + l^h_{ij} C_i + e_t
	  unsigned int fhij = es + lhij * ti.getExecutionTime() + et;
	  if (fhi < fhij) {
	    fhi = fhij;
	  }
	  x += g;
	}
      }
    }
    return fhi;
  }


  unsigned int MKGenerator::calcGCD(unsigned int _a, unsigned int _b) {
    // use euclid's algorithm
    unsigned int a = max(_a, _b);
    unsigned int b = min(_a, _b);
    unsigned int tmp;
    if (b == 0)
      return a;

    while (b != 0) {
      tmp = a % b;
      a = b;
      b = tmp;
    }
    return a;
  }


  unsigned int MKGenerator::calcLhij(const MKCTask& task, unsigned int low, unsigned int high) {

    unsigned int first = floor( ( (double) low - (double) task.getOffset() )
				/ (double) task.getPeriod() ) + 1;
    unsigned int last = floor ( ( (double) high - (double) task.getOffset() )
				/ (double) task.getPeriod() );

    unsigned int ctr = 0;

    for (int i = first; i <= last; ++i) {
      if (task.isJobMandatory(i))
	++ctr;
    }
    return ctr;
  }


} // NS tmssim
