#pragma once
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>



namespace Desant
{
	/// <summary>Defines max value of single digit in RBInteger object.</summary>
	constexpr unsigned long long RBI_INFINITY{ static_cast<unsigned long long>(1E11) };

	/// <summary>Count digits given number's digits.</summary>
	/// <param name="number">Number of which digits will be counted.</param>
	/// <returns>Number of given number's digits.</returns>
	template<class T>
	unsigned length(T number) noexcept
	{
		static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type.");
		return static_cast<unsigned>(std::floor(std::log10(number))+1.0);
	}

	class RBI
	{
		typedef std::vector<unsigned long long>::size_type vector_size_t; // cannot be any unsigned type including std::vector<unsigned long long>::size_type because then some decreasing loops won't work and goes from 0u to max value for std::vector<unsigned long long>::size_type and never ends, often causing undefined behaviour
	public:

		/// <summary>Performs internal string validation and returns number as string.</summary>
		/// <returns>String containing whole number.</returns>
		std::string string() noexcept;

	#pragma region operators
		std::ostream& operator <<(std::ostream& stream) noexcept;
		std::istream& operator >>(std::istream& stream);

		RBI& operator =(const std::string& v);

		template<typename T>
		RBI& operator =(const T& v)
		{
			static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");
			m_values.clear();
			if constexpr(std::numeric_limits<T>::max >= RBI_INFINITY)
			{
				if (v >= RBI_INFINITY)
				{
					m_values.push_back(v%RBI_INFINITY);
					m_values.push_back((v - v%RBI_INFINITY) / RBI_INFINITY);
				}
			}
			m_values.push_back(v);
			return *this;
		}


		bool operator !=(const RBI& v) noexcept;
		bool operator ==(const RBI& v) noexcept;

		template<typename T>
		bool operator ==(const T& v) noexcept
		{
			static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");
			if ((v == 0u || v == 0.0u) && (m_values.size() == 0u || (m_values.size() == 1u && m_values[0] == 0u)))
				return true;
			if (v >= RBI_INFINITY)
				return m_values.size() == 2u && m_values[0] == v%RBI_INFINITY && m_values[1] == (v - v%RBI_INFINITY) / RBI_INFINITY;
			else 
				return m_values.size() == 1u && m_values[0] == v;
		}
		template<typename T>
		bool operator !=(const T& v) noexcept
		{
			static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");

			if ((v == 0u || v == 0.0u) && (m_values.size() == 0u || (m_values.size() == 1u && m_values[0] == 0u)))
				return false;

			if constexpr(std::numeric_limits<T>::max() >= RBI_INFINITY)
			{
				if (v >= RBI_INFINITY)
					return m_values.size() != 2u || (m_values[0] != v%RBI_INFINITY || m_values[1] != (v - v%RBI_INFINITY) / RBI_INFINITY);
				else
					return m_values.size() != 1u || m_values[0] != v;
			}

			return m_values.size() != 1u || m_values[0] != v;
		}


		RBI operator +(const RBI& v) noexcept;
		RBI& operator +=(const RBI& v) noexcept;

		template<typename T>
		RBI operator +(const T& v) noexcept
		{
			static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");
			RBI copy{ *this };
			if (copy.m_digits[0] += static_cast<unsigned long long>(v) >= RBI_INFINITY) 
				copy.check_values();
			return copy;
		}
		template<typename T>
		RBI& operator +=(const T& v) noexcept
		{
			static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");
			if (m_digits[0] + v >= RBI_INFINITY)
				check_values();
			return *this;
		}

		RBI operator *(const RBI& v) noexcept;
		RBI& operator *=(const RBI& v) noexcept;

		template<typename T>
		RBI operator *(const T& v) noexcept
		{
			static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");
			return SBI{ v }*SBI{ *this };
		}

		template<>
		RBI operator *(const unsigned short& v) noexcept
		{
			RBI copy{ *this };
			for (unsigned long long & val : copy.m_values)
				val *= v;
			copy.check_values();
			return copy;
		}

