import socket as soc

s = soc.socket(soc.AF_INET, soc.SOCK_STREAM)
s.connect(('localhost', 23456))

while True: print 'hi'
