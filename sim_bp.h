#ifndef SIM_BP_H
#define SIM_BP_H
#include <stdlib.h>
#include <vector>
#include <math.h>

// Starter code -------------------------------------------------------------------------------------------------------------------------
typedef struct bp_params{
    unsigned long int K = 0;
    unsigned long int M1 = 0;
    unsigned long int M2 = 0;
    unsigned long int N = 0;
    char*             bp_name;
}bp_params;
// Starter code -------------------------------------------------------------------------------------------------------------------------


int predictions = 0;
int mispredictions = 0;

class entry{
    private:
        int counter = 0b10;

    public:
        int getCounter(){
            return counter;
        }

        // If counter is greater than 0b10 - branch taken, else - not taken
        bool taken(){
            if(counter >= 0b10)
                return true;
            else
                return false;
        }

        // Limit counter between 0b00 and 0b11
        void increment(){
            counter++;
            if(counter > 0b11)
                counter = 0b11;
        }
        void decrement(){
            counter--;
            if(counter < 0b00)
                counter = 0b00;
        }
};

class predictor{
    public:
        unsigned long int M = 0;
        unsigned long int N = 0;
        std::vector<entry> *table;
        unsigned long int indexMask = 0;

        predictor(unsigned long int Mval, unsigned long int Nval){
            M = Mval;
            N = Nval;

            if(Mval != 0){
                table = new std::vector<entry>(pow(2, M));
            }

            for(unsigned long int i=0; i<M; i++){
                indexMask = indexMask << 1;
                indexMask++;
            }
        }

        unsigned long int getIndex(unsigned long addr){
            addr = addr >> 2;
            return (addr & indexMask);
        }

        void printMeasurements(){
            printf("OUTPUT\n");
            printf(" number of predictions:\t%d\n", predictions);
            printf(" number of mispredictions:\t%d\n", mispredictions);
            printf(" misprediction rate:\t%0.2f%%\n", ((double)mispredictions/(double)predictions)*100);
        }

        void predict(unsigned long int addr, char outcome){
            printf("ERROR");
        }

        void printContents(){
            printf("ERROR");
        }
};

class bimodal: public predictor{
    public:
        bimodal(unsigned long int Mval) : predictor(Mval, 0){}

        void predict(unsigned long int addr, char outcome){
            int index = (int)getIndex(addr);
            
            // Make a prediction
            if(table->at(index).taken() && (outcome == 'n'))
                mispredictions++;
            if(!table->at(index).taken() && (outcome == 't'))
                mispredictions++;
            predictions++;

            // Update branch predictor based on actual outcome
            if(outcome == 't')
                table->at(index).increment();
            else if(outcome == 'n'){
                table->at(index).decrement();
            }
        }

        void printContents(){
            printf("FINAL BIMODAL CONTENTS\n");
            for(unsigned long int i=0; i<table->size(); i++){
                printf("%ld\t%d\n", i, table->at(i).getCounter());
            }

        }
};

class gshare : public predictor{
    public:
        unsigned long int BHR = 0;
        unsigned long int nBitMask = 0;
        unsigned long int mnBitMask = 0;

        gshare(unsigned long int Mval, unsigned long int Nval) : predictor(Mval, Nval){
            for(unsigned long int i=0; i<Nval; i++){
                nBitMask = nBitMask << 1;
                nBitMask++;
            }
            for(unsigned long int i=0; i<Mval-Nval; i++){
                mnBitMask = mnBitMask << 1;
                mnBitMask++;
            }
        }

        unsigned long int getIndex(unsigned long addr){
            unsigned long int mnBits = 0;
            
            // Get n bits of BHR, remove 2 lsb from PC addr
            BHR = BHR & nBitMask;
            addr = addr >> 2;
            
            // Hold onto m-n bits
            mnBits = addr & mnBitMask;

            // XOR top n bits with BHR
            addr = addr >> (M-N);
            addr = addr & nBitMask;
            addr = BHR^addr;

            // Recombine XORed bits with m-n bits
            addr = addr << (M-N);
            return (addr + mnBits);
        }

        void predict(unsigned long int addr, char outcome){
            int index = (int)getIndex(addr);
            
            // Make a prediction
            if(table->at(index).taken() && (outcome == 'n'))
                mispredictions++;
            if(!table->at(index).taken() && (outcome == 't'))
                mispredictions++;
            predictions++;

            // Update branch predictor based on actual outcome
            if(outcome == 't')
                table->at(index).increment();
            else if(outcome == 'n'){
                table->at(index).decrement();
            }

            //Update global table, shift right, then if taken - 1 in msb place, not taken - 0 in msb place
            BHR = BHR >> 1;
            if(outcome == 't'){
                BHR += pow(2, N-1);
            }
        }

        void printContents(){
            printf("FINAL GSHARE CONTENTS\n");
            for(unsigned long int i=0; i<table->size(); i++){
                printf("%ld\t%d\n", i, table->at(i).getCounter());
            }
        }
};

#endif
