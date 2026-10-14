This is a school project that I had to develop from scratch in 8 weeks.

rECR (rECR executes commands recursively) does what it says on the tin. rECR is a recursive wrapper for any arbitrary command. The Program is executed in the Linux Terminal and takes a command string along with other parameters and then executes that command recursively throughout a directory.

YOU ARE RESPONSIBLE FOR KNOWING HOW THE COMMAND THAT YOU WANT TO RUN RECURSIVLY WORKS

rECR makes no atempt to safe guard or handle errors from the commands it runs, it just runs them.

Due to the nature of being able to run any linux application as a part of this program there will likely be a lot of edge cases. Feel free to submit bug reports to the Github or attempt to fix them yourself. I <3 open source.

rECR [OPTIONS] [COMMAND STRING] [path to directory] NAME0=""
EXAMPLES:
    rECR "cp -v FILE.PATH0/FILE.NAME0 FILE.PATH0/FILE.NAME1" NAME0="*.txt"  NAME1="moved*"
        copies all .txt files and appends "moved" to there filename
    rECR "cat FILE.PATH0/FILE.NAME0" NAME0="*.sh"
        displays the content of all .sh files
    rECR -f "cat FILE.PATH0/FILE.NAME0 " | grep -i "lorem"
        same as the last example but searching using grep
    rECR -d "echo FILE.PATH0/FILE.NAME0"
        displays every files filepath

Options:
    -d      run command on directories as well as files
    -f       ignore warning message, useful for use with grep
    -a      run on hidden files and directories

Using the wildcard in the NAME1 or greater will append NAME1 to the file's name ignoring file types.
    EXAMPLE: NAME1="Moved*" NAME0="*.txt" output will be Moved<name of file>.txt

Included is a Test Dir to play around with instead of potentally ruining the rest of your file system.
