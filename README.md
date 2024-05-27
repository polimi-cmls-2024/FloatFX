Authors: Alberto Colucci, Davide Rizzotti, Guido Elli, Madhav Gopi 

Team name: fast furries transform

## FloatFX
Singers, coding musicians, instrumentalists, sound technicians over the world face
major challenges when having to control the tone of their sound/instruments in
realtime. FloatFX adds an exciting flavour to live performances by enabling musicians/technicians to control various fx parameters of filters, distortion and delay with
just simple motions of the hand. This would save them from the trouble of looking
for knobs on their pedals in the dark, having to communicate between the stage
and the console area, plus, it also makes for a cool trick for the audience. It has
been designed to avoid accidental parameter changes while playing the instrument
and works only with finger pinches. 

The processing of the effects and GUI have been implemented on ’JUCE’ and ’Processing’ respectively, using ’Arduino’ and an accelerometer attached to the hand to capture the
movements. To demonstrate the viability of this product in a live music scene, we
have made a live SuperCollider music performance, without the usage of any external audio files. 

![](https://github.com/polimi-cmls-2024/FloatFX/blob/main/floatfx.JPG)

### List of modules to compile and build the plugin:

![](https://github.com/polimi-cmls-2024/FloatFX/blob/main/modules.JPG)

juce_serialport can be found at https://github.com/cpr2323/juce_serialport
