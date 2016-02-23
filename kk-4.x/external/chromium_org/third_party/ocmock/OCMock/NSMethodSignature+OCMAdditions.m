//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/NSMethodSignature+OCMAdditions.m#1 $
//  Copyright (c) 2009 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import "NSMethodSignature+OCMAdditions.h"


@implementation NSMethodSignature(OCMAdditions)

- (const char *)methodReturnTypeWithoutQualifiers
{
	const char *returnType = [self methodReturnType];
	while(strchr("rnNoORV", returnType[0]) != NULL)
		returnType += 1;
	return returnType;
}

@end
