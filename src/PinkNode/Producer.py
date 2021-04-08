from Queue import Queue
from threading import Thread
from threading import Event
import time

class Producer():

	def __init__(self, packages_to_send, initial_value, packet_loss_probability, timeout, printer):

		# Command line arguments.
		self._packet_loss_probability = packet_loss_probability
		self._packages_to_send = packages_to_send
		self._initial_value = initial_value - 1
		self._timeout = timeout

		# Threads and queues.
		self._sender = None
		self._receiver_queue = None

		self._ack_queue_listener = None
		self._ack_shared_queue = Queue() 
		self._ack_local_queue = Queue()
		self._ack_queue_listener_work = True

		# Other variables.
		self._sequence_number = 0
		self._timeout_event = Event()
		self._printer = printer

	def get_acknowledgment_queue(self):
		return self._ack_shared_queue

	def set_receiver_queue(self, queue):
		self._receiver_queue = queue

	def start(self):
		self._sender = Thread(target = self._run_sender)
		self._sender.start()98888888
		self._ack_queue_listener = Thread(target = self._run_ack_queue_listener)
		self._ack_queue_listener.start()

	def wait_to_finish(self):
		self._sender.join()
		self._ack_queue_listener.join()

	def _run_sender(self):
		packages_sent = 0

		while packages_sent < self._packages_to_send:

			# It creates and sends a package for the first time.
			package = Package(self._sequence_number, 0, self._accept_package_from_higher_layer())
			self._receiver_queue.enqueue(package)
			self._print_on_sending(package, NEW)

			# It retransmits the package until the acknowledgment is received.

			was_ack_received = self._timeout_event.wait(self._timeout)

			while was_ack_received == False:
				was_ack_received = self._timeout_event.wait(self._timeout)
				self._receiver_queue.enqueue(package)
				self._print_on_sending(package, RET)

			# The acknowledgment was received.
			ack_package = self._ack_local_queue.dequeue()

			if ack_package.request_number > self._sequence_number:
				self._sequence_number = ack_package.request_number

			elif ack_package.request_number < self._sequence_number:
				# Retransmit by wrong ack.
				self._receiver_queue.enqueue(package)
				self._print_on_sending(package, ACK)

			packages_sent += 1
		
		# It sends termination package to consumer to end the simulation.
		terminating_package = Package(-1, 0, -1)
		self._receiver_queue.enqueue(terminating_package)
		self._ack_queue_listener_work = False

	def _run_ack_queue_listener(self):

		# It waits on internal sempahore until an acknowledgment arrives.
		acknowledgment_count = 0

		while acknowledgment_count < self._packages_to_send:

			package = self._ack_shared_queue.dequeue()
			self._print_ack_received(package)
			self._ack_local_queue.enqueue(package)
			self._timeout_event.set()
			acknowledgment_count += 1

	def _accept_package_from_higher_layer(self):
		self._initial_value += 1
		return self._initial_value