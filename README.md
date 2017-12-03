# sorterPart3

To run you do wanna compile both. To compile server do: gcc server.c -o server    To compile client do: gcc client.c -o client
Then to run you want to have 2 different terminals open, and on one do ./server -p <port number>
In the other terminal, you want to do ./client -c <col name> -h <hostname> -p <port number>
If your client is running on the same ilab machine, then the hostname would be localhost
