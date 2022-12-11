#pragma once

#define __comma ,
#define __leftBracket (
#define __rightBracket )

#define __eat_comma(...) ,##__VA_ARGS__
#define __mid_layer(...) __VA_ARGS__