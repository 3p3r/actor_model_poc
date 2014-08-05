#include <Theron/Theron.h>
#include "Worker.h"
#include <vector>
#include <queue>

/************************************************************************/
/* Work managers														*/
/************************************************************************/

/*
* A WorkManager actor that processes work items.
* Internally the WorkManager creates and controls a pool of workers. 
* It coordinates the workers to process the work items in parallel.
*
* Assume different systems in a game engine:
* Render engine
* Script engine
* Scene graph engine
* AI
*
* WorkManager refers to a specific part of the engine and can
* process the requests of that type, for example Render Engine's
* WorkManager only render entities
*/

class WorkManager : public Theron::Actor 
{ 
public: 
	WorkManager(Theron::Framework &framework, const int id = 0, const int workerCount = 1) : Theron::Actor(framework)
	{ 
		// Create the workers and add them to the free list. 
		for (int i = 0; i < workerCount; ++i) 
		{ 
			mWorkers.push_back(new Worker(framework)); 
			mFreeQueue.push(mWorkers.back()->GetAddress()); 
		} 

		RegisterHandler(this, &WorkManager::Handler); 
	} 

	~WorkManager() 
	{ 
		// Destroy the workers. 
		const int workerCount(static_cast<int>(mWorkers.size())); 
		for (int i = 0; i < workerCount; ++i) 
		{ 
			delete mWorkers[i]; 
		} 
	} 
	
	int id;
private: 
	// Handles work requests from clients. 
	void Handler(const Work &message, const Theron::Address from) 
	{ 
		// Has this work item been processed? 
		if (message.Processed()) 
		{ 
			// Send the result back to the caller that requested it. 
			Send(message, message.Client()); 

			// Add the worker that sent the result to the free list. 
			mFreeQueue.push(from); 
		} 
		else 
		{ 
			// Add the unprocessed work message to the work list. 
			mWorkQueue.push(message); 
		} 

		// Service the work queue. 
		while (!mWorkQueue.empty() && !mFreeQueue.empty()) 
		{ 
			Send(mWorkQueue.front(), mFreeQueue.front()); 

			mFreeQueue.pop(); 
			mWorkQueue.pop(); 
		} 
	} 

	std::vector<Worker *> mWorkers;         // Pointers to the owned workers. 
	std::queue<Theron::Address> mFreeQueue;     // Queue of available workers. 
	std::queue<Work> mWorkQueue;         // Queue of unprocessed work messages. 
};