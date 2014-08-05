#include <Theron/Theron.h>
#include "Work.h"

/************************************************************************/
/* Worker class															*/
/************************************************************************/

/*
* A worker simply fires the Process method of work requests
* Since Work objects are not actors, you can think of workers
* as the bridge between object-oriented world and the actor world.
*
* PS: Each worker can only process one work item at a time.
*/
class Worker : public Theron::Actor
{
public:
	Worker(Theron::Framework &framework) : Theron::Actor(framework)
	{
		RegisterHandler(this, &Worker::Handler);
	}

private:
	// Message handler for WorkMessage messages.
	void Handler(const Work &message, const Theron::Address from)
	{
		// The message parameter is const so we need to copy it to change it.
		Work result(message);
		result.Process();

		// Forward the processed message back to the sender.
		Send(result, from);
	}
};