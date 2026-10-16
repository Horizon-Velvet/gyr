/*
   Copyright 2026 Horizon-Velvet

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

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