// vim: ft=cpp
#pragma once

// Global namespace for all programs.

using error_t = int;

const error_t OK        = 0;
const error_t ERR       = -1;
const error_t ERR_EOF   = -2;
const error_t ERR_LIMIT = -3;
const error_t ERR_IO    = -4;
