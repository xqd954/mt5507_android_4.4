//---------------------------------------------------------------------------------------
//  $Id: //DTV/MP_BR/DTV_X_IDTV0801_002298_3_001/android/kk-4.x/external/chromium_org/third_party/ocmock/OCMock/OCMBlockCaller.h#1 $
//  Copyright (c) 2010 by Mulle Kybernetik. See License file for details.
//---------------------------------------------------------------------------------------

#import <Foundation/Foundation.h>

#if NS_BLOCKS_AVAILABLE

@interface OCMBlockCaller : NSObject 
{
	void (^block)(NSInvocation *);
}

- (id)initWithCallBlock:(void (^)(NSInvocation *))theBlock;

- (void)handleInvocation:(NSInvocation *)anInvocation;

@end

#endif
