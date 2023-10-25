# multish - create multiple lines of shell command line using a configuration file

# Introduction

Say you need to run a command four times like so:

```
sshin2rmd.exp -h 172.19.15.21 -u admin -p [RMDPASS] -c rmdcmds.cmd
sshin2rmd.exp -h 172.19.15.22 -u admin -p [RMDPASS] -c rmdcmds.cmd
sshin2rmd.exp -h 172.19.15.23 -u admin -p [RMDPASS] -c rmdcmds.cmd
sshin2rmd.exp -h 172.19.15.24 -u admin -p [RMDPASS] -c rmdcmds.cmd
```

You could put these in a file, make it executable and run it something like.

```
allfour.sh
```

But a neater way is to create a file called:

```
00Multish.conf
```

Put this content in it:

```
#
# 00Multish.conf
#
# for logging onto multiple RMDs
#

-h              -u              -p              -c

172.19.15.21  admin           [RMDPASS]       rmdcmds.cmd
172.19.15.22
172.19.15.23
172.19.15.24
```

and then run:

```
multish sshin2rmd.exp | sh
```

To see what is going on run:

```
multish
```

That should get you started.

More documentation and examples when I have time.

------------------------------------
End of README.md
