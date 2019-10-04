#include "ModuleTaskManager.h"


void ModuleTaskManager::threadMain()
{
	while (true)
	{
		Task* temp = nullptr;
		{ 
			std::unique_lock<std::mutex> lock(mtx); 
			while (scheduledTasks.empty()) { //COMPROVAR FLAG
				event.wait(lock); 
			}
			temp = scheduledTasks.front();
			scheduledTasks.pop();
		}
		temp->execute();
		std::unique_lock<std::mutex> lock(mtx);
		finishedTasks.push(temp);

		// TODO 3:
		// - Wait for new tasks to arrive
		// - Retrieve a task from scheduledTasks
		// - Execute it
		// - Insert it into finishedTasks
	}
}

bool ModuleTaskManager::init()
{
	// TODO 1: Create threads (they have to execute threadMain
	for (int i = 0; i < MAX_THREADS ; ++i)
	{
		threads[i] = std::thread(&ModuleTaskManager::threadMain, this);
	}

	return true;
}

bool ModuleTaskManager::update()
{
	// TODO 4: Dispatch all finished tasks to their owner module (use Module::onTaskFinished() callback)
	//std::unique_lock<std::mutex> lock(mtx);
	while (!finishedTasks.empty())
	{
		Task* task = finishedTasks.front();
		task->owner->onTaskFinished(task);
		finishedTasks.pop();
	}

	return true;
}

bool ModuleTaskManager::cleanUp()
{
	// TODO 5: Notify all threads to finish and join them
	// TODO protect this
	exitFlag = true;
	event.notify_all();

	for (int i = 0; i < MAX_THREADS; ++i)
	{
		threads[i].join();
	}

	return true;
}

void ModuleTaskManager::scheduleTask(Task *task, Module *owner)
{
	task->owner = owner;

	// TODO 2: Insert the task into scheduledTasks so it is executed by some
	std::unique_lock<std::mutex> lock(mtx);
	scheduledTasks.push(task);
	event.notify_one();
}
