//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/NSNotificationCenter+OCMAdditions.m#1 $
//  Copyright (c) 2009 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import "NSNotificationCenter+OCMAdditions.h"
#import "OCObserverMockObject.h"


@implementation NSNotificationCenter(OCMAdditions)

- (void)addMockObserver:(OCMockObserver *)notificationObserver name:(NSString *)notificationName object:(id)notificationSender
{
	[self addObserver:notificationObserver selector:@selector(handleNotification:) name:notificationName object:notificationSender];
}

@end
