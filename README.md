# C Compiler written in... C!

Learning about compiler theory and how it would relate to the process of writing a decompiler! 

Following this guide for the general process, which is broken into steps without code for how to parse C into assembly: https://norasandler.com/2017/11/29/Write-a-Compiler.html


## Installion

Its about as easy as it gets - clone the repo, compile it with 'make',
and run the program with ./compiler \<file to compile>

Once the .s file is generated, use gcc to assemble and link the .s file.
(gcc -m32 \<.s file> -o \<executable name> )
