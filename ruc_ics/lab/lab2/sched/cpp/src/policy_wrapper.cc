#include <iostream>

#include <nlohmann/json.hpp>

#include "policy_wrapper.h"

using json = nlohmann::json;

#if defined(JAVA_SOLUTION)

#include <jni.h>

static JavaVM *javaVM;
static JNIEnv *env;
static jclass cls2;
static jmethodID ctor;
static jobject myo;
static jmethodID policy_func;
#endif

Action ask_policy(const std::vector<EventExternal>& events, int currentCpuTask,
                  int currentIoTask) {
#if defined(JAVA_SOLUTION)
  if (javaVM == nullptr)
    create_jvm();
  return java_policy(json(events).dump(), currentCpuTask, currentIoTask);

#elif defined(CPP_SOLUTION)
  return cpp_policy(events, currentCpuTask, currentIoTask);
  
#else
  std::cout << json(events) << std::endl;
  std::cout << json(currentCpuTask) << std::endl;
  std::cout << json(currentIoTask) << std::endl;

  json in;
  std::cin >> in;
  return in;
#endif
}

#if defined(CPP_SOLUTION)
Action cpp_policy(const std::vector<EventExternal>& events, int currentCpuTask,
                  int currentIoTask) {
  std::vector<Event> events_new;
  for (const EventExternal& event : events) {
    Event event_new;
    event_new.time = event.time;
    if (event.type == EventBase::Type::kTimer) {
      event_new.type = Event::Type::kTimer;
    } else if (event.type == EventBase::Type::kTaskArrival) {
      event_new.type = Event::Type::kTaskArrival;
    } else if (event.type == EventBase::Type::kTaskFinish) {
      event_new.type = Event::Type::kTaskFinish;
    } else if (event.type == EventBase::Type::kIoRequest) {
      event_new.type = Event::Type::kIoRequest;
    } else if (event.type == EventBase::Type::kIoEnd) {
      event_new.type = Event::Type::kIoEnd;
    } else {
      assert(0);
    }
    if (event.task.has_value()) {
      event_new.task.arrivalTime = event.task.value().arrivalTime;
      event_new.task.deadline = event.task.value().deadline;
      event_new.task.taskId = event.task.value().taskId;
      event_new.task.priority =
          event.task.value().priority == TaskBase::Priority::kHigh
              ? Event::Task::Priority::kHigh
              : Event::Task::Priority::kLow;
    } else {
      event_new.task.taskId = 0;
    }

    events_new.push_back(event_new);
  }

  return policy(events_new, currentCpuTask, currentIoTask);
}
#endif

#if defined(JAVA_SOLUTION)
Action java_policy(const std::string events, int currentCpuTask, int currentIoTask) {
  assert(javaVM != nullptr);

  jstring events_json = (env)->NewStringUTF(events.c_str());
  jint current_cpu = currentCpuTask;
  jint current_io = currentIoTask;

  jstring jaction = (jstring)env->CallObjectMethod(myo, policy_func, events_json, current_cpu, current_io);
  const char* action = env->GetStringUTFChars(jaction, NULL);
  // std::cerr << "Return is:" << action << std::endl;

  std::string str = action;
  json j = json::parse(str.begin(), str.end());

  env->ReleaseStringUTFChars(jaction, action);
  env->DeleteLocalRef(events_json);
  env->DeleteLocalRef(jaction);

  return j;
}

void create_jvm() {
  JavaVMOption jvmopt[1];
  jvmopt[0].optionString = (char *) "-Djava.class.path=./java/target:./java/lib/gson-2.8.5.jar";

  JavaVMInitArgs vmArgs;
  vmArgs.version = JNI_VERSION_10;
  vmArgs.nOptions = 1;
  vmArgs.options = jvmopt;
  vmArgs.ignoreUnrecognized = JNI_TRUE;

  // Create the JVM
  long flag = JNI_CreateJavaVM(&javaVM, (void**)&env, &vmArgs);
  if (flag == JNI_ERR) {
    std::cerr << "Error creating VM. Exiting..." << std::endl;
    return;
  }

  // which policy to use
  // cls2 = env->FindClass("RoundRobinPolicy");
  // cls2 = env->FindClass("FIFOPolicy");
  cls2 = env->FindClass("Policy");
  if(cls2 == nullptr) {
    std::cerr << "ERROR: class not found !" << std::endl;
    return;
	}

  ctor = env->GetMethodID(cls2, "<init>", "()V");  // FIND AN OBJECT CONSTRUCTOR
  if(ctor == nullptr) {
    std::cerr << "ERROR: constructor not found !" << std::endl;
    return;
  }

  myo = env->NewObject(cls2, ctor);
  policy_func = env->GetMethodID(cls2, "ask_policy", "(Ljava/lang/String;II)Ljava/lang/String;");
  if(policy_func == nullptr) {
    std::cerr << "No ask_policy method !!" << std::endl;
    return;
  }
}

void destroy_jvm() {
  if (javaVM == nullptr)
    return;
  env->DeleteLocalRef(myo);
  javaVM->DestroyJavaVM();
}
#endif