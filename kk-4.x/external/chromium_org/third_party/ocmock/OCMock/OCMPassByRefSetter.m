//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/OCMPassByRefSetter.m#1 $
//  Copyright (c) 2009 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import "OCMPassByRefSetter.h"


@implementation OCMPassByRefSetter

- (id)initWithValue:(id)aValue
{
	self = [super init];
	value = [aValue retain];
	return self;
}

- (void)dealloc
{
	[value release];
	[super dealloc];
}

- (id)value
{
	return value;
}

@end
