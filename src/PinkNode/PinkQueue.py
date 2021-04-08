from Semaphore import Sempahore
import queue
from threading import Lock

# A thread-safe queue for producer-consumer pattern.
class PinkQueue:
    def __init__(self):
        self._queue = queue.Queue()
        self._consume = Sempahore(0)
        self._lock = Lock()

    def push(self, element):
        self._lock.acquire()
        self._queue.put(element)
        self._lock.release()
        self._consume.signal()

    def pop(self):
        self._consume.wait()
        self._lock.acquire()
        element = self._queue.get()
        self._lock.release()
        return element
