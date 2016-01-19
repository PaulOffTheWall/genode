/**
 * $Id: mkuedf.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file mkuedf.cpp
 * @brief HCUF-based (m,k)-firm real-time scheduling
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/mkuedf.h>

namespace tmssim {


  static const std::string myId = "MKUEDFScheduler";
  
  MKUEDFScheduler::MKUEDFScheduler() : OEDFMaxScheduler() {
  }


  MKUEDFScheduler::~MKUEDFScheduler() {
  }

  
  const std::string& MKUEDFScheduler::getId(void) const {
    return myId;
  }


  double MKUEDFScheduler::calcValue(int time, const Job *job) const {
    return job->getPossibleFailHistoryValue();
  }
 

  bool MKUEDFScheduler::isCancelCandidate(double value) const {
    return value >= 1;
  }

} // NS tmssim
