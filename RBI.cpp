#include "RBI.hpp"
#include <cmath>
#include <ctime>

namespace Desant
{

#pragma region Constructors
	RBI::RBI() noexcept : m_values{1u,0}
	{
	}

	RBI::RBI(const char* val)
	{
		set_value(std::string(val));
	}

	RBI::RBI(const std::string intval)
	{
		set_value(intval);
	}

	RBI::RBI(const RBI&v) noexcept : m_values{v.m_values}
	{
	}

	RBI::RBI(const std::vector<unsigned long long>& v) noexcept : m_values{v}
	{
		check_values();
	}

	void RBI::set_value(const std::string& intval)
	{
		validate_value(intval);
		for (int i{ static_cast<int>(intval.length() - 1) }; i >= 0; i--) // 'i' cannot be std::string::size_type because if it is then it goes from intval.length()-1 to 0 and then reaches upper bound of unsigned
			m_values.push_back(intval[i] - '0');
		rearrange_values();
	}
#pragma endregion

#pragma region Operators and operations
	std::ostream& RBI::operator <<(std::ostream& stream) noexcept
	{
		return stream << string();
	}

	std::istream& RBI::operator >>(std::istream& stream)
	{
		std::string buffer;
		stream >> buffer;
		set_value(buffer);
		return stream;
	}

	RBI& RBI::operator =(const std::string& v)
	{
		set_value(v);
		return *this;
	}

	bool RBI::operator !=(const RBI& v) noexcept
	{ 
return (v.m_values != m_values);
	}

	bool RBI::operator ==(const RBI& v) noexcept
	{
		return (v.m_values == m_values);
	}

	RBI& RBI::operator +=(const RBI& v) noexcept
	{
		auto first_size{ v.m_values.size() }, second_size{ m_values.size() };
		if (first_size >= second_size)
		{
			for (vector_size_t i{ 0 }; i < second_size; i++)
				m_values[i] += v.m_values[i];
			for (vector_size_t i{ static_cast<vector_size_t>(second_size) }; i < first_size; i++)
				m_values.push_back(v.m_values[i]);
		}
		else
		{
			for (vector_size_t i{ 0u }; i < first_size; i++)
				m_values[i] += v.m_values[i];
		}
		m_control++;
		check_values();
		return *this;
	}

	RBI RBI::operator +(const RBI& v) noexcept
	{
		RBI out{ *this };
		out += v;
		return out;
	}

	RBI& RBI::operator *=(const RBI& v) noexcept
	{
		return *this = *this*v;
	}
#pragma endregion

#pragma region Output
	void RBI::validate_value(const std::string& string)
	{
		for (const char & x : string)
			if (x < '0' || x > '9') throw invalid_string_exception();
	}

	std::string RBI::string() noexcept
	{
		check_values();
		update_string();
		return m_string;
	}
#pragma endregion

#pragma region validating overflow
	std::stringstream RBI::revert_string(std::string x) noexcept
	{
		std::stringstream stream;
		for (int i{ static_cast<int>(x.length() - 1) }; i >= 0; i--) // 'i' cannot be std::string::size_type because if it is then it goes from intval.length()-1 to 0 and then reaches upper bound of unsigned
			stream << x[i];
		return stream;
	}

	void RBI::rearrange_values() noexcept
	{
		std::vector<unsigned long long> temp_values;
		int counter{ 0 };
		unsigned long long * valptr = &m_values[0];
		std::stringstream str;
		while (true)
		{
			str << (*valptr);
			if (counter == m_values.size() - 1)
			{
				unsigned long long val;
				revert_string(str.str()) >> val;
				temp_values.push_back(val);
				break;
			}
			counter++;
			if (counter % 11 == 0)
			{
				std::stringstream nullstr;
				unsigned long long val;
				revert_string(str.str()) >> val;
				temp_values.push_back(val);
				str.swap(nullstr);
			}
			valptr++;
		}
		m_values = temp_values;
		check_values();
		return;
	}

	void RBI::check_values() noexcept
	{
		for (vector_size_t i{ 0u }; i < m_values.size(); i++)
		{
			if (m_values[i] >= RBI_INFINITY)
			{
				if (i + 1 == m_values.size())
					m_values.push_back(0ull);

				m_values[i + 1] += static_cast<unsigned long long>(std::floor(m_values[i] / RBI_INFINITY));
				m_values[i] -= static_cast<unsigned long long>(std::floor(m_values[i] / RBI_INFINITY) * RBI_INFINITY);
			}
		}
	}

	void RBI::update_string() noexcept
	{
		std::ostringstream oss;
		oss << m_values[m_values.size() - 1];
		vector_size_t i{ m_values.size()-1u };
		if (m_values.size() > 1u)
		{
			do
			{
				i--;
				for (long long x{ RBI_INFINITY / 10 }; x >= 10; x /= 10)
				{
					if (m_values[i] < static_cast<unsigned long long>(x))
						oss << "0";
					else
						break;
				}
				oss << m_values[i];
			} while (i != 0u);
		}
		m_string = oss.str();
	}

