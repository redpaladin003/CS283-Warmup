1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_The remote client typically detects the end of a command's output by reading until it encounters a special delimiter or has read the expected number of bytes. Because TCP is a stream protocol, partial reads can occur, so the client must keep reading until the entire message is received. A common technique is to send a unique end-of-message marker that tells the client when the output is complete. The server can prefix the output with its length, and the client will read exactly that many bytes. Proper handling of partial reads ensures no data is lost or truncated_

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_Because TCP is a stream protocol, a networked shell must define explicit message boundaries using techniques like a length header or a unique delimiter. Without such boundaries, multiple commands can become interleaved, resulting in corrupted or merged data. Delimiter-based protocols read until a special character is encountered, while length-prefixed protocols read a fixed number of bytes. If the boundaries are not enforced, commands can be partially consumed or appended to subsequent commands_

3. Describe the general differences between stateful and stateless protocols.

_A stateful protocol remembers information about past interactions, so each request can rely on data from previous requests. A stateless protocol, however, treats every request as independent and does not retain context between them. Stateful designs often simplify complex interactions at the expense of scalability and resilience. Stateless designs are easier to scale horizontally and recover from failures. Many modern protocols adopt stateless principles with optional mechanisms for session management_

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_UDP is chosen when speed and reduced overhead are more critical than guaranteed delivery, such as for real-time video, gaming, or voice streaming. It does not perform connection setup or maintain ordering, allowing data to flow without delay. Applications that can tolerate occasional packet loss can benefit from UDP's simplicity. Broadcasting and multicasting are also simpler with UDP. Consequently, UDP is ideal for scenarios that prioritize performance and scalability over reliability_

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_The operating system provides the socket API as the fundamental abstraction for network communication. Sockets allow applications to create endpoints, bind to addresses, and send or receive data over various transport protocols. This abstraction hides low-level details of the underlying networking hardware. Developers can perform connection-based or connectionless communication through the same interface. As a result, sockets form the backbone of most modern networked applications_
