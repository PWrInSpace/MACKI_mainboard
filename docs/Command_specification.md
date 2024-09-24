# Mainboard <=> MACKI App commands specification

## Command format
The command format is as follows:

```
<command> <arg1> <arg2> ... <argN>
```

Where the arguments are separated by a space character. The EOF is "\r\n".

There are no optional arguments. The number of arguments is fixed for each command.

## Commands

There are two subgroups of commands. The first group are the procedure commands. The second group are the "everything else" commands, which will be more 'freely' used.

Both the MACKI_app as well as Mainboard will be responsible for checking the correctness of the command arguments. 

### Command acknowledgement

The Mainboard will acknowledge the reception of a command by sending a response. The response is a single line of text, which is either "OK: 'optional string' \r\n" or "ERR: 'optional string'\r\n".

### Communication interfaces

**UART** - Commands + responses are sent over UART. The baudrate is 115200.
**USB** - Logs are sent over USB. Communication here is uni-directional.