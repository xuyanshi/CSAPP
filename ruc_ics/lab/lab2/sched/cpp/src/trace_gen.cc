#include <fstream>
#include <iostream>
#include <random>

#include <nlohmann/json.hpp>

#include "task.h"
#include "trace_gen.h"
#include "util.h"

using json = nlohmann::json;

TaskTrait TaskTrait::short_default() {
  TaskTrait task_trait;
  task_trait.type = TaskTrait::TaskType::kShort;
  return task_trait;
}

TaskTrait TaskTrait::regular_default() {
  TaskTrait task_trait;
  task_trait.type = TaskTrait::TaskType::kRegular;
  NonShortDetail detail;
  detail.io_dominance_ratio = 0.5;
  detail.long_io_ratio = 0.5;
  task_trait.detail = detail;
  return task_trait;
}

TaskTrait TaskTrait::long_default() {
  TaskTrait task_trait;
  task_trait.type = TaskTrait::TaskType::kLong;
  NonShortDetail detail;
  detail.io_dominance_ratio = 0.5;
  detail.long_io_ratio = 0.5;
  task_trait.detail = detail;
  return task_trait;
}

TaskTrait TaskTrait::mixed_default() {
  TaskTrait task_trait;
  task_trait.type = TaskTrait::TaskType::kMixed;
  MixedDetail detail;
  detail.short_ratio = 0.6;
  detail.regular_ratio = 0.35;
  detail.regular_task.io_dominance_ratio = 0.5;
  detail.regular_task.long_io_ratio = 0.5;
  detail.long_task.io_dominance_ratio = 0.5;
  detail.long_task.long_io_ratio = 0.5;
  task_trait.detail = detail;
  return task_trait;
}

TaskTrait TaskTrait::shifting_default() {
  TaskTrait task_trait;
  task_trait.type = TaskTrait::TaskType::kShifting;
  ShiftingDetail detail;
  // stage 1
  detail.emplace_back();
  detail.back().first = 0;
  detail.back().second.short_ratio = 0.8;
  detail.back().second.regular_ratio = 0.1;
  detail.back().second.regular_task.io_dominance_ratio = 0.5;
  detail.back().second.regular_task.long_io_ratio = 0.5;
  detail.back().second.long_task.io_dominance_ratio = 0.5;
  detail.back().second.long_task.long_io_ratio = 0.5;
  // stage 2
  detail.emplace_back();
  detail.back().first = 0.5;
  detail.back().second.short_ratio = 0.2;
  detail.back().second.regular_ratio = 0.7;
  detail.back().second.regular_task.io_dominance_ratio = 0.5;
  detail.back().second.regular_task.long_io_ratio = 0.5;
  detail.back().second.long_task.io_dominance_ratio = 0.5;
  detail.back().second.long_task.long_io_ratio = 0.5;
  task_trait.detail = detail;
  return task_trait;
}

int main(int argc, const char *argv[]) {
  INVARIANT(argc == 3);

  std::fstream f(argv[1]);
  INVARIANT(f.is_open());
  json j;
  f >> j;
  trace_config = j;

  generate(argv[2]);
  return 0;
}

