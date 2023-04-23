#include <cassert>
#include <fstream>
#include <iostream>
#include <random>

#include <nlohmann/json.hpp>

#include "task.h"
#include "trace_gen.h"

using json = nlohmann::json;

int main(int argc, const char *argv[]) {
  assert(argc == 3);
  config_name = argv[1];

  generate(argv[2]);
  return 0;
}

void generate(const std::string &prefix) {
  std::fstream out;

  // subtask 1, 2
  out.close();
  out.open(prefix + "-1.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kShort, ComputeTrait::kCpu,
                             ArrivalTrait::kPoisson, TimeLimitTrait::kLoose,
                             TimeLimitVarianceTrait::kSmall))
      << std::endl;

  out.close();
  out.open(prefix + "-2.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kShort, ComputeTrait::kCpu,
                             ArrivalTrait::kPoisson, TimeLimitTrait::kTight,
                             TimeLimitVarianceTrait::kSmall))
      << std::endl;

  // subtask 3, 4
  out.close();
  out.open(prefix + "-3.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kLong, ComputeTrait::kIo,
                             ArrivalTrait::kPoisson, TimeLimitTrait::kLoose,
                             TimeLimitVarianceTrait::kSmall))
      << std::endl;

  out.close();
  out.open(prefix + "-4.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kLong, ComputeTrait::kIo,
                             ArrivalTrait::kPoisson, TimeLimitTrait::kTight,
                             TimeLimitVarianceTrait::kSmall))
      << std::endl;

  // subtask 5, 6
  out.close();
  out.open(prefix + "-5.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kMixed, ComputeTrait::kMixed,
                             ArrivalTrait::kPoisson, TimeLimitTrait::kLoose,
                             TimeLimitVarianceTrait::kSmall))
      << std::endl;

  out.close();
  out.open(prefix + "-6.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kMixed, ComputeTrait::kMixed,
                             ArrivalTrait::kPoisson, TimeLimitTrait::kTight,
                             TimeLimitVarianceTrait::kSmall))
      << std::endl;

  // subtask 7, 8
  out.close();
  out.open(prefix + "-7.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kMixed, ComputeTrait::kMixed,
                             ArrivalTrait::kPoisson, TimeLimitTrait::kLoose,
                             TimeLimitVarianceTrait::kLarge))
      << std::endl;

  out.close();
  out.open(prefix + "-8.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kMixed, ComputeTrait::kMixed,
                             ArrivalTrait::kPoisson, TimeLimitTrait::kTight,
                             TimeLimitVarianceTrait::kLarge))
      << std::endl;

  // subtask 9, 10
  out.close();
  out.open(prefix + "-9.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kMixed, ComputeTrait::kMixed,
                             ArrivalTrait::kBurst, TimeLimitTrait::kLoose,
                             TimeLimitVarianceTrait::kLarge))
      << std::endl;

  out.close();
  out.open(prefix + "-10.json",
           std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  assert(out.is_open());
  out << json(generate_serie(IOTrait::kMixed, ComputeTrait::kMixed,
                             ArrivalTrait::kBurst, TimeLimitTrait::kTight,
                             TimeLimitVarianceTrait::kLarge))
      << std::endl;
}