		template<>
		RBI operator *(const unsigned char& v) noexcept
		{
			RBI copy{ *this };
			for (unsigned long long & val : copy.m_values)
				val *= v;
			copy.check_values();
			return copy;
		}

		template<typename T>
		RBI& operator *=(const T& v)
		{
			static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");
			return *this = SBI{ v }*SBI{ *this };
		}

		template<>
		RBI& operator *=(const unsigned short& v) noexcept
		{
			for (unsigned long long & val : m_values)
				val *= v;
			return *this;
		}

		template<>
		RBI& operator *=(const unsigned char& v) noexcept
		{
			for (unsigned long long & val : m_values)
				val *= v;
			return *this;
		}
	#pragma endregion

	#pragma region constructors
		/// <summary>Initializes RBI object with value of any unsigned arithmetical type.</summary>
		/// <param name="initial_value">Initial value to be set.</param>
		/// <returns>Reference to created object.</returns>
		template<class T>
		RBI(const T& initial_value) noexcept : m_values{ {static_cast<unsigned long long>(initial_value)} }
		{
			static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");
		}

		/// <summary>Initializes RBI object with value of any number given in string.</summary>
		/// <param name="initial_value">Initial value to be set.</param>
		/// <returns>Reference to created object.</returns>
		RBI(const std::string initial_value);

		/// <summary>Initializes RBI object with value of any number given in c-string.</summary>
		/// <param name="initial_value">Initial value to be set.</param>
		/// <returns>Reference to created object.</returns>
		RBI(const char* initial_value);

		/// <summary>Initializes RBI object with another RBI object.</summary>
		/// <param name="initial_value">Initial value to be set.</param>
		/// <returns>Reference to created object.</returns>
		RBI(const RBI&initial_value) noexcept;

		/// <summary>Initializes RBI object with initial value of 0.</summary>
		/// <returns>Reference to created object.</returns>
		RBI() noexcept;
	#pragma endregion
	
		/// <summary>Method used to get informations about memory usage by the RBI object.</summary>
		/// <returns>Memory used by the RBI object (in bytes).</returns>
		inline int get_size() noexcept 
		{ 
			return static_cast<int>(m_values.capacity()) * sizeof(unsigned long long);
		}

		/// <summary>Used to get number of digits in object.</summary>
		/// <returns>Number of digits.</returns>
		vector_size_t digits() noexcept;

	private:
		RBI(const std::vector<unsigned long long>& value) noexcept;
		
		/// <summary><b>Performs string validation</b> and updates RBI to given string.</summary>
		void set_value(const std::string& intval);
		void validate_value(const std::string& string);
		void update_string() noexcept;
		void rearrange_values() noexcept;
		void check_values() noexcept;
		std::stringstream revert_string(std::string x) noexcept;

		int m_control;
		std::string m_string;
		std::vector<unsigned long long> m_values;

#pragma region Smaller Big Integer
		class SBI
		{
			static constexpr unsigned LOW_RBI_INFINITY{ static_cast<unsigned>(1E5) };

			friend class RBI;	
		public:
			SBI(const std::vector<unsigned long long>& x) noexcept : m_values(x) { }
			SBI(const RBI& x) noexcept : m_values{ x.m_values } { check_values(); }
			std::string string() noexcept;
			RBI operator*(const SBI& v) noexcept;
		private:
			template<typename T>
			SBI(const T& x) noexcept : m_values{ { static_cast<unsigned long long>(x) } }
			{
				static_assert(std::is_unsigned<T>::value, "T must be an unsigned arithmetic type.");
			}

			std::vector<unsigned long long> m_values;
			void check_values() noexcept;
		};
#pragma endregion
	};

	/// <summary>Throwed when string/c-string given to constructor contains characters that aren't digits.</summary>
	class invalid_string_exception : public std::exception
	{
	};
}