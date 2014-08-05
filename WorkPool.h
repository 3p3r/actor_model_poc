#include <Theron/Theron.h>
#include "WorkManager.h"

/************************************************************************/
/* WorkPool                                                             */
/************************************************************************/

/*
* This class is really a wrapper for WorkManager
* it can add (enqueue) work requests to its hosted WorkManager
* 
* Assume different systems in a game engine:
* Render engine
* Script engine
* Scene graph engine
* AI
*
* Each of them can be encapsulated as a WorkPool
*/

class WorkPool
{
public:
	WorkPool() : mReady(true) , mFramework(new Theron::Framework()) {
		receiver.RegisterHandler(&resultCatcher, &Theron::Catcher<Work>::Push);
		dispatcher = new WorkManager(*mFramework);
	}

	~WorkPool() {
		if (dispatcher)
		{
			delete dispatcher;
		}
		if (mFramework)
		{
			delete mFramework;
		}
	}

	void HandleCallBack() {
		if (!results.empty())
		{
			results.clear();
		}
		while (!resultCatcher.Empty())
		{
			Work w;
			resultCatcher.Pop(w, from);
			results.push_back(w);
		}
		mReady = true;
	}

	void Enqueue(const Work& aWork) {
		mReady = false;
		mFramework->Send(aWork, receiver.GetAddress(), dispatcher->GetAddress());
		receiver.Wait();
		HandleCallBack();
	}

	void Enqueue(const std::vector<Work>& aWorks) {
		mReady = false;
		for(auto it = aWorks.begin(); it != aWorks.end(); ++it) {
			mFramework->Send(*it, receiver.GetAddress(), dispatcher->GetAddress());
		}
		// yes, two loops must be separate if you were wondering!
		for(auto it = aWorks.begin(); it != aWorks.end(); ++it) {
			receiver.Wait();
		}
		HandleCallBack();
	}

	void EnqueueAsync(const std::vector<Work>& aWorks) {
		std::thread t([=](){
			mReady = false;
			for(auto it = aWorks.begin(); it != aWorks.end(); ++it) {
				mFramework->Send(*it, receiver.GetAddress(), dispatcher->GetAddress());
			}
			// yes, two loops must be separate if you were wondering!
			for(auto it = aWorks.begin(); it != aWorks.end(); ++it) {
				receiver.Wait();
			}
			HandleCallBack();
		});
		t.detach();
	}

	std::vector<Work> getResults() {
		return results;
	}

	bool isReady() {return mReady;}
	Theron::Receiver receiver;
private:
	bool mReady;
	std::vector<Work> results;
	Theron::Address from;
	Theron::Framework *mFramework;
	Theron::Catcher<Work> resultCatcher;
	WorkManager *dispatcher;
};