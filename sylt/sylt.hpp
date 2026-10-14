// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Horizon-Velvet
// Licensed under the MIT License.

#ifndef SYLT_HPP
#define SYLT_HPP

#include <string>
#include <string_view>

namespace sylt
{
class BaseComposite
{
  public:
	virtual ~BaseComposite() = default;

	virtual BaseComposite& render() = 0;
};
class Panel : public BaseComposite
{
  public:
	Panel(std::string_view t) : title_(t) {}
	Panel() {}
	~Panel() override = default;

	Panel& render() override;

	Panel& add();

	Panel& title(std::string_view t) {
		title_ = std::string(t);
		return *this;
	}

	[[nodiscard]] std::string_view title() const {
		return title_;
	}

  private:
	std::string title_;
};
} // namespace sylt

#endif