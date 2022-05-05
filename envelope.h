#include"portaudio.h"
#include<stdio.h>
#include<math.h>
#include <sys/time.h>
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 441

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

typedef struct{
    float duration;
    float totalDuration;
    float value;
    int tension;
    //calculate multiplier based on values after being given them, makes getting tension value faster
    float multiplier;
}envelopePoint;
typedef struct{
    envelopePoint attack;
    //duration of hold
    float hold;
    envelopePoint decay;
    float sustain;
    envelopePoint release;
}envelope;
float calculateMultiplier(float startX, float startY, float endX, float endY, int tension){
    float base = (float)tension;
    if (base < 0){
        base = ((float)(-1*tension))*(0.01);
    }
    if (startY == endY || startX == endX){
        return -1;
    }
    else{
        float offsetX = (endX-startX)/1000;
        float offsetY = (endY-startY);
        return (float)(offsetY)/(pow(base, offsetX)-1);
    }
}
void calculateMultiplierEnvolope(envelope *currEnvelope){
    envelopePoint *attack = &(currEnvelope->attack);
    envelopePoint *decay = &(currEnvelope->decay);
    envelopePoint *release = &(currEnvelope->release);
    (attack->multiplier) = calculateMultiplier(0, 0, attack->duration, attack->value, attack->tension);
    printf("new attack multiplier:%f, from %f\n ", (currEnvelope->attack).multiplier, attack->multiplier);
    decay->multiplier = calculateMultiplier(attack->duration, attack->value, decay->totalDuration, decay->value, decay->tension);
    release->multiplier = calculateMultiplier(decay->totalDuration, currEnvelope->sustain, release->totalDuration, release->value, release->tension);
}
float applyTension(float startX, float startY, float endX, float endY, int tension, float multiplier, float posX){
    //get curve by starting from 0,0
    float base = (float)tension;
    if (base < 0){
        base = ((float)(-1*tension))*(0.01);
    }
    if (startY == endY || startX == endX){
        return startY;
    }
    else if(base == 1){
        return startY + (endY - startY)*((posX-startX)/(endX-startX));
    }
    else{
        return multiplier*pow(base, ((posX-startX)/1000))+(-1*multiplier)+startY;
    }
}
float getEnvolopeValue(envelope currEnvelope, float noteDuration){
    //linear
    if (noteDuration < currEnvelope.attack.duration){
        //printf("a,%f\n", noteDuration);
        return applyTension(0, 0, currEnvelope.attack.duration, currEnvelope.attack.value, currEnvelope.attack.tension, currEnvelope.attack.multiplier, noteDuration);
    }
    else if (noteDuration <= (currEnvelope.decay.totalDuration)){
        //printf("d\n");
        return applyTension(currEnvelope.attack.duration, currEnvelope.attack.value, currEnvelope.decay.totalDuration, currEnvelope.decay.value, currEnvelope.decay.tension, currEnvelope.decay.multiplier, noteDuration);
    }
    else{
        //printf("s\n");
        return currEnvelope.sustain;
    }
}
envelope initializeTestEnvelope(){
    envelope envelopeTest;
    envelopeTest.attack.duration = 10;
    envelopeTest.attack.value = 1;
    envelopeTest.attack.tension = 1;
    envelopeTest.decay.value = 1;
    envelopeTest.decay.duration = 200;
    envelopeTest.decay.tension = 1;
    envelopeTest.sustain = 1.0;
    envelopeTest.decay.totalDuration = envelopeTest.attack.duration + envelopeTest.decay.duration;
    calculateMultiplierEnvolope(&envelopeTest);
    return envelopeTest;
}
static void setEnvelopeAttack(envelope *self){

}
static void setEnvelopeDelay(envelope *self){

}
static void setEnvelopeRelease(envelope *self){

}