void generate(const std::string &prefix) {
  TaskTrait task_trait;
  TaskSerie serie;

  std::random_device rd;
  std::mt19937 gen(rd());

  // subtask 1
  task_trait = TaskTrait::short_default();
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-1.json", serie);

  // subtask 2
  task_trait = TaskTrait::regular_default();
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-2.json", serie);

  // subtask 3
  task_trait = TaskTrait::long_default();
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-3.json", serie);

  // subtask 4
  task_trait = TaskTrait::mixed_default();
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-4.json", serie);

  // subtask 5
  task_trait = TaskTrait::mixed_default();
  std::reference_wrapper<TaskTrait::MixedDetail> mixed_detail =
      std::get<TaskTrait::MixedDetail>(task_trait.detail);
  mixed_detail.get().short_ratio = 0.6;
  mixed_detail.get().regular_ratio = 0.4;
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-5.json", serie);

  // subtask 6
  task_trait = TaskTrait::mixed_default();
  mixed_detail = std::get<TaskTrait::MixedDetail>(task_trait.detail);
  mixed_detail.get().short_ratio = 0.8;
  mixed_detail.get().regular_ratio = 0;
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-6.json", serie);

  // subtask 7
  task_trait = TaskTrait::shifting_default();
  std::reference_wrapper<TaskTrait::ShiftingDetail> shifting_detail =
      std::get<TaskTrait::ShiftingDetail>(task_trait.detail);
  shifting_detail.get()[0].second.short_ratio = 0.8;
  shifting_detail.get()[0].second.regular_ratio = 0.1;
  shifting_detail.get()[1].second.short_ratio = 0.2;
  shifting_detail.get()[1].second.regular_ratio = 0.7;
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-7.json", serie);

  // subtask 8
  task_trait = TaskTrait::shifting_default();
  shifting_detail = std::get<TaskTrait::ShiftingDetail>(task_trait.detail);
  shifting_detail.get().emplace_back();
  shifting_detail.get()[0].first = 0;
  shifting_detail.get()[0].second.short_ratio = 0.9;
  shifting_detail.get()[0].second.regular_ratio = 0.05;
  shifting_detail.get()[1].first = 0.33;
  shifting_detail.get()[1].second.short_ratio = 0.33;
  shifting_detail.get()[1].second.regular_ratio = 0.33;
  shifting_detail.get()[2].first = 0.67;
  shifting_detail.get()[2].second.short_ratio = 0.05;
  shifting_detail.get()[2].second.regular_ratio = 0.9;
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-8.json", serie);

  // subtask 9
  task_trait = TaskTrait::mixed_default();
  serie = generate_serie(task_trait, BudgetTrait::kTight,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-9.json", serie);

  // subtask 10
  task_trait = TaskTrait::mixed_default();
  serie = generate_serie(task_trait, BudgetTrait::kTight,
                         PriorityTrait::kRandom, ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-10.json", serie);

  // subtask 11
  task_trait = TaskTrait::mixed_default();
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kTightBudgetProne,
                         ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-11.json", serie);

  // subtask 12
  task_trait = TaskTrait::mixed_default();
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kTightBudgetProne,
                         ArrivalTrait::kPoisson, gen);
  file_helper(prefix + "-12.json", serie);

  // subtask 13
  task_trait = TaskTrait::mixed_default();
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kBurst, gen);
  file_helper(prefix + "-13.json", serie);

  // subtask 14
  task_trait = TaskTrait::mixed_default();
  serie = generate_serie(task_trait, BudgetTrait::kLoose,
                         PriorityTrait::kRandom, ArrivalTrait::kBurst, gen);
  file_helper(prefix + "-14.json", serie);

  // subtask 15
  task_trait = TaskTrait::shifting_default();
  serie = generate_serie(task_trait, BudgetTrait::kTight,
                         PriorityTrait::kTightBudgetProne, ArrivalTrait::kBurst,
                         gen);
  file_helper(prefix + "-15.json", serie);

  // subtask 16
  task_trait = TaskTrait::shifting_default();
  serie = generate_serie(task_trait, BudgetTrait::kTight,
                         PriorityTrait::kTightBudgetProne, ArrivalTrait::kBurst,
                         gen);
  file_helper(prefix + "-16.json", serie);
}

