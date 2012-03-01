#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
#
# Copyright: See COPYING file that comes with this distribution
#
#

import socket, re, sys

UDPSock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
UDPSock.setsockopt(socket.SOL_SOCKET,socket.SO_BROADCAST,1)
UDPSock.setblocking(1)
UDPSock.settimeout(3)

UDPSock.bind( ('', 42314) )

print "Autodiscover TellStick Net..."
UDPSock.sendto("D", ('255.255.255.255',30303))

p = re.compile('(.+):(.+):(.+):(.+)')
ip = None

while 1:
	try:
		(buf, (ip, port)) = UDPSock.recvfrom(2048)
	except socket.error, msg:
		break
	m = p.match(buf)
	print "Found %s on ip %s firmware version %s" % (m.group(1), ip, m.group(4))
	if (m.group(4) == 'X'):
		print "  This has firmware X, lets use it"
		break
	else:
		print "  Not using correct firmware"
	ip = None

if (ip == None):
	print "No TellStick Net with correct firmware found"
	sys.exit(1)

print "Send Arctech code switch A1 turn on to %s" % ip
protocol = '$k$k$k$k$k$k$k$k$k$k$k$k$k$k$k$k$k$k$kk$$kk$$kk$$k' #On
#protocol = '$k$k$k$k$k$k$k$k$k$k$k$k$k$k$k$k$k$k$kk$$kk$$k$k$k' #Off
msg = "4:sendh1:S%X:%ss" % (len(protocol), protocol)
UDPSock.sendto(msg,(ip,42314))

sys.exit(0)