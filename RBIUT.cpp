// Really big integer basic unit tests implementation
#include "RBI.hpp"
#include <string>
#include <fstream>
#include <chrono>

class RBIUT
{
	// Class that outputs results and stores informations about test suit
	class TestSuit
	{
	private:
		std::fstream log; // Prints advanced logs
		std::fstream values; // Prints inputs with valid output
		long long time; // Timestamp of tests
		std::string name; // Name of testsuit category

	public:
		// Prints results of given tests
		void Failed(std::string input, std::string output, std::string receivedOutput)
		{
			std::cout << name << " test failed.\n\tGiven input:\t\t" << input << "\n\tReceived output:\t" << receivedOutput << "\n\tCorrect output:\t\t" << output << std::endl << std::endl;
			values << input << std::endl;
			return;
		}

		// Constructor and destructor, nothing special
		TestSuit(std::string v) : name(v)
		{
			std::stringstream filestringstream;
			filestringstream << name;
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
			time = ms.count();
			filestringstream << " " << time << ".txt";
			log.open(filestringstream.str(), std::ios::app);
			std::stringstream valuesstringstream;
			valuesstringstream << "values " << name << " " << time << ".txt";
			values.open(valuesstringstream.str(), std::ios::app);
		}
		~TestSuit()
		{
			log.close();
			values.close();
		}
	};

public:
	// Tests string-based correctors. Correct (output) and input must be the same! Count stands for count of elements in array
	void ConstructorTest(std::string input[], std::string correct[], int count)
	{
		int counter = 0;
		TestSuit constructor("String-based constructor");
		for (int i = 0; i < count; i++)
		{
			if (input[i] != correct[i])
			{
				throw std::exception("Co xD");
			}
			if (Desant::RBI(input[i]).string() != correct[i])
			{
				constructor.Failed(input[i], correct[i], Desant::RBI(input[i]).string());
				counter++;
			}
		}
		std::cout << counter << " fails of " << count << " string-based constructors tests.\n";
	}

	// RBI -> SBI constructor test
	void ConverterTest(std::string input[], std::string correct[], int count)
	{
		int counter = 0;
		TestSuit converter("RBI to SBI constructor");
		for (int i = 0; i < count; i++)
		{
			if (input[i] != correct[i])
			{
				throw std::exception("Co xD");
			}
			if (Desant::RBI(Desant::RBI(input[i])).string() != correct[i])
			{
				converter.Failed(input[i], correct[i], Desant::RBI(Desant::RBI(input[i])).string());
				counter++;
			}
		}
		std::cout << counter << " fails of " << count << " RBI to SBI constructors tests.\n";
	}

	// Tests multiplication operations
	void MultiplicationTest(Desant::RBI a[], Desant::RBI b[], Desant::RBI sum[], int count)
	{
		TestSuit multiplicationTest("Multiplication");
		int fails = 0;
		for (int i = 0; i < count; i++)
		{
			Desant::RBI out = (a[i] * b[i]);
			if (out != sum[i])
			{
				std::stringstream oss;
				oss << out.string();
				std::string output = oss.str();
				std::string correct = sum[i].string();
				std::stringstream nuller;
				oss.swap(nuller);
				oss << a[i].string() << " * " << b[i].string();
				std::string input = oss.str();
				multiplicationTest.Failed(input, correct, output);
				fails++;
			}
		}
		std::cout << fails << " fails of " << count << " multiplication tests.\n";
	}

	RBIUT() { }

	// Automatical dynamic / static tests
	void RunConstructor(int digits = 250)
	{
		constexpr unsigned test_count = 100;
		const unsigned max_digits = digits; // max digits count
		std::string output[test_count];
		for (int i = 0; i < test_count; i++) output[i] = [](int digits)->std::string
		{
			std::stringstream x;
			x << (rand() % 9) + 1;
			for (int a = 1; a < digits; a++)
				x << rand() % 10;
			return x.str();
		}(rand() % max_digits + 1);
		ConstructorTest(output, output, test_count);
	}

	void RunConverter(int digits = 250)
	{
		constexpr unsigned test_count = 5;
		const unsigned max_digits = digits; // max digits count
		std::string output[test_count];
		for (int i = 0; i < test_count; i++) output[i] = [](int digits)->std::string
		{
			std::stringstream x;
			x << (rand() % 9) + 1;
			for (int a = 1; a < digits; a++)
			{
				x << rand() % 10;
			}
			return x.str();
		}(rand() % max_digits + 1);
		ConverterTest(output, output, test_count);
	}

	// Runs multiplication test
	void RunMultiplication()
	{
		Desant::RBI a[] =
		{
			"2",
			"10",
			"100",
			"8432",
			"58435",
			"543895",
			"5843955",
			"589436845976845932342"
		};

		Desant::RBI b[] =
		{
			"2",
			"10",
			"100",
			"9348",
			"84239",
			"498235",
			"5438932",
			"10"
		};

		Desant::RBI c[] =
		{
			"4",
			"100",
			"10000",
			"78822336",
			"4922505965",
			"270987525325",
			"31784873856060",
			"5894368459768459323420"
		};

		const int _testcnt = 0;
		MultiplicationTest(a + _testcnt, b + _testcnt, c + _testcnt, 8 - _testcnt);
	}
};