#include <chrono>
#include <iostream>

#include "RBI.hpp"

void factorial(unsigned n)
{
	Desant::RBI factorial{ 1u };

	auto start{ std::chrono::high_resolution_clock::now() };
	for (unsigned i{ 1u }; i <= n; i++)
	{
		factorial = factorial*i;
	}
	auto end{ std::chrono::high_resolution_clock::now() };

	std::cout << std::endl << n << "!: ";
	std::cout << factorial.string();
	std::cout << "\nTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\nRBI size: " << factorial.get_size() << "B\nDigits: " << factorial.digits() << "\n";
}

void fibonacci(unsigned n)
{
	Desant::RBI first{ "1" };
	Desant::RBI second{ "1" };
	Desant::RBI temp{ "0" };
	unsigned a{ n };

	auto start{ std::chrono::high_resolution_clock::now() };
	do
	{
		temp = second;
		second += first;
		first = temp;
		a--;
	} while (a >= 3u);
	auto end{ std::chrono::high_resolution_clock::now() };

	std::cout << std::endl << n << ". number of Fibbonacci: ";
	std::cout << second.string();
	std::cout << "\nTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\nRBI size: " << second.get_size() << "B\nDigits: " << second.digits() << "\n";
}

int main()
{
	fibonacci(77000u);
}