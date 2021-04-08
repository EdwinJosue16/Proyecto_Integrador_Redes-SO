/// @copyright 2020 ECCI, Universidad de Costa Rica. All rights reserved
/// This code is released under the GNU Public License version 3
/// @author Jeisson Hidalgo-Céspedes <jeisson.hidalgo@ucr.ac.cr>

#include "Semaphore.hpp"

Semaphore::Semaphore(unsigned value)
{
	// todo: use named semaphores on macOS
	sem_init(&this->semaphore, 0 /*pshared*/, value);
}

Semaphore::~Semaphore()
{
	// todo: use named semaphores on macOS
	sem_destroy(&this->semaphore);
}

void Semaphore::signal()
{
	sem_post(&this->semaphore);
}

void Semaphore::wait()
{
	sem_wait(&this->semaphore);
}
