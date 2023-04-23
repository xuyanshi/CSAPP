#if !defined(POLICY_H)
#define POLICY_H

#include <vector>

struct Event {
  enum class Type { kTimer, kTaskArrival, kTaskFinish, kIoRequest, kIoEnd };
  struct Task {
    enum class Priority { kHigh, kLow };

    int arrivalTime;
    int deadline;
    Priority priority;
    int taskId;
  };

  Type type;
  int time;
  Task task;
};

struct Action {
  int cpuTask, ioTask;
};

Action policy(const std::vector<Event>& events, int current_cpu,
              int current_io);

#endif  // POLICY_H