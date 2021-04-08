/// @copyright 2020 ECCI, Universidad de Costa Rica. All rights reserved
/// This code is released under the GNU Public License version 3
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>
#include <mutex>

#include "../common/common.hpp"

/**
 * @brief A thread-safe log manager singleton object
 */

class Log
{
	DISABLE_COPY(Log);

private:
	/// Output will be sent to this stream
	std::ostream *output = &std::cout;
	/// Mutual exclusion of writing operations
	std::mutex mutex;

public:
	/// Get access to the unique instance of this Singleton class
	static Log &getInstance();
	/// Changes the output stream
	inline void setOutput(std::ostream &output) { this->output = &output; }
	/// Writes the given text to the log
	void write(const std::string &text, bool addNewLine = true);
	/// Convenience static method for calling write
	inline static void append(const std::string &text) { return getInstance().write(text); }

private:
	/// Private constructor. Call @a getInstance() to get the unique instance of this class
	Log() = default;
	//// Private destructor
	~Log() = default;
};

#endif // LOGTEST_HPP
