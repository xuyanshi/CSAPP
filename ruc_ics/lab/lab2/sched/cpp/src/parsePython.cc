#include <vector>
#include <string>
#include <iostream>
#include "policy_wrapper.h"
#include "sim.h"

extern "C" {
  #include "Python.h"
}


const std::string EnumStringType[] = {"Timer", "TaskArrival", "TaskFinish",
                                      "IoRequest", "IoEnd"};
const std::string EnumStringPriority[] = {"high", "low"};

bool __init__flag__ = false;
PyObject* __sourceFunc__;

PyObject* parseEvent(const std::vector<Event>& events,
                int currentCpuTask, int currentIoTask) {
  
  int len=events.size();
  PyObject* target = PyTuple_New(3);
  PyObject* eventList=PyList_New(len);
  for(int i=0;i<len;i++){
    std::string vType = EnumStringType[(int)events[i].type];
    if((int)events[i].type){
      std::string vPriority = EnumStringPriority[(int)events[i].task.priority];
      PyList_SetItem(
        eventList,
        i, 
        Py_BuildValue(
          "{"
            "s:{"
              "s:i,"
              "s:i,"
              "s:s,"
              "s:i"
            "},"
            "s:i,"
            "s:s"
          "}",
          "task",
          "arrivalTime",events[i].task.arrivalTime,
          "deadline",events[i].task.deadline,
          "priority",vPriority.c_str(),
          "taskId",events[i].task.taskId,
          "time",events[i].time,
          "type",vType.c_str()
        )
      );
    }
    else 
      PyList_SetItem(
        eventList,
        i, 
        Py_BuildValue(
          "{"
            "s:{},"
            "s:i,"
            "s:s"
          "}",
          "task",
          "time",events[i].time,
          "type",vType.c_str()
        )
      );
  }
  PyTuple_SetItem(target,0,eventList);
  PyTuple_SetItem(target,1,Py_BuildValue("i",currentCpuTask));
  PyTuple_SetItem(target,2,Py_BuildValue("i",currentIoTask));
  return target;
}

Action parseResult(PyObject* result){
  Action action{};
  if(!result)
    return action;
  action.cpuTask=PyLong_AsLong(PyDict_GetItemString(result,"cpuTask"));
  action.ioTask=PyLong_AsLong(PyDict_GetItemString(result,"ioTask"));
  return action;
}

void __initial__(){
  Py_Initialize();
  //判断初始化是否成功
  if (!Py_IsInitialized()) {
    printf("Python init failed!\n");
    exit(-1);
  }
  // PyRun_SimpleString 为宏，执行一段python代码
  //导入当前路径
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append('./')");
  // 加载source.py
  PyObject *pModule = PyImport_ImportModule("source");
  if (!pModule) {
    std::cerr<<"Load source.py failed!\n";
    exit(-1);
  }
  // 加载__dict__表
  PyObject *pDict = PyModule_GetDict(pModule);
  if (!pDict) {
    std::cerr<<"Can't find dict in source!";
    exit(-1);
  }
  // 加载函数
  __sourceFunc__ = PyDict_GetItemString(pDict, "policy");

  if (!__sourceFunc__ || !PyCallable_Check(__sourceFunc__)) {
    std::cerr<<"Can't find function!";
    exit(-1);
  }
  __init__flag__=true;
  return;
}

//main entry
Action policy(const std::vector<Event>& events, int currentCpuTask,
              int currentIoTask) {
  if(!__init__flag__) __initial__();
  return parseResult(
    PyObject_CallObject(
      __sourceFunc__,
      parseEvent(events,currentCpuTask,currentIoTask)
    )
  
  );
  
}