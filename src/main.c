//
//  main.c
//  BuddyBox
//
//  Created by Nicholas Robinson on 12/23/12.
//  Copyright (c) 2012 Nicholas Robinson. All rights reserved.
//

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "PortAudioStream.h"
#include "BuddyBox.h"

static const unsigned int DEFAULT_SAMPLE_RATE = 124000;

static unsigned int running = 1;

void intHandler(int sig) {
    running = 0;
}

int main(int argc, const char * argv[])
{
    PortAudioStream pas;
    BuddyBox bb;
    unsigned int sampleRate, i;
    
    sampleRate = (argc > 1) ? (unsigned int) strtol(argv[1], NULL, 0) : DEFAULT_SAMPLE_RATE;
    
    signal(SIGKILL, intHandler);
    signal(SIGINT, intHandler);
    
    initializePortAudioStream(&pas, sampleRate);
    
    while(running)
    {
        initializeBuddyBox(&bb, sampleRate);
        
        while(running && bb.active && readPortAudioStream(&pas))
        {
            readBufferIntoBuddyBoxInputChannelBuffer(&bb, pas.bufferedSamples, pas.bufferSize);
            
            bb.outputChannelCount = 9;
            for (i = 0; i < bb.outputChannelCount; i++)
                setBuddyBoxOutputChannelDuration(&bb, i, rand() % 1200 + 500);
            
            writeBuddyBoxOutputChannelBufferIntoBuffer(&bb, pas.bufferedSamples, pas.bufferSize);
            
            writePortAudioStream(&pas);
        }
        
        sleep(1);
    }
    
    closePortAudioStream(&pas);
    
    printf("Program Halted...\n");
}

