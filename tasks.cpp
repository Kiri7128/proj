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

    
    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count)
    {
    TasksInfo updated_tasks, untouched_tasks;
    
    if (Persons.count(person) == 0){return tie(updated_tasks,untouched_tasks);}
    
    TasksInfo current_tasks = GetPersonTasksInfo(person);
    TasksInfo Odlcurrent_tasks = current_tasks;
    int count = current_tasks[TaskStatus::NEW] + current_tasks[TaskStatus::IN_PROGRESS] + current_tasks[TaskStatus::TESTING] + current_tasks[TaskStatus::DONE];
    if (task_count == 0){
        current_tasks[TaskStatus::DONE] = 0;
        return tie(updated_tasks,current_tasks);}
    if (task_count > count)
    {
        
        updated_tasks[TaskStatus::IN_PROGRESS] = current_tasks[TaskStatus::NEW];
        updated_tasks[TaskStatus::TESTING] = current_tasks[TaskStatus::IN_PROGRESS];
        updated_tasks[TaskStatus::DONE] = current_tasks[TaskStatus::TESTING];
        current_tasks[TaskStatus::NEW] = 0;
        current_tasks[TaskStatus::IN_PROGRESS] = updated_tasks[TaskStatus::IN_PROGRESS];
        current_tasks[TaskStatus::TESTING]=updated_tasks[TaskStatus::TESTING];
        current_tasks[TaskStatus::DONE]+=updated_tasks[TaskStatus::DONE];
        
    }
    
    else if (task_count < count)
    {
        for (int i = 0;i < task_count;i++)
        {
            if (current_tasks[TaskStatus::NEW] > 0)
            {  
                current_tasks[TaskStatus::NEW]--;
                updated_tasks[TaskStatus::IN_PROGRESS]++;
                current_tasks[TaskStatus::IN_PROGRESS]++;
                count++;
            }
            else if (current_tasks[TaskStatus::IN_PROGRESS] > 0)
            {
                current_tasks[TaskStatus::IN_PROGRESS]--;
                updated_tasks[TaskStatus::TESTING]++;
                current_tasks[TaskStatus::TESTING]++;
                count++;
            }
            else if (current_tasks[TaskStatus::TESTING] > 0)
            {
                current_tasks[TaskStatus::TESTING]--;
                current_tasks[TaskStatus::DONE]++;
                updated_tasks[TaskStatus::TESTING]++;
                count++;
            }
        else{break;}
        untouched_tasks[TaskStatus::NEW] = Odlcurrent_tasks[TaskStatus::NEW] - updated_tasks[TaskStatus::IN_PROGRESS];
        untouched_tasks[TaskStatus::IN_PROGRESS] = current_tasks[TaskStatus::IN_PROGRESS] - updated_tasks[TaskStatus::IN_PROGRESS];
        untouched_tasks[TaskStatus::TESTING] = current_tasks[TaskStatus::TESTING] - updated_tasks[TaskStatus::TESTING];
    }
Persons[person] = current_tasks;
untouched_tasks[TaskStatus::DONE] = 0;
 }
        
   
    
   
    
  
      Persons[person] = current_tasks;  

    return tie(updated_tasks, untouched_tasks);
}

 

private:
    map<string, TasksInfo> Persons;

};



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
