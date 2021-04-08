import threading

class Sempahore():
	def __init__(self, value):
		self._semaphore = threading.Semaphore(value)

	def wait(self):
		self._semaphore.acquire()

	def signal(self):
		self._semaphore.release()