// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Horizon-Velvet
// Licensed under the MIT License.

#include <iostream>

#include "./sylt.hpp"

using namespace sylt;

namespace sylt
{
Panel& Panel::render() {
	std::cout << "[\033[1m " << this->title_ << " \033[0m]" << std::endl;

	return *this;
}
Panel& Panel::add() {
	return *this;
}
} // namespace sylt
