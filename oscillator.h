#include"portaudio.h"
#include<stdio.h>
#include<math.h>
#include <sys/time.h>
#include"note.h"
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 441
#define MAX_UNISON 16
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

typedef struct{
    int oscillatorType;
    float detune;
    int unison;
    float unisonFrequencies[MAX_UNISON];
    float unisonLevels[MAX_UNISON];
} oscillatorData;

float getSinValue(float wavePosition){
    return (float)sin((double)wavePosition*(2*M_PI));
}
float getSawValue(float wavePosition){
    //wavePosition in percentage of wave
    float value = wavePosition*2;
    if (wavePosition > 0.5){
        value -= 2;
    }
    //printf("saw value:%f\n", value);
    return value;
}
float getSquareValue(float wavePosition){
    if (wavePosition > 0.5){
        return 1;
    }
    else{
        return -1;
    }
}
/*static void setDetune(oscillatorData *currOscillator, float newDetune){
    if (newDetune < 1){
        currOscillator->detune = newDetune;
        //calculate unison frequencies
    }
}
static void setUnison(oscillatorData *currOscillator, int newUnison){
    for (int i = 0; i < MAX_UNISON; i++){
        currOscillator->unisonFrequencies[i] = -1;
        currOscillator->unisonLevels[i] = -1;
    }
    if (!(newUnison > 16)){
        currOscillator->unison = newUnison;
        //calculate unison frequencies
        if (newUnison %2 == 0){
            for (int i = 1; i < (int)(newUnison/2); i++){
                float detuneModifier = currOscillator->(float)detune/i;
                currOscillator->unisonFrequencies[i] = detuneModifier;
                currOscillator->unisonFrequencies[(int)(newUnison/2)+i] = -1*detuneModifier;
                if (i == 1){
                    currOscillator->unisonLevels[i] = 1;
                    currOscillator->unisonLevels[(int)(newUnison/2)+i] = 1;
                }
                else{
                    currOscillator->unisonLevels[i] = 0.5;
                    currOscillator->unisonLevels[(int)(newUnison/2)+i] = 0.5;
                }
            }
        }
        else{
            currOscillator->unisonLevels[0] = 1;
            currOscillator->unisonFrequencies[0] = 0;
            for (int i = 1; i < (int)(newUnison-1)/2; i++){
                float detuneModifier = currOscillator->(float)detune/i;
                currOscillator->unisonFrequencies[i] = detuneModifier;
                currOscillator->unisonFrequencies[((int)(newUnison-1)/2)+i] = -1*detuneModifier;
                currOscillator->unisonLevels[i] = 0.5;
                currOscillator->unisonLevels[((int)(newUnison-1)/2)+i] = 0.5;
            }
        }
    }
}*/
void updateUnison(oscillatorData *currOscillator, int newUnison, float newDetune){
    currOscillator->detune = newDetune;
    for (int i = 0; i < MAX_UNISON; i++){
        currOscillator->unisonFrequencies[i] = -1;
        currOscillator->unisonLevels[i] = -1;
    }
    if (newUnison < 16){
        currOscillator->unison = newUnison;
        if (newUnison %2 == 0){
            for (int i = 1; i < (int)(newUnison/2); i++){
                float detuneModifier = pow(2, (currOscillator->detune/i)/12);    //assuming semitone
                currOscillator->unisonFrequencies[i] = detuneModifier;
                currOscillator->unisonFrequencies[(int)(newUnison/2)+i] = -1*detuneModifier;
                if (i == 1){
                    currOscillator->unisonLevels[i] = 1;
                    currOscillator->unisonLevels[(int)(newUnison/2)+i] = 1;
                }
                else{
                    currOscillator->unisonLevels[i] = 0.5;
                    currOscillator->unisonLevels[(int)(newUnison/2)+i] = 0.5;
                }
            }
        }
        else{
            currOscillator->unisonLevels[0] = 1;
            currOscillator->unisonFrequencies[0] = 0;
            for (int i = 1; i < (int)(newUnison+1)/2; i++){
                float detuneModifier = pow(2, (currOscillator->detune/i)/120); //assuming cents
                printf("detune modifier:%f, %f\n", detuneModifier, currOscillator->detune);
                currOscillator->unisonFrequencies[i] = detuneModifier;
                currOscillator->unisonFrequencies[((int)(newUnison-1)/2)+i] = -1*detuneModifier;
                currOscillator->unisonLevels[i] = 0.5;
                currOscillator->unisonLevels[((int)(newUnison-1)/2)+i] = 0.5;
            }
        }
    }
}
float getOscillatorValue(int oscillatorType, float wavePosition){
    //add more complexity later (unison, etc)
    if (oscillatorType == 0){
        return getSinValue(wavePosition);
    }
    else if (oscillatorType == 1){
        return getSawValue(wavePosition);
    }
    else{
        return getSquareValue(wavePosition);
    }
}
float playNote(oscillatorData *currOscillator, note *currNote, int currPos){
    //apply for each unison
    float returnValue = 0;
    for (int i = 0; i < currOscillator->unison; i++){
        returnValue += getOscillatorValue(currOscillator->oscillatorType ,getWaveTablePositionModified(currNote, currOscillator->unisonFrequencies[i], currPos));
        //printf("frequencies: %f\n", currOscillator->unisonFrequencies[i]);
        returnValue *= currOscillator->unisonLevels[i];
        //printf("unison level: %f, unisonValue: %f\n", currOscillator->unisonLevels[i], currOscillator->unisonFrequencies[i]);
    }
    //printf("returnvalue:%f\n", returnValue);
    //printf("outputValue:%f\n", (returnValue / currOscillator->unison));
    //printf("unison amount:%d\n", currOscillator->unison);
    return (returnValue / currOscillator->unison);
}
void printUnisonValues(oscillatorData *currOscillator){
    for (int i = 0; i < currOscillator->unison;i++){
        printf("frequencies: %f, unisonLevel: %f\n", currOscillator->unisonFrequencies[i], currOscillator->unisonLevels[i]);
    }
}
oscillatorData initOscillator(void){
    printf("creating oscillator\n");
    oscillatorData newOscillator;
    newOscillator.oscillatorType = 1;
    updateUnison(&newOscillator, 1, 1);
    printf("finished creating oscillator\n");
    return newOscillator;
}