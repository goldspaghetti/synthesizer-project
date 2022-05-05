#include"portaudio.h"
#include<stdio.h>
#include<math.h>
#include <sys/time.h>
#include<stdbool.h>
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 441

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
typedef struct{
    int noteValue;  //from C4
    int currPos;
    float frequency;
    float samplePerWave;
    bool letGo;
    struct timeval startTime;
    struct timeval endTime;
}note;
typedef struct{
    float frequency;
    float samplePerWave;
    int currPos;
}frequencyData;
note getNote(int noteDistance){
    note currNote;
    currNote.frequency = (float)(261.625)*powf(2, (float)noteDistance/12);
    currNote.noteValue = noteDistance;
    currNote.currPos = 0;
    currNote.samplePerWave = SAMPLE_RATE/currNote.frequency;
    currNote.letGo = 0;
    gettimeofday(&(currNote.startTime), NULL);
    return currNote;
}
float getWaveTablePositionModified(note *currNote, float frequencyModifier, int currPos){ //returns wavePosition (between 0 and 1)
    float samplePerWave = SAMPLE_RATE/(currNote-> frequency * frequencyModifier);
    //printf("samplePerWave:%f, %f\n", samplePerWave, currNote->samplePerWave);
    //printf("wavetable pos:%f\n", fmod(currPos, samplePerWave)/samplePerWave);
    return fmod(currPos, samplePerWave)/samplePerWave;
}