TaskSerie generate_serie(const TaskTrait &task_trait,
                         const BudgetTrait &budget_trait,
                         const PriorityTrait &priority_trait,
                         const ArrivalTrait &arrival_trait, std::mt19937 &gen) {
  int duration = trace_config.duration;
  double provision = trace_config.provision;

  int task_drtn_avg = expected_task_drtn(task_trait);
  int task_n = duration * provision / task_drtn_avg;
  double task_per_tick = 1.0 * task_n / duration;

  COUT_VAR(task_drtn_avg);
  COUT_VAR(task_n);
  COUT_VAR(task_per_tick);

  // simulate burst with normal distribution
  double total_normal_pdf = 0;
  double std_deviation = 0.1;
  for (int time = 0; time < duration; time++) {
    total_normal_pdf += normal_pdf(1.0 * time / duration, 0.5, std_deviation);
  }

  double accumulated_task_frac = 1;
  TaskSerie serie;

  for (int time = 0; time < duration; time++) {
    if (arrival_trait == ArrivalTrait::kBurst) {
      double expected = normal_pdf(1.0 * time / duration, 0.5, std_deviation) /
                        total_normal_pdf * task_n;
      double variance = 1; /* [0.5 * expected, 1.5 * expected] */
      accumulated_task_frac += fluctuate(expected, variance, gen);
    } else {
      accumulated_task_frac += rand(gen) * 2 * task_per_tick;
    }

    if (accumulated_task_frac >= 1) {
      TaskComplete task =
          generate_task(task_trait, budget_trait, priority_trait, time,
                        1.0 * time / duration, gen);
      serie.push_back(task);
      accumulated_task_frac -= 1;
    }
  }

  INVARIANT(serie.front().arrivalTime == 0);

  return serie;
}

int expected_task_drtn(const TaskTrait &task_trait) {
  int short_duration = (trace_config.short_task.duration_min +
                        trace_config.short_task.duration_max) /
                       2;
  int regular_duration = (trace_config.regular_task.duration_min +
                          trace_config.regular_task.duration_max) /
                         2;
  int long_duration = (trace_config.long_task.duration_min +
                       trace_config.long_task.duration_max) /
                      2;

  if (task_trait.type == TaskTrait::TaskType::kShort) {
    return short_duration;
  } else if (task_trait.type == TaskTrait::TaskType::kRegular) {
    return regular_duration;
  } else if (task_trait.type == TaskTrait::TaskType::kLong) {
    return long_duration;
  } else if (task_trait.type == TaskTrait::TaskType::kMixed) {
    const TaskTrait::MixedDetail &detail =
        std::get<TaskTrait::MixedDetail>(task_trait.detail);
    return short_duration * detail.short_ratio +
           regular_duration * detail.regular_ratio +
           long_duration * (1 - detail.short_ratio - detail.regular_ratio);
  } else {
    const TaskTrait::ShiftingDetail &shifting_detail =
        std::get<TaskTrait::ShiftingDetail>(task_trait.detail);
    double avg_drtn = 0;
    for (size_t i = 0; i < shifting_detail.size(); i++) {
      double weight;
      if (i == shifting_detail.size() - 1) {
        weight = 1 - shifting_detail[i].first;
      } else {
        weight = shifting_detail[i + 1].first - shifting_detail[i].first;
      }

      const TaskTrait::MixedDetail &detail = shifting_detail[i].second;
      avg_drtn +=
          weight *
          (short_duration * detail.short_ratio +
           regular_duration * detail.regular_ratio +
           long_duration * (1 - detail.short_ratio - detail.regular_ratio));
    }
    return avg_drtn;
  }
}

