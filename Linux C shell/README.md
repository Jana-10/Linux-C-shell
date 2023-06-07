#### Information
- Max length of string commands is 5000 characters (can be changed in headers.h)
- Memory in pinfo is shown in KB
- CTRL-D / quit command are used to exit the shell safely
- CTRL-C / CTRL-Z signal are not implemented
- Redirection handled 
- Piping is handled


#### Brief documentation
- shell.c 
    - contains the main while loop
    - has some signal handlers
    - prints the cwd and hostname 
- parser.c
    - breaks down the input and parses it as required
    - supports &, ;, spaces, <, >, >> parsing
- ls.c
    - contains implementation for ls command
- pinfo.c 
    - contains implementation for pinfo command
- kill.c 
    - contains implementation for for handling children processes which have become zombies
- util.c 
    - contains implementation for frequently used functions like handling ~, min, max, color output etc
    - contains implementation for cd and echo too as the implementation is trivial
    - cd supports - (previous working directory)
- headers.h
    - contains most commonly used headers
- process_maker.c
    - handles creating child processes in both background and foreground to run different executables 
    - Also takes care of job related things, like adding & removing jobs
- history.c
    - contains implementation for the history command which stores upto 20 commands


#### To Run
- make
- ./run



