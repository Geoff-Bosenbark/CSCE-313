from scapy.all import Ether, IP, TCP, sendp

# Define the Ethernet frame
eth = Ether(src='00:11:22:33:44:55', dst='55:44:33:22:11:00', type=0x0800)

# Define the IP packet
ip = IP(src='192.168.0.1', dst='192.168.0.2', proto='tcp')

# Define the TCP segment
tcp = TCP(sport=1234, dport=80, chksum=0)

# Define the payload
payload = b'Packet crafting is neat!'

# Combine everything into a single packet
packet = eth/ip/tcp/payload

# Print the packet summary
print(packet.show())

# Send the packet on the network
sendp(packet)