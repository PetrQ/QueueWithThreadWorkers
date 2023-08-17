#ifndef IMESSAGEQUEUEEVENTS_H
#define IMESSAGEQUEUEEVENTS_H

#include <memory>

namespace pkus {

class IMessageQueueEvents
{
public:
     virtual void on_start() = 0;
     virtual void on_stop() = 0;
     virtual void on_hwm() = 0;
     virtual void on_lwm() = 0;
};

using IMessageQueueEventsPtr = std::shared_ptr< IMessageQueueEvents >;

} // namespace pkus

#endif // IMESSAGEQUEUEEVENTS_H
