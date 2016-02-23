//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/OCPartialMockObject.h#1 $
//  Copyright (c) 2009 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import "OCClassMockObject.h"

@interface OCPartialMockObject : OCClassMockObject 
{
	NSObject	*realObject;
}

- (id)initWithObject:(NSObject *)anObject;

- (NSObject *)realObject;

- (void)stop;

- (void)setupSubclassForObject:(id)anObject;
- (void)setupForwarderForSelector:(SEL)selector;

@end


extern NSString *OCMRealMethodAliasPrefix;