TaskComplete generate_task(const TaskTrait &task_trait,
                           const BudgetTrait &budget_trait,
                           const PriorityTrait &priority_trait, const int time,
                           const double progress, std::mt19937 &gen) {
  TaskTrait::TaskType task_type;
  std::optional<std::reference_wrapper<const TaskTrait::NonShortDetail>> detail;

  int task_duration =
      rand_task_drtn(task_trait, progress, gen, task_type, detail);

  std::vector<TaskComplete::Slice> slices =
      generate_slices(task_duration, task_type, detail, gen);

  /* budget */
  int task_budget;
  double variance = 0.5; /* in [0.75 * expected, 1.25 * expected] */

  int budget_tight;
  int budget_loose;
  if (task_type == TaskTrait::TaskType::kShort) {
    budget_tight = fluctuate(
        task_duration * trace_config.short_task.budget_tight, variance, gen);
    budget_loose = fluctuate(
        task_duration * trace_config.short_task.budget_loose, variance, gen);
  } else if (task_type == TaskTrait::TaskType::kRegular) {
    budget_tight = fluctuate(
        task_duration * trace_config.regular_task.budget_tight, variance, gen);
    budget_loose = fluctuate(
        task_duration * trace_config.regular_task.budget_loose, variance, gen);
  } else if (task_type == TaskTrait::TaskType::kLong) {
    budget_tight = fluctuate(
        task_duration * trace_config.long_task.budget_tight, variance, gen);
    budget_loose = fluctuate(
        task_duration * trace_config.long_task.budget_loose, variance, gen);
  } else {
    INVARIANT(0);
  }

  if (budget_trait == BudgetTrait::kTight) {
    task_budget = budget_tight;
  } else if (budget_trait == BudgetTrait::kLoose) {
    task_budget = budget_loose;
  } else {
    task_budget = coin(gen) ? budget_tight : budget_loose;
  }

  int task_actual_duration = 0;
  for (TaskComplete::Slice &slice : slices) {
    task_actual_duration += slice.second;
  }

  if (task_budget < task_actual_duration) {
    task_budget = task_actual_duration;
  }

  /* priority */
  TaskComplete::Priority prio;
  if (priority_trait == PriorityTrait::kRandom) {
    prio = coin(gen) ? TaskComplete::Priority::kHigh
                     : TaskComplete::Priority::kLow;
  } else {
    if (budget_trait == BudgetTrait::kTight) {
      prio = chance(trace_config.priority_proneness, gen)
                 ? TaskComplete::Priority::kHigh
                 : TaskComplete::Priority::kLow;
    } else {
      prio = coin(gen) ? TaskComplete::Priority::kHigh
                       : TaskComplete::Priority::kLow;
    }
  }

  TaskComplete task;
  task.arrivalTime = time;
  task.deadline = time + task_budget;
  task.priority = prio;
  task.slices = slices;

  return task;
}

int rand_task_drtn(
    const TaskTrait &task_trait, double progress, std::mt19937 &gen,
    TaskTrait::TaskType &task_type,
    std::optional<std::reference_wrapper<const TaskTrait::NonShortDetail>>
        &detail) {
  static std::uniform_int_distribution<int> dist_short(
      trace_config.short_task.duration_min,
      trace_config.short_task.duration_max);
  static std::uniform_int_distribution<int> dist_regualr(
      trace_config.regular_task.duration_min,
      trace_config.regular_task.duration_max);
  static std::uniform_int_distribution<int> dist_long(
      trace_config.long_task.duration_min, trace_config.long_task.duration_max);

  if (task_trait.type == TaskTrait::TaskType::kShort) {
    task_type = TaskTrait::TaskType::kShort;
    return dist_short(gen);
  } else if (task_trait.type == TaskTrait::TaskType::kRegular) {
    task_type = TaskTrait::TaskType::kRegular;
    detail = std::get<TaskTrait::NonShortDetail>(task_trait.detail);
    return dist_regualr(gen);
  } else if (task_trait.type == TaskTrait::TaskType::kLong) {
    task_type = TaskTrait::TaskType::kLong;
    detail = std::get<TaskTrait::NonShortDetail>(task_trait.detail);
    return dist_long(gen);
  }

  std::optional<std::reference_wrapper<const TaskTrait::MixedDetail>>
      mixed_detail;
  if (task_trait.type == TaskTrait::TaskType::kMixed) {
    mixed_detail = std::get<TaskTrait::MixedDetail>(task_trait.detail);
  } else if (task_trait.type == TaskTrait::TaskType::kShifting) {
    const TaskTrait::ShiftingDetail &shifting_detail =
        std::get<TaskTrait::ShiftingDetail>(task_trait.detail);
    INVARIANT(shifting_detail.size() > 0);
    for (size_t i = 0; i < shifting_detail.size(); i++) {
      if (shifting_detail[i].first >= progress) {
        mixed_detail = shifting_detail[i].second;
        break;
      }
    }
    if (!mixed_detail.has_value()) {
      mixed_detail = shifting_detail.back().second;
    }
  }

  double dice = rand(gen);
  if (dice < mixed_detail.value().get().short_ratio) {
    task_type = TaskTrait::TaskType::kShort;
    return dist_short(gen);
  } else if (dice < mixed_detail.value().get().short_ratio +
                        mixed_detail.value().get().regular_ratio) {
    task_type = TaskTrait::TaskType::kRegular;
    detail = mixed_detail.value().get().regular_task;
    return dist_regualr(gen);
  } else {
    task_type = TaskTrait::TaskType::kLong;
    detail = mixed_detail.value().get().long_task;
    return dist_long(gen);
  }
}

