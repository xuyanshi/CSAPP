#if !defined(TRACE_GEN_H)
#define TRACE_GEN_H

#include <variant>
#include <vector>

#include "task.h"

struct TraceConfig {
  struct TaskConfig {
    int duration_min, duration_max;
    double budget_tight, budget_loose;
    double io_total_long, io_total_short;
    double io_slice_long, io_slice_short;
  };

  int duration;
  double provision;
  double priority_proneness;
  TaskConfig short_task, regular_task, long_task;
};

/* globals */
TraceConfig trace_config;

/* types */
class TaskTrait {
 public:
  enum class TaskType { kShort, kRegular, kLong, kMixed, kShifting };
  struct NonShortDetail {
    double long_io_ratio;
    double io_dominance_ratio;
  };
  struct MixedDetail {
    double short_ratio;
    double regular_ratio;
    NonShortDetail regular_task;
    NonShortDetail long_task;
  };
  typedef std::vector<std::pair<double, MixedDetail>> ShiftingDetail;

 public:
  static TaskTrait short_default();
  static TaskTrait regular_default();
  static TaskTrait long_default();
  static TaskTrait mixed_default();
  static TaskTrait shifting_default();

 public:
  TaskType type;
  std::variant<NonShortDetail, MixedDetail, ShiftingDetail> detail;
};
enum class BudgetTrait { kTight, kLoose, kMixed };
enum class PriorityTrait { kRandom, kTightBudgetProne };
enum class ArrivalTrait { kPoisson, kBurst };

/* prototypes */
void generate(const std::string &prefix);
TaskSerie generate_serie(const TaskTrait &task_trait,
                         const BudgetTrait &time_limit_trait,
                         const PriorityTrait &priority_trait,
                         const ArrivalTrait &arrival_trait, std::mt19937 &gen);
int expected_task_drtn(const TaskTrait &task_trait);
TaskComplete generate_task(const TaskTrait &task_trait,
                           const BudgetTrait &budget_trait,
                           const PriorityTrait &priority_trait, const int time,
                           const double progress, std::mt19937 &gen);
int rand_task_drtn(
    const TaskTrait &task_trait, double progress, std::mt19937 &gen,
    TaskTrait::TaskType &task_type,
    std::optional<std::reference_wrapper<const TaskTrait::NonShortDetail>>
        &detail);
std::vector<TaskComplete::Slice> generate_slices(
    const int task_duration, const TaskTrait::TaskType &task_type,
    const std::optional<std::reference_wrapper<const TaskTrait::NonShortDetail>>
        &detail,
    std::mt19937 &gen);
std::vector<TaskComplete::Slice> generate_io_slices(
    const int task_duration, const TaskTrait::TaskType &task_type,
    const std::optional<std::reference_wrapper<const TaskTrait::NonShortDetail>>
        &detail,
    std::mt19937 &gen, int &task_total_io);
;
double rand(std::mt19937 &gen);
bool chance(const double change, std::mt19937 &gen);
bool coin(std::mt19937 &gen);
int fluctuate(int expected, double variance, std::mt19937 &gen);
double fluctuate(double expected, double variance, std::mt19937 &gen);

double normal_pdf(double x, double m, double s);
void file_helper(const std::string &file_name, const TaskSerie &serie);

void from_json(const json &j, TraceConfig &trace_config);
void from_json(const json &j, TraceConfig::TaskConfig &task_config);

#endif  // TRACE_GEN_H
