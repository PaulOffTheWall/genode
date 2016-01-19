/**
 * $Id: mkmonitor.h 689 2014-09-13 19:31:06Z klugeflo $
 * @file mkmonitor.h
 * @brief Monitoring of (m,k)-firm condition
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef TASKMODELS_MKMONITOR_H
#define TASKMODELS_MKMONITOR_H 1

namespace tmssim {

  /**
   * @brief Monitoring of (m,k) constraint.
   *
   * This class implements a ring buffer to monitor the a sliding window
   * of \f$k\f$ job executions.
   *
   * @todo add check for (m,k) constraint, actually use m
   */
  class MKMonitor {
  public:
    /**
     * @brief C'tor
     *
     * @param _m \f$m\f$ of the (m,k) constraint (currently not used!)
     * @param _k \f$k\f$ of the (m,k) constraint
     * @param _init initial value for all buffer entries
     */
    MKMonitor(unsigned int _m, unsigned int _k, unsigned int _init=1);
    /**
     * @brief D'tor
     */
    virtual ~MKMonitor();

    /**
     * @brief Add a new value to the buffer.
     *
     * Pushing a new value into the buffer leads to the oldest value
     * being removed from the buffer.
     * @param _val the new value to be added
     */
    void push(unsigned int _val);

    /**
     * @brief Get the current sum of all buffer entries.
     * @return Current sum
     */
    int getCurrentSum() const;

    /**
     * @brief Get the complete buffer.
     *
     * Note: the returned array is not ordered! Use the getPos() method
     * to get the position of the oldest entry. The new entry is
     * located at \f$p-1 (\% k)\f$.
     * @return the complete buffer
     */
    const unsigned int* getVals() const;

    /**
     * @brief Get the position of the oldest entry in the buffer.
     *
     * @return position
     */
    unsigned int getPos() const;

    /**
     * @brief Get the number of violations of the (m,k)-constraint
     * @return the number
     */
    unsigned int getViolations() const;

  private:
    unsigned int m;
    unsigned int k;
    /**
     * Points to next write field in buffer
     */
    unsigned int pos;

    /**
     * Ring buffer
     */
    unsigned int* vals;

    /**
     * Current sum of the values that are in the buffer. This value is
     * updated any time the buffer is changed by the push() method.
     */
    unsigned int sum;

    unsigned int violations;
  };

} // NS tmssim

#endif /* !TASKMODELS_MKMONITOR_H */