std::vector<TaskComplete::Slice> generate_slices(
    const int task_duration, const TaskTrait::TaskType &task_type,
    const std::optional<std::reference_wrapper<const TaskTrait::NonShortDetail>>
        &detail,
    std::mt19937 &gen) {
  int task_total_io;
  std::vector<TaskComplete::Slice> io_slices =
      generate_io_slices(task_duration, task_type, detail, gen, task_total_io);

  int cpu_slice_n = io_slices.size() + 1;
  int cpu_slice_duration_avg = (task_duration - task_total_io) / cpu_slice_n;

  std::vector<TaskComplete::Slice> all_slices;
  std::vector<TaskComplete::Slice>::iterator io_slices_it = io_slices.begin();

  for (int cpu_slice_i = 0; cpu_slice_i < cpu_slice_n; cpu_slice_i++) {
    int cpu_slice_duration = (coin(gen) * 2) * cpu_slice_duration_avg;
    if (cpu_slice_duration <= 0) {
      cpu_slice_duration = 1;
    }

    all_slices.push_back(TaskComplete::Slice(TaskComplete::ComputeType::kCpu,
                                             cpu_slice_duration));
    if (io_slices_it != io_slices.end()) {
      all_slices.push_back(*io_slices_it);
      io_slices_it++;
    }
  }

  // verify compute types are correct
  TaskComplete::ComputeType last_compute_type = TaskComplete::ComputeType::kCpu;
  for (auto it = all_slices.begin() + 1; it != all_slices.end(); it++) {
    INVARIANT(last_compute_type != it->first);
    last_compute_type = it->first;
  }
  INVARIANT(last_compute_type == TaskComplete::ComputeType::kCpu);

  return all_slices;
}

