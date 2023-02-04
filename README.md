# unix_shell
  Basic unix shell. Works by forking processes for commands to execute. Supports the following features - 
* **history** - implemented based on most used commands.
<img src = "https://user-images.githubusercontent.com/95434969/216759590-b3e42cfe-3c7e-4a3c-b0b5-824535e7b3a1.png" width = "800">

* **& for background** - append the end of the command with & to run the process in background. (proces becomes zombie after execution)
<img src = "https://user-images.githubusercontent.com/95434969/216759678-4066e2ee-4898-4323-af8b-9bbad3c03342.png" width = "500">

* **Signal Handling** - to terminate child processes using Ctrl - C instead of exiting parent process. (process becomes zombie after interrupting)
<img src = "https://user-images.githubusercontent.com/95434969/216759755-65198574-1684-415b-9ac8-f8964dfb1011.png" width = "500">

* **IO** redirection to files using **>** and **<** operands. Supports only one of these in a command currently.
<img src = "https://user-images.githubusercontent.com/95434969/216759831-042b48e2-e5fb-4251-a8f7-3870bcbb8641.png" width = "500">

* **pipeline** feature to redirect the output of one command as input to another.
<img src = "https://user-images.githubusercontent.com/95434969/216759887-c49f13c9-cb20-4d1b-b3c6-849a9721dd0c.png" width = "500">
