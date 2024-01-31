#!/bin/env python3
from scapy.all import *

print("Sniffing for packetsat dst host...")

def print_packet(packet):
   print(f"Packet data is: {packet.show()}")
   print("\n")
   print(f"Packet summary is: {packet.summary()}")
   print("\n")#

''' filter by dst IP '''
sniff(filter="dst host 192.168.0.2", prn=print_packet, timeout=15)