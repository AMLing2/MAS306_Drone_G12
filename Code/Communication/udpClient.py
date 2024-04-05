import socket

localIP = "127.0.0.1"
localPort = 20002

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

	def recv(self):
			return self.sock.recv(self.buffersize)

	def sendto(self,msg):
		self.sock.sendto(msg,(localIP,20001))
	
mysocket = GenSocket()
initStr = "hi1"
msgBytes = str.encode(initStr)
mysocket.sendto(msgBytes)
while(1):
	print((mysocket.recv()))
