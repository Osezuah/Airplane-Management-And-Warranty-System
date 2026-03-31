#pragma once

#include <cstdint>

enum class ErrorCode : int32_t {
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    UNPROCESSABLE = 422,
    INTERNAL = 500,
    NOT_IMPLEMENTED = 501,
};