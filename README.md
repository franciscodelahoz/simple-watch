# Other Watch

I started this project as a challenge to create my own CLI utilities in C. The goal was to emulate the functionality of the watch utility. 
While it works well for programs that complete their execution, it encounters issues with those that run continuously in the background, refreshing the terminal.

In the future, I hope to revisit this project to address one of two challenges: either remove the Vim-style pagination—currently designed to wait for programs to finish 
execution before adding lines to the scroll buffer—or develop a solution that retains pagination while effectively handling programs 
that never terminate, preventing infinite growth in the buffer.

Additionally, I aim to clean up and streamline the codebase to improve its readability.
