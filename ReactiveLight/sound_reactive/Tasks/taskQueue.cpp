#include "stdafx.h"
#include "taskQueue.h"

TaskQueue	g_TaskQueue;

TaskQueue::TaskQueue()
{
	mMsQueueChanged= 0;
}

void TaskQueue::add(ELEM_TYPE newElem)
{
	ELEM_TYPE pData = NULL;
	
	//LOGF4("Adding task ", (unsigned long)(void*)newElem, F(", "), newElem->getTaskname());
		
	for (int i = 0; i < this->getUsedElems(); ++i)
	{
		pData= getData()[i];  // Note: getData might change because of an interrupt adding elements
		if (pData == NULL) {
			getData()[i]= newElem;
			mMsQueueChanged= millis();
			newElem->onStarted();
			return;
		}
	}
	
	// append
	this->write(newElem);
	mMsQueueChanged= millis();
			
	newElem->onStarted();
}

void TaskQueue::executeReadyTasks()
{
	//LOGF("executeReadyTasks");
			
	ELEM_TYPE pData = NULL;
	
	for (int i = 0; i < this->getUsedElems(); ++i)
	{
		pData = getData()[i]; // Note: getData might change because of an interrupt adding elements
		if ((pData != NULL) && pData->isReadyForExecution()) {
			executeAndDeleteTask(pData);
		}
	}
}

void TaskQueue::executeAndDeleteTask(ELEM_TYPE pData)
{
	if (pData == NULL)
		return;
		
	for (int i = 0; i < this->getUsedElems(); ++i)
	{
		if (pData == getData()[i])  // Note: getData might change because of an interrupt adding elements
		{ 
			long deltaMs= (millis() - pData->getMillisToExec());
			if ( abs(deltaMs) > 30) {				
				LOGF6("executing task ", (unsigned long)(void*)pData, F(", <"), pData->getTaskname(), F("> UNPUNCTUAL: delta ms="), deltaMs); 
			}
			else {
				//LOGF4("executing task ", (unsigned long)(void*)pData, F(", "), pData->getTaskname()); 
			}
	
			pData->execute();
			
			if (pData->isFinished()) // Note: getData might change because of an interrupt adding elements
			{
				getData()[i]= NULL;
				delete pData;
				pData= NULL;
			}
			else
				pData->refreshInterval(); // recurring tasks might have another interval
				
			break;
		}
	}
}

TaskQueue::ELEM_TYPE TaskQueue::nextTaskToExecute() const
{
	int counter= 0;
	ELEM_TYPE pNext= NULL;
	
	for (int i = 0; i < this->getUsedElems(); ++i)
	{
		ELEM_TYPE pData= getData()[i];  // Note: getData might change because of an interrupt adding elements
		if ((pData != NULL) && ((pNext==NULL) || (pData->getMillisToExec() < pNext->getMillisToExec())) ) {
			pNext= pData;
		}
	}
	
	return pNext;
}

void TaskQueue::waitAndExecuteAllTasks()
{
	waitAndExecuteAllTasks(0xffffffff, NULL, NULL) ;
}

#define CHECK_QUEUE_INTERV_MS	50

static unsigned long s_msQueueChanged= 0;

// return true => continue busyDelay otherwise interrupt
/*static*/ bool TaskQueue::checkQueueForChanges(void* pUserData)
{
	const TaskQueue* pTaskQueue= (const TaskQueue*)pUserData;
	if (pTaskQueue->mMsQueueChanged > s_msQueueChanged)
	{
		return false; // interrupt waiting as queue has changed
	}

	return true; // continue	
}

void TaskQueue::waitAndExecuteAllTasks(unsigned long inbetweenDelayMs, CallbackT pCallback, void* pUserData)
{
	//LOGBEGEOF("waitAndExecuteAllTasks");
	executeReadyTasks();
	
	s_msQueueChanged= mMsQueueChanged;
	ELEM_TYPE pData = nextTaskToExecute();
	//LOGF2("nextTaskToExecute @", pData->getMillisToExec());
	
	while (pData != NULL)
	{
		long waitMs= (long)pData->getMillisToExec() - (long)millis();
		if (waitMs > 0) {
			waitMs= min(inbetweenDelayMs, waitMs);
			
			if (inbetweenDelayMs <= CHECK_QUEUE_INTERV_MS)
			{
				Utils::delayMs(waitMs); // SYNC
			}
			else
			{
				Utils::busyDelayMs(waitMs, CHECK_QUEUE_INTERV_MS, checkQueueForChanges, this); // SYNC
			}
							
			// any changes in the queue meanwhile?
			if (mMsQueueChanged > s_msQueueChanged)
			{
				s_msQueueChanged= mMsQueueChanged;
				pData = nextTaskToExecute();
			}
			
			if (pCallback != NULL) {
				pCallback(pUserData);
			}
		}
		
		if (pData->isReadyForExecution())
		{
			executeAndDeleteTask(pData);
			pData= nextTaskToExecute();		
		}
	}
	
}
