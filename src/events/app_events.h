#ifndef _BMCDISPLAYAPP_APP_APPEVENTS_H_
#define _BMCDISPLAYAPP_APP_APPEVENTS_H_

#include <ds/app/event.h>

namespace bmc {

class IdleStartedEvent : public ds::RegisteredEvent < IdleStartedEvent > {
public:
	IdleStartedEvent(){};
};

class IdleEndedEvent : public ds::RegisteredEvent < IdleEndedEvent > {
public:
	IdleEndedEvent(){};

};

} // !namespace bmc

#endif // !_BMCDISPLAYAPP_APP_APPEVENTS_H_