	RBI::vector_size_t RBI::digits() noexcept
	{
		return static_cast<vector_size_t>(m_values.size() - 1) * 11 + length(*(m_values.end() - 1));
	}
#pragma endregion

	// Implementation of RBI::SBI class and multiplication
#pragma region Smaller Big Integer

	RBI RBI::operator *(const RBI& v) noexcept
	{
		return SBI(*this)*SBI(v);
	}

	std::string RBI::SBI::string() noexcept
	{
		std::stringstream outstream;
		outstream << m_values[m_values.size() - 1];
		for (auto i{ m_values.end() - 1 }; i != m_values.begin()-1; i--)
		{
			for (int j{ LOW_RBI_INFINITY / 10 }; j >= 10; j /= 10)
			{
				if (*i < j)
					outstream << 0;
				else break;
			}
			outstream << *i;
		}
		return outstream.str();
	}

	RBI RBI::SBI::operator*(const SBI&v) noexcept
	{
		std::vector<std::vector<unsigned long long>> end_values;

		// Multiplication of "digits"
		int counter{ 0 };
		for (auto j{ v.m_values.begin() }; j < v.m_values.end(); j++)
		{
			std::vector<unsigned long long> temp_values;
			for (int s{ 0 }; s < counter; s++) temp_values.push_back(0ull);
			for (auto i{ m_values.begin() }; i != m_values.end(); i++)
			{
				temp_values.push_back((*i) * (*j));
			}
			end_values.push_back(temp_values);
			counter++;
		}

		// Finding the biggest vector of digits (like a row) and copying it as x
		counter = 0;
		for (vector_size_t i{ 0u }; i < end_values.size(); i++)
			if (end_values[i].size() > counter) counter = static_cast<int>(i);

		std::vector<unsigned long long>& summary = end_values[counter];

		// Adding columns from specified rows to summary
		vector_size_t it{ end_values.size()-1 };
		do
		{
			if (it == counter)
			{
				if (it == 0u) break;
				it--;
				continue;
			}
			for (vector_size_t a{ 0 }; a < summary.size(); a++)
			{
				if (a >= end_values[it].size()) break;
				summary[a] += end_values[it][a];
			}
			if (it == 0u) break;
			it--;
		} while(true);

		// Checking if any digit isn't bigger than LOW_RBI_INFINITY
		vector_size_t xit{ summary.size() };
		do
		{
			xit--;
			if (summary[xit] >= LOW_RBI_INFINITY)
			{
				unsigned long long overflow{ static_cast<unsigned long long>(floor(summary[xit] / LOW_RBI_INFINITY)) };
				summary[xit] -= overflow * LOW_RBI_INFINITY;
				if (xit + 1 == summary.size()) summary.push_back(overflow);
				else summary[xit + 1] += overflow;
			}
		} while (xit != 0u);

		// Sending all values to stream and creating RBI from stringstream
		std::stringstream outstream;
		outstream << summary[summary.size() - 1];
		if (summary.size() != 1u)
		{
			it = summary.size() - 1u;
			do
			{
				it--;
				for (int p{ LOW_RBI_INFINITY / 10 }; p >= 10; p /= 10)
				{
					if (summary[it] < p) outstream << 0;
					else break;
				}
				outstream << summary[it];
				if (it == 0u) break;
			} while (true);
		}
		return RBI{ outstream.str() };
	}

	void RBI::SBI::check_values() noexcept
	{
		std::stringstream stringbuffer;
		stringbuffer << m_values[m_values.size() - 1];
		if (m_values.size() > 1u)
		{
			vector_size_t i{ m_values.size() };
			do
			{
				i--;
				for (unsigned long long j{ RBI_INFINITY / 10 }; j >= 10; j /= 10)
				{
					if (m_values[i] < j) stringbuffer << 0;
					else break;
				}
				stringbuffer << m_values[i];
			} while (i != 0u);
		}

		int counter = 0;

		std::string separated_string(stringbuffer.str());
		int char_counter = 0;
		for (int i{ static_cast<int>(separated_string.length() - 1) }; i >= 1; i--)
		{
			if ((char_counter + 1) % 5 == 0)
			{
				separated_string.insert(i, " ");
				counter++;
				char_counter = 0;
				i--;
			}
			char_counter++;
		}

		std::stringstream m_values_stream(separated_string);
		m_values.clear();
		int spaces{ static_cast<int>(std::floor((stringbuffer.str().length() - 1) / 5)) };
		while (m_values.size() <= spaces) m_values.push_back(0);
		for (int i{ 0 }; i <= spaces; i++) m_values_stream >> m_values[m_values.size() - 1 - i];
		return;
	}
#pragma endregion
}