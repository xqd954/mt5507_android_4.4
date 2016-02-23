//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/OCMExceptionReturnValueProvider.m#1 $
//  Copyright (c) 2009 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import "OCMExceptionReturnValueProvider.h"


@implementation OCMExceptionReturnValueProvider

- (void)handleInvocation:(NSInvocation *)anInvocation
{
	@throw returnValue;
}

@end
