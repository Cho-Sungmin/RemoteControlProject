# RemoteControlProject

  ▶ RemoteControl v2.0.0 is a personal project to improve skills.<br>
  ▶ I've tried to understand technic what I used and why it's useful, when I use it.<br>
  
# Support Function
▶Remote client, which I call 'customer', offers its screen to another client, which I call 'host'.<br>
The host could control customer's mouse pointer and keyboard but, it doesn't work perfectly.<br>
Server support p2p mode and relay mode. It's possible to connect on these 2 modes in parallel.<br>
In addition, I made an application for smart phone. It is like the'host' client.<br>

< p.s I couldn't test well on relay mode because of limited environment... <br>
      This project might work well without optimize option in VS20xx ><br>

# I used...
  C, C++, MFC, Android, Java<br>
  
# Version Information
  ▶v1.x.x is based on udp protocol. It includes udp server and 2 clients.<br>
  ▶v2.0.0 has different sever which is based on IOCP.<br>
  Actually, I know that IOCP is not appropriate for p2p but, I just wanna implement the server using it.<br>
  And it support relay mode so, I think, in this case, it's quite meaningful.<br>
