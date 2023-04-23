#include <iostream>

#include "event.h"
#include "util.h"

bool operator<(const EventInternal &left, const EventInternal &right) {
  return left.time < right.time;  // only partial order
}

Timer::Timer(int timer) : timer(timer) { prev_time = 0; }

EventInternal Timer::peek() {
  EventInternal event;
  event.type = EventInternal::Type::kTimer;
  event.time = prev_time + timer;
  return event;
}

EventInternal Timer::next(int cur_time) {
  EventInternal event = peek();
  prev_time = cur_time;
  return event;
}

TaskGen::TaskGen(const TaskSerie &serie, std::vector<RuntimeTask> &tasks) {
  tasks.clear();
  for (const TaskComplete &task : serie) {
    tasks.push_back(task);
  }
  task_next = tasks.begin();
  task_end = tasks.end();
}

bool TaskGen::has_next() { return task_next != task_end; }

EventInternal TaskGen::peek() {
  EventInternal event;
  event.type = EventInternal::Type::kTaskArrival;
  event.time = task_next->arrivalTime;
  event.task = *task_next;
  return event;
}

EventInternal TaskGen::next() {
  EventInternal event = peek();
  task_next++;
  return event;
}

bool Cpu::has_next() { return task.has_value(); }

EventInternal Cpu::peek() {
  assert(task.has_value());

  EventInternal event;
  if (task.value().get().final_slice()) {
    event.type = EventInternal::Type::kTaskFinish;
  } else {
    event.type = EventInternal::Type::kIoRequest;
  }
  event.time = prev_time + task.value().get().slice_remaining();
  event.task = task;
  return event;
}

void Cpu::progress(int elapsed) {
  if (task.has_value()) {
    task.value().get().progress(elapsed);
    if (!task.value().get().cpu_next()) {
      task.reset();
    }
  } else {
    idle_duration += elapsed;
  }
  prev_time += elapsed;
}

void Cpu::switch_to(RuntimeTask &task) { this->task = task; }

void Cpu::set_idle() { this->task.reset(); }

int Cpu::cur_taskId() {
  if (task.has_value()) {
    return task.value().get().taskId;
  } else {
    return 0;
  }
}

bool Io::has_next() { return task.has_value(); }

EventInternal Io::peek() {
  assert(task.has_value());

  EventInternal event;
  event.type = EventInternal::Type::kIoEnd;
  event.time = prev_time + task.value().get().slice_remaining();
  event.task = task;
  return event;
}

void Io::progress(int elapsed) {
  if (task.has_value()) {
    task.value().get().progress(elapsed);
    if (task.value().get().cpu_next()) {
      task.reset();
    }

  } else {
    idle_duration += elapsed;
  }
  prev_time += elapsed;
}

void Io::switch_to(RuntimeTask &task) {
  if (!this->task.has_value()) {
    this->task = task;
  } else if (this->task.value().get().taskId != task.taskId) {
#if defined(SCHED_DEBUG)
    std::cerr << "IO is serving, cannot switch" << std::endl;
#endif
  }
}

int Io::cur_taskId() {
  if (task.has_value()) {
    return task.value().get().taskId;
  } else {
    return 0;
  }
}

void to_json(json &j, const EventInternal &event) {
  j = json{{"type", event.type}, {"time", event.time}};
  if (event.task.has_value()) {
    j["task"] = event.task.value();
  }
}

void to_json(json &j, const EventExternal &event) {
  j = json{{"type", event.type}, {"time", event.time}};
  if (event.task.has_value()) {
    j["task"] = event.task.value();
  }
}

void to_json(json &j, const EventBase::Type &event_type) {
  if (event_type == EventBase::Type::kTimer) {
    j = json("Timer");
  } else if (event_type == EventBase::Type::kTaskArrival) {
    j = json("TaskArrival");
  } else if (event_type == EventBase::Type::kTaskFinish) {
    j = json("TaskFinish");
  } else if (event_type == EventBase::Type::kIoRequest) {
    j = json("IoRequest");
  } else if (event_type == EventBase::Type::kIoEnd) {
    j = json("IoEnd");
  } else {
    assert(0);
  }
}
