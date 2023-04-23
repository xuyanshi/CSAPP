#include <string>

#include "task.h"
#include "util.h"

RuntimeTask::RuntimeTask(const TaskComplete& task) : TaskComplete(task) {
  static int next_taskId = 1;  // 0 for idle

  taskId = next_taskId;
  current_slice = 0;
  time_spent_in_cur_slice = 0;

  next_taskId++;
}

bool RuntimeTask::final_slice() { return current_slice == (slices.size() - 1); }

int RuntimeTask::slice_remaining() {
  assert(current_slice < slices.size());
  assert(slices[current_slice].second >= time_spent_in_cur_slice);
  return slices[current_slice].second - time_spent_in_cur_slice;
}

void RuntimeTask::progress(int elapsed) {
  int remaining = slice_remaining();
  assert(elapsed <= remaining);
  if (elapsed == remaining) {
    current_slice++;
    time_spent_in_cur_slice = 0;
  } else {
    time_spent_in_cur_slice += elapsed;
  }
}

bool RuntimeTask::cpu_next() {
  if (current_slice == slices.size() ||
      slices[current_slice].first == TaskComplete::ComputeType::kIo) {
    return false;
  } else {
    return true;
  }
}

int cal_needed_time(const TaskComplete& task) {
  int sum = 0;
  for (const TaskBase::Slice& slice : task.slices) {
    sum += slice.second;
  }
  return sum;
}

int cal_needed_time(const TaskSerie& serie) {
  int now = 0;
  for (const TaskComplete& task : serie) {
    if (task.arrivalTime > now) {
      now = task.arrivalTime;
    }
    now += cal_needed_time(task);
  }
  return now;
}

void from_json(const json& j, TaskSerie& serie) {
  serie.clear();
  for (auto it = j.begin(); it != j.end(); ++it) {
    TaskComplete task = *it;
    serie.push_back(task);
  }
}

void to_json(json& j, const TaskComplete& task) {
  j = json{{"arrivalTime", task.arrivalTime},
           {"deadline", task.deadline},
           {"priority", task.priority},
           {"slices", task.slices}};
}

void from_json(const json& j, TaskComplete& task) {
  j.at("arrivalTime").get_to(task.arrivalTime);
  j.at("deadline").get_to(task.deadline);
  j.at("priority").get_to(task.priority);
  j.at("slices").get_to(task.slices);
}

void to_json(json& j, const TaskComplete::ComputeType& compute_type) {
  j = json(compute_type == TaskComplete::ComputeType::kCpu ? "CPU" : "IO");
}

void from_json(const json& j, TaskComplete::ComputeType& compute_type) {
  compute_type = j.get<std::string>() == "CPU" ? TaskComplete::ComputeType::kCpu
                                               : TaskComplete::ComputeType::kIo;
}

void to_json(json& j, const TaskComplete::Priority& prio) {
  j = json(prio == TaskComplete::Priority::kHigh ? "high" : "low");
}

void from_json(const json& j, TaskComplete::Priority& prio) {
  prio = j.get<std::string>() == "high" ? TaskComplete::Priority::kHigh
                                        : TaskComplete::Priority::kLow;
}

void from_json(const json& j, std::vector<TaskComplete::Slice>& slices) {
  slices.clear();
  for (auto it = j.begin(); it != j.end(); ++it) {
    TaskComplete::Slice slice = *it;
    slices.push_back(slice);
  }
}

void from_json(const json& j, TaskComplete::Slice& slice) {
  slice.first = j[0];
  slice.second = j[1];
}

void to_json(json& j, const RuntimeTask& task) {
  j = json{{"arrivalTime", task.arrivalTime},
           {"deadline", task.deadline},
           {"priority", task.priority},
           {"taskId", task.taskId},
           {"current_slice", task.current_slice},
           {"slices", task.slices},
           {"time_spent_in_cur_slice", task.time_spent_in_cur_slice}};
}

void to_json(json& j, const TaskExternal& task) {
  j = json{{"arrivalTime", task.arrivalTime},
           {"deadline", task.deadline},
           {"priority", task.priority},
           {"taskId", task.taskId}};
}
