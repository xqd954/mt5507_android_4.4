//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/OCMNotificationPoster.m#1 $
//  Copyright (c) 2009 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import "OCMNotificationPoster.h"


@implementation OCMNotificationPoster

- (id)initWithNotification:(id)aNotification
{
	self = [super init];
	notification = [aNotification retain];
	return self;
}

- (void)dealloc
{
	[notification release];
	[super dealloc];
}

- (void)handleInvocation:(NSInvocation *)anInvocation
{
	[[NSNotificationCenter defaultCenter] postNotification:notification];
}


@end
