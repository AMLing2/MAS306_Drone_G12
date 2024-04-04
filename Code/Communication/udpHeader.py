import socket

localIP = "127.0.0.1"
localPort = 20001

class GenSocket:
	buffersize = 1024
	def __init__(self):
		self.sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) #unsure if using AF_INET
		self.sock.bind((localIP,localPort))

	def connect(self,addressPort):
		self.sock.connect(addressPort)

	def gethostname(self):
		self.sock.getsockname()

	def getclient(self):
		return self.sock.recvfrom(self.buffersize)

	def send(self,msg):
		self.sock.send(msg)
	
mysocket = GenSocket()
#mysocket.connect(localIP,localPort)
print(mysocket.gethostname)
clientRecieve = mysocket.getclient()
print(clientRecieve)
clientAddress = clientRecieve[1]
mysocket.connect(clientAddress)
