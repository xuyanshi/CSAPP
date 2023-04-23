#if !defined(POLICY_WRAPPER_H)
#define POLICY_WRAPPER_H

#include "event.h"
#include "policy.h"

Action ask_policy(const std::vector<EventExternal>& events, int currentCpuTask,
                  int currentIoTask);
  
#if defined(CPP_SOLUTION)                
Action cpp_policy(const std::vector<EventExternal>& events, int currentCpuTask,
                  int currentIoTask);
#endif

#if defined(JAVA_SOLUTION)
Action java_policy(const std::string events, int currentCpuTask, int currentIoTask);

void create_jvm();
void destroy_jvm();
#endif

extern void to_json(json& j, const Action& action);
extern void from_json(const json& j, Action& action);

#endif  // POLICY_WRAPPER_H