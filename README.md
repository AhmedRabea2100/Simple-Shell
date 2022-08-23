# Simple-Shell
A simple shell using the C language.
# Design Decision
At the start of execution the shell directory will be set to the directory where the source code is placed.
# Main Functions 
### Evaluation
Checks if there is a variable and swaps it with variable value.
![image](https://user-images.githubusercontent.com/95850756/186195702-1108fc98-c55d-4606-85f4-9733dc464f46.png).

### Execute
Checks if the entered command is `exit`, `echo`, `cd`… and so on.
![image](https://user-images.githubusercontent.com/95850756/186196064-845dbeb5-859f-4e8a-a5f4-564cfc2d0880.png)

### Execute commands independent of shell
Forks another process that executes the required command (either in foreground orbackground).
![image](https://user-images.githubusercontent.com/95850756/186196312-a670203c-5a23-45e6-a948-a7ed5621ddb7.png)

### Shell
Contains the main cycle repeated until user types `exit`.
![image](https://user-images.githubusercontent.com/95850756/186196633-05ebd9bb-3a65-4fb0-a681-650ce6ab4e80.png)


