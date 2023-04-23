#if !defined(TASK_H)
#define TASK_H

#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct TaskComplete;
typedef std::vector<TaskComplete> TaskSerie;

struct TaskBase {
  enum class ComputeType;
  typedef std::pair<ComputeType, int> Slice;

  enum class ComputeType { kCpu, kIo };
  enum class Priority { kHigh, kLow };

  int arrivalTime;
  int deadline;
  Priority priority;
};

struct TaskExternal : public TaskBase {
  int taskId;
};

struct TaskComplete : public TaskBase {
  std::vector<Slice> slices;
};

class RuntimeTask : public TaskComplete {
  friend class TaskGen;

 public:
  RuntimeTask(const TaskComplete& task);

  bool final_slice();
  int slice_remaining();
  void progress(int elapsed);
  bool cpu_next();

  friend void to_json(json& j, const RuntimeTask& task);

 public:
  int taskId;
  int current_slice;
  int time_spent_in_cur_slice;
};

int cal_needed_time(const TaskComplete& task);
int cal_needed_time(const TaskSerie& serie);

void from_json(const json& j, TaskSerie& serie);
void to_json(json& j, const TaskComplete& task);
void from_json(const json& j, TaskComplete& task);
void to_json(json& j, const TaskComplete::ComputeType& compute_type);
void from_json(const json& j, TaskComplete::ComputeType& compute_type);
void to_json(json& j, const TaskComplete::Priority& prio);
void from_json(const json& j, TaskComplete::Priority& prio);
void from_json(const json& j, std::vector<TaskComplete::Slice>& slices);
void from_json(const json& j, TaskComplete::Slice& slice);
void to_json(json& j, const TaskExternal& task);

#endif  // TASK_H
