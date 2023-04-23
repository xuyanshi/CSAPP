#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>

#include <nlohmann/json.hpp>

#include "policy_wrapper.h"
#include "sim.h"
#include "util.h"

using json = nlohmann::json;

int main(int argc, char const *argv[]) {
  INVARIANT(argc == 3);

  std::fstream f(argv[1]);
  INVARIANT(f.is_open());
  json j;
  f >> j;

  SimConfig sim_config = j;

  f.close();
  f.open(argv[2]);
  INVARIANT(f.is_open());
  f >> j;

  TaskSerie serie = j;
  SimRes res = simulate(serie, sim_config);
  CERR_VAR(res.finish_rate_hi_prio);
  CERR_VAR(res.finish_rate_lo_prio);
  CERR_VAR(res.elapsed_time);
  CERR_VAR(cal_needed_time(serie));

  return 0;
}

SimRes simulate(const TaskSerie &serie, const SimConfig &sim_config) {
  std::vector<RuntimeTask> all_tasks;  // all concrete RuntimeTask objects
  Timer timer(sim_config.timer);
  TaskGen task_gen(serie, all_tasks);  // populate all_tasks
  Cpu cpu;
  Io io;

  int time = 0;
  std::map<int, RuntimeTask &> exposed_and_running_tasks;

  int finish_cnt_hi_prio = 0;
  int missed_cnt_hi_prio = 0;
  int finish_cnt_lo_prio = 0;
  int missed_cnt_lo_prio = 0;
  double amplification = 0;

  int last_event_time = -1;

  int max_time = cal_needed_time(serie);

  while (true) {
    std::vector<EventInternal> candidates = {timer.peek()};
    if (task_gen.has_next()) {
      candidates.push_back(task_gen.peek());
    }
    if (cpu.has_next()) {
      candidates.push_back(cpu.peek());
    }
    if (io.has_next()) {
      candidates.push_back(io.peek());
    }

    std::sort(candidates.begin(), candidates.end());
    std::vector<std::reference_wrapper<EventInternal>> nearests = {
        candidates.front()};
    for (auto it = candidates.begin() + 1; it != candidates.end(); it++) {
      if (it->time == nearests.front().get().time) {
        nearests.emplace_back(*it);
      } else {
        break;
      }
    }

    int elapsed = nearests.front().get().time - time;
    time = nearests.front().get().time;
    cpu.progress(elapsed);
    io.progress(elapsed);

    if (time == last_event_time) {
      terminate("error");
    }
    if (time > max_time) {
      terminate("error TLE");
    }

    last_event_time = time;

    for (const EventInternal &nearest : nearests) {
      if (nearest.type == EventInternal::Type::kTimer) {
        timer.next(time);
      } else if (nearest.type == EventInternal::Type::kTaskArrival) {
        INVARIANT(nearest.task.has_value());
        RuntimeTask &task = nearest.task.value().get();

        task_gen.next();
        exposed_and_running_tasks.emplace(task.taskId, task);
      } else if (nearest.type == EventInternal::Type::kTaskFinish) {
        INVARIANT(nearest.task.has_value());
        RuntimeTask &task = nearest.task.value().get();

        exposed_and_running_tasks.erase(task.taskId);

        if (task.priority == TaskBase ::Priority::kHigh) {
          if (time <= task.deadline) {
            finish_cnt_hi_prio++;
          } else {
            missed_cnt_hi_prio++;
          }
        } else {
          if (time <= task.deadline) {
            finish_cnt_lo_prio++;
          } else {
            missed_cnt_lo_prio++;
          }
        }
        amplification +=
            1.0 * (time - task.arrivalTime) / (task.deadline - task.arrivalTime);

      } else if (nearest.type == EventInternal::Type::kIoRequest) {
      } else if (nearest.type == EventInternal::Type::kIoEnd) {
      }
    }

    int currentCpuTask = cpu.cur_taskId();
    int currentIoTask = io.cur_taskId();

    Action action = policy(nearests, currentCpuTask, currentIoTask);
    if (action.cpuTask == action.ioTask && action.cpuTask != 0) {
      terminate("error invalid action");
    }

    if (action.cpuTask != 0) {
      INVARIANT(exposed_and_running_tasks.count(action.cpuTask) == 1);
      RuntimeTask &task = exposed_and_running_tasks.at(action.cpuTask);
      if (!task.cpu_next()) {
        terminate("error invalid action");
      }
      cpu.switch_to(task);
    } else {
      cpu.set_idle();
    }
    if (action.ioTask != 0) {
      INVARIANT(exposed_and_running_tasks.count(action.ioTask) == 1);
      RuntimeTask &task = exposed_and_running_tasks.at(action.ioTask);
      if (task.cpu_next()) {
        terminate("error invalid action");
      }
      io.switch_to(task);
    }

    if (exposed_and_running_tasks.empty() && !task_gen.has_next()) {
      finish();
      break;
    }

#if defined(SCHED_DEBUG)
    if (cpu.cur_taskId() == 0 && io.cur_taskId() == 0 &&
        !exposed_and_running_tasks.empty()) {
      CERR_THIS("machine has been idle since last event, but "
                << exposed_and_running_tasks.size()
                << " task(s) is/are still pending");
    }
#endif
  }

  SimRes res;
  res.finish_rate_hi_prio =
      1.0 * finish_cnt_hi_prio / (finish_cnt_hi_prio + missed_cnt_hi_prio);
  res.finish_rate_lo_prio =
      1.0 * finish_cnt_lo_prio / (finish_cnt_lo_prio + missed_cnt_lo_prio);
  res.elapsed_time = time;
  amplification /= all_tasks.size();
  CERR_VAR(amplification);

  return res;
}

Action policy(const std::vector<std::reference_wrapper<EventInternal>> &events,
              int currentCpuTask, int currentIoTask) {
  std::vector<EventExternal> events_stripped;
  for (const EventInternal &event : events) {
    EventExternal strip;
    strip.type = event.type;
    strip.time = event.time;
    if (event.task.has_value()) {
      strip.task = TaskExternal();
      strip.task.value().taskId = event.task.value().get().taskId;
      strip.task.value().arrivalTime = event.task.value().get().arrivalTime;
      strip.task.value().deadline = event.task.value().get().deadline;
      strip.task.value().priority = event.task.value().get().priority;
    }
    events_stripped.push_back(strip);
  }

  return ask_policy(events_stripped, currentCpuTask, currentIoTask);
}

void terminate(const std::string &msg) {
  std::cout << json(msg) << std::endl;
  CERR_THIS("0");
  exit(0);
}

void finish() {
#if defined(JAVA_SOLUTION)
  destroy_jvm();
  return;
#endif
#if !defined(CPP_SOLUTION)
  std::cout << json("end") << std::endl;
#endif
}

void to_json(json &j, const SimConfig &config) {
  j = json{{"timer", config.timer}};
}

void from_json(const json &j, SimConfig &config) {
  j.at("timer").get_to(config.timer);
}

void to_json(json &j, const Action &action) {
  j = json{{"cpuTask", action.cpuTask}, {"ioTask", action.ioTask}};
}

void from_json(const json &j, Action &action) {
  j.at("cpuTask").get_to(action.cpuTask);
  j.at("ioTask").get_to(action.ioTask);
}
