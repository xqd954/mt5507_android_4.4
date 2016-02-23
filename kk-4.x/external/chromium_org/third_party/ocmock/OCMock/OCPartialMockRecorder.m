//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/OCPartialMockRecorder.m#1 $
//  Copyright (c) 2009-2010 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import "OCPartialMockObject.h"
#import "OCMRealObjectForwarder.h"
#import "OCPartialMockRecorder.h"


@implementation OCPartialMockRecorder

- (id)andForwardToRealObject
{
	[invocationHandlers addObject:[[[OCMRealObjectForwarder	alloc] init] autorelease]];
	return self;
}


- (void)forwardInvocation:(NSInvocation *)anInvocation
{
	[super forwardInvocation:anInvocation];
	// not as clean as I'd wish...
	[(OCPartialMockObject *)signatureResolver setupForwarderForSelector:[anInvocation selector]];
}

@end
