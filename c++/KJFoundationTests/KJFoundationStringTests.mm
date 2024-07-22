//
//  KJFoundationStringTests.m
//  KJFoundationTests
//
//  Created by Ronny Falk on 7/22/24.
//

#import <XCTest/XCTest.h>
#import <kj/kj.h>
#import <string.h>

#define EXPECT_EQ XCTAssertEqual

using namespace kj;

@interface KJFoundationStringTests : XCTestCase

@end

@implementation KJFoundationStringTests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testStr {
    EXPECT_EQ("foobar", str("foo", "bar"));
    EXPECT_EQ("1 2 3 4", str(1, " ", 2u, " ", 3l, " ", 4ll));
    EXPECT_EQ("1.5 foo 1e15 bar -3", str(1.5f, " foo ", 1e15, " bar ", -3));
    EXPECT_EQ("foo", str('f', 'o', 'o'));
    EXPECT_EQ("123 234 -123 e7", str((int8_t)123, " ", (uint8_t)234, " ", (int8_t)-123, " ", hex((uint8_t)0xe7)));
    EXPECT_EQ("-128 -32768 -2147483648 -9223372036854775808", str((signed char)-128, ' ', (signed short)-32768, ' ', ((int)-2147483647) - 1, ' ', ((long long)-9223372036854775807ll) - 1));
    EXPECT_EQ("ff ffff ffffffff ffffffffffffffff", str(hex((uint8_t)0xff), ' ', hex((uint16_t)0xffff), ' ', hex((uint32_t)0xffffffffu), ' ', hex((uint64_t)0xffffffffffffffffull)));
}

@end
