#if !defined(EVENT_H)
#define EVENT_H

#include <optional>

#include "task.h"

struct EventBase {
  enum class Type { kTimer, kTaskArrival, kTaskFinish, kIoRequest, kIoEnd };

  Type type;
  int time;
};

struct EventInternal : public EventBase {
  std::optional<std::reference_wrapper<RuntimeTask>> task;
};

struct EventExternal : public EventBase {
  std::optional<TaskExternal> task;

  bool operator==(const EventExternal& compare) const {
    return task->taskId == compare.task->taskId &&
           task->arrivalTime == compare.task->arrivalTime &&
           task->deadline == compare.task->deadline &&
           task->priority == compare.task->priority;
  }
};

class Timer {
 public:
  Timer(int timer);

  EventInternal peek();
  EventInternal next(int cur_time);

 private:
  const int timer;
  int prev_time;
};

class TaskGen {
 public:
  TaskGen(const TaskSerie& serie, std::vector<RuntimeTask>& tasks);

  bool has_next();
  EventInternal peek();
  EventInternal next();

 private:
  std::vector<RuntimeTask>::iterator task_next;
  std::vector<RuntimeTask>::iterator task_end;
};

class Cpu {
 public:
  bool has_next();
  EventInternal peek();

  void progress(int elapsed);
  void switch_to(RuntimeTask& task);
  void set_idle();
  int cur_taskId();

 private:
  int idle_duration = 0;
  int prev_time = 0;
  std::optional<std::reference_wrapper<RuntimeTask>> task;
};

class Io {
 public:
  bool has_next();
  EventInternal peek();

  void progress(int elapsed);
  void switch_to(RuntimeTask& task);
  int cur_taskId();

 private:
  int idle_duration = 0;
  int prev_time = 0;
  std::optional<std::reference_wrapper<RuntimeTask>> task;
};

bool operator<(const EventInternal& left, const EventInternal& right);
void to_json(json& j, const EventInternal& event);
void to_json(json& j, const EventExternal& event);
void to_json(json& j, const EventBase::Type& event_type);

#endif  // EVENT_H