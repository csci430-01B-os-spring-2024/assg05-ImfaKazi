/** @file RRSchedulingPolicy.hpp
 * @brief RRSchedulingPolicy class implementation file
 *
 * @author Ethan Jansak-Noble
 * @note   cwid: 50290824
 * @date   Spring 2024
 * @note   ide:  g++ 8.2.0 / GNU Make 4.2.1
 *
 * Implementation file for our RRSchedulingPolicy class.  The
 * RRSchedulingPolicy class is an abstract base class that defines the
 * interface for implementing different job scheduling policies.
 * Basically any scheduling policy needs to be notified when a
 * newProcess() arrives, a process runs a runCpuCycle(), and when a
 * process finished().  The main method of this stategy abstraction is
 * the dispatch() method, which is called by the scheduling simulator
 * whenever the cpu is idle, and the scheduling policy needs to make a
 * decision.
 */
#include "RRSchedulingPolicy.hpp"
#include <queue>

using namespace std;

/**
 * @brief constructor
 *
 * The basic constructor is called by the PagingSystem when setting up/creating
 * the paging scheme to use for a simulation.  We keep track of the paging
 * system instance we are associated with, so we can call the paging system to
 * get needed information to make replacment decisions.
 */
RRSchedulingPolicy::RRSchedulingPolicy(int simQuantum)
  : SchedulingPolicy()
{
  sys = NULL;
  // set the quantum for the current simulation
  quantum = simQuantum;
  resetPolicy();
}

/**
 * @brief destructor
 *
 * Define a concrete destructor.  This destructor has no work to do, but
 * base classes that need a destructor should define their own.
 */
RRSchedulingPolicy::~RRSchedulingPolicy() {}

/** new process
 * Handle new process arrivals.  When a new process arrives it is
 * put in a ready to run state.  We add it to the end of our
 * ready queue.
 *
 * @param pid The process identifier (pid) of the newly arriving
 *   process that should now be managed by this policy.
 */
void RRSchedulingPolicy::newProcess(Pid pid)
{
  // put the new process on the end of the ready queue
  readyQueue.push(pid);
}

/** dispatch a process
 * When the cpu is idle, the scheduling simulator calls this
 * method of the policy object to select which process to
 * dispatch and schedule and run next on the cpu.  The
 * Round Robin (RR) policy simply selects the
 * process that has been waiting the longest in the ready
 * queue giving it a time quantum, after a process has been
 * run to completion or been preempted by the time quantum.
 *
 * @returns pid Returns the process identifier of the process
 *   we select to run next.
 */
Pid RRSchedulingPolicy::dispatch()
{
  // make sure the ready queue is not empty, if it is we
  // can't dispatch at this time
  if (readyQueue.empty())
  {
    return IDLE;
  }
  // otherwise pop the front item and return it
  else
  {
    runningPid = readyQueue.front();
    readyQueue.pop();
    // set the quantum clock to the simulation quantum
    quantumClock = quantum;
    // decrease clock as the process runs for one cycle after being dispatched
    quantumClock -= 1;
    return runningPid;
  }
}

/**
 * @brief preemption
 *
 * Determine if current running processes needs to be preempted.
 * RR is a preemptive(at time quantum) policy, so when the quantum
 * clock reaches zero the process is preempted. This means every time
 * a cpu cycle is simulated this method is called meaning the clock is
 * decremented by one.
 *
 * @returns bool returns true when the time quantum clock has reached
 *  zero
 */
bool RRSchedulingPolicy::preempt()
{
  // if the quantum clock reaches zero preempt
  if (quantumClock == 0)
  {
    // push the currently running process onto the queue as it hasn't
    // finished running
    readyQueue.push(this->runningPid);
    // no process is running on the CPU so runningPid is set to idle
    runningPid = IDLE;
    return true;
  }
  else
  {
    // decrease the time quantum to simulate a CPU cycle
    quantumClock -= 1;
    // should not be preempted as time slice is not used
    return false;
  }
}

/** reset policy
 * Reset or initialize the scheduling policty to an initial state,
 * in preparation for beginning a simulation.  For RR this means
 * we want to clear out the ready queue and make sure it is
 * empty to begin with, set the proper quantum for the simulation,
 * and set the initial value for the quantumClock.
 */
void RRSchedulingPolicy::resetPolicy()
{
  // make sure the queue is empty, swap a queue with a known
  // empty queue will free up the old queue and ensure we start
  // with an empty one.
  queue<Pid> empty;
  swap(readyQueue, empty);
  // set the clock to innitially be the same time as the quantum
  quantumClock = quantum;
}