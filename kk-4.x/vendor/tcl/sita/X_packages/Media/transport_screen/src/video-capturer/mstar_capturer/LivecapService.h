#ifndef ANDROID_LIVECAPSERVICE_H
#define ANDROID_LIVECAPSERVICE_H

#include <utils/Log.h>
#include <utils/threads.h>
#include <utils/List.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include <utils/Vector.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryBase.h>


namespace android
{
  class LivecapService : public BBinder
  {
    public:
      static int instantiate();
      LivecapService();
      virtual ~LivecapService();
      virtual status_t onTransact( uint32_t, const Parcel&, Parcel*, uint32_t );
      static  status_t setCallback(void *callback,void *caller);

  };
}   

#endif
