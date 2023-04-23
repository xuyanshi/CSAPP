#if !defined(SIM_H)
#define SIM_H

#include <optional>

#include "event.h"
#include "policy.h"
#include "task.h"

/* types */
struct SimConfig {
  int timer;
};

struct SimRes {
  double finish_rate_hi_prio;
  double finish_rate_lo_prio;
  int elapsed_time;
};

/* prototypes */
SimRes simulate(const TaskSerie& serie, const SimConfig& sim_config);
Action policy(const std::vector<std::reference_wrapper<EventInternal>>& events,
              int currentCpuTask, int currentIoTask);
void terminate(const std::string& msg);
void finish();

void to_json(json& j, const SimConfig& config);
void from_json(const json& j, SimConfig& config);
void to_json(json& j, const Action& action);
void from_json(const json& j, Action& action);

#endif  // SIM_H
