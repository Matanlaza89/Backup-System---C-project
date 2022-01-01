<h2><u>Backup System - Final project in a real-time operating systems course using C & Linux</u></h2>
<p>
A customized information backup system according to the API list requirements using the tools I learned that included working XXXXXs.
</p>
<h4><u>User manual:</u></h4>
<p>
- 1. In order to use run the system the following items are needed: s.c, c.c, functions.c, functions.h, image.txt<br>
- 2. There are two more text files for checking the backup system: text.txt, doron.txt<br>
- 3. You should keep all the files mentioned earlier in the same directory<br>
- 4. When compiling use the following commands (including Math library):<br>
  * gcc s.c functions.c functions.h -lm -o server<br>
  * gcc c.c functions.c functions.h -lm -o client<br>
- 5. Open two terminals - you can use more than two if you want.<br>
- 6. On the first terminal write the following line: ./server ipv4 / ipv6 / domain<br> 
 choose one of the options ,this line must include two arguments , the second argument configures the protocol.<br>
- 7.On the second terminal write the following line: ./client ipv4 / ipv6 / domain test  or  - ./client ipv4 / ipv6 / domain doron<br>
 choose one of the options ,this line must include three arguments, the second argument configures the protocol and the third argument is the name of the file without extension.<br>
- 8. Both the server and the client must be configured with the same protocol in order to work together.
</p>
<h1><u>API List</u></h1>
<p>
- 1. Working with a number of arguments - agrc , argv <br>
- 2. Work with a null terminated strings: strcpy , strcat ,strlen , strstr, strcmp <br>
- 3. FILE API - C style <br>
- 4. FILE API - Unix stlye <br>
- 5. Working with files using mmap <br>
- 6. IPv4<br>
- 7. IPv6<br>
- 8. Unix domain sockets <br>
- 9. Stream sockets<br>
- 10. Fork<br>
- 11. Math library<br>
</p>
