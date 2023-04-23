#if !defined(TRACE_GEN_H)
#define TRACE_GEN_H

#include <vector>

#include "task.h"

/* globals */
const char *config_name;

/* types */
enum class IOTrait { kShort, kLong, kMixed };
enum class ComputeTrait { kCpu, kIo, kMixed };
enum class ArrivalTrait { kPoisson, kBurst };
enum class TimeLimitTrait { kTight, kLoose, kMixed };
enum class TimeLimitVarianceTrait { kSmall, kLarge };

/* prototypes */
void generate(const std::string &prefix);
TaskSerie generate_serie(IOTrait io_trait, ComputeTrait compute_type,
                         ArrivalTrait arrival_trait,
                         TimeLimitTrait time_limit_trait,
                         TimeLimitVarianceTrait time_limit_variance_trait);
double normal_pdf(double x, double m, double s);

#endif  // TRACE_GEN_H
