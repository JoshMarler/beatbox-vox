/*
  ==============================================================================

    NoiseSynth.h
    Created: 4 Nov 2016 2:57:18pm
    Author:  Joshua.Marler

  ==============================================================================
*/

#ifndef NOISESYNTH_H_INCLUDED
#define NOISESYNTH_H_INCLUDED

#include "JuceHeader.h"

class NoiseSound : public SynthesiserSound
{
public:
	explicit NoiseSound(int initNoteNumberAllowed = 0) : SynthesiserSound()
	{
		if (initNoteNumberAllowed != 0)
			noteNumberAllowed = initNoteNumberAllowed;
	}

    bool appliesToNote (int midiNoteNumber) override  
	{  
		if (noteNumberAllowed == 0)
			return true;

		if (midiNoteNumber != noteNumberAllowed)
			return false;

		return true;
	}
    bool appliesToChannel (int /*midiChannel*/) override    { return true; }

private: 
	int noteNumberAllowed;
};


//==============================================================================
/** A simple demo synth voice that just plays a sine wave.. */
class NoiseVoice  : public SynthesiserVoice
{
public:
    NoiseVoice()
       : level (0), tailOff (0)
    {
    }

    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<NoiseSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound* /*sound*/,
                    int /*currentPitchWheelPosition*/) override
    {
        level = velocity;
        tailOff = 0.0;
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.

            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
			level = 0.0;
            clearCurrentNote();
        }
    }

    void pitchWheelMoved (int /*newValue*/) override
    {
        // not implemented for the purposes of this demo!
    }

    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
        // not implemented for the purposes of this demo!
    }

    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
            if (tailOff > 0)
            {
                while (--numSamples >= 0)
                {
					const float currentSample = (random.nextFloat() * 0.25f - 0.125f) * level;

                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    ++startSample;

                    tailOff *= 0.99;

                    if (tailOff <= 0.005)
                    {
						level = 0.0;

                        // tells the synth that this voice has stopped
                        clearCurrentNote();
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
					const float currentSample = (random.nextFloat() * 0.25f - 0.125f) * level;

                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    ++startSample;
                }
            }
    }

private:
	double level, tailOff;
	Random random;
};




#endif  // NOISESYNTH_H_INCLUDED
