#include <Theron/Theron.h>
#include <boost/any.hpp>
#include <iostream>

/************************************************************************/
/* Work class															*/
/************************************************************************/

/*
* Work class is an internal representation of a request to be handled
* You can assume work class as a routine that takes so much time to
* process in sync.
* E.g: Loading a huge texture or querying size of an image
* I use boost::any because of the flexibility of the datatype that can
* be transfered between workers.
*/

struct Work
{
public:
	explicit Work(
		boost::any req = nullptr, const Theron::Address client = Theron::Address()) :
		mClient(client),
		mContainer(req),
		mProcessed(false)
	{}

	virtual void Process()
	{
		std::cout << "processing: " << boost::any_cast<std::string>(mContainer) << std::endl;
		//modify the req here. your modifications will be passed to the receiver of the request
		mProcessed = true;
	}

	bool Processed() const
	{
		return mProcessed;
	}

	const Theron::Address& Client() const
	{
		return mClient;
	}

	Theron::Address mClient;            // Address of the requesting client.
	boost::any mContainer;				// container that will be changed after the Process is done
	bool mProcessed;                    // Whether the file has been read.
};