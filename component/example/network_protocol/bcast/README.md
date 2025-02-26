# Example Description

This example will listen broadcast message on port 49152.

Send packet with the content of received packet to broadcast address.

# HW Configuration

None

# SW configuration

How to use:
   - Run `./build.py -a bcast` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

A broadcast example thread will be started automatically when booting.

To test:
1. Prepare a NB and connect to the same AP Ameba connected.
2. NB: iperf -c 192.168.1.255 -t 60 -i 1 -p 49152 -u
3. The recv/send messages should be printed out on Ameba console.
4. Use sniffer to make sure the packets send from Ameba are broadcast messages.

# Note

If you encounter some message like:
```
ERROR: sendto broadcast
[Driver]: skb_unavailable=1 in last 2 seconds
```
It means that the skb buffer is not enough for the massive UDP packets to be sent.

If you want to prevent the error you can add some delay time between sending packets or enlarge the skb buffer configuration.

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
