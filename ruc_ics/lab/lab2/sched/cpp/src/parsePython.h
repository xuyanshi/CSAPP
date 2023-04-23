#if !defined(PARSE_PYTHON_H)
#define PARSE_PYTHON_H

#include <iostream>
#include <string>
#include <vector>
#include "policy_wrapper.h"
#include "sim.h"

extern "C" {
#include "Python.h"
}

void __initial__();
Action policy(const std::vector<Event>& events, int currentCpuTask,
              int currentIoTask);
PyObject* parseEvent(const std::vector<Event>& events, int currentCpuTask,
                     int currentIoTask);
Action parseResult(PyObject* result)
#endif  // POLICY_WRAPPER_H