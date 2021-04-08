/// @copyright 2020 ECCI, Universidad de Costa Rica. All rights reserved
/// This code is released under the GNU Public License version 3
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <semaphore.h>

#include "common.hpp"

/// Wrapper class for POSIX semaphores
class Semaphore
{
	DISABLE_COPY(Semaphore);

  private:
	/// A thread-safe integer value
	sem_t semaphore;

  public:
	/// Constructs a semaphore with the given value
	explicit Semaphore(unsigned value);
	/// Destructor
	~Semaphore();
	/// Increment the semaphore and wake a waiting thread if any
	void signal();
	/// Decrement the semaphore and block if the result is negative
	void wait();
};

#endif // SEMAPHORE_HPP
