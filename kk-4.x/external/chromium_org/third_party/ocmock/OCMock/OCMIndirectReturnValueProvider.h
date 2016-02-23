//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/OCMIndirectReturnValueProvider.h#1 $
//  Copyright (c) 2009 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import <Foundation/Foundation.h>

@interface OCMIndirectReturnValueProvider : NSObject 
{
	id	provider;
	SEL	selector;
}

- (id)initWithProvider:(id)aProvider andSelector:(SEL)aSelector;

- (void)handleInvocation:(NSInvocation *)anInvocation;

@end