std::vector<TaskComplete::Slice> generate_io_slices(
    const int task_duration, const TaskTrait::TaskType &task_type,
    const std::optional<std::reference_wrapper<const TaskTrait::NonShortDetail>>
        &detail,
    std::mt19937 &gen, int &task_total_io) {
  double variance = 0.5; /* [0.75 * expected, 1.25 * expected] */
  if (task_type == TaskTrait::TaskType::kShort) {
    int expected = task_duration * trace_config.short_task.io_total_long;
    task_total_io = coin(gen) ? 0 : fluctuate(expected, variance, gen);
  } else if (task_type == TaskTrait::TaskType::kRegular) {
    if (chance(detail.value().get().io_dominance_ratio, gen)) {
      int expected = task_duration * trace_config.regular_task.io_total_long;
      task_total_io = fluctuate(expected, variance, gen);
    } else {
      int expected = task_duration * trace_config.regular_task.io_total_short;
      task_total_io = fluctuate(expected, variance, gen);
    }
  } else if (task_type == TaskTrait::TaskType::kLong) {
    if (chance(detail.value().get().io_dominance_ratio, gen)) {
      int expected = task_duration * trace_config.long_task.io_total_long;
      task_total_io = fluctuate(expected, variance, gen);
    } else {
      int expected = task_duration * trace_config.long_task.io_total_short;
      task_total_io = fluctuate(expected, variance, gen);
    }
  } else {
    INVARIANT(0);
  }

  std::vector<TaskComplete::Slice> io_slices;
  int io_left = task_total_io;

  while (io_left > 0) {
    int io_slice_duration;
    if (task_type == TaskTrait::TaskType::kShort) {
      io_slice_duration = task_total_io;
    } else if (task_type == TaskTrait::TaskType::kRegular) {
      if (chance(detail.value().get().long_io_ratio, gen)) {
        int expected = task_duration * trace_config.regular_task.io_slice_long;
        io_slice_duration = fluctuate(expected, variance, gen);
      } else {
        int expected = task_duration * trace_config.regular_task.io_slice_short;
        io_slice_duration = fluctuate(expected, variance, gen);
      }
    } else if (task_type == TaskTrait::TaskType::kLong) {
      if (chance(detail.value().get().long_io_ratio, gen)) {
        int expected = task_duration * trace_config.long_task.io_slice_long;
        io_slice_duration = fluctuate(expected, variance, gen);
      } else {
        int expected = task_duration * trace_config.long_task.io_slice_short;
        io_slice_duration = fluctuate(expected, variance, gen);
      }
    } else {
      INVARIANT(0);
    }

    if (io_slice_duration > io_left) {
      io_slice_duration = io_left;
    }
    if (io_slice_duration <= 0) {
      io_slice_duration = 1;
    }

    io_slices.push_back(
        TaskComplete::Slice(TaskComplete::ComputeType::kIo, io_slice_duration));
    io_left -= io_slice_duration;
  }

  return io_slices;
}

double rand(std::mt19937 &gen) {
  static std::uniform_real_distribution<double> dist(0, 1);
  return dist(gen);
}

bool chance(const double change, std::mt19937 &gen) {
  return rand(gen) < change;
}

bool coin(std::mt19937 &gen) { return chance(0.5, gen); }

int fluctuate(int expected, double variance, std::mt19937 &gen) {
  return ((rand(gen) - 0.5) * variance + 1) * expected;
}

double fluctuate(double expected, double variance, std::mt19937 &gen) {
  return ((rand(gen) - 0.5) * variance + 1) * expected;
}

double normal_pdf(double x, double m, double s) {
  static const double inv_sqrt_2pi = 0.3989422804014327;
  double a = (x - m) / s;

  return inv_sqrt_2pi / s * std::exp(-0.5f * a * a);
}

void file_helper(const std::string &file_name, const TaskSerie &serie) {
  std::fstream out(
      file_name, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  INVARIANT(out.is_open());
  COUT_VAR(file_name);
  out << json(serie) << std::endl;
}

void from_json(const json &j, TraceConfig &trace_config) {
  j.at("duration").get_to(trace_config.duration);
  j.at("provision").get_to(trace_config.provision);
  j.at("priority_proneness").get_to(trace_config.priority_proneness);
  j.at("short_task").get_to(trace_config.short_task);
  j.at("regular_task").get_to(trace_config.regular_task);
  j.at("long_task").get_to(trace_config.long_task);
}

void from_json(const json &j, TraceConfig::TaskConfig &task_config) {
  j.at("duration_min").get_to(task_config.duration_min);
  j.at("duration_max").get_to(task_config.duration_max);
  j.at("budget_tight").get_to(task_config.budget_tight);
  j.at("budget_loose").get_to(task_config.budget_loose);
  j.at("io_total_long").get_to(task_config.io_total_long);
  j.at("io_total_short").get_to(task_config.io_total_short);
  j.at("io_slice_long").get_to(task_config.io_slice_long);
  j.at("io_slice_short").get_to(task_config.io_slice_short);
}