TaskSerie generate_serie(IOTrait io_trait, ComputeTrait compute_trait,
                         ArrivalTrait arrival_trait,
                         TimeLimitTrait time_limit_trait,
                         TimeLimitVarianceTrait time_limit_variance_trait) {
  std::fstream f(config_name);
  assert(f.is_open());
  json j;
  f >> j;

  int duration = j["duration"];
  double provision = j["provision"];

  int task_drtn_min = j["minimal task duration"];
  int task_drtn_max = j["maximal task duration"];
  int task_drtn_avg = (task_drtn_min + task_drtn_max) / 2;

  double ta_amp_lo = j["turnarround amplification low"];
  double ta_amp_hi = j["turnarround amplification high"];
  double ta_amp_var_lo = j["turnarround amplification variance low"];
  double ta_amp_var_hi = j["turnarround amplification variance high"];

  double dom_min = j["minimal dominance"];
  double dom_max = j["maximal dominance"];

  int io_slice_drtn_lo = j["io slice duration low"];
  int io_slice_drtn_hi = j["io slice duration high"];

  int task_n = duration * provision / task_drtn_avg;
  double task_per_tick = 1.0 * task_n / duration;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::poisson_distribution<> poisson(task_per_tick);
  std::uniform_int_distribution<int> uniform_task_duration(task_drtn_min,
                                                           task_drtn_max);
  std::uniform_real_distribution<double> uniform_dominance(dom_min, dom_max);
  std::uniform_real_distribution<double> uniform_zero2one(0, 1);
  std::uniform_int_distribution<int> uniform_io_slice_duration_short(
      io_slice_drtn_lo * 0.5, io_slice_drtn_lo * 1.5);
  std::uniform_int_distribution<int> uniform_io_slice_duration_long(
      io_slice_drtn_hi * 0.5, io_slice_drtn_hi * 1.5);

  // simulate burst with normal distribution
  double total_normal_pdf = 0;
  double std_deviation = 1;
  for (int time = 0; time < duration; time++) {
    total_normal_pdf += normal_pdf(1.0 * time / duration, 0.5, std_deviation);
  }

  double accumulated_task_frac = 1;
  TaskSerie serie;

  for (int time = 0; time < duration; time++) {
    if (arrival_trait == ArrivalTrait::kBurst) {
      accumulated_task_frac +=
          normal_pdf(1.0 * time / duration, 0.5, std_deviation) /
          total_normal_pdf * task_n;
    } else {
      accumulated_task_frac += poisson(gen);
    }

    if (accumulated_task_frac >= 1) {
      int task_duration = uniform_task_duration(gen);
      int task_total_io;
      if (compute_trait == ComputeTrait::kCpu) {
        task_total_io = (1 - uniform_dominance(gen)) * task_duration;
      } else if (compute_trait == ComputeTrait::kIo) {
        task_total_io = uniform_dominance(gen) * task_duration;
      } else {
        task_total_io = uniform_zero2one(gen) > 0.5
                            ? uniform_dominance(gen) * task_duration
                            : (1 - uniform_dominance(gen)) * task_duration;
      }

      std::vector<TaskComplete::Slice> io_slices;
      int io_left = task_total_io;

      while (io_left > 0) {
        int io_slice_duration;
        if (io_trait == IOTrait::kShort) {
          io_slice_duration = uniform_io_slice_duration_short(gen);
        } else if (io_trait == IOTrait::kLong) {
          io_slice_duration = uniform_io_slice_duration_long(gen);
        } else {
          io_slice_duration = uniform_zero2one(gen) > 0.5
                                  ? uniform_io_slice_duration_short(gen)
                                  : uniform_io_slice_duration_long(gen);
        }

        if (io_slice_duration > io_left) {
          io_slice_duration = io_left;
        }
        if (io_slice_duration <= 0) {
          io_slice_duration = 1;
        }

        io_slices.push_back(TaskComplete::Slice(TaskComplete::ComputeType::kIo,
                                                io_slice_duration));
        io_left -= io_slice_duration;
      }

      int cpu_slice_n = io_slices.size() + 1;
      int cpu_slice_duration_avg =
          (task_duration - task_total_io) / cpu_slice_n;

      std::vector<TaskComplete::Slice> all_slices;
      std::vector<TaskComplete::Slice>::iterator io_slices_it =
          io_slices.begin();

      for (int cpu_slice_i = 0; cpu_slice_i < cpu_slice_n; cpu_slice_i++) {
        int cpu_slice_duration =
            (uniform_zero2one(gen) * 2) * cpu_slice_duration_avg;
        if (cpu_slice_duration <= 0) {
          cpu_slice_duration = 1;
        }

        all_slices.push_back(TaskComplete::Slice(
            TaskComplete::ComputeType::kCpu, cpu_slice_duration));
        if (io_slices_it != io_slices.end()) {
          all_slices.push_back(*io_slices_it);
          io_slices_it++;
        }
      }

      // verify compute types are correct
      TaskComplete::ComputeType last_compute_type =
          TaskComplete::ComputeType::kCpu;
      for (auto it = all_slices.begin() + 1; it != all_slices.end(); it++) {
        assert(last_compute_type != it->first);
        last_compute_type = it->first;
      }
      assert(last_compute_type == TaskComplete::ComputeType::kCpu);

      int task_budget;
      int budget_variance =
          time_limit_variance_trait == TimeLimitVarianceTrait::kSmall
              ? ta_amp_var_lo
              : ta_amp_var_hi;
      if (time_limit_trait == TimeLimitTrait::kTight) {
        task_budget = ((uniform_zero2one(gen) - 0.5) * budget_variance + 1) *
                      (ta_amp_lo * task_duration);
      } else if (time_limit_trait == TimeLimitTrait::kLoose) {
        task_budget = ((uniform_zero2one(gen) - 0.5) * budget_variance + 1) *
                      (ta_amp_hi * task_duration);
      } else {
        task_budget =
            uniform_zero2one(gen) > 0.5
                ? ((uniform_zero2one(gen) - 0.5) * budget_variance + 1) *
                      (ta_amp_lo * task_duration)
                : ((uniform_zero2one(gen) - 0.5) * budget_variance + 1) *
                      (ta_amp_hi * task_duration);
      }

      int task_actual_duration = 0;
      for (TaskComplete::Slice &slice : all_slices) {
        task_actual_duration += slice.second;
      }

      if (task_budget < task_actual_duration) {
        task_budget = task_actual_duration;
      }

      TaskComplete::Priority prio = uniform_zero2one(gen) > 0.5
                                        ? TaskComplete::Priority::kHigh
                                        : TaskComplete::Priority::kLow;

      accumulated_task_frac -= 1;
      TaskComplete task;
      task.arrivalTime = time;
      task.deadline = time + task_budget;
      task.priority = prio;
      task.slices = all_slices;
      serie.push_back(task);
    }
  }

  assert(serie.front().arrivalTime == 0);

  return serie;
}

double normal_pdf(double x, double m, double s) {
  static const double inv_sqrt_2pi = 0.3989422804014327;
  double a = (x - m) / s;

  return inv_sqrt_2pi / s * std::exp(-0.5f * a * a);
}
