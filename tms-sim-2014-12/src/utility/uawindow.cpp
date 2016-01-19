/**
 * $Id: uawindow.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file uawindow.cpp
 * @brief Implementation of aggregator over a sliding window
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/uawindow.h>

#include <utils/tlogger.h>

#include <cassert>
using namespace std;
namespace tmssim {

  UAWindow::UAWindow(size_t _size, double initValue)
    : UtilityAggregator(), size(_size), current(_size-1)
  {
    values = new double[size];
    for (unsigned int i = 0; i < size; ++i) {
      values[i] = initValue;
    }
  }


  UAWindow::UAWindow(const UAWindow& rhs)
    : UtilityAggregator(), size(rhs.size), current(size-1)
  {
    values = new double[size];
    for (unsigned int i = 0; i < size; ++i) {
      values[i] = rhs.values[i];
    }
  }
  
  
  UAWindow::~UAWindow(void) {
    delete[] values;
  }
  
  
  void UAWindow::addHook(double u) {
    if (current == 0) current += size;
    --current;
    tDebug() << "current @ addHook: " << current;
    assert(current < size && current >= 0);
    values[current] = u;
  }
  
  
  double UAWindow::operator [](size_t offset) const {
    return values[(current+offset)%size];
  }
  
  
  size_t UAWindow::getSize(void) const {
    return size;
  }
  
  
  double UAWindow::oldest(void) const {
    return (*this)[size-1];
  }


  int UAWindow::writeData(xmlTextWriterPtr writer) {
    return UtilityAggregator::writeData(writer);
  }

} // NS tmssim
