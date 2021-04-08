/// @copyright 2020 ECCI, Universidad de Costa Rica. All rights reserved
/// This code is released under the GNU Public License version 3
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#include "Log.hpp"

Log &Log::getInstance()
{
	static Log log;
	return log;
}

void Log::write(const std::string &text, bool addNewLine)
{
	this->mutex.lock();
	(*this->output) << text;
	if (addNewLine)
		(*this->output) << std::endl;
	this->mutex.unlock();
}
