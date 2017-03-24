
#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include "DynamicPtrBuffer.h"

#define DEFINE_TASK(classname) \
	virtual const char* getTaskname() const { \
		return #classname; \
	}

class TaskData
{
	friend class TaskQueue;
protected:
	// timestamp when to execute job
	unsigned long	mMillisToExec, mIntervalMs;

protected:
	DEFINE_TASK(Taskdata);
	
	TaskData(unsigned long intervalMs) {
		setInterval(intervalMs);
	}
	
	void init(unsigned long intervalMs) {
		mMillisToExec= millis() + intervalMs;
	}
	
	void setInterval(unsigned long intervalMs) {
		mIntervalMs= intervalMs;
	}
	
public:
	virtual ~TaskData() {
		//LOGF2("~TaskData:", (unsigned long)(void*)this); // TODO
				
	}
	
	unsigned long getMillisToExec() const {
		return mMillisToExec;
	}
	
	bool isReadyForExecution() const {
		return (millis() >= mMillisToExec);
	}
	
	virtual void onStarted() {
		init(mIntervalMs);
	}
	
	virtual bool isFinished() const {
		return true;
	}
	
	virtual void refreshInterval() {
	}
	
	virtual void execute()= 0;
};

class RecurringTask : public TaskData
{
protected:
	unsigned long	mEofLifeTimsMs;
	
protected:
	DEFINE_TASK(RecurringTask);
	
	// intervalMs: this task runs every <intervalMs ms
	// liftimeMs: after this amount of time the task is deleted
	RecurringTask(unsigned long intervalMs,  unsigned long liftimeMs) : TaskData(intervalMs) {
		mEofLifeTimsMs= millis() + liftimeMs;
	}
	
	virtual ~RecurringTask() {

	}
	
public:
	virtual bool isFinished() const {
		return (millis() >= mEofLifeTimsMs);
	}
	
	virtual void refreshInterval()
	{
		// next interval
		init(mIntervalMs);
	}	
	
};

// a task that does nothing and just forces to wait for a certain time
class MinWaitTimeTask : public TaskData
{
private:
	unsigned long	mOffsetMs;
	
public:
	DEFINE_TASK(MinWaitTimeTask);

	MinWaitTimeTask(unsigned long offsetMs) : TaskData(offsetMs) {
		mOffsetMs= offsetMs;
	}
	
	virtual void execute()
	{
		// empty by intention	
		//LOGF2("Reached min wait time: ms=", mOffsetMs)
	}

};

class SerialLogTask : public TaskData
{
	private:
	String	str;
	
	public:
	DEFINE_TASK(SerialLogTask);
	
	SerialLogTask(const String& sz) : TaskData(10) {
		str= sz;
	}
	SerialLogTask(const char* sz) : TaskData(10) {
		str= sz;
	}
	
	virtual void execute()
	{
		LOG(str);
	}

};


class SetPwmTask : public TaskData
{
	public:
	DEFINE_TASK(SetPwmTask);
	
	SetPwmTask(unsigned long offsetMs, unsigned int inPin, int inValue) : TaskData(offsetMs) {
		mPin= inPin;
		mValue= inValue;
	}
	
	virtual void execute()
	{
		LOGF("SetPwmTask");
		analogWrite(mPin, mValue);
	}
	
	private:
	unsigned int mPin;
	int mValue;
};



class TaskQueue : public DynamicPtrBuffer<DynamicBufferData<TaskData*, 2>, TaskData*, 3> {
public: 
	TaskQueue();

	void add(ELEM_TYPE newElem);
	void executeReadyTasks();
	
	void waitAndExecuteAllTasks();
	
	typedef void(*CallbackT)(void* pUserData);
	void waitAndExecuteAllTasks(unsigned long inbetweenDelayMs, CallbackT pCallback, void* pUserData= NULL);

	bool isEmpty() const {
		return (NULL == nextTaskToExecute());
	}
	
private:
	ELEM_TYPE nextTaskToExecute() const;
	void executeAndDeleteTask(ELEM_TYPE pData);
	
	static bool checkQueueForChanges(void* pUserData);

private:
	unsigned long mMsQueueChanged;
};


extern TaskQueue	g_TaskQueue;

#endif /* TASKQUEUE_H_ */