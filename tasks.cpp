#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

// Перечислимый тип для статуса задачи
enum class TaskStatus {
    NEW,          // новая
    IN_PROGRESS,  // в разработке
    TESTING,      // на тестировании
    DONE          // завершена
};

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
public:
    // Получить статистику по статусам задач конкретного разработчика
    const TasksInfo& GetPersonTasksInfo(const string& person) const
    {
        return Persons.at(person);
    }

    // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
    void AddNewTask(const string& person)
    {
        Persons[person][TaskStatus::NEW]++;

    }

    // Обновить статусы по данному количеству задач конкретного разработчика,
    // подробности см. ниже
    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count)
    {
        TasksInfo CurrentTask = GetPersonTasksInfo(person);
        TasksInfo oldTask = GetPersonTasksInfo(person);
        TasksInfo UntouchedTask;
        TasksInfo UpdatedTask;
        if (Persons.count(person) == 0)
        {
            return tie(UpdatedTask, UntouchedTask);
        }
        while (task_count > 0)
        {
            if (CurrentTask[TaskStatus::NEW] > 0)
            {
                CurrentTask[TaskStatus::NEW]--;
                CurrentTask[TaskStatus::IN_PROGRESS]++;
                UpdatedTask[TaskStatus::IN_PROGRESS]++;
                task_count--;
            }
            else if (CurrentTask[TaskStatus::IN_PROGRESS] > 0)
            {
                CurrentTask[TaskStatus::IN_PROGRESS]--;
                CurrentTask[TaskStatus::TESTING]++;
                UpdatedTask[TaskStatus::TESTING]++;
                task_count--;
            }
            else if (CurrentTask[TaskStatus::TESTING] > 0)
            {
                CurrentTask[TaskStatus::TESTING]--;
                CurrentTask[TaskStatus::DONE]++;
                UpdatedTask[TaskStatus::DONE]++;
                task_count--;
            }
            else {
                for (int i = static_cast<int>(TaskStatus::NEW); i < static_cast<int>(TaskStatus::DONE); i++)
                {
                    TaskStatus t = static_cast<TaskStatus>(i);
                    UntouchedTask[t] = oldTask[t] - CurrentTask[t];
                }
                Persons[person] = CurrentTask;
                return tie(UpdatedTask, UntouchedTask);
            }
        }
        for (int i = static_cast<int>(TaskStatus::NEW); i != static_cast<int>(TaskStatus::DONE); i++)
        {
            TaskStatus t = static_cast<TaskStatus>(i);
            UntouchedTask[t] = (oldTask[t] - (oldTask[t] - CurrentTask[t])) - UpdatedTask[t];
        }
        Persons[person] = CurrentTask;
        return tie(UpdatedTask, UntouchedTask);

    }

private:
    map<string, TasksInfo> Persons;
};

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь.
// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
    cout << tasks_info[TaskStatus::NEW] << " new tasks" <<
        ", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" <<
        ", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested" <<
        ", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
    TeamTasks tasks;
    tasks.AddNewTask("Ilia");
    for (int i = 0; i < 3; ++i) {
        tasks.AddNewTask("Ivan");
    }
    cout << "Ilia's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
    cout << "Ivan's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));
  
    TasksInfo updated_tasks, untouched_tasks;
  
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);
  
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);
}
