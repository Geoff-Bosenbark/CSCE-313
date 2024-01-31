from scapy.all import *
import time

def main():
    """Driver function"""
    while True:
        print_menu()
        option = input('Choose a menu option: ')
        if option == '1':
            print("Creating and sending packets ...")
            number = int(input("Enter the number of packets to send: "))
            interval = int(input("Enter the number of seconds between each packet: "))
            send_pkt(number, interval)
        elif option == '2':
            print("Listening to all traffic to 8.8.4.4 for 1 minute ...")
            sniff(filter="dst host 8.8.4.4", timeout=60, prn=print_pkt)
        elif option == '3':
            print("Listening continuously to only ping commands to 8.8.4.4 ...")
            sniff(filter="icmp and dst host 8.8.4.4", prn=print_pkt)
        elif option == '4':
            print("Listening continuously to only outgoing telnet commands ...")
            sniff(filter="tcp and dst port 23", prn=print_pkt)
        elif option == '5':
            print("End")
            break
        else:
            print(f"\nInvalid entry\n")

def send_pkt(number, interval):
    """Send a custom packet"""
    packet = Ether(src="00:11:22:33:44:55", dst="55:44:33:22:11:00") / \
             IP(src="192.168.10.4", dst="8.8.4.4", ttl=26) / \
             TCP(sport=23, dport=80) / \
             Raw(load="Teamup Apps for Good: https://teamup.org/")
    for _ in range(number):
        sendp(packet)
        time.sleep(interval)

def print_pkt(packet):
    """Print Packet fields"""
    src_ip = packet[IP].src if IP in packet else 'N/A'
    dst_ip = packet[IP].dst if IP in packet else 'N/A'
    proto = packet[IP].proto if IP in packet else 'N/A'
    ttl = packet[IP].ttl if IP in packet else 'N/A'
    length = len(packet)
    payload = packet[Raw].load if Raw in packet else 'N/A'
    print(f"Source IP: {src_ip}\nDestination IP: {dst_ip}\nProtocol number: {proto}\nTTL: {ttl}\nLength in bytes: {length}\nRaw Payload: {payload}\n\n")

def print_menu():
    """Prints the menu of options"""
    print("*******************Main Menu*******************")
    print('1. Create and send packets')
    print('2. Listen to all traffic to 8.8.4.4 for 1 minute')
    print('3. Listen continuously to only ping commands to 8.8.4.4')
    print('4. Listen continuously to only outgoing telnet commands')
    print('5. Quit')
    print('***********************************************\n')

if __name__ == "__main__":
    main()