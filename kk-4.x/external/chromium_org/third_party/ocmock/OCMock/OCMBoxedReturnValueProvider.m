//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/OCMBoxedReturnValueProvider.m#1 $
//  Copyright (c) 2009 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import "OCMBoxedReturnValueProvider.h"


@implementation OCMBoxedReturnValueProvider

- (void)handleInvocation:(NSInvocation *)anInvocation
{
	if(strcmp([[anInvocation methodSignature] methodReturnType], [(NSValue *)returnValue objCType]) != 0)
		@throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"Return value does not match method signature." userInfo:nil];
	void *buffer = malloc([[anInvocation methodSignature] methodReturnLength]);
	[returnValue getValue:buffer];
	[anInvocation setReturnValue:buffer];
	free(buffer);
}

@end
