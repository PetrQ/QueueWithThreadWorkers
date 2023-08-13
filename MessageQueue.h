#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "ImessageQueueEvents.h"

namespace pkus {

class MessageQueue
{
public:
     explicit MessageQueue( int queue_size, int lwm, int hwm ) {};
     void setEvents( IMessageQueueEventsPtr events ) {};

     void run()
     {
          m_events->on_start();
     }
     void stop()
     {
          m_events->on_stop();
     };

private:
     IMessageQueueEventsPtr m_events;
};

using MessageQueuePtr = ::std::shared_ptr< MessageQueue >;

} // namespace pkus

#endif // MESSAGEQUEUE_H
