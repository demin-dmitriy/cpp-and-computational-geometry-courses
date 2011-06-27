#pragma once

template<bool> struct ctassert;
template<> struct ctassert<true>{};
