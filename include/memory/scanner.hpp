#pragma once
#include <memory/module.hpp>

namespace memory
{
	inline bool correlate(std::uint8_t* target, const std::optional<std::uint8_t>* sig, std::size_t length)
	{
		for (std::size_t i = 0; i < length; ++i)
		{
			if (sig[i] && *sig[i] != target[i])
				return false;
		}

		return true;
	}

	inline int pattern_id = 0;

	template <typename type = uintptr_t>
	inline type find_pattern(module_t mod, std::string name, std::string pattern, int offset = 0)
	{
		static auto char_to_integer = [](const char c)
		{
			if (c >= '0' && c <= '9')
				return c - '0';
			if (c >= 'A' && c <= 'F')
				return c - 'A' + 10;
			if (c >= 'a' && c <= 'f')
				return c - 'a' + 10;

			return -1;
		};

		std::vector<std::optional<uint8_t>> bytes;

		std::istringstream sigstream(pattern);
		std::vector<std::string> pattern_array
		{
			std::istream_iterator<std::string>(sigstream), {}
		};

		for (auto& byte : pattern_array)
		{
			if (byte == "?")
			{
				bytes.push_back(std::nullopt);
				continue;
			}

			if (auto c1 = char_to_integer(byte[0]); c1 != -1)
			{
				if (auto c2 = char_to_integer(byte[1]); c2 != -1)
				{
					bytes.emplace_back(
						static_cast<uint8_t>(c1 * 0x10 + c2)
					);
				}
			}
		}

		auto data = bytes.data();
		auto length = bytes.size();

		pattern_id++;

		auto base = mod.base();
		for (size_t i = 0; i < mod.size(); i++)
			if (correlate(reinterpret_cast<uint8_t*>(base + i), data, length)) {
				Log::Debug(_xor_("[+] Memory >>"), name, (base + i + offset), "#" + std::to_string(pattern_id));
				return (type)(base + i + offset);
			}

		Log::Error(_xor_("Failed to find signature pattern"), name);
		return (type)(nullptr);
	}

	template <typename type>
	type as_relative(type address, int offset = 3)
	{
		return reinterpret_cast<type>(reinterpret_cast<uintptr_t>(address) + *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(address) + offset) + (offset + 4i32));
	}

	template <typename type = uintptr_t>
	type as_relative(void* address, int offset = 3)
	{
		return reinterpret_cast<type>(reinterpret_cast<uintptr_t>(address) + *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(address) + offset) + (offset + 4i32));
	}

	template <typename type = uintptr_t>
	type as_relative(uintptr_t address, int offset = 3)
	{
		return (type)(address + *reinterpret_cast<int*>(address + offset) + (offset + 4i32));
	}
}