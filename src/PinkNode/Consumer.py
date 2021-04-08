from Queue import Queue
#from Package import Package
from threading import Thread
import random
import time

END_CONSUMER = -1
NEW_ACK = "new"
TIMEOUT_ACK = "timeout"

class Consumer():

	def __init__(self, packet_loss_probability, timeout, printer):
		# Command line arguments.
		self._packet_loss_probability = packet_loss_probability
		self._timeout = timeout
		# Thread and queues.
		self._thread = None
		self._reception_queue = Queue()
		self._ack_queue = None 

		# Other variables.
		self._request_number = 0
		self._printer = printer

	def get_reception_queue(self):
		return self._reception_queue

	def set_ack_queue(self, ack_queue):
		self._ack_queue = ack_queue

	def start(self):
		self._thread = Thread(target = self._run)
		self._thread.start()

	def wait_to_finish(self):
		self._thread.join()

	def _run(self):
		# Condition to end consumer work.
		condition = True
		while condition:
			package = self._reception_queue.dequeue()
			if package.sequence_number == END_CONSUMER:
				condition = False
			else:
				expected_package = False
				if package.sequence_number == self._request_number:
					expected_package = True
					self._print_on_reception(package, expected_package)
					self._request_number += 1
					self._send_acknowledgment(package, NEW_ACK)

				elif package.sequence_number < self._request_number:
					self._send_acknowledgment(package, TIMEOUT_ACK)
				
	def _send_acknowledgment(self, package, reason):
		package.request_number = self._request_number
		random_number = random.uniform(0, 1)
		if self._packet_loss_probability >= random_number:

			self._ack_queue.enqueue(package)
			self._print_on_ack(package, reason)
		else:
			# Packet loss
			time.sleep(self._timeout + random_number)
			self._ack_queue.enqueue(package)