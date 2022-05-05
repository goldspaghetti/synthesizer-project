#include"portaudio.h"
#include<stdio.h>
#include<math.h>
#include <sys/time.h>
#include"envelope.h"
#include"oscillator.h"
//#include"note.h"  //included in oscillator.h
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 441

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
//#define SIN_TABLE_LEN ((int)SAMPLE_RATE/FREQUENCY)
#define SIN_TABLE_LEN 5000
#define OSCILLATOR_NUM 3
#define MAX_NOTES 5

typedef struct{
    float leftPhase;
    float rightPhase;
    envelope testEnvolope;
    note notesPlaying[MAX_NOTES];
    oscillatorData oscillators[OSCILLATOR_NUM];
}combinedData;

static combinedData combined;
static PaStream *stream;
static PaError err;

static int callbackTest(const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData ){
    float *out = (float*)outputBuffer;
    combinedData *combinedOscillators = (combinedData*) userData;
    struct timeval currTime;
    gettimeofday(&currTime, NULL);
    //unsigned long timeDifference = (currTime.tv_sec - curr)
    for (unsigned long i = 0; i < framesPerBuffer; i++){
        float value = 0;
        for (int j = 0; j < MAX_NOTES; j++){
            float currNoteValue = 0;
            note *currNote = &(combinedOscillators->notesPlaying[j]);
            if (currNote->frequency == 0){
                continue;
            }
            if (currNote->samplePerWave == 0){
                continue;
            }
            //float wavePosition = (float)currNote->currPos/currNote->samplePerWave;
            for (int k = 0; k < OSCILLATOR_NUM; k++){
                currNoteValue += playNote(&(combined.oscillators[k]), currNote, currNote->currPos);
                //printf("currNoteValue:%f\n", playNote(&(combined.oscillators[k]), currNote));
            }
            float timeDifference = ((float)(currTime.tv_sec - currNote->startTime.tv_sec) * 1000000 + (currTime.tv_usec - currNote->startTime.tv_usec))/1000;
            //in milliseconds
            currNoteValue *= getEnvolopeValue(combinedOscillators->testEnvolope, timeDifference);
            currNote->currPos++;
            value += (currNoteValue / OSCILLATOR_NUM);
        }
        value /= MAX_NOTES;
        //for now, ears are dead
        value /= 5;
        if (value > 0.2 || value < -0.2){
            value = 0.2;
        }
        //printf("valud:%f\n", value);
        combinedOscillators->leftPhase = value;
        combinedOscillators->rightPhase = value;
        *out = combinedOscillators->leftPhase;
        out++;
        *out = combinedOscillators->rightPhase;
        out++;
    }
    return 0;
}
int addnote(int noteDistance){
    note currNote = getNote(noteDistance);
   // gettimeofday(&currNote.startTime, NULL);
    for (int i = 0; i < MAX_NOTES; i++){
        if (combined.notesPlaying[i].frequency == currNote.frequency){
            //note already held down, return
            printf("removing frequency");
            //maybe?
            combined.notesPlaying[i].currPos = 0;
            combined.notesPlaying[i].frequency = 0;
            combined.notesPlaying[i].noteValue = 0;
            combined.notesPlaying[i].samplePerWave = 0;

            return  -1;
        }
    }
    for (int i = 0; i < MAX_NOTES; i++){
        if (combined.notesPlaying[i].frequency == 0){
            printf("adding frequency:%f\n", currNote.frequency);
            //replace this frequency
            combined.notesPlaying[i] = currNote;
            return 0;
        }
    }
    //all notesPlaying taken
    return -1;
}
int removenote(int noteDistance){
    for (int i = 0; i < MAX_NOTES; i++){
        if (combined.notesPlaying[i].noteValue == noteDistance){
            combined.notesPlaying[i].currPos = 0;
            combined.notesPlaying[i].frequency = -1;
            combined.notesPlaying[i].noteValue = 0;
            combined.notesPlaying[i].samplePerWave = -1;
        }
    }
    return 0;
}
void printFrequencies(void){
    for (int i = 0; i < MAX_NOTES; i++){
        printf("frequency%d: %f\n", i, combined.notesPlaying[i].frequency);
    }
}
int handleError(PaError err){
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
void initCombinedData(void){
    printf("init combined data\n");
    for (int i = 0; i < OSCILLATOR_NUM; i++){
        combined.oscillators[i] = initOscillator();
    }
    combined.oscillators[0].oscillatorType = 1;
    updateUnison(&combined.oscillators[0], 7, 0.4);
    combined.oscillators[1].oscillatorType = 0;
    combined.oscillators[2].oscillatorType = 0;
    for (int i = 0 ; i < MAX_NOTES; i++){
        combined.notesPlaying[i].frequency = 0;
    }
    combined.testEnvolope = initializeTestEnvelope();
    printf("finish init\n");
}
int initialize(void){
    //initializeTables();
    initCombinedData();
    err = Pa_Initialize();
    if (err != paNoError){
        return handleError(err);
    }

    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                2,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                SAMPLE_RATE,
                                256,        /* frames per buffer */
                                callbackTest,
                                &combined );
    
    err = Pa_StartStream(stream);
    if(err != paNoError){
        return handleError(err);
    }
    if (err != paNoError){
        return handleError(err);
    }
    return 0;
}
int terminate(void){
    err = Pa_StopStream( stream );
    if( err != paNoError ){
        return handleError(err);
    }
    err = Pa_CloseStream( stream );
    if( err != paNoError ){
        return handleError(err);
    }
    Pa_Terminate();
    return err;
}