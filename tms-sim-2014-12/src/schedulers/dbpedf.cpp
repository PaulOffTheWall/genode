/**
 * $Id: dbpedf.cpp 715 2014-09-25 09:25:02Z klugeflo $
 * @file dbpedf.cpp
 * @brief Utility-based EDF using distance from fail state
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/dbpedf.h>

namespace tmssim {

  static const std::string myId = "DBPEDFScheduler";
  
  DBPEDFScheduler::DBPEDFScheduler() : OEDFMaxScheduler() {
  }


  DBPEDFScheduler::~DBPEDFScheduler() {
  }

  
  const std::string& DBPEDFScheduler::getId(void) const {
    return myId;
  }


  double DBPEDFScheduler::calcValue(int time, const Job *job) const {
    //return job->getPossibleFailHistoryValue();
    return job->getTask()->getDistance();
  }
 

  bool DBPEDFScheduler::isCancelCandidate(double value) const {
    return value > 1;
  }

} // NS tmssim
